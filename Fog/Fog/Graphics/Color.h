// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_COLOR_H
#define _FOG_GRAPHICS_COLOR_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Swap.h>
#include <Fog/Graphics/Constants.h>

namespace Fog {

// ============================================================================
// [Constants]
// ============================================================================

// Defined also by Fog/Graphics/RasterEngine_p.h, we need it for efficient
// demultiply.
extern FOG_API const uint32_t raster_demultiply_reciprocal_table_d[256];

//! @addtogroup Fog_Graphics_Painting
//! @{

// ============================================================================
// [Fog::Argb]
// ============================================================================

#include <Fog/Core/Compiler/PackByte.h>
//! @brief Color in ARGB32 format.
//!
//! Use to store and manipulate 32-bit ARGB color entities (8-bits for each
//! component).
struct FOG_HIDDEN Argb
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create the @c Argb instance, uninitialized.
  FOG_INLINE Argb()
  {
  }

  //! @brief Create the @c Argb instance, initializing it to packed @a value
  //! data.
  FOG_INLINE Argb(uint32_t value) :
    _value(value)
  {
  }

  //! @brief Create the @c Argb instance, initializing individual components
  //! to @a a, @a r, @a g and @a b.
  FOG_INLINE Argb(uint32_t a, uint32_t r, uint32_t g, uint32_t b) :
    _value((a << ARGB32_ASHIFT) | (r << ARGB32_RSHIFT) |
           (g << ARGB32_GSHIFT) | (b << ARGB32_BSHIFT) )
  {
  }

