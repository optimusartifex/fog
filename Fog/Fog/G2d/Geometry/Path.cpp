// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Util.h>
#include <Fog/Core/Global/Internal_Core_p.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/MathVec.h>
#include <Fog/Core/Math/Solve.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Internals_p.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathTmp_p.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Api.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Global/Init_G2d_p.h>
#include <Fog/G2d/Tools/Region.h>

namespace Fog {

// ============================================================================
// [Fog::Path - DNull]
// ============================================================================

static PathDataF _PathF_dnull;
static PathDataD _PathD_dnull;

template<typename NumT>
FOG_INLINE NumT_(PathData)* _PathT_getDNull() { return NULL; }

template<>
FOG_INLINE PathDataF* _PathT_getDNull<float>() { return &_PathF_dnull; }

template<>
FOG_INLINE PathDataD* _PathT_getDNull<double>() { return &_PathD_dnull; }

// ============================================================================
// [Fog::Path - Helpers]
// ============================================================================

template<typename NumT>
static FOG_INLINE size_t _PathT_getDataSize(size_t capacity)
{
  return sizeof(NumT_(PathData)) + capacity * (sizeof(NumT_(Point)) + 1);
}

template<typename NumT>
static FOG_INLINE void _PathT_updateDataPointers(
  NumT_(PathData)* d, size_t capacity)
{
  d->vertices = (NumT_(Point)*) (((size_t)d->commands + capacity + 15) & ~(size_t)15);
}

template<typename NumT>
static bool FOG_FASTCALL _PathT_getLastPoint(
  NumT_(PathData)* d, NumT_(Point)& dst)
{
  size_t last = d->length;
  if (!last) return false;

  const uint8_t* commands = d->commands;

  uint8_t c = commands[--last];
  if (PathCmd::isVertex(c)) goto _End;

  while (last)
  {
    c = commands[--last];
    if (PathCmd::isMoveTo(c)) goto _End;
  }
  return false;

_End:
  dst = d->vertices[last];
  return true;
}

#define PATH_ADD_VERTEX_BEGIN(NumT, _Count_) \
  { \
    size_t _length = self._d->length; \
    \
    { \
      size_t _remain = self._d->capacity - _length; \
      \
      if (FOG_UNLIKELY(!_length) || \
          FOG_UNLIKELY(!PathCmd::isVertex(self._d->commands[_length - 1]))) \
      { \
        return ERR_PATH_NO_VERTEX; \
      } \
      \
      if (self._d->refCount.get() == 1 && _Count_ <= _remain) \
      { \
        self._d->length += _Count_; \
      } \
      else \
      { \
        if (self._add(_Count_) == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY; \
      } \
    } \
    \
    uint8_t* commands = self._d->commands + _length; \
    NumT_(Point)* vertices = self._d->vertices + _length;

#define PATH_ADD_VERTEX_END() \
  } \
  return ERR_OK;

// ============================================================================
// [Fog::Path - Statics]
// ============================================================================

template<typename NumT>
static NumT_(PathData)* _PathT_dalloc(size_t capacity)
{
  size_t dsize = _PathT_getDataSize<NumT>(capacity);

  NumT_(PathData)* newd = reinterpret_cast<NumT_(PathData)*>(Memory::alloc(dsize));
  if (FOG_IS_NULL(newd)) return NULL;

  newd->refCount.init(1);
  newd->flags = PATH_DATA_DIRTY_BBOX | PATH_DATA_DIRTY_CMD;
#if FOG_ARCH_BITS >= 64
  newd->padding = 0;
#endif // FOG_ARCH_BITS >= 64
  newd->capacity = capacity;
  newd->length = 0;
  newd->boundingBox.reset();

  _PathT_updateDataPointers<NumT>(newd, capacity);
  return newd;
}

template<typename NumT>
static NumT_(PathData)* _PathT_drealloc(NumT_(PathData)* d, size_t capacity)
{
  FOG_ASSERT(d->length <= capacity);
  size_t dsize = _PathT_getDataSize<NumT>(capacity);

  NumT_(PathData)* newd = reinterpret_cast<NumT_(PathData)*>(Memory::alloc(dsize));
  if (FOG_IS_NULL(newd)) return NULL;

  size_t length = d->length;

  newd->refCount.init(1);
  newd->flags = d->flags & PATH_DATA_OWN_FLAGS;
#if FOG_ARCH_BITS >= 64
  newd->padding = 0;
#endif // FOG_ARCH_BITS >= 64
  newd->capacity = capacity;
  newd->length = length;
  newd->boundingBox = d->boundingBox;

  _PathT_updateDataPointers<NumT>(newd, capacity);
  Memory::copy(newd->commands, d->commands, length);
  Memory::copy(newd->vertices, d->vertices, length * sizeof(NumT_(Point)));

  d->deref();
  return newd;
}

template<typename NumT>
static NumT_(PathData)* _PathT_dcopy(const NumT_(PathData)* d)
{
  size_t length = d->length;
  if (length == 0) return _PathT_getDNull<NumT>()->ref();

  NumT_(PathData)* newd = _PathT_dalloc<NumT>(length);
  if (FOG_IS_NULL(newd)) return NULL;

  newd->flags |= d->flags & PATH_DATA_OWN_FLAGS;
  newd->length = length;
  newd->boundingBox = d->boundingBox;

  Memory::copy(newd->commands, d->commands, length);
  Memory::copy(newd->vertices, d->vertices, length * sizeof(NumT_(Point)));

  return newd;
}

// ============================================================================
// [Fog::Path - Construction / Destruction]
// ============================================================================

template<typename NumT>
static void _PathT_ctor(NumT_(Path)& self)
{
  self._d = _PathT_getDNull<NumT>()->ref();
}

template<typename NumT>
static void _PathT_ctorCopyT(NumT_(Path)& self, const NumT_(Path)& other)
{
  self._d = other._d->ref();
}

template<typename NumT>
static void _PathT_dtor(NumT_(Path)& self)
{
  self._d->deref();
}

// ============================================================================
// [Fog::Path - Data]
// ============================================================================

template<typename NumT>
static err_t _PathT_detach(NumT_(Path)& self)
{
  if (self.isDetached()) return ERR_OK;

  NumT_(PathData)* newd = _PathT_dcopy<NumT>(self._d);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&self._d, newd)->deref();
  return ERR_OK;
}

template<typename NumT>
static err_t _PathT_reserve(NumT_(Path)& self, size_t capacity)
{
  if (self._d->refCount.get() == 1 && self._d->capacity >= capacity) return ERR_OK;

  size_t length = self._d->length;
  if (capacity < length) capacity = length;

  NumT_(PathData)* newd = _PathT_dalloc<NumT>(capacity);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  newd->flags |= self._d->flags & PATH_DATA_OWN_FLAGS;
  newd->length = length;
  newd->boundingBox = self._d->boundingBox;

  Memory::copy(newd->commands, self._d->commands, length);
  Memory::copy(newd->vertices, self._d->vertices, length * sizeof(NumT_(Point)));

  atomicPtrXchg(&self._d, newd)->deref();
  return ERR_OK;
}

template<typename NumT>
static void _PathT_squeeze(NumT_(Path)& self)
{
  if (self._d->length == self._d->capacity) return;

  if (self._d->refCount.get() == 1)
  {
    NumT_(PathData)* newd = _PathT_drealloc<NumT>(self._d, self._d->length);
    if (FOG_IS_NULL(newd)) return;

    atomicPtrXchg(&self._d, newd);
  }
  else
  {
    NumT_(PathData)* newd = _PathT_dcopy<NumT>(self._d);
    if (FOG_IS_NULL(newd)) return;

    atomicPtrXchg(&self._d, newd)->deref();
  }
}

template<typename NumT>
static size_t _PathT_prepare(NumT_(Path)& self, size_t count, uint32_t cntOp)
{
  size_t start = (cntOp == CONTAINER_OP_REPLACE) ? 0 : self._d->length;
  size_t remain = self._d->capacity - start;

  if (self._d->refCount.get() == 1 && count <= remain)
  {
    self._d->length = start + count;
  }
  else
  {
    size_t optimalCapacity =
      Util::getGrowCapacity(sizeof(NumT_(PathData)), sizeof(NumT_(Point)) + sizeof(uint8_t), start, start + count);

    NumT_(PathData)* newd = _PathT_dalloc<NumT>(optimalCapacity);
    if (FOG_IS_NULL(newd)) return INVALID_INDEX;

    newd->flags |= self._d->flags & PATH_DATA_OWN_FLAGS;
    newd->length = start + count;
    newd->boundingBox = self._d->boundingBox;

    if (start)
    {
      Memory::copy(newd->commands, self._d->commands, start);
      Memory::copy(newd->vertices, self._d->vertices, start * sizeof(NumT_(Point)));
    }

    atomicPtrXchg(&self._d, newd)->deref();
  }

  if (cntOp == CONTAINER_OP_REPLACE) self._d->flags &= ~PATH_DATA_OWN_FLAGS;
  return start;
}

template<typename NumT>
static size_t _PathT_add(NumT_(Path)& self, size_t count)
{
  size_t length = self._d->length;
  size_t remain = self._d->capacity - length;

  if (self._d->refCount.get() == 1 && count <= remain)
  {
    self._d->length += count;
  }
  else
  {
    size_t optimalCapacity =
      Util::getGrowCapacity(sizeof(NumT_(PathData)), sizeof(NumT_(Point)) + sizeof(uint8_t), length, length + count);

    NumT_(PathData)* newd = _PathT_dalloc<NumT>(optimalCapacity);
    if (FOG_IS_NULL(newd)) return INVALID_INDEX;

    newd->flags |= self._d->flags & PATH_DATA_OWN_FLAGS;
    newd->length = length + count;
    newd->boundingBox = self._d->boundingBox;

    Memory::copy(newd->commands, self._d->commands, length);
    Memory::copy(newd->vertices, self._d->vertices, length * sizeof(NumT_(Point)));

    atomicPtrXchg(&self._d, newd)->deref();
  }

  return length;
}

// ============================================================================
// [Fog::Path - Clear / Free]
// ============================================================================

template<typename NumT>
static void _PathT_clear(NumT_(Path)& self)
{
  if (self._d->refCount.get() > 1)
  {
    atomicPtrXchg(&self._d, _PathT_getDNull<NumT>()->ref())->deref();
  }
  else
  {
    self._d->length = 0;
    self._d->flags &= ~PATH_DATA_OWN_FLAGS;
    self._d->boundingBox.reset();
  }
}

template<typename NumT>
static void _PathT_reset(NumT_(Path)& self)
{
  atomicPtrXchg(&self._d, _PathT_getDNull<NumT>()->ref())->deref();
}

// ============================================================================
// [Fog::Path - Set]
// ============================================================================

template<typename NumT>
static err_t _PathT_setPathT(NumT_(Path)& self, const NumT_(Path)& other)
{
  if (self._d == other._d) return ERR_OK;

  atomicPtrXchg(&self._d, other._d->ref())->deref();
  return ERR_OK;
}

template<typename NumT, typename SrcT>
static err_t _PathT_setDeepT(NumT_(Path)& self, const SrcT_(Path)& other)
{
  NumT_(PathData)* self_d = self._d;
  SrcT_(PathData)* other_d = other._d;

  if (sizeof(NumT) == sizeof(SrcT) && (void*)self_d == (void*)other_d)
    return ERR_OK;

  if (other_d->length == 0)
  {
    self.clear();
    return ERR_OK;
  }

  err_t err = self.reserve(other_d->length);
  if (FOG_IS_ERROR(err))
  {
    self.clear();
    return ERR_RT_OUT_OF_MEMORY;
  }

  size_t length = other_d->length;

  self_d = self._d;
  self_d->flags = (self._d->flags & ~PATH_DATA_OWN_FLAGS) | (other_d->flags & PATH_DATA_OWN_FLAGS);
  self_d->length = length;
  self_d->boundingBox = other_d->boundingBox;

  Memory::copy(self_d->commands, other_d->commands, length);
  Math::vConvertFloat<NumT, SrcT>(
    reinterpret_cast<NumT*>(self_d->vertices),
    reinterpret_cast<const SrcT*>(other_d->vertices), length * 2);

  return ERR_OK;
}

// ============================================================================
// [Fog::Path - SubPath]
// ============================================================================

template<typename NumT>
static Range _PathT_getSubpathRange(const NumT_(Path)& self, size_t index)
{
  size_t length = self._d->length;
  if (index >= length) return Range(INVALID_INDEX, INVALID_INDEX);

  size_t i = index + 1;
  const uint8_t* commands = self._d->commands;

  while (i < length)
  {
    uint8_t c = commands[i];
    if (PathCmd::isMoveTo(c)) break;
    if (PathCmd::isClose(c)) { i++; break; }
    i++;
  }

  return Range(index, i);
}

// ============================================================================
// [Fog::Path - MoveTo]
// ============================================================================

template<typename NumT>
static err_t _PathT_moveTo(NumT_(Path)& self, const NumT_(Point)& pt0)
{
  size_t pos = self._add(1);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  NumT_(Point)* vertices = self._d->vertices + pos;

  commands[0] = PATH_CMD_MOVE_TO;
  vertices[0] = pt0;
  self._d->flags |= PATH_DATA_DIRTY_BBOX;

  return ERR_OK;
}

template<typename NumT>
static err_t _PathT_moveToRel(NumT_(Path)& self, const NumT_(Point)& pt0)
{
  NumT_(Point) tr;
  if (!_PathT_getLastPoint<NumT>(self._d, tr)) return ERR_PATH_NO_RELATIVE;

  return self.moveTo(pt0 + tr);
}

// ============================================================================
// [Fog::Path - LineTo]
// ============================================================================

template<typename NumT>
static err_t _PathT_lineTo(NumT_(Path)& self, const NumT_(Point)& pt1)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 1)
    commands[0] = PATH_CMD_LINE_TO;
    vertices[0] = pt1;
    self._d->flags |= PATH_DATA_DIRTY_BBOX;
  PATH_ADD_VERTEX_END()
}

template<typename NumT>
static err_t _PathT_lineToRel(NumT_(Path)& self, const NumT_(Point)& pt1)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 1)
    NumT_(Point) tr(vertices[-1]);

    commands[0] = PATH_CMD_LINE_TO;
    vertices[0] = pt1 + tr;
    self._d->flags |= PATH_DATA_DIRTY_BBOX;
  PATH_ADD_VERTEX_END()
}

template<typename NumT>
static err_t _PathT_hlineTo(NumT_(Path)& self, NumT x)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 1)
    commands[0] = PATH_CMD_LINE_TO;
    vertices[0].set(x, vertices[-1].y);
    self._d->flags |= PATH_DATA_DIRTY_BBOX;
  PATH_ADD_VERTEX_END()
}

