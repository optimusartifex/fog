// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/ThreadLocal.h>

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <errno.h>
# include <sys/time.h>
#endif // FOG_OS_POSIX

namespace Fog {

// ============================================================================
// [Fog::ThreadLocal - Constants]
// ============================================================================

enum { THREAD_LOCAL_SIZE = 96 };

// ============================================================================
// [Fog::ThreadLocal - Helpers]
// ============================================================================

static void ThreadLocal_dummy(void*) {}

// To get understanding how TLS is implemented under windows please try to
// search for 'Thread Local Storage, Windows'. The technique used by Fog is to
// allocate one global TLS index at Fog startup and free it on shutdown (fini).
//
// This allows us to register one function, called OnThreadExit() which is
// responsible for all 'our' thread-local storages allocated by @c ThreadLocal.
//
// Please see these excellent blog-posts about TLS on Windows:
//   http://www.nynaeve.net/?tag=tls
//
// NOTE: The technique to allocate one TLS variable and to use it for all
// thread-local indices allocated by @c ThreadLocal instance is the same for
// all supported platforms. The only difference is the used technique.

// Local TLS index to our thread-local table.
static Atomic<uint32_t> ThreadLocal_index;

// Destructors of our thread-local table.
static ThreadLocalDestructorFunc ThreadLocal_dtorList[THREAD_LOCAL_SIZE];

// Forward Declarations.
static void ThreadLocal_onThreadExit();

// ============================================================================
// [Fog::ThreadLocal - Windows Defs]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

// Global TLS index obtained by TlsAlloc().
static DWORD ThreadLocal_global;

// Defs.
#define _FOG_TLS_GET() ::TlsGetValue(ThreadLocal_global)
#define _FOG_TLS_SET(_Value_) ::TlsSetValue(ThreadLocal_global, _Value_)

#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::ThreadLocal - Posix Defs]
// ============================================================================

#if defined(FOG_OS_POSIX)

// Global TLS key.
static pthread_key_t ThreadLocal_global;

// Defs.
#define _FOG_TLS_GET() ::pthread_getspecific(ThreadLocal_global)
#define _FOG_TLS_SET(_Value_) ::pthread_setspecific(ThreadLocal_global, _Value_)

#endif // FOG_OS_POSIX

// ============================================================================
// [Fog::ThreadLocal - Generic]
// ============================================================================

static err_t ThreadLocal_create(uint32_t* slot, void* _dtor)
{
  FOG_ASSERT(slot != NULL);
  FOG_ASSERT(ThreadLocal_global != TLS_OUT_OF_INDEXES);

  ThreadLocalDestructorFunc dtor = (ThreadLocalDestructorFunc)_dtor;
  if (dtor == NULL) dtor = ThreadLocal_dummy;

  uint32_t i = ThreadLocal_index.addXchg(1);
  if (FOG_UNLIKELY(i >= THREAD_LOCAL_SIZE))
  {
    // If all thread-local indices are used, we try to find free index looking
    // for NULL destructor in ThreadLocal_dtorList. But we also decrement the
    // index to cleanup our addXchg() operation.
    ThreadLocal_index.dec();

    for (i = 1; i < THREAD_LOCAL_SIZE; i++)
    {
      if (AtomicCore<ThreadLocalDestructorFunc>::cmpXchg(&ThreadLocal_dtorList[i], NULL, dtor))
      {
        // Index found.
        *slot = i;
        return ERR_OK;
      }
    }

    return ERR_THREAD_TLS_EXHAUSTED;
  }

  AtomicCore<ThreadLocalDestructorFunc>::set(&ThreadLocal_dtorList[i], dtor);
  *slot = i;

  return ERR_OK;
}

static err_t ThreadLocal_destroy(uint32_t slot)
{
  if (FOG_UNLIKELY(slot - 1 >= THREAD_LOCAL_SIZE - 1))
    return (slot == 0) ? (err_t)ERR_OK : ERR_THREAD_TLS_INVALID;

  // The TLS index is not reused until all thread-local indices are allocated.
  // So this is the only needed step to free the TLS index.
  AtomicCore<ThreadLocalDestructorFunc>::setXchg(&ThreadLocal_dtorList[slot], NULL);

  return ERR_OK;
}

static void* ThreadLocal_get(uint32_t slot)
{
  FOG_ASSERT(ThreadLocal_global != TLS_OUT_OF_INDEXES);

  if (FOG_UNLIKELY(slot - 1 >= THREAD_LOCAL_SIZE - 1))
    return NULL;

  void* g = _FOG_TLS_GET();

  if (FOG_IS_NULL(g))
    // TLS hasn't been allocated by this thread yet. We don't need to allocate
    // it, because the value is not changed by @c ThradLocal::get() call.
    return NULL;
  else
    return reinterpret_cast<void**>(g)[slot];
}

static err_t ThreadLocal_set(uint32_t slot, void* value)
{
  FOG_ASSERT(ThreadLocal_global != TLS_OUT_OF_INDEXES);

  if (FOG_UNLIKELY(slot - 1 >= THREAD_LOCAL_SIZE - 1))
    return ERR_THREAD_TLS_INVALID;

  void* g = _FOG_TLS_GET();
  if (FOG_IS_NULL(g))
  {
    // TLS hasn't been allocated by this thread yet. It's needed to allocate
    // the memory to store our TLS values.
    g = MemMgr::calloc(THREAD_LOCAL_SIZE * sizeof(void*));
    if (FOG_IS_NULL(g))
      return ERR_RT_OUT_OF_MEMORY;

    _FOG_TLS_SET(g);
  }

  reinterpret_cast<void**>(g)[slot] = value;
  return ERR_OK;
}

static void ThreadLocal_onThreadExit()
{
  void* g = _FOG_TLS_GET();
  if (FOG_IS_NULL(g)) return;

  uint32_t i, count = ThreadLocal_index.get();
  ThreadLocalDestructorFunc dummy = ThreadLocal_dummy;

  // Call all registered destructors.
  for (i = 1; i < count; i++)
  {
    ThreadLocalDestructorFunc dtor = AtomicCore<ThreadLocalDestructorFunc>::get(&ThreadLocal_dtorList[i]);
    if (dtor == NULL || dtor == dummy) continue;
    dtor(reinterpret_cast<void**>(g)[i]);
  }

  // Clean-up.
  MemMgr::free(g);
  _FOG_TLS_SET(NULL);
}

#if defined(FOG_OS_WINDOWS)

void WINAPI OnThreadExit(PVOID module, DWORD dwReason, PVOID reserved)
{
  switch (dwReason)
  {
    // Called by WinXP-SP2 and later.
    case DLL_THREAD_ATTACH:
    case DLL_PROCESS_ATTACH:
      break;

    // Called always.
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      ThreadLocal_onThreadExit();
      break;
  }
}

// TODO: MSC only, is there a way how to do that with MinGW?
#ifdef _WIN64
# pragma comment(linker, "/INCLUDE:_tls_used")
# pragma comment(linker, "/INCLUDE:_tls_fog_cb")
  // .CRT section is merged with .rdata on x64 so it must be const.
# pragma const_seg(".CRT$XLB")
  extern "C" const PIMAGE_TLS_CALLBACK _tls_fog_cb = OnThreadExit;
# pragma const_seg()
#else
# pragma comment(linker, "/INCLUDE:__tls_used")
# pragma comment(linker, "/INCLUDE:__tls_fog_cb")
# pragma data_seg(".CRT$XLB")
  extern "C" PIMAGE_TLS_CALLBACK _tls_fog_cb = OnThreadExit;
# pragma data_seg()
#endif

FOG_NO_EXPORT void ThreadLocal_init(void)
{
  ThreadLocal_index.init(1);
  ThreadLocal_global = ::TlsAlloc();

  if (ThreadLocal_global == TLS_OUT_OF_INDEXES)
  {
    Debug::failFunc("Fog::ThreadLocal", "$init", "Failed to create system TLS using TlsAlloc().\n");
  }

  // Initialize destructor list to dummy.
  ThreadLocalDestructorFunc dummy = ThreadLocal_dummy;
  for (uint i = 1; i < THREAD_LOCAL_SIZE; i++)
    ThreadLocal_dtorList[i] = dummy;

  fog_api.threadlocal_create = ThreadLocal_create;
  fog_api.threadlocal_destroy = ThreadLocal_destroy;
  fog_api.threadlocal_get = ThreadLocal_get;
  fog_api.threadlocal_set = ThreadLocal_set;
}

FOG_NO_EXPORT void ThreadLocal_fini(void)
{
  ::TlsFree(ThreadLocal_global);
  ThreadLocal_global = TLS_OUT_OF_INDEXES;
}

#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::ThreadLocal - Posix]
// ============================================================================

#if defined(FOG_OS_POSIX)

static void ThreadLocal_pThreadExit(void* p)
{
  ThreadLocal_onThreadExit();
}

FOG_NO_EXPORT void ThreadLocal_init(void)
{
  ThreadLocal_index.init(1);

  int result = ::pthread_key_create(&ThreadLocal_global, ThreadLocal_pThreadExit);
  if (FOG_UNLIKELY(result != 0))
  {
    Debug::failFunc("Fog::ThreadLocal", "$init", "Failed to create system TLS using pthread_key_create().\n");
  }

  // Initialize destructor list to dummy.
  ThreadLocalDestructorFunc dummy = ThreadLocal_dummy;
  for (uint i = 1; i < THREAD_LOCAL_SIZE; i++)
    ThreadLocal_dtorList[i] = dummy;

  fog_api.threadlocal_create = ThreadLocal_create;
  fog_api.threadlocal_destroy = ThreadLocal_destroy;
  fog_api.threadlocal_get = ThreadLocal_get;
  fog_api.threadlocal_set = ThreadLocal_set;
}

FOG_NO_EXPORT void ThreadLocal_fini(void)
{
  ::pthread_key_delete(ThreadLocal_global);
}

#endif // FOG_OS_POSIX

} // Fog namespace
