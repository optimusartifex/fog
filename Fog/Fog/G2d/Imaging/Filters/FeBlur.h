// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_FEBLUR_H
#define _FOG_G2D_IMAGING_FILTERS_FEBLUR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Imaging/Filters/FeBorder.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::Blur]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
struct FOG_NO_EXPORT FeBlur : public FeBorder
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeBlur()
  {
    _api.feblur_ctor(this);
  }

  FOG_INLINE FeBlur(const FeBlur& other)
  {
    _api.feblur_ctorCopy(this, &other);
  }

  FOG_INLINE FeBlur(uint32_t blurType, float radius)
  {
    _api.feblur_ctor(this);

    _blurType = blurType;
    _hRadius = radius;
    _vRadius = radius;
  }

  FOG_INLINE FeBlur(uint32_t blurType, float hRadius, float vRadius)
  {
    _api.feblur_ctor(this);

    _blurType = blurType;
    _hRadius = hRadius;
    _vRadius = vRadius;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getBlurType() const { return _blurType; }
  FOG_INLINE float getHorizontalRadius() const { return _hRadius; }
  FOG_INLINE float getVerticalRadius() const { return _vRadius; }

  FOG_INLINE err_t setBlurType(uint32_t blurType)
  {
    if (blurType >= FE_BLUR_TYPE_COUNT)
      return ERR_RT_INVALID_ARGUMENT;

    _blurType = blurType;
    return ERR_OK;
  }

  FOG_INLINE void setHorizontalRadius(float radius) { _hRadius = radius; }
  FOG_INLINE void setVerticalRadius(float radius) { _vRadius = radius; }

  FOG_INLINE void setRadius(float radius)
  {
    _hRadius = radius;
    _vRadius = radius;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeBlur& other) const
  {
    return _api.feblur_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FeBlur& operator=(const FeBlur& other)
  {
    _api.feblur_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeBlur& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeBlur& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FeBlur* a, const FeBlur* b)
  {
    return _api.feblur_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.feblur_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The blur type, see @c FE_BLUR_TYPE.
  uint32_t _blurType;

  //! @brief The horizontal radius.
  float _hRadius;
  //! @brief The vertical radius.
  float _vRadius;
};
#include <Fog/Core/C++/PackRestore.h>

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_FEBLUR_H
