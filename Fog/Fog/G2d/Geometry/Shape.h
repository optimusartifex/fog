// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_SHAPE_TYPE_H
#define _FOG_G2D_GEOMETRY_SHAPE_TYPE_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/G2d/Geometry/Arc.h>
#include <Fog/G2d/Geometry/CBezier.h>
#include <Fog/G2d/Geometry/Chord.h>
#include <Fog/G2d/Geometry/Circle.h>
#include <Fog/G2d/Geometry/Ellipse.h>
#include <Fog/G2d/Geometry/Line.h>
#include <Fog/G2d/Geometry/Pie.h>
#include <Fog/G2d/Geometry/QBezier.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Round.h>
#include <Fog/G2d/Geometry/Triangle.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Global/Api.h>

// Common geometric structures:
//
//   - Point      - [x , y ].
//   - Size       - [w , h ].
//
//   - Line       - [x0, y0] -> [x1, y1].
//   - QBezier    - [x0, y0], [x1, y1], [x2, y2] - Quadric Bezier curve.
//   - CBezier    - [x0, y0], [x1, y1], [x2, y2], [x3, y3] - Cubic Bezier curve.
//
//   - Box        - [x0, y0] -> [x1, y1].
//   - Rect       - [x , y ] -> [w , h ].
//   - Round      - [x , y ] -> [w , h ] && [rx, ry].
//
//   - Circle     - [cx, cy] && [r].
//   - Ellipse    - [cx, cy] && [rx, ry].
//   - Arc        - [cx, cy] && [rx, ry] && angle and sweep.
//   - Chord      - [cx, cy] && [rx, ry] && angle and sweep.
//   - Pie        - [cx, cy] && [rx, ry] && angle and sweep.
//   - Triangle   - [x0, y0] -> [x1, y1] -> [x2, y2]
//
// The implementation for several data-types exists:
//   - I - All members as 32-bit integers - int.
//   - F - All members as 32-bit floats   - float.
//   - D - All members as 64-bit floats   - double.
//
// Each datatype contains implicit conversion from less-precision and
// high-precision types. These structures are very simple so the members
// are not prefixed with underscore and all members are public.

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::ShapeDataF]
// ============================================================================

union ShapeDataF
{
  // --------------------------------------------------------------------------
  // [Unclosed]
  // --------------------------------------------------------------------------

  Static<LineF> line;
  Static<QBezierF> qbezier;
  Static<CBezierF> cbezier;
  Static<ArcF> arc;

  // --------------------------------------------------------------------------
  // [Closed]
  // --------------------------------------------------------------------------

  Static<RectF> rect;
  Static<RoundF> round;
  Static<CircleF> circle;
  Static<EllipseF> ellipse;
  Static<ChordF> chord;
  Static<PieF> pie;
  Static<TriangleF> triangle;
};

// ============================================================================
// [Fog::ShapeDataD]
// ============================================================================

union ShapeDataD
{
  // --------------------------------------------------------------------------
  // [Unclosed]
  // --------------------------------------------------------------------------

  Static<LineD> line;
  Static<QBezierD> qbezier;
  Static<CBezierD> cbezier;
  Static<ArcD> arc;

  // --------------------------------------------------------------------------
  // [Closed]
  // --------------------------------------------------------------------------

  Static<RectD> rect;
  Static<RoundD> round;
  Static<CircleD> circle;
  Static<EllipseD> ellipse;
  Static<ChordD> chord;
  Static<PieD> pie;
  Static<TriangleD> triangle;
};

// ============================================================================
// [Fog::ShapeF]
// ============================================================================

//! @brief Shape is data structure that can hold any geometric primitive.
struct FOG_NO_EXPORT ShapeF
{
  // --------------------------------------------------------------------------
  // [Construction & Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ShapeF() { _type = SHAPE_TYPE_NONE; }
  FOG_INLINE ShapeF(const ShapeF& other) { _type = other._type; Memory::copy(&_data, &other._data, sizeof(ShapeDataF)); }
  FOG_INLINE ShapeF(_Uninitialized) {}

