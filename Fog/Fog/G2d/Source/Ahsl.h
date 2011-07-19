// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_COLOR_AHSL_H
#define _FOG_G2D_COLOR_AHSL_H

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Ops.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Source/ColorBase.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

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

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE AhslF fromArgb(const ArgbBase32& argb32)
  {
    AhslF ahslf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_AHSL][_COLOR_MODEL_ARGB32](&ahslf, &argb32);
    return ahslf;
  }

  static FOG_INLINE AhslF fromArgb(const ArgbBase64& argb64)
  {
    AhslF ahslf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_AHSL][_COLOR_MODEL_ARGB64](&ahslf, &argb64);
    return ahslf;
  }

  static FOG_INLINE AhslF fromArgb(const ArgbBaseF& argbf)
  {
    AhslF ahslf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_ARGB](&ahslf, &argbf);
    return ahslf;
  }

  static FOG_INLINE AhslF fromAhsv(const AhsvBaseF& ahsvf)
  {
    AhslF ahslf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_AHSV](&ahslf, &ahsvf);
    return ahslf;
  }

  static FOG_INLINE AhslF fromAcmyk(const AcmykBaseF& acmykf)
  {
    AhslF ahslf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_AHSL][COLOR_MODEL_ACMYK](&ahslf, &acmykf);
    return ahslf;
  }
};
#include <Fog/Core/Pack/PackRestore.h>

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::AhslF, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE(Fog::AhslF, Math::feqv(a.data, b.data, 4))

// [Guard]
#endif // _FOG_G2D_COLOR_AHSL_H
