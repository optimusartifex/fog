// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Core/Build.h>

#if defined(FOG_OS_MAC)

#include <Fog/Core/ByteArray.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Core/MacUtil.h>

namespace Fog {

// =============================================================================
// [RetainPtr]
// =============================================================================

template <typename T> template <typename U>
inline RetainPtr<T>& RetainPtr<T>::operator=(const RetainPtr<U>& o)
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

template <typename T> template <typename U>
inline RetainPtr<T>& RetainPtr<T>::operator=(U* optr)
{
  if (optr)
    CFRetain(optr);
  PtrType ptr = _ptr;
  _ptr = optr;
  if (ptr)
    CFRelease(ptr);
  return *this;
}

// ==============================================================================
// [String Conversion]
// ==============================================================================

String stringFromCFString(const RetainPtr<CFStringRef>& str)
{
  sysuint_t len = CFStringGetLength(str.get());
  char* data;
  if (CFStringGetCString(str.get(), data, len, kCFStringEncodingUnicode))
  return String(Utf16((Char*)data, len));
  return String();
}

CFStringRef CFStringFromString(const String& str)
{
  ByteArray tmp;
  TextCodec::utf32().fromUnicode(tmp, str);
  return CFStringCreateWithCString(NULL, tmp.getData(), kCFStringEncodingUnicode);
}

// TODO NSString -> String and vice versa

} // Fog namespace

#endif // defined(FOG_OS_MAC)