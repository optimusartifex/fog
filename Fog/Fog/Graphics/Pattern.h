// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PATTERN_H
#define _FOG_GRAPHICS_PATTERN_H

// [Dependencies]
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Gradient.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Matrix.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Painting
//! @{

// ============================================================================
// [Fog::PatternData]
// ============================================================================

//! Pattern data.
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

  void deleteResources();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;

  //! @brief pattern type, see @c PATTERN_TYPE enum.
  uint32_t type;
  //! @brief Pattern spread, see @c PATTERN_SPREAD enum.
  uint32_t spread;

  //! @brief Pattern transformation matrix.
  DoubleMatrix matrix;

  //! @internal
  struct GradientData
  {
    //! @brief Start and end points. These values have different meaning for
    //! each pattern type:
    //! - Linear gradient - points[0] is start point and points[1] is end point.
    //! - Radial gradient - points[0] is center point, points[1] is focal point.
    //! - Conical gradient - points[0] is center point, points[1] is end point (for angle).
    DoublePoint points[2];
    //! @brief Used only for PATTERN_TYPE_RADIAL_GRADIENT - circle radius.
    double radius;
  };

  //! @internal
  struct TextureData
  {
    IntRect area;
  };

  //! @internal
  //!
  //! @brief Embedded objects in pattern, this can be solid color, raster
  //! texture data and gradient stops.
  union ObjectInst
  {
    Static< Argb > argb;
    Static< Image > texture;
    Static< List<ArgbStop> > stops;
  } obj;

  //! @internal
  union ObjectData
  {
    Static< TextureData > texture;
    Static< GradientData > gradient;
  };

  //! @brief Data for all pattern types.
  ObjectData data;
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
  static Static<PatternData> _dnull;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Pattern();
  Pattern(const Pattern& other);
  explicit Pattern(const Argb& argb);
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

  FOG_INLINE bool isNull() const { return _d->type == PATTERN_TYPE_NONE; }
  FOG_INLINE bool isSolid() const { return _d->type == PATTERN_TYPE_SOLID; }
  FOG_INLINE bool isTexture() const { return _d->type == PATTERN_TYPE_TEXTURE; }
  FOG_INLINE bool isGradient() const { return (_d->type & PATTERN_TYPE_IS_GRADIENT) == PATTERN_TYPE_IS_GRADIENT; }
  FOG_INLINE bool isLinearGradient() const { return _d->type == PATTERN_TYPE_LINEAR_GRADIENT; }
  FOG_INLINE bool isRadialGradient() const { return _d->type == PATTERN_TYPE_RADIAL_GRADIENT; }
  FOG_INLINE bool isConicalGradient() const { return _d->type == PATTERN_TYPE_CONICAL_GRADIENT; }

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
  // [Solid]
  // --------------------------------------------------------------------------

  //! @brief Get pattern color (for @c PATTERN_TYPE_SOLID type).
  //!
  //! If the pattern is not @c PATTERN_TYPE_SOLID type, the Argb(0, 0, 0, 0) color is
  //! returned.
  Argb getColor() const;

  //! @brief Set pattern type to @c PATTERN_TYPE_SOLID and its color to @a argb.
  err_t setColor(const Argb& argb);

  // --------------------------------------------------------------------------
  // [Texture]
  // --------------------------------------------------------------------------

  //! @brief Get pattern texture (for @c PATTERN_TYPE_TEXTURE type).
  Image getTexture() const;

  //! @brief Get texture area (for @c PATTERN_TYPE_TEXTURE type).
  IntRect getTextureArea() const;

  //! @brief Set pattern type to @c PATTERN_TYPE_TEXTURE and the texture to @a texture.
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_OBJECT - If pattern is not @c PATTERN_TYPE_RADIAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t setTexture(const Image& texture);
  //! @overload
  err_t setTexture(const Image& texture, const IntRect& textureArea);

  // --------------------------------------------------------------------------
  // [Gradient]
  // --------------------------------------------------------------------------

  Gradient getGradient() const;
  err_t setGradient(const Gradient& gradient);

  DoublePoint getStartPoint() const;
  DoublePoint getEndPoint() const;

  err_t setStartPoint(const IntPoint& pt);
  err_t setStartPoint(const FloatPoint& pt);
  err_t setStartPoint(const DoublePoint& pt);

  err_t setEndPoint(const IntPoint& pt);
  err_t setEndPoint(const FloatPoint& pt);
  err_t setEndPoint(const DoublePoint& pt);

  err_t setPoints(const IntPoint& startPt, const IntPoint& endPt);
  err_t setPoints(const FloatPoint& startPt, const FloatPoint& endPt);
  err_t setPoints(const DoublePoint& startPt, const DoublePoint& endPt);

  //! @brief Get radial gradient radius (for @c PATTERN_TYPE_RADIAL_GRADIENT type)
  double getRadius() const;

  //! @brief Set radial gradient radius to @a r.
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_OBJECT - If pattern is not @c PATTERN_TYPE_RADIAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t setRadius(float r);
  //! @overload
  err_t setRadius(double r);

  //! @brief Get generic gradient argb stops (for @c PATTERN_TYPE_LINEAR_GRADIENT,
  //! @c PATTERN_TYPE_RADIAL_GRADIENT and @c PATTERN_TYPE_CONICAL_GRADIENT types).
  List<ArgbStop> getStops() const;

  //! @brief Set generic gradient argb stops to @a stops.
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_OJBECT - If pattern is not @c PATTERN_TYPE_LINEAR_GRADIENT,
  //!   @c PATTERN_TYPE_RADIAL_GRADIENT or @c PATTERN_TYPE_CONICAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t setStops(const List<ArgbStop>& stops);

  //! @brief Reset generic gradient argb stops (stops will be empty).
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_OBJECT - If pattern is not @c PATTERN_TYPE_LINEAR_GRADIENT,
  //!   @c PATTERN_TYPE_RADIAL_GRADIENT or @c PATTERN_TYPE_CONICAL_GRADIENT.
  //! - @c ERR_RT_OUT_OF_MEMORY - If memory allocation failed.
  err_t resetStops();

  //! @brief Add generic gradient argb stop @a stop to stops list.
  //!
  //! @return
  //! - @c ERR_OK - If function succeed.
  //! - @c ERR_RT_INVALID_OBJECT - If pattern is not @c PATTERN_TYPE_LINEAR_GRADIENT,
  //!   @c PATTERN_TYPE_RADIAL_GRADIENT or @c PATTERN_TYPE_CONICAL_GRADIENT.
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

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PATTERN_H
