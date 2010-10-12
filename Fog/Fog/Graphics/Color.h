// [Fog-Graphics]
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
#include <Fog/Core/Static.h>
#include <Fog/Core/Swap.h>
#include <Fog/Graphics/Constants.h>

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct AcmykF;
struct AhsvF;
struct ArgbF;
struct ArgbI;
struct Color;
struct ColorStop;

//! @addtogroup Fog_Graphics_Painting
//! @{

// ============================================================================
// [Fog::ArgbI]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief ARGB color as 32-bit entity (8-bit per component).
struct FOG_HIDDEN ArgbI
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create the @c ArgbI instance, uninitialized.
  FOG_INLINE ArgbI() :
    _packed(0x00000000)
  {
  }

  //! @brief Create the @c ArgbI instance, initializing individual components
  //! to @a a, @a r, @a g and @a b.
  FOG_INLINE ArgbI(uint32_t a, uint32_t r, uint32_t g, uint32_t b) :
    _packed(pack(a, r, g, b))
  {
  }

  //! @brief Create the @c ArgbI instance, copying components from @a other one.
  FOG_INLINE ArgbI(const ArgbI& other) :
    _packed(other._packed)
  {
  }

  //! @brief Create the @c ArgbI instance, initializing it to packed @a value
  //! data.
  FOG_INLINE ArgbI(uint32_t packed) :
    _packed(packed)
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
  //! @brief Get grey value (converting RGB components to grey, ignoring alpha).
  FOG_INLINE uint32_t getGrey() const { return getGrey(_packed); }

  //! @brief Get packed value.
  FOG_INLINE uint32_t getPacked() const { return _packed; }

  //! @brief Convert color to @c ArgbI (convenience).
  FOG_INLINE ArgbI getArgbI() const { return ArgbI(_packed); }
  //! @brief Convert color to @c ArgbF.
  FOG_INLINE ArgbF getArgbF() const;
  //! @brief Convert color to @c AhsvF.
  FOG_INLINE AhsvF getAhsvF() const;
  //! @brief Convert color to @c AcmykF.
  FOG_INLINE AcmykF getAcmykF() const;

  //! @brief Set alpha value of ARGB quad.
  FOG_INLINE void setAlpha(uint32_t a) { _a = a; }
  //! @brief Set red value of ARGB quad.
  FOG_INLINE void setRed(uint32_t r) { _r = r; }
  //! @brief Set green value of ARGB quad.
  FOG_INLINE void setGreen(uint32_t g) { _g = g; }
  //! @brief Set blue value of ARGB quad.
  FOG_INLINE void setBlue(uint32_t b) { _b = b; }
  //! @brief Set red, green and blue components to @a grey.
  FOG_INLINE void setGrey (uint32_t grey) { _r = _g = _b = grey; }

  //! @brief Set all values to @a packed.
  FOG_INLINE void setPacked(uint32_t packed) { _packed = packed; }

  //! @brief Set all values to @a other.
  FOG_INLINE void setArgb(const ArgbI& other) { _packed = other._packed; }
  //! @brief Set all values to @a a, @a r, @a g, @a b.
  FOG_INLINE void setArgb(uint32_t a, uint32_t r, uint32_t g, uint32_t b) { _packed = pack(a, r, g, b); }

  //! @brief Set all values to @a other (converting color entities to 8-bit).
  FOG_INLINE void setAhsv(const AhsvF& other);
  //! @brief Set all values to @a other (converting from AHSV color format).
  FOG_INLINE void setAhsv(float a, float h, float s, float v);
  //! @brief Set all values to @a other (converting from CMYK color format).
  FOG_INLINE void setAcmyk(const AcmykF& other);
  //! @brief Set all values to @a other (converting from CMYK color format).
  FOG_INLINE void setAcmyk(float a, float c, float m, float y, float k);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { _packed = 0x00000000; }

  // --------------------------------------------------------------------------
  // [Swap]
  // --------------------------------------------------------------------------

  //! @brief Swap RGB (swap red and blue).
  FOG_INLINE void swapRgb() { swap(_r, _b); }
  //! @brief Swap ARGB (spaw alpha adn blue, red and green == byteswap).
  FOG_INLINE void swapArgb() { _packed = Memory::bswap32(_packed); }

  // --------------------------------------------------------------------------
  // [IsOpaque / IsTransparent]
  // --------------------------------------------------------------------------

  //! @brief Get whether the alpha is fully-opaque (255 in decimal).
  FOG_INLINE bool isOpaque() const { return (_packed >= 0xFF000000); }
  //! @brief Get whether the alpha is fully-transparent (0 in decimal).
  FOG_INLINE bool isTransparent() const { return (_packed <= 0x00FFFFFF); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Cast to packed 32-bit integer.
  FOG_INLINE operator uint32_t() const { return _packed; }

  FOG_INLINE ArgbI& operator=(const ArgbI& other) { _packed = other._packed; return *this; }
  FOG_INLINE ArgbI& operator=(uint32_t packed) { _packed = packed; return *this; }

  FOG_INLINE ArgbI& operator|=(uint32_t packed) { _packed |= packed; return *this; }
  FOG_INLINE ArgbI& operator&=(uint32_t packed) { _packed &= packed; return *this; }
  FOG_INLINE ArgbI& operator^=(uint32_t packed) { _packed ^= packed; return *this; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t getComponent(uint32_t argb, uint32_t position)
  {
    argb >>= position;
    if (position < 24) argb &= 0xFFU;
    return argb;
  }

  static FOG_INLINE uint32_t getAlpha(uint32_t argb) { return getComponent(argb, ARGB32_ASHIFT); }
  static FOG_INLINE uint32_t getRed  (uint32_t argb) { return getComponent(argb, ARGB32_RSHIFT); }
  static FOG_INLINE uint32_t getGreen(uint32_t argb) { return getComponent(argb, ARGB32_GSHIFT); }
  static FOG_INLINE uint32_t getBlue (uint32_t argb) { return getComponent(argb, ARGB32_BSHIFT); }

  static FOG_INLINE uint32_t getGrey(uint32_t argb)
  {
    uint32_t grey =
      ((argb >> 16) & 0xFF) * 13938 +
      ((argb >>  8) & 0xFF) * 46868 +
      ((argb      ) & 0xFF) *  4730 ;

    return grey >> 16;
  }

  static FOG_INLINE uint32_t pack(uint32_t a, uint32_t r, uint32_t g, uint32_t b)
  {
    return (a << ARGB32_ASHIFT) | (r << ARGB32_RSHIFT) |
           (g << ARGB32_GSHIFT) | (b << ARGB32_BSHIFT) ;
  }

  static FOG_INLINE uint32_t swapRgb(uint32_t argb)
  {
    return (argb & 0xFF00FF00) | ((argb & 0xFF) << 16) | ((argb >> 16) & 0xFF);
  }

  static FOG_INLINE uint32_t swapArgb(uint32_t argb)
  {
    return Memory::bswap32(argb);
  }

  //! @brief Get whether the alpha is fully-opaque (255 in decimal).
  //!
  //! Advantage of using this function is that it should expand only to 2 assembler
  //! instructions, using the whole 32-bit packed @a argb value.
  static FOG_INLINE bool isOpaque(uint32_t argb) { return (argb >= 0xFF000000); }

  //! @brief Get whether the alpha is fully-transparent (0 in decimal).
  //!
  //! Advantage of using this function is that it should expand only to 2 assembler
  //! instructions, using the whole 32-bit packed @a argb value.
  static FOG_INLINE bool isTransparent(uint32_t argb) { return (argb <= 0x00FFFFFF); }

  // --------------------------------------------------------------------------
  // [Operations]
  // --------------------------------------------------------------------------

  // TODO: Do something with these methods...

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
    uint32_t _packed;
  };
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::ArgbF]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief ARGB color in floating point format (32-bit float per component).
struct FOG_HIDDEN ArgbF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create the @c ArgbF instance, initializing all components to zero.
  FOG_INLINE ArgbF() :
    _a(0.0f),
    _r(0.0f),
    _g(0.0f),
    _b(0.0f)
  {
  }

  explicit FOG_INLINE ArgbF(_DONT_INITIALIZE) {};

  explicit FOG_INLINE ArgbF(const ArgbI& other)
  {
    _a = (float)other.getAlpha() * (1.0f / 255.0f);
    _r = (float)other.getRed()   * (1.0f / 255.0f);
    _g = (float)other.getGreen() * (1.0f / 255.0f);
    _b = (float)other.getBlue()  * (1.0f / 255.0f);
  }

  //! @brief Create the @c ArgbF instance, initializing individual components
  //! from @a packed 32-bit value.
  explicit FOG_INLINE ArgbF(uint32_t packed)
  {
    _a = (float)(ArgbI::getAlpha(packed)) * (1.0f / 255.0f);
    _r = (float)(ArgbI::getRed  (packed)) * (1.0f / 255.0f);
    _g = (float)(ArgbI::getGreen(packed)) * (1.0f / 255.0f);
    _b = (float)(ArgbI::getBlue (packed)) * (1.0f / 255.0f);
  }

  //! @brief Create the @c ArgbI instance, copying components from @a other one.
  FOG_INLINE ArgbF(const ArgbF& other) :
    _a(other._a),
    _r(other._r),
    _g(other._g),
    _b(other._b)
  {
  }

  //! @brief Create the @c ArgbF instance, initializing individual components
  //! to @a a, @a r, @a g and @a b.
  FOG_INLINE ArgbF(float a, float r, float g, float b) :
    _a(a),
    _r(r),
    _g(g),
    _b(b)
  {
  }

  explicit FOG_INLINE ArgbF(const AhsvF& ahsv) { setAhsv(ahsv); }
  explicit FOG_INLINE ArgbF(const AcmykF& acmyk) { setAcmyk(acmyk); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get alpha value of ARGB quad.
  FOG_INLINE float getAlpha() const { return _a; }
  //! @brief Get red value of ARGB quad.
  FOG_INLINE float getRed() const { return _r; }
  //! @brief Get green value of ARGB quad.
  FOG_INLINE float getGreen() const { return _g; }
  //! @brief Get blue value of ARGB quad.
  FOG_INLINE float getBlue() const { return _b; }

  //! @brief Get grey value (converting RGB components to grey).
  FOG_INLINE float getGrey() const
  {
    return Math::min(_r * 0.212671f + _g * 0.715160f + _b * 0.072169f, 1.0f);
  }

  FOG_INLINE float* getData() { return _data; }
  FOG_INLINE const float* getData() const { return _data; }

  //! @brief Convert color to @c ArgbI.
  FOG_INLINE ArgbI getArgbI() const
  {
    return ArgbI((int)(_a * 255.0f), (int)(_r * 255.0f), (int)(_g * 255.0f), (int)(_b * 255.0f));
  }

  //! @brief Convert color to @c ArgbF (convenience).
  FOG_INLINE ArgbF getArgbF() const 
  {
    return ArgbF(*this);
  }

  //! @brief Convert color to @c AhsvF.
  FOG_INLINE AhsvF getAhsvF() const;
  //! @brief Convert color to @c AcmykF.
  FOG_INLINE AcmykF getAcmykF() const;

  //! @brief Set alpha value of ARGB quad.
  FOG_INLINE void setAlpha(float a) { _a = a; }
  //! @brief Set red value of ARGB quad.
  FOG_INLINE void setRed(float r) { _r = r; }
  //! @brief Set green value of ARGB quad.
  FOG_INLINE void setGreen(float g) { _g = g; }
  //! @brief Set blue value of ARGB quad.
  FOG_INLINE void setBlue(float b) { _b = b; }

  //! @brief Set red, green and blue components to @a grey.
  FOG_INLINE void setGrey (float grey) { _r = _g = _b = grey; }

  FOG_INLINE void setArgb(const ArgbI& other)
  {
    _a = (float)other.getAlpha() * (1.0f / 255.0f);
    _r = (float)other.getRed()   * (1.0f / 255.0f);
    _g = (float)other.getGreen() * (1.0f / 255.0f);
    _b = (float)other.getBlue()  * (1.0f / 255.0f);
  }

  FOG_INLINE void setArgb(const ArgbF& other)
  {
    _a = other._a;
    _r = other._r;
    _g = other._g;
    _b = other._b;
  }

  FOG_INLINE void setArgb(float a, float r, float g, float b)
  {
    _a = a;
    _r = r;
    _g = g;
    _b = b;
  }

  FOG_INLINE void setAhsv(const AhsvF& ahsv);
  FOG_INLINE void setAhsv(float a, float h, float s, float v);
  FOG_INLINE void setAcmyk(const AcmykF& acmyk);
  FOG_INLINE void setAcmyk(float a, float c, float m, float y, float k);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { _a = 0.0f; _r = 0.0f; _g = 0.0f; _b = 0.0f; }

  // --------------------------------------------------------------------------
  // [Swap]
  // --------------------------------------------------------------------------

  //! @brief Swap RGB (swap red and blue).
  FOG_INLINE void swapRgb() { swap(_r, _b); }
  //! @brief Swap ARGB (swap alpha and blue, red and green).
  FOG_INLINE void swapArgb() { swap(_a, _b); swap(_r, _g); }

  // --------------------------------------------------------------------------
  // [IsOpaque / IsTransparent]
  // --------------------------------------------------------------------------

  //! @brief Get whether the alpha is close to 1.0 (fully-opaque).
  FOG_INLINE bool isOpaque() const { return Math::fgt(_a, 1.0f); }
  //! @brief Get whether the alpha is close to 0.0 (fully-transparent).
  FOG_INLINE bool isTransparent() const { return Math::flt(_a, 0.0f); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union
  {
    struct
    {
      float _a;
      float _r;
      float _g;
      float _b;
    };
    float _data[4];
  };
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::AhsvF]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief Color in AHSV (alpha, hue, saturation, value) format.
//!
//! Used to manipulate with 32-bit floating point colors using AHSV format.
struct FOG_HIDDEN AhsvF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE AhsvF() :
    _a(1.0),
    _h(0.0),
    _s(0.0),
    _v(0.0)
  {
  }

  explicit FOG_INLINE AhsvF(_DONT_INITIALIZE) {};

  FOG_INLINE AhsvF(const AhsvF& other) :
    _a(other._a),
    _h(other._h),
    _s(other._s),
    _v(other._v)
  {
  }

  FOG_INLINE AhsvF(float a, float h, float s, float v) :
    _a(a),
    _h(h),
    _s(s),
    _v(v)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getAlpha() const { return _a; }
  FOG_INLINE float getHue() const { return _h; }
  FOG_INLINE float getSaturation() const { return _s; }
  FOG_INLINE float getValue() const { return _v; }

  FOG_INLINE float* getData() { return _data; }
  FOG_INLINE const float* getData() const { return _data; }

  FOG_INLINE ArgbI getArgbI() const;
  FOG_INLINE ArgbF getArgbF() const;
  FOG_INLINE AhsvF getAhsvF() const { return AhsvF(*this); }
  FOG_INLINE AcmykF getAcmykF() const;

  FOG_INLINE void setAlpha(float a) { _a = a; }
  FOG_INLINE void setHue(float h) { _h = h; }
  FOG_INLINE void setSaturation(float s) { _s = s; }
  FOG_INLINE void setValue(float v) { _v = v; }

  FOG_INLINE void setArgb(const ArgbI& argb);
  FOG_INLINE void setArgb(const ArgbF& argb);
  FOG_INLINE void setArgb(float a, float r, float g, float b);
  FOG_INLINE void setAhsv(const AhsvF& ahsv) { _a = ahsv._a; _h = ahsv._h, _s = ahsv._s; _v = ahsv._v; }
  FOG_INLINE void setAhsv(float a, float h, float s, float v) { _a = a; _h = h; _s = s; _v = v; }
  FOG_INLINE void setAcmyk(const AcmykF& acmyk);
  FOG_INLINE void setAcmyk(float a, float c, float m, float y, float k);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { _a = 0.0f; _h = 0.0f; _s = 0.0f; _v = 0.0f; }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t lighten(float factor);
  FOG_INLINE err_t darken(float factor);

  // --------------------------------------------------------------------------
  // [IsOpaque / IsTransparent]
  // --------------------------------------------------------------------------

  //! @brief Get whether the alpha is close to 1.0 (fully-opaque).
  FOG_INLINE bool isOpaque() const { return Math::fgt(_a, 1.0f); }
  //! @brief Get whether the alpha is close to 0.0 (fully-transparent).
  FOG_INLINE bool isTransparent() const { return Math::flt(_a, 0.0f); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union
  {
    struct
    {
      float _a;
      float _h;
      float _s;
      float _v;
    };
    float _data[4];
  };
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::AcmykF]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
// TODO:
//! @brief Color in ACMYK format (alpha, cyan, magenta, yellow, black).
struct FOG_HIDDEN AcmykF
{
  FOG_INLINE AcmykF() : 
    _a(0.0f),
    _c(0.0f),
    _m(0.0f),
    _y(0.0f),
    _k(0.0f)
  {
  }

  explicit FOG_INLINE AcmykF(_DONT_INITIALIZE) {};

  FOG_INLINE AcmykF(float a, float c, float m, float y, float k) : 
    _a(a),
    _c(c),
    _m(m),
    _y(y),
    _k(k)
  {
  }

  FOG_INLINE AcmykF(const AcmykF& other) :
    _a(other._a),
    _c(other._c),
    _m(other._m),
    _y(other._y),
    _k(other._k)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getAlpha() const { return _a; }
  FOG_INLINE float getCyan() const { return _c; }
  FOG_INLINE float getMagenta() const { return _m; }
  FOG_INLINE float getYellow() const { return _y; }
  FOG_INLINE float getBlack() const { return _k; }

  FOG_INLINE float* getData() { return _data; }
  FOG_INLINE const float* getData() const { return _data; }

  FOG_INLINE ArgbI getArgbI() const;
  FOG_INLINE ArgbF getArgbF() const;
  FOG_INLINE AhsvF getAhsvF() const;
  FOG_INLINE AcmykF getAcmykF() const { return AcmykF(*this); }

  FOG_INLINE void setAlpha(float a) { _a = a; }
  FOG_INLINE void setCyan(float c) { _c = c; }
  FOG_INLINE void setMagenta(float m) { _m = m; }
  FOG_INLINE void setYellow(float y) { _y = y; }
  FOG_INLINE void setBlack(float k) { _k = k; }

  FOG_INLINE void setArgb(const ArgbI& argb);
  FOG_INLINE void setArgb(const ArgbF& argb);
  FOG_INLINE void setArgb(float a, float r, float g, float b);
  FOG_INLINE void setAhsv(const AhsvF& ahsv);
  FOG_INLINE void setAhsv(float a, float h, float s, float v);
  FOG_INLINE void setAcmyk(const AcmykF& acmyk) { _a = acmyk._a; _c = acmyk._c; _m = acmyk._m; _y = acmyk._y; _k = acmyk._k; }
  FOG_INLINE void setAcmyk(float a, float c, float m, float y, float k) { _a = a; _c = c; _m = m; _y = y; _k = k; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { _a = 0.0f; _c = 0.0f; _m = 0.0f; _y = 0.0f; _k = 0.0f; }

  // --------------------------------------------------------------------------
  // [IsOpaque / IsTransparent]
  // --------------------------------------------------------------------------

  //! @brief Get whether the alpha is close to 1.0 (fully-opaque).
  FOG_INLINE bool isOpaque() const { return Math::fgt(_a, 1.0f); }
  //! @brief Get whether the alpha is close to 0.0 (fully-transparent).
  FOG_INLINE bool isTransparent() const { return Math::flt(_a, 0.0f); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union
  {
    struct
    {
      float _a;
      float _c;
      float _m;
      float _y;
      float _k;
    };
    float _data[5];
  };
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::Color]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief Color value using ARGB or AHSV format.
//!
//! @c Color is universal container for storing color value. It remembers the
//! format used to store the color and it can convert color to any of supported
//! types.
//!
//! @section Converting color to grey.
//!
//! We are using this formula:
//!  0.212671 * R + 0.715160 * G + 0.072169 * B;
//! As:
//!  (13938 * R + 46868 * G + 4730 * B) / 65536
struct FOG_API Color
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Color() { reset(); }
  FOG_INLINE explicit Color(const ArgbI& argb) { setArgb(argb); }
  FOG_INLINE explicit Color(const ArgbF& argb) { setArgb(argb); }
  FOG_INLINE explicit Color(const AhsvF& ahsv) { setAhsv(ahsv); }
  FOG_INLINE explicit Color(const AcmykF& acmyk) { setAcmyk(acmyk); }
  FOG_INLINE Color(const Color& other) { setColor(other); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getColorModel() const { return _colorModel; }

  FOG_INLINE ArgbI getArgbI() const { return _argb; }
  ArgbF getArgbF() const;
  AhsvF getAhsvF() const;
  AcmykF getAcmykF() const;

  void setColorModel(uint32_t colorModel);

  void setArgb(const ArgbI& argb);
  void setArgb(const ArgbF& argb);
  void setAhsv(const AhsvF& ahsv);
  void setAcmyk(const AcmykF& acmyk);
  void setColor(const Color& other);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  err_t op(uint32_t opType, const Color& other, float factor);
  err_t lighten(float factor);
  err_t darken(float factor);

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Color& operator=(const ArgbI& argb) { setArgb(argb); return *this; }
  FOG_INLINE Color& operator=(const ArgbF& argb) { setArgb(argb); return *this; }
  FOG_INLINE Color& operator=(const AhsvF& argb) { setAhsv(argb); return *this; }
  FOG_INLINE Color& operator=(const AcmykF& argb) { setAcmyk(argb); return *this; }
  FOG_INLINE Color& operator=(const Color& other) { setColor(other); return *this; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static uint32_t _convertArgbiFromAhsvf(const float* src);
  static uint32_t _convertArgbiFromAcmykf(const float* src);

  static void _convertArgbfFromAhsvf(float* dst, const float* src);
  static void _convertArgbfFromAcmykf(float* dst, const float* src);

  static void _convertAhsvfFromArgbi(float* dst, uint32_t src);
  static void _convertAhsvfFromArgbf(float* dst, const float* src);

  static void _convertAcmykfFromArgbi(float* dst, uint32_t src);
  static void _convertAcmykfFromArgbf(float* dst, const float* src);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Color model.
  uint32_t _colorModel;
  //! @breief 32-bit ARGB entity for fast use/access.
  ArgbI _argb;

  //! @brief Shared ARGB, AHSV and ACMYK data.
  float _data[5];
};
#include <Fog/Core/Pack/PackRestore.h>























// ============================================================================
// [Fog::ArgbStop]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief ArgbI color stop.
struct FOG_HIDDEN ArgbStop
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ArgbStop()
  {
  }

  FOG_INLINE ArgbStop(float offset, ArgbI argb) :
    _offset(offset),
    _argb(argb)
  {
  }

  FOG_INLINE ArgbStop(float offset, uint32_t a, uint32_t r, uint32_t g, uint32_t b) :
    _offset(offset),
    _argb(ArgbI(a, r, g, b))
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
  FOG_INLINE ArgbI getArgbI() const { return _argb; }

  FOG_INLINE void setOffset(float offset) { _offset = offset; }
  FOG_INLINE void setArgb(const ArgbI& argb) { _argb = argb; }

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
  ArgbI _argb;
};
#include <Fog/Core/Pack/PackRestore.h>






























// ============================================================================
// [Fog::ArgbI - Implemented-Later]
// ============================================================================

FOG_INLINE ArgbF ArgbI::getArgbF() const
{
  return ArgbF(*this);
}

FOG_INLINE AhsvF ArgbI::getAhsvF() const
{
  AhsvF result(DONT_INITIALIZE);
  Color::_convertAhsvfFromArgbi(result.getData(), _packed);
  return result;
}

FOG_INLINE AcmykF ArgbI::getAcmykF() const
{
  AcmykF result(DONT_INITIALIZE);
  Color::_convertAcmykfFromArgbi(result.getData(), _packed);
  return result;
}

FOG_INLINE void ArgbI::setAhsv(const AhsvF& other)
{
  setPacked(Color::_convertArgbiFromAhsvf(other.getData()));
}

FOG_INLINE void ArgbI::setAhsv(float a, float h, float s, float v)
{
  float tmp[4] = { a, h, s, v };
  setPacked(Color::_convertArgbiFromAhsvf(tmp));
}

FOG_INLINE void ArgbI::setAcmyk(const AcmykF& other)
{
  setPacked(Color::_convertArgbiFromAcmykf(other.getData()));
}

FOG_INLINE void ArgbI::setAcmyk(float a, float c, float m, float y, float k)
{
  float tmp[5] = { a, c, m, y, k };
  setPacked(Color::_convertArgbiFromAcmykf(tmp));
}

// ============================================================================
// [Fog::ArgbF - Implemented-Later]
// ============================================================================

FOG_INLINE AhsvF ArgbF::getAhsvF() const
{
  AhsvF result(DONT_INITIALIZE);
  Color::_convertAhsvfFromArgbf(result.getData(), _data);
  return result;
}

FOG_INLINE AcmykF ArgbF::getAcmykF() const
{
  AcmykF result(DONT_INITIALIZE);
  Color::_convertAcmykfFromArgbf(result.getData(), _data);
  return result;
}

FOG_INLINE void ArgbF::setAhsv(const AhsvF& ahsv)
{
  Color::_convertArgbfFromAhsvf(_data, ahsv.getData());
}

FOG_INLINE void ArgbF::setAhsv(float a, float h, float s, float v)
{
  float tmp[4] = { a, h, s, v };
  Color::_convertArgbfFromAhsvf(_data, tmp);
}

FOG_INLINE void ArgbF::setAcmyk(const AcmykF& acmyk)
{
  Color::_convertArgbfFromAhsvf(_data, acmyk.getData());
}

FOG_INLINE void ArgbF::setAcmyk(float a, float c, float m, float y, float k)
{
  float tmp[5] = { a, c, m, y, k };
  Color::_convertArgbfFromAhsvf(_data, tmp);
}

// ============================================================================
// [Fog::AhsvF - Implemented-Later]
// ============================================================================

FOG_INLINE ArgbI AhsvF::getArgbI() const
{
  return ArgbI(Color::_convertArgbiFromAhsvf(_data));
}

FOG_INLINE ArgbF AhsvF::getArgbF() const
{
  ArgbF result(DONT_INITIALIZE);
  Color::_convertArgbfFromAhsvf(result.getData(), _data);
  return result;
}

FOG_INLINE AcmykF AhsvF::getAcmykF() const
{
  AcmykF result(DONT_INITIALIZE);
  Color::_convertArgbfFromAhsvf(result.getData(), _data);
  Color::_convertAcmykfFromArgbf(result.getData(), result.getData());
  return result;
}

FOG_INLINE void AhsvF::setArgb(const ArgbI& argb)
{
  Color::_convertAhsvfFromArgbi(_data, argb.getPacked());
}

FOG_INLINE void AhsvF::setArgb(const ArgbF& argb)
{
  Color::_convertAhsvfFromArgbf(_data, argb.getData());
}

FOG_INLINE void AhsvF::setArgb(float a, float r, float g, float b)
{
  float tmp[4] = { a, r, g, b };
  Color::_convertAhsvfFromArgbf(_data, tmp);
}

FOG_INLINE void AhsvF::setAcmyk(const AcmykF& acmyk)
{
  Color::_convertArgbfFromAcmykf(_data, acmyk.getData());
  Color::_convertAhsvfFromArgbf(_data, _data);
}

FOG_INLINE void AhsvF::setAcmyk(float a, float c, float m, float y, float k)
{
  float tmp[5] = { a, c, m, y, k };
  Color::_convertArgbfFromAcmykf(_data, tmp);
  Color::_convertAhsvfFromArgbf(_data, _data);
}

// ============================================================================
// [Fog::AcmykF - Implemented-Later]
// ============================================================================

FOG_INLINE ArgbI AcmykF::getArgbI() const
{
  return ArgbI(Color::_convertArgbiFromAcmykf(_data));
}

FOG_INLINE ArgbF AcmykF::getArgbF() const
{
  ArgbF result(DONT_INITIALIZE);
  Color::_convertArgbfFromAcmykf(result.getData(), _data);
  return result;
}

FOG_INLINE AhsvF AcmykF::getAhsvF() const
{
  AhsvF result(DONT_INITIALIZE);
  Color::_convertArgbfFromAcmykf(result.getData(), _data);
  Color::_convertAhsvfFromArgbf(result.getData(), result.getData());
  return result;
}

FOG_INLINE void AcmykF::setArgb(const ArgbI& argb)
{
  Color::_convertAcmykfFromArgbi(_data, argb.getPacked());
}

FOG_INLINE void AcmykF::setArgb(const ArgbF& argb)
{
  Color::_convertAcmykfFromArgbf(_data, argb.getData());
}

FOG_INLINE void AcmykF::setArgb(float a, float r, float g, float b)
{
  float tmp[4] = { a, r, g, b };
  Color::_convertAcmykfFromArgbf(_data, tmp);
}

FOG_INLINE void AcmykF::setAhsv(const AhsvF& ahsv)
{
  Color::_convertArgbfFromAhsvf(_data, ahsv.getData());
  Color::_convertAcmykfFromArgbf(_data, _data);
}

FOG_INLINE void AcmykF::setAhsv(float a, float h, float s, float v)
{
  float tmp[4] = { a, h, s, v };
  Color::_convertArgbfFromAhsvf(_data, tmp);
  Color::_convertAcmykfFromArgbf(_data, _data);
}

//! @}

} // Fog namespace

FOG_DECLARE_TYPEINFO(Fog::AcmykF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::AhsvF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ArgbF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ArgbI, Fog::TYPEINFO_PRIMITIVE)

FOG_DECLARE_TYPEINFO(Fog::Color, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ColorStop, Fog::TYPEINFO_PRIMITIVE)

FOG_DECLARE_TYPEINFO(Fog::ArgbStop, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_GRAPHICS_COLOR_H
