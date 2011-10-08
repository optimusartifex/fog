// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_TRANSFORM_H
#define _FOG_G2D_GEOMETRY_TRANSFORM_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/HashUtil.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::ParallelogramParamsF]
// ============================================================================

struct FOG_NO_EXPORT ParallelogramParamsF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ParallelogramParamsF(
    const PointF& dst0, const PointF& dst1, const PointF& dst2,
    const PointF& src0, const PointF& src1, const PointF& src2)
  {
    dst[0] = dst0; dst[1] = dst1; dst[2] = dst2;
    src[0] = src0; src[1] = src1; src[2] = src2;
  }

  FOG_INLINE ParallelogramParamsF(
    float dx0, float dy0, float dx1, float dy1, float dx2, float dy2,
    float sx0, float sy0, float sx1, float sy1, float sx2, float sy2)
  {
    dst[0].set(dx0, dy0); dst[1].set(dx1, dy1); dst[2].set(dx2, dy2);
    src[0].set(sx0, sy0); src[1].set(sx1, sy1); src[2].set(sx2, sy2);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF dst[3];
  PointF src[3];
};

// ============================================================================
// [Fog::ParallelogramParamsD]
// ============================================================================

struct FOG_NO_EXPORT ParallelogramParamsD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ParallelogramParamsD(
    const PointD& dst0, const PointD& dst1, const PointD& dst2,
    const PointD& src0, const PointD& src1, const PointD& src2)
  {
    dst[0] = dst0; dst[1] = dst1; dst[2] = dst2;
    src[0] = src0; src[1] = src1; src[2] = src2;
  }

  FOG_INLINE ParallelogramParamsD(
    double dx0, double dy0, double dx1, double dy1, double dx2, double dy2,
    double sx0, double sy0, double sx1, double sy1, double sx2, double sy2)
  {
    dst[0].set(dx0, dy0); dst[1].set(dx1, dy1); dst[2].set(dx2, dy2);
    src[0].set(sx0, sy0); src[1].set(sx1, sy1); src[2].set(sx2, sy2);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointD dst[3];
  PointD src[3];
};

// ============================================================================
// [Fog::QuadToQuadParamsF]
// ============================================================================

struct FOG_NO_EXPORT QuadToQuadParamsF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE QuadToQuadParamsF(
    const PointF& dst0, const PointF& dst1, const PointF& dst2, const PointF& dst3,
    const PointF& src0, const PointF& src1, const PointF& src2, const PointF& src3)
  {
    dst[0] = dst0; dst[1] = dst1; dst[2] = dst2; dst[3] = dst3;
    src[0] = src0; src[1] = src1; src[2] = src2; src[3] = src3;
  }

  FOG_INLINE QuadToQuadParamsF(
    float dx0, float dy0, float dx1, float dy1, float dx2, float dy2, float dx3, float dy3,
    float sx0, float sy0, float sx1, float sy1, float sx2, float sy2, float sx3, float sy3)
  {
    dst[0].set(dx0, dy0); dst[1].set(dx1, dy1); dst[2].set(dx2, dy2); dst[3].set(dx3, dy3);
    src[0].set(sx0, sy0); src[1].set(sx1, sy1); src[2].set(sx2, sy2); src[3].set(sx3, sy3);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointF dst[4];
  PointF src[4];
};

// ============================================================================
// [Fog::QuadToQuadParamsD]
// ============================================================================

struct FOG_NO_EXPORT QuadToQuadParamsD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE QuadToQuadParamsD(
    const PointD& dst0, const PointD& dst1, const PointD& dst2, const PointD& dst3,
    const PointD& src0, const PointD& src1, const PointD& src2, const PointD& src3)
  {
    dst[0] = dst0; dst[1] = dst1; dst[2] = dst2; dst[3] = dst3;
    src[0] = src0; src[1] = src1; src[2] = src2; src[3] = src3;
  }

  FOG_INLINE QuadToQuadParamsD(
    double dx0, double dy0, double dx1, double dy1, double dx2, double dy2, double dx3, double dy3,
    double sx0, double sy0, double sx1, double sy1, double sx2, double sy2, double sx3, double sy3)
  {
    dst[0].set(dx0, dy0); dst[1].set(dx1, dy1); dst[2].set(dx2, dy2); dst[3].set(dx3, dy3);
    src[0].set(sx0, sy0); src[1].set(sx1, sy1); src[2].set(sx2, sy2); src[3].set(sx3, sy3);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointD dst[4];
  PointD src[4];
};

// ============================================================================
// [Fog::TransformF]
// ============================================================================