template<typename NumT>
static err_t _PathT_hlineToRel(NumT_(Path)& self, NumT x)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 1)
    commands[0] = PATH_CMD_LINE_TO;
    vertices[0].set(x + vertices[-1].x, vertices[-1].y);
    self._d->flags |= PATH_DATA_DIRTY_BBOX;
  PATH_ADD_VERTEX_END()
}

template<typename NumT>
static err_t _PathT_vlineTo(NumT_(Path)& self, NumT y)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 1)
    commands[0] = PATH_CMD_LINE_TO;
    vertices[0].set(vertices[-1].x, y);
    self._d->flags |= PATH_DATA_DIRTY_BBOX;
  PATH_ADD_VERTEX_END()
}

template<typename NumT>
static err_t _PathT_vlineToRel(NumT_(Path)& self, NumT y)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 1)
    commands[0] = PATH_CMD_LINE_TO;
    vertices[0].set(vertices[-1].x, y + vertices[-1].y);
    self._d->flags |= PATH_DATA_DIRTY_BBOX;
  PATH_ADD_VERTEX_END()
}

// ============================================================================
// [Fog::Path - PolyTo]
// ============================================================================

template<typename NumT>
static err_t _PathT_polyTo(NumT_(Path)& self, const NumT_(Point)* pts, size_t count)
{
  if (count == 0) return ERR_OK;
  FOG_ASSERT(pts != NULL);

  PATH_ADD_VERTEX_BEGIN(NumT, count)
    size_t i;

    for (i = 0; i < count; i++) commands[i] = PATH_CMD_LINE_TO;
    for (i = 0; i < count; i++) vertices[i] = pts[i];
    self._d->flags |= PATH_DATA_DIRTY_BBOX;
  PATH_ADD_VERTEX_END()
}

template<typename NumT>
static err_t _PathT_polyToRel(NumT_(Path)& self, const NumT_(Point)* pts, size_t count)
{
  if (count == 0) return ERR_OK;
  FOG_ASSERT(pts != NULL);

  PATH_ADD_VERTEX_BEGIN(NumT, count)
    size_t i;
    NumT_(Point) tr(vertices[-1]);

    for (i = 0; i < count; i++) commands[i] = PATH_CMD_LINE_TO;
    for (i = 0; i < count; i++) vertices[i] = pts[i] + tr;
    self._d->flags |= PATH_DATA_DIRTY_BBOX;
  PATH_ADD_VERTEX_END()
}

// ============================================================================
// [Fog::Path - QuadTo]
// ============================================================================

template<typename NumT>
static err_t _PathT_quadTo(
  NumT_(Path)& self,
  const NumT_(Point)& pt1,
  const NumT_(Point)& pt2)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 2)
    commands[0] = PATH_CMD_QUAD_TO;
    commands[1] = PATH_CMD_QUAD_TO;

    vertices[0] = pt1;
    vertices[1] = pt2;
    self._d->flags |= PATH_DATA_DIRTY_BBOX | PATH_DATA_HAS_QBEZIER;
  PATH_ADD_VERTEX_END()
}

template<typename NumT>
static err_t _PathT_quadToRel(
  NumT_(Path)& self,
  const NumT_(Point)& pt1,
  const NumT_(Point)& pt2)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 2)
    NumT_(Point) tr(vertices[-1]);

    commands[0] = PATH_CMD_QUAD_TO;
    commands[1] = PATH_CMD_QUAD_TO;

    vertices[0] = pt1 + tr;
    vertices[1] = pt2 + tr;
    self._d->flags |= PATH_DATA_DIRTY_BBOX | PATH_DATA_HAS_QBEZIER;
  PATH_ADD_VERTEX_END()
}

template<typename NumT>
static err_t _PathT_smoothQuadTo(
  NumT_(Path)& self,
  const NumT_(Point)& pt2)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 2)
    NumT_(Point) pt1 = vertices[-1];

    commands[0] = PATH_CMD_QUAD_TO;
    commands[1] = PATH_CMD_QUAD_TO;

    if (_length >= 2 && PathCmd::isQuadOrCubicTo(commands[-2]))
    {
      pt1 += pt1;
      pt1 -= vertices[-2];
    }

    vertices[0] = pt1;
    vertices[1] = pt2;
    self._d->flags |= PATH_DATA_DIRTY_BBOX | PATH_DATA_HAS_QBEZIER;
  PATH_ADD_VERTEX_END()
}

template<typename NumT>
static err_t _PathT_smoothQuadToRel(
  NumT_(Path)& self,
  const NumT_(Point)& pt2)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 2)
    NumT_(Point) pt1(vertices[-1]);

    commands[0] = PATH_CMD_QUAD_TO;
    commands[1] = PATH_CMD_QUAD_TO;
    vertices[1] = pt2 + pt1;

    if (_length >= 2 && PathCmd::isQuadOrCubicTo(commands[-2]))
    {
      pt1 += pt1;
      pt1 -= vertices[-2];
    }

    vertices[0] = pt1;
    self._d->flags |= PATH_DATA_DIRTY_BBOX | PATH_DATA_HAS_QBEZIER;
  PATH_ADD_VERTEX_END()
}

// ============================================================================
// [Fog::Path - CubicTo]
// ============================================================================

template<typename NumT>
static err_t _PathT_cubicTo(
  NumT_(Path)& self,
  const NumT_(Point)& pt1,
  const NumT_(Point)& pt2,
  const NumT_(Point)& pt3)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 3)
    commands[0] = PATH_CMD_CUBIC_TO;
    commands[1] = PATH_CMD_CUBIC_TO;
    commands[2] = PATH_CMD_CUBIC_TO;

    vertices[0] = pt1;
    vertices[1] = pt2;
    vertices[2] = pt3;
    self._d->flags |= PATH_DATA_DIRTY_BBOX | PATH_DATA_HAS_CBEZIER;
  PATH_ADD_VERTEX_END()
}

template<typename NumT>
static err_t _PathT_cubicToRel(
  NumT_(Path)& self,
  const NumT_(Point)& pt1,
  const NumT_(Point)& pt2,
  const NumT_(Point)& pt3)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 3)
    NumT_(Point) tr(vertices[-1]);

    commands[0] = PATH_CMD_CUBIC_TO;
    commands[1] = PATH_CMD_CUBIC_TO;
    commands[2] = PATH_CMD_CUBIC_TO;

    vertices[0] = pt1 + tr;
    vertices[1] = pt2 + tr;
    vertices[2] = pt3 + tr;
    self._d->flags |= PATH_DATA_DIRTY_BBOX | PATH_DATA_HAS_CBEZIER;
  PATH_ADD_VERTEX_END()
}

template<typename NumT>
static err_t _PathT_smoothCubicTo(
  NumT_(Path)& self,
  const NumT_(Point)& pt2,
  const NumT_(Point)& pt3)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 3)
    NumT_(Point) pt1(vertices[-1]);

    commands[0] = PATH_CMD_CUBIC_TO;
    commands[1] = PATH_CMD_CUBIC_TO;
    commands[2] = PATH_CMD_CUBIC_TO;

    if (_length >= 2 && PathCmd::isQuadOrCubicTo(commands[-2]))
    {
      pt1 += pt1;
      pt1 -= vertices[-2];
    }

    vertices[0] = pt1;
    vertices[1] = pt2;
    vertices[2] = pt3;
    self._d->flags |= PATH_DATA_DIRTY_BBOX | PATH_DATA_HAS_CBEZIER;
  PATH_ADD_VERTEX_END()
}

template<typename NumT>
static err_t _PathT_smoothCubicToRel(
  NumT_(Path)& self,
  const NumT_(Point)& pt2,
  const NumT_(Point)& pt3)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 3)
    NumT_(Point) pt1(vertices[-1]);

    commands[0] = PATH_CMD_CUBIC_TO;
    commands[1] = PATH_CMD_CUBIC_TO;
    commands[2] = PATH_CMD_CUBIC_TO;
    vertices[1] = pt2 + pt1;
    vertices[2] = pt3 + pt1;

    if (_length >= 2 && PathCmd::isQuadOrCubicTo(commands[-2]))
    {
      pt1 += pt1;
      pt1 -= vertices[-2];
    }

    vertices[0] = pt1;
    self._d->flags |= PATH_DATA_DIRTY_BBOX | PATH_DATA_HAS_CBEZIER;
  PATH_ADD_VERTEX_END()
}

// ============================================================================
// [Fog::Path - ArcTo]
// ============================================================================

template<typename NumT>
static err_t _PathT_arcTo(
  NumT_(Path)& self,
  const NumT_(Point)& cp,
  const NumT_(Point)& rp,
  NumT start, NumT sweep, bool startPath)
{
  NumT_(Arc) arc(cp, rp, start, sweep);

  uint8_t* commands;
  NumT_(Point)* vertices;

  uint8_t initial = startPath ? PATH_CMD_MOVE_TO : PATH_CMD_LINE_TO;

  // Degenerated.
  if (Math::isFuzzyZero(sweep))
  {
    NumT as, ac;

    size_t pos = self._add(2);
    if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

    commands = self._d->commands + pos;
    vertices = self._d->vertices + pos;

    Math::sincos(start, &as, &ac);
    commands[0] = initial;
    vertices[0].set(cp.x + rp.x * ac, cp.y + rp.y * as);

    if (!(!startPath && pos > 0 && Fuzzy<NumT_(Point)>::eq(vertices[-1], vertices[0])))
    {
      commands++;
      vertices++;
    }
    else
    {
      self._d->length--;
    }

    Math::sincos(start + sweep, &as, &ac);
    commands[0] = PATH_CMD_LINE_TO;
    vertices[0].set(cp.x + rp.x * ac, cp.y + rp.y * as);

    self._d->flags |= PATH_DATA_DIRTY_BBOX;
  }
  else
  {
    size_t pos = self._add(13);
    if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

    commands = self._d->commands + pos;
    vertices = self._d->vertices + pos;

    uint len = arc.toCSpline(vertices);

    commands[0] = initial;
    for (uint i = 1; i < len; i++) commands[i] = PATH_CMD_CUBIC_TO;

    self._d->length = pos + len;
    self._d->flags |= PATH_DATA_DIRTY_BBOX | PATH_DATA_HAS_CBEZIER;
  }

  return ERR_OK;
}

template<typename NumT>
static err_t _PathT_arcToRel(
  NumT_(Path)& self,
  const NumT_(Point)& cp,
  const NumT_(Point)& r,
  NumT start, NumT sweep, bool startPath)
{
  NumT_(Point) tr;
  if (!_PathT_getLastPoint<NumT>(self._d, tr)) return ERR_PATH_NO_RELATIVE;

  return self.arcTo(cp + tr, r, start, sweep, startPath);
}

template<typename NumT>
static err_t _PathT_svgArcTo(
  NumT_(Path)& self,
  const NumT_(Point)& rp,
  NumT angle, bool largeArcFlag, bool sweepFlag,
  const NumT_(Point)& p2)
{
  // Mark current length (will be position where the first bezier would start).
  size_t mark = self._d->length;

  NumT_(Point) p0;
  bool radiiOk = true;

  // Get initial point - p0.
  if (!_PathT_getLastPoint<NumT>(self._d, p0)) return ERR_PATH_NO_RELATIVE;

  // Normalize radius.
  NumT rx = (rp.x >= NumT(0.0)) ? rp.x : -rp.x;
  NumT ry = (rp.y >= NumT(0.0)) ? rp.y : -rp.y;

  // Calculate the middle point between the current and the final points.
  NumT dx2 = (p0.x - p2.x) * NumT(0.5);
  NumT dy2 = (p0.y - p2.y) * NumT(0.5);

  NumT as, ac;
  Math::sincos(angle, &as, &ac);

  // Calculate middle point - p1.
  NumT_(Point) p1;
  p1.x =  ac * dx2 + as * dy2;
  p1.y = -as * dx2 + ac * dy2;

  // Ensure radii are large enough.
  NumT rx_2 = rx * rx;
  NumT ry_2 = ry * ry;
  NumT p1x_2 = p1.x * p1.x;
  NumT p1y_2 = p1.y * p1.y;

  // Check that radii are large enough.
  NumT radiiCheck = p1x_2 / rx_2 + p1y_2 / ry_2;

  if (radiiCheck > NumT(1.0))
  {
    NumT s = Math::sqrt(radiiCheck);
    rx *= s;
    ry *= s;
    rx_2 = Math::pow2(rx);
    ry_2 = Math::pow2(ry);
    if (radiiCheck > NumT(10.0)) radiiOk = false;
  }

  // Calculate (cx1, cy1).
  NumT sign = (largeArcFlag == sweepFlag) ? -NumT(1.0) : NumT(1.0);
  NumT sq   = (rx_2 * ry_2  - rx_2 * p1y_2 - ry_2 * p1x_2) /
                (rx_2 * p1y_2 + ry_2 * p1x_2);
  NumT coef = sign * (sq <= NumT(0.0) ? NumT(0.0) : Math::sqrt(sq));

  NumT_(Point) cp(coef *  ((rx * p1.y) / ry), coef * -((ry * p1.x) / rx));

  // Calculate (cx, cy) from (cx1, cy1).
  NumT sx2 = (p0.x + p2.x) * NumT(0.5);
  NumT sy2 = (p0.y + p2.y) * NumT(0.5);
  NumT cx = sx2 + (ac * cp.x - as * cp.y);
  NumT cy = sy2 + (as * cp.x + ac * cp.y);

  // Calculate the start_angle (angle1) and the sweep_angle (dangle).
  NumT ux = ( p1.x - cp.x) / rx;
  NumT uy = ( p1.y - cp.y) / ry;
  NumT vx = (-p1.x - cp.x) / rx;
  NumT vy = (-p1.y - cp.y) / ry;
  NumT p, n;

  // Calculate the angle start.
  n = Math::sqrt(ux * ux + uy * uy);
  p = ux; // (1 * ux) + (0 * uy)
  sign = (uy < NumT(0.0)) ? -NumT(1.0) : NumT(1.0);

  NumT v = p / n;
  if (v < -NumT(1.0)) v = -NumT(1.0);
  if (v >  NumT(1.0)) v =  NumT(1.0);
  NumT startAngle = sign * Math::acos(v);

  // Calculate the sweep angle.
  n = Math::sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
  p = ux * vx + uy * vy;
  sign = (ux * vy - uy * vx < 0) ? -NumT(1.0) : NumT(1.0);
  v = p / n;
  if (v < -NumT(1.0)) v = -NumT(1.0);
  if (v >  NumT(1.0)) v =  NumT(1.0);
  NumT sweepAngle = sign * Math::acos(v);

  if (!sweepFlag && sweepAngle > 0)
    sweepAngle -= (NumT)(MATH_TWO_PI);
  else if (sweepFlag && sweepAngle < 0)
    sweepAngle += (NumT)(MATH_TWO_PI);

  FOG_RETURN_ON_ERROR(
    self.arcTo(NumT_(Point)(NumT(0.0), NumT(0.0)), NumT_(Point)(rx, ry), startAngle, sweepAngle, false)
  );

  // If no error was reported then _arcTo had to add at least two vertices, for
  // matrix transform and fixing the end point we need at least one.
  FOG_ASSERT(self._d->length > 0);

  // We can now transform the resulting arc.
  {
    NumT_(Transform) transform = NumI_(Transform)::fromRotation(angle);
    transform.translate(NumT_(Point)(cx, cy), MATRIX_ORDER_APPEND);

    NumT_(Point)* pts = self._d->vertices + mark;
    transform.mapPoints(pts, pts, self._d->length - mark);
  }

  // We must make sure that the starting and ending points exactly coincide
  // with the initial p0 and p2.
  {
    NumT_(Point)* vertex = self._d->vertices;
    vertex[mark].set(p0.x, p0.y);
    vertex[self._d->length - 1].set(p2.x, p2.y);
  }

  // Bounding box is no longer valid.
  self._d->flags |= PATH_DATA_DIRTY_BBOX;
  return ERR_OK;
}

