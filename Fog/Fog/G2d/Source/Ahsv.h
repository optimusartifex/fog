// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_AHSV_H
#define _FOG_G2D_SOURCE_AHSV_H

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Ops.h>
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
struct AhslF;

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

  explicit FOG_INLINE AhsvF(const ArgbBase32& argb32)
  {
    setArgb32(argb32);
  }

  explicit FOG_INLINE AhsvF(const ArgbBase64& argb64)
  {
    setArgb64(argb64);
  }

  explicit FOG_INLINE AhsvF(const ArgbBaseF& argbf)
  {
    setArgbF(argbf);
  }

  explicit FOG_INLINE AhsvF(const AcmykBaseF& acmykf)
  {
    setAcmykF(acmykf);
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

  FOG_INLINE Argb32 getArgb32() const
  {
    Argb32 argb32(UNINITIALIZED);
    _g2d.color.convert[_COLOR_MODEL_ARGB32][COLOR_MODEL_AHSV](&argb32, this);
    return argb32;
  }

  FOG_INLINE Argb64 getArgb64() const
  {
    Argb64 argb64(UNINITIALIZED);
    _g2d.color.convert[_COLOR_MODEL_ARGB64][COLOR_MODEL_AHSV](&argb64, this);
    return argb64;
  }

  FOG_INLINE ArgbF getArgbF() const
  {
    ArgbF argbf(UNINITIALIZED);
    _g2d.color.convert[COLOR_MODEL_ARGB][COLOR_MODEL_AHSV](&argbf, this);
    return argbf;
  }

  FOG_INLINE AhsvF getAhsvF() const
  {
    return AhsvF(*this);
  }

  FOG_INLINE AhslF getAhslF() const;
  FOG_INLINE AcmykF getAcmykF() const;

  FOG_INLINE void setArgb32(const ArgbBase32& argb32)
  {
    return _g2d.color.convert[COLOR_MODEL_AHSV][_COLOR_MODEL_ARGB32](this, &argb32);
  }

  FOG_INLINE void setArgb64(const ArgbBase64& argb64)
  {
    return _g2d.color.convert[COLOR_MODEL_AHSV][_COLOR_MODEL_ARGB64](this, &argb64);
  }

  FOG_INLINE void setArgbF(const ArgbBaseF& argbf)
  {
    return _g2d.color.convert[COLOR_MODEL_AHSV][COLOR_MODEL_ARGB](this, &argbf);
  }

  FOG_INLINE void setArgbF(float af, float rf, float gf, float bf)
  {
    float argbf[4] = { af, rf, gf, bf };
    return _g2d.color.convert[COLOR_MODEL_AHSV][_COLOR_MODEL_ARGB32](this, argbf);
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

  FOG_INLINE void setAcmykF(const AcmykBaseF& acmykf)
  {
    _g2d.color.convert[COLOR_MODEL_AHSV][COLOR_MODEL_ACMYK](this, &acmykf);
  }

  FOG_INLINE void setAcmykF(float a, float c, float m, float y, float k)
  {
    float acmykf[5] = { a, c, m, y, k };
    _g2d.color.convert[COLOR_MODEL_AHSV][COLOR_MODEL_ACMYK](this, acmykf);
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
#endif // _FOG_G2D_SOURCE_AHSV_H
