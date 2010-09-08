// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MACUTIL_H
#define _FOG_CORE_MACUTIL_H

#include <Fog/Core/Build.h>

#if defined(FOG_OS_MAC)

// [Dependencies]
#include <Fog/Core/String.h>
#include <CoreFoundation/CoreFoundation.h>

namespace Fog {

// =================================================================================
// [CFType]
// =================================================================================

template <typename T>
struct RemovePointer
{
  typedef T type;
};

template <typename T>
struct RemovePointer<T*>
{
  typedef T type;
};

template <typename T>
struct CFType
{
  typedef typename RemovePointer<T>::type ValueType;
  typedef ValueType* PtrType;

  CFType() : _ptr(0) {}
  CFType(PtrType ptr) : _ptr(ptr) { if (ptr) CFRetain(ptr); }
  CFType(const CFType& o) : _ptr(o._ptr) { if (PtrType ptr = _ptr) CFRetain(ptr); }
  ~CFType() { if (PtrType ptr = _ptr) CFRelease(ptr); }
  template <typename U> CFType(const CFType<U>& o) : _ptr(o.get()) { if (PtrType ptr = _ptr) CFRetain(ptr); }

  PtrType get() const { return _ptr; }
  PtrType releaseRef() { PtrType tmp = _ptr; _ptr = 0; return tmp; }

  PtrType operator->() const { return _ptr; }
  bool operator!() const { return !_ptr; }
  
  //! @brief  This conversion operator allows implicit conversion to bool but not to other integer types.
  typedef PtrType CFType::*UnspecifiedBoolType;
  operator UnspecifiedBoolType() const { return _ptr ? &CFType::_ptr : 0; }

  CFType& operator=(const CFType&);

  template <typename U>
  CFType& operator=(const CFType<U>&);

  CFType& operator=(PtrType optr)
  {
    if (optr)
      CFRetain(optr);
    PtrType ptr = _ptr;
    _ptr = optr;
    if (ptr)
      CFRelease(ptr);
    return *this;
  
  }
  template <typename U>
  CFType& operator=(U*);

private:
  PtrType _ptr;
};

template <typename T>
FOG_INLINE CFType<T>& CFType<T>::operator=(const CFType<T>& o)
{
  PtrType optr = o.get();
  if (optr)
    CFRetain(optr);
  PtrType ptr = _ptr;
  _ptr = optr;
  if (ptr)
    CFRelease(ptr);
  return *this;
}

template <class T>
FOG_INLINE void swap(CFType<T>& a, CFType<T>& b)
{
  a.swap(b);
}


// TODO For any unknown reason, these compare operators hide the string compare 
// operators
#if 0
template <typename T, typename U> inline bool operator==(const CFType<T>& a, const CFType<U>& b)
{ 
  return a.get() == b.get(); 
}

template <typename T, typename U> inline bool operator==(const CFType<T>& a, U* b)
{ 
  return a.get() == b; 
}

template <typename T, typename U> inline bool operator==(T* a, const CFType<U>& b) 
{
  return a == b.get(); 
}

template <typename T, typename U> inline bool operator!=(const CFType<T>& a, const CFType<U>& b)
{ 
  return a.get() != b.get(); 
}

template <typename T, typename U> inline bool operator!=(const CFType<T>& a, U* b)
{
  return a.get() != b; 
}

template <typename T, typename U> inline bool operator!=(T* a, const CFType<U>& b)
{ 
  return a != b.get(); 
}
#endif

// =================================================================================
// [Carbon and Cocoa String Conversions]
// =================================================================================

String stringFromCFString(const CFType<CFStringRef>& str);
CFStringRef CFStringFromString(const String& str);

} // Fog namespace

#else
#warning "Fog::MacUtil support not enabled but header file included!"
#endif // FOG_OS_MAC

// [Guard]
#endif // _FOG_CORE_MACUTIL_H
