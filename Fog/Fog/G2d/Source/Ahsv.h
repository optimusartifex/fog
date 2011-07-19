// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_COLOR_AHSV_H
#define _FOG_G2D_COLOR_AHSV_H

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
//! @brief Color in AHSV format (alpha, hue, saturation, value).
struct FOG_NO_EXPORT AhsvF : public AhsvBaseF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE AhsvF()
  {
    reset();
  }

  explicit FOG_INLINE AhsvF(_Uninitialized)
  {
  }

  FOG_INLINE AhsvF(const AhsvF& ahsvf)
  {
    setAhsvF(ahsvf);
  }

  FOG_INLINE AhsvF(float af, float hf, float sf, float vf)
  {
    setAhsvF(af, hf, sf, vf);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getAlpha() const { return a; }
  FOG_INLINE float getHue() const { return h; }
  FOG_INLINE float getSaturation() const { return s; }
  FOG_INLINE float getValue() const { return v; }

  FOG_INLINE float* getData() { return data; }
  FOG_INLINE const float* getData() const { return data; }

  FOG_INLINE void setAlpha(float af) { a = af; }
  FOG_INLINE void setHue(float hf) { h = hf; }
  FOG_INLINE void setSaturation(float sf) { s = sf; }
  FOG_INLINE void setValue(float vf) { v = vf; }

  // --------------------------------------------------------------------------
  // [Conversion]
  // --------------------------------------------------------------------------

  FOG_INLINE AhsvF getAhsvF() const
  {
    return AhsvF(*this);
  }

  FOG_INLINE void setAhsvF(const AhsvBaseF& ahsvf)
  {
    a = ahsvf.a;
    h = ahsvf.h;
    s = ahsvf.s;
    v = ahsvf.v;
  }

  FOG_INLINE void setAhsvF(float af, float hf, float sf, float vf)
  {
    a = af;
    h = hf;
    s = sf;
    v = vf;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    Memory::zero_t<AhsvF>(this);
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

  static FOG_INLINE AhsvF fromArgb(const ArgbBase32& argb32)
  {
    AhsvF ahsvf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_AHSV][_COLOR_MODEL_ARGB32](&ahsvf, &argb32);
    return ahsvf;
  }

  static FOG_INLINE AhsvF fromArgb(const ArgbBase64& argb64)
  {
    AhsvF ahsvf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_AHSV][_COLOR_MODEL_ARGB64](&ahsvf, &argb64);
    return ahsvf;
  }

  static FOG_INLINE AhsvF fromArgb(const ArgbBaseF& argbf)
  {
    AhsvF ahsvf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_AHSV][COLOR_MODEL_ARGB](&ahsvf, &argbf);
    return ahsvf;
  }

  static FOG_INLINE AhsvF fromAhsl(const AhslBaseF& ahslf)
  {
    AhsvF ahsvf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_AHSV][COLOR_MODEL_AHSL](&ahsvf, &ahslf);
    return ahsvf;
  }

  static FOG_INLINE AhsvF fromAcmyk(const AcmykBaseF& acmykf)
  {
    AhsvF ahsvf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_AHSV][COLOR_MODEL_ACMYK](&ahsvf, &acmykf);
    return ahsvf;
  }
};
#include <Fog/Core/Pack/PackRestore.h>

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::AhsvF, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE(Fog::AhsvF, Math::feqv(a.data, b.data, 4))

// [Guard]
#endif // _FOG_G2D_COLOR_AHSV_H