  explicit FOG_INLINE ShapeF(const LineF& line) { _type = SHAPE_TYPE_LINE; _data.line.instance() = line; }
  explicit FOG_INLINE ShapeF(const QBezierF& qbezier) { _type = SHAPE_TYPE_QBEZIER; _data.qbezier.instance() = qbezier; }
  explicit FOG_INLINE ShapeF(const CBezierF& cbezier) { _type = SHAPE_TYPE_CBEZIER; _data.cbezier.instance() = cbezier; }
  explicit FOG_INLINE ShapeF(const ArcF& arc) { _type = SHAPE_TYPE_ARC; _data.arc.instance() = arc; }

  explicit FOG_INLINE ShapeF(const BoxF& box) { _type = SHAPE_TYPE_RECT; _data.rect.instance() = box; }
  explicit FOG_INLINE ShapeF(const RectF& rect) { _type = SHAPE_TYPE_RECT; _data.rect.instance() = rect; }
  explicit FOG_INLINE ShapeF(const RoundF& round) { _type = SHAPE_TYPE_ROUND; _data.round.instance() = round; }
  explicit FOG_INLINE ShapeF(const CircleF& circle) { _type = SHAPE_TYPE_CIRCLE; _data.circle.instance() = circle; }
  explicit FOG_INLINE ShapeF(const EllipseF& ellipse) { _type = SHAPE_TYPE_ELLIPSE; _data.ellipse.instance() = ellipse; }
  explicit FOG_INLINE ShapeF(const ChordF& chord) { _type = SHAPE_TYPE_CHORD; _data.chord.instance() = chord; }
  explicit FOG_INLINE ShapeF(const PieF& pie) { _type = SHAPE_TYPE_PIE; _data.pie.instance() = pie; }
  explicit FOG_INLINE ShapeF(const TriangleF& triangle) { _type = SHAPE_TYPE_TRIANGLE; _data.triangle.instance() = triangle; }

  // --------------------------------------------------------------------------
  // [Accessors - Type]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _type; }
  FOG_INLINE bool isClosed() const { return _type >= SHAPE_TYPE_RECT; }

  // --------------------------------------------------------------------------
  // [Accessors - Data]
  // --------------------------------------------------------------------------

  FOG_INLINE const ShapeDataF* getData() const { return &_data; }
  FOG_INLINE ShapeDataF* getData() { return &_data; }

  // --------------------------------------------------------------------------
  // [Accessors - Unclosed]
  // --------------------------------------------------------------------------

  FOG_INLINE LineF& getLine() { return _data.line.instance(); }
  FOG_INLINE const LineF& getLine() const { return _data.line.instance(); }

  FOG_INLINE QBezierF& getQBezier() { return _data.qbezier.instance(); }
  FOG_INLINE const QBezierF& getQBezier() const { return _data.qbezier.instance(); }

  FOG_INLINE CBezierF& getCBezier() { return _data.cbezier.instance(); }
  FOG_INLINE const CBezierF& getCBezier() const { return _data.cbezier.instance(); }

  FOG_INLINE ArcF& getArc() { return _data.arc.instance(); }
  FOG_INLINE const ArcF& getArc() const { return _data.arc.instance(); }

  FOG_INLINE void setLine(const LineF& line) { _type = SHAPE_TYPE_LINE; _data.line.instance() = line; }
  FOG_INLINE void setQBezier(const QBezierF& quad) { _type = SHAPE_TYPE_LINE; _data.qbezier.instance() = quad; }
  FOG_INLINE void setCBezier(const CBezierF& cubic) { _type = SHAPE_TYPE_LINE; _data.cbezier.instance() = cubic; }
  FOG_INLINE void setArc(const ArcF& arc) { _type = SHAPE_TYPE_ARC; _data.arc.instance() = arc; }

