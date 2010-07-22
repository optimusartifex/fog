// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_BASICS_H
#define _FOG_CORE_BASICS_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Char.h>
#include <Fog/Core/TypeInfo.h>

namespace Fog {

//! @addtogroup Fog_Core_Essentials
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ByteArray;
struct Char;
struct String;

// ============================================================================
// [Fog::Str8]
// ============================================================================

//! @brief 8-bit generic string (stub).
struct FOG_HIDDEN Str8
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Str8(const char* data, sysuint_t length = DETECT_LENGTH) : _data(data), _length(length) {}
  explicit FOG_INLINE Str8(const uint8_t* data, sysuint_t length = DETECT_LENGTH) : _data(reinterpret_cast<const char*>(data)), _length(length) {}

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE const char* getData() const { return _data; }
  FOG_INLINE sysuint_t getLength() const { return _length; }

  // --------------------------------------------------------------------------
  // [Operator overload]
  // --------------------------------------------------------------------------

  FOG_INLINE char operator[](sysuint_t index) const
  {
    FOG_ASSERT_X(index <= _length, "Fog::Str8::operator[] - Index out of range");
    return _data[index];
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  const char* _data;
  sysuint_t _length;
};

// ============================================================================
// [Fog::Ascii8]
// ============================================================================

//! @brief 8-bit ASCII string (stub).
struct FOG_HIDDEN Ascii8 : public Str8
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Ascii8(const char* data, sysuint_t length = DETECT_LENGTH) : Str8(data, length) {}
  explicit FOG_INLINE Ascii8(const uint8_t* data, sysuint_t length = DETECT_LENGTH) : Str8(data, length) {}
};

// ============================================================================
// [Fog::Local8]
// ============================================================================

//! @brief 8-bit string encoded in local encoding (stub).
struct FOG_HIDDEN Local8 : public Str8
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Local8(const char* data, sysuint_t length = DETECT_LENGTH) : Str8(data, length) {}
  explicit FOG_INLINE Local8(const uint8_t* data, sysuint_t length = DETECT_LENGTH) : Str8(data, length) {}
};

// ============================================================================
// [Fog::Utf8]
// ============================================================================

//! @brief UTF-8 string (stub).
struct FOG_HIDDEN Utf8 : public Str8
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Utf8(const char* data, sysuint_t length = DETECT_LENGTH) : Str8(data, length) {}
  explicit FOG_INLINE Utf8(const uint8_t* data, sysuint_t length = DETECT_LENGTH) : Str8(data, length) {}
};

// ============================================================================
// [Fog::Utf16]
// ============================================================================

//! @brief UTF-16 string (stub).
struct FOG_HIDDEN Utf16
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  explicit FOG_INLINE Utf16(const uint16_t* data, sysuint_t length = DETECT_LENGTH) : _data(reinterpret_cast<const Char*>(data)), _length(length) {}
  explicit FOG_INLINE Utf16(const Char* data, sysuint_t length = DETECT_LENGTH) : _data(data), _length(length) {}

#if FOG_SIZEOF_WCHAR_T == 2
  explicit FOG_INLINE Utf16(const wchar_t* data, sysuint_t length = DETECT_LENGTH) : _data(reinterpret_cast<const Char*>(data)), _length(length) {}
#endif // FOG_SIZEOF_WCHAR_T == 2

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE const Char* getData() const { return _data; }
  FOG_INLINE sysuint_t getLength() const { return _length; }

  // --------------------------------------------------------------------------
  // [Operator overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const Char& operator[](sysuint_t index) const
  {
    FOG_ASSERT_X(index < _length, "Fog::Utf16::operator[] - Index out of range");
    return _data[index];
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  const Char* _data;
  sysuint_t _length;
};

// ============================================================================
// [Fog::FormatFlags]
// ============================================================================

//! @brief String format flags.
struct FOG_HIDDEN FormatFlags
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FormatFlags(sysuint_t precision = NO_PRECISION, sysuint_t width = NO_WIDTH, uint32_t flags = NO_FLAGS) :
    precision(precision), width(width), flags(flags)
  {
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  sysuint_t precision;
  sysuint_t width;
  uint32_t flags;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<T>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Str8       , Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::Ascii8     , Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::Local8     , Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::Utf8       , Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::Utf16      , Fog::TYPEINFO_PRIMITIVE)

FOG_DECLARE_TYPEINFO(Fog::FormatFlags, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_CORE_BASICS_H
