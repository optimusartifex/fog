// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_PATTERN_H
#define _FOG_G2D_SOURCE_PATTERN_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/ColorStop.h>
#include <Fog/G2d/Source/ColorStopCache.h>
#include <Fog/G2d/Source/ColorStopList.h>
#include <Fog/G2d/Source/ConicalGradient.h>
#include <Fog/G2d/Source/Gradient.h>
#include <Fog/G2d/Source/LinearGradient.h>
#include <Fog/G2d/Source/RadialGradient.h>
#include <Fog/G2d/Source/RectangularGradient.h>
#include <Fog/G2d/Source/Texture.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

// ============================================================================
// [Fog::PatternDataF]
// ============================================================================

//! @brief Pattern data (float).
struct FOG_NO_EXPORT PatternDataF
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE PatternDataF* addRef() const
  {
    reference.inc();
    return const_cast<PatternDataF*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
    {
      destroy();
      MemMgr::free(this);
    }
  }

  // --------------------------------------------------------------------------
  // [Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void destroy()
  {
    if (type == PATTERN_TYPE_TEXTURE)
      texture.destroy();
    else if (type == PATTERN_TYPE_GRADIENT)
      gradient.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Pattern type.
  uint32_t type;

  union
  {
    //! @brief Color data.
    Static<Color> color;
    //! @brief Texture data.
    Static<Texture> texture;
    //! @brief Gradient data.
    Static<GradientF> gradient;
  };

  //! @brief Transformation matrix.
  TransformF transform;
};

// ============================================================================
// [Fog::PatternDataD]
// ============================================================================

//! @brief Pattern data (double).
struct FOG_NO_EXPORT PatternDataD
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE PatternDataD* addRef() const
  {
    reference.inc();
    return const_cast<PatternDataD*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
    {
      destroy();
      MemMgr::free(this);
    }
  }

  // --------------------------------------------------------------------------
  // [Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void destroy()
  {
    if (type == PATTERN_TYPE_TEXTURE)
      texture.destroy();
    else if (type == PATTERN_TYPE_GRADIENT)
      gradient.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Pattern type.
  uint32_t type;

  union
  {
    //! @brief Color data.
    Static<Color> color;
    //! @brief Texture data.
    Static<Texture> texture;
    //! @brief Gradient data.
    Static<GradientD> gradient;
  };

  //! @brief Transformation matrix.
  TransformD transform;
};

// ============================================================================
// [Fog::PatternF]
// ============================================================================

