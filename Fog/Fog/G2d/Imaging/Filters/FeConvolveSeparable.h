// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_FECONVOLVESEPARABLE_H
#define _FOG_G2D_IMAGING_FILTERS_FECONVOLVESEPARABLE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/G2d/Imaging/Filters/FeBorder.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::FeConvolveSeparable]
// ============================================================================

//! @brief Convolve separable.
struct FOG_NO_EXPORT FeConvolveSeparable : public FeBorder
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeConvolveSeparable()
  {
    fog_api.feconvolveseparable_ctor(this);
  }

  FOG_INLINE FeConvolveSeparable(const FeConvolveSeparable& other)
  {
    fog_api.feconvolveseparable_ctorCopy(this, &other);
  }

  FOG_INLINE ~FeConvolveSeparable()
  {
    fog_api.feconvolveseparable_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getHorzScale() const { return _hScale; }
  FOG_INLINE float getHorzBias() const { return _hBias; }
  FOG_INLINE const List<float>& getHorzVector() const { return _hVector(); }

  FOG_INLINE float getVertScale() const { return _vScale; }
  FOG_INLINE float getVertBias() const { return _vBias; }
  FOG_INLINE const List<float>& getVertVector() const { return _vVector(); }

  FOG_INLINE void setHorzScale(float scale) { _hScale = scale; }
  FOG_INLINE void setHorzBias(float bias) { _hBias = bias; }
  FOG_INLINE void setHorzVector(const List<float>& vector) { _hVector() = vector; }

  FOG_INLINE void setVertScale(float scale) { _vScale = scale; }
  FOG_INLINE void setVertBias(float bias) { _vBias = bias; }
  FOG_INLINE void setVertVector(const List<float>& vector) { _vVector() = vector; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.feconvolveseparable_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeConvolveSeparable& other) const
  {
    return fog_api.feconvolveseparable_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE FeConvolveSeparable& operator=(const FeConvolveSeparable& other)
  {
    fog_api.feconvolveseparable_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeConvolveSeparable& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeConvolveSeparable& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FeConvolveSeparable* a, const FeConvolveSeparable* b)
  {
    return fog_api.feconvolveseparable_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.feconvolveseparable_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static< List<float> > _hVector;
  Static< List<float> > _vVector;

  float _hScale;
  float _vScale;

  float _hBias;
  float _vBias;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_FECONVOLVESEPARABLE_H
