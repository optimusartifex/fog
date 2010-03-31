// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PATTERN_H
#define _FOG_GRAPHICS_PATTERN_H

// [Dependencies]
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Gradient.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Matrix.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Fog::PatternData]
// ============================================================================

struct FOG_API PatternData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  PatternData();
  PatternData(const PatternData& other);
  ~PatternData();

  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  FOG_INLINE PatternData* ref() const
  {
    refCount.inc();
    return const_cast<PatternData*>(this);
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref()) delete this;
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  PatternData* copy();
  void deleteResources();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;

  //! @brief pattern type, see @c PATTERN_TYPE enum.
  uint32_t type;
  //! @brief Pattern spread, see @c SPREAD_TYPE enum.
  uint32_t spread;

  //! @brief Pattern transformation matrix.
  DoubleMatrix matrix;

  //! @brief Start and end points. These values have different meaning for
  //! each pattern type:
  //! - Null and Solid - Not used, should be zeros.
  //! - Texture - points[0] is texture offset (starting point).
  //! - Linear gradient - points[0 to 1] is start and end point.
  //! - Radial gradient - points[0] is circle center point, points[1] is focal point.
  //! - Conical gradient - points[0] is center point, points[1] is end point (for angle).
  DoublePoint points[3];
  //! @brief Used only for PATTERN_RADIAL_GRADIENT - circle radius.
  double radius;

  //! @brief Embedded objects in pattern, this can be solid color, raster
  //! texture data and gradient stops.
  union Objects
  {
    Static< Argb > argb;
    Static< Image > texture;
    Static< List<ArgbStop> > stops;
  } obj;
};

// ============================================================================
// [Fog::Pattern]
// ============================================================================

