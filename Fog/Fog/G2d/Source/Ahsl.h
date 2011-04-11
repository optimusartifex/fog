// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_AHSL_H
#define _FOG_G2D_SOURCE_AHSL_H

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

struct AcmykF;
struct AhsvF;

// ============================================================================
// [Fog::AhsvF]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @brief Color in AHSL format (alpha, hue, saturation, lightness).
struct FOG_NO_EXPORT AhslF : public AhslBaseF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE AhslF()
  {
    reset();
  }

  explicit FOG_INLINE AhslF(_Uninitialized)
  {
  }

  FOG_INLINE AhslF(const AhslF& ahslf)
  {
    setAhslF(ahslf);
  }

  FOG_INLINE AhslF(float af, float hf, float sf, float lf)
  {
    setAhslF(af, hf, sf, lf);
  }

  explicit FOG_INLINE AhslF(const ArgbBase32& argb32)
  {
    setArgb32(argb32);
  }

  explicit FOG_INLINE AhslF(const ArgbBase64& argb64)
  {
    setArgb64(argb64);
  }

  explicit FOG_INLINE AhslF(const ArgbBaseF& argbf)
  {
    setArgbF(argbf);
  }

  explicit FOG_INLINE AhslF(const AcmykBaseF& acmykf)
  {
    setAcmykF(acmykf);
  }

  explicit FOG_INLINE AhslF(const AhsvBaseF& ahsvf)
  {
    setAhsvF(ahsvf);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getAlpha() const { return a; }
  FOG_INLINE float getHue() const { return h; }
  FOG_INLINE float getSaturation() const { return s; }
  FOG_INLINE float getLightness() const { return l; }

  FOG_INLINE float* getData() { return data; }
  FOG_INLINE const float* getData() const { return data; }

  FOG_INLINE void setAlpha(float af) { a = af; }
  FOG_INLINE void setHue(float hf) { h = hf; }
  FOG_INLINE void setSaturation(float sf) { s = sf; }
  FOG_INLINE void setLightness(float lf) { l = lf; }

  // --------------------------------------------------------------------------
  // [Conversion]
  // --------------------------------------------------------------------------

  FOG_INLINE Argb32 getArgb32() const
  {
    Argb32 argb32(UNINITIALIZED);
    _g2d.color.convert[_COLOR_MODEL_ARGB32][COLOR_MODEL_AHSL](&argb32, this);
    return argb32;
  }

  FOG_INLINE Argb64 getArgb64() const
  {
    Argb64 argb64(UNINITIALIZED);
    _g2d.color.convert[_COLOR_MODEL_ARGB64][COLOR_MODEL_AHSL](&argb64, this);
    return argb64;
  }

  FOG_INLINE ArgbF getArgbF() const
  {
    ArgbF argbf(UNINITIALIZED);
    _g2d.color.convert[COLOR_MODEL_ARGB][COLOR_MODEL_AHSL](&argbf, this);
    return argbf;
  }

  FOG_INLINE AhslF getAhslF() const
  {
    return AhslF(*this);
  }

  FOG_INLINE AhsvF getAhsvF() const;
  FOG_INLINE AcmykF getAcmykF() const;

  FOG_INLINE void setArgb32(const ArgbBase32& argb32)
  {
    _g2d.color.convert[COLOR_MODEL_AHSL][_COLOR_MODEL_ARGB32](this, &argb32);
  }

  FOG_INLINE void setArgb64(const ArgbBase64& argb64)
  {
    _g2d.color.convert[COLOR_MODEL_AHSL][_COLOR_MODEL_ARGB64](this, &argb64);
  }

  FOG_INLINE void setArgbF(const ArgbBaseF& argbf)
  {
    _g2d.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_ARGB](this, &argbf);
  }

  FOG_INLINE void setArgbF(float af, float rf, float gf, float bf)
  {
    float argbf[4] = { af, rf, gf, bf };
    _g2d.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_ARGB](this, argbf);
  }

  FOG_INLINE void setAhsvF(const AhsvBaseF& ahsvf)
  {
    _g2d.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_AHSV](this, &ahsvf);
  }

  FOG_INLINE void setAhsvF(float af, float hf, float sf, float vf)
  {
    float ahsvf[4] = { af, hf, sf, vf };
    _g2d.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_AHSV](this, ahsvf);
  }

  FOG_INLINE void setAhslF(const AhslBaseF& ahslf)
  {
    a = ahslf.a;
    h = ahslf.h;
    s = ahslf.s;
    l = ahslf.l;
  }

  FOG_INLINE void setAhslF(float af, float hf, float sf, float lf)
  {
    a = af;
    h = hf;
    s = sf;
    l = lf;
  }

  FOG_INLINE void setAcmykF(const AcmykBaseF& acmykf)
  {
    _g2d.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_ACMYK](this, &acmykf);
  }

  FOG_INLINE void setAcmykF(float af, float cf, float mf, float yf, float kf)
  {
    float acmykf[5] = { af, cf, mf, yf, kf };
    _g2d.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_ACMYK](this, acmykf);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    Memory::zero_t<AhslF>(this);
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

FOG_DECLARE_TYPEINFO(Fog::AhslF, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_DECLARE_FUZZY(Fog::AhslF, Math::feqv(a.data, b.data, 4))

// [Guard]
#endif // _FOG_G2D_SOURCE_AHSL_H
