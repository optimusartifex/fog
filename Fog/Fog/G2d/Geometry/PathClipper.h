// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PATHCLIPPER_H
#define _FOG_G2D_GEOMETRY_PATHCLIPPER_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Memory/MemoryOps.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct PathF;
struct PathD;

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

  FOG_INLINE sysuint_t getLastIndex() const { return _lastIndex; }

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
  FOG_INLINE uint32_t initPath(const PathF& src)
  {
    return _g2d.pathclipperf.initPath(*this, src);
  }

  //! @brief Continue with the clipping using the info collected by
  //! @c initPath() method. The path @a src must be the same as used by
  //! @c initPath() method.
  //!
  //! @note Calling this function without calling @c initPath() will cause
  //! the @c ERR_RT_INVALID_STATE error.
  FOG_INLINE err_t continuePath(PathF& dst, const PathF& src)
  {
    return _g2d.pathclipperf.continuePath(*this, dst, src);
  }

  FOG_INLINE err_t continueRaw(PathF& dst, const PointF* srcPts, const uint8_t* srcCmd, sysuint_t srcLength)
  {
    return _g2d.pathclipperf.continueRaw(*this, dst, srcPts, srcCmd, srcLength);
  }

  FOG_INLINE void resetPath()
  {
    _lastIndex = INVALID_INDEX;
  }

  //! @brief Clip path @a src to @a dst.
  FOG_INLINE err_t clipPath(PathF& dst, const PathF& src)
  {
    return _g2d.pathclipperf.clipPath(*this, dst, src, NULL);
  }

  //! @brief Clip transformed path @a src to @a dst.
  FOG_INLINE err_t clipPath(PathF& dst, const PathF& src, const TransformF& tr)
  {
    return _g2d.pathclipperf.clipPath(*this, dst, src, &tr);
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
  sysuint_t _lastIndex;
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

  FOG_INLINE sysuint_t getLastIndex() const { return _lastIndex; }

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
  FOG_INLINE uint32_t initPath(const PathD& src)
  {
    return _g2d.pathclipperd.initPath(*this, src);
  }

  //! @brief Continue with the clipping using the info collected by
  //! @c initPath() method. The path @a src must be the same as used by
  //! @c initPath() method.
  //!
  //! @note Calling this function without calling @c initPath() will cause
  //! the @c ERR_RT_INVALID_STATE error.
  FOG_INLINE err_t continuePath(PathD& dst, const PathD& src)
  {
    return _g2d.pathclipperd.continuePath(*this, dst, src);
  }

  FOG_INLINE err_t continueRaw(PathD& dst, const PointD* srcPts, const uint8_t* srcCmd, sysuint_t srcLength)
  {
    return _g2d.pathclipperd.continueRaw(*this, dst, srcPts, srcCmd, srcLength);
  }

  FOG_INLINE void resetPath()
  {
    _lastIndex = INVALID_INDEX;
  }

  //! @brief Clip path @a src to @a dst.
  FOG_INLINE err_t clipPath(PathD& dst, const PathD& src)
  {
    return _g2d.pathclipperd.clipPath(*this, dst, src, NULL);
  }

  //! @brief Clip transformed path @a src to @a dst.
  FOG_INLINE err_t clipPath(PathD& dst, const PathD& src, const TransformD& tr)
  {
    return _g2d.pathclipperd.clipPath(*this, dst, src, &tr);
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
  sysuint_t _lastIndex;
};

// ============================================================================
// [Fog::PathClipperT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D(PathClipper)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::PathClipperF, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::PathClipperD, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PATHCLIPPER_H
