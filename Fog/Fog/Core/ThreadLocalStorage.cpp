// [Fog-Core Library - Public API]
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
#include <Fog/Core/Assert.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/ThreadLocalStorage.h>

#if defined(FOG_OS_WINDOWS)
#include <windows.h>
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
#include <errno.h>
#include <sys/time.h>
#endif // FOG_OS_POSIX

namespace Fog {

// ============================================================================
// [Fog::ThreadLocalStorage]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
// In order to make TLS destructors work, we need to keep function
// pointers to the destructor for each TLS that we allocate.
// We make this work by allocating a single OS-level TLS, which
// contains an array of slots for the application to use.  In
// parallel, we also allocate an array of destructors, which we
// keep track of and call when threads terminate.

// _tlsKey is the one native TLS that we use.  It stores our
// table.
long ThreadLocalStorage::_tlsKey = TLS_OUT_OF_INDEXES;

// _tlsMax is the high-water-mark of allocated thread local storage.
// We intentionally skip 0 so that it is not confused with an
// unallocated TLS slot.
long ThreadLocalStorage::_tlsMax = 1;

// An array of destructor function pointers for the slots.  If
// a slot has a destructor, it will be stored in its corresponding
// entry in this array.
ThreadLocalStorage::TLSDestructorFunc ThreadLocalStorage::_tlsDestructors[ThreadLocalStorageSize];

void** ThreadLocalStorage::initialize()
{
  if (_tlsKey == TLS_OUT_OF_INDEXES)
  {
    long value = TlsAlloc();
    FOG_ASSERT(value != TLS_OUT_OF_INDEXES);

    // Atomically test-and-set the tlsKey.  If the key is TLS_OUT_OF_INDEXES,
    // go ahead and set it.  Otherwise, do nothing, as another
    // thread already did our dirty work.
    if (InterlockedCompareExchange(&_tlsKey, value, TLS_OUT_OF_INDEXES) != TLS_OUT_OF_INDEXES)
    {
      // We've been shortcut. Another thread replaced _tlsKey first so we need
      // to destroy our index and use the one the other thread got first.
      TlsFree(value);
    }
  }
  FOG_ASSERT(TlsGetValue(_tlsKey) == NULL);

  // Create an array to store our data.
  void** tlsData = new(std::nothrow) void*[ThreadLocalStorageSize];
  memset(tlsData, 0, sizeof(void*[ThreadLocalStorageSize]));
  TlsSetValue(_tlsKey, tlsData);
  return tlsData;
}

ThreadLocalStorage::Slot::Slot(TLSDestructorFunc destructor) : _initialized(false)
{
  initialize(destructor);
}

ThreadLocalStorage::Slot::Slot(Fog::_DONT_INITIALIZE)
{
}

bool ThreadLocalStorage::Slot::initialize(TLSDestructorFunc destructor)
{
  if (_tlsKey == TLS_OUT_OF_INDEXES || !TlsGetValue(_tlsKey))
  {
    ThreadLocalStorage::initialize();
  }

  // Grab a new slot.
  _slot = InterlockedIncrement(&_tlsMax) - 1;
  if (_slot >= ThreadLocalStorageSize)
  {
    FOG_ASSERT_NOT_REACHED();
    return false;
  }

  // Setup our destructor.
  _tlsDestructors[_slot] = destructor;
  _initialized = true;
  return true;
}

void ThreadLocalStorage::Slot::free()
{
  // At this time, we don't reclaim old indices for TLS slots.
  // So all we need to do is wipe the destructor.
  _tlsDestructors[_slot] = NULL;
  _initialized = false;
}

void* ThreadLocalStorage::Slot::get() const
{
  void** tls_data = static_cast<void**>(TlsGetValue(_tlsKey));
  if (!tls_data)
    tls_data = ThreadLocalStorage::initialize();
  FOG_ASSERT(_slot >= 0 && _slot < ThreadLocalStorageSize);
  return tls_data[_slot];
}

void ThreadLocalStorage::Slot::set(void* value)
{
  void** tls_data = static_cast<void**>(TlsGetValue(_tlsKey));
  if (!tls_data)
    tls_data = ThreadLocalStorage::initialize();
  FOG_ASSERT(_slot >= 0 && _slot < ThreadLocalStorageSize);
  tls_data[_slot] = value;
}

void ThreadLocalStorage::threadExit()
{
  void** tls_data = static_cast<void**>(TlsGetValue(_tlsKey));

  // Maybe we have never initialized TLS for this thread.
  if (!tls_data)
    return;

  for (int slot = 0; slot < _tlsMax; slot++) {
    if (_tlsDestructors[slot] != NULL) {
      void* value = tls_data[slot];
      _tlsDestructors[slot](value);
    }
  }

  delete[] tls_data;

  // In case there are other "onexit" handlers...
  TlsSetValue(_tlsKey, NULL);
}

// Thread Termination Callbacks.
// Windows doesn't support a per-thread destructor with its
// TLS primitives.  So, we build it manually by inserting a
// function to be called on each thread's exit.
// This magic is from http://www.codeproject.com/threads/tls.asp
// and it works for VC++ 7.0 and later.

#ifdef _WIN64

// This makes the linker create the TLS directory if it's not already
// there.  (e.g. if __declspec(thread) is not used).
#pragma comment(linker, "/INCLUDE:_tls_used")

#else  // _WIN64

// This makes the linker create the TLS directory if it's not already
// there.  (e.g. if __declspec(thread) is not used).
#pragma comment(linker, "/INCLUDE:__tls_used")

#endif  // _WIN64

// Static callback function to call with each thread termination.
void NTAPI OnThreadExit(PVOID module, DWORD reason, PVOID reserved)
{
  // On XP SP0 & SP1, the DLL_PROCESS_ATTACH is never seen. It is sent on SP2+
  // and on W2K and W2K3. So don't assume it is sent.
  if (DLL_THREAD_DETACH == reason || DLL_PROCESS_DETACH == reason)
    ThreadLocalStorage::threadExit();
}

// .CRT$XLA to .CRT$XLZ is an array of PIMAGE_TLS_CALLBACK pointers that are
// called automatically by the OS loader code (not the CRT) when the module is
// loaded and on thread creation. They are NOT called if the module has been
// loaded by a LoadLibrary() call. It must have implicitly been loaded at
// process startup.
// By implicitly loaded, I mean that it is directly referenced by the main EXE
// or by one of its dependent DLLs. Delay-loaded DLL doesn't count as being
// implicitly loaded.
//
// See VC\crt\src\tlssup.c for reference.
#ifdef _WIN64

// .CRT section is merged with .rdata on x64 so it must be constant data.
#pragma const_seg(".CRT$XLB")
// When defining a const variable, it must have external linkage to be sure the
// linker doesn't discard it. If this value is discarded, the OnThreadExit
// function will never be called.
extern const PIMAGE_TLS_CALLBACK p_thread_callback;
const PIMAGE_TLS_CALLBACK p_thread_callback = OnThreadExit;

// Reset the default section.
#pragma const_seg()

#else  // _WIN64

#pragma data_seg(".CRT$XLB")
PIMAGE_TLS_CALLBACK p_thread_callback = OnThreadExit;

// Reset the default section.
#pragma data_seg()

#endif  // _WIN64
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
// [Fog::ThreadLocalStorage::Slot]

ThreadLocalStorage::Slot::Slot(TLSDestructorFunc destructor) : 
  _initialized(false)
{
  initialize(destructor);
}

ThreadLocalStorage::Slot::Slot(Fog::_DONT_INITIALIZE)
{
}

bool ThreadLocalStorage::Slot::initialize(TLSDestructorFunc destructor)
{
  FOG_ASSERT(!_initialized);
  int error = pthread_key_create(&_slot, destructor);
  if (error)
  {
    FOG_ASSERT_NOT_REACHED();
    return false;
  }

  _initialized = true;
  return true;
}

void ThreadLocalStorage::Slot::free()
{
  FOG_ASSERT(_initialized);
  int error = pthread_key_delete(_slot);
  if (error) FOG_ASSERT_NOT_REACHED();
  _initialized = false;
}

void* ThreadLocalStorage::Slot::get() const
{
  FOG_ASSERT(_initialized);
  return pthread_getspecific(_slot);
}

void ThreadLocalStorage::Slot::set(void* value)
{
  FOG_ASSERT(_initialized);
  int error = pthread_setspecific(_slot, value);
  if (error) FOG_ASSERT_NOT_REACHED();
}
#endif // FOG_OS_POSIX

} // Fog namespace