//! @brief Pattern can be used to define stroke or fill source when painting.
//!
//! Pattern is class that can be used to define these types of sources:
//! - Null - NOP (no source, no painting...).
//! - Solid color - Everything is painted by solid color.
//! - Texture - Raster texture is used as a source.
//! - Linear gradient - Linear gradient defined between two points.
//! - Radial gradient - Radial gradient defined by one circle, radius and focal point.
//! - Conical gradient - Conical gradient - atan() function is used.
struct FOG_API Pattern
{
  static Static<PatternData> sharedNull;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Pattern();
  Pattern(const Pattern& other);
  FOG_INLINE explicit Pattern(PatternData* d) : _d(d) {}
  ~Pattern();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }
  FOG_INLINE sysuint_t isDetached() const { return _d->refCount.get() == 1; }

  FOG_INLINE err_t detach() { return (_d->refCount.get() > 1) ? _detach() : ERR_OK; }
  err_t _detach();

  void free();

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _d->type; }
  err_t setType(uint32_t type);

  FOG_INLINE bool isNull() const { return _d->type == PATTERN_NULL; }
  FOG_INLINE bool isSolid() const { return _d->type == PATTERN_SOLID; }
  FOG_INLINE bool isTexture() const { return _d->type == PATTERN_TEXTURE; }
  FOG_INLINE bool isGradient() const { return (_d->type & PATTERN_GRADIENT_MASK) == PATTERN_GRADIENT_MASK; }
  FOG_INLINE bool isLinearGradient() const { return _d->type == PATTERN_LINEAR_GRADIENT; }
  FOG_INLINE bool isRadialGradient() const { return _d->type == PATTERN_RADIAL_GRADIENT; }
  FOG_INLINE bool isConicalGradient() const { return _d->type == PATTERN_CONICAL_GRADIENT; }

  // --------------------------------------------------------------------------
  // [Null]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Spread]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getSpread() const { return _d->spread; }
  err_t setSpread(uint32_t spread);

  // --------------------------------------------------------------------------
  // [Matrix]
  // --------------------------------------------------------------------------

  FOG_INLINE DoubleMatrix getMatrix() const { return _d->matrix; }
  err_t setMatrix(const FloatMatrix& matrix);
  err_t setMatrix(const DoubleMatrix& matrix);
  err_t resetMatrix();

  err_t translate(float x, float y, uint32_t order = MATRIX_PREPEND);
  err_t translate(double x, double y, uint32_t order = MATRIX_PREPEND);
  err_t rotate(float a, uint32_t order = MATRIX_PREPEND);
  err_t rotate(double a, uint32_t order = MATRIX_PREPEND);
  err_t scale(float x, float y, uint32_t order = MATRIX_PREPEND);
  err_t scale(double x, double y, uint32_t order = MATRIX_PREPEND);
  err_t skew(float x, float y, uint32_t order = MATRIX_PREPEND);
  err_t skew(double x, double y, uint32_t order = MATRIX_PREPEND);
  err_t transform(const FloatMatrix& m, uint32_t order = MATRIX_PREPEND);
  err_t transform(const DoubleMatrix& m, uint32_t order = MATRIX_PREPEND);

  // --------------------------------------------------------------------------
  // [Start Point / End Point]
  // --------------------------------------------------------------------------

  FOG_INLINE DoublePoint getStartPoint() const { return _d->points[0]; }
  FOG_INLINE DoublePoint getEndPoint() const { return _d->points[1]; }

  err_t setStartPoint(const IntPoint& pt);
  err_t setStartPoint(const FloatPoint& pt);
  err_t setStartPoint(const DoublePoint& pt);

  err_t setEndPoint(const IntPoint& pt);
  err_t setEndPoint(const FloatPoint& pt);
  err_t setEndPoint(const DoublePoint& pt);

  err_t setPoints(const IntPoint& startPt, const IntPoint& endPt);
  err_t setPoints(const FloatPoint& startPt, const FloatPoint& endPt);
  err_t setPoints(const DoublePoint& startPt, const DoublePoint& endPt);

  // --------------------------------------------------------------------------
  // [Solid]
  // --------------------------------------------------------------------------

  //! @brief Get pattern color (for @c PATTERN_SOLID type).
  //!
  //! If the pattern is not @c PATTERN_SOLID type, the Argb(0, 0, 0, 0) color is
  //! returned.
  Argb getColor() const;

  //! @brief Set pattern type to @c PATTERN_SOLID and its color to @a argb.
  err_t setColor(const Argb& argb);

  // --------------------------------------------------------------------------
  // [Texture]
  // --------------------------------------------------------------------------

  //! @brief Get pattern texture (for @c PATTERN_TEXTURE type).
  Image getTexture() const;

  //! @brief Set pattern type to @c PATTERN_TEXTURE and the texture to @a texture.
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_CONTEXT - If pattern is not @c PATTERN_RADIAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t setTexture(const Image& texture);

  // --------------------------------------------------------------------------
  // [Gradient]
  // --------------------------------------------------------------------------

  Gradient getGradient() const;
  err_t setGradient(const Gradient& gradient);

  //! @brief Get radial gradient radius (for @c PATTERN_RADIAL_GRADIENT type)
  FOG_INLINE double getRadius() const { return _d->radius; }

  //! @brief Set radial gradient radius to @a r.
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_CONTEXT - If pattern is not @c PATTERN_RADIAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t setRadius(float r);
  //! @overload
  err_t setRadius(double r);

  //! @brief Get generic gradient argb stops (for @c PATTERN_LINEAR_GRADIENT,
  //! @c PATTERN_RADIAL_GRADIENT and @c PATTERN_CONICAL_GRADIENT types).
  List<ArgbStop> getStops() const;

  //! @brief Set generic gradient argb stops to @a stops.
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_CONTEXT - If pattern is not @c PATTERN_LINEAR_GRADIENT,
  //!   @c PATTERN_RADIAL_GRADIENT or @c PATTERN_CONICAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t setStops(const List<ArgbStop>& stops);

  //! @brief Reset generic gradient argb stops (stops will be empty).
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_CONTEXT - If pattern is not @c PATTERN_LINEAR_GRADIENT,
  //!   @c PATTERN_RADIAL_GRADIENT or @c PATTERN_CONICAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t resetStops();

  //! @brief Add generic gradient argb stop @a stop to stops list.
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_CONTEXT - If pattern is not @c PATTERN_LINEAR_GRADIENT,
  //!   @c PATTERN_RADIAL_GRADIENT or @c PATTERN_CONICAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t addStop(const ArgbStop& stop);

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  Pattern& operator=(const Pattern& other);
  Pattern& operator=(const Argb& rgba);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(PatternData)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_PATTERN_H