  // --------------------------------------------------------------------------
  // [Accessors - Closed]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF& getRect() { return _data.rect.instance(); }
  FOG_INLINE const RectF& getRect() const { return _data.rect.instance(); }

  FOG_INLINE RoundF& getRound() { return _data.round.instance(); }
  FOG_INLINE const RoundF& getRound() const { return _data.round.instance(); }

  FOG_INLINE CircleF& getCircle() { return _data.circle.instance(); }
  FOG_INLINE const CircleF& getCircle() const { return _data.circle.instance(); }

  FOG_INLINE EllipseF& getEllipse() { return _data.ellipse.instance(); }
  FOG_INLINE const EllipseF& getEllipse() const { return _data.ellipse.instance(); }

  FOG_INLINE ChordF& getChord() { return _data.chord.instance(); }
  FOG_INLINE const ChordF& getChord() const { return _data.chord.instance(); }

  FOG_INLINE PieF& getPie() { return _data.pie.instance(); }
  FOG_INLINE const PieF& getPie() const { return _data.pie.instance(); }

  FOG_INLINE TriangleF& getTriangle() { return _data.triangle.instance(); }
  FOG_INLINE const TriangleF& getTriangle() const { return _data.triangle.instance(); }

  FOG_INLINE void setBox(const BoxF& box) { _type = SHAPE_TYPE_RECT; _data.rect.instance() = box; }
  FOG_INLINE void setRect(const RectF& rect) { _type = SHAPE_TYPE_RECT; _data.rect.instance() = rect; }
  FOG_INLINE void setRound(const RoundF& round) { _type = SHAPE_TYPE_ROUND; _data.round.instance() = round; }
  FOG_INLINE void setCircle(const CircleF& circle) { _type = SHAPE_TYPE_CIRCLE; _data.circle.instance() = circle; }
  FOG_INLINE void setEllipse(const EllipseF& ellipse) { _type = SHAPE_TYPE_ELLIPSE; _data.ellipse.instance() = ellipse; }
  FOG_INLINE void setChord(const ChordF& chord) { _type = SHAPE_TYPE_CHORD; _data.chord.instance() = chord; }
  FOG_INLINE void setPie(const PieF& pie) { _type = SHAPE_TYPE_PIE; _data.pie.instance() = pie; }
  FOG_INLINE void setTriangle(const TriangleF& triangle) { _type = SHAPE_TYPE_TRIANGLE; _data.triangle.instance() = triangle; }

  // --------------------------------------------------------------------------
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getBoundingBox(BoxF& dst) const
  {
    return _getBoundingBox(dst, NULL);
  }

  FOG_INLINE err_t getBoundingBox(BoxF& dst, const TransformF& tr) const
  {
    return _getBoundingBox(dst, &tr);
  }

  FOG_INLINE err_t getBoundingRect(RectF& dst) const
  {
    return _getBoundingRect(dst, NULL);
  }

  FOG_INLINE err_t getBoundingRect(RectF& dst, const TransformF& tr) const
  {
    return _getBoundingRect(dst, &tr);
  }

