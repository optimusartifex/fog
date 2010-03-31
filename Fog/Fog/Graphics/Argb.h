// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_ARGB_H
#define _FOG_GRAPHICS_ARGB_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Graphics/ArgbUtil.h>
#include <Fog/Graphics/Constants.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Fog::Argb]
// ============================================================================

#include <Fog/Core/Compiler/PackByte.h>
//! @brief Stores 32 bit RGBA color entity (8 bits for each channel).
struct FOG_HIDDEN Argb
{
  // --------------------------------------------------------------------------
  // [Enums]
  // --------------------------------------------------------------------------

  enum { SIZE = 4 };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Argb() {}
  FOG_INLINE Argb(uint32_t _value) : value(_value) {}

  FOG_INLINE Argb(uint8_t _a, uint8_t _r, uint8_t _g, uint8_t _b) :
    value(
      ((uint32_t)(_a) << ARGB32_ASHIFT) |
      ((uint32_t)(_r) << ARGB32_RSHIFT) |
      ((uint32_t)(_g) << ARGB32_GSHIFT) |
      ((uint32_t)(_b) << ARGB32_BSHIFT) )
  {}

  FOG_INLINE Argb(const Argb& rgba) :
    value(rgba.value)
  {}

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE Argb& set(const Argb& rgba) { value = rgba.value; return* this; }
  FOG_INLINE Argb& set(uint32_t rgba) { value = rgba; return* this; }
  FOG_INLINE Argb& set(uint8_t _a, uint8_t _r, uint8_t _g, uint8_t _b)
  {
    value = (((uint32_t)(_a) << ARGB32_ASHIFT) |
             ((uint32_t)(_r) << ARGB32_RSHIFT) |
             ((uint32_t)(_g) << ARGB32_GSHIFT) |
             ((uint32_t)(_b) << ARGB32_BSHIFT) );
    return* this;
  }

  // --------------------------------------------------------------------------
  // [Channels]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getAlpha() const { return a; }
  FOG_INLINE uint32_t getRed  () const { return r; }
  FOG_INLINE uint32_t getGreen() const { return g; }
  FOG_INLINE uint32_t getBlue () const { return b; }

  FOG_INLINE uint32_t getGrey () const { return getGrey(value); }
  FOG_INLINE uint32_t getValue() const { return value; }

  // --------------------------------------------------------------------------
  // [Swap]
  // --------------------------------------------------------------------------