struct FOG_API PatternF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  PatternF();
  PatternF(const PatternF& other);

  explicit PatternF(const ArgbBase32& argb32);
  explicit PatternF(const Color& color);
  explicit PatternF(const Texture& texture);
  explicit PatternF(const GradientF& gradient);
  explicit PatternF(const GradientD& gradient);
  explicit PatternF(const PatternD& other);

  explicit FOG_INLINE PatternF(PatternDataF* d) : _d(d) {}
  ~PatternF();

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }

  FOG_INLINE bool isDetached() const { return getReference() == 1; }
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }

  err_t _detach();

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _d->type; }

  FOG_INLINE bool isNone() const { return (_d->type == PATTERN_TYPE_NONE); }
  FOG_INLINE bool isColor() const { return (_d->type == PATTERN_TYPE_COLOR); }
  FOG_INLINE bool isGradient() const { return (_d->type == PATTERN_TYPE_GRADIENT); }
  FOG_INLINE bool isTexture() const { return (_d->type == PATTERN_TYPE_TEXTURE); }

  //! @overload
  FOG_INLINE bool isGradient(uint32_t gradientType) const
  { return (_d->type == PATTERN_TYPE_GRADIENT) && _d->gradient->getGradientType() == gradientType; }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  void clear();
  void reset();

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  err_t getTransform(TransformF& tr) const;
  err_t getTransform(TransformD& tr) const;

  err_t setTransform(const TransformF& tr);
  err_t setTransform(const TransformD& tr);

  err_t resetTransform();

  err_t _transform(uint32_t transformOp, const void* params);

  // --------------------------------------------------------------------------
  // [Transform - Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t translate(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_TRANSLATE | (order << 4), &p);
  }

  // --------------------------------------------------------------------------
  // [Transform - Scale]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t scale(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_SCALE | (order << 4), &p);
  }

  // --------------------------------------------------------------------------
  // [Transform - Skew]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t skew(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_SKEW | (order << 4), &p);
  }

  // --------------------------------------------------------------------------
  // [Transform - Rotate]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t rotate(float angle, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_ROTATE | (order << 4), &angle);
  }

  FOG_INLINE err_t rotate(float angle, const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    float params[3] = { angle, p.x, p.y };
    return _transform(TRANSFORM_OP_ROTATE_PT | (order << 4), params);
  }

  // --------------------------------------------------------------------------
  // [Transform - Multiply]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t transform(const TransformF& tr, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_MULTIPLY | (order << 4), &tr);
  }

  // --------------------------------------------------------------------------
  // [Color]
  // --------------------------------------------------------------------------

  //! @brief Get pattern color (for @c PATTERN_TYPE_COLOR).
  err_t getArgb32(ArgbBase32& argb32) const;
  //! @brief Get pattern color (for @c PATTERN_TYPE_COLOR).
  err_t getColor(Color& color) const;

  //! @brief Set pattern to @a argb32.
  err_t setArgb32(const ArgbBase32& argb32);
  //! @brief Set pattern to @a color.
  err_t setColor(const Color& color);

  // --------------------------------------------------------------------------
  // [Texture]
  // --------------------------------------------------------------------------

  //! @brief Get texture.
  err_t getTexture(Texture& texture) const;

  //! @brief Set texture.
  err_t setTexture(const Texture& texture);

  // --------------------------------------------------------------------------
  // [Gradient]
  // --------------------------------------------------------------------------

  err_t _getGradientF(uint32_t gradientType, GradientF& gr) const;
  err_t _getGradientD(uint32_t gradientType, GradientD& gr) const;

  FOG_INLINE err_t getGradient(GradientF& gr) const { return _getGradientF(0xFFFFFFFF, gr); }
  FOG_INLINE err_t getGradient(GradientD& gr) const { return _getGradientD(0xFFFFFFFF, gr); }

  FOG_INLINE err_t getGradient(LinearGradientF& gr) const { return _getGradientF(GRADIENT_TYPE_LINEAR, gr); }
  FOG_INLINE err_t getGradient(LinearGradientD& gr) const { return _getGradientD(GRADIENT_TYPE_LINEAR, gr); }

  FOG_INLINE err_t getGradient(RadialGradientF& gr) const { return _getGradientF(GRADIENT_TYPE_RADIAL, gr); }
  FOG_INLINE err_t getGradient(RadialGradientD& gr) const { return _getGradientD(GRADIENT_TYPE_RADIAL, gr); }

  FOG_INLINE err_t getGradient(ConicalGradientF& gr) const { return _getGradientF(GRADIENT_TYPE_CONICAL, gr); }
  FOG_INLINE err_t getGradient(ConicalGradientD& gr) const { return _getGradientD(GRADIENT_TYPE_CONICAL, gr); }

  FOG_INLINE err_t getGradient(RectangularGradientF& gr) const { return _getGradientF(GRADIENT_TYPE_RECTANGULAR, gr); }
  FOG_INLINE err_t getGradient(RectangularGradientD& gr) const { return _getGradientD(GRADIENT_TYPE_RECTANGULAR, gr); }

  err_t setGradient(const GradientF& gr);
  err_t setGradient(const GradientD& gr);

  // --------------------------------------------------------------------------
  // [Pattern]
  // --------------------------------------------------------------------------

  err_t setPattern(const PatternF& other);
  err_t setPattern(const PatternD& other);

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PatternF& operator=(const Color& color) { setColor(color); return *this; }
  FOG_INLINE PatternF& operator=(const Texture& tex) { setTexture(tex); return *this; }

  FOG_INLINE PatternF& operator=(const GradientF& gr) { setGradient(gr); return *this; }
  FOG_INLINE PatternF& operator=(const GradientD& gr) { setGradient(gr); return *this; }

  FOG_INLINE PatternF& operator=(const PatternF& other) { setPattern(other); return *this; }
  FOG_INLINE PatternF& operator=(const PatternD& other) { setPattern(other); return *this; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Static<PatternDataF> _dnull;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(PatternDataF)
};

// ============================================================================
// [Fog::PatternD]
// ============================================================================

