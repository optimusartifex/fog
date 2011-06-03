// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_STRINGTMP_P_H
#define _FOG_CORE_TOOLS_STRINGTMP_P_H

// [Dependencies]
#include <Fog/Core/Tools/String.h>

//! @addtogroup Fog_Core_Tools
//! @{

namespace Fog {

// ============================================================================
// [Fog::StringTmp<N>]
// ============================================================================

template<size_t N>
struct StringTmp : public String
{
  // --------------------------------------------------------------------------
  // [Temporary Storage]
  // --------------------------------------------------------------------------

  // Keep 'Storage' name for this struct for Borland compiler
  struct Storage
  {
    StringData _d;
    Char _str[N];
  } _storage;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE StringTmp() :
    String(StringData::adopt((void*)&_storage, N))
  {
  }

  FOG_INLINE StringTmp(Char ch) :
    String(StringData::adopt((void*)&_storage, N, &ch, 1))
  {
  }

  FOG_INLINE StringTmp(const Ascii8& str) :
    String(StringData::adopt((void*)&_storage, N, str.getData(), str.getLength()))
  {
  }

  FOG_INLINE StringTmp(const Utf16& str) :
    String(StringData::adopt((void*)&_storage, N, str.getData(), str.getLength()))
  {
  }

  FOG_INLINE StringTmp(const String& other) :
    String(StringData::adopt((void*)&_storage, N, other.getData(), other.getLength()))
  {
  }

  FOG_INLINE StringTmp(const StringTmp<N>& other) :
    String(StringData::adopt((void*)&_storage, N, other.getData(), other.getLength()))
  {
  }

  FOG_INLINE StringTmp(const Char* str) :
    String(StringData::adopt((void*)&_storage, N, str, DETECT_LENGTH))
  {
  }

  // --------------------------------------------------------------------------
  // [Implicit StringData]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    if ((void*)_d != (void*)&_storage)
    {
      atomicPtrXchg(&_d, StringData::adopt((void*)&_storage, N))->deref();
    }
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  // These overloads are needed to succesfully use this template (or implicit
  // conversion will break template and new string will be allocated).

  FOG_INLINE StringTmp<N>& operator=(Char ch) { set(ch); return *this; }
  FOG_INLINE StringTmp<N>& operator=(const Ascii8& str) { set(str); return *this; }
  FOG_INLINE StringTmp<N>& operator=(const Utf16& str) { set(str); return *this; }
  FOG_INLINE StringTmp<N>& operator=(const String& other) { set(other); return *this; }
  FOG_INLINE StringTmp<N>& operator=(const StringTmp<N>& other) { set(other); return *this; }
  FOG_INLINE StringTmp<N>& operator=(const Char* str) { set(str); return *this; }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_STRINGTMP_P_H
