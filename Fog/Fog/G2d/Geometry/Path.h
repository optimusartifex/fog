// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PATH_H
#define _FOG_G2D_GEOMETRY_PATH_H

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Shape.h>
#include <Fog/G2d/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct PathF;
struct PathD;
struct PathFlattenParamsF;
struct PathFlattenParamsD;

struct Region;
struct TransformF;
struct TransformD;

// ============================================================================
// [Fog::PathCmd]
// ============================================================================

//! @brief Path command helpers.
struct FOG_NO_EXPORT PathCmd
{
  //! @brief Get whether the path command is 'move-to'.
  static FOG_INLINE bool isMoveTo(uint8_t cmd) { return (cmd == PATH_CMD_MOVE_TO); }
  //! @brief Get whether the path command is 'line-to'.
  static FOG_INLINE bool isLineTo(uint8_t cmd) { return (cmd == PATH_CMD_LINE_TO); }
  //! @brief Get whether the path command is 'quad-to'.
  static FOG_INLINE bool isQuadTo(uint8_t cmd) { return (cmd == PATH_CMD_QUAD_TO); }
  //! @brief Get whether the path command is 'cubic-to'.
  static FOG_INLINE bool isCubicTo(uint8_t cmd) { return (cmd == PATH_CMD_CUBIC_TO); }
  //! @brief Get whether the path command is data related to @c PATH_CMD_QUAD_TO
  //! or @c PATH_CMD_CUBIC_TO.
  static FOG_INLINE bool isData(uint8_t cmd) { return (cmd == PATH_CMD_DATA); }
  //! @brief Get whether the path command is 'close'.
  static FOG_INLINE bool isClose(uint8_t cmd) { return (cmd == PATH_CMD_CLOSE); }

  //! @brief Get whether the path command is 'move-to' or 'line-to'.
  static FOG_INLINE bool isMoveOrLineTo(uint8_t cmd) { return ((cmd | 0x01) == PATH_CMD_LINE_TO); }
  //! @brief Get whether the path command is 'quad-to' or 'cubic-to'.
  static FOG_INLINE bool isQuadOrCubicTo(uint8_t cmd) { return ((cmd | 0x01) == PATH_CMD_CUBIC_TO); }

  //! @brief Get whether the path command is a vertex.
  static FOG_INLINE bool isVertex(uint8_t cmd) { return (cmd != PATH_CMD_CLOSE); }
};

// ============================================================================
// [Fog::PathFlattenParamsF]
// ============================================================================

struct FOG_NO_EXPORT PathFlattenParamsF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathFlattenParamsF(
    float flatness = Math2dConst<float>::getDefaultFlatness(),
    const TransformF* transform = NULL,
    const BoxF* clipBox = NULL)
    :
    _flatness(flatness),
    _transform(transform),
    _clipBox(clipBox)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasTransform() const { return _transform != NULL; }
  FOG_INLINE bool hasClipBox() const { return _clipBox != NULL; }

  FOG_INLINE float getFlatness() const { return _flatness; }
  FOG_INLINE void setFlatness(float flatness) { _flatness = flatness; }

  FOG_INLINE const TransformF* getTransform() const { return _transform; }
  FOG_INLINE void setTransform(const TransformF* transform) { _transform = transform; }

  FOG_INLINE const BoxF* getClipBox() const { return _clipBox; }
  FOG_INLINE void setClipBox(const BoxF* clipBox) { _clipBox = clipBox; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _flatness;
  const TransformF* _transform;
  const BoxF* _clipBox;
};

// ============================================================================
// [Fog::PathFlattenParamsD]
// ============================================================================

struct FOG_NO_EXPORT PathFlattenParamsD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathFlattenParamsD(
    double flatness = Math2dConst<double>::getDefaultFlatness(),
    const TransformD* transform = NULL,
    const BoxD* clipBox = NULL)
    :
    _flatness(flatness),
    _transform(transform),
    _clipBox(clipBox)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasTransform() const { return _transform != NULL; }
  FOG_INLINE bool hasClipBox() const { return _clipBox != NULL; }

  FOG_INLINE double getFlatness() const { return _flatness; }
  FOG_INLINE void setFlatness(double flatness) { _flatness = flatness; }

  FOG_INLINE const TransformD* getTransform() const { return _transform; }
  FOG_INLINE void setTransform(const TransformD* transform) { _transform = transform; }

  FOG_INLINE const BoxD* getClipBox() const { return _clipBox; }
  FOG_INLINE void setClipBox(const BoxD* clipBox) { _clipBox = clipBox; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double _flatness;
  const TransformD* _transform;
  const BoxD* _clipBox;
};

// ============================================================================
// [Fog::PathDataF]
// ============================================================================

//! @brief Path data (float)
struct FOG_NO_EXPORT PathDataF
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasBoundingBox() const
  {
    return (flags & (PATH_DATA_DIRTY_BOUNDING_BOX | PATH_DATA_HAS_BOUNDING_BOX)) == PATH_DATA_HAS_BOUNDING_BOX;
  }

  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  FOG_INLINE PathDataF* ref() const
  {
    refCount.inc();
    return const_cast<PathDataF*>(this);
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref() && !(flags & PATH_DATA_STATIC)) Memory::free(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;
  //! @brief Flags (see @c PATH_DATA).
  volatile uint32_t flags;
#if FOG_ARCH_BITS >= 64
  uint32_t padding;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Path capacity (allocated space for vertices).
  sysuint_t capacity;
  //! @brief Path length (count of vertices used).
  sysuint_t length;

  //! @brief Path bounding box.
  BoxF boundingBox;

  //! @brief Vertices data (aligned to 16 bytes).
  PointF* vertices;
  //! @brief Commands data (plus space for aligning).
  uint8_t commands[16];
};

// ============================================================================
// [Fog::PathDataD]
// ============================================================================