  FOG_INLINE void swapRgb() { uint8_t t = r; r = g; g = t; }
  FOG_INLINE void bswap() { value = Memory::bswap32(value); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE operator uint32_t() const { return value; }

  FOG_INLINE Argb& operator=(const Argb& rgba) { return set(rgba); }
  FOG_INLINE Argb& operator=(uint32_t rgba) { return set(rgba); }

  FOG_INLINE Argb& operator|=(uint32_t _value) { value |= _value; return *this; }
  FOG_INLINE Argb& operator&=(uint32_t _value) { value &= _value; return *this; }
  FOG_INLINE Argb& operator^=(uint32_t _value) { value ^= _value; return *this; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t getAlpha(uint32_t rgba) { return (rgba & ARGB32_AMASK) >> ARGB32_ASHIFT; }
  static FOG_INLINE uint32_t getRed  (uint32_t rgba) { return (rgba & ARGB32_RMASK) >> ARGB32_RSHIFT; }
  static FOG_INLINE uint32_t getGreen(uint32_t rgba) { return (rgba & ARGB32_GMASK) >> ARGB32_GSHIFT; }
  static FOG_INLINE uint32_t getBlue (uint32_t rgba) { return (rgba & ARGB32_BMASK) >> ARGB32_BSHIFT; }

  static FOG_INLINE uint32_t getGrey(uint32_t rgba) { return ArgbUtil::getGrey(rgba); }

  static FOG_INLINE uint32_t half(uint32_t color1, uint32_t color2)
  {
    return (((((color1) ^ (color2)) & 0xFEFEFEFEU) >> 1) + ((color1) & (color2)));
  }

  static FOG_INLINE uint32_t add(uint32_t color1, uint32_t color2)
  {
#if defined(FOG_HARDCODE_SSE2)
    __m128i xmm0 = _mm_cvtsi32_si128((int)color1);
    __m128i xmm1 = _mm_cvtsi32_si128((int)(color2 & 0x00FFFFFF));
    return _mm_cvtsi128_si32(_mm_adds_epu8(xmm0, xmm1));
#else
    uint32_t c13 = (color1 & 0x00FF00FF) + (color2 & 0x00FF00FF);
    uint32_t c24 = (color1 & 0xFF00FF00) + (color2 & 0x0000FF00);

    if (c13 & 0xFF000000) c13 |= 0x00FF0000;
    if (c24 & 0x00FF0000) c24 |= 0x0000FF00;
    if (c13 & 0x0000FF00) c13 |= 0x000000FF;

    return (c13 & 0x00FF00FF) | (c24 & 0xFF00FF00);
#endif // FOG_HARDCODE_SSE2
  }

  static FOG_INLINE uint32_t sub(uint32_t color1, uint32_t color2)
  {
#if defined(FOG_HARDCODE_SSE2)
    __m128i xmm0 = _mm_cvtsi32_si128((int)(color1));
    __m128i xmm1 = _mm_cvtsi32_si128((int)(color2 & 0x00FFFFFF));
    return _mm_cvtsi128_si32(_mm_subs_epu8(xmm0, xmm1));
#else
    uint32_t c1 = (color1 & 0x00FF0000) - (color2 & 0x00FF0000);
    uint32_t c2 = (color1 & 0x0000FF00) - (color2 & 0x0000FF00);
    uint32_t c3 = (color1 & 0x000000FF) - (color2 & 0x000000FF);

    if (c1 & 0xFF00FFFF) c1 = 0;
    if (c2 & 0xFFFF00FF) c2 = 0;
    if (c3 & 0xFFFFFF00) c3 = 0;

    return (color1 & 0xFF000000) | c1 | c2 | c3;
#endif // FOG_HARDCODE_SSE2
  }

  static FOG_INLINE uint32_t make(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
  {
    return ((uint32_t)(r) << ARGB32_RSHIFT) |
           ((uint32_t)(g) << ARGB32_GSHIFT) |
           ((uint32_t)(b) << ARGB32_BSHIFT) |
           ((uint32_t)(a) << ARGB32_ASHIFT) ;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union
  {
    struct
    {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
      uint8_t b;
      uint8_t g;
      uint8_t r;
      uint8_t a;
#else
      uint8_t a;
      uint8_t r;
      uint8_t g;
      uint8_t b;
#endif // FOG_BYTE_ORDER
    };
    uint32_t value;
  };
};
#include <Fog/Core/Compiler/PackRestore.h>

// ============================================================================
// [Fog::ArgbStop]
// ============================================================================

struct FOG_HIDDEN ArgbStop
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ArgbStop() {}
  FOG_INLINE ArgbStop(float offset, Argb argb) :
    offset(offset), argb(argb) {}
  FOG_INLINE ArgbStop(float offset, uint8_t a, uint8_t r, uint8_t g, uint8_t b) :
    offset(offset), argb(Argb(a, r, g, b)) {}
  FOG_INLINE ArgbStop(const ArgbStop& other) :
    offset(other.offset), argb(other.argb) {}

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE void normalize()
  { offset = Math::bound<float>(offset, 0.0f, 1.0f); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ArgbStop& operator=(const ArgbStop& other)
  { offset = other.offset; argb = other.argb; return *this; }

  FOG_INLINE bool operator==(const Fog::ArgbStop& other) const { return (offset == other.offset) && (argb == other.argb); }
  FOG_INLINE bool operator!=(const Fog::ArgbStop& other) const { return (offset != other.offset) || (argb != other.argb); }
  FOG_INLINE bool operator< (const Fog::ArgbStop& other) const { return (offset <  other.offset) || (offset == other.offset && argb <  other.argb); }
  FOG_INLINE bool operator<=(const Fog::ArgbStop& other) const { return (offset <  other.offset) || (offset == other.offset && argb <= other.argb); }
  FOG_INLINE bool operator> (const Fog::ArgbStop& other) const { return (offset >  other.offset) || (offset == other.offset && argb >  other.argb); }
  FOG_INLINE bool operator>=(const Fog::ArgbStop& other) const { return (offset >  other.offset) || (offset == other.offset && argb >= other.argb); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float offset;
  Argb argb;
};

} // Fog namespace

//! @}

FOG_DECLARE_TYPEINFO(Fog::Argb, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ArgbStop, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_GRAPHICS_ARGB_H