  //! @brief Create the @c Argb instance, copying components from @a other one.
  FOG_INLINE Argb(const Argb& other) :
    _value(other._value)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors - Members]
  // --------------------------------------------------------------------------

  //! @brief Get alpha value of ARGB quad.
  FOG_INLINE uint32_t getAlpha() const { return _a; }
  //! @brief Get red value of ARGB quad.
  FOG_INLINE uint32_t getRed  () const { return _r; }
  //! @brief Get green value of ARGB quad.
  FOG_INLINE uint32_t getGreen() const { return _g; }
  //! @brief Get blue value of ARGB quad.
  FOG_INLINE uint32_t getBlue () const { return _b; }
  //! @brief Get packed value.
  FOG_INLINE uint32_t getValue() const { return _value; }

  //! @brief Get packed value as an another @c Argb instance.
  FOG_INLINE Argb getArgb() const { return Argb(_value); }

  //! @brief Get grey value (converting RGB components to grey).
  FOG_INLINE uint32_t getGrey() const { return getGrey(_value); }

  //! @brief Set alpha value of ARGB quad.
  FOG_INLINE void setAlpha(uint32_t a) { _a = a; }
  //! @brief Set red value of ARGB quad.
  FOG_INLINE void setRed  (uint32_t r) { _r = r; }
  //! @brief Set green value of ARGB quad.
  FOG_INLINE void setGreen(uint32_t g) { _g = g; }
  //! @brief Set blue value of ARGB quad.
  FOG_INLINE void setBlue (uint32_t b) { _b = b; }
  //! @brief Set packed value of ARGB quad.
  FOG_INLINE void setValue(uint32_t value) { _value = value; }

  FOG_INLINE void setArgb(const Argb& other) { _value = other._value; }
  FOG_INLINE void setArgb(uint32_t a, uint32_t r, uint32_t g, uint32_t b)
  {
    _value = (a << ARGB32_ASHIFT) | (r << ARGB32_RSHIFT) |
             (g << ARGB32_GSHIFT) | (b << ARGB32_BSHIFT) ;
  }

  //! @brief Set red, green and blue components to @a grey.
  FOG_INLINE void setGrey (uint32_t grey) { _r = _g = _b = grey; }

  // --------------------------------------------------------------------------
  // [Accessors - Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t getAlpha(uint32_t argb) { return (argb & ARGB32_AMASK) >> ARGB32_ASHIFT; }
  static FOG_INLINE uint32_t getRed  (uint32_t argb) { return (argb & ARGB32_RMASK) >> ARGB32_RSHIFT; }
  static FOG_INLINE uint32_t getGreen(uint32_t argb) { return (argb & ARGB32_GMASK) >> ARGB32_GSHIFT; }
  static FOG_INLINE uint32_t getBlue (uint32_t argb) { return (argb & ARGB32_BMASK) >> ARGB32_BSHIFT; }

  static FOG_INLINE uint32_t getGrey(uint32_t argb)
  {
    // We are using this formula:
    //  0.212671 * R + 0.715160 * G + 0.072169 * B;
    // As:
    //  (13938 * R + 46868 * G + 4730 * B) / 65536

    uint32_t grey =
      ((argb >> 16) & 0xFF) * 13938 +
      ((argb >>  8) & 0xFF) * 46868 +
      ((argb      ) & 0xFF) *  4730 ;

    return grey >> 16;
  }

  static FOG_INLINE uint32_t make(uint32_t a, uint32_t r, uint32_t g, uint32_t b)
  {
    return (a << ARGB32_ASHIFT) | (r << ARGB32_RSHIFT) |
           (g << ARGB32_GSHIFT) | (b << ARGB32_BSHIFT) ;
  }

  // --------------------------------------------------------------------------
  // [Swap - Members]
  // --------------------------------------------------------------------------

  //! @brief Swap RGB (swapping red and blue values).
  FOG_INLINE void swapRgb() { swap(_r, _b); }
  //! @brief Swap ARGB (byteswapping all packed components).
  FOG_INLINE void swapArgb() { _value = Memory::bswap32(_value); }

  // --------------------------------------------------------------------------
  // [Swap - Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t swapRgb(uint32_t argb)
  {
    return (argb & 0xFF00FF00) | ((argb & 0xFF) << 16) | ((argb >> 16) & 0xFF);
  }

  static FOG_INLINE uint32_t swapArgb(uint32_t argb)
  {
    return Memory::bswap32(argb);
  }

  // --------------------------------------------------------------------------
  // [IsAlpha0xXX - Members]
  // --------------------------------------------------------------------------

  //! @brief Get whether an alpha in @a argb is 0xFF (fully opaque, 255 in decimal).
  FOG_INLINE bool isAlpha0xFF() const { return (_value >= 0xFF000000); }
  //! @brief Get whether an alpha in @a argb is 0x00 (fully transparent, 0 in decimal).
  FOG_INLINE bool isAlpha0x00() const { return (_value <= 0x00FFFFFF); }

  // --------------------------------------------------------------------------
  // [IsAlpha0xXX - Statics]
  // --------------------------------------------------------------------------

  //! @brief Get whether an alpha in @a argb is 0xFF (fully opaque, 255 in decimal).
  //!
  //! Adventage of this function is that it should expand only to 2 assembler
  //! instructions, using the whole 32-bit packed @a argb value.
  static FOG_INLINE bool isAlpha0xFF(uint32_t argb) { return (argb >= 0xFF000000); }

  //! @brief Get whether an alpha in @a argb is 0x00 (fully transparent, 0 in decimal).
  //!
  //! Adventage of this function is that it should expand only to 2 assembler
  //! instructions, using the whole 32-bit packed @a argb value.
  static FOG_INLINE bool isAlpha0x00(uint32_t argb) { return (argb <= 0x00FFFFFF); }

  // --------------------------------------------------------------------------
  // [Operations - Statics]
  // --------------------------------------------------------------------------

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

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Cast to packed 32-bit integer.
  FOG_INLINE operator uint32_t() const { return _value; }

  FOG_INLINE Argb& operator=(const Argb& other) { _value = other._value; return *this; }
  FOG_INLINE Argb& operator=(uint32_t value) { _value = value; return *this; }

  FOG_INLINE Argb& operator|=(uint32_t value) { _value |= value; return *this; }
  FOG_INLINE Argb& operator&=(uint32_t value) { _value &= value; return *this; }
  FOG_INLINE Argb& operator^=(uint32_t value) { _value ^= value; return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union
  {
    struct
    {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
      uint8_t _b;
      uint8_t _g;
      uint8_t _r;
      uint8_t _a;
#else
      uint8_t _a;
      uint8_t _r;
      uint8_t _g;
      uint8_t _b;
#endif // FOG_BYTE_ORDER
    };
    uint32_t _value;
  };
};
#include <Fog/Core/Compiler/PackRestore.h>

