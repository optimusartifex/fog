// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_BLUR_H
#define _FOG_G2D_IMAGING_FILTERS_BLUR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Imaging/BorderFilterTag.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::Blur]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
struct FOG_NO_EXPORT Blur : public BorderFilterTag
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Blur()
  {
    _api.blur_ctor(this);
  }

  FOG_INLINE Blur(const Blur& other)
  {
    _api.blur_ctorCopy(this, &other);
  }

  FOG_INLINE Blur(uint32_t blurType, float radius)
  {
    _api.blur_ctor(this);

    _blurType = blurType;
    _hRadius = radius;
    _vRadius = radius;
  }

  FOG_INLINE Blur(uint32_t blurType, float hRadius, float vRadius)
  {
    _api.blur_ctor(this);

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

  FOG_INLINE void setBlurType(uint32_t blurType)
  {
    FOG_ASSERT(blurType < BLUR_TYPE_COUNT);
    _blurType = blurType;
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

  FOG_INLINE bool eq(const Blur& other) const
  {
    return _api.blur_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Blur& operator=(const Blur& other)
  {
    _api.blur_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const Blur& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Blur& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const Blur* a, const Blur* b)
  {
    return _api.blur_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.blur_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The blur type, see @c BLUR_TYPE.
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
#endif // _FOG_G2D_IMAGING_FILTERS_BLUR_H
