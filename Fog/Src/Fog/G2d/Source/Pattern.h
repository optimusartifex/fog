// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_PATTERN_H
#define _FOG_G2D_SOURCE_PATTERN_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Math.h>
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
// [Fog::PatternData]
// ============================================================================

struct FOG_NO_EXPORT PatternData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE PatternData* addRef() const
  {
    if (vType != VAR_TYPE_NULL)
      reference.inc();

    return const_cast<PatternData*>(this);
  }

  FOG_INLINE void release()
  {
    if (vType != VAR_TYPE_NULL && reference.deref())
      fog_api.pattern_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // ${VAR:BEGIN}
  //
  // This data-object is binary compatible with the VarData header in the first
  // form called - "implicitly shared class". The members must be binary
  // compatible with the header below:
  //
  // +==============+============+============================================+
  // | Size         | Name       | Description / Purpose                      |
  // +==============+============+============================================+
  // | size_t       | reference  | Atomic reference count, can be managed by  |
  // |              |            | VarData without calling container specific |
  // |              |            | methods.                                   |
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | vType      | Variable type and flags.                   |
  // +==============+============+============================================+
  //
  // ${VAR:END}

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

  //! @brief Pattern type, see @ref PATTERN_TYPE.
  uint32_t patternType;
};

// ============================================================================
// [Fog::PatternColorData]
// ============================================================================

struct FOG_NO_EXPORT PatternColorData : public PatternData
{
  Static<Color> color;
};

// ============================================================================
// [Fog::PatternTextureDataF]
// ============================================================================

struct FOG_NO_EXPORT PatternTextureDataF : public PatternData
{
  Static<Texture> texture;
  Static<TransformF> transform;
};

// ============================================================================
// [Fog::PatternTextureDataD]
// ============================================================================

struct FOG_NO_EXPORT PatternTextureDataD : public PatternData
{
  Static<Texture> texture;
  Static<TransformD> transform;
};

// ============================================================================
// [Fog::PatternGradientDataF]
// ============================================================================

struct FOG_NO_EXPORT PatternGradientDataF : public PatternData
{
  Static<GradientF> gradient;
  Static<TransformF> transform;
};

// ============================================================================
// [Fog::PatternGradientDataD]
// ============================================================================

struct FOG_NO_EXPORT PatternGradientDataD : public PatternData
{
  Static<GradientD> gradient;
  Static<TransformD> transform;
};

// ============================================================================
// [Fog::Pattern]
// ============================================================================

