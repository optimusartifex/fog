// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_ARGB_H
#define _FOG_G2D_SOURCE_ARGB_H

// [Dependencies]
#include <Fog/Core/Face/Face_C.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Ops.h>
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/G2d/Global/Api.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Source/ColorBase.h>
#include <Fog/G2d/Source/ColorUtil.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct AcmykF;
struct AhsvF;
struct AhslF;
struct Argb32;
struct Argb64;
struct ArgbF;

// ============================================================================
// [Fog::Argb32]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief ARGB32 color (8-bit per component).
struct FOG_NO_EXPORT Argb32 : public ArgbBase32
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create the @c Argb32 instance, initializing all components to zero.
  FOG_INLINE Argb32()
  {
    reset();
  }

  //! @brief Create the @a Argb32 instance, uninitialized.
  explicit FOG_INLINE Argb32(_Uninitialized)
  {
  }

  //! @brief Create the @c Argb32 instance, initializing it to packed @a value
  //! data.
  FOG_INLINE Argb32(uint32_t packed32)
  {
    setPacked32(packed32);
  }

  //! @brief Create the @c Argb32 instance, initializing individual components
  //! to @a a, @a r, @a g and @a b.
  FOG_INLINE Argb32(uint32_t a8, uint32_t r8, uint32_t g8, uint32_t b8)
  {
    setArgb32(a8, r8, g8, b8);
  }

  //! @brief Create the @c Argb32 instance, copying components from @a argb32.
  FOG_INLINE Argb32(const Argb32& argb32)
  {
    setArgb32(argb32);
  }

  //! @brief Create the @c Argb32 instance, copying components from @a argb32.
  explicit FOG_INLINE Argb32(const ArgbBase32& argb32)
  {
    setArgb32(argb32);
  }

  //! @brief Create the @c Argb32 instance, copying components from @a argb64.
  explicit FOG_INLINE Argb32(const ArgbBase64& argb64)
  {
    setArgb64(argb64);
  }

  //! @brief Create the @c Argb32 instance, copying components from @a argb64.
  explicit FOG_INLINE Argb32(const ArgbBaseF& argbf)
  {
    setArgbF(argbf);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get alpha value of ARGB quad.
  FOG_INLINE uint32_t getAlpha() const { return a; }
  //! @brief Get red value of ARGB quad.
  FOG_INLINE uint32_t getRed() const { return r; }
  //! @brief Get green value of ARGB quad.
  FOG_INLINE uint32_t getGreen() const { return g; }
  //! @brief Get blue value of ARGB quad.
  FOG_INLINE uint32_t getBlue() const { return b; }
  //! @brief Get grey value (converting RGB components to grey, ignoring alpha).
  FOG_INLINE uint32_t getGrey() const { return getGrey(u32); }

  //! @brief Set alpha value of ARGB quad.
  FOG_INLINE void setAlpha(uint32_t a8) { a = a8; }
  //! @brief Set red value of ARGB quad.
  FOG_INLINE void setRed(uint32_t r8) { r = r8; }
  //! @brief Set green value of ARGB quad.
  FOG_INLINE void setGreen(uint32_t g8) { g = g8; }
  //! @brief Set blue value of ARGB quad.
  FOG_INLINE void setBlue(uint32_t b8) { b = b8; }
  //! @brief Set red, green and blue components to @a grey.
  FOG_INLINE void setGrey(uint32_t grey8) { r = grey8; g = grey8; b = grey8; }

  //! @brief Get the packed value.
  FOG_INLINE uint32_t getPacked32() const
  {
    return u32;
  }

  //! @brief Get the packed value.
  FOG_INLINE uint64_t getPacked64() const
  {
    ArgbBase64 argb64;
    ColorUtil::argb64From32(argb64, *this);
    return argb64.u64;
  }

  //! @brief Set all values to @a packed32.
  FOG_INLINE void setPacked32(uint32_t packed32)
  {
    u32 = packed32;
  }

  //! @brief Set all values to @a packed64.
  FOG_INLINE void setPacked64(uint64_t packed64)
  {
    ArgbBase64 argb64;
    argb64.u64 = packed64;
    ColorUtil::argb32From64(*this, argb64);
  }

  // --------------------------------------------------------------------------
  // [Conversion]
  // --------------------------------------------------------------------------

  //! @brief Convert color to @c Argb32 (convenience).
  FOG_INLINE Argb32 getArgb32() const { return Argb32(u32); }
  //! @brief Convert color to @c Argb64.
  FOG_INLINE Argb64 getArgb64() const;
  //! @brief Convert color to @c ArgbF.
  FOG_INLINE ArgbF getArgbF() const;
  //! @brief Convert color to @c AhsvF.
  FOG_INLINE AhsvF getAhsvF() const;
  //! @brief Convert color to @c AhslF.
  FOG_INLINE AhslF getAhslF() const;
  //! @brief Convert color to @c AcmykF.
  FOG_INLINE AcmykF getAcmykF() const;

  //! @brief Set all values to @a argb32.
  FOG_INLINE void setArgb32(const ArgbBase32& argb32)
  {
    u32 = argb32.u32;
  }

  //! @brief Set all values to @a a, @a r, @a g, @a b.
  FOG_INLINE void setArgb32(uint32_t a8, uint32_t r8, uint32_t g8, uint32_t b8)
  {
    u32 = pack(a8, r8, g8, b8);
  }

  //! @brief Set all values to @a argb64.
  FOG_INLINE void setArgb64(const ArgbBase64& argb64)
  {
    Face::p64ARGB32FromARGB64(p32, argb64.p64);
  }

  //! @brief Set all values to @a a, @a r, @a g, @a b.
  FOG_INLINE void setArgb64(uint32_t a16, uint32_t r16, uint32_t g16, uint32_t b16)
  {
    uint32_t t0 = ((a16 << 16) | (g16     )) & 0xFF00FF00;
    uint32_t t1 = ((r16 <<  8) | (b16 >> 8)) & 0x00FF00FF;

    u32 = t0 + t1;
  }

  //! @brief Set all values to @a argb (converting color entities to 8-bit).
  FOG_INLINE void setArgbF(const ArgbBaseF& argb)
  {
    ColorUtil::argb32FromF(*this, argb.data);
  }

  //! @brief Set all values to @a a, @a r, @a g, and @b.
  FOG_INLINE void setArgbF(float a, float r, float g, float b)
  {
    float argbf[4] = { a, r, g, b };
    ColorUtil::argb32FromF(*this, argbf);
  }

  //! @brief Set all values to @a ahsv (converting from AHSV color format).
  FOG_INLINE void setAhsvF(const AhsvBaseF& ahsvf)
  {
    _g2d.color.convert[_COLOR_MODEL_ARGB32][COLOR_MODEL_AHSV](this, &ahsvf);
  }

  //! @brief Set all values to @a a, @a h, @a s, and @v (converting from AHSV color format).
  FOG_INLINE void setAhsvF(float a, float h, float s, float v)
  {
    float ahsvf[4] = { a, h, s, v };
    _g2d.color.convert[_COLOR_MODEL_ARGB32][COLOR_MODEL_AHSV](this, ahsvf);
  }

  //! @brief Set all values to @a ahsl (converting from AHSL color format).
  FOG_INLINE void setAhslF(const AhslBaseF& ahslf)
  {
    _g2d.color.convert[_COLOR_MODEL_ARGB32][COLOR_MODEL_AHSL](this, &ahslf);
  }

  //! @brief Set all values to @a a, @a h, @a s, and @l (converting from AHSL color format).
  FOG_INLINE void setAhslF(float a, float h, float s, float l)
  {
    float ahslf[4] = { a, h, s, l };
    _g2d.color.convert[_COLOR_MODEL_ARGB32][COLOR_MODEL_AHSL](this, ahslf);
  }

  //! @brief Set all values to @a acmyk (converting from the ACMYK color format).
  FOG_INLINE void setAcmykF(const AcmykBaseF& acmykf)
  {
    _g2d.color.convert[_COLOR_MODEL_ARGB32][COLOR_MODEL_ACMYK](this, &acmykf);
  }

  //! @brief Set all values to @a a, @a c, @a m, @a y, and @a k (converting from
  //! the ACMYK color format).
  FOG_INLINE void setAcmykF(float a, float c, float m, float y, float k)
  {
    float acmykf[5] = { a, c, m, y, k };
    _g2d.color.convert[_COLOR_MODEL_ARGB32][COLOR_MODEL_ACMYK](this, acmykf);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    u32 = 0x00000000U;
  }

  // --------------------------------------------------------------------------
  // [Swap]
  // --------------------------------------------------------------------------

  //! @brief Byte-swap the packed color data.
  FOG_INLINE void bswap()
  {
    u32 = Memory::bswap32(u32);
  }

  // --------------------------------------------------------------------------
  // [IsOpaque / IsTransparent]
  // --------------------------------------------------------------------------

  //! @brief Get whether the alpha is fully-opaque (255 in decimal).
  FOG_INLINE bool isOpaque() const
  {
    return (u32 >= 0xFF000000);
  }

  //! @brief Get whether the alpha is fully-transparent (0 in decimal).
  FOG_INLINE bool isTransparent() const
  {
    return (u32 <= 0x00FFFFFF);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Cast to packed 32-bit integer.
  FOG_INLINE operator uint32_t() const { return u32; }

  FOG_INLINE Argb32& operator=(const ArgbBase32& argb32) { setArgb32(argb32); return *this; }
  FOG_INLINE Argb32& operator=(const ArgbBase64& argb64) { setArgb64(argb64); return *this; }
  FOG_INLINE Argb32& operator=(const ArgbBaseF& argbf) { setArgbF(argbf); return *this; }
  FOG_INLINE Argb32& operator=(const AhsvBaseF& ahsvf) { setAhsvF(ahsvf); return *this; }
  FOG_INLINE Argb32& operator=(const AhslBaseF& ahslf) { setAhslF(ahslf); return *this; }
  FOG_INLINE Argb32& operator=(const AcmykBaseF& acmykf) { setAcmykF(acmykf); return *this; }

  FOG_INLINE Argb32& operator=(uint32_t packed32) { u32 = packed32; return *this; }
  FOG_INLINE Argb32& operator|=(uint32_t packed32) { u32 |= packed32; return *this; }
  FOG_INLINE Argb32& operator&=(uint32_t packed32) { u32 &= packed32; return *this; }
  FOG_INLINE Argb32& operator^=(uint32_t packed32) { u32 ^= packed32; return *this; }

  // --------------------------------------------------------------------------
  // [Statics - Access]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t getComponent(uint32_t argb32, uint32_t position)
  {
    argb32 >>= position;

    // Should be done at compile time since we are expecting 'position' to be
    // a constant.
    if (position < 24) argb32 &= 0xFFU;

    return argb32;
  }

  static FOG_INLINE uint32_t getAlpha(uint32_t argb32) { return getComponent(argb32, ARGB32_ASHIFT); }
  static FOG_INLINE uint32_t getRed  (uint32_t argb32) { return getComponent(argb32, ARGB32_RSHIFT); }
  static FOG_INLINE uint32_t getGreen(uint32_t argb32) { return getComponent(argb32, ARGB32_GSHIFT); }
  static FOG_INLINE uint32_t getBlue (uint32_t argb32) { return getComponent(argb32, ARGB32_BSHIFT); }

  static FOG_INLINE uint32_t getGrey(uint32_t argb32)
  {
    uint32_t grey = ((argb32 >> 16) & 0xFF) * 13938 +
                    ((argb32 >>  8) & 0xFF) * 46868 +
                    ((argb32      ) & 0xFF) *  4730 ;

    return grey >> 16;
  }

  // --------------------------------------------------------------------------
  // [Statics - Pack]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t pack(uint32_t a8, uint32_t r8, uint32_t g8, uint32_t b8)
  {
    return (a8 << ARGB32_ASHIFT) | (r8 << ARGB32_RSHIFT) |
           (g8 << ARGB32_GSHIFT) | (b8 << ARGB32_BSHIFT) ;
  }

  // --------------------------------------------------------------------------
  // [Statics - BSwap]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t bswap(uint32_t argb32)
  {
    return Memory::bswap32(argb32);
  }

  // --------------------------------------------------------------------------
  // [Statics - Transparency]
  // --------------------------------------------------------------------------

  //! @brief Get whether the alpha is fully-opaque (255 in decimal).
  //!
  //! Advantage of using this function is that it should expand only to 2 assembler
  //! instructions, using the whole 32-bit packed @a argb value.
  static FOG_INLINE bool isOpaque(uint32_t argb32)
  {
    return (argb32 >= 0xFF000000);
  }

  //! @brief Get whether the alpha is fully-transparent (0 in decimal).
  //!
  //! Advantage of using this function is that it should expand only to 2 assembler
  //! instructions, using the whole 32-bit packed @a argb value.
  static FOG_INLINE bool isTransparent(uint32_t argb32)
  {
    return (argb32 <= 0x00FFFFFF);
  }

  // --------------------------------------------------------------------------
  // [Statics - Combine]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t chalf(uint32_t x32, uint32_t y32) { return ColorUtil::chalf32(x32, y32); }
  static FOG_INLINE uint32_t cadd(uint32_t x32, uint32_t y32) { return ColorUtil::cadd32(x32, y32); }
  static FOG_INLINE uint32_t csub(uint32_t x32, uint32_t y32) { return ColorUtil::csub32(x32, y32); }
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::Argb64]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief ARGB64 color (16-bit per component).
struct FOG_NO_EXPORT Argb64 : public ArgbBase64
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create the @c Argb32 instance, initializing all components to zero.
  FOG_INLINE Argb64()
  {
    reset();
  }

  //! @brief Create the @a Argb64 instance, uninitialized.
  explicit FOG_INLINE Argb64(_Uninitialized)
  {
  }

  //! @brief Create the @c Argb64 instance, initializing data to the @a packed64
  //! value.
  FOG_INLINE Argb64(uint64_t packed64)
  {
    setPacked64(packed64);
  }

  //! @brief Create the @c Argb64 instance, initializing individual components
  //! to @a a, @a r, @a g and @a b.
  FOG_INLINE Argb64(uint32_t a16, uint32_t r16, uint32_t g16, uint32_t b16)
  {
    a = a16;
    r = r16;
    g = g16;
    b = b16;
  }

  //! @brief Create the @c Argb64 instance, copying components from @a argb64.
  FOG_INLINE Argb64(const ArgbBase64& argb64)
  {
    setArgb64(argb64);
  }

  //! @brief Create the @c Argb64 instance, copying components from @a argb32.
  explicit FOG_INLINE Argb64(const ArgbBase32& argb32)
  {
    setArgb32(argb32);
  }

  //! @brief Create the @c Argb64 instance, copying components from @a argb32.
  explicit FOG_INLINE Argb64(const ArgbBaseF& argbf)
  {
    setArgbF(argbf);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get alpha value of ARGB quad.
  FOG_INLINE uint32_t getAlpha() const { return a; }
  //! @brief Get red value of ARGB quad.
  FOG_INLINE uint32_t getRed() const { return r; }
  //! @brief Get green value of ARGB quad.
  FOG_INLINE uint32_t getGreen() const { return g; }
  //! @brief Get blue value of ARGB quad.
  FOG_INLINE uint32_t getBlue() const { return b; }
  //! @brief Get grey value (converting RGB components to grey, ignoring alpha).
  FOG_INLINE uint32_t getGrey() const { return getGrey(u64); }

  //! @brief Set the alpha value to @a a.
  FOG_INLINE void setAlpha(uint32_t a16) { a = (uint16_t)a16; }
  //! @brief Set the red value to @a r.
  FOG_INLINE void setRed(uint32_t r16) { r = (uint16_t)r16; }
  //! @brief Set the green value to @a g.
  FOG_INLINE void setGreen(uint32_t g16) { g = (uint16_t)g16; }
  //! @brief Set the blue value to @a b.
  FOG_INLINE void setBlue(uint32_t b16) { b = (uint16_t)b16; }
  //! @brief Set the red, green and blue components to @a grey.
  FOG_INLINE void setGrey(uint32_t grey) { r = (uint16_t)grey; g = (uint16_t)grey; b = (uint16_t)grey; }

  //! @brief Get the packed 32-bit value.
  FOG_INLINE uint32_t getPacked32() const
  {
    ArgbBase32 argb32;
    ColorUtil::argb32From64(argb32, *this);
    return argb32.u32;
  }

  //! @brief Get the packed 64-bit value.
  FOG_INLINE uint64_t getPacked64() const
  {
    return u64;
  }

  //! @brief Set all values to @a packed32.
  FOG_INLINE void setPacked32(uint32_t packed32)
  {
    ArgbBase32 argb32;
    argb32.u32 = packed32;
    ColorUtil::argb64From32(*this, argb32);
  }

  //! @brief Set all values to @a packed64.
  FOG_INLINE void setPacked64(uint64_t packed64)
  {
    u64 = packed64;
  }

  // --------------------------------------------------------------------------
  // [Conversion]
  // --------------------------------------------------------------------------

  //! @brief Convert color to @c Argb32.
  FOG_INLINE Argb32 getArgb32() const
  {
    Argb32 c(UNINITIALIZED);
    ColorUtil::argb32From64(c, *this);
    return c;
  }

  //! @brief Convert color to @c Argb64 (convenience).
  FOG_INLINE Argb64 getArgb64() const
  {
    return Argb64(u64);
  }

  //! @brief Convert color to @c ArgbF.
  FOG_INLINE ArgbF getArgbF() const;
  //! @brief Convert color to @c AhsvF.
  FOG_INLINE AhsvF getAhsvF() const;
  //! @brief Convert color to @c AhslF.
  FOG_INLINE AhslF getAhslF() const;
  //! @brief Convert color to @c AcmykF.
  FOG_INLINE AcmykF getAcmykF() const;

  //! @brief Set all values to @a argb32.
  FOG_INLINE void setArgb32(const ArgbBase32& argb32)
  {
    ColorUtil::argb64From32(*this, argb32);
  }

  //! @brief Set all values to @a a, @a r, @a g, @a b.
  FOG_INLINE void setArgb32(uint32_t a8, uint32_t r8, uint32_t g8, uint32_t b8)
  {
#if FOG_ARCH_BITS >= 64
    u64 = pack64(a8, r8, g8, b8);
    u64 *= FOG_UINT64_C(0x0101);
#else
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    u32[0] = (b8 + (g8 << 16)) * 0x0101U;
    u32[1] = (r8 + (a8 << 16)) * 0x0101U;
#else
    u32[0] = (r8 + (a8 << 16)) * 0x0101U;
    u32[1] = (b8 + (g8 << 16)) * 0x0101U;
#endif // FOG_BYTE_ORDER
#endif // FOG_ARCH_BITS
  }

  //! @brief Set all values to @a other.
  FOG_INLINE void setArgb64(const ArgbBase64& argb64)
  {
    u64 = argb64.u64;
  }

  //! @brief Set all values to @a a, @a r, @a g, @a b.
  FOG_INLINE void setArgb64(uint32_t a16, uint32_t r16, uint32_t g16, uint32_t b16)
  {
    u64 = pack64(a16, r16, g16, b16);
  }

  //! @brief Set all values to @a other (converting color entities to 8-bit).
  FOG_INLINE void setArgbF(const ArgbBaseF& argbf)
  {
    ColorUtil::argb64FromF(*this, argbf.data);
  }

  //! @brief Set all values to @a other (converting from AHSV color format).
  FOG_INLINE void setArgbF(float af, float rf, float gf, float bf)
  {
    a = Face::f32CvtU16FromFX(af);
    r = Face::f32CvtU16FromFX(rf);
    g = Face::f32CvtU16FromFX(gf);
    b = Face::f32CvtU16FromFX(bf);
  }

  FOG_INLINE void setAhsvF(const AhsvBaseF& ahsvf)
  {
    _g2d.color.convert[_COLOR_MODEL_ARGB64][COLOR_MODEL_AHSV](this, &ahsvf);
  }

  FOG_INLINE void setAhsvF(float af, float hf, float sf, float vf)
  {
    float ahsvf[4] = { af, hf, sf, vf };
    _g2d.color.convert[_COLOR_MODEL_ARGB64][COLOR_MODEL_AHSV](this, ahsvf);
  }

  FOG_INLINE void setAhslF(const AhslBaseF& ahslf)
  {
    _g2d.color.convert[_COLOR_MODEL_ARGB64][COLOR_MODEL_AHSV](this, &ahslf);
  }

  FOG_INLINE void setAhslF(float af, float hf, float sf, float lf)
  {
    float ahslf[4] = { af, hf, sf, lf };
    _g2d.color.convert[_COLOR_MODEL_ARGB64][COLOR_MODEL_AHSL](this, ahslf);
  }

  FOG_INLINE void setAcmykF(const AcmykBaseF& acmykf)
  {
    _g2d.color.convert[_COLOR_MODEL_ARGB64][COLOR_MODEL_ACMYK](this, &acmykf);
  }

  FOG_INLINE void setAcmykF(float af, float cf, float mf, float yf, float kf)
  {
    float acmykf[5] = { af, cf, mf, yf, kf };
    _g2d.color.convert[_COLOR_MODEL_ARGB64][COLOR_MODEL_ACMYK](this, acmykf);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    u64 = FOG_UINT64_C(0);
  }

  // --------------------------------------------------------------------------
  // [BSwap]
  // --------------------------------------------------------------------------

  //! @brief Byte-swap the packed color data.
  FOG_INLINE void bswap()
  {
#if FOG_ARCH_BITS >= 64
    u64 = Memory::bswap64(u64);
#else
    uint32_t t0 = u32[0];
    uint32_t t1 = u32[1];

    u32[0] = Memory::bswap32(t1);
    u32[1] = Memory::bswap32(t0);
#endif
  }

  // --------------------------------------------------------------------------
  // [IsOpaque / IsTransparent]
  // --------------------------------------------------------------------------

  //! @brief Get whether the alpha is fully-opaque (255 in decimal).
  FOG_INLINE bool isOpaque() const
  {
#if FOG_ARCH_BITS >= 64
    return (u64 >= FOG_UINT64_C(0xFFFF000000000000));
#else
    return a == 0xFFFF;
#endif
  }

  //! @brief Get whether the alpha is fully-transparent (0 in decimal).
  FOG_INLINE bool isTransparent() const
  {
#if FOG_ARCH_BITS >= 64
    return (u64 <= FOG_UINT64_C(0x0000FFFFFFFFFFFF));
#else
    return a == 0x0000;
#endif
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Cast to packed 64-bit integer.
  FOG_INLINE operator uint64_t() const { return u64; }

  FOG_INLINE Argb64& operator=(const ArgbBase32& argb32) { setArgb32(argb32); return *this; }
  FOG_INLINE Argb64& operator=(const ArgbBase64& argb64) { setArgb64(argb64); return *this; }
  FOG_INLINE Argb64& operator=(const ArgbBaseF& argbf) { setArgbF(argbf); return *this; }
  FOG_INLINE Argb64& operator=(const AhsvBaseF& ahsvf) { setAhsvF(ahsvf); return *this; }
  FOG_INLINE Argb64& operator=(const AhslBaseF& ahslf) { setAhslF(ahslf); return *this; }
  FOG_INLINE Argb64& operator=(const AcmykBaseF& acmykf) { setAcmykF(acmykf); return *this; }

  FOG_INLINE Argb64& operator=(uint64_t packed64) { u64 = packed64; return *this; }
  FOG_INLINE Argb64& operator|=(uint64_t packed64) { u64 |= packed64; return *this; }
  FOG_INLINE Argb64& operator&=(uint64_t packed64) { u64 &= packed64; return *this; }
  FOG_INLINE Argb64& operator^=(uint64_t packed64) { u64 ^= packed64; return *this; }

  // --------------------------------------------------------------------------
  // [Statics - Access]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t getComponent(uint64_t argb, uint32_t position)
  {
    argb >>= position;

    // Should be done at compile time since we are expecting 'position' to be
    // a constant.
    if (position < 48) argb &= FOG_UINT64_C(0xFFFF);

    return (uint32_t)argb;
  }

  static FOG_INLINE uint32_t getAlpha(uint64_t argb64) { return getComponent(argb64, ARGB64_ASHIFT); }
  static FOG_INLINE uint32_t getRed  (uint64_t argb64) { return getComponent(argb64, ARGB64_RSHIFT); }
  static FOG_INLINE uint32_t getGreen(uint64_t argb64) { return getComponent(argb64, ARGB64_GSHIFT); }
  static FOG_INLINE uint32_t getBlue (uint64_t argb64) { return getComponent(argb64, ARGB64_BSHIFT); }

  static FOG_INLINE uint32_t getGrey(uint64_t argb64)
  {
    uint32_t r = ((uint32_t)(argb64 >> 32) & 0xFFFFU) * 13938;
    uint32_t g = ((uint32_t)(argb64 >> 16) & 0xFFFFU) * 46868;
    uint32_t b = ((uint32_t)(argb64      ) & 0xFFFFU) *  4730;

    // We can't sum three R.G.B components using 32-bit integer, but we can
    // sum two, improving performance when running in 32-bit mode.
    return (uint32_t)( ((uint64_t)(r + g) + (uint64_t)b) >> 16 );
  }

  // --------------------------------------------------------------------------
  // [Statics - Pack]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint64_t pack64(uint32_t a16, uint32_t r16, uint32_t g16, uint32_t b16)
  {
    return ((uint64_t)a16 << ARGB64_ASHIFT) | ((uint64_t)r16 << ARGB64_RSHIFT) |
           ((uint64_t)g16 << ARGB64_GSHIFT) | ((uint64_t)b16 << ARGB64_BSHIFT) ;
  }

  // --------------------------------------------------------------------------
  // [Statics - Transparency]
  // --------------------------------------------------------------------------

  //! @brief Get whether the alpha is fully-opaque (255 in decimal).
  //!
  //! Advantage of using this function is that it should expand only to 2 assembler
  //! instructions, using the whole 32-bit packed @a argb value.
  static FOG_INLINE bool isOpaque(uint64_t argb64)
  {
    return (argb64 >= FOG_UINT64_C(0xFFFF000000000000));
  }

  //! @brief Get whether the alpha is fully-transparent (0 in decimal).
  //!
  //! Advantage of using this function is that it should expand only to 2 assembler
  //! instructions, using the whole 32-bit packed @a argb value.
  static FOG_INLINE bool isTransparent(uint64_t argb64)
  {
    return (argb64 <= FOG_UINT64_C(0x0000FFFFFFFFFFFF));
  }

  // --------------------------------------------------------------------------
  // [Statics - Combine]
  // --------------------------------------------------------------------------

  // TODO: Do something with these methods...

  static FOG_INLINE uint64_t half(uint64_t x64, uint64_t y64)
  {
    return ColorUtil::chalf64(x64, y64);
  }
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::ArgbF]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief ARGBF color (32-bit float per component).
struct FOG_NO_EXPORT ArgbF : public ArgbBaseF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create the @c ArgbF instance, initializing all components to zero.
  FOG_INLINE ArgbF()
  {
    reset();
  }

  //! @brief Create the @c ArgbF instance, uninitialized.
  explicit FOG_INLINE ArgbF(_Uninitialized)
  {
  }

  //! @brief Create the @c Argb32 instance, copying components from @a argbf.
  FOG_INLINE ArgbF(const ArgbF& argbf)
  {
    setArgbF(argbf);
  }

  //! @brief Create the @c ArgbF instance, initializing components to @a a,
  //! @a r, @a g, and @a b.
  FOG_INLINE ArgbF(float af, float rf, float gf, float bf)
  {
    setArgbF(af, rf, gf, bf);
  }

  //! @brief Create the @c ArgbF instance, initializing components from @a argb32.
  explicit FOG_INLINE ArgbF(const ArgbBase32& argb32)
  {
    setArgb32(argb32);
  }

  //! @brief Create the @c ArgbF instance, initializing components from @a argb64.
  explicit FOG_INLINE ArgbF(const ArgbBase64& argb64)
  {
    setArgb64(argb64);
  }

  explicit FOG_INLINE ArgbF(const AhsvBaseF& ahsvf)
  {
    setAhsvF(ahsvf);
  }

  explicit FOG_INLINE ArgbF(const AcmykBaseF& acmykf)
  {
    setAcmykF(acmykf);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get alpha value of ARGB quad.
  FOG_INLINE float getAlpha() const { return a; }
  //! @brief Get red value of ARGB quad.
  FOG_INLINE float getRed() const { return r; }
  //! @brief Get green value of ARGB quad.
  FOG_INLINE float getGreen() const { return g; }
  //! @brief Get blue value of ARGB quad.
  FOG_INLINE float getBlue() const { return b; }

  //! @brief Set alpha value of ARGB quad.
  FOG_INLINE void setAlpha(float af) { a = af; }
  //! @brief Set red value of ARGB quad.
  FOG_INLINE void setRed(float rf) { r = rf; }
  //! @brief Set green value of ARGB quad.
  FOG_INLINE void setGreen(float gf) { g = gf; }
  //! @brief Set blue value of ARGB quad.
  FOG_INLINE void setBlue(float bf) { b = bf; }

  //! @brief Get grey value (converting RGB components to grey).
  FOG_INLINE float getGrey() const
  {
    return r * 0.212671f + g * 0.715160f + b * 0.072169f;
  }

  //! @brief Set red, green and blue components to @a grey.
  FOG_INLINE void setGrey(float greyf)
  {
    r = greyf;
    g = greyf;
    b = greyf;
  }

  FOG_INLINE float* getData() { return data; }
  FOG_INLINE const float* getData() const { return data; }

  // --------------------------------------------------------------------------
  // [Conversion]
  // --------------------------------------------------------------------------

  //! @brief Convert color to @c Argb32.
  FOG_INLINE Argb32 getArgb32() const
  {
    Argb32 argb32(UNINITIALIZED);
    ColorUtil::argb32FromF(argb32, data);
    return argb32;
  }

  //! @brief Convert color to @c Argb64.
  FOG_INLINE Argb64 getArgb64() const
  {
    Argb64 argb64(UNINITIALIZED);
    ColorUtil::argb64FromF(argb64, data);
    return argb64;
  }

  //! @brief Convert color to @c ArgbF (convenience).
  FOG_INLINE ArgbF getArgbF() const
  {
    return ArgbF(*this);
  }

  //! @brief Convert color to @c AhsvF.
  FOG_INLINE AhsvF getAhsvF() const;
  //! @brief Convert color to @c AhslF.
  FOG_INLINE AhslF getAhslF() const;
  //! @brief Convert color to @c AcmykF.
  FOG_INLINE AcmykF getAcmykF() const;

  FOG_INLINE void setArgb32(const ArgbBase32& argb32)
  {
    ColorUtil::argbfFromArgb32(data, argb32);
  }

  FOG_INLINE void setArgb64(const ArgbBase64& argb64)
  {
    ColorUtil::argbfFromArgb64(data, argb64);
  }

  FOG_INLINE void setArgbF(const ArgbBaseF& argbf)
  {
    a = argbf.a;
    r = argbf.r;
    g = argbf.g;
    b = argbf.b;
  }

  FOG_INLINE void setArgbF(float af, float rf, float gf, float bf)
  {
    a = af;
    r = rf;
    g = gf;
    b = bf;
  }

  FOG_INLINE void setAhsvF(const AhsvBaseF& ahsvf)
  {
    _g2d.color.convert[COLOR_MODEL_ARGB][COLOR_MODEL_AHSV](this, &ahsvf);
  }

  FOG_INLINE void setAhsvF(float af, float hf, float sf, float vf)
  {
    float ahsvf[4] = { af, hf, sf, vf };
    _g2d.color.convert[COLOR_MODEL_ARGB][COLOR_MODEL_AHSV](this, ahsvf);
  }

  FOG_INLINE void setAhslF(const AhslBaseF& ahslf)
  {
    _g2d.color.convert[COLOR_MODEL_ARGB][COLOR_MODEL_AHSV](this, &ahslf);
  }

  FOG_INLINE void setAhslF(float af, float hf, float sf, float lf)
  {
    float ahslf[4] = { af, hf, sf, lf };
    _g2d.color.convert[COLOR_MODEL_ARGB][COLOR_MODEL_AHSL](this, ahslf);
  }

  FOG_INLINE void setAcmykF(const AcmykBaseF& acmykf)
  {
    _g2d.color.convert[COLOR_MODEL_ARGB][COLOR_MODEL_ACMYK](this, &acmykf);
  }

  FOG_INLINE void setAcmykF(float af, float cf, float mf, float yf, float kf)
  {
    float acmykf[5] = { af, cf, mf, yf, kf };
    _g2d.color.convert[COLOR_MODEL_ARGB][COLOR_MODEL_ACMYK](this, acmykf);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    Memory::zero_t<ArgbF>(this);
  }

  // --------------------------------------------------------------------------
  // [IsOpaque / IsTransparent]
  // --------------------------------------------------------------------------

  //! @brief Get whether the alpha is close to 1.0 (fully-opaque).
  FOG_INLINE bool isOpaque() const
  {
    return Math::isFuzzyGreaterEq(a, 1.0f);
  }

  //! @brief Get whether the alpha is close to 0.0 (fully-transparent).
  FOG_INLINE bool isTransparent() const
  {
    return Math::isFuzzyLowerEq(a, 0.0f);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ArgbF& operator=(const ArgbBase32& argb32) { setArgb32(argb32); return *this; }
  FOG_INLINE ArgbF& operator=(const ArgbBase64& argb64) { setArgb64(argb64); return *this; }
  FOG_INLINE ArgbF& operator=(const ArgbBaseF& argbf) { setArgbF(argbf); return *this; }
  FOG_INLINE ArgbF& operator=(const AhsvBaseF& ahsvf) { setAhsvF(ahsvf); return *this; }
  FOG_INLINE ArgbF& operator=(const AhslBaseF& ahslf) { setAhslF(ahslf); return *this; }
  FOG_INLINE ArgbF& operator=(const AcmykBaseF& acmyk) { setAcmykF(acmyk); return *this; }
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE Argb64 Argb32::getArgb64() const
{
  Argb64 argb64(UNINITIALIZED);
  ColorUtil::argb64From32(argb64, *this);
  return argb64;
}

FOG_INLINE ArgbF Argb32::getArgbF() const
{
  ArgbF argbf(UNINITIALIZED);
  ColorUtil::argbfFromArgb32(argbf.data, *this);
  return argbf;
}

FOG_INLINE ArgbF Argb64::getArgbF() const
{
  ArgbF argbf(UNINITIALIZED);
  ColorUtil::argbfFromArgb64(argbf.data, *this);
  return argbf;
}

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::Argb32, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::Argb64, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::ArgbF, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE(Fog::ArgbF, Math::feqv(a.data, b.data, 4))

// [Guard]
#endif // _FOG_G2D_SOURCE_ARGB_H