//! @brief Matrix (float).
struct FOG_NO_EXPORT TransformF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create an identity transform.
  FOG_INLINE TransformF()
  {
    _setData(TRANSFORM_TYPE_IDENTITY,
      1.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 1.0f);
  }

  //! @brief Create a copy of @a other transform.
  FOG_INLINE TransformF(const TransformF& other)
  {
    setTransform(other);
  }

  //! @brief Create an affine transform.
  FOG_INLINE TransformF(
    float m00, float m01,
    float m10, float m11,
    float m20, float m21)
  {
    _setData(TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY,
      m00, m01, 0.0f,
      m10, m11, 0.0f,
      m20, m21, 1.0f);
  }

  //! @brief Create a projection transform.
  FOG_INLINE TransformF(
    float m00, float m01, float m02,
    float m10, float m11, float m12,
    float m20, float m21, float m22)
  {
    _setData(TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY,
      m00, m01, m02,
      m10, m11, m12,
      m20, m21, m22);
  }

  explicit FOG_INLINE TransformF(_Uninitialized) {}

  explicit FOG_INLINE TransformF(const TransformD& other)
  {
    setTransform(other);
  }

  //! @brief Create custom transform from @a data[9].
  explicit FOG_INLINE TransformF(const float* data)
  {
    _setData(TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY,
      data[0], data[1], data[2],
      data[3], data[4], data[5],
      data[6], data[7], data[8]);
  }

  // --------------------------------------------------------------------------
  // [Construction - Static]
  // --------------------------------------------------------------------------

  //! @brief Create translation transform.
  static FOG_INLINE TransformF fromTranslation(float x, float y)
  {
    TransformF result(UNINITIALIZED);
    float params[2] = { x, y };
    _api.transformf_create(&result, TRANSFORM_CREATE_TRANSLATION, params);
    return result;
  }

  //! @brief Create translation transform.
  static FOG_INLINE TransformF fromTranslation(const PointF& p)
  {
    TransformF result(UNINITIALIZED);
    _api.transformf_create(&result, TRANSFORM_CREATE_TRANSLATION, reinterpret_cast<const float*>(&p));
    return result;
  }

  //! @brief Create scaling transform.
  static FOG_INLINE TransformF fromScaling(float x, float y)
  {
    TransformF result(UNINITIALIZED);
    float params[2] = { x, y };
    _api.transformf_create(&result, TRANSFORM_CREATE_SCALING, params);
    return result;
  }

  //! @brief Create scaling transform.
  static FOG_INLINE TransformF fromScaling(const PointF& p)
  {
    TransformF result(UNINITIALIZED);
    _api.transformf_create(&result, TRANSFORM_CREATE_SCALING, reinterpret_cast<const float*>(&p));
    return result;
  }

  //! @brief Create scaling transform.
  static FOG_INLINE TransformF scaling(float scale)
  {
    TransformF result(UNINITIALIZED);
    float params[2] = { scale, scale };
    _api.transformf_create(&result, TRANSFORM_CREATE_SCALING, params);
    return result;
  }

  //! @brief Create rotation transform.
  static FOG_INLINE TransformF fromRotation(float angle)
  {
    TransformF result(UNINITIALIZED);
    _api.transformf_create(&result, TRANSFORM_CREATE_ROTATION, &angle);
    return result;
  }

  //! @brief Create skewing transform.
  static FOG_INLINE TransformF fromSkewing(float x, float y)
  {
    TransformF result(UNINITIALIZED);
    float params[2] = { x, y };
    _api.transformf_create(&result, TRANSFORM_CREATE_SKEWING, params);
    return result;
  }

  //! @brief Create skewing (shear) transform.
  static FOG_INLINE TransformF fromSkewing(const PointF& p)
  {
    TransformF result(UNINITIALIZED);
    _api.transformf_create(&result, TRANSFORM_CREATE_SKEWING, reinterpret_cast<const float*>(&p));
    return result;
  }

  //! @brief Create line segment transform.
  //!
  //! Rotate, scale and translate, associating 0 to @a dist with line segment
  //! @a x0, @a y0, @a x1 and @a y1.
  static FOG_INLINE TransformF fromLineSegment(float x0, float y0, float x1, float y1, float dist)
  {
    TransformF result(UNINITIALIZED);
    float params[5] = { x0, y0, x1, y1, dist };
    _api.transformf_create(&result, TRANSFORM_CREATE_LINE_SEGMENT, params);
    return result;
  }

  //! @brief Create reflection transform.
  static FOG_INLINE TransformF fromReflection(float u)
  {
    TransformF result(UNINITIALIZED);
    _api.transformf_create(&result, TRANSFORM_CREATE_REFLECTION_U, &u);
    return result;
  }

  //! @brief Create reflection transform.
  static FOG_INLINE TransformF fromReflection(float x, float y)
  {
    TransformF result(UNINITIALIZED);
    float params[2] = { x, y };
    _api.transformf_create(&result, TRANSFORM_CREATE_REFLECTION_XY, params);
    return result;
  }

  //! @brief Create reflection transform.
  static FOG_INLINE TransformF fromReflection(const PointF& p)
  {
    TransformF result(UNINITIALIZED);
    _api.transformf_create(&result, TRANSFORM_CREATE_REFLECTION_XY, reinterpret_cast<const float*>(&p));
    return result;
  }

  //! @brief Create reflection-unit transform.
  //!
  //! Reflect coordinates across the line through the origin containing
  //! the unit vector (ux, uy).
  //!
  //! Contributed by John Horigan
  static FOG_INLINE TransformF fromReflectionUnit(float ux, float uy)
  {
    TransformF result(UNINITIALIZED);
    float params[2] = { ux, uy };
    _api.transformf_create(&result, TRANSFORM_CREATE_REFLECTION_UNIT, params);
    return result;
  }

  //! @brief Create reflection-unit transform.
  static FOG_INLINE TransformF fromReflectionUnit(const PointF& u)
  {
    TransformF result(UNINITIALIZED);
    _api.transformf_create(&result, TRANSFORM_CREATE_REFLECTION_UNIT, reinterpret_cast<const float*>(&u));
    return result;
  }

  static FOG_INLINE TransformF fromParallelogram(const ParallelogramParamsF& params)
  {
    TransformF result(UNINITIALIZED);
    _api.transformf_create(&result, TRANSFORM_CREATE_PARALLELOGRAM, &params);
    return result;
  }

  static FOG_INLINE TransformF fromParallelogram(
    const BoxF& dst,
    const PointF& src0, const PointF& src1, const PointF& src2)
  {
    return fromParallelogram(
      ParallelogramParamsF(
        PointF(dst.x0, dst.y0),
        PointF(dst.x1, dst.y0),
        PointF(dst.x1, dst.y1),
        src0,
        src1,
        src2
      )
    );
  }

  static FOG_INLINE TransformF fromParallelogram(
    const RectF& dst,
    const PointF& src0, const PointF& src1, const PointF& src2)
  {
    return fromParallelogram(
      ParallelogramParamsF(
        PointF(dst.x        , dst.y        ),
        PointF(dst.x + dst.w, dst.y        ),
        PointF(dst.x + dst.w, dst.y + dst.h),
        src0,
        src1,
        src2
      )
    );
  }

  static FOG_INLINE TransformF fromParallelogram(
    const PointF& dst0, const PointF& dst1, const PointF& dst2,
    const BoxF& src)
  {
    return fromParallelogram(
      ParallelogramParamsF(
        dst0,
        dst1,
        dst2,
        PointF(src.x0, src.y0),
        PointF(src.x1, src.y0),
        PointF(src.x1, src.y1)
      )
    );
  }

  static FOG_INLINE TransformF fromParallelogram(
    const PointF& dst0, const PointF& dst1, const PointF& dst2,
    const RectF& src)
  {
    return fromParallelogram(
      ParallelogramParamsF(
        dst0,
        dst1,
        dst2,
        PointF(src.x        , src.y        ),
        PointF(src.x + src.w, src.y        ),
        PointF(src.x + src.w, src.y + src.h)
      )
    );
  }

  static FOG_INLINE TransformF fromParallelogram(
    const PointF& dst0, const PointF& dst1, const PointF& dst2,
    const PointF& src0, const PointF& src1, const PointF& src2)
  {
    return fromParallelogram(ParallelogramParamsF(dst0, dst1, dst2, src0, src1, src2));
  }

  static FOG_INLINE TransformF fromQuadToQuad(const QuadToQuadParamsF& params)
  {
    TransformF result(UNINITIALIZED);
    _api.transformf_create(&result, TRANSFORM_CREATE_QUAD_TO_QUAD, &params);
    return result;
  }

  static FOG_INLINE TransformF fromQuadToQuad(
    const PointF& dst0, const PointF& dst1, const PointF& dst2, const PointF& dst3,
    const BoxF& src)
  {
    return fromQuadToQuad(
      QuadToQuadParamsF(
        dst0,
        dst1,
        dst2,
        dst3,
        PointF(src.x0, src.y0),
        PointF(src.x1, src.y0),
        PointF(src.x1, src.y1),
        PointF(src.x0, src.y1)
      )
    );
  }

  static FOG_INLINE TransformF fromQuadToQuad(
    const PointF& dst0, const PointF& dst1, const PointF& dst2, const PointF& dst3,
    const RectF& src)
  {
    float x0 = src.x;
    float y0 = src.y;
    float x1 = src.x + src.w;
    float y1 = src.y + src.h;

    return fromQuadToQuad(
      QuadToQuadParamsF(
        dst0,
        dst1,
        dst2,
        dst3,
        PointF(x0, y0),
        PointF(x1, y0),
        PointF(x1, y1),
        PointF(x0, y1)
      )
    );
  }

  static FOG_INLINE TransformF fromQuadToQuad(
    const BoxF& dst,
    const PointF& src0, const PointF& src1, const PointF& src2, const PointF& src3)
  {
    return fromQuadToQuad(
      QuadToQuadParamsF(
        PointF(dst.x0, dst.y0),
        PointF(dst.x1, dst.y0),
        PointF(dst.x1, dst.y1),
        PointF(dst.x0, dst.y1),
        src0,
        src1,
        src2,
        src3
      )
    );
  }

  static FOG_INLINE TransformF fromQuadToQuad(
    const RectF& dst,
    const PointF& src0, const PointF& src1, const PointF& src2, const PointF& src3)
  {
    float x0 = dst.x;
    float y0 = dst.y;
    float x1 = dst.x + dst.w;
    float y1 = dst.y + dst.h;

    return fromQuadToQuad(
      QuadToQuadParamsF(
        PointF(x0, y0),
        PointF(x1, y0),
        PointF(x1, y1),
        PointF(x0, y1),
        src0,
        src1,
        src2,
        src3
      )
    );
  }

  static FOG_INLINE TransformF fromQuadToQuad(
    const PointF& dst0, const PointF& dst1, const PointF& dst2, const PointF& dst3,
    const PointF& src0, const PointF& src1, const PointF& src2, const PointF& src3)
  {
    return fromQuadToQuad(QuadToQuadParamsF(dst0, dst1, dst2, dst3, src0, src1, src2, src3));
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float* getData() { return _data; }
  FOG_INLINE const float* getData() const { return _data; }

  FOG_INLINE void setData(
    float m00, float m01,
    float m10, float m11,
    float m20, float m21)
  {
    _setData(TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY,
      m00, m01, 0.0f,
      m10, m11, 0.0f,
      m20, m21, 1.0f);
  }

  FOG_INLINE void setData(
    float m00, float m01, float m02,
    float m10, float m11, float m12,
    float m20, float m21, float m22)
  {
    _setData(TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY,
      m00, m01, m02,
      m10, m11, m12,
      m20, m21, m22);
  }

  FOG_INLINE void setData(const float* data)
  {
    _setData(TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY,
      data[0], data[1], data[2],
      data[3], data[4], data[5],
      data[6], data[7], data[8]);
  }

  FOG_INLINE void _setData(uint32_t type,
    float m00, float m01, float m02,
    float m10, float m11, float m12,
    float m20, float m21, float m22)
  {
    _type = type;

    _00 = m00; _01 = m01; _02 = m02;
    _10 = m10; _11 = m11; _12 = m12;
    _20 = m20; _21 = m21; _22 = m22;
  }

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  //! @brief Get the type of the transform, updating it when needed.
  //!
  //! Type of transform allows to use specialized method for mapping vertices.
  FOG_INLINE uint32_t getType() const
  {
    if (FOG_LIKELY(_type < TRANSFORM_TYPE_COUNT)) return _type;

    uint32_t type = _api.transformf_update(this);
    FOG_ASSUME(_type == type);
    return type;
  }

  FOG_INLINE uint32_t _getType() const
  {
    FOG_ASSUME(_type < TRANSFORM_TYPE_COUNT);
    return _type;
  }

  //! @brief Update the type of the transform and get it.
  FOG_INLINE uint32_t updateType() const
  {
    uint32_t type = _api.transformf_update(this);
    FOG_ASSUME(_type == type);
    return type;
  }

  //! @brief Invalidate the type of the transform.
  //!
  //! Invalidate must be called after you modified transform members.
  FOG_INLINE void invalidate(uint32_t toType = TRANSFORM_TYPE_DEGENERATE)
  {
    _type = toType | TRANSFORM_TYPE_DIRTY;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @brief Reset transform to identity.
  FOG_INLINE void reset()
  {
    _setData(TRANSFORM_TYPE_IDENTITY,
      1.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 1.0f);
  }

  // --------------------------------------------------------------------------
  // [Assign]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setTransform(const TransformF& other);
  FOG_INLINE err_t setTransform(const TransformD& other);

  //! @brief Create translation transform.
  FOG_INLINE err_t setTranslation(float x, float y)
  {
    float params[2] = { x, y };
    return _api.transformf_create(this, TRANSFORM_CREATE_TRANSLATION, params);
  }

  //! @brief Create translation transform.
  FOG_INLINE err_t setTranslation(const PointF& p)
  {
    return _api.transformf_create(this, TRANSFORM_CREATE_TRANSLATION, &p);
  }

  //! @brief Create scaling transform.
  FOG_INLINE err_t setScaling(float x, float y)
  {
    float params[2] = { x, y };
    return _api.transformf_create(this, TRANSFORM_CREATE_SCALING, params);
  }

  //! @brief Create scaling transform.
  FOG_INLINE err_t setScaling(const PointF& p)
  {
    return _api.transformf_create(this, TRANSFORM_CREATE_SCALING, &p);
  }

  //! @brief Create scaling transform.
  FOG_INLINE err_t setScaling(float scale)
  {
    float params[2] = { scale, scale };
    return _api.transformf_create(this, TRANSFORM_CREATE_SCALING, params);
  }

  //! @brief Create rotation transform.
  FOG_INLINE err_t setRotation(float angle)
  {
    return _api.transformf_create(this, TRANSFORM_CREATE_ROTATION, &angle);
  }

  //! @brief Create skewing (shear) transform.
  FOG_INLINE err_t setSkewing(float x, float y)
  {
    float params[2] = { x, y };
    return _api.transformf_create(this, TRANSFORM_CREATE_SKEWING, params);
  }

  //! @brief Create skewing (shear) transform.
  FOG_INLINE err_t setSkewing(const PointF& p)
  {
    return _api.transformf_create(this, TRANSFORM_CREATE_SKEWING, &p);
  }

  //! @brief Create line segment transform.
  //!
  //! Rotate, scale and translate, associating 0 to @a dist with line segment
  //! @a x0, @a y0, @a x1 and @a y1.
  FOG_INLINE err_t setLineSegment(float x0, float y0, float x1, float y1, float dist)
  {
    float params[5] = { x0, y0, x1, y1, dist };
    return _api.transformf_create(this, TRANSFORM_CREATE_LINE_SEGMENT, params);
  }

  //! @brief Create reflection transform.
  FOG_INLINE err_t setReflection(float u)
  {
    return _api.transformf_create(this, TRANSFORM_CREATE_REFLECTION_U, &u);
  }

  //! @brief Create reflection transform.
  FOG_INLINE err_t setReflection(float x, float y)
  {
    float params[2] = { x, y };
    return _api.transformf_create(this, TRANSFORM_CREATE_REFLECTION_XY, params);
  }

  //! @brief Create reflection transform.
  FOG_INLINE err_t setReflection(const PointF& p)
  {
    return _api.transformf_create(this, TRANSFORM_CREATE_REFLECTION_XY, &p);
  }

  //! @brief Create reflection-unit transform.
  //!
  //! Reflect coordinates across the line through the origin containing
  //! the unit vector (ux, uy).
  //!
  //! Contributed by John Horigan
  FOG_INLINE err_t setReflectionUnit(float ux, float uy)
  {
    float params[2] = { ux, uy };
    return _api.transformf_create(this, TRANSFORM_CREATE_REFLECTION_UNIT, params);
  }

  //! @brief Create reflection-unit transform.
  FOG_INLINE err_t setReflectionUnit(const PointF& u)
  {
    return _api.transformf_create(this, TRANSFORM_CREATE_REFLECTION_UNIT, &u);
  }

  FOG_INLINE err_t setParallelogram(const ParallelogramParamsF& params)
  {
    return _api.transformf_create(this, TRANSFORM_CREATE_PARALLELOGRAM, &params);
  }

  FOG_INLINE err_t setParallelogram(
    const BoxF& dst,
    const PointF& src0, const PointF& src1, const PointF& src2)
  {
    return setParallelogram(
      ParallelogramParamsF(
        PointF(dst.x0, dst.y0),
        PointF(dst.x1, dst.y0),
        PointF(dst.x1, dst.y1),
        src0,
        src1,
        src2
      )
    );
  }

  FOG_INLINE err_t setParallelogram(
    const RectF& dst,
    const PointF& src0, const PointF& src1, const PointF& src2)
  {
    return setParallelogram(
      ParallelogramParamsF(
        PointF(dst.x        , dst.y        ),
        PointF(dst.x + dst.w, dst.y        ),
        PointF(dst.x + dst.w, dst.y + dst.h),
        src0,
        src1,
        src2
      )
    );
  }

  FOG_INLINE err_t setParallelogram(
    const PointF& dst0, const PointF& dst1, const PointF& dst2,
    const BoxF& src)
  {
    return setParallelogram(
      ParallelogramParamsF(
        dst0,
        dst1,
        dst2,
        PointF(src.x0, src.y0),
        PointF(src.x1, src.y0),
        PointF(src.x1, src.y1)
      )
    );
  }

  FOG_INLINE err_t setParallelogram(
    const PointF& dst0, const PointF& dst1, const PointF& dst2,
    const RectF& src)
  {
    return setParallelogram(
      ParallelogramParamsF(
        dst0,
        dst1,
        dst2,
        PointF(src.x        , src.y        ),
        PointF(src.x + src.w, src.y        ),
        PointF(src.x + src.w, src.y + src.h)
      )
    );
  }

  FOG_INLINE err_t setParallelogram(
    const PointF& dst0, const PointF& dst1, const PointF& dst2,
    const PointF& src0, const PointF& src1, const PointF& src2)
  {
    return setParallelogram(ParallelogramParamsF(dst0, dst1, dst2, src0, src1, src2));
  }

  FOG_INLINE err_t setQuadToQuad(const QuadToQuadParamsF& params)
  {
    return _api.transformf_create(this, TRANSFORM_CREATE_QUAD_TO_QUAD, &params);
  }

  FOG_INLINE err_t setQuadToQuad(
    const PointF& dst0, const PointF& dst1, const PointF& dst2, const PointF& dst3,
    const BoxF& src)
  {
    return setQuadToQuad(
      QuadToQuadParamsF(
        dst0,
        dst1,
        dst2,
        dst3,
        PointF(src.x0, src.y0),
        PointF(src.x1, src.y0),
        PointF(src.x1, src.y1),
        PointF(src.x0, src.y1)
      )
    );
  }

  FOG_INLINE err_t setQuadToQuad(
    const PointF& dst0, const PointF& dst1, const PointF& dst2, const PointF& dst3,
    const RectF& src)
  {
    float x0 = src.x;
    float y0 = src.y;
    float x1 = src.x + src.w;
    float y1 = src.y + src.h;

    return setQuadToQuad(
      QuadToQuadParamsF(
        dst0,
        dst1,
        dst2,
        dst3,
        PointF(x0, y0),
        PointF(x1, y0),
        PointF(x1, y1),
        PointF(x0, y1)
      )
    );
  }

  FOG_INLINE err_t setQuadToQuad(
    const BoxF& dst,
    const PointF& src0, const PointF& src1, const PointF& src2, const PointF& src3)
  {
    return setQuadToQuad(
      QuadToQuadParamsF(
        PointF(dst.x0, dst.y0),
        PointF(dst.x1, dst.y0),
        PointF(dst.x1, dst.y1),
        PointF(dst.x0, dst.y1),
        src0,
        src1,
        src2,
        src3
      )
    );
  }

  FOG_INLINE err_t setQuadToQuad(
    const RectF& dst,
    const PointF& src0, const PointF& src1, const PointF& src2, const PointF& src3)
  {
    float x0 = dst.x;
    float y0 = dst.y;
    float x1 = dst.x + dst.w;
    float y1 = dst.y + dst.h;

    return setQuadToQuad(
      QuadToQuadParamsF(
        PointF(x0, y0),
        PointF(x1, y0),
        PointF(x1, y1),
        PointF(x0, y1),
        src0,
        src1,
        src2,
        src3
      )
    );
  }

  FOG_INLINE err_t setQuadToQuad(
    const PointF& dst0, const PointF& dst1, const PointF& dst2, const PointF& dst3,
    const PointF& src0, const PointF& src1, const PointF& src2, const PointF& src3)
  {
    return setQuadToQuad(QuadToQuadParamsF(dst0, dst1, dst2, dst3, src0, src1, src2, src3));
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t _transform(uint32_t transformOp, const void* params)
  {
    return _api.transformf_transform(this, transformOp, params);
  }

  FOG_INLINE err_t translate(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _api.transformf_transform(this, TRANSFORM_OP_TRANSLATEF | (order << 4), &p);
  }

  FOG_INLINE err_t scale(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _api.transformf_transform(this, TRANSFORM_OP_SCALEF | (order << 4), &p);
  }

  FOG_INLINE err_t skew(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _api.transformf_transform(this, TRANSFORM_OP_SKEWF | (order << 4), &p);
  }

  FOG_INLINE err_t rotate(float angle, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _api.transformf_transform(this, TRANSFORM_OP_ROTATEF | (order << 4), &angle);
  }

  FOG_INLINE err_t rotate(float angle, float x, float y, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    float params[3] = { angle, x, y };
    return _api.transformf_transform(this, TRANSFORM_OP_ROTATE_POINTF | (order << 4), params);
  }

  FOG_INLINE err_t rotate(float angle, const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    float params[3] = { angle, p.x, p.y };
    return _api.transformf_transform(this, TRANSFORM_OP_ROTATE_POINTF | (order << 4), params);
  }

  FOG_INLINE err_t flip(uint32_t axis)
  {
    return _api.transformf_transform(this, TRANSFORM_OP_FLIP, &axis);
  }

  FOG_INLINE err_t transform(const TransformF& other, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _api.transformf_transform(this, TRANSFORM_OP_MULTIPLYF | (order << 4), &other);
  }

  FOG_INLINE TransformF translated(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND) const
  {
    return _api.transformf_transformed(this, TRANSFORM_OP_TRANSLATEF | (order << 4), &p);
  }

  FOG_INLINE TransformF scaled(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND) const
  {
    return _api.transformf_transformed(this, TRANSFORM_OP_SCALEF | (order << 4), &p);
  }

  FOG_INLINE TransformF skewed(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND) const
  {
    return _api.transformf_transformed(this, TRANSFORM_OP_SKEWF | (order << 4), &p);
  }

  FOG_INLINE TransformF rotated(float angle, uint32_t order = MATRIX_ORDER_PREPEND) const
  {
    return _api.transformf_transformed(this, TRANSFORM_OP_ROTATEF | (order << 4), &angle);
  }

  FOG_INLINE TransformF rotated(float angle, const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND) const
  {
    float params[3] = { angle, p.x, p.y };
    return _api.transformf_transformed(this, TRANSFORM_OP_ROTATE_POINTF | (order << 4), params);
  }

  FOG_INLINE TransformF fliped(uint32_t axis)
  {
    return _api.transformf_transformed(this, TRANSFORM_OP_FLIP, &axis);
  }

  FOG_INLINE TransformF transformed(const TransformF& other, uint32_t order = MATRIX_ORDER_PREPEND) const
  {
    return _api.transformf_transformed(this, TRANSFORM_OP_MULTIPLYF | (order << 4), &other);
  }

  // --------------------------------------------------------------------------
  // [Multiply / Premultiply]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t multiply(const TransformF& m)
  {
    return _api.transformf_transform(this, TRANSFORM_OP_MULTIPLYF | (MATRIX_ORDER_APPEND << 4), &m);
  }

  FOG_INLINE err_t multiplyInv(const TransformF& m)
  {
    return _api.transformf_transform(this, TRANSFORM_OP_MULTIPLY_INVF | (MATRIX_ORDER_APPEND << 4), &m);
  }

  FOG_INLINE err_t premultiply(const TransformF& m)
  {
    return _api.transformf_transform(this, TRANSFORM_OP_MULTIPLYF | (MATRIX_ORDER_PREPEND << 4), &m);
  }

  FOG_INLINE err_t premultiplyInv(const TransformF& m)
  {
    return _api.transformf_transform(this, TRANSFORM_OP_MULTIPLY_INVF | (MATRIX_ORDER_PREPEND << 4), &m);
  }

  FOG_INLINE TransformF multiplied(const TransformF& m) const
  {
    TransformF result(UNINITIALIZED);
    _api.transformf_multiply(&result, this, &m);
    return result;
  }

  FOG_INLINE TransformF multipliedInv(const TransformF& m) const
  {
    return _api.transformf_transformed(this, TRANSFORM_OP_MULTIPLY_INVF | (MATRIX_ORDER_APPEND << 4), &m);
  }

  FOG_INLINE TransformF premultiplied(const TransformF& m) const
  {
    TransformF result(UNINITIALIZED);
    _api.transformf_multiply(&result, &m, this);
    return result;
  }

  FOG_INLINE TransformF premultipliedInv(const TransformF& m) const
  {
    return _api.transformf_transformed(this, TRANSFORM_OP_MULTIPLY_INVF | (MATRIX_ORDER_PREPEND << 4), &m);
  }

  // --------------------------------------------------------------------------
  // [Invert]
  // --------------------------------------------------------------------------

  //! @brief Invert transform.
  //!
  //! Do not try to invert degenerate matrices, there's no check for validity.
  //! If you set scale to 0 and then try to invert transform, the result is
  //! undefined.
  FOG_INLINE bool invert()
  {
    return _api.transformf_invert(this, this);
  }

  //! @brief Get inverted transform.
  FOG_INLINE TransformF inverted() const
  {
    TransformF result(UNINITIALIZED);
    _api.transformf_invert(&result, this);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  FOG_INLINE void mapPoint(PointF& pt) const
  {
    _api.transformf_mapPointF(this, &pt, &pt);
  }

  FOG_INLINE void mapPoint(PointF& dst, const PointF& src) const
  {
    _api.transformf_mapPointF(this, &dst, &src);
  }

  FOG_INLINE void mapPoints(PointF* pts, size_t count) const
  {
    _api.transformf_mapPointsF[getType()](this, pts, pts, count);
  }

  FOG_INLINE void mapPoints(PointF* dst, const PointF* src, size_t count) const
  {
    _api.transformf_mapPointsF[getType()](this, dst, src, count);
  }

  FOG_INLINE void _mapPoints(PointF* pts, size_t count) const
  {
    FOG_ASSERT(_type < TRANSFORM_TYPE_COUNT);
    _api.transformf_mapPointsF[_type](this, pts, pts, count);
  }

  FOG_INLINE void _mapPoints(PointF* dst, const PointF* src, size_t count) const
  {
    FOG_ASSERT(_type < TRANSFORM_TYPE_COUNT);
    _api.transformf_mapPointsF[_type](this, dst, src, count);
  }

  FOG_INLINE void mapBox(BoxF& dst, const BoxF& src) const
  {
    _api.transformf_mapBoxF(this, &dst, &src);
  }

  FOG_INLINE void mapVector(PointF& pt) const
  {
    _api.transformf_mapVectorF(this, &pt, &pt);
  }

  FOG_INLINE void mapVector(PointF& dst, const PointF& src) const
  {
    _api.transformf_mapVectorF(this, &dst, &src);
  }

  FOG_INLINE err_t mapPath(PathF& dst, const PathF& src, uint32_t cntOp = CONTAINER_OP_REPLACE) const
  {
    return _api.transformf_mapPathF(this, &dst, &src, cntOp);
  }

  FOG_INLINE err_t mapPathData(PathF& dst, const uint8_t* srcCmd, const PointF* srcPts, size_t srcLength, uint32_t cntOp = CONTAINER_OP_REPLACE) const
  {
    return _api.transformf_mapPathDataF(this, &dst, srcCmd, srcPts, srcLength, cntOp);
  }

  // --------------------------------------------------------------------------
  // [Auxiliary]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isIdentity() const { return getType() == TRANSFORM_TYPE_IDENTITY; }
  FOG_INLINE bool isTranslation() const { return getType() == TRANSFORM_TYPE_TRANSLATION; }
  FOG_INLINE bool isAffine() const { return getType() <= TRANSFORM_TYPE_AFFINE; }
  FOG_INLINE bool isProjection() const { return getType() >= TRANSFORM_TYPE_PROJECTION; }

  FOG_INLINE bool isDegenerate() const { return getType() >= TRANSFORM_TYPE_DEGENERATE; }
  FOG_INLINE bool isInvertible() const { return getType() != TRANSFORM_TYPE_DEGENERATE; }

  FOG_INLINE float getDeterminant() const
  {
    if (getType() <= TRANSFORM_TYPE_AFFINE)
    {
      return (_00 * _11 - _01 * _10);
    }
    else
    {
      return (_22 * _11 - _21 * _12) * _00 -
             (_22 * _01 - _21 * _02) * _10 +
             (_12 * _01 - _11 * _02) * _20 ;
    }
  }

  //! @brief Get translation part of transform.
  FOG_INLINE PointF getTranslation() const { return PointF(_20, _21); }
  FOG_INLINE PointF getScaling(bool absolute) const { return _api.transformf_getScaling(this, absolute); }
  FOG_INLINE float getRotation() const { return _api.transformf_getRotation(this); }

  //! @brief Get the average scale (by X and Y).
  //!
  //! Basically used to calculate the approximation scale when decomposinting
  //! curves into line segments.
  FOG_INLINE float getAverageScaling() const { return _api.transformf_getAverageScaling(this); }

  // --------------------------------------------------------------------------
  // [HashCode]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getHashCode() const
  {
    return HashUtil::hashVectorD(_data, 9);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE TransformF& operator=(const TransformF& other)
  {
    setTransform(other);
    return *this;
  }

  FOG_INLINE TransformF& operator=(const TransformD& other)
  {
    setTransform(other);
    return *this;
  }

  //! @brief Multiply the transform by another one.
  FOG_INLINE TransformF& operator*=(const TransformF& m)
  {
    multiply(m);
    return *this;
  }

  //! @brief Multiply the transform by inverse of another one.
  FOG_INLINE TransformF& operator/=(const TransformF& m)
  {
    multiplyInv(m);
    return *this;
  }

  //! @brief Multiply the transform by another one and return
  //! the result in a separete transform.
  FOG_INLINE TransformF operator*(const TransformF& m) const
  {
    return multiplied(m);
  }

  //! @brief Multiply the transform by inverse of another one
  //! and return the result in a separete transform.
  FOG_INLINE TransformF operator/(const TransformF& m) const
  {
    return multipliedInv(m);
  }

  //! @brief Calculate and return the inverse transform.
  FOG_INLINE TransformF operator~() const
  {
    return inverted();
  }

  FOG_INLINE bool operator==(const TransformF& other) const { return  MemOps::eq_s<9 * sizeof(float)>(_data, other._data); }
  FOG_INLINE bool operator!=(const TransformF& other) const { return !MemOps::eq_s<9 * sizeof(float)>(_data, other._data); }

  FOG_INLINE TransformF& operator+=(float val)
  {
    _00 += val; _01 += val; _02 += val;
    _10 += val; _11 += val; _12 += val;
    _20 += val; _21 += val; _22 += val;

    _type = TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY;
    return *this;
  }

  FOG_INLINE TransformF& operator-=(float val)
  {
    _00 -= val; _01 -= val; _02 -= val;
    _10 -= val; _11 -= val; _12 -= val;
    _20 -= val; _21 -= val; _22 -= val;

    _type = TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY;
    return *this;
  }

  FOG_INLINE TransformF& operator*=(float val)
  {
    _00 *= val; _01 *= val; _02 *= val;
    _10 *= val; _11 *= val; _12 *= val;
    _20 *= val; _21 *= val; _22 *= val;

    _type = TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY;
    return *this;
  }

  FOG_INLINE TransformF& operator/=(float val)
  {
    if (val != 0.0f)
    {
      float recip = 1.0f / val;
      return operator*=(recip);
    }
    else
    {
      return *this;
    }
  }
  
  FOG_INLINE const float& operator[](size_t i) const { return _data[i]; }
  FOG_INLINE float& operator[](size_t i) { return _data[i]; }

  // --------------------------------------------------------------------------
  // [Statics - Instances]
  // --------------------------------------------------------------------------

  static FOG_INLINE const TransformF& identity()
  {
    return *_api.transformf_oIdentity;
  }

  // --------------------------------------------------------------------------
  // [Statics - Multiply]
  // --------------------------------------------------------------------------

  static FOG_INLINE void multiply(TransformF& dst, const TransformF& a, const TransformF& b)
  {
    _api.transformf_multiply(&dst, &a, &b);
  }

  // --------------------------------------------------------------------------
  // [Statics - Invert]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool invert(TransformF& dst, const TransformF& a)
  {
    return _api.transformf_invert(&dst, &a);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Matrix type.
  mutable uint32_t _type;

  //! @brief Matrix data.
  //!
  //! @verbatim
  //! Indexed values:
  //!
  //!   [M00 M01 M02]
  //!   [M10 M11 M12]
  //!   [M20 M21 M22]
  //!
  //! Named values:
  //!
  //!   [ScaleX ShearY W0]
  //!   [ShearX ScaleY W1]
  //!   [TransX TransY W2]
  //! @endverbatim
  union
  {
    float _data[9];
    struct { float _00, _01, _02, _10, _11, _12, _20, _21, _22; };
  };
};

// ============================================================================
// [Fog::TransformD]
// ============================================================================

//! @brief Matrix (double).
struct FOG_NO_EXPORT TransformD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create an identity transform.
  FOG_INLINE TransformD()
  {
    _setData(TRANSFORM_TYPE_IDENTITY,
      1.0, 0.0, 0.0,
      0.0, 1.0, 0.0,
      0.0, 0.0, 1.0);
  }

  //! @brief Create a copy of @a other transform.
  FOG_INLINE TransformD(const TransformD& other)
  {
    setTransform(other);
  }

  //! @brief Create an affine transform.
  FOG_INLINE TransformD(
    double m00, double m01,
    double m10, double m11,
    double m20, double m21)
  {
    _setData(TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY,
      m00, m01, 0.0,
      m10, m11, 0.0,
      m20, m21, 1.0);
  }

  //! @brief Create a projection transform.
  FOG_INLINE TransformD(
    double m00, double m01, double m02,
    double m10, double m11, double m12,
    double m20, double m21, double m22)
  {
    _setData(TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY,
      m00, m01, m02,
      m10, m11, m12,
      m20, m21, m22);
  }

  explicit FOG_INLINE TransformD(_Uninitialized) {}

  //! @brief Create a copy of @a other transform.
  explicit FOG_INLINE TransformD(const TransformF& other)
  {
    setTransform(other);
  }

  //! @brief Create custom transform from @a data[9].
  explicit FOG_INLINE TransformD(const float* data)
  {
    _setData(TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY,
      data[0], data[1], data[2],
      data[3], data[4], data[5],
      data[6], data[7], data[8]);
  }

  //! @brief Create custom transform from @a data[9].
  explicit FOG_INLINE TransformD(const double* data)
  {
    _setData(TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY,
      data[0], data[1], data[2],
      data[3], data[4], data[5],
      data[6], data[7], data[8]);
  }

  // --------------------------------------------------------------------------
  // [Construction - Static]
  // --------------------------------------------------------------------------

  //! @brief Create translation transform.
  static FOG_INLINE TransformD fromTranslation(double x, double y)
  {
    TransformD result(UNINITIALIZED);
    double params[2] = { x, y };
    _api.transformd_create(&result, TRANSFORM_CREATE_TRANSLATION, params);
    return result;
  }

  //! @brief Create translation transform.
  static FOG_INLINE TransformD fromTranslation(const PointD& p)
  {
    TransformD result(UNINITIALIZED);
    _api.transformd_create(&result, TRANSFORM_CREATE_TRANSLATION, reinterpret_cast<const double*>(&p));
    return result;
  }

  //! @brief Create scaling transform.
  static FOG_INLINE TransformD fromScaling(double x, double y)
  {
    TransformD result(UNINITIALIZED);
    double params[2] = { x, y };
    _api.transformd_create(&result, TRANSFORM_CREATE_SCALING, params);
    return result;
  }

  //! @brief Create scaling transform.
  static FOG_INLINE TransformD fromScaling(const PointD& p)
  {
    TransformD result(UNINITIALIZED);
    _api.transformd_create(&result, TRANSFORM_CREATE_SCALING, reinterpret_cast<const double*>(&p));
    return result;
  }

  //! @brief Create scaling transform.
  static FOG_INLINE TransformD scaling(double scale)
  {
    TransformD result(UNINITIALIZED);
    double params[2] = { scale, scale };
    _api.transformd_create(&result, TRANSFORM_CREATE_SCALING, params);
    return result;
  }

  //! @brief Create rotation transform.
  static FOG_INLINE TransformD fromRotation(double angle)
  {
    TransformD result(UNINITIALIZED);
    _api.transformd_create(&result, TRANSFORM_CREATE_ROTATION, &angle);
    return result;
  }

  //! @brief Create skewing transform.
  static FOG_INLINE TransformD fromSkewing(double x, double y)
  {
    TransformD result(UNINITIALIZED);
    double params[2] = { x, y };
    _api.transformd_create(&result, TRANSFORM_CREATE_SKEWING, params);
    return result;
  }

  //! @brief Create skewing (shear) transform.
  static FOG_INLINE TransformD fromSkewing(const PointD& p)
  {
    TransformD result(UNINITIALIZED);
    _api.transformd_create(&result, TRANSFORM_CREATE_SKEWING, reinterpret_cast<const double*>(&p));
    return result;
  }

  //! @brief Create line segment transform.
  //!
  //! Rotate, scale and translate, associating 0 to @a dist with line segment
  //! @a x0, @a y0, @a x1 and @a y1.
  static FOG_INLINE TransformD fromLineSegment(double x0, double y0, double x1, double y1, double dist)
  {
    TransformD result(UNINITIALIZED);
    double params[5] = { x0, y0, x1, y1, dist };
    _api.transformd_create(&result, TRANSFORM_CREATE_LINE_SEGMENT, params);
    return result;
  }

  //! @brief Create reflection transform.
  static FOG_INLINE TransformD fromReflection(double u)
  {
    TransformD result(UNINITIALIZED);
    _api.transformd_create(&result, TRANSFORM_CREATE_REFLECTION_U, &u);
    return result;
  }

  //! @brief Create reflection transform.
  static FOG_INLINE TransformD fromReflection(double x, double y)
  {
    TransformD result(UNINITIALIZED);
    double params[2] = { x, y };
    _api.transformd_create(&result, TRANSFORM_CREATE_REFLECTION_XY, params);
    return result;
  }

  //! @brief Create reflection transform.
  static FOG_INLINE TransformD fromReflection(const PointD& p)
  {
    TransformD result(UNINITIALIZED);
    _api.transformd_create(&result, TRANSFORM_CREATE_REFLECTION_XY, reinterpret_cast<const double*>(&p));
    return result;
  }

  //! @brief Create reflection-unit transform.
  //!
  //! Reflect coordinates across the line through the origin containing
  //! the unit vector (ux, uy).
  //!
  //! Contributed by John Horigan
  static FOG_INLINE TransformD fromReflectionUnit(double ux, double uy)
  {
    TransformD result(UNINITIALIZED);
    double params[2] = { ux, uy };
    _api.transformd_create(&result, TRANSFORM_CREATE_REFLECTION_UNIT, params);
    return result;
  }

  //! @brief Create reflection-unit transform.
  static FOG_INLINE TransformD fromReflectionUnit(const PointD& u)
  {
    TransformD result(UNINITIALIZED);
    _api.transformd_create(&result, TRANSFORM_CREATE_REFLECTION_UNIT, reinterpret_cast<const double*>(&u));
    return result;
  }

  static FOG_INLINE TransformD fromParallelogram(const ParallelogramParamsD& params)
  {
    TransformD result(UNINITIALIZED);
    _api.transformd_create(&result, TRANSFORM_CREATE_PARALLELOGRAM, &params);
    return result;
  }

  static FOG_INLINE TransformD fromParallelogram(
    const BoxD& dst,
    const PointD& src0, const PointD& src1, const PointD& src2)
  {
    return fromParallelogram(
      ParallelogramParamsD(
        PointD(dst.x0, dst.y0),
        PointD(dst.x1, dst.y0),
        PointD(dst.x1, dst.y1),
        src0,
        src1,
        src2
      )
    );
  }

  static FOG_INLINE TransformD fromParallelogram(
    const RectD& dst,
    const PointD& src0, const PointD& src1, const PointD& src2)
  {
    return fromParallelogram(
      ParallelogramParamsD(
        PointD(dst.x        , dst.y        ),
        PointD(dst.x + dst.w, dst.y        ),
        PointD(dst.x + dst.w, dst.y + dst.h),
        src0,
        src1,
        src2
      )
    );
  }

  static FOG_INLINE TransformD fromParallelogram(
    const PointD& dst0, const PointD& dst1, const PointD& dst2,
    const BoxD& src)
  {
    return fromParallelogram(
      ParallelogramParamsD(
        dst0,
        dst1,
        dst2,
        PointD(src.x0, src.y0),
        PointD(src.x1, src.y0),
        PointD(src.x1, src.y1)
      )
    );
  }

  static FOG_INLINE TransformD fromParallelogram(
    const PointD& dst0, const PointD& dst1, const PointD& dst2,
    const RectD& src)
  {
    return fromParallelogram(
      ParallelogramParamsD(
        dst0,
        dst1,
        dst2,
        PointD(src.x        , src.y        ),
        PointD(src.x + src.w, src.y        ),
        PointD(src.x + src.w, src.y + src.h)
      )
    );
  }

  static FOG_INLINE TransformD fromParallelogram(
    const PointD& dst0, const PointD& dst1, const PointD& dst2,
    const PointD& src0, const PointD& src1, const PointD& src2)
  {
    return fromParallelogram(ParallelogramParamsD(dst0, dst1, dst2, src0, src1, src2));
  }

  static FOG_INLINE TransformD fromQuadToQuad(const QuadToQuadParamsD& params)
  {
    TransformD result(UNINITIALIZED);
    _api.transformd_create(&result, TRANSFORM_CREATE_QUAD_TO_QUAD, &params);
    return result;
  }

  static FOG_INLINE TransformD fromQuadToQuad(
    const PointD& dst0, const PointD& dst1, const PointD& dst2, const PointD& dst3,
    const BoxD& src)
  {
    return fromQuadToQuad(
      QuadToQuadParamsD(
        dst0,
        dst1,
        dst2,
        dst3,
        PointD(src.x0, src.y0),
        PointD(src.x1, src.y0),
        PointD(src.x1, src.y1),
        PointD(src.x0, src.y1)
      )
    );
  }

  static FOG_INLINE TransformD fromQuadToQuad(
    const PointD& dst0, const PointD& dst1, const PointD& dst2, const PointD& dst3,
    const RectD& src)
  {
    double x0 = src.x;
    double y0 = src.y;
    double x1 = src.x + src.w;
    double y1 = src.y + src.h;

    return fromQuadToQuad(
      QuadToQuadParamsD(
        dst0,
        dst1,
        dst2,
        dst3,
        PointD(x0, y0),
        PointD(x1, y0),
        PointD(x1, y1),
        PointD(x0, y1)
      )
    );
  }

  static FOG_INLINE TransformD fromQuadToQuad(
    const BoxD& dst,
    const PointD& src0, const PointD& src1, const PointD& src2, const PointD& src3)
  {
    return fromQuadToQuad(
      QuadToQuadParamsD(
        PointD(dst.x0, dst.y0),
        PointD(dst.x1, dst.y0),
        PointD(dst.x1, dst.y1),
        PointD(dst.x0, dst.y1),
        src0,
        src1,
        src2,
        src3
      )
    );
  }

  static FOG_INLINE TransformD fromQuadToQuad(
    const RectD& dst,
    const PointD& src0, const PointD& src1, const PointD& src2, const PointD& src3)
  {
    double x0 = dst.x;
    double y0 = dst.y;
    double x1 = dst.x + dst.w;
    double y1 = dst.y + dst.h;

    return fromQuadToQuad(
      QuadToQuadParamsD(
        PointD(x0, y0),
        PointD(x1, y0),
        PointD(x1, y1),
        PointD(x0, y1),
        src0,
        src1,
        src2,
        src3
      )
    );
  }

  static FOG_INLINE TransformD fromQuadToQuad(
    const PointD& dst0, const PointD& dst1, const PointD& dst2, const PointD& dst3,
    const PointD& src0, const PointD& src1, const PointD& src2, const PointD& src3)
  {
    return fromQuadToQuad(QuadToQuadParamsD(dst0, dst1, dst2, dst3, src0, src1, src2, src3));
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double* getData() { return _data; }
  FOG_INLINE const double* getData() const { return _data; }

  FOG_INLINE void setData(
    double m00, double m01,
    double m10, double m11,
    double m20, double m21)
  {
    _setData(TRANSFORM_TYPE_AFFINE | TRANSFORM_TYPE_DIRTY,
      m00, m01, 0.0,
      m10, m11, 0.0,
      m20, m21, 1.0);
  }

  FOG_INLINE void setData(
    double m00, double m01, double m02,
    double m10, double m11, double m12,
    double m20, double m21, double m22)
  {
    _setData(TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY,
      m00, m01, m02,
      m10, m11, m12,
      m20, m21, m22);
  }

  FOG_INLINE void setData(const double* data)
  {
    _setData(TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY,
      data[0], data[1], data[2],
      data[3], data[4], data[5],
      data[6], data[7], data[8]);
  }

  FOG_INLINE void setData(const float* data)
  {
    _setData(TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY,
      data[0], data[1], data[2],
      data[3], data[4], data[5],
      data[6], data[7], data[8]);
  }

  FOG_INLINE void _setData(uint32_t type,
    double m00, double m01, double m02,
    double m10, double m11, double m12,
    double m20, double m21, double m22)
  {
    _type = type;

    _00 = m00; _01 = m01; _02 = m02;
    _10 = m10; _11 = m11; _12 = m12;
    _20 = m20; _21 = m21; _22 = m22;
  }

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  //! @brief Get the type of the transform, updating it when needed.
  //!
  //! Type of transform allows to use specialized method for mapping vertices.
  FOG_INLINE uint32_t getType() const
  {
    if (FOG_LIKELY(_type < TRANSFORM_TYPE_COUNT)) return _type;

    uint32_t type = _api.transformd_update(this);
    FOG_ASSUME(_type == type);
    return type;
  }

  FOG_INLINE uint32_t _getType() const
  {
    FOG_ASSUME(_type < TRANSFORM_TYPE_COUNT);
    return _type;
  }

  //! @brief Update the type of the transform and get it.
  FOG_INLINE uint32_t updateType() const
  {
    uint32_t type = _api.transformd_update(this);
    FOG_ASSUME(_type == type);
    return type;
  }

  //! @brief Invalidate the type of the transform.
  //!
  //! Invalidate must be called after you modified transform members.
  FOG_INLINE void invalidate(uint32_t toType = TRANSFORM_TYPE_PROJECTION)
  {
    _type = toType | TRANSFORM_TYPE_DIRTY;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @brief Reset transform to identity.
  FOG_INLINE void reset()
  {
    _setData(TRANSFORM_TYPE_IDENTITY,
      1.0, 0.0, 0.0,
      0.0, 1.0, 0.0,
      0.0, 0.0, 1.0);
  }

  // --------------------------------------------------------------------------
  // [Assign]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setTransform(const TransformD& other)
  {
    _setData(other._type,
      other._00, other._01, other._02,
      other._10, other._11, other._12,
      other._20, other._21, other._22);
    return ERR_OK;
  }

  FOG_INLINE err_t setTransform(const TransformF& other)
  {
    _setData(other._type,
      other._00, other._01, other._02,
      other._10, other._11, other._12,
      other._20, other._21, other._22);
    return ERR_OK;
  }

  //! @brief Create translation transform.
  FOG_INLINE err_t setTranslation(double x, double y)
  {
    double params[2] = { x, y };
    return _api.transformd_create(this, TRANSFORM_CREATE_TRANSLATION, params);
  }

  //! @brief Create translation transform.
  FOG_INLINE err_t setTranslation(const PointD& p)
  {
    return _api.transformd_create(this, TRANSFORM_CREATE_TRANSLATION, &p);
  }

  //! @brief Create scaling transform.
  FOG_INLINE err_t setScaling(double x, double y)
  {
    double params[2] = { x, y };
    return _api.transformd_create(this, TRANSFORM_CREATE_SCALING, params);
  }

  //! @brief Create scaling transform.
  FOG_INLINE err_t setScaling(const PointD& p)
  {
    return _api.transformd_create(this, TRANSFORM_CREATE_SCALING, &p);
  }

  //! @brief Create scaling transform.
  FOG_INLINE err_t setScaling(double scale)
  {
    double params[2] = { scale, scale };
    return _api.transformd_create(this, TRANSFORM_CREATE_SCALING, params);
  }

  //! @brief Create rotation transform.
  FOG_INLINE err_t setRotation(double angle)
  {
    return _api.transformd_create(this, TRANSFORM_CREATE_ROTATION, &angle);
  }

  //! @brief Create skewing (shear) transform.
  FOG_INLINE err_t setSkewing(double x, double y)
  {
    double params[2] = { x, y };
    return _api.transformd_create(this, TRANSFORM_CREATE_SKEWING, params);
  }

  //! @brief Create skewing (shear) transform.
  FOG_INLINE err_t setSkewing(const PointD& p)
  {
    return _api.transformd_create(this, TRANSFORM_CREATE_SKEWING, &p);
  }

  //! @brief Create line segment transform.
  //!
  //! Rotate, scale and translate, associating 0 to @a dist with line segment
  //! @a x0, @a y0, @a x1 and @a y1.
  FOG_INLINE err_t setLineSegment(double x0, double y0, double x1, double y1, double dist)
  {
    double params[5] = { x0, y0, x1, y1, dist };
    return _api.transformd_create(this, TRANSFORM_CREATE_LINE_SEGMENT, params);
  }

  //! @brief Create reflection transform.
  FOG_INLINE err_t setReflection(double u)
  {
    return _api.transformd_create(this, TRANSFORM_CREATE_REFLECTION_U, &u);
  }

  //! @brief Create reflection transform.
  FOG_INLINE err_t setReflection(double x, double y)
  {
    double params[2] = { x, y };
    return _api.transformd_create(this, TRANSFORM_CREATE_REFLECTION_XY, params);
  }

  //! @brief Create reflection transform.
  FOG_INLINE err_t setReflection(const PointD& p)
  {
    return _api.transformd_create(this, TRANSFORM_CREATE_REFLECTION_XY, &p);
  }

  //! @brief Create reflection-unit transform.
  //!
  //! Reflect coordinates across the line through the origin containing
  //! the unit vector (ux, uy).
  //!
  //! Contributed by John Horigan
  FOG_INLINE err_t setReflectionUnit(double ux, double uy)
  {
    double params[2] = { ux, uy };
    return _api.transformd_create(this, TRANSFORM_CREATE_REFLECTION_UNIT, params);
  }

  //! @brief Create reflection-unit transform.
  FOG_INLINE err_t setReflectionUnit(const PointD& u)
  {
    return _api.transformd_create(this, TRANSFORM_CREATE_REFLECTION_UNIT, &u);
  }

  FOG_INLINE err_t setParallelogram(const ParallelogramParamsD& params)
  {
    return _api.transformd_create(this, TRANSFORM_CREATE_PARALLELOGRAM, &params);
  }

  FOG_INLINE err_t setParallelogram(
    const BoxD& dst,
    const PointD& src0, const PointD& src1, const PointD& src2)
  {
    return setParallelogram(
      ParallelogramParamsD(
        PointD(dst.x0, dst.y0),
        PointD(dst.x1, dst.y0),
        PointD(dst.x1, dst.y1),
        src0,
        src1,
        src2
      )
    );
  }

  FOG_INLINE err_t setParallelogram(
    const RectD& dst,
    const PointD& src0, const PointD& src1, const PointD& src2)
  {
    return setParallelogram(
      ParallelogramParamsD(
        PointD(dst.x        , dst.y        ),
        PointD(dst.x + dst.w, dst.y        ),
        PointD(dst.x + dst.w, dst.y + dst.h),
        src0,
        src1,
        src2
      )
    );
  }

  FOG_INLINE err_t setParallelogram(
    const PointD& dst0, const PointD& dst1, const PointD& dst2,
    const BoxD& src)
  {
    return setParallelogram(
      ParallelogramParamsD(
        dst0,
        dst1,
        dst2,
        PointD(src.x0, src.y0),
        PointD(src.x1, src.y0),
        PointD(src.x1, src.y1)
      )
    );
  }

  FOG_INLINE err_t setParallelogram(
    const PointD& dst0, const PointD& dst1, const PointD& dst2,
    const RectD& src)
  {
    return setParallelogram(
      ParallelogramParamsD(
        dst0,
        dst1,
        dst2,
        PointD(src.x        , src.y        ),
        PointD(src.x + src.w, src.y        ),
        PointD(src.x + src.w, src.y + src.h)
      )
    );
  }

  FOG_INLINE err_t setParallelogram(
    const PointD& dst0, const PointD& dst1, const PointD& dst2,
    const PointD& src0, const PointD& src1, const PointD& src2)
  {
    return setParallelogram(ParallelogramParamsD(dst0, dst1, dst2, src0, src1, src2));
  }

  FOG_INLINE err_t setQuadToQuad(const QuadToQuadParamsD& params)
  {
    return _api.transformd_create(this, TRANSFORM_CREATE_QUAD_TO_QUAD, &params);
  }

  FOG_INLINE err_t setQuadToQuad(
    const PointD& dst0, const PointD& dst1, const PointD& dst2, const PointD& dst3,
    const BoxD& src)
  {
    return setQuadToQuad(
      QuadToQuadParamsD(
        dst0,
        dst1,
        dst2,
        dst3,
        PointD(src.x0, src.y0),
        PointD(src.x1, src.y0),
        PointD(src.x1, src.y1),
        PointD(src.x0, src.y1)
      )
    );
  }

  FOG_INLINE err_t setQuadToQuad(
    const PointD& dst0, const PointD& dst1, const PointD& dst2, const PointD& dst3,
    const RectD& src)
  {
    double x0 = src.x;
    double y0 = src.y;
    double x1 = src.x + src.w;
    double y1 = src.y + src.h;

    return setQuadToQuad(
      QuadToQuadParamsD(
        dst0,
        dst1,
        dst2,
        dst3,
        PointD(x0, y0),
        PointD(x1, y0),
        PointD(x1, y1),
        PointD(x0, y1)
      )
    );
  }

  FOG_INLINE err_t setQuadToQuad(
    const BoxD& dst,
    const PointD& src0, const PointD& src1, const PointD& src2, const PointD& src3)
  {
    return setQuadToQuad(
      QuadToQuadParamsD(
        PointD(dst.x0, dst.y0),
        PointD(dst.x1, dst.y0),
        PointD(dst.x1, dst.y1),
        PointD(dst.x0, dst.y1),
        src0,
        src1,
        src2,
        src3
      )
    );
  }

  FOG_INLINE err_t setQuadToQuad(
    const RectD& dst,
    const PointD& src0, const PointD& src1, const PointD& src2, const PointD& src3)
  {
    double x0 = dst.x;
    double y0 = dst.y;
    double x1 = dst.x + dst.w;
    double y1 = dst.y + dst.h;

    return setQuadToQuad(
      QuadToQuadParamsD(
        PointD(x0, y0),
        PointD(x1, y0),
        PointD(x1, y1),
        PointD(x0, y1),
        src0,
        src1,
        src2,
        src3
      )
    );
  }

  FOG_INLINE err_t setQuadToQuad(
    const PointD& dst0, const PointD& dst1, const PointD& dst2, const PointD& dst3,
    const PointD& src0, const PointD& src1, const PointD& src2, const PointD& src3)
  {
    return setQuadToQuad(QuadToQuadParamsD(dst0, dst1, dst2, dst3, src0, src1, src2, src3));
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t _transform(uint32_t transformOp, const void* params)
  {
    return _api.transformd_transform(this, transformOp, params);
  }

  FOG_INLINE err_t translate(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _api.transformd_transform(this, TRANSFORM_OP_TRANSLATED | (order << 4), &p);
  }

  FOG_INLINE err_t scale(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _api.transformd_transform(this, TRANSFORM_OP_SCALED | (order << 4), &p);
  }

  FOG_INLINE err_t skew(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _api.transformd_transform(this, TRANSFORM_OP_SKEWD | (order << 4), &p);
  }

  FOG_INLINE err_t rotate(double angle, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _api.transformd_transform(this, TRANSFORM_OP_ROTATED | (order << 4), &angle);
  }

  FOG_INLINE err_t rotate(double angle, const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    double params[3] = { angle, p.x, p.y };
    return _api.transformd_transform(this, TRANSFORM_OP_ROTATE_POINTD | (order << 4), params);
  }

  FOG_INLINE err_t flip(uint32_t axis)
  {
    return _api.transformd_transform(this, TRANSFORM_OP_FLIP, &axis);
  }

  FOG_INLINE err_t transform(const TransformD& other, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _api.transformd_transform(this, TRANSFORM_OP_MULTIPLYD | (order << 4), &other);
  }

  FOG_INLINE TransformD translated(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND) const
  {
    return _api.transformd_transformed(this, TRANSFORM_OP_TRANSLATED | (order << 4), &p);
  }

  FOG_INLINE TransformD scaled(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND) const
  {
    return _api.transformd_transformed(this, TRANSFORM_OP_SCALED | (order << 4), &p);
  }

  FOG_INLINE TransformD skewed(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND) const
  {
    return _api.transformd_transformed(this, TRANSFORM_OP_SKEWD | (order << 4), &p);
  }

  FOG_INLINE TransformD rotated(double angle, uint32_t order = MATRIX_ORDER_PREPEND) const
  {
    return _api.transformd_transformed(this, TRANSFORM_OP_ROTATED | (order << 4), &angle);
  }

  FOG_INLINE TransformD rotated(double angle, const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND) const
  {
    double params[3] = { angle, p.x, p.y };
    return _api.transformd_transformed(this, TRANSFORM_OP_ROTATED | (order << 4), params);
  }

  FOG_INLINE TransformD fliped(uint32_t axis)
  {
    return _api.transformd_transformed(this, TRANSFORM_OP_FLIP, &axis);
  }

  FOG_INLINE TransformD transformed(const TransformD& other, uint32_t order = MATRIX_ORDER_PREPEND) const
  {
    return _api.transformd_transformed(this, TRANSFORM_OP_MULTIPLYD | (order << 4), &other);
  }

  // --------------------------------------------------------------------------
  // [Multiply / Premultiply]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t multiply(const TransformD& m)
  {
    return _api.transformd_transform(this, TRANSFORM_OP_MULTIPLYD | (MATRIX_ORDER_APPEND << 4), &m);
  }

  FOG_INLINE err_t multiplyInv(const TransformD& m)
  {
    return _api.transformd_transform(this, TRANSFORM_OP_MULTIPLY_INVD | (MATRIX_ORDER_APPEND << 4), &m);
  }

  FOG_INLINE err_t premultiply(const TransformD& m)
  {
    return _api.transformd_transform(this, TRANSFORM_OP_MULTIPLYD | (MATRIX_ORDER_PREPEND << 4), &m);
  }

  FOG_INLINE err_t premultiplyInv(const TransformD& m)
  {
    return _api.transformd_transform(this, TRANSFORM_OP_MULTIPLY_INVD | (MATRIX_ORDER_PREPEND << 4), &m);
  }

  FOG_INLINE TransformD multiplied(const TransformD& m) const
  {
    TransformD result(UNINITIALIZED);
    _api.transformd_multiply(&result, this, &m);
    return result;
  }

  FOG_INLINE TransformD multipliedInv(const TransformD& m) const
  {
    return _api.transformd_transformed(this, TRANSFORM_OP_MULTIPLY_INVD | (MATRIX_ORDER_APPEND << 4), &m);
  }

  FOG_INLINE TransformD premultiplied(const TransformD& m) const
  {
    TransformD result(UNINITIALIZED);
    _api.transformd_multiply(&result, &m, this);
    return result;
  }

  FOG_INLINE TransformD premultipliedInv(const TransformD& m) const
  {
    return _api.transformd_transformed(this, TRANSFORM_OP_MULTIPLY_INVD | (MATRIX_ORDER_PREPEND << 4), &m);
  }

  // --------------------------------------------------------------------------
  // [Invert]
  // --------------------------------------------------------------------------

  //! @brief Invert transform.
  //!
  //! Do not try to invert degenerate matrices, there's no check for validity.
  //! If you set scale to 0 and then try to invert transform, the result is
  //! undefined.
  FOG_INLINE bool invert()
  {
    return _api.transformd_invert(this, this);
  }

  //! @brief Get inverted transform.
  FOG_INLINE TransformD inverted() const
  {
    TransformD result(UNINITIALIZED);
    _api.transformd_invert(&result, this);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  FOG_INLINE void mapPoint(PointD& pt) const
  {
    _api.transformd_mapPointD(this, &pt, &pt);
  }

  FOG_INLINE void mapPoint(PointD& dst, const PointD& src) const
  {
    _api.transformd_mapPointD(this, &dst, &src);
  }

  FOG_INLINE void mapPoints(PointD* pts, size_t count) const
  {
    _api.transformd_mapPointsD[getType()](this, pts, pts, count);
  }

  FOG_INLINE void mapPoints(PointD* dst, const PointF* src, size_t count) const
  {
    _api.transformd_mapPointsF[getType()](this, dst, src, count);
  }

  FOG_INLINE void mapPoints(PointD* dst, const PointD* src, size_t count) const
  {
    _api.transformd_mapPointsD[getType()](this, dst, src, count);
  }

  FOG_INLINE void _mapPoints(PointD* pts, size_t count) const
  {
    FOG_ASSERT(_type < TRANSFORM_TYPE_COUNT);
    _api.transformd_mapPointsD[_type](this, pts, pts, count);
  }

  FOG_INLINE void _mapPoints(PointD* dst, const PointF* src, size_t count) const
  {
    FOG_ASSERT(_type < TRANSFORM_TYPE_COUNT);
    _api.transformd_mapPointsF[_type](this, dst, src, count);
  }

  FOG_INLINE void _mapPoints(PointD* dst, const PointD* src, size_t count) const
  {
    FOG_ASSERT(_type < TRANSFORM_TYPE_COUNT);
    _api.transformd_mapPointsD[_type](this, dst, src, count);
  }

  FOG_INLINE err_t mapPath(PathD& dst, const PathF& src, uint32_t cntOp = CONTAINER_OP_REPLACE) const
  {
    return _api.transformd_mapPathF(this, &dst, &src, cntOp);
  }

  FOG_INLINE err_t mapPath(PathD& dst, const PathD& src, uint32_t cntOp = CONTAINER_OP_REPLACE) const
  {
    return _api.transformd_mapPathD(this, &dst, &src, cntOp);
  }

  FOG_INLINE err_t mapPathData(PathD& dst, const uint8_t* srcCmd, const PointF* srcPts, size_t srcLength, uint32_t cntOp = CONTAINER_OP_REPLACE) const
  {
    return _api.transformd_mapPathDataF(this, &dst, srcCmd, srcPts, srcLength, cntOp);
  }

  FOG_INLINE err_t mapPathData(PathD& dst, const uint8_t* srcCmd, const PointD* srcPts, size_t srcLength, uint32_t cntOp = CONTAINER_OP_REPLACE) const
  {
    return _api.transformd_mapPathDataD(this, &dst, srcCmd, srcPts, srcLength, cntOp);
  }

  FOG_INLINE void mapBox(BoxD& dst, const BoxD& src) const
  {
    _api.transformd_mapBoxD(this, &dst, &src);
  }

  FOG_INLINE void mapVector(PointD& pt) const
  {
    _api.transformd_mapVectorD(this, &pt, &pt);
  }

  FOG_INLINE void mapVector(PointD& dst, const PointD& src) const
  {
    _api.transformd_mapVectorD(this, &dst, &src);
  }

  // --------------------------------------------------------------------------
  // [Auxiliary]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isIdentity() const { return getType() == TRANSFORM_TYPE_IDENTITY; }
  FOG_INLINE bool isTranslation() const { return getType() == TRANSFORM_TYPE_TRANSLATION; }

  FOG_INLINE bool isAffine() const { return getType() <= TRANSFORM_TYPE_AFFINE; }
  FOG_INLINE bool isProjection() const { return getType() >= TRANSFORM_TYPE_PROJECTION; }
  FOG_INLINE bool isDegenerate() const { return getType() >= TRANSFORM_TYPE_DEGENERATE; }
  FOG_INLINE bool isInvertible() const { return getType() != TRANSFORM_TYPE_DEGENERATE; }

  FOG_INLINE double getDeterminant() const
  {
    if (getType() <= TRANSFORM_TYPE_AFFINE)
    {
      return (_00 * _11 - _01 * _10);
    }
    else
    {
      return (_22 * _11 - _21 * _12) * _00 -
             (_22 * _01 - _21 * _02) * _10 +
             (_12 * _01 - _11 * _02) * _20 ;
    }
  }

  //! @brief Get translation part of transform.
  FOG_INLINE PointD getTranslation() const { return PointD(_20, _21); }
  FOG_INLINE PointD getScaling(bool absolute) const { return _api.transformd_getScaling(this, absolute); }
  FOG_INLINE double getRotation() const { return _api.transformd_getRotation(this); }

  //! @brief Get the average scale (by X and Y).
  //!
  //! Basically used to calculate the approximation scale when decomposinting
  //! curves into line segments.
  FOG_INLINE double getAverageScaling() const { return _api.transformd_getAverageScaling(this); }

  // --------------------------------------------------------------------------
  // [HashCode]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getHashCode() const
  {
    return HashUtil::hashVectorQ(_data, 9);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE TransformD& operator=(const TransformD& other)
  {
    setTransform(other);
    return *this;
  }

  FOG_INLINE TransformD& operator=(const TransformF& other)
  {
    setTransform(other);
    return *this;
  }

  //! @brief Multiply the transform by another one.
  FOG_INLINE TransformD& operator*=(const TransformD& m)
  {
    multiply(m);
    return *this;
  }

  //! @brief Multiply the transform by inverse of another one.
  FOG_INLINE TransformD& operator/=(const TransformD& m)
  {
    multiplyInv(m);
    return *this;
  }

  //! @brief Multiply the transform by another one and return
  //! the result in a separete transform.
  FOG_INLINE TransformD operator*(const TransformD& m) const
  {
    return multiplied(m);
  }

  //! @brief Multiply the transform by inverse of another one
  //! and return the result in a separete transform.
  FOG_INLINE TransformD operator/(const TransformD& m) const
  {
    return multipliedInv(m);
  }

  //! @brief Calculate and return the inverse transform.
  FOG_INLINE TransformD operator~() const
  {
    return inverted();
  }

  FOG_INLINE bool operator==(const TransformD& other) const { return  MemOps::eq_s<9 * sizeof(double)>(_data, other._data); }
  FOG_INLINE bool operator!=(const TransformD& other) const { return !MemOps::eq_s<9 * sizeof(double)>(_data, other._data); }

  FOG_INLINE TransformD& operator+=(double val)
  {
    _00 += val; _01 += val; _02 += val;
    _10 += val; _11 += val; _12 += val;
    _20 += val; _21 += val; _22 += val;

    _type = TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY;
    return *this;
  }

  FOG_INLINE TransformD& operator-=(double val)
  {
    _00 -= val; _01 -= val; _02 -= val;
    _10 -= val; _11 -= val; _12 -= val;
    _20 -= val; _21 -= val; _22 -= val;

    _type = TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY;
    return *this;
  }

  FOG_INLINE TransformD& operator*=(double val)
  {
    _00 *= val; _01 *= val; _02 *= val;
    _10 *= val; _11 *= val; _12 *= val;
    _20 *= val; _21 *= val; _22 *= val;

    _type = TRANSFORM_TYPE_DEGENERATE | TRANSFORM_TYPE_DIRTY;
    return *this;
  }

  FOG_INLINE TransformD& operator/=(double val)
  {
    if (val != 0.0)
    {
      double recip = 1.0 / val;
      return operator*=(recip);
    }
    else
    {
      return *this;
    }
  }

  FOG_INLINE const double& operator[](size_t i) const { return _data[i]; }
  FOG_INLINE double& operator[](size_t i) { return _data[i]; }

  // --------------------------------------------------------------------------
  // [Statics - Instances]
  // --------------------------------------------------------------------------

  static FOG_INLINE const TransformD& identity()
  {
    return *_api.transformd_oIdentity;
  }

  // --------------------------------------------------------------------------
  // [Statics - Multiply]
  // --------------------------------------------------------------------------

  static FOG_INLINE void multiply(TransformD& dst, const TransformD& a, const TransformD& b)
  {
    _api.transformd_multiply(&dst, &a, &b);
  }

  // --------------------------------------------------------------------------
  // [Statics - Invert]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool invert(TransformD& dst, const TransformD& a)
  {
    return _api.transformd_invert(&dst, &a);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Matrix type.
  mutable uint32_t _type;

  //! @brief Matrix data.
  //!
  //! @verbatim
  //! Indexed values:
  //!
  //!   [M00 M01 M02]
  //!   [M10 M11 M12]
  //!   [M20 M21 M22]
  //!
  //! Named values:
  //!
  //!   [ScaleX ShearY W0]
  //!   [ShearX ScaleY W1]
  //!   [TransX TransY W2]
  //! @endverbatim
  union
  {
    double _data[9];
    struct { double _00, _01, _02, _10, _11, _12, _20, _21, _22; };
  };
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE err_t TransformF::setTransform(const TransformF& other)
{
  _setData(other._type,
    other._00, other._01, other._02,
    other._10, other._11, other._12,
    other._20, other._21, other._22);
  return ERR_OK;
}

FOG_INLINE err_t TransformF::setTransform(const TransformD& other)
{
  _setData(other._type | TRANSFORM_TYPE_DIRTY,
    (float)other._00, (float)other._01, (float)other._02,
    (float)other._10, (float)other._11, (float)other._12,
    (float)other._20, (float)other._21, (float)other._22);
  return ERR_OK;
}

// ============================================================================
// [Fog::ParallelogramParamsT<> / Fog::QuadToQuadParamsT<> / Fog::TransformT<>]
// ============================================================================

_FOG_NUM_T(ParallelogramParams)
_FOG_NUM_T(QuadToQuadParams)
_FOG_NUM_T(Transform)
_FOG_NUM_F(ParallelogramParams)
_FOG_NUM_F(QuadToQuadParams)
_FOG_NUM_F(Transform)
_FOG_NUM_D(ParallelogramParams)
_FOG_NUM_D(QuadToQuadParams)
_FOG_NUM_D(Transform)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_TRANSFORM_H