struct FOG_NO_EXPORT Pattern
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Pattern()
  {
    fog_api.pattern_ctor(this);
  }

  FOG_INLINE Pattern(const Pattern& other)
  {
    fog_api.pattern_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE Pattern(Pattern&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  explicit FOG_INLINE Pattern(const ArgbBase32& argb32)
  {
    fog_api.pattern_ctorArgb32(this, &argb32);
  }

  explicit FOG_INLINE Pattern(const Color& color)
  {
    fog_api.pattern_ctorColor(this, &color);
  }

  explicit FOG_INLINE Pattern(const Texture& texture)
  {
    fog_api.pattern_ctorTextureF(this, &texture, NULL);
  }

  explicit FOG_INLINE Pattern(const GradientF& gradient)
  {
    fog_api.pattern_ctorGradientF(this, &gradient, NULL);
  }

  explicit FOG_INLINE Pattern(const GradientD& gradient)
  {
    fog_api.pattern_ctorGradientD(this, &gradient, NULL);
  }

  FOG_INLINE Pattern(const Texture& texture, const TransformF& transform)
  {
    fog_api.pattern_ctorTextureF(this, &texture, &transform);
  }

  FOG_INLINE Pattern(const Texture& texture, const TransformD& transform)
  {
    fog_api.pattern_ctorTextureD(this, &texture, &transform);
  }

  FOG_INLINE Pattern(const GradientF& gradient, const TransformF& transform)
  {
    fog_api.pattern_ctorGradientF(this, &gradient, &transform);
  }

  FOG_INLINE Pattern(const GradientD& gradient, const TransformD& transform)
  {
    fog_api.pattern_ctorGradientD(this, &gradient, &transform);
  }

  explicit FOG_INLINE Pattern(PatternData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~Pattern()
  {
    fog_api.pattern_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE bool isDetached() const { return getReference() == 1; }

  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  FOG_INLINE err_t _detach() { return fog_api.pattern_detach(this); }

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getVarType() const { return _d->vType; }
  FOG_INLINE uint32_t getPatternType() const { return _d->patternType; }

  FOG_INLINE bool isNull() const { return getPatternType() == PATTERN_TYPE_NULL; }
  FOG_INLINE bool isColor() const { return getPatternType() == PATTERN_TYPE_COLOR; }
  FOG_INLINE bool isTexture() const { return getPatternType() == PATTERN_TYPE_TEXTURE; }
  FOG_INLINE bool isGradient() const { return getPatternType() == PATTERN_TYPE_GRADIENT; }

  // TODO:
  //! @overload
  //FOG_INLINE bool isGradient(uint32_t gradientType) const
  //{
  //  return (_d->type == PATTERN_TYPE_GRADIENT) && _d->gradient->getGradientType() == gradientType;
  //}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get pattern color (for @c PATTERN_TYPE_COLOR).
  FOG_INLINE err_t getColor(ArgbBase32& argb32) const
  {
    return fog_api.pattern_getArgb32(this, &argb32);
  }

  //! @brief Get pattern color (for @c PATTERN_TYPE_COLOR)
  FOG_INLINE err_t getColor(Color& color) const
  {
    return fog_api.pattern_getColor(this, &color);
  }

  //! @brief Get texture.
  FOG_INLINE err_t getTexture(Texture& texture) const
  {
    return fog_api.pattern_getTexture(this, &texture);
  }

  FOG_INLINE err_t _getGradientF(uint32_t targetType, GradientF& gradient) const
  {
    return fog_api.pattern_getGradientF(this, targetType, &gradient);
  }

  FOG_INLINE err_t _getGradientD(uint32_t targetType, GradientD& gradient) const
  {
    return fog_api.pattern_getGradientD(this, targetType, &gradient);
  }

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

  FOG_INLINE err_t setPattern(const Pattern& other)
  {
    return fog_api.pattern_copy(this, &other);
  }

  FOG_INLINE err_t getTransform(TransformF& tr) const
  {
    return fog_api.pattern_getTransformF(this, &tr);
  }

  FOG_INLINE err_t getTransform(TransformD& tr) const
  {
    return fog_api.pattern_getTransformD(this, &tr);
  }

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  //! @brief Create a color pattern and set color to @a argb32.
  FOG_INLINE err_t createColor(const ArgbBase32& argb32)
  {
    return fog_api.pattern_createArgb32(this, &argb32);
  }

  //! @brief Create a color pattern and set color to @a color.
  FOG_INLINE err_t createColor(const Color& color)
  {
    return fog_api.pattern_createColor(this, &color);
  }

  //! @brief Create a texture pattern and set texture to @a texture.
  FOG_INLINE err_t createTexture(const Texture& texture)
  {
    return fog_api.pattern_createTextureF(this, &texture, NULL);
  }

  //! @brief Create a texture pattern and set texture to @a texture and transform
  //! to @a tr.
  FOG_INLINE err_t createTexture(const Texture& texture, const TransformF& tr)
  {
    return fog_api.pattern_createTextureF(this, &texture, &tr);
  }

  //! @brief Create a texture pattern and set texture to @a texture and transform
  //! to @a tr.
  FOG_INLINE err_t createTexture(const Texture& texture, const TransformD& tr)
  {
    return fog_api.pattern_createTextureD(this, &texture, &tr);
  }

  //! @brief Create a gradient pattern and set gradient to @a gradient.
  FOG_INLINE err_t createGradient(const GradientF& gradient)
  {
    return fog_api.pattern_createGradientF(this, &gradient, NULL);
  }

  //! @brief Create a gradient pattern and set gradient to @a gradient and transform
  //! to @a tr.
  FOG_INLINE err_t createGradient(const GradientF& gradient, const TransformF& tr)
  {
    return fog_api.pattern_createGradientF(this, &gradient, &tr);
  }

  //! @brief Create a gradient pattern and set gradient to @a gradient.
  FOG_INLINE err_t createGradient(const GradientD& gradient)
  {
    return fog_api.pattern_createGradientD(this, &gradient, NULL);
  }

  //! @brief Create a gradient pattern and set gradient to @a gradient and transform
  //! to @a tr.
  FOG_INLINE err_t createGradient(const GradientD& gradient, const TransformD& tr)
  {
    return fog_api.pattern_createGradientD(this, &gradient, &tr);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setTransform(const TransformF& tr)
  {
    return fog_api.pattern_setTransformF(this, &tr);
  }

  FOG_INLINE err_t setTransform(const TransformD& tr)
  {
    return fog_api.pattern_setTransformD(this, &tr);
  }

  FOG_INLINE err_t _transform(uint32_t transformOp, const void* params)
  {
    return fog_api.pattern_applyTransform(this, transformOp, params);
  }

  FOG_INLINE err_t resetTransform()
  {
    return fog_api.pattern_resetTransform(this);
  }

  // --------------------------------------------------------------------------
  // [Transform - Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t translate(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_TRANSLATEF | (order << 4), &p);
  }

  FOG_INLINE err_t translate(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_TRANSLATED | (order << 4), &p);
  }

  // --------------------------------------------------------------------------
  // [Transform - Scale]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t scale(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_SCALEF | (order << 4), &p);
  }

  FOG_INLINE err_t scale(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_SCALED | (order << 4), &p);
  }

  // --------------------------------------------------------------------------
  // [Transform - Skew]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t skew(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_SKEWF | (order << 4), &p);
  }

  FOG_INLINE err_t skew(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_SKEWD | (order << 4), &p);
  }

  // --------------------------------------------------------------------------
  // [Transform - Rotate]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t rotate(float angle, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_ROTATEF | (order << 4), &angle);
  }

  FOG_INLINE err_t rotate(double angle, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_ROTATED | (order << 4), &angle);
  }

  FOG_INLINE err_t rotate(float angle, const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    float params[3] = { angle, p.x, p.y };
    return _transform(TRANSFORM_OP_ROTATE_POINTF | (order << 4), params);
  }

  FOG_INLINE err_t rotate(double angle, const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    double params[3] = { angle, p.x, p.y };
    return _transform(TRANSFORM_OP_ROTATE_POINTD | (order << 4), params);
  }

  // --------------------------------------------------------------------------
  // [Transform - Multiply]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t transform(const TransformF& tr, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_MULTIPLYF | (order << 4), &tr);
  }

  FOG_INLINE err_t transform(const TransformD& tr, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _transform(TRANSFORM_OP_MULTIPLYD | (order << 4), &tr);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.pattern_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Pattern& other) const
  {
    return fog_api.pattern_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Pattern& operator=(const Pattern& other)
  {
    fog_api.pattern_copy(this, &other); return *this;
  }

  FOG_INLINE bool operator==(const Pattern& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Pattern& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const Pattern& null() { return *fog_api.pattern_oNull; }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const Pattern* a, const Pattern* b)
  {
    return fog_api.pattern_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.pattern_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(PatternData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SOURCE_PATTERN_H
