// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_STUB_H
#define _FOG_CORE_TOOLS_STUB_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/Char.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::StubA]
// ============================================================================

//! @brief 8-bit generic string (stub).
struct FOG_NO_EXPORT StubA
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE StubA(const char* data, size_t length = DETECT_LENGTH) : _data(data), _length(length) {}
  explicit FOG_INLINE StubA(const uint8_t* data, size_t length = DETECT_LENGTH) : _data(reinterpret_cast<const char*>(data)), _length(length) {}

  explicit FOG_INLINE StubA(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE const char* getData() const { return _data; }
  FOG_INLINE void setData(const char* data) { _data = data; }
  FOG_INLINE void setData(const uint8_t* data) { _data = reinterpret_cast<const char*>(data); }

  FOG_INLINE size_t getLength() const { return _length; }
  FOG_INLINE void setLength(size_t length) { _length = length; }

  FOG_INLINE size_t getComputedLength() const { return _length == DETECT_LENGTH ? strlen(_data) : _length; }

  // --------------------------------------------------------------------------
  // [Operator overload]
  // --------------------------------------------------------------------------

  FOG_INLINE char operator[](size_t index) const
  {
    FOG_ASSERT_X(index <= _length, "Fog::StubA::operator[] - Index out of range");
    return _data[index];
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  const char* _data;
  size_t _length;
};

// ============================================================================
// [Fog::Ascii8]
// ============================================================================

//! @brief 8-bit ASCII string (stub).
struct FOG_NO_EXPORT Ascii8 : public StubA
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Ascii8(const char* data, size_t length = DETECT_LENGTH) : StubA(data, length) {}
  explicit FOG_INLINE Ascii8(const uint8_t* data, size_t length = DETECT_LENGTH) : StubA(data, length) {}

  explicit FOG_INLINE Ascii8(_Uninitialized) : StubA(UNINITIALIZED) {}
};

// ============================================================================
// [Fog::Local8]
// ============================================================================

//! @brief 8-bit string encoded in local encoding (stub).
struct FOG_NO_EXPORT Local8 : public StubA
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Local8(const char* data, size_t length = DETECT_LENGTH) : StubA(data, length) {}
  explicit FOG_INLINE Local8(const uint8_t* data, size_t length = DETECT_LENGTH) : StubA(data, length) {}

  explicit FOG_INLINE Local8(_Uninitialized) : StubA(UNINITIALIZED) {}
};

// ============================================================================
// [Fog::Utf8]
// ============================================================================

//! @brief UTF-8 string (stub).
struct FOG_NO_EXPORT Utf8 : public StubA
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Utf8(const char* data, size_t length = DETECT_LENGTH) : StubA(data, length) {}
  explicit FOG_INLINE Utf8(const uint8_t* data, size_t length = DETECT_LENGTH) : StubA(data, length) {}

  explicit FOG_INLINE Utf8(_Uninitialized) : StubA(UNINITIALIZED) {}
};

// ============================================================================
// [Fog::StubW]
// ============================================================================

//! @brief UTF-16 string (stub).
struct FOG_NO_EXPORT StubW
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE StubW(const uint16_t* data, size_t length = DETECT_LENGTH) :
    _data(reinterpret_cast<const CharW*>(data)),
    _length(length)
  {
  }

  explicit FOG_INLINE StubW(const CharW* data, size_t length = DETECT_LENGTH) :
    _data(data),
    _length(length)
  {
  }

  explicit FOG_INLINE StubW(_Uninitialized)
  {
  }

#if FOG_SIZEOF_WCHAR_T == 2
  explicit FOG_INLINE StubW(const wchar_t* data, size_t length = DETECT_LENGTH) :
    _data(reinterpret_cast<const CharW*>(data)),
    _length(length)
  {
  }
#endif // FOG_SIZEOF_WCHAR_T == 2

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE const CharW* getData() const { return _data; }
  FOG_INLINE void setData(const CharW* data) { _data = data; }

  FOG_INLINE size_t getLength() const { return _length; }
  FOG_INLINE void setLength(size_t length) { _length = length; }

  FOG_INLINE size_t getComputedLength() const { return _length == DETECT_LENGTH ? fog_api.stringutil_lenW(_data) : _length; }

  // --------------------------------------------------------------------------
  // [Operator overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const CharW& operator[](size_t index) const
  {
    FOG_ASSERT_X(index < _length, "Fog::StubW::operator[] - Index out of range");
    return _data[index];
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  const CharW* _data;
  size_t _length;
};

// ============================================================================
// [Fog::StubT<>]
// ============================================================================

_FOG_CHAR_T(Stub)
_FOG_CHAR_A(Stub)
_FOG_CHAR_W(Stub)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_STUB_H
