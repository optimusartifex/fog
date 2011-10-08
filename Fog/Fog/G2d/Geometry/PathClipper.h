// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PATHCLIPPER_H
#define _FOG_G2D_GEOMETRY_PATHCLIPPER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Rect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::PathClipperF]
// ============================================================================

struct FOG_NO_EXPORT PathClipperF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathClipperF(const BoxF& clipBox) :
    _clipBox(clipBox),
    _lastMoveTo(Math::getQNanF(), Math::getQNanF()),
    _lastIndex(INVALID_INDEX)
  {
  }

  FOG_INLINE PathClipperF(const RectF& clipBox) :
    _clipBox(clipBox),
    _lastMoveTo(Math::getQNanF(), Math::getQNanF()),
    _lastIndex(INVALID_INDEX)
  {
  }

  FOG_INLINE PathClipperF(const PathClipperF& clipper) :
    _clipBox(clipper._clipBox),
    _lastMoveTo(clipper._lastMoveTo),
    _lastIndex(clipper._lastIndex)
  {
  }

  FOG_INLINE ~PathClipperF()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxF& getClipBox() const { return _clipBox; }
  FOG_INLINE void setClipBox(const BoxF& clipBox) { _clipBox = clipBox; }

  FOG_INLINE size_t getLastIndex() const { return _lastIndex; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _clipBox.reset();
    _lastMoveTo.setNaN();
    _lastIndex = INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Initialize the PathClipperF context to clip the path @a path.
  //!
  //! The given path is not transformed.
  //!
  //! @retval @true The path must be clipped, the function continuePath should
  //! be called to clip it.
  //! @retval @false The path do not need to be clipped.
  FOG_INLINE uint32_t measurePath(const PathF& src)
  {
    return _api.pathclipperf_measurePath(this, &src);
  }

  //! @brief Continue with the clipping using the info collected by
  //! @c measurePath() method. The path @a src must be the same as used by
  //! @c measurePath() method.
  //!
  //! @note Calling this function without calling @c measurePath() will cause
  //! the @c ERR_RT_INVALID_STATE error.
  FOG_INLINE err_t continuePath(PathF& dst, const PathF& src)
  {
    return _api.pathclipperf_continuePath(this, &dst, &src);
  }

  FOG_INLINE err_t continuePathData(PathF& dst, const PointF* srcPts, const uint8_t* srcCmd, size_t srcLength)
  {
    return _api.pathclipperf_continuePathData(this, &dst, srcPts, srcCmd, srcLength);
  }

  FOG_INLINE void resetPath()
  {
    _lastIndex = INVALID_INDEX;
  }

  //! @brief Clip path @a src to @a dst.
  FOG_INLINE err_t clipPath(PathF& dst, const PathF& src)
  {
    return _api.pathclipperf_clipPath(this, &dst, &src, NULL);
  }

  //! @brief Clip transformed path @a src to @a dst.
  FOG_INLINE err_t clipPath(PathF& dst, const PathF& src, const TransformF& tr)
  {
    return _api.pathclipperf_clipPath(this, &dst, &src, &tr);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PathClipperF& operator=(const PathClipperF& other)
  {
    _clipBox = other._clipBox;
    _lastMoveTo = other._lastMoveTo;
    _lastIndex = other._lastIndex;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  BoxF _clipBox;
  PointF _lastMoveTo;
  size_t _lastIndex;
};

// ============================================================================
// [Fog::PathClipperD]
// ============================================================================

struct FOG_NO_EXPORT PathClipperD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathClipperD(const BoxD& clipBox) :
    _clipBox(clipBox),
    _lastMoveTo(Math::getQNanD(), Math::getQNanD()),
    _lastIndex(INVALID_INDEX)
  {
  }

  FOG_INLINE PathClipperD(const RectF& clipBox) :
    _clipBox(clipBox),
    _lastMoveTo(Math::getQNanD(), Math::getQNanD()),
    _lastIndex(INVALID_INDEX)
  {
  }

  FOG_INLINE PathClipperD(const PathClipperD& clipper) :
    _clipBox(clipper._clipBox),
    _lastMoveTo(clipper._lastMoveTo),
    _lastIndex(clipper._lastIndex)
  {
  }

  FOG_INLINE ~PathClipperD()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxD& getClipBox() const { return _clipBox; }
  FOG_INLINE void setClipBox(const BoxD& clipBox) { _clipBox = clipBox; }

  FOG_INLINE size_t getLastIndex() const { return _lastIndex; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _clipBox.reset();
    _lastMoveTo.setNaN();
    _lastIndex = INVALID_INDEX;
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Initialize the PathClipperD context to clip the path @a path.
  //!
  //! The given path is not transformed.
  //!
  //! @retval @true The path must be clipped, the function continuePath should
  //! be called to clip it.
  //! @retval @false The path do not need to be clipped.
  FOG_INLINE uint32_t measurePath(const PathD& src)
  {
    return _api.pathclipperd_measurePath(this, &src);
  }

  //! @brief Continue with the clipping using the info collected by
  //! @c measurePath() method. The path @a src must be the same as used by
  //! @c measurePath() method.
  //!
  //! @note Calling this function without calling @c measurePath() will cause
  //! the @c ERR_RT_INVALID_STATE error.
  FOG_INLINE err_t continuePath(PathD& dst, const PathD& src)
  {
    return _api.pathclipperd_continuePath(this, &dst, &src);
  }

  FOG_INLINE err_t continuePathData(PathD& dst, const PointD* srcPts, const uint8_t* srcCmd, size_t srcLength)
  {
    return _api.pathclipperd_continuePathData(this, &dst, srcPts, srcCmd, srcLength);
  }

  FOG_INLINE void resetPath()
  {
    _lastIndex = INVALID_INDEX;
  }

  //! @brief Clip path @a src to @a dst.
  FOG_INLINE err_t clipPath(PathD& dst, const PathD& src)
  {
    return _api.pathclipperd_clipPath(this, &dst, &src, NULL);
  }

  //! @brief Clip transformed path @a src to @a dst.
  FOG_INLINE err_t clipPath(PathD& dst, const PathD& src, const TransformD& tr)
  {
    return _api.pathclipperd_clipPath(this, &dst, &src, &tr);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PathClipperD& operator=(const PathClipperD& other)
  {
    _clipBox = other._clipBox;
    _lastMoveTo = other._lastMoveTo;
    _lastIndex = other._lastIndex;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  BoxD _clipBox;
  PointD _lastMoveTo;
  size_t _lastIndex;
};

// ============================================================================
// [Fog::PathClipperT<>]
// ============================================================================

_FOG_NUM_T(PathClipper)
_FOG_NUM_F(PathClipper)
_FOG_NUM_D(PathClipper)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PATHCLIPPER_H
