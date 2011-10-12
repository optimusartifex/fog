// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PATH_H
#define _FOG_G2D_GEOMETRY_PATH_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/PathInfo.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Shape.h>
#include <Fog/G2d/Geometry/Triangle.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

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
    float flatness = MathConstant<float>::getDefaultFlatness(),
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
    double flatness = MathConstant<double>::getDefaultFlatness(),
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
    return (vType & (PATH_FLAG_DIRTY_BBOX | PATH_FLAG_HAS_BBOX)) == PATH_FLAG_HAS_BBOX;
  }

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE PathDataF* addRef() const
  {
    reference.inc();
    return const_cast<PathDataF*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      _api.pathf_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // ${VAR:BEGIN}
  //
  // This data-object is binary compatible with the VarData header in the second
  // form called - "implicitly shared container". The members must be binary
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
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | padding0_32| Not used by the Var. This member is only   |
  // |              |            | defined for 64-bit compilation to pad      |
  // |              |            | other members!                             |
  // +--------------+------------+--------------------------------------------+
  // | size_t       | capacity   | Capacity of the container (items).         |
  // +--------------+------------+--------------------------------------------+
  // | size_t       | length     | Length of the container (items).           |
  // +==============+============+============================================+
  //
  // ${VAR:END}

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Flags (see @c PATH_FLAG).
  volatile uint32_t vType;

#if FOG_ARCH_BITS >= 64
  uint32_t padding0_32;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Path capacity (allocated space for vertices).
  size_t capacity;
  //! @brief Path length (count of vertices used).
  size_t length;

  //! @brief Path bounding box.
  BoxF boundingBox;

  //! @brief Link to the path info.
  const PathInfoF* info;

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
    return (vType & (PATH_FLAG_DIRTY_BBOX | PATH_FLAG_HAS_BBOX)) == PATH_FLAG_HAS_BBOX;
  }

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE PathDataD* addRef() const
  {
    reference.inc();
    return const_cast<PathDataD*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      _api.pathd_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Flags (see @c PATH_FLAG).
  volatile uint32_t vType;

#if FOG_ARCH_BITS >= 64
  uint32_t padding0_32;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Path capacity (allocated space for vertices).
  size_t capacity;
  //! @brief Path length (count of vertices used).
  size_t length;

  //! @brief Path bounding box.
  BoxD boundingBox;

  //! @brief Link to the path info.
  const PathInfoD* info;

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
    _api.pathf_ctor(this);
  }

  FOG_INLINE PathF(const PathF& other)
  {
    _api.pathf_ctorCopyF(this, &other);
  }

  explicit FOG_INLINE PathF(PathDataF* d) : _d(d) {}

  FOG_INLINE ~PathF()
  {
    _api.pathf_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _api.pathf_detach(this);
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  //! @brief Get path capacity (count of allocated vertices).
  FOG_INLINE size_t getCapacity() const { return _d->capacity; }
  //! @brief Get path length (count of vertices used).
  FOG_INLINE size_t getLength() const { return _d->length; }
  //! @brief Get whether the path is empty.
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return _api.pathf_reserve(this, capacity);
  }

  FOG_INLINE void squeeze()
  {
    return _api.pathf_squeeze(this);
  }

  FOG_INLINE size_t _prepare(uint32_t cntOp, size_t count)
  {
    return _api.pathf_prepare(this, cntOp, count);
  }

  FOG_INLINE size_t _add(size_t count)
  {
    return _api.pathf_add(this, count);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get path commands array (const).
  FOG_INLINE const uint8_t* getCommands() const
  {
    return _d->commands;
  }

  //! @brief Get path commands array (mutable).
  FOG_INLINE uint8_t* getCommandsX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::PathF::getCommandsX() - Not detached.");
    return _d->commands;
  }

  //! @brief Get path vertices array (const).
  FOG_INLINE const PointF* getVertices() const
  {
    return _d->vertices;
  }

  //! @brief Get path vertices array (mutable).
  FOG_INLINE PointF* getVerticesX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::PathF::getVerticesX() - Not detached.");
    return _d->vertices;
  }

  FOG_INLINE const PointF& getVertex(size_t index) const
  {
    FOG_ASSERT_X(index < _d->length, "Fog::PathF::getVertex() - Index out of range.");
    return _d->vertices[index];
  }

  FOG_INLINE PointF getLastVertex() const
  {
    PointF result(UNINITIALIZED);
    _api.pathf_getLastVertex(this, &result);
    return result;
  }

  FOG_INLINE err_t setVertex(size_t index, const PointF& pt)
  {
    FOG_ASSERT_X(index < _d->length, "Fog::PathF::setVertex() - Index out of range.");
    return _api.pathf_setVertex(this, index, &pt);
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _api.pathf_clear(this);
  }

  FOG_INLINE void reset()
  {
    _api.pathf_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setPath(const PathF& other)
  {
    return _api.pathf_setPathF(this, &other);
  }

  FOG_INLINE err_t setDeep(const PathF& other)
  {
    return _api.pathf_setDeepF(this, &other);
  }

  // --------------------------------------------------------------------------
  // [SubPath]
  // --------------------------------------------------------------------------

  //! @brief Get range of subpath at index @a index (range is from the @a index
  //! to next 'move-to' command or to the end of the path).
  FOG_INLINE err_t getSubpathRange(Range& dst, size_t index) const
  {
    return _api.pathf_getSubpathRange(this, &dst, index);
  }

  // --------------------------------------------------------------------------
  // [MoveTo]
  // --------------------------------------------------------------------------

  //! @brief Move to @a pt0 (absolute).
  FOG_INLINE err_t moveTo(const PointF& pt0)
  {
    return _api.pathf_moveTo(this, &pt0);
  }

  //! @brief Move to @a pt0 (relative).
  FOG_INLINE err_t moveToRel(const PointF& pt0)
  {
    return _api.pathf_moveToRel(this, &pt0);
  }

  // --------------------------------------------------------------------------
  // [LineTo]
  // --------------------------------------------------------------------------

  //! @brief Line to @a pt1 (absolute).
  FOG_INLINE err_t lineTo(const PointF& pt1)
  {
    return _api.pathf_lineTo(this, &pt1);
  }

  //! @brief Line to @a pt1 (relative).
  FOG_INLINE err_t lineToRel(const PointF& pt1)
  {
    return _api.pathf_lineToRel(this, &pt1);
  }

  //! @brief Horizontal line to @a x (absolute).
  FOG_INLINE err_t hlineTo(float x)
  {
    return _api.pathf_hlineTo(this, x);
  }

  //! @brief Horizontal line to @a x (relative).
  FOG_INLINE err_t hlineToRel(float x)
  {
    return _api.pathf_hlineToRel(this, x);
  }

  //! @brief Horizontal line to @a y (absolute).
  FOG_INLINE err_t vlineTo(float y)
  {
    return _api.pathf_vlineTo(this, y);
  }

  //! @brief Horizontal line to @a y (relative).
  FOG_INLINE err_t vlineToRel(float y)
  {
    return _api.pathf_vlineToRel(this, y);
  }

  // --------------------------------------------------------------------------
  // [PolyTo]
  // --------------------------------------------------------------------------

  //! @brief Polyline to @a pts (absolute).
  FOG_INLINE err_t polyTo(const PointF* pts, size_t count)
  {
    return _api.pathf_polyTo(this, pts, count);
  }

  //! @brief Polyline to @a pts (relative).
  FOG_INLINE err_t polyToRel(const PointF* pts, size_t count)
  {
    return _api.pathf_polyToRel(this, pts, count);
  }

  // --------------------------------------------------------------------------
  // [QuadTo]
  // --------------------------------------------------------------------------

  //! @brief Quadratic curve to @a pt1, and @a pt2 (absolute).
  FOG_INLINE err_t quadTo(const PointF& pt1, const PointF& pt2)
  {
    return _api.pathf_quadTo(this, &pt1, &pt2);
  }

  //! @brief Quadratic curve to @a pt1, and @a pt2 (relative).
  FOG_INLINE err_t quadToRel(const PointF& pt1, const PointF& pt2)
  {
    return _api.pathf_quadToRel(this, &pt1, &pt2);
  }

  //! @brief Smooth quadratic curve to @a pt2, calculating pt1 from last points (absolute).
  FOG_INLINE err_t smoothQuadTo(const PointF& pt2)
  {
    return _api.pathf_smoothQuadTo(this, &pt2);
  }

  //! @brief Smooth quadratic curve to @a pt2, calculating pt1 from last points (relative).
  FOG_INLINE err_t smoothQuadToRel(const PointF& pt2)
  {
    return _api.pathf_smoothQuadToRel(this, &pt2);
  }

  // --------------------------------------------------------------------------
  // [CubicTo]
  // --------------------------------------------------------------------------

  //! @brief Cubic curve to @a pt1, @a pt2, and @a pt3 (absolute).
  FOG_INLINE err_t cubicTo(const PointF& pt1, const PointF& pt2, const PointF& pt3)
  {
    return _api.pathf_cubicTo(this, &pt1, &pt2, &pt3);
  }

  //! @brief Cubic curve to @a pt1, @a pt2, and @a pt3 (relative).
  FOG_INLINE err_t cubicToRel(const PointF& pt1, const PointF& pt2, const PointF& pt3)
  {
    return _api.pathf_cubicToRel(this, &pt1, &pt2, &pt3);
  }

  //! @brief Smooth cubic curve to @a pt2, and @a pt3, calculating pt1 from last points (absolute).
  FOG_INLINE err_t smoothCubicTo(const PointF& pt2, const PointF& pt3)
  {
    return _api.pathf_smoothCubicTo(this, &pt2, &pt3);
  }

  //! @brief Smooth cubic curve to @a pt2, and @a pt3, calculating pt1 from last points (relative).
  FOG_INLINE err_t smoothCubicToRel(const PointF& pt2, const PointF& pt3)
  {
    return _api.pathf_smoothCubicToRel(this, &pt2, &pt3);
  }

  // --------------------------------------------------------------------------
  // [ArcTo]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t arcTo(const PointF& cp, const PointF& rp, float start, float sweep, bool startPath = false)
  {
    return _api.pathf_arcTo(this, &cp, &rp, start, sweep, startPath);
  }

  FOG_INLINE err_t arcToRel(const PointF& cp, const PointF& rp, float start, float sweep, bool startPath = false)
  {
    return _api.pathf_arcToRel(this, &cp, &rp, start, sweep, startPath);
  }

  FOG_INLINE err_t svgArcTo(const PointF& rp, float angle, bool largeArcFlag, bool sweepFlag, const PointF& pt)
  {
    return _api.pathf_svgArcTo(this, &rp, angle, largeArcFlag, sweepFlag, &pt);
  }

  FOG_INLINE err_t svgArcToRel(const PointF& rp, float angle, bool largeArcFlag, bool sweepFlag, const PointF& pt)
  {
    return _api.pathf_svgArcToRel(this, &rp, angle, largeArcFlag, sweepFlag, &pt);
  }

  // --------------------------------------------------------------------------
  // [Close]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t close()
  {
    return _api.pathf_close(this);
  }

  // --------------------------------------------------------------------------
  // [Box / Rect]
  // --------------------------------------------------------------------------

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t box(const BoxI& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_boxI(this, &r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t box(const BoxF& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_boxF(this, &r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t rect(const RectI& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_rectI(this, &r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t rect(const RectF& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_rectF(this, &r, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t boxes(const BoxI* b, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_boxesI(this, b, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t boxes(const BoxF* b, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_boxesF(this, b, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t rects(const RectI* r, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_rectsI(this, r, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t rects(const RectF* r, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_rectsF(this, r, count, direction);
  }

  // --------------------------------------------------------------------------
  // [Region]
  // --------------------------------------------------------------------------

  //! @brief Add a closed region (converted to set of rectangles).
  FOG_INLINE err_t region(const Region& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_region(this, &r, direction);
  }

  // --------------------------------------------------------------------------
  // [Polyline / Polygon]
  // --------------------------------------------------------------------------

  //! @brief Add a polyline to the path.
  FOG_INLINE err_t polyline(const PointI* pts, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_polylineI(this, pts, count, direction);
  }

  //! @brief Add a polyline to the path.
  FOG_INLINE err_t polyline(const PointF* pts, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_polylineF(this, pts, count, direction);
  }

  //! @brief Add a closed polygon to the path.
  FOG_INLINE err_t polygon(const PointI* pts, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_polygonI(this, pts, count, direction);
  }

  //! @brief Add a closed polygon to the path.
  FOG_INLINE err_t polygon(const PointF* pts, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathf_polygonF(this, pts, count, direction);
  }

  // --------------------------------------------------------------------------
  // [Shape]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t _shape(uint32_t shapeType, const void* shapeData, uint32_t direction, const TransformF* tr = NULL)
  {
    return _api.pathf_shape(this, shapeType, shapeData, direction, tr);
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
  FOG_INLINE err_t qbezier(const QBezierF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_QBEZIER, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t qbezier(const QBezierF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(SHAPE_TYPE_QBEZIER, &object, direction, &tr);
  }

  //! @brief Add an unclosed cubic bézier curve to the path.
  FOG_INLINE err_t cbezier(const QBezierF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_CBEZIER, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t cbezier(const QBezierF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(SHAPE_TYPE_CBEZIER, &object, direction, &tr);
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

  //! @brief Add a closed triangle.
  FOG_INLINE err_t triangle(const TriangleF& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_TRIANGLE, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t triangle(const TriangleF& object, uint32_t direction, const TransformF& tr)
  {
    return _shape(SHAPE_TYPE_TRIANGLE, &object, direction, &tr);
  }

  // --------------------------------------------------------------------------
  // [Path]
  // --------------------------------------------------------------------------

  //! @brief Add another @a path to the path.
  FOG_INLINE err_t append(const PathF& path)
  {
    return _api.pathf_appendPathF(this, &path, NULL);
  }

  //! @brief Add another @a path to the path.
  FOG_INLINE err_t append(const PathF& path, const Range& range)
  {
    return _api.pathf_appendPathF(this, &path, &range);
  }

  //! @brief Add another @a path (translated by the @a pt) to the path.
  FOG_INLINE err_t appendTranslated(const PathF& path, const PointF& pt)
  {
    return _api.pathf_appendTranslatedPathF(this, &path, &pt, NULL);
  }

  //! @brief Add another @a path (translated by the @a pt) to the path.
  FOG_INLINE err_t appendTranslated(const PathF& path, const PointF& pt, const Range& range)
  {
    return _api.pathf_appendTranslatedPathF(this, &path, &pt, &range);
  }

  //! @brief Add another @a path (transformed by the @a tr) to the path.
  FOG_INLINE err_t appendTransformed(const PathF& path, const TransformF& tr)
  {
    return _api.pathf_appendTransformedPathF(this, &path, &tr, NULL);
  }

  //! @brief Add another @a path (transformed by the @a tr) to the path.
  FOG_INLINE err_t appendTransformed(const PathF& path, const TransformF& tr, const Range& range)
  {
    return _api.pathf_appendTransformedPathF(this, &path, &tr, &range);
  }

  // --------------------------------------------------------------------------
  // [Flat]
  // --------------------------------------------------------------------------

  //! @brief Get whether the path has quadratic or cubic Bezier curves.
  FOG_INLINE bool hasBeziers() const
  {
    if (_d->vType & PATH_FLAG_DIRTY_CMD)
      _api.pathf_updateFlat(this);

    return (_d->vType & (PATH_FLAG_HAS_QBEZIER | PATH_FLAG_HAS_CBEZIER)) != 0;
  }

  FOG_INLINE bool hasQBeziers() const
  {
    if (_d->vType & PATH_FLAG_DIRTY_CMD)
      _api.pathf_updateFlat(this);

    return (_d->vType & PATH_FLAG_HAS_QBEZIER) != 0;
  }

  FOG_INLINE bool hasCBeziers() const
  {
    if (_d->vType & PATH_FLAG_DIRTY_CMD)
      _api.pathf_updateFlat(this);

    return (_d->vType & PATH_FLAG_HAS_CBEZIER) != 0;
  }

  FOG_INLINE err_t flatten(const PathFlattenParamsF& params)
  {
    return _api.pathf_flatten(this, this, &params, NULL);
  }

  static FOG_INLINE err_t flatten(PathF& dst, const PathF& src, const PathFlattenParamsF& params)
  {
    return _api.pathf_flatten(&dst, &src, &params, NULL);
  }

  static FOG_INLINE err_t flatten(PathF& dst, const PathF& src, const Range& range, const PathFlattenParamsF& params)
  {
    return _api.pathf_flatten(&dst, &src, &params, &range);
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
    return _api.pathf_getBoundingBox(this, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectF& dst, const TransformF* tr) const
  {
    err_t err = _api.pathf_getBoundingBox(this, reinterpret_cast<BoxF*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointF& pt, uint32_t fillRule) const
  {
    return _api.pathf_hitTest(this, &pt, fillRule);
  }

  // --------------------------------------------------------------------------
  // [GetClosestVertex]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getClosestVertex(const PointF& pt, float maxDistance, float* distance = NULL) const
  {
    return _api.pathf_getClosestVertex(this, &pt, maxDistance, distance);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  //! @brief Translate all vertices by @a pt.
  FOG_INLINE err_t translate(const PointF& pt)
  {
    return _api.pathf_translate(this, &pt, NULL);
  }

  //! @brief Translate all vertices in range @a range by @a pt.
  FOG_INLINE err_t translate(const PointF& pt, const Range& range)
  {
    return _api.pathf_translate(this, &pt, &range);
  }

  //! @brief Transform all vertices by @a tr.
  FOG_INLINE err_t transform(const TransformF& tr)
  {
    return _api.pathf_transform(this, &tr, NULL);
  }

  //! @brief Transform all vertices in range @a range by @a tr.
  FOG_INLINE err_t transform(const TransformF& tr, const Range& range)
  {
    return _api.pathf_transform(this, &tr, &range);
  }

  //! @brief Fit path into the given rectangle @a r.
  FOG_INLINE err_t fitTo(const RectF& r)
  {
    return _api.pathf_fitTo(this, &r);
  }

  //! @brief Scale each vertex by @a pt.
  FOG_INLINE err_t scale(const PointF& pt, bool keepStartPos = false)
  {
    return _api.pathf_scale(this, &pt, keepStartPos);
  }

  // --------------------------------------------------------------------------
  // [FlipX / FlipY]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t flipX(float x0, float x1)
  {
    return _api.pathf_flipX(this, x0, x1);
  }

  FOG_INLINE err_t flipY(float y0, float y1)
  {
    return _api.pathf_flipY(this, y0, y1);
  }

  // --------------------------------------------------------------------------
  // [Path-Info]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasPathInfo() const
  {
    return (_d->vType & PATH_FLAG_DIRTY_INFO) == 0 && _d->info != NULL;
  }

  FOG_INLINE void buildPathInfo() const
  {
    (void)_api.pathf_getPathInfo(this);
  }

  FOG_INLINE const PathInfoF* getPathInfo() const
  {
    return _api.pathf_getPathInfo(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const PathF& other) const
  {
    return _api.pathf_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Modified]
  // --------------------------------------------------------------------------

  //! @brief Must be called when the path was modified to invalidate the cached
  //! bounding-box, path type, and path-info.
  FOG_INLINE void _modified()
  {
    FOG_ASSERT(isDetached());
    _d->vType |= PATH_FLAG_DIRTY_BBOX | PATH_FLAG_DIRTY_CMD | PATH_FLAG_DIRTY_INFO;
  }

  //! @brief Called when the path vertices were manually modified, invalidating
  //! bounding-box and path-info cache.
  FOG_INLINE void _modifiedVertices() const
  {
    FOG_ASSERT(isDetached());
    _d->vType |= PATH_FLAG_DIRTY_BBOX | PATH_FLAG_DIRTY_INFO;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PathF& operator=(const PathF& other) { setPath(other); return *this; }
  FOG_INLINE PathF& operator+=(const PathF& other) { append(other); return *this; }

  FOG_INLINE bool operator==(const PathF& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const PathF& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const PathF& empty()
  {
    return *_api.pathf_oEmpty;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const PathF* a, const PathF* b)
  {
    return _api.pathf_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.pathf_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(PathDataF)
};

// ============================================================================
// [Fog::PathD]
// ============================================================================

//! @brief Path (double).
struct FOG_NO_EXPORT PathD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathD()
  {
    _api.pathd_ctor(this);
  }

  FOG_INLINE PathD(const PathD& other)
  {
    _api.pathd_ctorCopyD(this, &other);
  }

  explicit FOG_INLINE PathD(PathDataD* d) : _d(d) {}

  FOG_INLINE ~PathD()
  {
    _api.pathd_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE bool isDetached() const { return getReference() == 1; }

  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : _api.pathd_detach(this);
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  //! @brief Get path capacity (count of allocated vertices).
  FOG_INLINE size_t getCapacity() const { return _d->capacity; }
  //! @brief Get path length (count of vertices used).
  FOG_INLINE size_t getLength() const { return _d->length; }
  //! @brief Get whether path is empty.
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  FOG_INLINE err_t reserve(size_t capacity)
  {
    return _api.pathd_reserve(this, capacity);
  }

  FOG_INLINE void squeeze()
  {
    return _api.pathd_squeeze(this);
  }

  FOG_INLINE size_t _prepare(uint32_t cntOp, size_t count)
  {
    return _api.pathd_prepare(this, cntOp, count);
  }

  FOG_INLINE size_t _add(size_t count)
  {
    return _api.pathd_add(this, count);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get path commands array (const).
  FOG_INLINE const uint8_t* getCommands() const
  {
    return _d->commands;
  }

  //! @brief Get path commands array (mutable).
  FOG_INLINE uint8_t* getCommandsX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::PathD::getCommandsX() - Not detached.");
    return _d->commands;
  }

  //! @brief Get path vertices array (const).
  FOG_INLINE const PointD* getVertices() const
  {
    return _d->vertices;
  }

  //! @brief Get path vertices array (mutable).
  FOG_INLINE PointD* getVerticesX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::PathD::getVerticesX() - Not detached.");
    return _d->vertices;
  }

  FOG_INLINE const PointD& getVertex(size_t index) const
  {
    FOG_ASSERT_X(index < _d->length, "Fog::PathD::getVertex() - Index out of range.");
    return _d->vertices[index];
  }

  FOG_INLINE PointD getLastVertex() const
  {
    PointD result(UNINITIALIZED);
    _api.pathd_getLastVertex(this, &result);
    return result;
  }

  FOG_INLINE err_t setVertex(size_t index, const PointD& pt)
  {
    FOG_ASSERT_X(index < _d->length, "Fog::PathD::setVertex() - Index out of range.");
    return _api.pathd_setVertex(this, index, &pt);
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _api.pathd_clear(this);
  }

  FOG_INLINE void reset()
  {
    _api.pathd_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setPath(const PathD& other)
  {
    return _api.pathd_setPathD(this, &other);
  }

  FOG_INLINE err_t setPath(const PathF& other)
  {
    return _api.pathd_setPathF(this, &other);
  }

  FOG_INLINE err_t setDeep(const PathD& other)
  {
    return _api.pathd_setDeepD(this, &other);
  }

  // --------------------------------------------------------------------------
  // [SubPath]
  // --------------------------------------------------------------------------

  //! @brief Get range of subpath at index @a index (range is from the @a index
  //! to next 'move-to' command or to the end of the path).
  FOG_INLINE err_t getSubpathRange(Range& dst, size_t index) const
  {
    return _api.pathd_getSubpathRange(this, &dst, index);
  }

  // --------------------------------------------------------------------------
  // [MoveTo]
  // --------------------------------------------------------------------------

  //! @brief Move to @a pt0 (absolute).
  FOG_INLINE err_t moveTo(const PointD& pt0)
  {
    return _api.pathd_moveTo(this, &pt0);
  }

  //! @brief Move to @a pt0 (relative).
  FOG_INLINE err_t moveToRel(const PointD& pt0)
  {
    return _api.pathd_moveToRel(this, &pt0);
  }

  // --------------------------------------------------------------------------
  // [LineTo]
  // --------------------------------------------------------------------------

  //! @brief Line to @a pt1 (absolute).
  FOG_INLINE err_t lineTo(const PointD& pt1)
  {
    return _api.pathd_lineTo(this, &pt1);
  }

  //! @brief Line to @a pt1 (relative).
  FOG_INLINE err_t lineToRel(const PointD& pt1)
  {
    return _api.pathd_lineToRel(this, &pt1);
  }

  //! @brief Horizontal line to @a x (absolute).
  FOG_INLINE err_t hlineTo(double x)
  {
    return _api.pathd_hlineTo(this, x);
  }

  //! @brief Horizontal line to @a x (relative).
  FOG_INLINE err_t hlineToRel(double x)
  {
    return _api.pathd_hlineToRel(this, x);
  }

  //! @brief Horizontal line to @a y (absolute).
  FOG_INLINE err_t vlineTo(double y)
  {
    return _api.pathd_vlineTo(this, y);
  }

  //! @brief Horizontal line to @a y (relative).
  FOG_INLINE err_t vlineToRel(double y)
  {
    return _api.pathd_vlineToRel(this, y);
  }

  // --------------------------------------------------------------------------
  // [PolyTo]
  // --------------------------------------------------------------------------

  //! @brief Polyline to @a pts (absolute).
  FOG_INLINE err_t polyTo(const PointD* pts, size_t count)
  {
    return _api.pathd_polyTo(this, pts, count);
  }

  //! @brief Polyline to @a pts (relative).
  FOG_INLINE err_t polyToRel(const PointD* pts, size_t count)
  {
    return _api.pathd_polyToRel(this, pts, count);
  }

  // --------------------------------------------------------------------------
  // [QuadTo]
  // --------------------------------------------------------------------------

  //! @brief Quadratic curve to @a pt1, and @a pt2 (absolute).
  FOG_INLINE err_t quadTo(const PointD& pt1, const PointD& pt2)
  {
    return _api.pathd_quadTo(this, &pt1, &pt2);
  }

  //! @brief Quadratic curve to @a pt1, and @a pt2 (relative).
  FOG_INLINE err_t quadToRel(const PointD& pt1, const PointD& pt2)
  {
    return _api.pathd_quadToRel(this, &pt1, &pt2);
  }

  //! @brief Smooth quadratic curve to @a pt2, calculating pt1 from last points (absolute).
  FOG_INLINE err_t smoothQuadTo(const PointD& pt2)
  {
    return _api.pathd_smoothQuadTo(this, &pt2);
  }

  //! @brief Smooth quadratic curve to @a pt2, calculating pt1 from last points (relative).
  FOG_INLINE err_t smoothQuadToRel(const PointD& pt2)
  {
    return _api.pathd_smoothQuadToRel(this, &pt2);
  }

  // --------------------------------------------------------------------------
  // [CubicTo]
  // --------------------------------------------------------------------------

  //! @brief Cubic curve to @a pt1, @a pt2, and @a pt3 (absolute).
  FOG_INLINE err_t cubicTo(const PointD& pt1, const PointD& pt2, const PointD& pt3)
  {
    return _api.pathd_cubicTo(this, &pt1, &pt2, &pt3);
  }

  //! @brief Cubic curve to @a pt1, @a pt2, and @a pt3 (relative).
  FOG_INLINE err_t cubicToRel(const PointD& pt1, const PointD& pt2, const PointD& pt3)
  {
    return _api.pathd_cubicToRel(this, &pt1, &pt2, &pt3);
  }

  //! @brief Smooth cubic curve to @a pt2, and @a pt3, calculating pt1 from last points (absolute).
  FOG_INLINE err_t smoothCubicTo(const PointD& pt2, const PointD& pt3)
  {
    return _api.pathd_smoothCubicTo(this, &pt2, &pt3);
  }

  //! @brief Smooth cubic curve to @a pt2, and @a pt3, calculating pt1 from last points (relative).
  FOG_INLINE err_t smoothCubicToRel(const PointD& pt2, const PointD& pt3)
  {
    return _api.pathd_smoothCubicToRel(this, &pt2, &pt3);
  }

  // --------------------------------------------------------------------------
  // [ArcTo]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t arcTo(const PointD& cp, const PointD& rp, double start, double sweep, bool startPath = false)
  {
    return _api.pathd_arcTo(this, &cp, &rp, start, sweep, startPath);
  }

  FOG_INLINE err_t arcToRel(const PointD& cp, const PointD& rp, double start, double sweep, bool startPath = false)
  {
    return _api.pathd_arcToRel(this, &cp, &rp, start, sweep, startPath);
  }

  FOG_INLINE err_t svgArcTo(const PointD& rp, double angle, bool largeArcFlag, bool sweepFlag, const PointD& pt)
  {
    return _api.pathd_svgArcTo(this, &rp, angle, largeArcFlag, sweepFlag, &pt);
  }

  FOG_INLINE err_t svgArcToRel(const PointD& rp, double angle, bool largeArcFlag, bool sweepFlag, const PointD& pt)
  {
    return _api.pathd_svgArcToRel(this, &rp, angle, largeArcFlag, sweepFlag, &pt);
  }

  // --------------------------------------------------------------------------
  // [Close]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t close()
  {
    return _api.pathd_close(this);
  }

  // --------------------------------------------------------------------------
  // [Box / Rect]
  // --------------------------------------------------------------------------

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t box(const BoxI& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_boxI(this, &r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t box(const BoxF& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_boxF(this, &r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t box(const BoxD& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_boxD(this, &r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t rect(const RectI& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_rectI(this, &r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t rect(const RectF& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_rectF(this, &r, direction);
  }

  //! @brief Add a closed rectangle to the path.
  FOG_INLINE err_t rect(const RectD& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_rectD(this, &r, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t boxes(const BoxI* b, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_boxesI(this, b, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t boxes(const BoxF* b, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_boxesF(this, b, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t boxes(const BoxD* b, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_boxesD(this, b, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t rects(const RectI* r, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_rectsI(this, r, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t rects(const RectF* r, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_rectsF(this, r, count, direction);
  }

  //! @brief Add a set of rectangles to the path.
  FOG_INLINE err_t rects(const RectD* r, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_rectsD(this, r, count, direction);
  }

  // --------------------------------------------------------------------------
  // [Region]
  // --------------------------------------------------------------------------

  //! @brief Add a closed region (converted to set of rectangles).
  FOG_INLINE err_t region(const Region& r, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_region(this, &r, direction);
  }

  // --------------------------------------------------------------------------
  // [Polyline / Polygon]
  // --------------------------------------------------------------------------

  //! @brief Add a polyline to the path.
  FOG_INLINE err_t polyline(const PointI* pts, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_polylineI(this, pts, count, direction);
  }

  //! @brief Add a polyline to the path.
  FOG_INLINE err_t polyline(const PointD* pts, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_polylineD(this, pts, count, direction);
  }

  //! @brief Add a closed polygon to the path.
  FOG_INLINE err_t polygon(const PointI* pts, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_polygonI(this, pts, count, direction);
  }

  //! @brief Add a closed polygon to the path.
  FOG_INLINE err_t polygon(const PointD* pts, size_t count, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _api.pathd_polygonD(this, pts, count, direction);
  }

  // --------------------------------------------------------------------------
  // [Shape]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t _shape(uint32_t shapeType, const void* shapeData, uint32_t direction, const TransformD* tr = NULL)
  {
    return _api.pathd_shape(this, shapeType, shapeData, direction, tr);
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
  FOG_INLINE err_t qbezier(const QBezierD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_QBEZIER, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t qbezier(const QBezierD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(SHAPE_TYPE_QBEZIER, &object, direction, &tr);
  }

  //! @brief Add an unclosed cubic bézier curve to the path.
  FOG_INLINE err_t cbezier(const QBezierD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_CBEZIER, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t cbezier(const QBezierD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(SHAPE_TYPE_CBEZIER, &object, direction, &tr);
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

  //! @brief Add a closed triangle.
  FOG_INLINE err_t triangle(const TriangleD& object, uint32_t direction = PATH_DIRECTION_CW)
  {
    return _shape(SHAPE_TYPE_TRIANGLE, &object, direction, NULL);
  }

  //! @overload
  FOG_INLINE err_t triangle(const TriangleD& object, uint32_t direction, const TransformD& tr)
  {
    return _shape(SHAPE_TYPE_TRIANGLE, &object, direction, &tr);
  }

  // --------------------------------------------------------------------------
  // [Path]
  // --------------------------------------------------------------------------

  //! @brief Add another @a path to the path.
  FOG_INLINE err_t append(const PathD& path)
  {
    return _api.pathd_appendPathD(this, &path, NULL);
  }

  //! @brief Add another @a path to the path.
  FOG_INLINE err_t append(const PathF& path)
  {
    return _api.pathd_appendPathF(this, &path, NULL);
  }

  //! @brief Add another @a path to the path.
  FOG_INLINE err_t append(const PathD& path, const Range& range)
  {
    return _api.pathd_appendPathD(this, &path, &range);
  }

  //! @brief Add another @a path to the path.
  FOG_INLINE err_t append(const PathF& path, const Range& range)
  {
    return _api.pathd_appendPathF(this, &path, &range);
  }

  //! @brief Add another @a path (translated by the @a pt) to the path.
  FOG_INLINE err_t appendTranslated(const PathF& path, const PointD& pt)
  {
    return _api.pathd_appendTranslatedPathF(this, &path, &pt, NULL);
  }

  //! @brief Add another @a path (translated by the @a pt) to the path.
  FOG_INLINE err_t appendTranslated(const PathD& path, const PointD& pt)
  {
    return _api.pathd_appendTranslatedPathD(this, &path, &pt, NULL);
  }

  //! @brief Add another @a path (translated by the @a pt) to the path.
  FOG_INLINE err_t appendTranslated(const PathF& path, const PointD& pt, const Range& range)
  {
    return _api.pathd_appendTranslatedPathF(this, &path, &pt, &range);
  }

  //! @brief Add another @a path (translated by the @a pt) to the path.
  FOG_INLINE err_t appendTranslated(const PathD& path, const PointD& pt, const Range& range)
  {
    return _api.pathd_appendTranslatedPathD(this, &path, &pt, &range);
  }

  //! @brief Add another @a path (transformed by the @a tr) to the path.
  FOG_INLINE err_t appendTransformed(const PathF& path, const TransformD& tr)
  {
    return _api.pathd_appendTransformedPathF(this, &path, &tr, NULL);
  }

  //! @brief Add another @a path (transformed by the @a tr) to the path.
  FOG_INLINE err_t appendTransformed(const PathD& path, const TransformD& tr)
  {
    return _api.pathd_appendTransformedPathD(this, &path, &tr, NULL);
  }

  //! @brief Add another @a path (transformed by the @a tr) to the path.
  FOG_INLINE err_t appendTransformed(const PathF& path, const TransformD& tr, const Range& range)
  {
    return _api.pathd_appendTransformedPathF(this, &path, &tr, &range);
  }

  //! @brief Add another @a path (transformed by the @a tr) to the path.
  FOG_INLINE err_t appendTransformed(const PathD& path, const TransformD& tr, const Range& range)
  {
    return _api.pathd_appendTransformedPathD(this, &path, &tr, &range);
  }

  // --------------------------------------------------------------------------
  // [Flat]
  // --------------------------------------------------------------------------

  //! @brief Get whether the path has quadratic or cubic Bezier curves.
  FOG_INLINE bool hasBeziers() const
  {
    if (_d->vType & PATH_FLAG_DIRTY_CMD)
      _api.pathd_updateFlat(this);

    return (_d->vType & (PATH_FLAG_HAS_QBEZIER | PATH_FLAG_HAS_CBEZIER)) != 0;
  }

  FOG_INLINE bool hasQBeziers() const
  {
    if (_d->vType & PATH_FLAG_DIRTY_CMD)
      _api.pathd_updateFlat(this);

    return (_d->vType & PATH_FLAG_HAS_QBEZIER) != 0;
  }

  FOG_INLINE bool hasCBeziers() const
  {
    if (_d->vType & PATH_FLAG_DIRTY_CMD)
      _api.pathd_updateFlat(this);

    return (_d->vType & PATH_FLAG_HAS_CBEZIER) != 0;
  }

  FOG_INLINE err_t flatten(const PathFlattenParamsD& params)
  {
    return _api.pathd_flatten(this, this, &params, NULL);
  }

  static FOG_INLINE err_t flatten(PathD& dst, const PathD& src, const PathFlattenParamsD& params)
  {
    return _api.pathd_flatten(&dst, &src, &params, NULL);
  }

  static FOG_INLINE err_t flatten(PathD& dst, const PathD& src, const Range& range, const PathFlattenParamsD& params)
  {
    return _api.pathd_flatten(&dst, &src, &params, &range);
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
    return _api.pathd_getBoundingBox(this, &dst, tr);
  }

  FOG_INLINE err_t _getBoundingRect(RectD& dst, const TransformD* tr) const
  {
    err_t err = _api.pathd_getBoundingBox(this, reinterpret_cast<BoxD*>(&dst), tr);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointD& pt, uint32_t fillRule) const
  {
    return _api.pathd_hitTest(this, &pt, fillRule);
  }

  // --------------------------------------------------------------------------
  // [GetClosestVertex]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getClosestVertex(const PointD& pt, double maxDistance, double* distance = NULL) const
  {
    return _api.pathd_getClosestVertex(this, &pt, maxDistance, distance);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  //! @brief Translate all vertices by @a pt.
  FOG_INLINE err_t translate(const PointD& pt)
  {
    return _api.pathd_translate(this, &pt, NULL);
  }

  //! @brief Translate all vertices in range @a range by @a pt.
  FOG_INLINE err_t translate(const PointD& pt, const Range& range)
  {
    return _api.pathd_translate(this, &pt, &range);
  }

  //! @brief Transform all vertices by @a tr.
  FOG_INLINE err_t transform(const TransformD& tr)
  {
    return _api.pathd_transform(this, &tr, NULL);
  }

  //! @brief Transform all vertices in range @a range by @a tr.
  FOG_INLINE err_t transform(const TransformD& tr, const Range& range)
  {
    return _api.pathd_transform(this, &tr, &range);
  }

  //! @brief Fit path into the given rectangle @a r.
  FOG_INLINE err_t fitTo(const RectD& r)
  {
    return _api.pathd_fitTo(this, &r);
  }

  //! @brief Scale each vertex by @a pt.
  FOG_INLINE err_t scale(const PointD& pt, bool keepStartPos = false)
  {
    return _api.pathd_scale(this, &pt, keepStartPos);
  }

  // --------------------------------------------------------------------------
  // [FlipX / FlipY]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t flipX(double x0, double x1)
  {
    return _api.pathd_flipX(this, x0, x1);
  }

  FOG_INLINE err_t flipY(double y0, double y1)
  {
    return _api.pathd_flipY(this, y0, y1);
  }

  // --------------------------------------------------------------------------
  // [Path-Info]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasPathInfo() const
  {
    return (_d->vType & PATH_FLAG_DIRTY_INFO) == 0 && _d->info != NULL;
  }

  FOG_INLINE void buildPathInfo() const
  {
    (void)_api.pathd_getPathInfo(this);
  }

  FOG_INLINE const PathInfoD* getPathInfo() const
  {
    return _api.pathd_getPathInfo(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const PathD& other) const
  {
    return _api.pathd_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Modified]
  // --------------------------------------------------------------------------

  //! @brief Must be called when the path was modified to invalidate the cached
  //! bounding-box, path type, and path-info.
  FOG_INLINE void _modified()
  {
    FOG_ASSERT(isDetached());
    _d->vType |= PATH_FLAG_DIRTY_BBOX | PATH_FLAG_DIRTY_CMD | PATH_FLAG_DIRTY_INFO;
  }

  //! @brief Called when the path vertices were manually modified, invalidating
  //! bounding-box and path-info cache.
  FOG_INLINE void _modifiedVertices() const
  {
    FOG_ASSERT(isDetached());
    _d->vType |= PATH_FLAG_DIRTY_BBOX | PATH_FLAG_DIRTY_INFO;
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
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const PathD& empty()
  {
    return *_api.pathd_oEmpty;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const PathD* a, const PathD* b)
  {
    return _api.pathd_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.pathd_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(PathDataD)
};

// ============================================================================
// [Fog::PathT<> / Fog::PathDataT<> / Fog::PathFlattenParamsT<>]
// ============================================================================

_FOG_NUM_T(Path)
_FOG_NUM_T(PathData)
_FOG_NUM_T(PathFlattenParams)
_FOG_NUM_F(Path)
_FOG_NUM_F(PathData)
_FOG_NUM_F(PathFlattenParams)
_FOG_NUM_D(Path)
_FOG_NUM_D(PathData)
_FOG_NUM_D(PathFlattenParams)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PATH_H
