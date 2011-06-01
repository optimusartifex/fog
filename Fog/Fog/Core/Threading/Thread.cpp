// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Debug.h>
#include <Fog/Core/Global/Init_Core_p.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Tools/ByteArray.h>
#include <Fog/Core/Cpu/Cpu.h>
#include <Fog/Core/OS/OS.h>
#include <Fog/Core/System/Application.h>
#include <Fog/Core/System/Event.h>
#include <Fog/Core/System/Object.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Threading/ThreadLocalStorage.h>
#include <Fog/Core/Tools/Lazy.h>
#include <Fog/Core/Tools/TextCodec.h>

#if defined(FOG_OS_WINDOWS)
#include <windows.h>
#include <process.h>
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sched.h>

#if defined(FOG_OS_MAC)
#include <mach/mach.h>
#elif defined(FOG_OS_LINUX)
#include <sys/syscall.h>
#include <unistd.h>
#endif
#endif

#if defined(FOG_OS_WINDOWS)
# if !defined(STACK_SIZE_PARAM_IS_A_RESERVATION)
#  define STACK_SIZE_PARAM_IS_A_RESERVATION 0x00010000
# endif // STACK_SIZE_PARAM_IS_A_RESERVATION
#endif // FOG_OS_WINDOWS

namespace Fog {

// A lazily created thread local storage for quick access to a thread's message
// loop, if one exists. This should be safe and free of static constructors.
static Static< ThreadLocalPointer<Thread> > thread_tls;

// ============================================================================
// [Fog::Thread]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

// The information on how to set the thread name comes from
// a MSDN article: http://msdn2.microsoft.com/en-us/library/xcb2z8hs.aspx
static const DWORD MS_VC_EXCEPTION = 0x406D1388;

typedef struct tagTHREADNAME_INFO {
  DWORD dwType;     // Must be 0x1000.
  LPCSTR szName;    // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;

static unsigned __stdcall threadFunc(void* closure)
{
  Thread* thread = static_cast<Thread*>(closure);
  thread_tls.instance().set(thread);
  thread->main();
  thread->finished();
  return 0;
}

uint32_t Thread::_tid()
{
  return GetCurrentThreadId();
}

void Thread::_yield()
{
  ::Sleep(0);
}

void Thread::_sleep(uint32_t ms)
{
  ::Sleep(ms);
}

static void __setThreadName(THREADNAME_INFO* info)
{
#if defined(FOG_CC_MSC)
  __try
  {
    RaiseException(MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(DWORD), reinterpret_cast<DWORD_PTR*>(info));
  }
  __except(EXCEPTION_CONTINUE_EXECUTION) {}
#else
#warning "SEH handling not enabled for this compiler"
#endif
}

void Thread::_setName(const String& name)
{
  // The debugger needs to be around to catch the name in the exception. If
  // there isn't a debugger, we are just needlessly throwing an exception.
  if (!::IsDebuggerPresent()) return;

  ByteArrayTmp<256> t;
  TextCodec::local8().appendFromUnicode(t, name);

  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = t.getData();
  info.dwThreadID = _tid();
  info.dwFlags = 0;

  __setThreadName(&info);
}

bool Thread::_create(sysuint_t stackSize, Thread* thread)
{
  uint flags = 0;
  if (stackSize > 0 && OS::getWindowsVersion() >= OS::WIN_VERSION_XP)
    flags = STACK_SIZE_PARAM_IS_A_RESERVATION;
  else
    stackSize = 0;

  thread->_handle = reinterpret_cast<Handle>(_beginthreadex(NULL, stackSize, threadFunc, thread, flags, NULL));
  return thread->_handle != NULL;
}

void Thread::_join(Thread* thread)
{
  FOG_ASSERT(thread->_handle);

  // Wait for the thread to exit. It should already have terminated but make
  // sure this assumption is valid.
  DWORD result = WaitForSingleObject(thread->_handle, INFINITE);
  FOG_ASSERT(result == WAIT_OBJECT_0);

  CloseHandle(thread->_handle);
  thread->_handle = NULL;
}

#elif defined(FOG_OS_POSIX)

static void* threadFunc(void* closure)
{
  Thread* thread = static_cast<Thread*>(closure);
  thread_tls.instance().set(thread);

  thread->main();
  thread->finished();
  return NULL;
}

uint32_t Thread::_tid()
{
  // Pthreads doesn't have the concept of a thread ID, so we have to reach down
  // into the kernel.
#if defined(FOG_OS_MAC)
  return (uint32_t)mach_thread_self();
#elif defined(FOG_OS_LINUX)
  return (uint32_t)syscall(__NR_gettid);
#endif
}

void Thread::_yield()
{
  sched_yield();
}

void Thread::_sleep(uint32_t ms)
{
  struct timespec sleep_time, remaining;

  // Contains the portion of ms >= 1 sec.
  sleep_time.tv_sec = ms / 1000;
  ms -= sleep_time.tv_sec * 1000;

  // Contains the portion of ms < 1 sec.
  sleep_time.tv_nsec = ms * 1000 * 1000; // nanoseconds.

  while (nanosleep(&sleep_time, &remaining) == -1 && errno == EINTR)
    sleep_time = remaining;
}

void Thread::_setName(const String& name)
{
  // The POSIX standard does not provide for naming threads, and neither Linux
  // nor Mac OS X (our two POSIX targets) provide any non-portable way of doing
  // it either. (Some BSDs provide pthread_set_name_np but that isn't much of a
  // consolation prize.)
}

bool Thread::_create(sysuint_t stackSize, Thread* thread)
{
  bool success = false;
  pthread_attr_t attributes;
  pthread_attr_init(&attributes);

  // Pthreads are joinable by default, so we don't need to specify any special
  // attributes to be able to call pthread_join later.

  if (stackSize > 0)
    pthread_attr_setstacksize(&attributes, stackSize);

  success = !pthread_create(&thread->_handle, &attributes, threadFunc, thread);

  pthread_attr_destroy(&attributes);
  return success;
}

void Thread::_join(Thread* thread)
{
  pthread_join(thread->_handle, NULL);
}

#endif

// ============================================================================
// [Fog::Thread]
// ============================================================================

Thread* Thread::getCurrent()
{
  return thread_tls.instance().get();
}

Thread::Thread(const String& name) :
  _handle(0),
  _id(0),
  _name(name),
  _stackSize(0),
  _startupData(NULL),
  _eventLoop(NULL)
{
}

Thread::~Thread()
{
  stop();
}

void Thread::setStackSize(sysuint_t ssize)
{
  _stackSize = ssize;
}

err_t Thread::setAffinity(int mask)
{
  if (mask <= 0) return resetAffinity();

#if defined(FOG_OS_WINDOWS)
  DWORD_PTR result = SetThreadAffinityMask(_handle, (DWORD_PTR)mask);
  if (result != 0)
    return ERR_OK;
  else
    return ERR_RT_INVALID_ARGUMENT;
#elif defined(FOG_OS_LINUX)
  size_t affinityMask = mask;
  return pthread_setaffinity_np(_handle, sizeof(affinityMask), (const cpu_set_t*)&affinityMask);
#else
  return ERR_RT_NOT_IMPLEMENTED;
#endif
}

err_t Thread::resetAffinity()
{
#if defined(FOG_OS_WINDOWS)
  size_t affinityMask = (1 << Cpu::get()->numberOfProcessors) - 1;
  DWORD_PTR result = SetThreadAffinityMask(_handle, (DWORD_PTR)(affinityMask));
  if (result != 0)
    return ERR_OK;
  else
    return ERR_RT_INVALID_ARGUMENT;
#elif defined(FOG_OS_LINUX)
  size_t affinityMask = (1 << Cpu::get()->numberOfProcessors) - 1;
  return pthread_setaffinity_np(_handle, sizeof(affinityMask), (const cpu_set_t*)&affinityMask);
#else
  return ERR_RT_NOT_IMPLEMENTED;
#endif
}

bool Thread::start(const String& eventLoopType)
{
  FOG_ASSERT(!_eventLoop);

  StartupData startupData(eventLoopType);
  _startupData = &startupData;

  if (!_create(_stackSize, this))
  {
    Debug::dbgFunc("Fog::Thread", "start", "failed to create a thread.\n");
    _startupData = NULL; // Record that we failed to start.
    return false;
  }

  // Wait for the thread to start and initialize _eventLoop
  startupData.event.wait();
  return true;
}

void Thread::stop()
{
  if (!_threadWasStarted()) return;

  // We should only be called on the same thread that started us.
  FOG_ASSERT(_id != _tid());

  // StopSoon may have already been called.
  if (_eventLoop) _eventLoop->postTask(fog_new QuitTask());

  // Wait for the thread to exit. It should already have terminated but make
  // sure this assumption is valid.
  Thread::_join(this);

  // The thread can't receive messages anymore.
  _eventLoop = NULL;

  // The thread no longer needs to be joined.
  _startupData = NULL;
}

void Thread::stopSoon()
{
  if (!_eventLoop) return;

  // We should only be called on the same thread that started us.
  FOG_ASSERT(_id != _tid());

  // We had better have a event loop at this point!  If we do not, then it
  // most likely means that the thread terminated unexpectedly, probably due
  // to someone calling Quit() on our event loop directly.
  FOG_ASSERT(_eventLoop);

  _eventLoop->postTask(fog_new QuitTask());

  // The thread can't receive messages anymore.
  _eventLoop = NULL;
}

void Thread::init()
{
}

void Thread::cleanUp()
{
}

void Thread::main()
{
  // Complete the initialization of our Thread object.
  _id = _tid();
  _setName(_name);

  if (!_startupData->eventLoopType.isEmpty())
  {
    // The event loop for this thread.
    _eventLoop = Application::createEventLoop(_startupData->eventLoopType);
  }

  if (_eventLoop)
  {
    // Event loop
    _startupData->event.signal();

    // _startupData can't be touched anymore since the starting thread is now
    // unlocked.

    // Let the thread do extra initialization.
    init();

    _eventLoop->run();

    // Let the thread do extra cleanup.
    cleanUp();

    // We can't receive messages anymore.
    fog_delete(_eventLoop);
    _eventLoop = NULL;
  }
  else
  {
    // Thread is waiting for it.
    _startupData->event.signal();
  }
}

void Thread::finished()
{
}

Thread* Thread::_mainThread;
uint32_t Thread::_mainThreadId;

// [Fog::MainThread]

struct MainThread : public Thread
{
  MainThread();
  virtual ~MainThread();

  virtual void main();
};

MainThread::MainThread()
{
  thread_tls.instance().set(this);

#if defined(FOG_OS_WINDOWS)
  _handle = GetCurrentThread();
  _id = GetCurrentThreadId();
#endif

#if defined(FOG_OS_POSIX)
  _handle = pthread_self();
  _id = _tid();
#endif // FOG_OS_POSIX

  _flags = 0;
  _name.set(Ascii8("Main"));

  _mainThread = this;
  _mainThreadId = _id;
}

MainThread::~MainThread()
{
  _mainThread = NULL;
}

void MainThread::main()
{
  FOG_ASSERT_NOT_REACHED();
}

static Static<MainThread> mainThread;

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_thread_init(void)
{
  thread_tls.init();
  mainThread.init();
}

FOG_NO_EXPORT void _core_thread_fini(void)
{
  mainThread.destroy();
  thread_tls.destroy();
}

} // Fog namespace
