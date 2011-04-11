// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_ACMYK_H
#define _FOG_G2D_SOURCE_ACMYK_H

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/G2d/Global/Api.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Source/Argb.h>
#include <Fog/G2d/Source/ColorBase.h>
#include <Fog/G2d/Source/ColorUtil.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct AhsvF;

// ============================================================================
// [Fog::AcmykF]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief Color in ACMYK format (alpha, cyan, magenta, yellow, black).
struct FOG_NO_EXPORT AcmykF : public AcmykBaseF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE AcmykF()
  {
    reset();
  }

  explicit FOG_INLINE AcmykF(_Uninitialized)
  {
  }

  FOG_INLINE AcmykF(const AcmykF& acmykf)
  {
    setAcmykF(acmykf);
  }

  FOG_INLINE AcmykF(float af, float cf, float mf, float yf, float kf)
  {
    setAcmykF(af, cf, mf, yf, kf);
  }

  explicit FOG_INLINE AcmykF(const ArgbBase32& argb32)
  {
    setArgb32(argb32);
  }

  explicit FOG_INLINE AcmykF(const ArgbBase64& argb64)
  {
    setArgb64(argb64);
  }

  explicit FOG_INLINE AcmykF(const ArgbBaseF& argbf)
  {
    setArgbF(argbf);
  }

  explicit FOG_INLINE AcmykF(const AhsvBaseF& ahsvf)
  {
    setAhsvF(ahsvf);
  }

  explicit FOG_INLINE AcmykF(const AhslBaseF& ahslf)
  {
    setAhslF(ahslf);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getAlpha() const { return a; }
  FOG_INLINE float getCyan() const { return c; }
  FOG_INLINE float getMagenta() const { return m; }
  FOG_INLINE float getYellow() const { return y; }
  FOG_INLINE float getBlack() const { return k; }

  FOG_INLINE float* getData() { return data; }
  FOG_INLINE const float* getData() const { return data; }

  FOG_INLINE void setAlpha(float af) { a = af; }
  FOG_INLINE void setCyan(float cf) { c = cf; }
  FOG_INLINE void setMagenta(float mf) { m = mf; }
  FOG_INLINE void setYellow(float yf) { y = yf; }
  FOG_INLINE void setBlack(float kf) { k = kf; }

  // --------------------------------------------------------------------------
  // [Conversion]
  // --------------------------------------------------------------------------

  FOG_INLINE Argb32 getArgb32() const
  {
    Argb32 argb32(UNINITIALIZED);
    _g2d.color.convert[_COLOR_MODEL_ARGB32][COLOR_MODEL_ACMYK](&argb32, data);
    return argb32;
  }

  FOG_INLINE Argb64 getArgb64() const
  {
    Argb64 argb64(UNINITIALIZED);
    _g2d.color.convert[_COLOR_MODEL_ARGB64][COLOR_MODEL_ACMYK](&argb64, data);
    return argb64;
  }

  FOG_INLINE ArgbF getArgbF() const
  {
    ArgbF argbf(UNINITIALIZED);
    _g2d.color.convert[COLOR_MODEL_ARGB][COLOR_MODEL_ACMYK](&argbf, data);
    return argbf;
  }

  FOG_INLINE AcmykF getAcmykF() const
  {
    return AcmykF(*this);
  }

  FOG_INLINE AhsvF getAhsvF() const;
  FOG_INLINE AhslF getAhslF() const;

  FOG_INLINE void setArgb32(const ArgbBase32& argb32)
  {
    _g2d.color.convert[COLOR_MODEL_ACMYK][_COLOR_MODEL_ARGB32](data, &argb32);
  }

  FOG_INLINE void setArgb64(const ArgbBase64& argb64)
  {
    _g2d.color.convert[COLOR_MODEL_ACMYK][_COLOR_MODEL_ARGB64](data, &argb64);
  }

  FOG_INLINE void setArgbF(const ArgbBaseF& argbf)
  {
    _g2d.color.convert[COLOR_MODEL_ACMYK][COLOR_MODEL_ARGB](data, &argbf);
  }

  FOG_INLINE void setArgbF(float af, float rf, float gf, float bf)
  {
    float argbf[4] = { af, rf, gf, bf };
    _g2d.color.convert[COLOR_MODEL_ACMYK][COLOR_MODEL_ARGB](data, argbf);
  }

  FOG_INLINE void setAcmykF(const AcmykF& acmyk)
  {
    a = acmyk.a;
    c = acmyk.c;
    m = acmyk.m;
    y = acmyk.y;
    k = acmyk.k;
  }

  FOG_INLINE void setAcmykF(float af, float cf, float mf, float yf, float kf)
  {
    a = af;
    c = cf;
    m = mf;
    y = yf;
    k = kf;
  }

  FOG_INLINE void setAhsvF(const AhsvBaseF& ahsv)
  {
    _g2d.color.convert[COLOR_MODEL_ACMYK][COLOR_MODEL_AHSV](data, &ahsv);
  }

  FOG_INLINE void setAhsvF(float af, float hf, float sf, float vf)
  {
    float ahsvf[4] = { af, hf, sf, vf };
    _g2d.color.convert[COLOR_MODEL_ACMYK][COLOR_MODEL_AHSV](data, ahsvf);
  }

  FOG_INLINE void setAhslF(const AhslBaseF& ahslf)
  {
    _g2d.color.convert[COLOR_MODEL_ACMYK][COLOR_MODEL_AHSL](data, &ahslf);
  }

  FOG_INLINE void setAhslF(float af, float hf, float sf, float vf)
  {
    float ahslf[4] = { af, hf, sf, vf };
    _g2d.color.convert[COLOR_MODEL_ACMYK][COLOR_MODEL_AHSL](data, ahslf);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    Memory::zero_t<AcmykF>(this);
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
};
#include <Fog/Core/Pack/PackRestore.h>

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::AcmykF, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_DECLARE_FUZZY(Fog::AcmykF, Math::feqv(a.data, b.data, 5))

// [Guard]
#endif // _FOG_G2D_SOURCE_ACMYK_H
