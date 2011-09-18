// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_ACMYK_H
#define _FOG_G2D_SOURCE_ACMYK_H

// [Dependencies]
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Source/ColorBase.h>
#include <Fog/G2d/Source/ColorUtil.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

// ============================================================================
// [Fog::AcmykF]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
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

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero_t<AcmykF>(this);
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

  static FOG_INLINE AcmykF fromArgb(const ArgbBase32& argb32)
  {
    AcmykF acmykf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_ACMYK][_COLOR_MODEL_ARGB32](&acmykf, &argb32);
    return acmykf;
  }

  static FOG_INLINE AcmykF fromArgb(const ArgbBase64& argb64)
  {
    AcmykF acmykf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_ACMYK][_COLOR_MODEL_ARGB64](&acmykf, &argb64);
    return acmykf;
  }

  static FOG_INLINE AcmykF fromArgb(const ArgbBaseF& argbf)
  {
    AcmykF acmykf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_ACMYK][COLOR_MODEL_ARGB](&acmykf, &argbf);
    return acmykf;
  }

  static FOG_INLINE AcmykF fromAhsv(const AhsvBaseF& ahsvf)
  {
    AcmykF acmykf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_ACMYK][COLOR_MODEL_AHSV](&acmykf, &ahsvf);
    return acmykf;
  }

  static FOG_INLINE AcmykF fromAhsl(const AhsvBaseF& ahslf)
  {
    AcmykF acmykf(UNINITIALIZED);
    _api.color.convert[COLOR_MODEL_ACMYK][COLOR_MODEL_AHSL](&acmykf, &ahslf);
    return acmykf;
  }
};
#include <Fog/Core/C++/PackRestore.h>

//! @}

} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F_VEC(Fog::AcmykF, 5)

// [Guard]
#endif // _FOG_G2D_SOURCE_ACMYK_H