//! @brief Path data (double)
struct FOG_NO_EXPORT PathDataD
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasBoundingBox() const
  {
    return (flags & (PATH_DATA_DIRTY_BOUNDING_BOX | PATH_DATA_HAS_BOUNDING_BOX)) == PATH_DATA_HAS_BOUNDING_BOX;
  }

  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  FOG_INLINE PathDataD* ref() const
  {
    refCount.inc();
    return const_cast<PathDataD*>(this);
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref() && !(flags & PATH_DATA_STATIC)) Memory::free(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;
  //! @brief Flags (see @c PATH_DATA).
  volatile uint32_t flags;
#if FOG_ARCH_BITS >= 64
  uint32_t padding;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Path capacity (allocated space for vertices).
  sysuint_t capacity;
  //! @brief Path length (count of vertices used).
  sysuint_t length;

  //! @brief Path bounding box.
  BoxD boundingBox;

  //! @brief Vertices data (aligned to 16 bytes).
  PointD* vertices;
  //! @brief Commands data (plus space for aligning).
  uint8_t commands[16];
};

// ============================================================================
// [Fog::PathF]
// ============================================================================

//! @brief Path (float).
struct FOG_NO_EXPORT PathF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathF()
  {
    _g2d.pathf.ctor(*this);
  }

  FOG_INLINE PathF(const PathF& other)
  {
    _g2d.pathf.ctorCopyF(*this, other);
  }

  explicit FOG_INLINE PathF(PathDataF* d) : _d(d) {}

  FOG_INLINE ~PathF()
  {
    _g2d.pathf.dtor(*this);
  }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Get path capacity (count of allocated vertices).
  FOG_INLINE sysuint_t getCapacity() const { return _d->capacity; }
  //! @brief Get path length (count of vertices used).
  FOG_INLINE sysuint_t getLength() const { return _d->length; }
  //! @brief Get whether path is empty.
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  //! @brief Get path commands array (const).
  FOG_INLINE const uint8_t* getCommands() const { return _d->commands; }
  //! @brief Get path vertices array (const).
  FOG_INLINE const PointF* getVertices() const { return _d->vertices; }

  //! @brief Get path commands array (mutable).
  FOG_INLINE uint8_t* getCommandsX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::PathF::getCommandsX() - Called on non-detached object.");
    return _d->commands;
  }

  //! @brief Get path vertices array (mutable).
  FOG_INLINE PointF* getVerticesX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::PathF::getVerticesX() - Called on non-detached object.");
    return _d->vertices;
  }

  FOG_INLINE sysuint_t getRefCount() const
  {
    return _d->refCount.get();
  }

  FOG_INLINE bool isDetached() const
  {
    return getRefCount() == 1;
  }

  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _g2d.pathf.detach(*this);
  }

  FOG_INLINE err_t reserve(sysuint_t capacity)
  {
    return _g2d.pathf.reserve(*this, capacity);
  }

  FOG_INLINE void squeeze()
  {
    return _g2d.pathf.squeeze(*this);
  }

  FOG_INLINE sysuint_t _prepare(sysuint_t count, uint32_t cntOp)
  {
    return _g2d.pathf.prepare(*this, count, cntOp);
  }

  FOG_INLINE sysuint_t _add(sysuint_t count)
  {
    return _g2d.pathf.add(*this, count);
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _g2d.pathf.clear(*this);
  }

  FOG_INLINE void reset()
  {
    _g2d.pathf.reset(*this);
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setPath(const PathF& other)
  {
    return _g2d.pathf.setPathF(*this, other);
  }

  FOG_INLINE err_t setDeep(const PathF& other)
  {
    return _g2d.pathf.setDeepF(*this, other);
  }

  // --------------------------------------------------------------------------
  // [SubPath]
  // --------------------------------------------------------------------------

  //! @brief Get range of subpath at index @a index (range is from the @a index
  //! to next 'move-to' command or to the end of the path).
  FOG_INLINE Range getSubpathRange(sysuint_t index) const
  {
    return _g2d.pathf.getSubpathRange(*this, index);
  }

  // --------------------------------------------------------------------------
  // [MoveTo]
  // --------------------------------------------------------------------------

  //! @brief Move to @a pt0 (absolute).
  FOG_INLINE err_t moveTo(const PointF& pt0)
  {
    return _g2d.pathf.moveTo(*this, pt0);
  }

  //! @brief Move to @a pt0 (relative).
  FOG_INLINE err_t moveToRel(const PointF& pt0)
  {
    return _g2d.pathf.moveToRel(*this, pt0);
  }

  // --------------------------------------------------------------------------
  // [LineTo]
  // --------------------------------------------------------------------------

  //! @brief Line to @a pt1 (absolute).
  FOG_INLINE err_t lineTo(const PointF& pt1)
  {
    return _g2d.pathf.lineTo(*this, pt1);
  }

  //! @brief Line to @a pt1 (relative).
  FOG_INLINE err_t lineToRel(const PointF& pt1)
  {
    return _g2d.pathf.lineToRel(*this, pt1);
  }

  //! @brief Horizontal line to @a x (absolute).
  FOG_INLINE err_t hlineTo(float x)
  {
    return _g2d.pathf.hlineTo(*this, x);
  }

  //! @brief Horizontal line to @a x (relative).
  FOG_INLINE err_t hlineToRel(float x)
  {
    return _g2d.pathf.hlineToRel(*this, x);
  }

  //! @brief Horizontal line to @a y (absolute).
  FOG_INLINE err_t vlineTo(float y)
  {
    return _g2d.pathf.vlineTo(*this, y);
  }

  //! @brief Horizontal line to @a y (relative).
  FOG_INLINE err_t vlineToRel(float y)
  {
    return _g2d.pathf.vlineToRel(*this, y);
  }

  // --------------------------------------------------------------------------
  // [PolyTo]
  // --------------------------------------------------------------------------

  //! @brief Polyline to @a pts (absolute).
  FOG_INLINE err_t polyTo(const PointF* pts, sysuint_t count)
  {
    return _g2d.pathf.polyTo(*this, pts, count);
  }

  //! @brief Polyline to @a pts (relative).
  FOG_INLINE err_t polyToRel(const PointF* pts, sysuint_t count)
  {
    return _g2d.pathf.polyToRel(*this, pts, count);
  }

  // --------------------------------------------------------------------------
  // [QuadTo]
  // --------------------------------------------------------------------------

  //! @brief Quadratic curve to @a pt1, and @a pt2 (absolute).
  FOG_INLINE err_t quadTo(const PointF& pt1, const PointF& pt2)
  {
    return _g2d.pathf.quadTo(*this, pt1, pt2);
  }

  //! @brief Quadratic curve to @a pt1, and @a pt2 (relative).
  FOG_INLINE err_t quadToRel(const PointF& pt1, const PointF& pt2)
  {
    return _g2d.pathf.quadToRel(*this, pt1, pt2);
  }

  //! @brief Smooth quadratic curve to @a pt2, calculating pt1 from last points (absolute).
  FOG_INLINE err_t smoothQuadTo(const PointF& pt2)
  {
    return _g2d.pathf.smoothQuadTo(*this, pt2);
  }

  //! @brief Smooth quadratic curve to @a pt2, calculating pt1 from last points (relative).
  FOG_INLINE err_t smoothQuadToRel(const PointF& pt2)
  {
    return _g2d.pathf.smoothQuadToRel(*this, pt2);
  }

  // --------------------------------------------------------------------------
  // [CubicTo]
  // --------------------------------------------------------------------------

  //! @brief Cubic curve to @a pt1, @a pt2, and @a pt3 (absolute).
  FOG_INLINE err_t cubicTo(const PointF& pt1, const PointF& pt2, const PointF& pt3)
  {
    return _g2d.pathf.cubicTo(*this, pt1, pt2, pt3);
  }

  //! @brief Cubic curve to @a pt1, @a pt2, and @a pt3 (relative).
  FOG_INLINE err_t cubicToRel(const PointF& pt1, const PointF& pt2, const PointF& pt3)
  {
    return _g2d.pathf.cubicToRel(*this, pt1, pt2, pt3);
  }

  //! @brief Smooth cubic curve to @a pt2, and @a pt3, calculating pt1 from last points (absolute).
  FOG_INLINE err_t smoothCubicTo(const PointF& pt2, const PointF& pt3)
  {
    return _g2d.pathf.smoothCubicTo(*this, pt2, pt3);
  }

  //! @brief Smooth cubic curve to @a pt2, and @a pt3, calculating pt1 from last points (relative).
  FOG_INLINE err_t smoothCubicToRel(const PointF& pt2, const PointF& pt3)
  {
    return _g2d.pathf.smoothCubicToRel(*this, pt2, pt3);
  }

  // --------------------------------------------------------------------------
  // [ArcTo]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t arcTo(const PointF& cp, const PointF& rp, float start, float sweep, bool startPath = false)
  {
    return _g2d.pathf.arcTo(*this, cp, rp, start, sweep, startPath);
  }

  FOG_INLINE err_t arcToRel(const PointF& cp, const PointF& rp, float start, float sweep, bool startPath = false)
  {
    return _g2d.pathf.arcToRel(*this, cp, rp, start, sweep, startPath);
  }

  FOG_INLINE err_t svgArcTo(const PointF& rp, float angle, bool largeArcFlag, bool sweepFlag, const PointF& pt)
  {
    return _g2d.pathf.svgArcTo(*this, rp, angle, largeArcFlag, sweepFlag, pt);
  }

  FOG_INLINE err_t svgArcToRel(const PointF& rp, float angle, bool largeArcFlag, bool sweepFlag, const PointF& pt)
  {
    return _g2d.pathf.svgArcToRel(*this, rp, angle, largeArcFlag, sweepFlag, pt);
  }

  // --------------------------------------------------------------------------
  // [Close]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t close()
  {
    return _g2d.pathf.close(*this);
  }

  // --------------------------------------------------------------------------
  // [Box / Rect]
  // --------------------------------------------------------------------------

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t box(const BoxI& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.boxI(*this, r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t box(const BoxF& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.boxF(*this, r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t rect(const RectI& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.rectI(*this, r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t rect(const RectF& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.rectF(*this, r, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t boxes(const BoxI* b, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.boxesI(*this, b, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t boxes(const BoxF* b, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.boxesF(*this, b, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t rects(const RectI* r, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.rectsI(*this, r, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t rects(const RectF* r, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.rectsF(*this, r, count, direction);
  }

  // --------------------------------------------------------------------------
  // [Region]
  // --------------------------------------------------------------------------

  //! @brief Add a closed region (converted to set of rectangles).
  FOG_INLINE err_t region(const Region& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.region(*this, r, direction);
  }

  // --------------------------------------------------------------------------
  // [Polyline / Polygon]
  // --------------------------------------------------------------------------

  //! @brief Add a polyline to the path.
  FOG_INLINE err_t polyline(const PointI* pts, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.polylineI(*this, pts, count, direction);
  }

  //! @brief Add a polyline to the path.
  FOG_INLINE err_t polyline(const PointF* pts, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.polylineF(*this, pts, count, direction);
  }

  //! @brief Add a closed polygon to the path.
  FOG_INLINE err_t polygon(const PointI* pts, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.polygonI(*this, pts, count, direction);
  }

  //! @brief Add a closed polygon to the path.
  FOG_INLINE err_t polygon(const PointF* pts, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathf.polygonF(*this, pts, count, direction);
  }

  // --------------------------------------------------------------------------
  // [Shape]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t _shape(uint32_t shapeType, const void* shapeData, uint32_t direction, const TransformF* tr = NULL)
  {
    return _g2d.pathf.shape(*this, shapeType, shapeData, direction, tr);
  }

  //! @brief Add a shape object to the path.
  FOG_INLINE err_t shape(const ShapeF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(object.getType(), object.getData(), direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t shape(const ShapeF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(object.getType(), object.getData(), direction, &tr);
  }

  //! @brief Add an unclosed line to the path.
  FOG_INLINE err_t line(const LineF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_LINE, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t line(const LineF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(SHAPE_TYPE_LINE, &object, direction, &tr);
  }

  //! @brief Add an unclosed quadratic bézier curve to the path.
  FOG_INLINE err_t quad(const QuadCurveF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_QUAD, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t quad(const QuadCurveF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(SHAPE_TYPE_QUAD, &object, direction, &tr);
  }

  //! @brief Add an unclosed cubic bézier curve to the path.
  FOG_INLINE err_t cubic(const QuadCurveF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_CUBIC, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t cubic(const QuadCurveF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(SHAPE_TYPE_CUBIC, &object, direction, &tr);
  }

  //! @brief Add an unclosed arc to the path.
  FOG_INLINE err_t arc(const ArcF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_ARC, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t arc(const ArcF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(SHAPE_TYPE_ARC, &object, direction, &tr);
  }

  //! @brief Add a closed rounded ractangle to the path.
  FOG_INLINE err_t round(const RoundF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_ROUND, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t round(const RoundF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(SHAPE_TYPE_ROUND, &object, direction, &tr);
  }

  //! @brief Add a closed circle to the path.
  FOG_INLINE err_t circle(const CircleF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_CIRCLE, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t circle(const CircleF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(SHAPE_TYPE_CIRCLE, &object, direction, &tr);
  }

  //! @brief Add a closed ellipse to the path.
  FOG_INLINE err_t ellipse(const EllipseF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_ELLIPSE, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t ellipse(const EllipseF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(SHAPE_TYPE_ELLIPSE, &object, direction, &tr);
  }

  //! @brief Add a closed chord to the path.
  FOG_INLINE err_t chord(const ArcF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_CHORD, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t chord(const ArcF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(SHAPE_TYPE_CHORD, &object, direction, &tr);
  }

  //! @brief Add a closed pie to the path.
  FOG_INLINE err_t pie(const ArcF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_PIE, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t pie(const ArcF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(SHAPE_TYPE_PIE, &object, direction, &tr);
  }

  // --------------------------------------------------------------------------
  // [Path]
  // --------------------------------------------------------------------------

  //! @brief Add another @a path to the path.
  FOG_INLINE err_t append(const PathF& path)
  {
    return _g2d.pathf.appendPathF(*this, path, NULL);
  }

  //! @brief Add another @a path to the path.
  FOG_INLINE err_t append(const PathF& path, const Range& range)
  {
    return _g2d.pathf.appendPathF(*this, path, &range);
  }

  //! @brief Add another @a path (translated by the @a pt) to the path.
  FOG_INLINE err_t appendTranslated(const PathF& path, const PointF& pt)
  {
    return _g2d.pathf.appendTranslatedPathF(*this, path, pt, NULL);
  }

  //! @brief Add another @a path (translated by the @a pt) to the path.
  FOG_INLINE err_t appendTranslated(const PathF& path, const PointF& pt, const Range& range)
  {
    return _g2d.pathf.appendTranslatedPathF(*this, path, pt, &range);
  }

  //! @brief Add another @a path (transformed by the @a tr) to the path.
  FOG_INLINE err_t appendTransformed(const PathF& path, const TransformF& tr)
  {
    return _g2d.pathf.appendTransformedPathF(*this, path, tr, NULL);
  }

  //! @brief Add another @a path (transformed by the @a tr) to the path.
  FOG_INLINE err_t appendTransformed(const PathF& path, const TransformF& tr, const Range& range)
  {
    return _g2d.pathf.appendTransformedPathF(*this, path, tr, &range);
  }

  // --------------------------------------------------------------------------
  // [Flat]
  // --------------------------------------------------------------------------

  //! @brief Get whether the path has quadratic or cubic Bezier curves.
  FOG_INLINE bool hasCurves() const
  {
    if (_d->flags & PATH_DATA_DIRTY_CMD) _g2d.pathf.updateFlat(*this);
    return (_d->flags & (PATH_DATA_HAS_QUAD_CMD | PATH_DATA_HAS_CUBIC_CMD)) != 0;
  }

  FOG_INLINE bool hasQuadCurves() const
  {
    if (_d->flags & PATH_DATA_DIRTY_CMD) _g2d.pathf.updateFlat(*this);
    return (_d->flags & PATH_DATA_HAS_QUAD_CMD) != 0;
  }

  FOG_INLINE bool hasCubicCurves() const
  {
    if (_d->flags & PATH_DATA_DIRTY_CMD) _g2d.pathf.updateFlat(*this);
    return (_d->flags & PATH_DATA_HAS_CUBIC_CMD) != 0;
  }

  FOG_INLINE err_t flatten(const PathFlattenParamsF& params)
  {
    return _g2d.pathf.flatten(*this, *this, params, NULL);
  }

  static FOG_INLINE err_t flatten(PathF& dst, const PathF& src, const PathFlattenParamsF& params)
  {
    return _g2d.pathf.flatten(dst, src, params, NULL);
  }

  static FOG_INLINE err_t flatten(PathF& dst, const PathF& src, const Range& range, const PathFlattenParamsF& params)
  {
    return _g2d.pathf.flatten(dst, src, params, &range);
  }

  // --------------------------------------------------------------------------
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE bool _hasBoundingBox() const
  {
    return _d->hasBoundingBox();
  }

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
    return _g2d.pathf.getBoundingBox(*this, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectF& dst, const TransformF* tr) const
  {
    err_t err = _g2d.pathf.getBoundingBox(*this, reinterpret_cast<BoxF*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointF& pt, uint32_t fillRule) const
  {
    return _g2d.pathf.hitTest(*this, pt, fillRule);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  //! @brief Translate all vertices by @a pt.
  FOG_INLINE err_t translate(const PointF& pt)
  {
    return _g2d.pathf.translate(*this, pt, NULL);
  }

  //! @brief Translate all vertices in range @a range by @a pt.
  FOG_INLINE err_t translate(const PointF& pt, const Range& range)
  {
    return _g2d.pathf.translate(*this, pt, &range);
  }

  //! @brief Transform all vertices by @a tr.
  FOG_INLINE err_t transform(const TransformF& tr)
  {
    return _g2d.pathf.transform(*this, tr, NULL);
  }

  //! @brief Transform all vertices in range @a range by @a tr.
  FOG_INLINE err_t transform(const TransformF& tr, const Range& range)
  {
    return _g2d.pathf.transform(*this, tr, &range);
  }

  //! @brief Fit path into the given rectangle @a r.
  FOG_INLINE err_t fitTo(const RectF& r)
  {
    return _g2d.pathf.fitTo(*this, r);
  }

  //! @brief Scale each vertex by @a pt.
  FOG_INLINE err_t scale(const PointF& pt, bool keepStartPos = false)
  {
    return _g2d.pathf.scale(*this, pt, keepStartPos);
  }

  // --------------------------------------------------------------------------
  // [FlipX / FlipY]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t flipX(float x0, float x1)
  {
    return _g2d.pathf.flipX(*this, x0, x1);
  }

  FOG_INLINE err_t flipY(float y0, float y1)
  {
    return _g2d.pathf.flipY(*this, y0, y1);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const PathF& other) const
  {
    return _g2d.pathf.eq(*this, other);
  }

  // --------------------------------------------------------------------------
  // [Modified]
  // --------------------------------------------------------------------------

  //! @brief Must be called when the path was modified to invalidate the cached
  //! bounding-box and the path type.
  FOG_INLINE void _modified()
  {
    FOG_ASSERT(isDetached());
    _d->flags |= PATH_DATA_DIRTY_BOUNDING_BOX | PATH_DATA_DIRTY_CMD;
  }

  //! @brief Must be called when the path vertex/vertices were manually
  //! modified, thus bounding-box must be invalidated.
  FOG_INLINE void _modifiedVertices() const
  {
    FOG_ASSERT(isDetached());
    _d->flags |= PATH_DATA_DIRTY_BOUNDING_BOX;
  }

  //! @brief Must be called when the path command/commands were manually
  //! modified, thus flat-type must be invalidated.
  FOG_INLINE void _modifiedCommands()
  {
    FOG_ASSERT(isDetached());
    _d->flags |= PATH_DATA_DIRTY_CMD;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PathF& operator=(const PathF& other) { setPath(other); return *this; }
  FOG_INLINE PathF& operator+=(const PathF& other) { append(other); return *this; }

  FOG_INLINE bool operator==(const PathF& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const PathF& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(PathDataF)
};

// ============================================================================
// [Fog::PathD]
// ============================================================================

//! @brief Path (double).
struct FOG_API PathD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathD()
  {
    _g2d.pathd.ctor(*this);
  }

  FOG_INLINE PathD(const PathD& other)
  {
    _g2d.pathd.ctorCopyD(*this, other);
  }

  explicit FOG_INLINE PathD(PathDataD* d) : _d(d) {}

  FOG_INLINE ~PathD()
  {
    _g2d.pathd.dtor(*this);
  }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Get path capacity (count of allocated vertices).
  FOG_INLINE sysuint_t getCapacity() const { return _d->capacity; }
  //! @brief Get path length (count of vertices used).
  FOG_INLINE sysuint_t getLength() const { return _d->length; }
  //! @brief Get whether path is empty.
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  //! @brief Get path commands array (const).
  FOG_INLINE const uint8_t* getCommands() const { return _d->commands; }
  //! @brief Get path vertices array (const).
  FOG_INLINE const PointD* getVertices() const { return _d->vertices; }

  //! @brief Get path commands array (mutable).
  FOG_INLINE uint8_t* getCommandsX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::PathD::getCommandsX() - Called on non-detached object.");
    return _d->commands;
  }

  //! @brief Get path vertices array (mutable).
  FOG_INLINE PointD* getVerticesX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::PathD::getVerticesX() - Called on non-detached object.");
    return _d->vertices;
  }

  FOG_INLINE sysuint_t getRefCount() const
  {
    return _d->refCount.get();
  }

  FOG_INLINE bool isDetached() const
  {
    return getRefCount() == 1;
  }

  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _g2d.pathd.detach(*this);
  }

  FOG_INLINE err_t reserve(sysuint_t capacity)
  {
    return _g2d.pathd.reserve(*this, capacity);
  }

  FOG_INLINE void squeeze()
  {
    return _g2d.pathd.squeeze(*this);
  }

  FOG_INLINE sysuint_t _prepare(sysuint_t count, uint32_t cntOp)
  {
    return _g2d.pathd.prepare(*this, count, cntOp);
  }

  FOG_INLINE sysuint_t _add(sysuint_t count)
  {
    return _g2d.pathd.add(*this, count);
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _g2d.pathd.clear(*this);
  }

  FOG_INLINE void reset()
  {
    _g2d.pathd.reset(*this);
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setPath(const PathD& other)
  {
    return _g2d.pathd.setPathD(*this, other);
  }

  FOG_INLINE err_t setPath(const PathF& other)
  {
    return _g2d.pathd.setPathF(*this, other);
  }

  FOG_INLINE err_t setDeep(const PathD& other)
  {
    return _g2d.pathd.setDeepD(*this, other);
  }

  // --------------------------------------------------------------------------
  // [SubPath]
  // --------------------------------------------------------------------------

  //! @brief Get range of subpath at index @a index (range is from the @a index
  //! to next 'move-to' command or to the end of the path).
  FOG_INLINE Range getSubpathRange(sysuint_t index) const
  {
    return _g2d.pathd.getSubpathRange(*this, index);
  }

  // --------------------------------------------------------------------------
  // [MoveTo]
  // --------------------------------------------------------------------------

  //! @brief Move to @a pt0 (absolute).
  FOG_INLINE err_t moveTo(const PointD& pt0)
  {
    return _g2d.pathd.moveTo(*this, pt0);
  }

  //! @brief Move to @a pt0 (relative).
  FOG_INLINE err_t moveToRel(const PointD& pt0)
  {
    return _g2d.pathd.moveToRel(*this, pt0);
  }

  // --------------------------------------------------------------------------
  // [LineTo]
  // --------------------------------------------------------------------------

  //! @brief Line to @a pt1 (absolute).
  FOG_INLINE err_t lineTo(const PointD& pt1)
  {
    return _g2d.pathd.lineTo(*this, pt1);
  }

  //! @brief Line to @a pt1 (relative).
  FOG_INLINE err_t lineToRel(const PointD& pt1)
  {
    return _g2d.pathd.lineToRel(*this, pt1);
  }

  //! @brief Horizontal line to @a x (absolute).
  FOG_INLINE err_t hlineTo(double x)
  {
    return _g2d.pathd.hlineTo(*this, x);
  }

  //! @brief Horizontal line to @a x (relative).
  FOG_INLINE err_t hlineToRel(double x)
  {
    return _g2d.pathd.hlineToRel(*this, x);
  }

  //! @brief Horizontal line to @a y (absolute).
  FOG_INLINE err_t vlineTo(double y)
  {
    return _g2d.pathd.vlineTo(*this, y);
  }

  //! @brief Horizontal line to @a y (relative).
  FOG_INLINE err_t vlineToRel(double y)
  {
    return _g2d.pathd.vlineToRel(*this, y);
  }

  // --------------------------------------------------------------------------
  // [PolyTo]
  // --------------------------------------------------------------------------

  //! @brief Polyline to @a pts (absolute).
  FOG_INLINE err_t polyTo(const PointD* pts, sysuint_t count)
  {
    return _g2d.pathd.polyTo(*this, pts, count);
  }

  //! @brief Polyline to @a pts (relative).
  FOG_INLINE err_t polyToRel(const PointD* pts, sysuint_t count)
  {
    return _g2d.pathd.polyToRel(*this, pts, count);
  }

  // --------------------------------------------------------------------------
  // [QuadTo]
  // --------------------------------------------------------------------------

  //! @brief Quadratic curve to @a pt1, and @a pt2 (absolute).
  FOG_INLINE err_t quadTo(const PointD& pt1, const PointD& pt2)
  {
    return _g2d.pathd.quadTo(*this, pt1, pt2);
  }

  //! @brief Quadratic curve to @a pt1, and @a pt2 (relative).
  FOG_INLINE err_t quadToRel(const PointD& pt1, const PointD& pt2)
  {
    return _g2d.pathd.quadToRel(*this, pt1, pt2);
  }

  //! @brief Smooth quadratic curve to @a pt2, calculating pt1 from last points (absolute).
  FOG_INLINE err_t smoothQuadTo(const PointD& pt2)
  {
    return _g2d.pathd.smoothQuadTo(*this, pt2);
  }

  //! @brief Smooth quadratic curve to @a pt2, calculating pt1 from last points (relative).
  FOG_INLINE err_t smoothQuadToRel(const PointD& pt2)
  {
    return _g2d.pathd.smoothQuadToRel(*this, pt2);
  }

  // --------------------------------------------------------------------------
  // [CubicTo]
  // --------------------------------------------------------------------------

  //! @brief Cubic curve to @a pt1, @a pt2, and @a pt3 (absolute).
  FOG_INLINE err_t cubicTo(const PointD& pt1, const PointD& pt2, const PointD& pt3)
  {
    return _g2d.pathd.cubicTo(*this, pt1, pt2, pt3);
  }

  //! @brief Cubic curve to @a pt1, @a pt2, and @a pt3 (relative).
  FOG_INLINE err_t cubicToRel(const PointD& pt1, const PointD& pt2, const PointD& pt3)
  {
    return _g2d.pathd.cubicToRel(*this, pt1, pt2, pt3);
  }

  //! @brief Smooth cubic curve to @a pt2, and @a pt3, calculating pt1 from last points (absolute).
  FOG_INLINE err_t smoothCubicTo(const PointD& pt2, const PointD& pt3)
  {
    return _g2d.pathd.smoothCubicTo(*this, pt2, pt3);
  }

  //! @brief Smooth cubic curve to @a pt2, and @a pt3, calculating pt1 from last points (relative).
  FOG_INLINE err_t smoothCubicToRel(const PointD& pt2, const PointD& pt3)
  {
    return _g2d.pathd.smoothCubicToRel(*this, pt2, pt3);
  }

  // --------------------------------------------------------------------------
  // [ArcTo]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t arcTo(const PointD& cp, const PointD& rp, double start, double sweep, bool startPath = false)
  {
    return _g2d.pathd.arcTo(*this, cp, rp, start, sweep, startPath);
  }

  FOG_INLINE err_t arcToRel(const PointD& cp, const PointD& rp, double start, double sweep, bool startPath = false)
  {
    return _g2d.pathd.arcToRel(*this, cp, rp, start, sweep, startPath);
  }

  FOG_INLINE err_t svgArcTo(const PointD& rp, double angle, bool largeArcFlag, bool sweepFlag, const PointD& pt)
  {
    return _g2d.pathd.svgArcTo(*this, rp, angle, largeArcFlag, sweepFlag, pt);
  }

  FOG_INLINE err_t svgArcToRel(const PointD& rp, double angle, bool largeArcFlag, bool sweepFlag, const PointD& pt)
  {
    return _g2d.pathd.svgArcToRel(*this, rp, angle, largeArcFlag, sweepFlag, pt);
  }

  // --------------------------------------------------------------------------
  // [Close]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t close()
  {
    return _g2d.pathd.close(*this);
  }

  // --------------------------------------------------------------------------
  // [Box / Rect]
  // --------------------------------------------------------------------------

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t box(const BoxI& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.boxI(*this, r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t box(const BoxF& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.boxF(*this, r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t box(const BoxD& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.boxD(*this, r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t rect(const RectI& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.rectI(*this, r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t rect(const RectF& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.rectF(*this, r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t rect(const RectD& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.rectD(*this, r, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t boxes(const BoxI* b, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.boxesI(*this, b, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t boxes(const BoxF* b, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.boxesF(*this, b, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t boxes(const BoxD* b, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.boxesD(*this, b, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t rects(const RectI* r, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.rectsI(*this, r, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t rects(const RectF* r, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.rectsF(*this, r, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t rects(const RectD* r, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.rectsD(*this, r, count, direction);
  }

  // --------------------------------------------------------------------------
  // [Region]
  // --------------------------------------------------------------------------

  //! @brief Add a closed region (converted to set of rectangles).
  FOG_INLINE err_t region(const Region& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.region(*this, r, direction);
  }

  // --------------------------------------------------------------------------
  // [Polyline / Polygon]
  // --------------------------------------------------------------------------

  //! @brief Add a polyline to the path.
  FOG_INLINE err_t polyline(const PointI* pts, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.polylineI(*this, pts, count, direction);
  }

  //! @brief Add a polyline to the path.
  FOG_INLINE err_t polyline(const PointD* pts, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.polylineD(*this, pts, count, direction);
  }

  //! @brief Add a closed polygon to the path.
  FOG_INLINE err_t polygon(const PointI* pts, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.polygonI(*this, pts, count, direction);
  }

  //! @brief Add a closed polygon to the path.
  FOG_INLINE err_t polygon(const PointD* pts, sysuint_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _g2d.pathd.polygonD(*this, pts, count, direction);
  }

  // --------------------------------------------------------------------------
  // [Shape]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t _shape(uint32_t shapeType, const void* shapeData, uint32_t direction, const TransformD* tr = NULL)
  {
    return _g2d.pathd.shape(*this, shapeType, shapeData, direction, tr);
  }

  //! @brief Add a shape object to the path.
  FOG_INLINE err_t shape(const ShapeD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(object.getType(), object.getData(), direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t shape(const ShapeD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(object.getType(), object.getData(), direction, &tr);
  }

  //! @brief Add an unclosed line to the path.
  FOG_INLINE err_t line(const LineD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_LINE, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t line(const LineD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(SHAPE_TYPE_LINE, &object, direction, &tr);
  }

  //! @brief Add an unclosed quadratic bézier curve to the path.
  FOG_INLINE err_t quad(const QuadCurveD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_QUAD, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t quad(const QuadCurveD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(SHAPE_TYPE_QUAD, &object, direction, &tr);
  }

  //! @brief Add an unclosed cubic bézier curve to the path.
  FOG_INLINE err_t cubic(const QuadCurveD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_CUBIC, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t cubic(const QuadCurveD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(SHAPE_TYPE_CUBIC, &object, direction, &tr);
  }

  //! @brief Add an unclosed arc to the path.
  FOG_INLINE err_t arc(const ArcD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_ARC, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t arc(const ArcD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(SHAPE_TYPE_ARC, &object, direction, &tr);
  }

  //! @brief Add a closed rounded ractangle to the path.
  FOG_INLINE err_t round(const RoundD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_ROUND, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t round(const RoundD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(SHAPE_TYPE_ROUND, &object, direction, &tr);
  }

  //! @brief Add a closed circle to the path.
  FOG_INLINE err_t circle(const CircleD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_CIRCLE, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t circle(const CircleD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(SHAPE_TYPE_CIRCLE, &object, direction, &tr);
  }

  //! @brief Add a closed ellipse to the path.
  FOG_INLINE err_t ellipse(const EllipseD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_ELLIPSE, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t ellipse(const EllipseD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(SHAPE_TYPE_ELLIPSE, &object, direction, &tr);
  }

  //! @brief Add a closed chord to the path.
  FOG_INLINE err_t chord(const ArcD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_CHORD, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t chord(const ArcD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(SHAPE_TYPE_CHORD, &object, direction, &tr);
  }

  //! @brief Add a closed pie to the path.
  FOG_INLINE err_t pie(const ArcD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_PIE, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t pie(const ArcD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(SHAPE_TYPE_PIE, &object, direction, &tr);
  }

  // --------------------------------------------------------------------------
  // [Path]
  // --------------------------------------------------------------------------

  //! @brief Add another @a path to the path.
  FOG_INLINE err_t append(const PathD& path)
  {
    return _g2d.pathd.appendPathD(*this, path, NULL);
  }

  //! @brief Add another @a path to the path.
  FOG_INLINE err_t append(const PathF& path)
  {
    return _g2d.pathd.appendPathF(*this, path, NULL);
  }

  //! @brief Add another @a path to the path.
  FOG_INLINE err_t append(const PathD& path, const Range& range)
  {
    return _g2d.pathd.appendPathD(*this, path, &range);
  }

  //! @brief Add another @a path to the path.
  FOG_INLINE err_t append(const PathF& path, const Range& range)
  {
    return _g2d.pathd.appendPathF(*this, path, &range);
  }

  //! @brief Add another @a path (translated by the @a pt) to the path.
  FOG_INLINE err_t appendTranslated(const PathD& path, const PointD& pt)
  {
    return _g2d.pathd.appendTranslatedPathD(*this, path, pt, NULL);
  }

  //! @brief Add another @a path (translated by the @a pt) to the path.
  FOG_INLINE err_t appendTranslated(const PathD& path, const PointD& pt, const Range& range)
  {
    return _g2d.pathd.appendTranslatedPathD(*this, path, pt, &range);
  }

  //! @brief Add another @a path (transformed by the @a tr) to the path.
  FOG_INLINE err_t appendTransformed(const PathD& path, const TransformD& tr)
  {
    return _g2d.pathd.appendTransformedPathD(*this, path, tr, NULL);
  }

  //! @brief Add another @a path (transformed by the @a tr) to the path.
  FOG_INLINE err_t appendTransformed(const PathD& path, const TransformD& tr, const Range& range)
  {
    return _g2d.pathd.appendTransformedPathD(*this, path, tr, &range);
  }

  // --------------------------------------------------------------------------
  // [Flat]
  // --------------------------------------------------------------------------

  //! @brief Get whether the path has quadratic or cubic Bezier curves.
  FOG_INLINE bool hasCurves() const
  {
    if (_d->flags & PATH_DATA_DIRTY_CMD) _g2d.pathd.updateFlat(*this);
    return (_d->flags & (PATH_DATA_HAS_QUAD_CMD | PATH_DATA_HAS_CUBIC_CMD)) != 0;
  }

  FOG_INLINE bool hasQuadCurves() const
  {
    if (_d->flags & PATH_DATA_DIRTY_CMD) _g2d.pathd.updateFlat(*this);
    return (_d->flags & PATH_DATA_HAS_QUAD_CMD) != 0;
  }

  FOG_INLINE bool hasCubicCurves() const
  {
    if (_d->flags & PATH_DATA_DIRTY_CMD) _g2d.pathd.updateFlat(*this);
    return (_d->flags & PATH_DATA_HAS_CUBIC_CMD) != 0;
  }

  FOG_INLINE err_t flatten(const PathFlattenParamsD& params)
  {
    return _g2d.pathd.flatten(*this, *this, params, NULL);
  }

  static FOG_INLINE err_t flatten(PathD& dst, const PathD& src, const PathFlattenParamsD& params)
  {
    return _g2d.pathd.flatten(dst, src, params, NULL);
  }

  static FOG_INLINE err_t flatten(PathD& dst, const PathD& src, const Range& range, const PathFlattenParamsD& params)
  {
    return _g2d.pathd.flatten(dst, src, params, &range);
  }

  // --------------------------------------------------------------------------
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE bool _hasBoundingBox() const
  {
    return _d->hasBoundingBox();
  }

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
    return _g2d.pathd.getBoundingBox(*this, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectD& dst, const TransformD* tr) const
  {
    err_t err = _g2d.pathd.getBoundingBox(*this, reinterpret_cast<BoxD*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointD& pt, uint32_t fillRule) const
  {
    return _g2d.pathd.hitTest(*this, pt, fillRule);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  //! @brief Translate all vertices by @a pt.
  FOG_INLINE err_t translate(const PointD& pt)
  {
    return _g2d.pathd.translate(*this, pt, NULL);
  }

  //! @brief Translate all vertices in range @a range by @a pt.
  FOG_INLINE err_t translate(const PointD& pt, const Range& range)
  {
    return _g2d.pathd.translate(*this, pt, &range);
  }

  //! @brief Transform all vertices by @a tr.
  FOG_INLINE err_t transform(const TransformD& tr)
  {
    return _g2d.pathd.transform(*this, tr, NULL);
  }

  //! @brief Transform all vertices in range @a range by @a tr.
  FOG_INLINE err_t transform(const TransformD& tr, const Range& range)
  {
    return _g2d.pathd.transform(*this, tr, &range);
  }

  //! @brief Fit path into the given rectangle @a r.
  FOG_INLINE err_t fitTo(const RectD& r)
  {
    return _g2d.pathd.fitTo(*this, r);
  }

  //! @brief Scale each vertex by @a pt.
  FOG_INLINE err_t scale(const PointD& pt, bool keepStartPos = false)
  {
    return _g2d.pathd.scale(*this, pt, keepStartPos);
  }

  // --------------------------------------------------------------------------
  // [FlipX / FlipY]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t flipX(double x0, double x1)
  {
    return _g2d.pathd.flipX(*this, x0, x1);
  }

  FOG_INLINE err_t flipY(double y0, double y1)
  {
    return _g2d.pathd.flipY(*this, y0, y1);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const PathD& other) const
  {
    return _g2d.pathd.eq(*this, other);
  }

  // --------------------------------------------------------------------------
  // [Modified]
  // --------------------------------------------------------------------------

  //! @brief Must be called when the path was modified to invalidate the cached
  //! bounding-box and the path type.
  FOG_INLINE void _modified()
  {
    FOG_ASSERT(isDetached());
    _d->flags |= PATH_DATA_DIRTY_BOUNDING_BOX | PATH_DATA_DIRTY_CMD;
  }

  //! @brief Must be called when the path vertex/vertices were manually
  //! modified, thus bounding-box must be invalidated.
  FOG_INLINE void _modifiedVertices() const
  {
    FOG_ASSERT(isDetached());
    _d->flags |= PATH_DATA_DIRTY_BOUNDING_BOX;
  }

  //! @brief Must be called when the path command/commands were manually
  //! modified, thus flat-type must be invalidated.
  FOG_INLINE void _modifiedCommands()
  {
    FOG_ASSERT(isDetached());
    _d->flags |= PATH_DATA_DIRTY_CMD;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PathD& operator=(const PathD& other)
  {
    setPath(other);
    return *this;
  }

  FOG_INLINE PathD& operator+=(const PathD& other)
  {
    append(other);
    return *this;
  }

  FOG_INLINE bool operator==(const PathD& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const PathD& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(PathDataD)
};

// ============================================================================
// [Fog::PathT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D(Path)
FOG_TYPEVARIANT_DECLARE_F_D(PathData)
FOG_TYPEVARIANT_DECLARE_F_D(PathFlattenParams)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::PathF, Fog::TYPEINFO_MOVABLE)
FOG_DECLARE_TYPEINFO(Fog::PathD, Fog::TYPEINFO_MOVABLE)

FOG_DECLARE_TYPEINFO(Fog::PathFlattenParamsF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::PathFlattenParamsD, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PATH_H
