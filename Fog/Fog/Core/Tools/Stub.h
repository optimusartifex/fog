// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_STUB_H
#define _FOG_CORE_TOOLS_STUB_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Tools/Char.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ByteArray;
struct String;

// Used by Fog::Utf16.
namespace StringUtil {
FOG_API size_t len(const Char* str);
}

// ============================================================================
// [Fog::Stub8]
// ============================================================================

//! @brief 8-bit generic string (stub).
struct FOG_NO_EXPORT Stub8
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Stub8(const char* data, size_t length = DETECT_LENGTH) : _data(data), _length(length) {}
  explicit FOG_INLINE Stub8(const uint8_t* data, size_t length = DETECT_LENGTH) : _data(reinterpret_cast<const char*>(data)), _length(length) {}

  explicit FOG_INLINE Stub8(_Uninitialized) {}

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
    FOG_ASSERT_X(index <= _length, "Fog::Stub8::operator[] - Index out of range");
    return _data[index];
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  const char* _data;
  size_t _length;
};

// ============================================================================
// [Fog::Ascii8]
// ============================================================================

//! @brief 8-bit ASCII string (stub).
struct FOG_NO_EXPORT Ascii8 : public Stub8
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Ascii8(const char* data, size_t length = DETECT_LENGTH) : Stub8(data, length) {}
  explicit FOG_INLINE Ascii8(const uint8_t* data, size_t length = DETECT_LENGTH) : Stub8(data, length) {}

  explicit FOG_INLINE Ascii8(_Uninitialized) : Stub8(UNINITIALIZED) {}
};

// ============================================================================
// [Fog::Local8]
// ============================================================================

//! @brief 8-bit string encoded in local encoding (stub).
struct FOG_NO_EXPORT Local8 : public Stub8
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Local8(const char* data, size_t length = DETECT_LENGTH) : Stub8(data, length) {}
  explicit FOG_INLINE Local8(const uint8_t* data, size_t length = DETECT_LENGTH) : Stub8(data, length) {}

  explicit FOG_INLINE Local8(_Uninitialized) : Stub8(UNINITIALIZED) {}
};

// ============================================================================
// [Fog::Utf8]
// ============================================================================

//! @brief UTF-8 string (stub).
struct FOG_NO_EXPORT Utf8 : public Stub8
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Utf8(const char* data, size_t length = DETECT_LENGTH) : Stub8(data, length) {}
  explicit FOG_INLINE Utf8(const uint8_t* data, size_t length = DETECT_LENGTH) : Stub8(data, length) {}

  explicit FOG_INLINE Utf8(_Uninitialized) : Stub8(UNINITIALIZED) {}
};

// ============================================================================
// [Fog::Utf16]
// ============================================================================

//! @brief UTF-16 string (stub).
struct FOG_NO_EXPORT Utf16
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Utf16(const uint16_t* data, size_t length = DETECT_LENGTH) : _data(reinterpret_cast<const Char*>(data)), _length(length) {}
  explicit FOG_INLINE Utf16(const Char* data, size_t length = DETECT_LENGTH) : _data(data), _length(length) {}

  explicit FOG_INLINE Utf16(_Uninitialized) {}

#if FOG_SIZEOF_WCHAR_T == 2
  explicit FOG_INLINE Utf16(const wchar_t* data, size_t length = DETECT_LENGTH) : _data(reinterpret_cast<const Char*>(data)), _length(length) {}
#endif // FOG_SIZEOF_WCHAR_T == 2

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE const Char* getData() const { return _data; }
  FOG_INLINE void setData(const Char* data) { _data = data; }

  FOG_INLINE size_t getLength() const { return _length; }
  FOG_INLINE void setLength(size_t length) { _length = length; }

  FOG_INLINE size_t getComputedLength() const { return _length == DETECT_LENGTH ? StringUtil::len(_data) : _length; }

  // --------------------------------------------------------------------------
  // [Operator overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Char& operator[](size_t index) const
  {
    FOG_ASSERT_X(index < _length, "Fog::Utf16::operator[] - Index out of range");
    return _data[index];
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  const Char* _data;
  size_t _length;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::Stub8 , Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::Ascii8, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::Local8, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::Utf8  , Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::Utf16 , Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_CORE_TOOLS_STUB_H
