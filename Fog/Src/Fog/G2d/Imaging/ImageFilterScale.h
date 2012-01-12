// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEFILTERSCALE_H
#define _FOG_G2D_IMAGING_IMAGEFILTERSCALE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Point.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ImageFilterScaleF]
// ============================================================================

//! @brief Scaling (float).
struct FOG_NO_EXPORT ImageFilterScaleF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageFilterScaleF()
  {
    reset();
  }

  FOG_INLINE ImageFilterScaleF(const ImageFilterScaleF& other)
  {
    setFilterScale(other);
  }

  explicit FOG_INLINE ImageFilterScaleF(_Uninitialized) {}
  explicit FOG_INLINE ImageFilterScaleF(const ImageFilterScaleD& other) { setFilterScale(other); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointF& getPoint() const { return _pt; }
  FOG_INLINE void setPoint(const PointF& pt) { _pt = pt; }

  FOG_INLINE uint32_t isSwapped() const { return _swapped; }
  FOG_INLINE void setSwapped(uint32_t swapped) { _swapped = swapped; }

  FOG_INLINE void setFilterScale(const ImageFilterScaleF& other)
  {
    _pt = other._pt;
    _swapped = other._swapped;
  }

  // Implemented-Later.
  FOG_INLINE void setFilterScale(const ImageFilterScaleD& other);

  FOG_INLINE void setFilterScale(const PointF& pt, uint32_t swapped)
  {
    _pt = pt;
    _swapped = swapped;
  }

  FOG_INLINE void setFilterScale(float x, float y, uint32_t swapped)
  {
    _pt.set(x, y);
    _swapped = swapped;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _pt.set(1.0f, 1.0f);
    _swapped = 0;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageFilterScaleF& operator=(const ImageFilterScaleF& other) { setFilterScale(other); return *this; }
  FOG_INLINE ImageFilterScaleF& operator=(const ImageFilterScaleD& other) { setFilterScale(other); return *this; }

  FOG_INLINE bool operator==(const ImageFilterScaleF& other) const { return  MemOps::eq_t<ImageFilterScaleF>(this, &other); }
  FOG_INLINE bool operator!=(const ImageFilterScaleF& other) const { return !MemOps::eq_t<ImageFilterScaleF>(this, &other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF _pt;
  uint32_t _swapped;
};

// ============================================================================
// [Fog::ImageFilterScaleD]
// ============================================================================

//! @brief Scaling (double).
struct FOG_NO_EXPORT ImageFilterScaleD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageFilterScaleD()
  {
    reset();
  }

  FOG_INLINE ImageFilterScaleD(const ImageFilterScaleD& other)
  {
    setFilterScale(other);
  }

  explicit FOG_INLINE ImageFilterScaleD(_Uninitialized) {}
  explicit FOG_INLINE ImageFilterScaleD(const ImageFilterScaleF& other) { setFilterScale(other); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PointD& getPoint() const { return _pt; }
  FOG_INLINE void setPoint(const PointD& pt) { _pt = pt; }

  FOG_INLINE uint32_t isSwapped() const { return _swapped; }
  FOG_INLINE void setSwapped(uint32_t swapped) { _swapped = swapped; }

  FOG_INLINE void setFilterScale(const ImageFilterScaleD& other)
  {
    _pt = other._pt;
    _swapped = other._swapped;
  }

  FOG_INLINE void setFilterScale(const ImageFilterScaleF& other)
  {
    _pt = other._pt;
    _swapped = other._swapped;
  }

  FOG_INLINE void setFilterScale(const PointD& pt, uint32_t swapped)
  {
    _pt = pt;
    _swapped = swapped;
  }

  FOG_INLINE void setFilterScale(double x, double y, uint32_t swapped)
  {
    _pt.set(x, y);
    _swapped = swapped;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _pt.set(1.0, 1.0);
    _swapped = 0;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageFilterScaleD& operator=(const ImageFilterScaleF& other) { setFilterScale(other); return *this; }
  FOG_INLINE ImageFilterScaleD& operator=(const ImageFilterScaleD& other) { setFilterScale(other); return *this; }

  FOG_INLINE bool operator==(const ImageFilterScaleD& other) const { return  MemOps::eq_t<ImageFilterScaleD>(this, &other); }
  FOG_INLINE bool operator!=(const ImageFilterScaleD& other) const { return !MemOps::eq_t<ImageFilterScaleD>(this, &other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointD _pt;
  uint32_t _swapped;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE void ImageFilterScaleF::setFilterScale(const ImageFilterScaleD& other)
{
  _pt = other._pt;
  _swapped = other._swapped;
}

// ============================================================================
// [Fog::ScalingT<>]
// ============================================================================

_FOG_NUM_T(ImageFilterScale)
_FOG_NUM_F(ImageFilterScale)
_FOG_NUM_D(ImageFilterScale)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F_VEC(Fog::ImageFilterScaleF, 2);
FOG_FUZZY_DECLARE_D_VEC(Fog::ImageFilterScaleD, 2);

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEFILTERSCALE_H
