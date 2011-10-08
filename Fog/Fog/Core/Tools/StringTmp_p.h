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
// [Fog::StringTmpA<N>]
// ============================================================================

template<size_t N>
struct StringTmpA : public StringA
{
  struct _Storage;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE StringTmpA() :
    StringA(StringA::_dAdopt((void*)&_storage, N))
  {
  }

  FOG_INLINE StringTmpA(const char ch) :
    StringA(StringA::_dAdopt((void*)&_storage, N, StubA(&ch, 1)))
  {
  }

  FOG_INLINE StringTmpA(const StubA& str) :
    StringA(StringA::_dAdopt((void*)&_storage, N, str))
  {
  }

  FOG_INLINE StringTmpA(const char* str) :
    StringA(StringA::_dAdopt((void*)&_storage, N, StubA(str, DETECT_LENGTH)))
  {
  }

  FOG_INLINE StringTmpA(const StringA& other) :
    StringA(StringA::_dAdopt((void*)&_storage, N, StubA(other.getData(), other.getLength())))
  {
  }

  FOG_INLINE StringTmpA(const StringTmpA<N>& other) :
    StringA(StringA::_dAdopt((void*)&_storage, N, StubA(other.getData(), other.getLength())))
  {
  }

  FOG_INLINE ~StringTmpA()
  {
    FOG_ASSERT( (_d == &_storage.d && _storage.d.reference.get() == 1) ||
                (_d != &_storage.d && _storage.d.reference.get() == 0) );
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _d->release();

    if (_storage.d.reference.get() == 0)
      _d = StringA::_dAdopt((void*)&_storage, N);
    else
      _d = _api.stringa_oEmpty->_d->addRef();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  // These overloads are needed to successfully use this template (or implicit
  // conversion will break template and new string will be allocated).

  FOG_INLINE StringTmpA<N>& operator=(char ch) { set(ch); return *this; }
  FOG_INLINE StringTmpA<N>& operator=(const StubA& str) { set(str); return *this; }
  FOG_INLINE StringTmpA<N>& operator=(const char* str) { set(str); return *this; }
  FOG_INLINE StringTmpA<N>& operator=(const StringA& other) { set(other); return *this; }
  FOG_INLINE StringTmpA<N>& operator=(const StringTmpA<N>& other) { set(other); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  struct _Storage
  {
    StringDataA d;
    char data[N];
  } _storage;
};

// ============================================================================
// [Fog::StringTmpW<N>]
// ============================================================================

template<size_t N>
struct StringTmpW : public StringW
{
  struct _Storage;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE StringTmpW() :
    StringW(StringW::_dAdopt((void*)&_storage, N))
  {
  }

  FOG_INLINE StringTmpW(CharW ch) :
    StringW(StringW::_dAdopt((void*)&_storage, N, StubW(&ch, 1)))
  {
  }

  FOG_INLINE StringTmpW(const Ascii8& str) :
    StringW(StringW::_dAdopt((void*)&_storage, N, StubW(str.getData(), str.getLength())))
  {
  }

  FOG_INLINE StringTmpW(const StubW& str) :
    StringW(StringW::_dAdopt((void*)&_storage, N, StubW(str.getData(), str.getLength())))
  {
  }

  FOG_INLINE StringTmpW(const StringW& other) :
    StringW(StringW::_dAdopt((void*)&_storage, N, StubW(other.getData(), other.getLength())))
  {
  }

  FOG_INLINE StringTmpW(const StringTmpW<N>& other) :
    StringW(StringW::_dAdopt((void*)&_storage, N, StubW(other.getData(), other.getLength())))
  {
  }

  FOG_INLINE StringTmpW(const CharW* str) :
    StringW(StringW::_dAdopt((void*)&_storage, N, StubW(str, DETECT_LENGTH)))
  {
  }

  FOG_INLINE ~StringTmpW()
  {
    FOG_ASSERT( (_d == &_storage.d && _storage.d.reference.get() == 1) ||
                (_d != &_storage.d && _storage.d.reference.get() == 0) );
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _d->release();

    if (_storage.d.reference.get() == 0)
      _d = StringW::_dAdopt((void*)&_storage, N);
    else
      _d = _api.stringw_oEmpty->_d->addRef();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  // These overloads are needed to successfully use this template (or implicit
  // conversion will break template and new string will be allocated).

  FOG_INLINE StringTmpW<N>& operator=(CharW ch) { set(ch); return *this; }
  FOG_INLINE StringTmpW<N>& operator=(const Ascii8& str) { set(str); return *this; }
  FOG_INLINE StringTmpW<N>& operator=(const StubW& str) { set(str); return *this; }
  FOG_INLINE StringTmpW<N>& operator=(const StringW& other) { set(other); return *this; }
  FOG_INLINE StringTmpW<N>& operator=(const StringTmpW<N>& other) { set(other); return *this; }
  FOG_INLINE StringTmpW<N>& operator=(const CharW* str) { set(str); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  struct _Storage
  {
    StringDataW d;
    CharW data[N];
  } _storage;
};

// ============================================================================
// [Fog::StringTmpT<>]
// ============================================================================

_FOG_CHAR_TEMPLATE1_T(StringTmp, size_t, N)
_FOG_CHAR_TEMPLATE1_A(StringTmp, size_t, N)
_FOG_CHAR_TEMPLATE1_W(StringTmp, size_t, N)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_STRINGTMP_P_H