struct FOG_API PatternD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  PatternD();
  PatternD(const PatternD& other);

  explicit PatternD(const ArgbBase32& argb32);
  explicit PatternD(const Color& color);
  explicit PatternD(const Texture& texture);
  explicit PatternD(const GradientF& gradient);
  explicit PatternD(const GradientD& gradient);
  explicit PatternD(const PatternF& other);

  explicit FOG_INLINE PatternD(PatternDataD* d) : _d(d) {}
  ~PatternD();

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }

  FOG_INLINE bool isDetached() const { return getReference() == 1; }
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }

  err_t _detach();

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _d->type; }

  FOG_INLINE bool isNone() const { return (_d->type == PATTERN_TYPE_NONE); }
  FOG_INLINE bool isColor() const { return (_d->type == PATTERN_TYPE_COLOR); }
  FOG_INLINE bool isGradient() const { return (_d->type == PATTERN_TYPE_GRADIENT); }
  FOG_INLINE bool isTexture() const { return (_d->type == PATTERN_TYPE_TEXTURE); }

  //! @overload
  FOG_INLINE bool isGradient(uint32_t gradientType) const
  { return (_d->type == PATTERN_TYPE_GRADIENT) && _d->gradient->getGradientType() == gradientType; }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  void clear();
  void reset();

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  err_t getTransform(TransformF& tr) const;
  err_t getTransform(TransformD& tr) const;

  err_t setTransform(const TransformF& tr);
  err_t setTransform(const TransformD& tr);

  err_t resetTransform();

  err_t _transform(uint32_t transformOp, const void* params);

  // --------------------------------------------------------------------------
  // [Transform - Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t translate(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_TRANSLATE | (order << 4), &p);
  }

  // --------------------------------------------------------------------------
  // [Transform - Scale]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t scale(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_SCALE | (order << 4), &p);
  }

  // --------------------------------------------------------------------------
  // [Transform - Skew]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t skew(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_SKEW | (order << 4), &p);
  }

  // --------------------------------------------------------------------------
  // [Transform - Rotate]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t rotate(double angle, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_ROTATE | (order << 4), &angle);
  }

  FOG_INLINE err_t rotate(double angle, const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    double params[3] = { angle, p.x, p.y };
    return _transform(TRANSFORM_OP_ROTATE_PT | (order << 4), params);
  }

  // --------------------------------------------------------------------------
  // [Transform - Multiply]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t transform(const TransformD& tr, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_MULTIPLY | (order << 4), &tr);
  }

  // --------------------------------------------------------------------------
  // [Color]
  // --------------------------------------------------------------------------

  //! @brief Get pattern color (for @c PATTERN_TYPE_COLOR).
  err_t getArgb32(ArgbBase32& argb32) const;
  //! @brief Get pattern color (for @c PATTERN_TYPE_COLOR).
  err_t getColor(Color& color) const;

  //! @brief Set pattern to @a argb32.
  err_t setArgb32(const ArgbBase32& argb32);
  //! @brief Set pattern to @a color.
  err_t setColor(const Color& color);

  // --------------------------------------------------------------------------
  // [Texture]
  // --------------------------------------------------------------------------

  //! @brief Get texture.
  err_t getTexture(Texture& texture) const;

  //! @brief Set texture.
  err_t setTexture(const Texture& texture);

  // --------------------------------------------------------------------------
  // [Gradient]
  // --------------------------------------------------------------------------

  err_t _getGradientF(uint32_t gradientType, GradientF& gr) const;
  err_t _getGradientD(uint32_t gradientType, GradientD& gr) const;

  FOG_INLINE err_t getGradient(GradientF& gr) const { return _getGradientF(0xFFFFFFFF, gr); }
  FOG_INLINE err_t getGradient(GradientD& gr) const { return _getGradientD(0xFFFFFFFF, gr); }

  FOG_INLINE err_t getGradient(LinearGradientF& gr) const { return _getGradientF(GRADIENT_TYPE_LINEAR, gr); }
  FOG_INLINE err_t getGradient(LinearGradientD& gr) const { return _getGradientD(GRADIENT_TYPE_LINEAR, gr); }

  FOG_INLINE err_t getGradient(RadialGradientF& gr) const { return _getGradientF(GRADIENT_TYPE_RADIAL, gr); }
  FOG_INLINE err_t getGradient(RadialGradientD& gr) const { return _getGradientD(GRADIENT_TYPE_RADIAL, gr); }

  FOG_INLINE err_t getGradient(ConicalGradientF& gr) const { return _getGradientF(GRADIENT_TYPE_CONICAL, gr); }
  FOG_INLINE err_t getGradient(ConicalGradientD& gr) const { return _getGradientD(GRADIENT_TYPE_CONICAL, gr); }

  FOG_INLINE err_t getGradient(RectangularGradientF& gr) const { return _getGradientF(GRADIENT_TYPE_RECTANGULAR, gr); }
  FOG_INLINE err_t getGradient(RectangularGradientD& gr) const { return _getGradientD(GRADIENT_TYPE_RECTANGULAR, gr); }

  err_t setGradient(const GradientF& gr);
  err_t setGradient(const GradientD& gr);

  // --------------------------------------------------------------------------
  // [Pattern]
  // --------------------------------------------------------------------------

  err_t setPattern(const PatternF& other);
  err_t setPattern(const PatternD& other);

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PatternD& operator=(const Color& color) { setColor(color); return *this; }
  FOG_INLINE PatternD& operator=(const Texture& tex) { setTexture(tex); return *this; }

  FOG_INLINE PatternD& operator=(const GradientF& gr) { setGradient(gr); return *this; }
  FOG_INLINE PatternD& operator=(const GradientD& gr) { setGradient(gr); return *this; }

  FOG_INLINE PatternD& operator=(const PatternF& other) { setPattern(other); return *this; }
  FOG_INLINE PatternD& operator=(const PatternD& other) { setPattern(other); return *this; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Static<PatternDataD> _dnull;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(PatternDataD)
};

// ============================================================================
// [Fog::PatternT<>]
// ============================================================================

_FOG_NUM_T(Pattern)
_FOG_NUM_F(Pattern)
_FOG_NUM_D(Pattern)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SOURCE_PATTERN_H
