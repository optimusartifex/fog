// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Guard]
#ifndef _FOG_CORE_THREADING_THREADLOCALSTORAGE_H
#define _FOG_CORE_THREADING_THREADLOCALSTORAGE_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Threading/Atomic.h>

// [Dependencies - Windows]
#if defined(FOG_OS_WINDOWS)
#include <windows.h>
#endif // FOG_OS_WINDOWS

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
#include <pthread.h>
#endif // FOG_OS_POSIX

namespace Fog {

//! @addtogroup Fog_Core_Threading
//! @{

// ============================================================================
// [Fog::ThreadLocalStorage]
// ============================================================================

// Wrapper for thread local storage.  This class doesn't do much except provide
// an API for portability.
struct FOG_API ThreadLocalStorage
{
public:
  // Helper functions that abstract the cross-platform APIs.
#if defined(FOG_OS_WINDOWS)
  typedef int SlotType;

  static FOG_INLINE void allocSlot(SlotType& slot)
  {
    slot = TlsAlloc();
    FOG_ASSERT(slot != TLS_OUT_OF_INDEXES);
  }

  static FOG_INLINE void freeSlot(SlotType& slot)
  {
    if (!TlsFree(slot)) FOG_ASSERT(false);
  }

  static FOG_INLINE void* getSlotValue(SlotType& slot)
  {
    return TlsGetValue(slot);
  }

  static FOG_INLINE void setSlotValue(SlotType& slot, void* value)
  {
    if (!TlsSetValue(slot, value)) FOG_ASSERT(false);
  }
#endif // CORE_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  typedef pthread_key_t SlotType;

  static FOG_INLINE void allocSlot(SlotType& slot)
  {
    int error = pthread_key_create(&slot, NULL);
    FOG_ASSERT(error == 0);
  }

  static FOG_INLINE void freeSlot(SlotType& slot)
  {
    int error = pthread_key_delete(slot);
    FOG_ASSERT(error == 0);
  }

  static FOG_INLINE void* getSlotValue(SlotType& slot)
  {
    return pthread_getspecific(slot);
  }

  static FOG_INLINE void setSlotValue(SlotType& slot, void* value)
  {
    int error = pthread_setspecific(slot, value);
    FOG_ASSERT(error == 0);
  }
#endif // FOG_OS_POSIX

  // Prototype for the TLS destructor function, which can be optionally used to
  // cleanup thread local storage on thread exit.  'value' is the data that is
  // stored in thread local storage.
  typedef void (*TLSDestructorFunc)(void* value);

  // A key representing one value stored in TLS.
  struct FOG_API Slot
  {
  public:
    Slot(TLSDestructorFunc destructor = NULL);

    // This constructor should be used for statics.
    // It returns an uninitialized Slot.
    Slot(_Uninitialized x);

    // Set up the TLS slot.  Called by the constructor.
    // 'destructor' is a pointer to a function to perform per-thread cleanup of
    // this object.  If set to NULL, no cleanup is done for this TLS slot.
    // Returns false on error.
    bool initialize(TLSDestructorFunc destructor);

    // Free a previously allocated TLS 'slot'.
    // If a destructor was set for this slot, removes
    // the destructor so that remaining threads exiting
    // will not free data.
    void free();

    // Get the thread-local value stored in slot 'slot'.
    // Values are guaranteed to initially be zero.
    void* get() const;

    // Set the thread-local value stored in slot 'slot' to
    // value 'value'.
    void set(void* value);

    bool initialized() const { return _initialized; }

  private:
    // The internals of this struct should be considered private.
    bool _initialized;
    SlotType _slot;

    _FOG_CLASS_NO_COPY(Slot)
  };

#if defined(FOG_OS_WINDOWS)
  // Function called when on thread exit to call TLS
  // destructor functions.  This function is used internally.
  static void threadExit();

private:
  // Function to lazily initialize our thread local storage.
  static void **initialize();

private:
  // The maximum number of 'slots' in our thread local storage stack.
  // For now, this is fixed.  We could either increase statically, or
  // we could make it dynamic in the future.
  static const int ThreadLocalStorageSize = 64;

  static long _tlsKey;
  static long _tlsMax;

  static TLSDestructorFunc _tlsDestructors[ThreadLocalStorageSize];
#endif  // FOG_OS_WINDOWS

private:
  _FOG_CLASS_NO_COPY(ThreadLocalStorage)
};

template <typename T>
struct FOG_NO_EXPORT ThreadLocalPointer
{
public:
  FOG_INLINE ThreadLocalPointer() : _slot()
  {
    ThreadLocalStorage::allocSlot(_slot);
  }

  FOG_INLINE ~ThreadLocalPointer()
  {
    ThreadLocalStorage::freeSlot(_slot);
  }

  FOG_INLINE T* get()
  {
    return static_cast<T*>(ThreadLocalStorage::getSlotValue(_slot));
  }

  FOG_INLINE void set(T* ptr)
  {
    ThreadLocalStorage::setSlotValue(_slot, ptr);
  }

private:
  typedef ThreadLocalStorage::SlotType SlotType;

  SlotType _slot;

  _FOG_CLASS_NO_COPY(ThreadLocalPointer<T>)
};

struct FOG_NO_EXPORT ThreadLocalBoolean
{
  FOG_INLINE ThreadLocalBoolean() {}
  FOG_INLINE ~ThreadLocalBoolean() {}

  FOG_INLINE bool get()
  {
    return _tlp.get() != NULL;
  }

  FOG_INLINE void set(bool val)
  {
    _tlp.set(reinterpret_cast<void*>(val ? 1 : 0));
  }

private:
  ThreadLocalPointer<void> _tlp;

  _FOG_CLASS_NO_COPY(ThreadLocalBoolean)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADING_THREADLOCALSTORAGE_H
