// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_SHAPE_H
#define _FOG_G2D_GEOMETRY_SHAPE_H

// [Dependencies]
#include <Fog/G2d/Geometry/Arc.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/CBezier.h>
#include <Fog/G2d/Geometry/Chord.h>
#include <Fog/G2d/Geometry/Circle.h>
#include <Fog/G2d/Geometry/Ellipse.h>
#include <Fog/G2d/Geometry/Line.h>
#include <Fog/G2d/Geometry/Pie.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/PointArray.h>
#include <Fog/G2d/Geometry/QBezier.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/RectArray.h>
#include <Fog/G2d/Geometry/Round.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::ShapeF]
// ============================================================================

struct FOG_NO_EXPORT ShapeF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ShapeF() :
    _type(SHAPE_TYPE_NONE),
    _data(NULL)
  {
  }

  FOG_INLINE ShapeF(uint32_t type, const void* data) :
    _type(type),
    _data(data)
  {
  }

  explicit ShapeF(const LineF*     data) : _type(SHAPE_TYPE_LINE    ), _data(data) {}
  explicit ShapeF(const QBezierF*  data) : _type(SHAPE_TYPE_QBEZIER ), _data(data) {}
  explicit ShapeF(const CBezierF*  data) : _type(SHAPE_TYPE_CBEZIER ), _data(data) {}
  explicit ShapeF(const ArcF*      data) : _type(SHAPE_TYPE_ARC     ), _data(data) {}
  explicit ShapeF(const RectF*     data) : _type(SHAPE_TYPE_RECT    ), _data(data) {}
  explicit ShapeF(const RoundF*    data) : _type(SHAPE_TYPE_ROUND   ), _data(data) {}
  explicit ShapeF(const CircleF*   data) : _type(SHAPE_TYPE_CIRCLE  ), _data(data) {}
  explicit ShapeF(const EllipseF*  data) : _type(SHAPE_TYPE_ELLIPSE ), _data(data) {}
  explicit ShapeF(const ChordF*    data) : _type(SHAPE_TYPE_CHORD   ), _data(data) {}
  explicit ShapeF(const PieF*      data) : _type(SHAPE_TYPE_PIE     ), _data(data) {}
  explicit ShapeF(const TriangleF* data) : _type(SHAPE_TYPE_TRIANGLE), _data(data) {}
  explicit ShapeF(const PathF*     data) : _type(SHAPE_TYPE_PATH    ), _data(data) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _type; }
  FOG_INLINE bool isClosed() const { return _type >= SHAPE_TYPE_RECT; }

  FOG_INLINE const void* getData() const { return _data; }

  FOG_INLINE void setShape(uint32_t type, const void* data)
  {
    _type = type;
    _data = data;
  }

  // --------------------------------------------------------------------------
  // [BoundingBox]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getBoundingBox(BoxF& dst) const
  {
    return fog_api.shapef_getBoundingBox(_type, _data, &dst, NULL);
  }

  FOG_INLINE err_t getBoundingBox(BoxF& dst, const TransformF& tr) const
  {
    return fog_api.shapef_getBoundingBox(_type, _data, &dst, &tr);
  }

  FOG_INLINE err_t getBoundingBox(BoxF& dst, const TransformF* tr) const
  {
    return fog_api.shapef_getBoundingBox(_type, _data, &dst, tr);
  }

  FOG_INLINE err_t getBoundingRect(RectF& dst) const
  {
    return getBoundingRect(dst, NULL);
  }

  FOG_INLINE err_t getBoundingRect(RectF& dst, const TransformF& tr) const
  {
    return getBoundingRect(dst, &tr);
  }

  FOG_INLINE err_t getBoundingRect(RectF& dst, const TransformF* tr) const
  {
    err_t err = fog_api.shapef_getBoundingBox(_type, &_data, reinterpret_cast<BoxF*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointF& pt, uint32_t fillRule) const
  {
    return fog_api.shapef_hitTest(_type, _data, &pt, fillRule);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t getBoundingBox(uint32_t shapeType, const void* shapeData,
    BoxF* dst, const TransformF* transform = NULL)
  {
    return fog_api.shapef_getBoundingBox(shapeType, shapeData, dst, transform);
  }

  static FOG_INLINE bool hitTest(uint32_t shapeType, const void* shapeData,
    const PointF* pt, uint32_t fillRule)
  {
    return fog_api.shapef_hitTest(shapeType, shapeData, pt, fillRule);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _type;
  const void* _data;
};

// ============================================================================
// [Fog::ShapeD]
// ============================================================================

struct FOG_NO_EXPORT ShapeD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ShapeD() :
    _type(SHAPE_TYPE_NONE),
    _data(NULL)
  {
  }

  FOG_INLINE ShapeD(uint32_t type, const void* data) :
    _type(type),
    _data(data)
  {
  }

  explicit ShapeD(const LineD*     data) : _type(SHAPE_TYPE_LINE    ), _data(data) {}
  explicit ShapeD(const QBezierD*  data) : _type(SHAPE_TYPE_QBEZIER ), _data(data) {}
  explicit ShapeD(const CBezierD*  data) : _type(SHAPE_TYPE_CBEZIER ), _data(data) {}
  explicit ShapeD(const ArcD*      data) : _type(SHAPE_TYPE_ARC     ), _data(data) {}
  explicit ShapeD(const RectD*     data) : _type(SHAPE_TYPE_RECT    ), _data(data) {}
  explicit ShapeD(const RoundD*    data) : _type(SHAPE_TYPE_ROUND   ), _data(data) {}
  explicit ShapeD(const CircleD*   data) : _type(SHAPE_TYPE_CIRCLE  ), _data(data) {}
  explicit ShapeD(const EllipseD*  data) : _type(SHAPE_TYPE_ELLIPSE ), _data(data) {}
  explicit ShapeD(const ChordD*    data) : _type(SHAPE_TYPE_CHORD   ), _data(data) {}
  explicit ShapeD(const PieD*      data) : _type(SHAPE_TYPE_PIE     ), _data(data) {}
  explicit ShapeD(const TriangleD* data) : _type(SHAPE_TYPE_TRIANGLE), _data(data) {}
  explicit ShapeD(const PathD*     data) : _type(SHAPE_TYPE_PATH    ), _data(data) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _type; }
  FOG_INLINE bool isClosed() const { return _type >= SHAPE_TYPE_RECT; }

  FOG_INLINE const void* getData() const { return _data; }

  FOG_INLINE void setShape(uint32_t type, const void* data)
  {
    _type = type;
    _data = data;
  }

  // --------------------------------------------------------------------------
  // [BoundingBox]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getBoundingBox(BoxD& dst) const
  {
    return fog_api.shaped_getBoundingBox(_type, _data, &dst, NULL);
  }

  FOG_INLINE err_t getBoundingBox(BoxD& dst, const TransformD& transform) const
  {
    return fog_api.shaped_getBoundingBox(_type, _data, &dst, &transform);
  }

  FOG_INLINE err_t getBoundingRect(RectD& dst) const
  {
    return getBoundingRect(dst, NULL);
  }

  FOG_INLINE err_t getBoundingRect(RectD& dst, const TransformD& tr) const
  {
    return getBoundingRect(dst, &tr);
  }

  FOG_INLINE err_t getBoundingRect(RectD& dst, const TransformD* tr) const
  {
    err_t err = fog_api.shaped_getBoundingBox(_type, &_data, reinterpret_cast<BoxD*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointD& pt, uint32_t fillRule) const
  {
    return fog_api.shaped_hitTest(_type, _data, &pt, fillRule);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t getBoundingBox(uint32_t shapeType, const void* shapeData,
    BoxD* dst, const TransformD* transform = NULL)
  {
    return fog_api.shaped_getBoundingBox(shapeType, shapeData, dst, transform);
  }

  static FOG_INLINE bool hitTest(uint32_t shapeType, const void* shapeData,
    const PointD* pt, uint32_t fillRule)
  {
    return fog_api.shaped_hitTest(shapeType, shapeData, pt, fillRule);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _type;
  const void* _data;
};

// ============================================================================
// [Fog::ShapeT<>]
// ============================================================================

_FOG_NUM_T(Shape)
_FOG_NUM_F(Shape)
_FOG_NUM_D(Shape)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_SHAPE_H