template<typename NumT>
static err_t _PathT_svgArcToRel(
  NumT_(Path)& self,
  const NumT_(Point)& rp,
  NumT angle, bool largeArcFlag, bool sweepFlag,
  const NumT_(Point)& pt)
{
  NumT_(Point) last;
  if (!_PathT_getLastPoint<NumT>(self._d, last)) return ERR_PATH_NO_RELATIVE;

  return self.svgArcTo(rp, angle, largeArcFlag, sweepFlag, pt + last);
}

// ============================================================================
// [Fog::Path - Close]
// ============================================================================

template<typename NumT>
static err_t _PathT_close(NumT_(Path)& self)
{
  PATH_ADD_VERTEX_BEGIN(NumT, 1)
    commands[0] = PATH_CMD_CLOSE;
    vertices[0].setNaN();
  PATH_ADD_VERTEX_END()
}

// ============================================================================
// [Fog::Path - Rect / Rects]
// ============================================================================

template<typename NumT, typename SrcT>
static err_t _PathT_rectT(
  NumT_(Path)& self,
  const SrcT_(Rect)& r, uint32_t direction)
{
  if (!r.isValid()) return ERR_OK;

  size_t pos = self._add(5);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  NumT_(Point)* vertices = self._d->vertices + pos;

  NumT x0 = (NumT)r.getX0();
  NumT y0 = (NumT)r.getY0();
  NumT x1 = (NumT)r.getX1();
  NumT y1 = (NumT)r.getY1();

  commands[0] = PATH_CMD_MOVE_TO;
  commands[1] = PATH_CMD_LINE_TO;
  commands[2] = PATH_CMD_LINE_TO;
  commands[3] = PATH_CMD_LINE_TO;
  commands[4] = PATH_CMD_CLOSE;

  vertices[0].set(x0, y0);
  vertices[2].set(x1, y1);
  vertices[4].setNaN();

  if (direction == PATH_DIRECTION_CW)
  {
    vertices[1].set(x1, y0);
    vertices[3].set(x0, y1);
  }
  else
  {
    vertices[1].set(x0, y1);
    vertices[3].set(x1, y0);
  }

  if (self._d->hasBoundingBox())
  {
    if (x0 < self._d->boundingBox.x0) self._d->boundingBox.x0 = x0;
    if (x1 > self._d->boundingBox.x1) self._d->boundingBox.x1 = x1;

    if (y0 < self._d->boundingBox.y0) self._d->boundingBox.y0 = y0;
    if (y1 > self._d->boundingBox.y1) self._d->boundingBox.y1 = y1;
  }

  return ERR_OK;
}

template<typename NumT, typename SrcT>
static err_t _PathT_boxT(
  NumT_(Path)& self,
  const SrcT_(Box)& r, uint32_t direction)
{
  if (!r.isValid()) return ERR_OK;

  size_t pos = self._add(5);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  NumT_(Point)* vertices = self._d->vertices + pos;

  NumT x0 = (NumT)r.getX0();
  NumT y0 = (NumT)r.getY0();
  NumT x1 = (NumT)r.getX1();
  NumT y1 = (NumT)r.getY1();

  commands[0] = PATH_CMD_MOVE_TO;
  commands[1] = PATH_CMD_LINE_TO;
  commands[2] = PATH_CMD_LINE_TO;
  commands[3] = PATH_CMD_LINE_TO;
  commands[4] = PATH_CMD_CLOSE;

  vertices[0].set(x0, y0);
  vertices[2].set(x1, y1);
  vertices[4].setNaN();

  if (direction == PATH_DIRECTION_CW)
  {
    vertices[1].set(x1, y0);
    vertices[3].set(x0, y1);
  }
  else
  {
    vertices[1].set(x0, y1);
    vertices[3].set(x1, y0);
  }

  if (self._d->hasBoundingBox())
  {
    if (x0 < self._d->boundingBox.x0) self._d->boundingBox.x0 = x0;
    if (x1 > self._d->boundingBox.x1) self._d->boundingBox.x1 = x1;

    if (y0 < self._d->boundingBox.y0) self._d->boundingBox.y0 = y0;
    if (y1 > self._d->boundingBox.y1) self._d->boundingBox.y1 = y1;
  }

  return ERR_OK;
}

template<typename NumT, typename SrcT>
static err_t _PathT_boxesT(
  NumT_(Path)& self,
  const SrcT_(Box)* r, size_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(r);

  size_t pos = self._add(count * 5);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  size_t i;
  size_t added = 0;
  NumT_(Point)* vertices = self._d->vertices + pos;

  if (direction == PATH_DIRECTION_CW)
  {
    for (i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      NumT x0 = (NumT)r->getX0();
      NumT y0 = (NumT)r->getY0();
      NumT x1 = (NumT)r->getX1();
      NumT y1 = (NumT)r->getY1();

      vertices[0].set(x0, y0);
      vertices[1].set(x1, y0);
      vertices[2].set(x1, y1);
      vertices[3].set(x0, y1);
      vertices[4].setNaN();

      vertices += 5;
      added++;
    }
  }
  else
  {
    for (i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      NumT x0 = (NumT)r->getX0();
      NumT y0 = (NumT)r->getY0();
      NumT x1 = (NumT)r->getX1();
      NumT y1 = (NumT)r->getY1();

      vertices[0].set(x0, y0);
      vertices[1].set(x0, y1);
      vertices[2].set(x1, y1);
      vertices[3].set(x1, y0);
      vertices[4].setNaN();

      vertices += 5;
      added++;
    }
  }

  uint8_t* commands = self._d->commands + pos;
  for (i = 0; i < added; i++, commands += 5)
  {
    commands[0] = PATH_CMD_MOVE_TO;
    commands[1] = PATH_CMD_LINE_TO;
    commands[2] = PATH_CMD_LINE_TO;
    commands[3] = PATH_CMD_LINE_TO;
    commands[4] = PATH_CMD_CLOSE;
  }

  self._d->flags |= PATH_DATA_DIRTY_BBOX;
  self._d->length = (size_t)(commands - self._d->commands);

  return ERR_OK;
}

template<typename NumT, typename SrcT>
static err_t _PathT_rectsT(
  NumT_(Path)& self,
  const SrcT_(Rect)* r, size_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(r);

  size_t pos = self._add(count * 5);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  size_t i;
  size_t added = 0;
  NumT_(Point)* vertices = self._d->vertices + pos;

  if (direction == PATH_DIRECTION_CW)
  {
    for (i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      NumT x0 = (NumT)r->x;
      NumT y0 = (NumT)r->y;
      NumT x1 = x0 + (NumT)r->w;
      NumT y1 = y0 + (NumT)r->h;

      vertices[0].set(x0, y0);
      vertices[1].set(x1, y0);
      vertices[2].set(x1, y1);
      vertices[3].set(x0, y1);
      vertices[4].setNaN();

      vertices += 5;
      added++;
    }
  }
  else
  {
    for (i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      NumT x0 = (NumT)r->x;
      NumT y0 = (NumT)r->y;
      NumT x1 = x0 + (NumT)r->w;
      NumT y1 = y0 + (NumT)r->h;

      vertices[0].set(x0, y0);
      vertices[1].set(x0, y1);
      vertices[2].set(x1, y1);
      vertices[3].set(x1, y0);
      vertices[4].setNaN();

      vertices += 5;
      added++;
    }
  }

  uint8_t* commands = self._d->commands + pos;
  for (i = 0; i < added; i++, commands += 5)
  {
    commands[0] = PATH_CMD_MOVE_TO;
    commands[1] = PATH_CMD_LINE_TO;
    commands[2] = PATH_CMD_LINE_TO;
    commands[3] = PATH_CMD_LINE_TO;
    commands[4] = PATH_CMD_CLOSE;
  }

  self._d->flags |= PATH_DATA_DIRTY_BBOX;
  self._d->length = (size_t)(commands - self._d->commands);

  return ERR_OK;
}

// ============================================================================
// [Fog::Path - Region]
// ============================================================================

template<typename NumT>
static err_t _PathT_region(NumT_(Path)& self, const Region& r, uint32_t direction)
{
  return self.boxes(r.getData(), r.getLength(), direction);
}

// ============================================================================
// [Fog::Path - Polyline / Polygon]
// ============================================================================

template<typename NumT>
static err_t _PathT_polylineI(NumT_(Path)& self, const PointI* pts, size_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(pts);

  size_t i;
  size_t pos = self._add(count);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  NumT_(Point)* vertices = self._d->vertices + pos;

  commands[0] = PATH_CMD_MOVE_TO;
  for (i = 1; i < count; i++) commands[i] = PATH_CMD_LINE_TO;

  if (direction == PATH_DIRECTION_CW)
  {
    for (i = 0; i < count; i++) vertices[i] = pts[i];
  }
  else
  {
    pts += count - 1;
    for (i = 0; i < count; i++, pts--) vertices[i] = pts[0];
  }

  self._d->flags |= PATH_DATA_DIRTY_BBOX;
  return ERR_OK;
}

template<typename NumT>
static err_t _PathT_polylineT(NumT_(Path)& self, const NumT_(Point)* pts, size_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(pts);

  size_t i;
  size_t pos = self._add(count);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  NumT_(Point)* vertices = self._d->vertices + pos;

  commands[0] = PATH_CMD_MOVE_TO;
  for (i = 1; i < count; i++) commands[i] = PATH_CMD_LINE_TO;

  if (direction == PATH_DIRECTION_CW)
  {
    for (i = 0; i < count; i++) vertices[i] = pts[i];
  }
  else
  {
    pts += count - 1;
    for (i = 0; i < count; i++, pts--) vertices[i] = pts[0];
  }

  self._d->flags |= PATH_DATA_DIRTY_BBOX;
  return ERR_OK;
}

template<typename NumT>
static err_t _PathT_polygonI(NumT_(Path)& self, const PointI* pts, size_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(pts);

  size_t i;
  size_t pos = self._add(count + 1);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  NumT_(Point)* vertices = self._d->vertices + pos;

  commands[0] = PATH_CMD_MOVE_TO;
  for (i = 1; i < count; i++) commands[i] = PATH_CMD_LINE_TO;
  commands[count] = PATH_CMD_CLOSE;

  vertices[0] = pts[0];
  if (direction == PATH_DIRECTION_CW)
  {
    for (i = 1; i < count; i++) vertices[i] = pts[i];
  }
  else
  {
    pts += count - 1;
    for (i = 1; i < count; i++, pts--) vertices[i] = pts[0];
  }
  vertices[count].setNaN();

  self._d->flags |= PATH_DATA_DIRTY_BBOX;
  return ERR_OK;
}

template<typename NumT>
static err_t _PathT_polygonT(NumT_(Path)& self, const NumT_(Point)* pts, size_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(pts);

  size_t i;
  size_t pos = self._add(count + 1);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  NumT_(Point)* vertices = self._d->vertices + pos;

  commands[0] = PATH_CMD_MOVE_TO;
  for (i = 1; i < count; i++) commands[i] = PATH_CMD_LINE_TO;
  commands[count] = PATH_CMD_CLOSE;

  vertices[0] = pts[0];
  if (direction == PATH_DIRECTION_CW)
  {
    for (i = 1; i < count; i++) vertices[i] = pts[i];
  }
  else
  {
    pts += count - 1;
    for (i = 1; i < count; i++, pts--) vertices[i] = pts[0];
  }
  vertices[count].setNaN();

  self._d->flags |= PATH_DATA_DIRTY_BBOX;
  return ERR_OK;
}

// ============================================================================
// [Fog::Path - Shape]
// ============================================================================

// ${SHAPE_TYPE:BEGIN}
static const uint8_t _PathT_shapeSize[] =
{
  /* 00: SHAPE_TYPE_NONE    */ 0,
  /* 01: SHAPE_TYPE_LINE    */ 2,
  /* 02: SHAPE_TYPE_QBEZIER */ 3,
  /* 03: SHAPE_TYPE_CBEZIER */ 4,
  /* 04: SHAPE_TYPE_ARC     */ 13,
  /* 05: SHAPE_TYPE_RECT    */ 5,
  /* 06: SHAPE_TYPE_ROUND   */ 18,
  /* 07: SHAPE_TYPE_CIRCLE  */ 14,
  /* 08: SHAPE_TYPE_ELLIPSE */ 14,
  /* 09: SHAPE_TYPE_CHORD   */ 20,
  /* 10: SHAPE_TYPE_PIE     */ 20,
  /* 11: SHAPE_TYPE_TRIANGLE*/ 4
};
// ${SHAPE_TYPE:END}