  FOG_INLINE err_t _getBoundingBox(BoxF& dst, const TransformF* tr) const
  {
    return _g2d.shapef.getBoundingBox(_type, &_data, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectF& dst, const TransformF* tr) const
  {
    err_t err = _g2d.shapef.getBoundingBox(_type, &_data, reinterpret_cast<BoxF*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointF& pt) const
  {
    return _g2d.shapef.hitTest(_type, &_data, &pt);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  ShapeF& operator=(const ShapeF& other)
  {
    _type = other._type;
    Memory::copy(&_data, &other._data, sizeof(ShapeDataF));
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _type;
  ShapeDataF _data;
};

// ============================================================================
// [Fog::ShapeD]
// ============================================================================

//! @brief Shape is data structure that can hold any geometric primitive.
struct FOG_NO_EXPORT ShapeD
{
  // --------------------------------------------------------------------------
  // [Construction & Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ShapeD() { _type = SHAPE_TYPE_NONE; }
  FOG_INLINE ShapeD(const ShapeD& other) { _type = other._type; Memory::copy(&_data, &other._data, sizeof(ShapeDataD)); }
  FOG_INLINE ShapeD(_Uninitialized) {}

  explicit FOG_INLINE ShapeD(const LineD& line) { _type = SHAPE_TYPE_LINE; _data.line.instance() = line; }
  explicit FOG_INLINE ShapeD(const QBezierD& quad) { _type = SHAPE_TYPE_LINE; _data.qbezier.instance() = quad; }
  explicit FOG_INLINE ShapeD(const CBezierD& cubic) { _type = SHAPE_TYPE_LINE; _data.cbezier.instance() = cubic; }
  explicit FOG_INLINE ShapeD(const ArcD& arc) { _type = SHAPE_TYPE_ARC; _data.arc.instance() = arc; }

  explicit FOG_INLINE ShapeD(const BoxD& box) { _type = SHAPE_TYPE_RECT; _data.rect.instance() = box; }
  explicit FOG_INLINE ShapeD(const RectD& rect) { _type = SHAPE_TYPE_RECT; _data.rect.instance() = rect; }
  explicit FOG_INLINE ShapeD(const RoundD& round) { _type = SHAPE_TYPE_ROUND; _data.round.instance() = round; }
  explicit FOG_INLINE ShapeD(const CircleD& circle) { _type = SHAPE_TYPE_CIRCLE; _data.circle.instance() = circle; }
  explicit FOG_INLINE ShapeD(const EllipseD& ellipse) { _type = SHAPE_TYPE_ELLIPSE; _data.ellipse.instance() = ellipse; }
  explicit FOG_INLINE ShapeD(const ChordD& chord) { _type = SHAPE_TYPE_CHORD; _data.chord.instance() = chord; }
  explicit FOG_INLINE ShapeD(const PieD& pie) { _type = SHAPE_TYPE_PIE; _data.pie.instance() = pie; }
  explicit FOG_INLINE ShapeD(const TriangleD& triangle) { _type = SHAPE_TYPE_TRIANGLE; _data.triangle.instance() = triangle; }

  // --------------------------------------------------------------------------
  // [Accessors - Type]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getType() const { return _type; }
  FOG_INLINE bool isClosed() const { return _type >= SHAPE_TYPE_RECT; }

  // --------------------------------------------------------------------------
  // [Accessors - Data]
  // --------------------------------------------------------------------------

  FOG_INLINE const ShapeDataD* getData() const { return &_data; }
  FOG_INLINE ShapeDataD* getData() { return &_data; }

  // --------------------------------------------------------------------------
  // [Accessors - Unclosed]
  // --------------------------------------------------------------------------

  FOG_INLINE LineD& getLine() { return _data.line.instance(); }
  FOG_INLINE const LineD& getLine() const { return _data.line.instance(); }

  FOG_INLINE QBezierD& getQBezier() { return _data.qbezier.instance(); }
  FOG_INLINE const QBezierD& getQBezier() const { return _data.qbezier.instance(); }

  FOG_INLINE CBezierD& getCBezier() { return _data.cbezier.instance(); }
  FOG_INLINE const CBezierD& getCBezier() const { return _data.cbezier.instance(); }

  FOG_INLINE ArcD& getArc() { return _data.arc.instance(); }
  FOG_INLINE const ArcD& getArc() const { return _data.arc.instance(); }

  FOG_INLINE void setLine(const LineD& line) { _type = SHAPE_TYPE_LINE; _data.line.instance() = line; }
  FOG_INLINE void setQBezier(const QBezierD& quad) { _type = SHAPE_TYPE_QBEZIER; _data.qbezier.instance() = quad; }
  FOG_INLINE void setCBezier(const CBezierD& cubic) { _type = SHAPE_TYPE_CBEZIER; _data.cbezier.instance() = cubic; }
  FOG_INLINE void setArc(const ArcD& arc) { _type = SHAPE_TYPE_ARC; _data.arc.instance() = arc; }

  // --------------------------------------------------------------------------
  // [Accessors - Closed]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD& getRect() { return _data.rect.instance(); }
  FOG_INLINE const RectD& getRect() const { return _data.rect.instance(); }

  FOG_INLINE RoundD& getRound() { return _data.round.instance(); }
  FOG_INLINE const RoundD& getRound() const { return _data.round.instance(); }

  FOG_INLINE CircleD& getCircle() { return _data.circle.instance(); }
  FOG_INLINE const CircleD& getCircle() const { return _data.circle.instance(); }

  FOG_INLINE EllipseD& getEllipse() { return _data.ellipse.instance(); }
  FOG_INLINE const EllipseD& getEllipse() const { return _data.ellipse.instance(); }

  FOG_INLINE ChordD& getChord() { return _data.chord.instance(); }
  FOG_INLINE const ChordD& getChord() const { return _data.chord.instance(); }

  FOG_INLINE PieD& getPie() { return _data.pie.instance(); }
  FOG_INLINE const PieD& getPie() const { return _data.pie.instance(); }

  FOG_INLINE TriangleD& getTriangle() { return _data.triangle.instance(); }
  FOG_INLINE const TriangleD& getTriangle() const { return _data.triangle.instance(); }

  FOG_INLINE void setBox(const BoxD& box) { _type = SHAPE_TYPE_RECT; _data.rect.instance() = box; }
  FOG_INLINE void setRect(const RectD& rect) { _type = SHAPE_TYPE_RECT; _data.rect.instance() = rect; }
  FOG_INLINE void setRound(const RoundD& round) { _type = SHAPE_TYPE_ROUND; _data.round.instance() = round; }

  FOG_INLINE void setCircle(const CircleD& circle) { _type = SHAPE_TYPE_CIRCLE; _data.circle.instance() = circle; }
  FOG_INLINE void setEllipse(const EllipseD& ellipse) { _type = SHAPE_TYPE_ELLIPSE; _data.ellipse.instance() = ellipse; }
  FOG_INLINE void setChord(const ChordD& chord) { _type = SHAPE_TYPE_CHORD; _data.chord.instance() = chord; }
  FOG_INLINE void setPie(const PieD& pie) { _type = SHAPE_TYPE_PIE; _data.pie.instance() = pie; }
  FOG_INLINE void setTriangle(const TriangleD& triangle) { _type = SHAPE_TYPE_TRIANGLE; _data.triangle.instance() = triangle; }

  // --------------------------------------------------------------------------
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getBoundingBox(BoxD& dst) const
  {
    return _getBoundingBox(dst, NULL);
  }

  FOG_INLINE err_t getBoundingBox(BoxD& dst, const TransformD& tr) const
  {
    return _getBoundingBox(dst, &tr);
  }

  FOG_INLINE err_t getBoundingRect(RectD& dst) const
  {
    return _getBoundingRect(dst, NULL);
  }

  FOG_INLINE err_t getBoundingRect(RectD& dst, const TransformD& tr) const
  {
    return _getBoundingRect(dst, &tr);
  }

  FOG_INLINE err_t _getBoundingBox(BoxD& dst, const TransformD* tr) const
  {
    return _g2d.shaped.getBoundingBox(_type, &_data, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectD& dst, const TransformD* tr) const
  {
    err_t err = _g2d.shaped.getBoundingBox(_type, &_data, reinterpret_cast<BoxD*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointD& pt) const
  {
    return _g2d.shaped.hitTest(_type, &_data, &pt);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  ShapeD& operator=(const ShapeD& other)
  {
    _type = other._type;
    Memory::copy(&_data, &other._data, sizeof(ShapeDataD));
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _type;
  ShapeDataD _data;
};

// ============================================================================
// [Fog::ShapeT<>]
// ============================================================================

FOG_CLASS_PRECISION_F_D(Shape)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_SHAPE_TYPE_H
