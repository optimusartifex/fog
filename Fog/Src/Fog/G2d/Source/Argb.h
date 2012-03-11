// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_ARGB_H
#define _FOG_G2D_SOURCE_ARGB_H

// [Dependencies]
#include <Fog/Core/Acc/AccC.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Source/ColorBase.h>
#include <Fog/G2d/Source/ColorUtil.h>
#include <Fog/G2d/Acc/AccC.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

// ============================================================================
// [Fog::Argb32]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
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
  FOG_INLINE void setAlpha(uint32_t a8) { a = static_cast<uint8_t>(a8); }
  //! @brief Set red value of ARGB quad.
  FOG_INLINE void setRed(uint32_t r8) { r = static_cast<uint8_t>(r8); }
  //! @brief Set green value of ARGB quad.
  FOG_INLINE void setGreen(uint32_t g8) { g = static_cast<uint8_t>(g8); }
  //! @brief Set blue value of ARGB quad.
  FOG_INLINE void setBlue(uint32_t b8) { b = static_cast<uint8_t>(b8); }

  //! @brief Set red, green and blue components to @a grey.
  FOG_INLINE void setGrey(uint32_t grey8)
  {
    r = static_cast<uint8_t>(grey8);
    g = static_cast<uint8_t>(grey8);
    b = static_cast<uint8_t>(grey8);
  }

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
    Acc::p64ARGB32FromARGB64(u32, argb64.p64);
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
    u32 = MemOps::bswap32(u32);
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

  static FOG_INLINE uint32_t getAlpha(uint32_t argb32) { return getComponent(argb32, PIXEL_ARGB32_SHIFT_A); }
  static FOG_INLINE uint32_t getRed  (uint32_t argb32) { return getComponent(argb32, PIXEL_ARGB32_SHIFT_R); }
  static FOG_INLINE uint32_t getGreen(uint32_t argb32) { return getComponent(argb32, PIXEL_ARGB32_SHIFT_G); }
  static FOG_INLINE uint32_t getBlue (uint32_t argb32) { return getComponent(argb32, PIXEL_ARGB32_SHIFT_B); }

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
    return (a8 << PIXEL_ARGB32_SHIFT_A) | (r8 << PIXEL_ARGB32_SHIFT_R) |
           (g8 << PIXEL_ARGB32_SHIFT_G) | (b8 << PIXEL_ARGB32_SHIFT_B) ;
  }

  // --------------------------------------------------------------------------
  // [Statics - BSwap]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t bswap(uint32_t argb32)
  {
    return MemOps::bswap32(argb32);
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

  // --------------------------------------------------------------------------
  // [Statics - Conversion]
  // --------------------------------------------------------------------------

  static FOG_INLINE Argb32 fromAhsv(const AhsvBaseF& ahsvf)
  {
    Argb32 argb32(UNINITIALIZED);
    fog_api.color_convert[_COLOR_MODEL_ARGB32][COLOR_MODEL_AHSV](&argb32, &ahsvf);
    return argb32;
  }

  static FOG_INLINE Argb32 fromAhsl(const AhslBaseF& ahslf)
  {
    Argb32 argb32(UNINITIALIZED);
    fog_api.color_convert[_COLOR_MODEL_ARGB32][COLOR_MODEL_AHSL](&argb32, &ahslf);
    return argb32;
  }

  static FOG_INLINE Argb32 fromAcmyk(const AcmykBaseF& acmykf)
  {
    Argb32 argb32(UNINITIALIZED);
    fog_api.color_convert[_COLOR_MODEL_ARGB32][COLOR_MODEL_ACMYK](&argb32, &acmykf);
    return argb32;
  }
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::Argb64]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
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
    a = static_cast<uint16_t>(a16);
    r = static_cast<uint16_t>(r16);
    g = static_cast<uint16_t>(g16);
    b = static_cast<uint16_t>(b16);
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

  //! @brief Set the alpha value to @a a.
  FOG_INLINE void setAlpha(uint32_t a16) { a = static_cast<uint16_t>(a16); }
  //! @brief Set the red value to @a r.
  FOG_INLINE void setRed(uint32_t r16) { r = static_cast<uint16_t>(r16); }
  //! @brief Set the green value to @a g.
  FOG_INLINE void setGreen(uint32_t g16) { g = static_cast<uint16_t>(g16); }
  //! @brief Set the blue value to @a b.
  FOG_INLINE void setBlue(uint32_t b16) { b = static_cast<uint16_t>(b16); }

  //! @brief Get grey value (converting RGB components to grey, ignoring alpha).
  FOG_INLINE uint32_t getGrey() const { return getGrey(u64); }

  //! @brief Set the red, green and blue components to @a grey.
  FOG_INLINE void setGrey(uint32_t grey)
  {
    r = static_cast<uint16_t>(grey);
    g = static_cast<uint16_t>(grey);
    b = static_cast<uint16_t>(grey);
  }

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
    a = Math::uroundToWord65535(af);
    r = Math::uroundToWord65535(rf);
    g = Math::uroundToWord65535(gf);
    b = Math::uroundToWord65535(bf);
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
    u64 = MemOps::bswap64(u64);