template<typename NumT>
static err_t _PathT_shape(
  NumT_(Path)& self,
  uint32_t shapeType, const void* shapeData, uint32_t direction,
  const NumT_(Transform)* tr)
{
  if (FOG_UNLIKELY(shapeType >= SHAPE_TYPE_COUNT))
    return ERR_RT_INVALID_ARGUMENT;

  uint32_t transformType = tr ? tr->getType() : TRANSFORM_TYPE_IDENTITY;
  if (transformType >= TRANSFORM_TYPE_PROJECTION)
  {
    if (transformType == TRANSFORM_TYPE_DEGENERATE)
      return ERR_GEOMETRY_DEGENERATE;

    NumT_T1(PathTmp, 32) tmp;
    FOG_RETURN_ON_ERROR(tmp._shape(shapeType, shapeData, direction));
    return self.appendTransformed(tmp, *tr);
  }

  size_t pos = self._add(_PathT_shapeSize[shapeType]);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* dstCmd = self._d->commands + pos;
  NumT_(Point)* dstPts = self._d->vertices + pos;

  err_t err = ERR_OK;

  bool combBBox = transformType <= TRANSFORM_TYPE_SWAP && (self._d->flags & PATH_DATA_DIRTY_BBOX) == 0;
  NumT_(Box) shapeBBox(UNINITIALIZED);

  switch (shapeType)
  {
    case SHAPE_TYPE_NONE:
    {
      return ERR_GEOMETRY_NONE;
    }

    // --------------------------------------------------------------------------
    // [Unclosed]
    // --------------------------------------------------------------------------

    case SHAPE_TYPE_LINE:
    {
      const NumT_(Line)* shape = reinterpret_cast<const NumT_(Line)*>(shapeData);

      dstCmd[0] = PATH_CMD_MOVE_TO;
      dstCmd[1] = PATH_CMD_LINE_TO;

      if (direction == PATH_DIRECTION_CW)
      {
        dstPts[0] = shape->p[0];
        dstPts[1] = shape->p[1];
      }
      else
      {
        dstPts[0] = shape->p[1];
        dstPts[1] = shape->p[0];
      }

      if (combBBox)
      {
        shapeBBox.x0 = shape->p[0].x;
        shapeBBox.y0 = shape->p[0].y;
        shapeBBox.x1 = shape->p[1].x;
        shapeBBox.y1 = shape->p[1].y;

        if (shapeBBox.x0 > shapeBBox.x1) swap(shapeBBox.x0, shapeBBox.x1);
        if (shapeBBox.y0 > shapeBBox.y1) swap(shapeBBox.y0, shapeBBox.y1);
      }
      break;
    }

    case SHAPE_TYPE_QBEZIER:
    {
      const NumT_(QBezier)* shape = reinterpret_cast<const NumT_(QBezier)*>(shapeData);

      dstCmd[0] = PATH_CMD_MOVE_TO;
      dstCmd[1] = PATH_CMD_QUAD_TO;
      dstCmd[2] = PATH_CMD_QUAD_TO;

      if (direction == PATH_DIRECTION_CW)
      {
        dstPts[0] = shape->p[0];
        dstPts[1] = shape->p[1];
        dstPts[2] = shape->p[2];
      }
      else
      {
        dstPts[0] = shape->p[2];
        dstPts[1] = shape->p[1];
        dstPts[2] = shape->p[0];
      }

      self._d->flags |= PATH_DATA_HAS_QBEZIER;
      combBBox = false;
      break;
    }

    case SHAPE_TYPE_CBEZIER:
    {
      const NumT_(CBezier)* shape = reinterpret_cast<const NumT_(CBezier)*>(shapeData);

      dstCmd[0] = PATH_CMD_MOVE_TO;
      dstCmd[1] = PATH_CMD_CUBIC_TO;
      dstCmd[2] = PATH_CMD_CUBIC_TO;
      dstCmd[3] = PATH_CMD_CUBIC_TO;

      if (direction == PATH_DIRECTION_CW)
      {
        dstPts[0] = shape->p[0];
        dstPts[1] = shape->p[1];
        dstPts[2] = shape->p[2];
        dstPts[3] = shape->p[3];
      }
      else
      {
        dstPts[0] = shape->p[3];
        dstPts[1] = shape->p[2];
        dstPts[2] = shape->p[1];
        dstPts[3] = shape->p[0];
      }

      self._d->flags |= PATH_DATA_DIRTY_BBOX | PATH_DATA_HAS_CBEZIER;

      combBBox = false;
      break;
    }

    case SHAPE_TYPE_ARC:
    {
      const NumT_(Arc)* shape = reinterpret_cast<const NumT_(Arc)*>(shapeData);

      NumT_(Arc) arc(UNINITIALIZED);
      if (direction != PATH_DIRECTION_CW)
      {
        arc.center = shape->center;
        arc.radius = shape->radius;
        arc.start  = shape->start;
        arc.sweep  =-shape->sweep;
        shape = &arc;
      }

      uint len = shape->toCSpline(dstPts);
      dstCmd[0] = PATH_CMD_MOVE_TO;
      for (uint i = 1; i < len; i++) dstCmd[i] = PATH_CMD_CUBIC_TO;

      self._d->flags |= PATH_DATA_HAS_CBEZIER;
      self._d->length = pos + len;
      combBBox = false;
      break;
    }

    // --------------------------------------------------------------------------
    // [Closed]
    // --------------------------------------------------------------------------

    case SHAPE_TYPE_RECT:
_ShapeRect:
    {
      const NumT_(Rect)* shape = reinterpret_cast<const NumT_(Rect)*>(shapeData);
      if (FOG_UNLIKELY(!shape->isValid()))
      {
        err = ERR_GEOMETRY_INVALID;
        goto _Fail;
      }

      NumT x0 = shape->x;
      NumT y0 = shape->y;
      NumT x1 = x0 + shape->w;
      NumT y1 = y0 + shape->h;

      dstCmd[0] = PATH_CMD_MOVE_TO;
      dstCmd[1] = PATH_CMD_LINE_TO;
      dstCmd[2] = PATH_CMD_LINE_TO;
      dstCmd[3] = PATH_CMD_LINE_TO;
      dstCmd[4] = PATH_CMD_CLOSE;

      dstPts[0].set(x0, y0);
      dstPts[2].set(x1, y1);
      dstPts[4].setNaN();

      if (direction == PATH_DIRECTION_CW)
      {
        dstPts[1].set(x1, y0);
        dstPts[3].set(x0, y1);
      }
      else
      {
        dstPts[1].set(x0, y1);
        dstPts[3].set(x1, y0);
      }

      if (combBBox)
      {
        shapeBBox.x0 = x0;
        shapeBBox.y0 = y0;
        shapeBBox.x1 = x1;
        shapeBBox.y1 = y1;
      }
      break;
    }

    case SHAPE_TYPE_ROUND:
    {
      const NumT_(Round)* shape = reinterpret_cast<const NumT_(Round)*>(shapeData);
      const NumT_(Rect)& r = shape->rect;

      if (FOG_UNLIKELY(!r.isValid()))
      {
        err = ERR_GEOMETRY_INVALID;
        goto _Fail;
      }

      NumT rx = Math::abs(shape->radius.x);
      NumT ry = Math::abs(shape->radius.y);

      NumT rxKappaInv = rx * NumT(MATH_1_MINUS_KAPPA);
      NumT ryKappaInv = ry * NumT(MATH_1_MINUS_KAPPA);

      NumT rw2 = r.w * NumT(0.5);
      NumT rh2 = r.h * NumT(0.5);

      if (rx > rw2) rx = rw2;
      if (ry > rh2) ry = rh2;
      if (Math::isFuzzyZero(rx) || Math::isFuzzyZero(ry)) goto _ShapeRect;

      NumT x0 = r.x;
      NumT y0 = r.y;
      NumT x1 = r.x + r.w;
      NumT y1 = r.y + r.h;

      if (direction == PATH_DIRECTION_CW)
      {
        dstCmd[ 0] = PATH_CMD_MOVE_TO;

        dstCmd[ 1] = PATH_CMD_LINE_TO;
        dstCmd[ 2] = PATH_CMD_CUBIC_TO;
        dstCmd[ 3] = PATH_CMD_CUBIC_TO;
        dstCmd[ 4] = PATH_CMD_CUBIC_TO;

        dstCmd[ 5] = PATH_CMD_LINE_TO;
        dstCmd[ 6] = PATH_CMD_CUBIC_TO;
        dstCmd[ 7] = PATH_CMD_CUBIC_TO;
        dstCmd[ 8] = PATH_CMD_CUBIC_TO;

        dstCmd[ 9] = PATH_CMD_LINE_TO;
        dstCmd[10] = PATH_CMD_CUBIC_TO;
        dstCmd[11] = PATH_CMD_CUBIC_TO;
        dstCmd[12] = PATH_CMD_CUBIC_TO;

        dstCmd[13] = PATH_CMD_LINE_TO;
        dstCmd[14] = PATH_CMD_CUBIC_TO;
        dstCmd[15] = PATH_CMD_CUBIC_TO;
        dstCmd[16] = PATH_CMD_CUBIC_TO;

        dstCmd[17] = PATH_CMD_CLOSE;

        dstPts[ 0].set(x0 + rx        , y0             );

        dstPts[ 1].set(x1 - rx        , y0             );
        dstPts[ 2].set(x1 - rxKappaInv, y0             );
        dstPts[ 3].set(x1             , y0 + ryKappaInv);
        dstPts[ 4].set(x1             , y0 + ry        );

        dstPts[ 5].set(x1             , y1 - ry        );
        dstPts[ 6].set(x1             , y1 - ryKappaInv);
        dstPts[ 7].set(x1 - rxKappaInv, y1             );
        dstPts[ 8].set(x1 - rx        , y1             );

        dstPts[ 9].set(x0 + rx        , y1             );
        dstPts[10].set(x0 + rxKappaInv, y1             );
        dstPts[11].set(x0             , y1 - ryKappaInv);
        dstPts[12].set(x0             , y1 - ry        );

        dstPts[13].set(x0             , y0 + ry        );
        dstPts[14].set(x0             , y0 + ryKappaInv);
        dstPts[15].set(x0 + rxKappaInv, y0             );
        dstPts[16].set(x0 + rx        , y0);

        dstPts[17].setNaN();
      }
      else
      {
        dstCmd[ 0] = PATH_CMD_MOVE_TO;
        dstCmd[ 1] = PATH_CMD_CUBIC_TO;
        dstCmd[ 2] = PATH_CMD_CUBIC_TO;
        dstCmd[ 3] = PATH_CMD_CUBIC_TO;

        dstCmd[ 4] = PATH_CMD_LINE_TO;
        dstCmd[ 5] = PATH_CMD_CUBIC_TO;
        dstCmd[ 6] = PATH_CMD_CUBIC_TO;
        dstCmd[ 7] = PATH_CMD_CUBIC_TO;

        dstCmd[ 8] = PATH_CMD_LINE_TO;
        dstCmd[ 9] = PATH_CMD_CUBIC_TO;
        dstCmd[10] = PATH_CMD_CUBIC_TO;
        dstCmd[11] = PATH_CMD_CUBIC_TO;

        dstCmd[12] = PATH_CMD_LINE_TO;
        dstCmd[13] = PATH_CMD_CUBIC_TO;
        dstCmd[14] = PATH_CMD_CUBIC_TO;
        dstCmd[15] = PATH_CMD_CUBIC_TO;

        dstCmd[16] = PATH_CMD_CLOSE;

        dstPts[ 0].set(x0 + rx        , y0             );
        dstPts[ 1].set(x0 + rxKappaInv, y0             );
        dstPts[ 2].set(x0             , y0 + ryKappaInv);
        dstPts[ 3].set(x0             , y0 + ry        );

        dstPts[ 4].set(x0             , y1 - ry        );
        dstPts[ 5].set(x0             , y1 - ryKappaInv);
        dstPts[ 6].set(x0 + rxKappaInv, y1             );
        dstPts[ 7].set(x0 + rx        , y1             );

        dstPts[ 8].set(x1 - rx        , y1             );
        dstPts[ 9].set(x1 - rxKappaInv, y1             );
        dstPts[10].set(x1             , y1 - ryKappaInv);
        dstPts[11].set(x1             , y1 - ry        );

        dstPts[12].set(x1             , y0 + ry        );
        dstPts[13].set(x1             , y0 + ryKappaInv);
        dstPts[14].set(x1 - rxKappaInv, y0             );
        dstPts[15].set(x1 - rx        , y0             );

        dstPts[16].setNaN();
        self._d->length--;
      }

      if (combBBox)
      {
        shapeBBox.x0 = x0;
        shapeBBox.y0 = y0;
        shapeBBox.x1 = x1;
        shapeBBox.y1 = y1;
      }

      self._d->flags |= PATH_DATA_HAS_CBEZIER;
      break;
    }

    case SHAPE_TYPE_CIRCLE:
    case SHAPE_TYPE_ELLIPSE:
    {
      NumT rx, rxKappa;
      NumT ry, ryKappa;
      NumT_(Point) c;

      if (shapeType == SHAPE_TYPE_CIRCLE)
      {
        const CircleF* shape = reinterpret_cast<const CircleF*>(shapeData);
        c = shape->center;
        rx = shape->radius;
        ry = Math::abs(rx);
      }
      else
      {
        const EllipseF* shape = reinterpret_cast<const EllipseF*>(shapeData);
        c = shape->center;
        rx = shape->radius.x;
        ry = shape->radius.y;
      }

      if (direction == PATH_DIRECTION_CCW) ry = -ry;

      rxKappa = rx * NumT(MATH_KAPPA);
      ryKappa = ry * NumT(MATH_KAPPA);

      dstCmd[ 0] = PATH_CMD_MOVE_TO;
      dstCmd[ 1] = PATH_CMD_CUBIC_TO;
      dstCmd[ 2] = PATH_CMD_CUBIC_TO;
      dstCmd[ 3] = PATH_CMD_CUBIC_TO;
      dstCmd[ 4] = PATH_CMD_CUBIC_TO;
      dstCmd[ 5] = PATH_CMD_CUBIC_TO;
      dstCmd[ 6] = PATH_CMD_CUBIC_TO;
      dstCmd[ 7] = PATH_CMD_CUBIC_TO;
      dstCmd[ 8] = PATH_CMD_CUBIC_TO;
      dstCmd[ 9] = PATH_CMD_CUBIC_TO;
      dstCmd[10] = PATH_CMD_CUBIC_TO;
      dstCmd[11] = PATH_CMD_CUBIC_TO;
      dstCmd[12] = PATH_CMD_CUBIC_TO;
      dstCmd[13] = PATH_CMD_CLOSE;

      dstPts[ 0].set(c.x + rx     , c.y          );
      dstPts[ 1].set(c.x + rx     , c.y + ryKappa);
      dstPts[ 2].set(c.x + rxKappa, c.y + ry     );
      dstPts[ 3].set(c.x          , c.y + ry     );
      dstPts[ 4].set(c.x - rxKappa, c.y + ry     );
      dstPts[ 5].set(c.x - rx     , c.y + ryKappa);
      dstPts[ 6].set(c.x - rx     , c.y          );
      dstPts[ 7].set(c.x - rx     , c.y - ryKappa);
      dstPts[ 8].set(c.x - rxKappa, c.y - ry     );
      dstPts[ 9].set(c.x          , c.y - ry     );
      dstPts[10].set(c.x + rxKappa, c.y - ry     );
      dstPts[11].set(c.x + rx     , c.y - ryKappa);
      dstPts[12].set(c.x + rx     , c.y          );
      dstPts[13].setNaN();

      if (combBBox)
      {
        rx = Math::abs(rx);
        ry = Math::abs(ry);

        shapeBBox.x0 = c.x - rx;
        shapeBBox.y0 = c.y - ry;
        shapeBBox.x1 = c.x + rx;
        shapeBBox.y1 = c.y + ry;
      }

      self._d->flags |= PATH_DATA_HAS_CBEZIER;
      break;
    }

    case SHAPE_TYPE_CHORD:
    case SHAPE_TYPE_PIE:
    {
      const NumT_(Arc)* shape = reinterpret_cast<const NumT_(Arc)*>(shapeData);

      dstCmd[0] = PATH_CMD_MOVE_TO;
      dstCmd[1] = PATH_CMD_LINE_TO;
      dstPts[0].set(shape->center);

      if (shapeType == SHAPE_TYPE_PIE)
      {
        dstCmd++;
        dstPts++;
      }

      NumT_(Arc) arc(UNINITIALIZED);
      if (direction != PATH_DIRECTION_CW)
      {
        arc.center = shape->center;
        arc.radius = shape->radius;
        arc.start  = shape->start;
        arc.sweep  =-shape->sweep;
        shape = &arc;
      }

      uint len = shape->toCSpline(dstPts);
      for (uint i = 1; i < len; i++) dstCmd[i] = PATH_CMD_CUBIC_TO;

      dstCmd[len] = PATH_CMD_CLOSE;
      dstPts[len].setNaN();

      dstCmd += len + 1;
      dstPts += len + 1;

      self._d->flags |= PATH_DATA_HAS_CBEZIER;
      self._d->length = (size_t)(dstCmd - self._d->commands);
      combBBox = false;
      break;
    }

    case SHAPE_TYPE_TRIANGLE:
    {
      const NumT_(Triangle)* shape = reinterpret_cast<const NumT_(Triangle)*>(shapeData);

      dstCmd[0] = PATH_CMD_MOVE_TO;
      dstCmd[1] = PATH_CMD_LINE_TO;
      dstCmd[2] = PATH_CMD_LINE_TO;
      dstCmd[3] = PATH_CMD_CLOSE;

      dstPts[0].set(shape->p[0]);
      dstPts[1].set(shape->p[1]);
      dstPts[2].set(shape->p[2]);
      dstPts[3].setNaN();

      combBBox = false;
      break;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  if (tr)
  {
    tr->_mapPoints(dstPts, dstPts, self._d->length - pos);
    if (combBBox) tr->mapBox(shapeBBox, shapeBBox);
  }

  if (combBBox)
  {
    if (self._d->flags & PATH_DATA_HAS_BBOX)
      NumI_(Box)::bound(self._d->boundingBox, self._d->boundingBox, shapeBBox);
    else
      self._d->boundingBox = shapeBBox;
  }
  else
  {
    self._d->flags |= PATH_DATA_DIRTY_BBOX;
  }

  _FOG_PATH_VERIFY_BOUNDING_BOX(self);
  return ERR_OK;

_Fail:
  // Restore the path state on error.
  if (self._d->length != pos) self._d->length = pos;
  return err;
}

// ============================================================================
// [Fog::Path - Append]
// ============================================================================

template<typename NumT, typename SrcT>
static err_t _PathT_appendPathT(
  NumT_(Path)& self,
  const SrcT_(Path)& path,
  const Range* range)
{
  size_t srcPos = 0;
  size_t srcLen = path.getLength();

  if (range != NULL)
  {
    srcPos = range->getStart();
    srcLen = Math::min(range->getEnd(), srcLen);
    if (srcPos > srcLen) return ERR_RT_INVALID_ARGUMENT;
    srcLen -= srcPos;
  }
  if (srcLen == 0) return ERR_OK;

  size_t pos = self._add(srcLen);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  Memory::copy(self._d->commands + pos, path._d->commands + srcPos, srcLen);
  Math::vConvertFloat<NumT, SrcT>(
    reinterpret_cast<NumT*>(self._d->vertices + pos),
    reinterpret_cast<const SrcT*>(path._d->vertices + srcPos), srcLen * 2);

  if (range == NULL &&
      ((self._d->flags | path._d->flags) & PATH_DATA_DIRTY_BBOX) == 0)
  {
    NumT x, y;

    x = (NumT)path._d->boundingBox.x0;
    y = (NumT)path._d->boundingBox.y0;

    if (x < self._d->boundingBox.x0) self._d->boundingBox.x0 = x;
    if (y < self._d->boundingBox.y0) self._d->boundingBox.y0 = y;

    x = (NumT)path._d->boundingBox.x1;
    y = (NumT)path._d->boundingBox.y1;

    if (x > self._d->boundingBox.x1) self._d->boundingBox.x1 = x;
    if (y > self._d->boundingBox.y1) self._d->boundingBox.y1 = y;

    self._d->flags |= (path._d->flags & (PATH_DATA_DIRTY_CMD | PATH_DATA_HAS_QBEZIER | PATH_DATA_HAS_CBEZIER));
  }
  else
  {
    self._d->flags |= PATH_DATA_DIRTY_BBOX |
                      (path._d->flags & (PATH_DATA_DIRTY_CMD | PATH_DATA_HAS_QBEZIER | PATH_DATA_HAS_CBEZIER));
  }

  return ERR_OK;
}

template<typename NumT, typename SrcT>
static err_t _PathT_appendTransformedPathT(
  NumT_(Path)& self,
  const SrcT_(Path)& path,
  const NumT_(Transform)& tr,
  const Range* range)
{
  size_t srcPos = 0;
  size_t srcLen = path.getLength();

  if (range != NULL)
  {
    srcPos = range->getStart();
    srcLen = Math::min(range->getEnd(), srcLen);
    if (srcPos > srcLen) return ERR_RT_INVALID_ARGUMENT;
    srcLen -= srcPos;
  }

  if (srcLen == 0) return ERR_OK;

  uint32_t transformType = tr.getType();
  if (transformType < TRANSFORM_TYPE_PROJECTION)
  {
    size_t pos = self._add(srcLen);
    if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

    Memory::copy(self._d->commands + pos, path._d->commands + srcPos, srcLen);
    tr._mapPoints(self._d->vertices + pos, path._d->vertices + srcPos, srcLen);

    if (srcPos == 0 && srcLen == path._d->length && transformType <= TRANSFORM_TYPE_SWAP &&
        ((self._d->flags | path._d->flags) & PATH_DATA_DIRTY_BBOX) == 0)
    {
      NumT_(Box) box(path._d->boundingBox);
      tr.mapBox(box, box);

      if (box.x0 < self._d->boundingBox.x0) self._d->boundingBox.x0 = box.x0;
      if (box.y0 < self._d->boundingBox.y0) self._d->boundingBox.y0 = box.y0;

      if (box.x1 > self._d->boundingBox.x1) self._d->boundingBox.x1 = box.x1;
      if (box.y1 > self._d->boundingBox.y1) self._d->boundingBox.y1 = box.y1;

      self._d->flags |= (path._d->flags & (PATH_DATA_DIRTY_CMD | PATH_DATA_HAS_QBEZIER | PATH_DATA_HAS_CBEZIER));
    }
    else
    {
      self._d->flags |= PATH_DATA_DIRTY_BBOX |
                        (path._d->flags & (PATH_DATA_DIRTY_CMD | PATH_DATA_HAS_QBEZIER | PATH_DATA_HAS_CBEZIER));
    }
    return ERR_OK;
  }
  else
  {
    if (transformType == TRANSFORM_TYPE_DEGENERATE)
      return ERR_GEOMETRY_DEGENERATE;

    if (sizeof(NumT) == sizeof(SrcT) && (void*)&self == (void*)&path)
    {
      SrcT_(Path) tmp(path);
      return tr.mapPathData(self, tmp.getCommands() + srcPos, tmp.getVertices() + srcPos, srcLen, CONTAINER_OP_APPEND);
    }
    else
    {
      return tr.mapPathData(self, path.getCommands() + srcPos, path.getVertices() + srcPos, srcLen, CONTAINER_OP_APPEND);
    }
  }
}

template<typename NumT, typename SrcT>
static err_t _PathT_appendTranslatedPathT(
  NumT_(Path)& self,
  const SrcT_(Path)& path,
  const NumT_(Point)& pt,
  const Range* range)
{
  // Build transform (fast).
  NumT_(Transform) tr(UNINITIALIZED);
  tr._type = TRANSFORM_TYPE_TRANSLATION;
  tr._20 = pt.x;
  tr._21 = pt.y;

  return _PathT_appendTransformedPathT<NumT, SrcT>(self, path, tr, range);
}

// ============================================================================
// [Fog::Path - Flat]
// ============================================================================

template<typename NumT>
static void _PathT_updateFlat(const NumT_(Path)& self)
{
  size_t i = 0;
  size_t len = self._d->length;

  const uint8_t* cmd = self._d->commands;
  uint32_t flags = NO_FLAGS;

  while (i < len)
  {
    uint8_t c = cmd[i];

    if (PathCmd::isQuadOrCubicTo(c))
    {
      if (c == PATH_CMD_QUAD_TO)
      {
        flags |= PATH_DATA_HAS_QBEZIER;
        i += 2;
      }
      else
      {
        flags |= PATH_DATA_HAS_CBEZIER;
        i += 3;
      }

      if (flags == (PATH_DATA_HAS_QBEZIER | PATH_DATA_HAS_CBEZIER)) break;
      continue;
    }

    i++;
  }

  self._d->flags = (self._d->flags & ~(PATH_DATA_DIRTY_CMD      |
                                       PATH_DATA_HAS_QBEZIER    |
                                       PATH_DATA_HAS_CBEZIER )) | flags;
}

// TODO: Path, not perspecive correct assumptions.
template<typename NumT>
static err_t _PathT_doFlatten(
  NumT_(Path)& dst,
  const uint8_t* srcCmd, const NumT_(Point)* srcPts, size_t srcLength,
  const NumT_(PathFlattenParams)& params)
{
  if (srcLength == 0) return ERR_OK;

  const NumT_(Transform)* tr = params.getTransform();
  NumT flatness = params.getFlatness();

  size_t i = 0;
  size_t dstInitial = dst.getLength();

  uint8_t c;
  err_t err = ERR_OK;

  // Minimal count of vertices added to the dst path is 'length'. We assume
  // that there are curves in the source path so length is multiplied by 4
  // and small constant is added to ensure that we don't reallocate if a given
  // path is small, but contains curves.
  size_t predict = dstInitial + (srcLength * 4U) + 128;

  // Overflow?
  if (predict < dstInitial) predict = dstInitial;
  FOG_RETURN_ON_ERROR(dst.reserve(predict));

  // Set destination path as flat (no quad/cubic curves).
  dst._d->flags &= ~(PATH_DATA_DIRTY_CMD | PATH_DATA_HAS_QBEZIER | PATH_DATA_HAS_CBEZIER);

  if (tr == NULL || tr->getType() == TRANSFORM_TYPE_IDENTITY)
  {
    for (;;)
    {
      // Collect 'move-to' and 'line-to' commands.
      while (i < srcLength)
      {
        c = srcCmd[i];
        if (!PathCmd::isMoveOrLineTo(c)) break;

        i++;
      }

      // Invalid state if 'i' is zero (no 'move-to' or 'line-to').
      if (i == 0) goto _InvalidState;

      // Include 'close' command if used.
      if (PathCmd::isClose(c)) i++;

      // Copy commands and vertices to the destination path.
      {
        size_t pos = dst._add(i);
        if (pos == INVALID_INDEX) goto _OutOfMemory;

        memcpy(dst._d->commands + pos, srcCmd, i);
        memcpy(dst._d->vertices + pos, srcPts, i * sizeof(NumT_(Point) ));
      }

      // Advance.
      FOG_ASSERT(i <= srcLength);
      if ((srcLength -= i) == 0) break;

      srcCmd += i;
      srcPts += i;
      i = 0;

      // Closed polygon (no curves?)
      if (PathCmd::isClose(c)) continue;

      // Approximate 'quad-to' or 'cubic-to' commands.
      do {
        c = srcCmd[i];
        if (PathCmd::isQuadTo(c))
        {
          if ((i += 2) > srcLength) goto _InvalidState;

          err = NumI_(QBezier)::flatten(srcPts + i - 3, dst, PATH_CMD_LINE_TO, flatness);
          if (FOG_IS_ERROR(err)) goto _Fail;
        }
        else if (PathCmd::isCubicTo(c))
        {
          if ((i += 3) > srcLength) goto _InvalidState;

          err = NumI_(CBezier)::flatten(srcPts + i - 4, dst, PATH_CMD_LINE_TO, flatness);
          if (FOG_IS_ERROR(err)) goto _Fail;
        }
        else
        {
          break;
        }
      } while (i < srcLength);

      if (PathCmd::isClose(c)) { dst.close(); i++; }

      // Advance.
      FOG_ASSERT(i <= srcLength);
      if ((srcLength -= i) == 0) break;

      srcCmd += i;
      srcPts += i;
      i = 0;
    }
  }
  else
  {
    NumT_(Point) pts[4];

    for (;;)
    {
      // Collect 'move-to' and 'line-to' commands.
      while (i < srcLength)
      {
        c = srcCmd[i];
        if (!PathCmd::isMoveOrLineTo(c)) break;

        i++;
      }

      // Invalid state if 'i' is zero (no 'move-to' or 'line-to').
      if (i == 0) goto _InvalidState;

      // Include 'close' command if used.
      if (PathCmd::isClose(c)) i++;

      // Copy commands and vertices to the destination path.
      {
        size_t pos = dst._add(i);
        if (pos == INVALID_INDEX) goto _OutOfMemory;

        memcpy(dst._d->commands + pos, srcCmd, i);
        tr->_mapPoints(dst._d->vertices + pos, srcPts, i);
      }

      // Advance.
      FOG_ASSERT(i <= srcLength);
      if ((srcLength -= i) == 0) break;

      srcCmd += i;
      srcPts += i;
      i = 0;

      // Closed polygon (no curves?)
      if (PathCmd::isClose(c)) continue;

      // Approximate 'quad-to' or 'cubic-to' commands.
      do {
        c = srcCmd[i];
        if (PathCmd::isQuadTo(c))
        {
          if ((i += 2) > srcLength) goto _InvalidState;

          tr->_mapPoints(pts, srcPts + i - 3, 3);
          err = NumI_(QBezier)::flatten(pts, dst, PATH_CMD_LINE_TO, flatness);
          if (FOG_IS_ERROR(err)) goto _Fail;
        }
        else if (PathCmd::isCubicTo(c))
        {
          if ((i += 3) > srcLength) goto _InvalidState;

          tr->_mapPoints(pts, srcPts + i - 4, 4);
          err = NumI_(CBezier)::flatten(pts,dst, PATH_CMD_LINE_TO, flatness);
          if (FOG_IS_ERROR(err)) goto _Fail;
        }
        else
        {
          break;
        }
      } while (i < srcLength);

      if (PathCmd::isClose(c)) { dst.close(); i++; }

      // Advance.
      FOG_ASSERT(i <= srcLength);
      if ((srcLength -= i) == 0) break;

      srcCmd += i;
      srcPts += i;
      i = 0;
    }
  }
  return ERR_OK;

_OutOfMemory:
  err = ERR_RT_OUT_OF_MEMORY;
  goto _Fail;

_InvalidState:
  err = ERR_GEOMETRY_INVALID;
  goto _Fail;

_Fail:
  if (dst._d->length != dstInitial) dst._d->length = dstInitial;
  return err;
}

template<typename NumT>
static err_t _PathT_flatten(
  NumT_(Path)& dst,
  const NumT_(Path)& src,
  const NumT_(PathFlattenParams)& params,
  const Range* range)
{
  if (range == NULL)
  {
    if (dst._d == src._d)
    {
      NumT_(Path) tmp;

      FOG_RETURN_ON_ERROR(
        _PathT_doFlatten<NumT>(tmp, src.getCommands(), src.getVertices(), src.getLength(), params)
      );
      return dst.setPath(tmp);
    }
    else
    {
      dst.clear();
      return _PathT_doFlatten<NumT>(dst, src.getCommands(), src.getVertices(), src.getLength(), params);
    }
  }
  else
  {
    size_t start = range->getStart();
    size_t end = Math::min(range->getEnd(), src.getLength());

    if (start >= end) return (start == 0) ? (err_t)ERR_OK : (err_t)ERR_RT_INVALID_ARGUMENT;
    size_t len = start - end;

    if (dst._d == src._d)
    {
      NumT_(Path) t;

      FOG_RETURN_ON_ERROR(
        _PathT_doFlatten<NumT>(t, src.getCommands() + start, src.getVertices() + start, len, params)
      );
      return dst.setPath(t);
    }
    else
    {
      dst.clear();

      return _PathT_doFlatten<NumT>(dst, src.getCommands() + start, src.getVertices() + start, len, params);
    }
  }
}

// ============================================================================
// [Fog::Path - GetBoundingBox]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL _PathT_getBoundingBox(const NumT_(Path)& self,
  NumT_(Box)* dst,
  const NumT_(Transform)* transform)
{
  uint32_t transformType = transform ? transform->getType() : TRANSFORM_TYPE_IDENTITY;
  bool hasBoundingBox = self._d->hasBoundingBox();

  size_t i = self._d->length;
  if (i == 0) goto _Empty;

  switch (transformType)
  {
    case TRANSFORM_TYPE_IDENTITY:
      if (hasBoundingBox)
      {
        dst->setBox(self._d->boundingBox);
        return ERR_OK;
      }
      goto _Update;

    case TRANSFORM_TYPE_TRANSLATION:
      if (hasBoundingBox)
      {
        dst->setBox(self._d->boundingBox);
        dst->translate(transform->_20, transform->_21);
        return ERR_OK;
      }
      goto _Update;

    case TRANSFORM_TYPE_SCALING:
      if (hasBoundingBox)
      {
        dst->x0 = self._d->boundingBox.x0 * transform->_00 + transform->_20;
        dst->y0 = self._d->boundingBox.y0 * transform->_11 + transform->_21;
        dst->x1 = self._d->boundingBox.x1 * transform->_00 + transform->_20;
        dst->y1 = self._d->boundingBox.y1 * transform->_11 + transform->_21;

        if (dst->x0 > dst->x1) swap(dst->x0, dst->x1);
        if (dst->y0 > dst->y1) swap(dst->y0, dst->y1);
        return ERR_OK;
      }
      goto _Update;

    case TRANSFORM_TYPE_SWAP:
      if (hasBoundingBox)
      {
        dst->x0 = self._d->boundingBox.y0 * transform->_10 + transform->_20;
        dst->y0 = self._d->boundingBox.x0 * transform->_01 + transform->_21;
        dst->x1 = self._d->boundingBox.y1 * transform->_10 + transform->_20;
        dst->y1 = self._d->boundingBox.x1 * transform->_01 + transform->_21;

        if (dst->x0 > dst->x1) swap(dst->x0, dst->x1);
        if (dst->y0 > dst->y1) swap(dst->y0, dst->y1);
        return ERR_OK;
      }
      goto _Update;

    case TRANSFORM_TYPE_ROTATION:
    case TRANSFORM_TYPE_AFFINE:
      goto _Affine;

    case TRANSFORM_TYPE_PROJECTION:
      goto _Projection;

    case TRANSFORM_TYPE_DEGENERATE:
      dst->reset();
      return ERR_GEOMETRY_DEGENERATE;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

_Update:
  {
    const uint8_t* cmd = self._d->commands;
    const NumT_(Point)* pts = self._d->vertices;

    NumT_(Box) box(UNINITIALIZED);
    bool isFirst = true;

_Update_Repeat:
    // Find the 'move-to' command.
    do {
      uint c = cmd[0];

      if (c == PATH_CMD_MOVE_TO)
      {
        if (isFirst)
        {
          box.x0 = pts[0].x;
          box.y0 = pts[0].y;
          box.x1 = pts[0].x;
          box.y1 = pts[0].y;

          i--;
          cmd++;
          pts++;

          isFirst = false;
        }
        break;
      }
      else if (c == PATH_CMD_CLOSE)
      {
        i--;
        cmd++;
        pts++;
        continue;
      }
      else
      {
        goto _Invalid;
      }
    } while (i);

    // Iterate over the path / sub-paths.
    while (i)
    {
      uint c = cmd[0];

      switch (c)
      {
        case PATH_CMD_MOVE_TO:
        case PATH_CMD_LINE_TO:
          if (pts[0].x < box.x0) box.x0 = pts[0].x; else if (pts[0].x > box.x1) box.x1 = pts[0].x;
          if (pts[0].y < box.y0) box.y0 = pts[0].y; else if (pts[0].y > box.y1) box.y1 = pts[0].y;

          i--;
          cmd++;
          pts++;
          break;

        case PATH_CMD_QUAD_TO:
          FOG_ASSERT(i >= 2);

          // Merge end point - pts[1].
          if (pts[1].x < box.x0) box.x0 = pts[1].x; else if (pts[1].x > box.x1) box.x1 = pts[1].x;
          if (pts[1].y < box.y0) box.y0 = pts[1].y; else if (pts[1].y > box.y1) box.y1 = pts[1].y;

          // Do calculation only when necessary.
          if (!(pts[0].x > box.x0 && pts[0].y > box.y0 &&
                pts[0].x < box.x1 && pts[0].y < box.y1 ))
          {
            NumT_(Box) e;
            if (NumI_(QBezier)::getBoundingBox(pts - 1, &e) == ERR_OK)
            {
              if (e.x0 < box.x0) box.x0 = e.x0;
              if (e.y0 < box.y0) box.y0 = e.y0;

              if (e.x1 > box.x1) box.x1 = e.x1;
              if (e.y1 > box.y1) box.y1 = e.y1;
            }
          }

          i -= 2;
          cmd += 2;
          pts += 2;
          break;

        case PATH_CMD_CUBIC_TO:
          FOG_ASSERT(i >= 3);

          // Merge end point - pts[2].
          if (pts[2].x < box.x0) box.x0 = pts[2].x; else if (pts[2].x > box.x1) box.x1 = pts[2].x;
          if (pts[2].y < box.y0) box.y0 = pts[2].y; else if (pts[2].y > box.y1) box.y1 = pts[2].y;

          // Do calculation only when necessary.
          if (!(pts[0].x > box.x0 && pts[1].x > box.x0 &&
                pts[0].y > box.y0 && pts[1].y > box.y0 &&
                pts[0].x < box.x1 && pts[1].x < box.x1 &&
                pts[0].y < box.y1 && pts[1].y < box.y1 ))
          {
            NumT_(Box) e;
            if (NumI_(CBezier)::getBoundingBox(pts - 1, &e) == ERR_OK)
            {
              if (e.x0 < box.x0) box.x0 = e.x0;
              if (e.y0 < box.y0) box.y0 = e.y0;

              if (e.x1 > box.x1) box.x1 = e.x1;
              if (e.y1 > box.y1) box.y1 = e.y1;
            }
          }

          i -= 3;
          cmd += 3;
          pts += 3;
          break;

        case PATH_CMD_CLOSE:
          cmd++;
          pts++;
          if (--i) goto _Update_Repeat;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }

    if (isFirst) goto _Empty;

    self._d->flags |= PATH_DATA_HAS_BBOX;
    self._d->boundingBox = box;

    switch (transformType)
    {
      case TRANSFORM_TYPE_SWAP:
        box.setBox(box.y0 * transform->_10, box.x0 * transform->_01,
                   box.y1 * transform->_10, box.x1 * transform->_01);
        goto _Update_Scaling;

      case TRANSFORM_TYPE_SCALING:
        box.x0 *= transform->_00;
        box.y0 *= transform->_11;
        box.x1 *= transform->_00;
        box.y1 *= transform->_11;

_Update_Scaling:
        if (box.x0 > box.x1) swap(box.x0, box.x1);
        if (box.y0 > box.y1) swap(box.y0, box.y1);
        // ... Fall through ...

      case TRANSFORM_TYPE_TRANSLATION:
        box.x0 += transform->_20;
        box.y0 += transform->_21;
        box.x1 += transform->_20;
        box.y1 += transform->_21;
        // ... Fall through ...

      case TRANSFORM_TYPE_IDENTITY:
        *dst = box;
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }

    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Affine]
  // --------------------------------------------------------------------------

_Affine:
  {
    const uint8_t* cmd = self._d->commands;
    const NumT_(Point)* pts = self._d->vertices;

    NumT _00 = transform->_00;
    NumT _01 = transform->_01;
    NumT _10 = transform->_10;
    NumT _11 = transform->_11;

    // Temporary transformed points.
    NumT_(Point) pta[4];

    NumT_(Box) box(UNINITIALIZED);
    bool isFirst = true;

_Affine_Repeat:
    // Find the 'move-to' command.
    do {
      uint c = cmd[0];

      if (c == PATH_CMD_MOVE_TO)
      {
        if (isFirst)
        {
          pta[0].set(pts[0].x * _00 + pts[0].y * _10, pts[0].x * _01 + pts[0].y * _11);

          box.x0 = pta[0].x;
          box.y0 = pta[0].y;
          box.x1 = pta[0].x;
          box.y1 = pta[0].y;

          i--;
          cmd++;
          pts++;

          isFirst = false;
        }
        break;
      }
      else if (c == PATH_CMD_CLOSE)
      {
        i--;
        cmd++;
        pts++;
        continue;
      }
      else
      {
        goto _Invalid;
      }
    } while (i);

    // Iterate over the path / sub-paths.
    while (i)
    {
      uint c = cmd[0];

      switch (c)
      {
        case PATH_CMD_MOVE_TO:
        case PATH_CMD_LINE_TO:
          pta[0].set(pts[0].x * _00 + pts[0].y * _10, pts[0].x * _01 + pts[0].y * _11);

          if (pta[0].x < box.x0) box.x0 = pta[0].x; else if (pta[0].x > box.x1) box.x1 = pta[0].x;
          if (pta[0].y < box.y0) box.y0 = pta[0].y; else if (pta[0].y > box.y1) box.y1 = pta[0].y;

          i--;
          cmd++;
          pts++;
          break;

        case PATH_CMD_QUAD_TO:
          FOG_ASSERT(i >= 2);

          pta[1].set(pts[0].x * _00 + pts[0].y * _10, pts[0].x * _01 + pts[0].y * _11);
          pta[2].set(pts[1].x * _00 + pts[1].y * _10, pts[1].x * _01 + pts[1].y * _11);

          // Merge end point - pta[2].
          if (pta[2].x < box.x0) box.x0 = pta[2].x; else if (pta[2].x > box.x1) box.x1 = pta[2].x;
          if (pta[2].y < box.y0) box.y0 = pta[2].y; else if (pta[2].y > box.y1) box.y1 = pta[2].y;

          // Do calculation only when necessary.
          if (pta[1].x < box.x0 || pta[1].y < box.y0 || pta[1].x > box.x1 || pta[1].y > box.y1)
          {
            NumT_(Box) e;
            if (NumI_(QBezier)::getBoundingBox(pta, &e) == ERR_OK)
            {
              if (e.x0 < box.x0) box.x0 = e.x0;
              if (e.y0 < box.y0) box.y0 = e.y0;

              if (e.x1 > box.x1) box.x1 = e.x1;
              if (e.y1 > box.y1) box.y1 = e.y1;
            }
          }

          // May be reused.
          pta[0] = pta[2];

          i -= 2;
          cmd += 2;
          pts += 2;
          break;

        case PATH_CMD_CUBIC_TO:
          FOG_ASSERT(i >= 3);

          pta[1].set(pts[0].x * _00 + pts[0].y * _10, pts[0].x * _01 + pts[0].y * _11);
          pta[2].set(pts[1].x * _00 + pts[1].y * _10, pts[1].x * _01 + pts[1].y * _11);
          pta[3].set(pts[2].x * _00 + pts[2].y * _10, pts[2].x * _01 + pts[2].y * _11);

          // Merge end point - pta[3].
          if (pta[3].x < box.x0) box.x0 = pta[3].x; else if (pta[3].x > box.x1) box.x1 = pta[3].x;
          if (pta[3].y < box.y0) box.y0 = pta[3].y; else if (pta[3].y > box.y1) box.y1 = pta[3].y;

          // Do calculation only when necessary.
          if (pta[1].x < box.x0 || pta[1].y < box.y0 || pta[1].x > box.x1 || pta[1].y > box.y1 ||
              pta[2].x < box.x0 || pta[2].y < box.y0 || pta[2].x > box.x1 || pta[2].y > box.y1)
          {
            NumT_(Box) e;
            if (NumI_(CBezier)::getBoundingBox(pta, &e) == ERR_OK)
            {
              if (e.x0 < box.x0) box.x0 = e.x0;
              if (e.y0 < box.y0) box.y0 = e.y0;

              if (e.x1 > box.x1) box.x1 = e.x1;
              if (e.y1 > box.y1) box.y1 = e.y1;
            }
          }

          // May be reused.
          pta[0] = pta[3];

          i -= 3;
          cmd += 3;
          pts += 3;
          break;

        case PATH_CMD_CLOSE:
          cmd++;
          pts++;
          if (--i) goto _Affine_Repeat;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }

    if (isFirst) goto _Empty;

    box.x0 += transform->_20;
    box.y0 += transform->_21;
    box.x1 += transform->_20;
    box.y1 += transform->_21;

    *dst = box;
    return ERR_OK;
  }

_Projection:
  {
    NumT_T1(PathTmp, 196) tmp;
    FOG_RETURN_ON_ERROR(transform->mapPath(tmp, self));
    return tmp.getBoundingBox(*dst);
  }

_Empty:
  self._d->flags &= ~PATH_DATA_OWN_FLAGS;
  self._d->boundingBox.reset();
  return ERR_GEOMETRY_NONE;

_Invalid:
  return ERR_GEOMETRY_INVALID;
}

// ============================================================================
// [Fog::Path - HitTest]
// ============================================================================

template<typename NumT>
static bool _PathT_hitTest(
  const NumT_(Path)& self,
  const NumT_(Point)& pt, uint32_t fillRule)
{
  size_t i = self.getLength();
  if (i == 0) return false;

  const NumT_(Point)* pts = self.getVertices();
  const uint8_t* cmd = self.getCommands();

  int windingNumber = 0;

  NumT_(Point) start;
  bool hasMoveTo = false;

  NumT px = pt.x;
  NumT py = pt.y;

  NumT x0, y0;
  NumT x1, y1;

  do {
    switch (cmd[0])
    {
      // ----------------------------------------------------------------------
      // [Move-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_MOVE_TO:
      {
        if (hasMoveTo)
        {
          x0 = pts[-1].x;
          y0 = pts[-1].y;
          x1 = start.x;
          y1 = start.y;

          hasMoveTo = false;
          goto _DoLine;
        }

        start = pts[0];

        pts++;
        cmd++;
        i--;

        hasMoveTo = true;
        break;
      }

      // ----------------------------------------------------------------------
      // [Line-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_LINE_TO:
      {
        if (FOG_UNLIKELY(!hasMoveTo)) goto _Invalid;

        x0 = pts[-1].x;
        y0 = pts[-1].y;
        x1 = pts[0].x;
        y1 = pts[0].y;

        pts++;
        cmd++;
        i--;

_DoLine:
        {
          NumT dx = x1 - x0;
          NumT dy = y1 - y0;

          if (dy > NumT(0.0))
          {
            if (py >= y0 && py < y1)
            {
              NumT ix = x0 + (py - y0) * dx / dy;
              windingNumber += (px >= ix);
            }
          }
          else if (dy < NumT(0.0))
          {
            if (py >= y1 && py < y0)
            {
              NumT ix = x0 + (py - y0) * dx / dy;
              windingNumber -= (px >= ix);
            }
          }
        }
        break;
      }

      // ----------------------------------------------------------------------
      // [Quad-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_QUAD_TO:
      {
        FOG_ASSERT(hasMoveTo);
        FOG_ASSERT(i >= 2);

        const NumT_(Point)* p = pts - 1;
        if (FOG_UNLIKELY(!hasMoveTo)) goto _Invalid;

        NumT minY = Math::min(p[0].y, p[1].y, p[2].y);
        NumT maxY = Math::max(p[0].y, p[1].y, p[2].y);

        pts += 2;
        cmd += 2;
        i   -= 2;

        if (py >= minY && py <= maxY)
        {
          bool degenerated =
            Math::isFuzzyEq(p[0].y, p[1].y) &&
            Math::isFuzzyEq(p[1].y, p[2].y) ;

          if (degenerated)
          {
            x0 = p[0].x;
            y0 = p[0].y;
            x1 = p[2].x;
            y1 = p[2].y;
            goto _DoLine;
          }

          // Subdivide curve to curve-spline separated at Y-extrama.
          NumT_(Point) left[3];
          NumT_(Point) rght[3];

          NumT tExtrema[2];
          NumT tCut = NumT(0.0);

          tExtrema[0] = (p[0].y - p[1].y) / (p[0].y - NumT(2.0) * p[1].y + p[2].y);

          int tIndex;
          int tLength = tExtrema[0] > NumT(0.0) && tExtrema[0] < NumT(1.0);

          tExtrema[tLength++] = NumT(1.0);

          rght[0] = p[0];
          rght[1] = p[1];
          rght[2] = p[2];

          for (tIndex = 0; tIndex < tLength; tIndex++)
          {
            NumT tVal = tExtrema[tIndex];
            if (tVal == tCut) continue;

            if (tVal == NumT(1.0))
            {
              left[0] = rght[0];
              left[1] = rght[1];
              left[2] = rght[2];
            }
            else
            {
              NumI_(QBezier)::splitAt(rght, left, rght, tCut == NumT(0.0) ? tVal : (tVal - tCut) / (NumT(1.0) - tCut));
            }

            minY = Math::min(left[0].y, left[2].y);
            maxY = Math::max(left[0].y, left[2].y);

            if (py >= minY && py < maxY)
            {
              NumT ax, ay, bx, by, cx, cy;
              _FOG_QUAD_EXTRACT_PARAMETERS(NumT, ax, ay, bx, by, cx, cy, left);

              NumT func[3];
              func[0] = ay;
              func[1] = by;
              func[2] = cy - py;

              int direction = 0;
              if (left[0].y < left[2].y)
                direction = 1;
              else if (left[0].y > left[2].y)
                direction = -1;

              // It should be only possible to have zero/one solution.
              NumT ti[2];
              NumT ix;

              if (Math::solve(ti, func, MATH_SOLVE_QUADRATIC, NumT_(Interval)(NumT(0.0), NumT(1.0))) >= 1)
                ix = ax * Math::pow2(ti[0]) + bx * ti[0] + cx;
              else if (py - minY < maxY - py)
                ix = p[0].x;
              else
                ix = p[2].x;

              if (px >= ix) windingNumber += direction;
            }

            tCut = tVal;
          }
        }
        break;
      }

      // ----------------------------------------------------------------------
      // [Cubic-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_CUBIC_TO:
      {
        FOG_ASSERT(hasMoveTo);
        FOG_ASSERT(i >= 3);

        const NumT_(Point)* p = pts - 1;
        if (FOG_UNLIKELY(!hasMoveTo)) goto _Invalid;

        NumT minY = Math::min(p[0].y, p[1].y, p[2].y, p[3].y);
        NumT maxY = Math::max(p[0].y, p[1].y, p[2].y, p[3].y);

        pts += 3;
        cmd += 3;
        i   -= 3;

        if (py >= minY && py <= maxY)
        {
          bool degenerated =
            Math::isFuzzyEq(p[0].y, p[1].y) &&
            Math::isFuzzyEq(p[1].y, p[2].y) &&
            Math::isFuzzyEq(p[2].y, p[3].y) ;

          if (degenerated)
          {
            x0 = p[0].x;
            y0 = p[0].y;
            x1 = p[3].x;
            y1 = p[3].y;
            goto _DoLine;
          }

          // Subdivide curve to curve-spline separated at Y-extrama.
          NumT_(Point) left[4];
          NumT_(Point) rght[4];

          NumT func[4];
          func[0] = NumT(3.0) * (-p[0].y + NumT(3.0) * (p[1].y - p[2].y) + p[3].y);
          func[1] = NumT(6.0) * ( p[0].y - NumT(2.0) *  p[1].y + p[2].y          );
          func[2] = NumT(3.0) * (-p[0].y +                p[1].y                   );

          NumT tExtrema[3];
          NumT tCut = NumT(0.0);

          int tIndex;
          int tLength;

          tLength = Math::solve(tExtrema, func, MATH_SOLVE_QUADRATIC, NumT_(Interval)(NumT(0.0), NumT(1.0)));
          tExtrema[tLength++] = NumT(1.0);

          rght[0] = p[0];
          rght[1] = p[1];
          rght[2] = p[2];
          rght[3] = p[3];

          for (tIndex = 0; tIndex < tLength; tIndex++)
          {
            NumT tVal = tExtrema[tIndex];
            if (tVal == tCut) continue;

            if (tVal == NumT(1.0))
            {
              left[0] = rght[0];
              left[1] = rght[1];
              left[2] = rght[2];
              left[3] = rght[3];
            }
            else
            {
              NumI_(CBezier)::splitAt(rght, left, rght, tCut == NumT(0.0) ? tVal : (tVal - tCut) / (NumT(1.0) - tCut));
            }

            minY = Math::min(left[0].y, left[3].y);
            maxY = Math::max(left[0].y, left[3].y);

            if (py >= minY && py < maxY)
            {
              NumT ax, ay, bx, by, cx, cy, dx, dy;
              _FOG_CUBIC_EXTRACT_PARAMETERS(NumT, ax, ay, bx, by, cx, cy, dx, dy, left);

              func[0] = ay;
              func[1] = by;
              func[2] = cy;
              func[3] = dy - py;

              int direction = 0;
              if (left[0].y < left[3].y)
                direction = 1;
              else if (left[0].y > left[3].y)
                direction = -1;

              // It should be only possible to have zero/one solution.
              NumT ti[3];
              NumT ix;

              if (Math::solve(ti, func, MATH_SOLVE_CUBIC, NumT_(Interval)(NumT(0.0), NumT(1.0))) >= 1)
                ix = ax * Math::pow3(ti[0]) + bx * Math::pow2(ti[0]) + cx * ti[0] + dx;
              else if (py - minY < maxY - py)
                ix = p[0].x;
              else
                ix = p[3].x;

              if (px >= ix) windingNumber += direction;
            }

            tCut = tVal;
          }
        }
        break;
      }

      // ----------------------------------------------------------------------
      // [Close-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_CLOSE:
      {
        if (hasMoveTo)
        {
          x0 = pts[-1].x;
          y0 = pts[-1].y;
          x1 = start.x;
          y1 = start.y;

          hasMoveTo = false;
          goto _DoLine;
        }

        pts++;
        cmd++;
        i--;
        break;
      }

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  } while (i);

  // Close the path.
  if (hasMoveTo)
  {
    x0 = pts[-1].x;
    y0 = pts[-1].y;
    x1 = start.x;
    y1 = start.y;

    hasMoveTo = false;
    goto _DoLine;
  }

  if (fillRule == FILL_RULE_EVEN_ODD) windingNumber &= 1;
  return windingNumber != 0;

_Invalid:
  return false;
}

// ============================================================================
// [Fog::Path - Transform]
// ============================================================================

template<typename NumT>
static err_t _PathT_transform(
  NumT_(Path)& self,
  const NumT_(Transform)& tr, const Range* range)
{
  size_t length = self._d->length;
  uint32_t transformType = tr.getType();

  if (transformType == TRANSFORM_TYPE_IDENTITY)
  {
    return ERR_OK;
  }

  if (transformType < TRANSFORM_TYPE_PROJECTION)
  {
    if (range == NULL)
    {
      if (length == 0) return ERR_OK;

      FOG_RETURN_ON_ERROR(self.detach());
      tr._mapPoints(self._d->vertices, self._d->vertices, self._d->length);

      self._d->flags |= PATH_DATA_DIRTY_BBOX;
      return ERR_OK;
    }
    else
    {
      size_t start = range->getStart();
      size_t end = range->getEnd();

      if (start >= length && start >= end)
        return ERR_RT_INVALID_ARGUMENT;
      if (end > length) end = length;

      FOG_RETURN_ON_ERROR(self.detach());
      tr._mapPoints(self._d->vertices + start, self._d->vertices + start, end - start);

      self._d->flags |= PATH_DATA_DIRTY_BBOX;
      return ERR_OK;
    }
  }
  else
  {
    if (transformType == TRANSFORM_TYPE_DEGENERATE)
      return ERR_GEOMETRY_DEGENERATE;

    NumT_T1(PathTmp, 128) tmp;

    const uint8_t* srcCmd = self._d->commands;
    const NumT_(Point)* srcPts = self._d->vertices;

    size_t srcLength = length;
    size_t start = 0;
    size_t end = length;

    if (range != NULL)
    {
      start = range->getStart();
      end   = range->getEnd();

      if (start >= length && start >= end)
        return ERR_RT_INVALID_ARGUMENT;
      if (end > length) end = length;

      srcCmd += start;
      srcPts += start;
      srcLength = end - start;
    }

    FOG_RETURN_ON_ERROR(tr.mapPathData(tmp, srcCmd, srcPts, srcLength));
    size_t tmpLength = tmp.getLength();

    if (tmpLength == srcLength)
    {
      FOG_RETURN_ON_ERROR(self.detach());

      Memory::copy(self._d->commands + start, tmp._d->commands, srcLength);
      Memory::copy(self._d->vertices + start, tmp._d->vertices, srcLength * sizeof(NumT_(Point)));
    }
    else
    {
      size_t moveToIndex = start + tmpLength;
      size_t moveToLength = length - end;

      size_t final = length - srcLength + tmpLength;
      FOG_RETURN_ON_ERROR(self.reserve(final));

      Memory::move(self._d->commands + moveToIndex, self._d->commands + start, moveToLength);
      Memory::move(self._d->vertices + moveToIndex, self._d->vertices + start, moveToLength * sizeof(NumT_(Point)));

      Memory::copy(self._d->commands + start, tmp._d->commands, tmpLength);
      Memory::copy(self._d->vertices + start, tmp._d->vertices, tmpLength * sizeof(NumT_(Point)));
    }

    return ERR_OK;
  }
}

template<typename NumT>
static err_t _PathT_translate(
  NumT_(Path)& self,
  const NumT_(Point)& pt, const Range* range)
{
  // Build a transform (fast).
  NumT_(Transform) tr(UNINITIALIZED);
  tr._type = TRANSFORM_TYPE_TRANSLATION;
  tr._20 = pt.x;
  tr._21 = pt.y;

  return _PathT_transform<NumT>(self, tr, range);
}

template<typename NumT>
static err_t _PathT_fitTo(
  NumT_(Path)& self,
  const NumT_(Rect)& bounds)
{
  NumT_(Rect) currentBounds(UNINITIALIZED);

  if (!bounds.isValid()) return ERR_RT_INVALID_ARGUMENT;
  if (self.getBoundingRect(currentBounds) != ERR_OK) return ERR_OK;

  NumT cx = currentBounds.x;
  NumT cy = currentBounds.y;

  NumT tx = bounds.x;
  NumT ty = bounds.y;

  NumT sx = bounds.w / currentBounds.w;
  NumT sy = bounds.h / currentBounds.h;

  FOG_RETURN_ON_ERROR(self.detach());

  size_t i, length = self._d->length;
  NumT_(Point)* pts = self._d->vertices;

  for (i = 0; i < length; i++, pts++)
  {
    pts[0].set((pts[0].x - cx) * sx + tx, (pts[0].y - cy) * sy + ty);
  }

  self._d->boundingBox.x0 = (self._d->boundingBox.x0 - cx) * sx + tx;
  self._d->boundingBox.y0 = (self._d->boundingBox.y0 - cy) * sy + ty;
  self._d->boundingBox.x1 = (self._d->boundingBox.x1 - cx) * sx + tx;
  self._d->boundingBox.y1 = (self._d->boundingBox.y1 - cy) * sy + ty;

  return ERR_OK;
}

template<typename NumT>
static err_t _PathT_scale(
  NumT_(Path)& self,
  const NumT_(Point)& pt, bool keepStartPos)
{
  size_t i, len = self._d->length;
  if (!len) return ERR_OK;

  FOG_RETURN_ON_ERROR(self.detach());
  NumT_(Point)* vertices = self._d->vertices;

  if (keepStartPos)
  {
    uint8_t* commands = self._d->commands;
    NumT tx = NumT(0.0);
    NumT ty = NumT(0.0);

    for (i = 0; i < len; i++)
    {
      if (PathCmd::isVertex(commands[i]))
      {
        tx = vertices[i].x;
        ty = vertices[i].y;
      }
    }

    NumT_(Point) tr(tx - tx * pt.x, ty - ty * pt.y);
    for (i = 0; i < len; i++)
    {
      vertices[i].x *= pt.x;
      vertices[i].y *= pt.y;
      vertices[i].x += tr.x;
      vertices[i].y += tr.y;
    }

    if (self._d->hasBoundingBox())
    {
      NumT_(Box)& b = self._d->boundingBox;

      b.x0 *= pt.x;
      b.y0 *= pt.y;
      b.x0 += tr.x;
      b.y0 += tr.y;

      b.x1 *= pt.x;
      b.y1 *= pt.y;
      b.x1 += tr.x;
      b.y1 += tr.y;

      if (b.x0 > b.x1) swap(b.x0, b.x1);
      if (b.y0 > b.y1) swap(b.y0, b.y1);
    }
  }
  else
  {
    for (i = 0; i < len; i++)
    {
      vertices[i].x *= pt.x;
      vertices[i].y *= pt.y;
    }

    if (self._d->hasBoundingBox())
    {
      NumT_(Box)& b = self._d->boundingBox;

      b.x0 *= pt.x;
      b.y0 *= pt.y;

      b.x1 *= pt.x;
      b.y1 *= pt.y;

      if (b.x0 > b.x1) swap(b.x0, b.x1);
      if (b.y0 > b.y1) swap(b.y0, b.y1);
    }
  }

  return ERR_OK;
}

template<typename NumT>
static err_t _PathT_flipX(
  NumT_(Path)& self, NumT x0, NumT x1)
{
  size_t i, len = self._d->length;
  if (!len) return ERR_OK;

  FOG_RETURN_ON_ERROR(self.detach());

  NumT x = x0 + x1;
  NumT_(Point)* vertices = self._d->vertices;

  for (i = 0; i < len; i++)
  {
    vertices[i].x = x - vertices[i].x;
  }

  if (self._d->hasBoundingBox())
  {
    NumT xMin = x - self._d->boundingBox.x0;
    NumT xMax = x - self._d->boundingBox.x1;
    if (xMax < xMin) swap(xMin, xMax);

    self._d->boundingBox.x0 = xMin;
    self._d->boundingBox.x1 = xMax;
  }

  return ERR_OK;
}

template<typename NumT>
static err_t _PathT_flipY(
  NumT_(Path)& self, NumT y0, NumT y1)
{
  size_t i, len = self._d->length;
  if (!len) return ERR_OK;

  FOG_RETURN_ON_ERROR(self.detach());

  NumT y = y0 + y1;
  NumT_(Point)* vertices = self._d->vertices;

  for (i = 0; i < len; i++)
  {
    vertices[i].y = y - vertices[i].y;
  }

  if (self._d->hasBoundingBox())
  {
    NumT yMin = y - self._d->boundingBox.y0;
    NumT yMax = y - self._d->boundingBox.y1;
    if (yMax < yMin) swap(yMin, yMax);

    self._d->boundingBox.y0 = yMin;
    self._d->boundingBox.y1 = yMax;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::NumT_(Path) - Equality]
// ============================================================================

template<typename NumT>
static bool _PathT_eq(const NumT_(Path)& _a, const NumT_(Path)& _b)
{
  const NumT_(PathData)* a = _a._d;
  const NumT_(PathData)* b = _b._d;

  if (a == b) return true;

  size_t length = a->length;
  if (length != b->length) return false;

  return memcmp(a->commands, b->commands, length) == 0 &&
         memcmp(a->vertices, b->vertices, length * sizeof(NumT_(Point) )) == 0 ;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_path_init(void)
{
  _g2d.pathf.ctor = _PathT_ctor<float>;
  _g2d.pathf.ctorCopyF = _PathT_ctorCopyT<float>;
  _g2d.pathf.dtor = _PathT_dtor<float>;
  _g2d.pathf.detach = _PathT_detach<float>;
  _g2d.pathf.reserve = _PathT_reserve<float>;
  _g2d.pathf.squeeze = _PathT_squeeze<float>;
  _g2d.pathf.prepare = _PathT_prepare<float>;
  _g2d.pathf.add = _PathT_add<float>;
  _g2d.pathf.clear = _PathT_clear<float>;
  _g2d.pathf.reset = _PathT_reset<float>;
  _g2d.pathf.setPathF = _PathT_setPathT<float>;
  _g2d.pathf.setDeepF = _PathT_setDeepT<float, float>;
  _g2d.pathf.getSubpathRange = _PathT_getSubpathRange<float>;
  _g2d.pathf.moveTo = _PathT_moveTo<float>;
  _g2d.pathf.moveToRel = _PathT_moveToRel<float>;
  _g2d.pathf.lineTo = _PathT_lineTo<float>;
  _g2d.pathf.lineToRel = _PathT_lineToRel<float>;
  _g2d.pathf.hlineTo = _PathT_hlineTo<float>;
  _g2d.pathf.hlineToRel = _PathT_hlineToRel<float>;
  _g2d.pathf.vlineTo = _PathT_vlineTo<float>;
  _g2d.pathf.vlineToRel = _PathT_vlineToRel<float>;
  _g2d.pathf.polyTo = _PathT_polyTo<float>;
  _g2d.pathf.polyToRel = _PathT_polyToRel<float>;
  _g2d.pathf.quadTo = _PathT_quadTo<float>;
  _g2d.pathf.quadToRel = _PathT_quadToRel<float>;
  _g2d.pathf.cubicTo = _PathT_cubicTo<float>;
  _g2d.pathf.cubicToRel = _PathT_cubicToRel<float>;
  _g2d.pathf.smoothQuadTo = _PathT_smoothQuadTo<float>;
  _g2d.pathf.smoothQuadToRel = _PathT_smoothQuadToRel<float>;
  _g2d.pathf.smoothCubicTo = _PathT_smoothCubicTo<float>;
  _g2d.pathf.smoothCubicToRel = _PathT_smoothCubicToRel<float>;
  _g2d.pathf.arcTo = _PathT_arcTo<float>;
  _g2d.pathf.arcToRel = _PathT_arcToRel<float>;
  _g2d.pathf.svgArcTo = _PathT_svgArcTo<float>;
  _g2d.pathf.svgArcToRel = _PathT_svgArcToRel<float>;
  _g2d.pathf.close = _PathT_close<float>;
  _g2d.pathf.boxI = _PathT_boxT<float, int>;
  _g2d.pathf.boxF = _PathT_boxT<float, float>;
  _g2d.pathf.rectI = _PathT_rectT<float, int>;
  _g2d.pathf.rectF = _PathT_rectT<float, float>;
  _g2d.pathf.boxesI = _PathT_boxesT<float, int>;
  _g2d.pathf.boxesF = _PathT_boxesT<float, float>;
  _g2d.pathf.rectsI = _PathT_rectsT<float, int>;
  _g2d.pathf.rectsF = _PathT_rectsT<float, float>;
  _g2d.pathf.region = _PathT_region<float>;
  _g2d.pathf.polylineI = _PathT_polylineI<float>;
  _g2d.pathf.polylineF = _PathT_polylineT<float>;
  _g2d.pathf.polygonI = _PathT_polygonI<float>;
  _g2d.pathf.polygonF = _PathT_polygonT<float>;
  _g2d.pathf.shape = _PathT_shape<float>;
  _g2d.pathf.appendPathF = _PathT_appendPathT<float, float>;
  _g2d.pathf.appendTranslatedPathF = _PathT_appendTranslatedPathT<float, float>;
  _g2d.pathf.appendTransformedPathF = _PathT_appendTransformedPathT<float, float>;
  _g2d.pathf.updateFlat = _PathT_updateFlat<float>;
  _g2d.pathf.flatten = _PathT_flatten<float>;
  _g2d.pathf.getBoundingBox = _PathT_getBoundingBox<float>;
  _g2d.pathf.hitTest = _PathT_hitTest<float>;
  _g2d.pathf.translate = _PathT_translate<float>;
  _g2d.pathf.transform = _PathT_transform<float>;
  _g2d.pathf.fitTo = _PathT_fitTo<float>;
  _g2d.pathf.scale = _PathT_scale<float>;
  _g2d.pathf.flipX = _PathT_flipX<float>;
  _g2d.pathf.flipY = _PathT_flipY<float>;
  _g2d.pathf.eq = _PathT_eq<float>;

  _g2d.pathd.ctor = _PathT_ctor<double>;
  _g2d.pathd.ctorCopyD = _PathT_ctorCopyT<double>;
  _g2d.pathd.dtor = _PathT_dtor<double>;
  _g2d.pathd.detach = _PathT_detach<double>;
  _g2d.pathd.reserve = _PathT_reserve<double>;
  _g2d.pathd.squeeze = _PathT_squeeze<double>;
  _g2d.pathd.prepare = _PathT_prepare<double>;
  _g2d.pathd.add = _PathT_add<double>;
  _g2d.pathd.clear = _PathT_clear<double>;
  _g2d.pathd.reset = _PathT_reset<double>;
  _g2d.pathd.setPathD = _PathT_setPathT<double>;
  _g2d.pathd.setPathF = _PathT_setDeepT<double, float>;
  _g2d.pathd.setDeepD = _PathT_setDeepT<double, double>;
  _g2d.pathd.getSubpathRange = _PathT_getSubpathRange<double>;
  _g2d.pathd.moveTo = _PathT_moveTo<double>;
  _g2d.pathd.moveToRel = _PathT_moveToRel<double>;
  _g2d.pathd.lineTo = _PathT_lineTo<double>;
  _g2d.pathd.lineToRel = _PathT_lineToRel<double>;
  _g2d.pathd.hlineTo = _PathT_hlineTo<double>;
  _g2d.pathd.hlineToRel = _PathT_hlineToRel<double>;
  _g2d.pathd.vlineTo = _PathT_vlineTo<double>;
  _g2d.pathd.vlineToRel = _PathT_vlineToRel<double>;
  _g2d.pathd.polyTo = _PathT_polyTo<double>;
  _g2d.pathd.polyToRel = _PathT_polyToRel<double>;
  _g2d.pathd.quadTo = _PathT_quadTo<double>;
  _g2d.pathd.quadToRel = _PathT_quadToRel<double>;
  _g2d.pathd.cubicTo = _PathT_cubicTo<double>;
  _g2d.pathd.cubicToRel = _PathT_cubicToRel<double>;
  _g2d.pathd.smoothQuadTo = _PathT_smoothQuadTo<double>;
  _g2d.pathd.smoothQuadToRel = _PathT_smoothQuadToRel<double>;
  _g2d.pathd.smoothCubicTo = _PathT_smoothCubicTo<double>;
  _g2d.pathd.smoothCubicToRel = _PathT_smoothCubicToRel<double>;
  _g2d.pathd.arcTo = _PathT_arcTo<double>;
  _g2d.pathd.arcToRel = _PathT_arcToRel<double>;
  _g2d.pathd.svgArcTo = _PathT_svgArcTo<double>;
  _g2d.pathd.svgArcToRel = _PathT_svgArcToRel<double>;
  _g2d.pathd.close = _PathT_close<double>;
  _g2d.pathd.boxI = _PathT_boxT<double, int>;
  _g2d.pathd.boxF = _PathT_boxT<double, float>;
  _g2d.pathd.boxD = _PathT_boxT<double, double>;
  _g2d.pathd.rectI = _PathT_rectT<double, int>;
  _g2d.pathd.rectF = _PathT_rectT<double, float>;
  _g2d.pathd.rectD = _PathT_rectT<double, double>;
  _g2d.pathd.boxesI = _PathT_boxesT<double, int>;
  _g2d.pathd.boxesF = _PathT_boxesT<double, float>;
  _g2d.pathd.boxesD = _PathT_boxesT<double, double>;
  _g2d.pathd.rectsI = _PathT_rectsT<double, int>;
  _g2d.pathd.rectsF = _PathT_rectsT<double, float>;
  _g2d.pathd.rectsD = _PathT_rectsT<double, double>;
  _g2d.pathd.region = _PathT_region<double>;
  _g2d.pathd.polylineI = _PathT_polylineI<double>;
  _g2d.pathd.polylineD = _PathT_polylineT<double>;
  _g2d.pathd.polygonI = _PathT_polygonI<double>;
  _g2d.pathd.polygonD = _PathT_polygonT<double>;
  _g2d.pathd.shape = _PathT_shape<double>;
  _g2d.pathd.appendPathF = _PathT_appendPathT<double, float>;
  _g2d.pathd.appendPathD = _PathT_appendPathT<double, double>;
  _g2d.pathd.appendTranslatedPathF = _PathT_appendTranslatedPathT<double, float>;
  _g2d.pathd.appendTranslatedPathD = _PathT_appendTranslatedPathT<double, double>;
  _g2d.pathd.appendTransformedPathF = _PathT_appendTransformedPathT<double, float>;
  _g2d.pathd.appendTransformedPathD = _PathT_appendTransformedPathT<double, double>;
  _g2d.pathd.updateFlat = _PathT_updateFlat<double>;
  _g2d.pathd.flatten = _PathT_flatten<double>;
  _g2d.pathd.getBoundingBox = _PathT_getBoundingBox<double>;
  _g2d.pathd.hitTest = _PathT_hitTest<double>;
  _g2d.pathd.translate = _PathT_translate<double>;
  _g2d.pathd.transform = _PathT_transform<double>;
  _g2d.pathd.fitTo = _PathT_fitTo<double>;
  _g2d.pathd.scale = _PathT_scale<double>;
  _g2d.pathd.flipX = _PathT_flipX<double>;
  _g2d.pathd.flipY = _PathT_flipY<double>;
  _g2d.pathd.eq = _PathT_eq<double>;

  {
    PathDataF* d = _PathT_getDNull<float>();

    d->refCount.init(1);
    d->flags = NO_FLAGS;
    d->capacity = 0;
    d->length = 0;
    d->boundingBox.reset();
  }

  {
    PathDataD* d = _PathT_getDNull<double>();

    d->refCount.init(1);
    d->flags = NO_FLAGS;
    d->capacity = 0;
    d->length = 0;
    d->boundingBox.reset();
  }
}

FOG_NO_EXPORT void _g2d_path_fini(void)
{
  _PathT_getDNull<float>()->refCount.dec();
  _PathT_getDNull<double>()->refCount.dec();
}

} // Fog namespace