// ============================================================================
// [Fog::ArgbStop]
// ============================================================================

//! @brief Argb color stop.
struct FOG_HIDDEN ArgbStop
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ArgbStop()
  {
  }

  FOG_INLINE ArgbStop(float offset, Argb argb) :
    _offset(offset),
    _argb(argb)
  {
  }

  FOG_INLINE ArgbStop(float offset, uint32_t a, uint32_t r, uint32_t g, uint32_t b) :
    _offset(offset),
    _argb(Argb(a, r, g, b))
  {
  }

  FOG_INLINE ArgbStop(const ArgbStop& other) :
    _offset(other._offset),
    _argb(other._argb)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getOffset() const { return _offset; }
  FOG_INLINE Argb getArgb() const { return _argb; }

  FOG_INLINE void setOffset(float offset) { _offset = offset; }
  FOG_INLINE void setArgb(const Argb& argb) { _argb = argb; }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE void normalize()
  {
    _offset = Math::bound<float>(_offset, 0.0f, 1.0f);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ArgbStop& operator=(const ArgbStop& other)
  {
    _offset = other._offset;
    _argb = other._argb;
    return *this;
  }

  FOG_INLINE bool operator==(const Fog::ArgbStop& other) const { return (_offset == other._offset) && (_argb == other._argb); }
  FOG_INLINE bool operator!=(const Fog::ArgbStop& other) const { return (_offset != other._offset) || (_argb != other._argb); }
  FOG_INLINE bool operator< (const Fog::ArgbStop& other) const { return (_offset <  other._offset) || (_offset == other._offset && _argb <  other._argb); }
  FOG_INLINE bool operator<=(const Fog::ArgbStop& other) const { return (_offset <  other._offset) || (_offset == other._offset && _argb <= other._argb); }
  FOG_INLINE bool operator> (const Fog::ArgbStop& other) const { return (_offset >  other._offset) || (_offset == other._offset && _argb >  other._argb); }
  FOG_INLINE bool operator>=(const Fog::ArgbStop& other) const { return (_offset >  other._offset) || (_offset == other._offset && _argb >= other._argb); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _offset;
  Argb _argb;
};

// ============================================================================
// [Fog::Ahsv]
// ============================================================================

//! @brief Color in AHSV (alpha, hue, saturation, value) format.
//!
//! Used to manipulate with 32-bit floating point colors using AHSV format.
struct FOG_HIDDEN Ahsv
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Ahsv() :
    _a(1.0), _h(0.0), _s(0.0), _v(0.0)
  {
  }

  FOG_INLINE Ahsv(float a, float h, float s, float v) :
    _a(a), _h(h), _s(s), _v(v)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE void setAhsv(float a, float h, float s, float v)
  { _a = a; _h = h; _s = s; _v = v; }

  FOG_INLINE void setAlpha(float a) { _a = a; }
  FOG_INLINE void setHue(float h) { _h = h; }
  FOG_INLINE void setSaturation(float s) { _s = s; }
  FOG_INLINE void setValue(float v) { _v = v; }

  FOG_INLINE float getAlpha() const { return _a; }
  FOG_INLINE float getHue() const { return _h; }
  FOG_INLINE float getSaturation() const { return _s; }
  FOG_INLINE float getValue() const { return _v; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _a;
  float _h;
  float _s;
  float _v;
};

//! @}

} // Fog namespace

FOG_DECLARE_TYPEINFO(Fog::Argb, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ArgbStop, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_GRAPHICS_COLOR_H