#else
    uint32_t t0 = u32[0];
    uint32_t t1 = u32[1];

    u32[0] = MemOps::bswap32(t1);
    u32[1] = MemOps::bswap32(t0);
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

  static FOG_INLINE uint32_t getAlpha(uint64_t argb64) { return getComponent(argb64, PIXEL_ARGB64_SHIFT_A); }
  static FOG_INLINE uint32_t getRed  (uint64_t argb64) { return getComponent(argb64, PIXEL_ARGB64_SHIFT_R); }
  static FOG_INLINE uint32_t getGreen(uint64_t argb64) { return getComponent(argb64, PIXEL_ARGB64_SHIFT_G); }
  static FOG_INLINE uint32_t getBlue (uint64_t argb64) { return getComponent(argb64, PIXEL_ARGB64_SHIFT_B); }

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
    return ((uint64_t)a16 << PIXEL_ARGB64_SHIFT_A) | ((uint64_t)r16 << PIXEL_ARGB64_SHIFT_R) |
           ((uint64_t)g16 << PIXEL_ARGB64_SHIFT_G) | ((uint64_t)b16 << PIXEL_ARGB64_SHIFT_B) ;
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

  // --------------------------------------------------------------------------
  // [Statics - Conversion]
  // --------------------------------------------------------------------------

  static FOG_INLINE Argb64 fromAhsv(const AhslBaseF& ahsvf)
  {
    Argb64 argb64(UNINITIALIZED);
    fog_api.color_convert[_COLOR_MODEL_ARGB64][COLOR_MODEL_AHSV](&argb64, &ahsvf);
    return argb64;
  }

  static FOG_INLINE Argb64 fromAhsl(const AhslBaseF& ahslf)
  {
    Argb64 argb64(UNINITIALIZED);
    fog_api.color_convert[_COLOR_MODEL_ARGB64][COLOR_MODEL_AHSL](&argb64, &ahslf);
    return argb64;
  }

  static FOG_INLINE Argb64 fromAcmyk(const AcmykBaseF& acmykf)
  {
    Argb64 argb64(UNINITIALIZED);
    fog_api.color_convert[_COLOR_MODEL_ARGB64][COLOR_MODEL_ACMYK](&argb64, &acmykf);
    return argb64;
  }
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::ArgbF]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
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

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero_t<ArgbF>(this);
  }

  // --------------------------------------------------------------------------
  // [IsOpaque / IsTransparent]
  // --------------------------------------------------------------------------

  //! @brief Get whether the alpha is close to 1.0 (fully-opaque).
  FOG_INLINE bool isOpaque() const
  {
    return Math::isFuzzyGe(a, 1.0f);
  }

  //! @brief Get whether the alpha is close to 0.0 (fully-transparent).
  FOG_INLINE bool isTransparent() const
  {
    return Math::isFuzzyLe(a, 0.0f);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ArgbF& operator=(const ArgbBase32& argb32) { setArgb32(argb32); return *this; }
  FOG_INLINE ArgbF& operator=(const ArgbBase64& argb64) { setArgb64(argb64); return *this; }
  FOG_INLINE ArgbF& operator=(const ArgbBaseF& argbf) { setArgbF(argbf); return *this; }

  // --------------------------------------------------------------------------
  // [Statics - Conversion]
  // --------------------------------------------------------------------------

  static FOG_INLINE ArgbF fromAhsv(const AhsvBaseF& ahsvf)
  {
    ArgbF argbf(UNINITIALIZED);
    fog_api.color_convert[COLOR_MODEL_ARGB][COLOR_MODEL_AHSV](&argbf, &ahsvf);
    return argbf;
  }

  static FOG_INLINE ArgbF fromAhsl(const AhslBaseF& ahslf)
  {
    ArgbF argbf(UNINITIALIZED);
    fog_api.color_convert[COLOR_MODEL_ARGB][COLOR_MODEL_AHSL](&argbf, &ahslf);
    return argbf;
  }

  static FOG_INLINE ArgbF fromAcmyk(const AcmykBaseF& acmykf)
  {
    ArgbF argbf(UNINITIALIZED);
    fog_api.color_convert[COLOR_MODEL_ARGB][COLOR_MODEL_ACMYK](&argbf, &acmykf);
    return argbf;
  }
};
#include <Fog/Core/C++/PackRestore.h>

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
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F_VEC(Fog::ArgbF, 4)

// [Guard]
#endif // _FOG_G2D_SOURCE_ARGB_H
