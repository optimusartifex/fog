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
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/G2d/Geometry/Internals_p.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Path.h>
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

static PathDataF _G2d_PathF_dnull;
static PathDataD _G2d_PathD_dnull;

template<typename Number>
FOG_INLINE typename PathDataT<Number>::T* _G2d_PathT_getDNull() { return NULL; }

template<>
FOG_INLINE PathDataF* _G2d_PathT_getDNull<float>() { return &_G2d_PathF_dnull; }

template<>
FOG_INLINE PathDataD* _G2d_PathT_getDNull<double>() { return &_G2d_PathD_dnull; }

// ============================================================================
// [Fog::Path - Helpers]
// ============================================================================

template<typename Number>
static FOG_INLINE sysuint_t _G2d_PathT_getDataSize(sysuint_t capacity)
{
  sysuint_t s = sizeof(typename PathDataT<Number>::T);

  s += capacity * sizeof(uint8_t);
  s += capacity * sizeof(typename PointT<Number>::T);

  return s;
}

template<typename Number>
static FOG_INLINE void _G2d_PathT_updateDataPointers(
  typename PathDataT<Number>::T* d, sysuint_t capacity)
{
  d->vertices = reinterpret_cast<typename PointT<Number>::T*>(
    d->commands + (((capacity + 15) & ~15) * sizeof(uint8_t))
  );
}

template<typename Number>
static bool FOG_FASTCALL _G2d_PathT_getLastPoint(
  typename PathDataT<Number>::T* d, typename PointT<Number>::T& dst)
{
  sysuint_t last = d->length;
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

template<typename Number>
static void _G2d_PathT_arcToBezier(
  const typename PointT<Number>::T& cp,
  const typename PointT<Number>::T& rp,
  Number start, Number sweep, typename PointT<Number>::T* dst)
{
  sweep *= Number(0.5);

  Number x0, y0;
  Math::sincos(sweep, &y0, &x0);

  Number tx = (Number(1.0) - x0) * Number(4.0 / 3.0);
  Number ty = y0 - tx * x0 / y0;
  typename PointT<Number>::T p[4];

  Number aSin, aCos;
  Math::sincos(start + sweep, &aSin, &aCos);

  p[0].set(x0     , -y0);
  p[1].set(x0 + tx, -ty);
  p[2].set(x0 + tx,  ty);
  p[3].set(x0     ,  y0);

  for (uint i = 0; i < 4; i++)
  {
    dst[i].set(cp.x + rp.x * (p[i].x * aCos - p[i].y * aSin),
               cp.y + rp.y * (p[i].x * aSin + p[i].y * aCos));
  }
}

#define PATH_ADD_VERTEX_BEGIN(Number, _Count_) \
  { \
    sysuint_t _length = self._d->length; \
    \
    { \
      sysuint_t _remain = self._d->capacity - _length; \
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
    typename PointT<Number>::T* vertices = self._d->vertices + _length;

#define PATH_ADD_VERTEX_END() \
  } \
  return ERR_OK;

// ============================================================================
// [Fog::Path - Statics]
// ============================================================================

template<typename Number>
static typename PathDataT<Number>::T* _G2d_PathT_dalloc(sysuint_t capacity)
{
  sysuint_t dsize = _G2d_PathT_getDataSize<Number>(capacity);

  typename PathDataT<Number>::T* newd = reinterpret_cast<typename PathDataT<Number>::T*>(Memory::alloc(dsize));
  if (FOG_IS_NULL(newd)) return NULL;

  newd->refCount.init(1);
  newd->flat = 1;
  newd->boundingBoxDirty = true;
  newd->capacity = capacity;
  newd->length = 0;
  newd->boundingBox.reset();

  _G2d_PathT_updateDataPointers<Number>(newd, capacity);
  return newd;
}

template<typename Number>
static typename PathDataT<Number>::T* _G2d_PathT_drealloc(typename PathDataT<Number>::T* d, sysuint_t capacity)
{
  FOG_ASSERT(d->length <= capacity);
  sysuint_t dsize = _G2d_PathT_getDataSize<Number>(capacity);

  typename PathDataT<Number>::T* newd = reinterpret_cast<typename PathDataT<Number>::T*>(Memory::alloc(dsize));
  if (FOG_IS_NULL(newd)) return NULL;

  sysuint_t length = d->length;

  newd->refCount.init(1);
  newd->flat = d->flat;
  newd->boundingBoxDirty = d->boundingBoxDirty;
  newd->capacity = capacity;
  newd->length = length;
  newd->boundingBox = d->boundingBox;

  _G2d_PathT_updateDataPointers<Number>(newd, capacity);
  Memory::copy(newd->commands, d->commands, length * sizeof(uint8_t));
  Memory::copy(newd->vertices, d->vertices, length * sizeof(typename PointT<Number>::T));

  d->deref();
  return newd;
}

template<typename Number>
static typename PathDataT<Number>::T* _G2d_PathT_dcopy(const typename PathDataT<Number>::T* d)
{
  sysuint_t length = d->length;
  if (!length) return _G2d_PathT_getDNull<Number>()->ref();

  typename PathDataT<Number>::T* newd = _G2d_PathT_dalloc<Number>(length);
  if (FOG_IS_NULL(newd)) return NULL;

  newd->length = length;
  newd->flat = d->flat;
  newd->boundingBoxDirty = d->boundingBoxDirty;
  newd->boundingBox = d->boundingBox;

  Memory::copy(newd->commands, d->commands, length * sizeof(uint8_t));
  Memory::copy(newd->vertices, d->vertices, length * sizeof(typename PointT<Number>::T));

  return newd;
}

// ============================================================================
// [Fog::Path - Construction / Destruction]
// ============================================================================

template<typename Number>
static void _G2d_PathT_ctor(typename PathT<Number>::T& self)
{
  self._d = _G2d_PathT_getDNull<Number>()->ref();
}

template<typename Number>
static void _G2d_PathT_ctorCopyT(typename PathT<Number>::T& self, const typename PathT<Number>::T& other)
{
  self._d = other._d->ref();
}

template<typename Number>
static void _G2d_PathT_dtor(typename PathT<Number>::T& self)
{
  self._d->deref();
}

// ============================================================================
// [Fog::Path - Data]
// ============================================================================

template<typename Number>
static err_t _G2d_PathT_detach(typename PathT<Number>::T& self)
{
  if (self.isDetached()) return ERR_OK;

  typename PathDataT<Number>::T* newd = _G2d_PathT_dcopy<Number>(self._d);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&self._d, newd)->deref();
  return ERR_OK;
}

template<typename Number>
static err_t _G2d_PathT_reserve(typename PathT<Number>::T& self, sysuint_t capacity)
{
  if (self._d->refCount.get() == 1 && self._d->capacity >= capacity) return ERR_OK;

  sysuint_t length = self._d->length;
  if (capacity < length) capacity = length;

  typename PathDataT<Number>::T* newd = _G2d_PathT_dalloc<Number>(capacity);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  newd->length = length;
  newd->flat = self._d->flat;
  newd->boundingBoxDirty = self._d->boundingBoxDirty;
  newd->boundingBox = self._d->boundingBox;

  Memory::copy(newd->commands, self._d->commands, length * sizeof(uint8_t));
  Memory::copy(newd->vertices, self._d->vertices, length * sizeof(typename PointT<Number>::T));

  atomicPtrXchg(&self._d, newd)->deref();
  return ERR_OK;
}

template<typename Number>
static void _G2d_PathT_squeeze(typename PathT<Number>::T& self)
{
  if (self._d->length == self._d->capacity) return;

  if (self._d->refCount.get() == 1)
  {
    typename PathDataT<Number>::T* newd = _G2d_PathT_drealloc<Number>(self._d, self._d->length);
    if (FOG_IS_NULL(newd)) return;

    atomicPtrXchg(&self._d, newd);
  }
  else
  {
    typename PathDataT<Number>::T* newd = _G2d_PathT_dcopy<Number>(self._d);
    if (FOG_IS_NULL(newd)) return;

    atomicPtrXchg(&self._d, newd)->deref();
  }
}

template<typename Number>
static sysuint_t _G2d_PathT_add(typename PathT<Number>::T& self, sysuint_t count)
{
  sysuint_t length = self._d->length;
  sysuint_t remain = self._d->capacity - length;

  if (self._d->refCount.get() == 1 && count <= remain)
  {
    self._d->length += count;
    return length;
  }
  else
  {
    sysuint_t optimalCapacity =
      Util::getGrowCapacity(sizeof(typename PathDataT<Number>::T), sizeof(typename PointT<Number>::T) + sizeof(uint8_t), length, length + count);

    typename PathDataT<Number>::T* newd = _G2d_PathT_dalloc<Number>(optimalCapacity);
    if (FOG_IS_NULL(newd)) return INVALID_INDEX;

    newd->length = length + count;
    newd->flat = self._d->flat;
    newd->boundingBoxDirty = self._d->boundingBoxDirty;
    newd->boundingBox = self._d->boundingBox;

    Memory::copy(newd->commands, self._d->commands, length * sizeof(uint8_t));
    Memory::copy(newd->vertices, self._d->vertices, length * sizeof(typename PointT<Number>::T));

    atomicPtrXchg(&self._d, newd)->deref();
    return length;
  }
}

template<typename Number>
static void _G2d_PathT_updateFlat(const typename PathT<Number>::T& self)
{
  uint flat = self._d->flat;
  if (flat <= 1) return;

  flat = 1;

  const uint8_t* commands = self._d->commands;
  for (sysuint_t i = self._d->length; i; i--, commands++)
  {
    if (PathCmd::isQuadOrCubicTo(commands[0])) { flat = 0; break; }
  }

  self._d->flat = flat;
}

template<typename Number>
static void _G2d_PathT_updateBoundingBox(const typename PathT<Number>::T& self)
{
  if (self._d->boundingBoxDirty)
  {
    sysuint_t i = self._d->length;

    const uint8_t* cmd = self._d->commands;
    const typename PointT<Number>::T* pts = self._d->vertices;

    bool isFirst = true;
    typename BoxT<Number>::T bounds(Number(0.0), Number(0.0), Number(0.0), Number(0.0));

    if (i > 0)
    {
_Repeat:
      // Find the 'move-to' command.
      do {
        uint c = cmd[0];

        if (c == PATH_CMD_MOVE_TO)
        {
          if (isFirst)
          {
            bounds.x0 = pts[0].x;
            bounds.y0 = pts[0].y;
            bounds.x1 = pts[0].x;
            bounds.y1 = pts[0].y;

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
        switch (cmd[0])
        {
          case PATH_CMD_MOVE_TO:
          case PATH_CMD_LINE_TO:
            if (pts[0].x < bounds.x0) bounds.x0 = pts[0].x; else if (pts[0].x > bounds.x1) bounds.x1 = pts[0].x;
            if (pts[0].y < bounds.y0) bounds.y0 = pts[0].y; else if (pts[0].y > bounds.y1) bounds.y1 = pts[0].y;

            i--;
            cmd++;
            pts++;
            break;

          case PATH_CMD_QUAD_TO:
            FOG_ASSERT(i >= 2);
            if (FOG_UNLIKELY(i < 2)) break;

            // Merge end point - pts[1].
            if (pts[1].x < bounds.x0) bounds.x0 = pts[1].x; else if (pts[1].x > bounds.x1) bounds.x1 = pts[1].x;
            if (pts[1].y < bounds.y0) bounds.y0 = pts[1].y; else if (pts[1].y > bounds.y1) bounds.y1 = pts[1].y;

            // Do calculation only when necessary.
            if (!(pts[0].x > bounds.x0 && pts[0].y > bounds.y0 &&
                  pts[0].x < bounds.x1 && pts[0].y < bounds.y1 ))
            {
              typename BoxT<Number>::T e = QuadCurveT<Number>::T::getBoundingBox(pts - 1);

              if (e.x0 < bounds.x0) bounds.x0 = e.x0;
              if (e.y0 < bounds.y0) bounds.y0 = e.y0;

              if (e.x1 > bounds.x1) bounds.x1 = e.x1;
              if (e.y1 > bounds.y1) bounds.y1 = e.y1;
            }

            i -= 2;
            cmd += 2;
            pts += 2;
            break;

          case PATH_CMD_CUBIC_TO:
            FOG_ASSERT(i >= 3);
            if (FOG_UNLIKELY(i < 3)) break;

            // Merge end point - pts[2].
            if (pts[2].x < bounds.x0) bounds.x0 = pts[2].x; else if (pts[2].x > bounds.x1) bounds.x1 = pts[2].x;
            if (pts[2].y < bounds.y0) bounds.y0 = pts[2].y; else if (pts[2].y > bounds.y1) bounds.y1 = pts[2].y;

            // Do calculation only when necessary.
            if (!(pts[0].x > bounds.x0 && pts[1].x > bounds.x0 &&
                  pts[0].y > bounds.y0 && pts[1].y > bounds.y0 &&
                  pts[0].x < bounds.x1 && pts[1].x < bounds.x1 &&
                  pts[0].y < bounds.y1 && pts[1].y < bounds.y1 ))
            {
              typename BoxT<Number>::T e = CubicCurveT<Number>::T::getBoundingBox(pts - 1);

              if (e.x0 < bounds.x0) bounds.x0 = e.x0;
              if (e.y0 < bounds.y0) bounds.y0 = e.y0;

              if (e.x1 > bounds.x1) bounds.x1 = e.x1;
              if (e.y1 > bounds.y1) bounds.y1 = e.y1;
            }

            i -= 3;
            cmd += 3;
            pts += 3;
            break;

          case PATH_CMD_CLOSE:
            cmd++;
            pts++;
            if (--i) goto _Repeat;
            break;
        }
      }
    }

    self._d->boundingBoxDirty = false;
    self._d->boundingBox = bounds;
  }
  return;

_Invalid:
  self._d->boundingBoxDirty = false;
  self._d->boundingBox.reset();
}

// ============================================================================
// [Fog::Path - Clear / Free]
// ============================================================================

template<typename Number>
static void _G2d_PathT_clear(typename PathT<Number>::T& self)
{
  if (self._d->refCount.get() > 1)
  {
    atomicPtrXchg(&self._d, _G2d_PathT_getDNull<Number>()->ref())->deref();
  }
  else
  {
    self._d->length = 0;

    self._d->flat = 1;
    self._d->boundingBoxDirty = true;
    self._d->boundingBox.reset();
  }
}

template<typename Number>
static void _G2d_PathT_reset(typename PathT<Number>::T& self)
{
  atomicPtrXchg(&self._d, _G2d_PathT_getDNull<Number>()->ref())->deref();
}

// ============================================================================
// [Fog::Path - Set]
// ============================================================================

template<typename Number>
static err_t _G2d_PathT_setPathT(typename PathT<Number>::T& self, const typename PathT<Number>::T& other)
{
  if (self._d == other._d) return ERR_OK;

  atomicPtrXchg(&self._d, other._d->ref())->deref();
  return ERR_OK;
}

template<typename Number>
static err_t _G2d_PathT_setDeepT(typename PathT<Number>::T& self, const typename PathT<Number>::T& other)
{
  typename PathDataT<Number>::T* self_d = self._d;
  typename PathDataT<Number>::T* other_d = other._d;

  if (self_d == other_d) return ERR_OK;
  if (other_d->length == 0) { self.clear(); return ERR_OK; }

  err_t err = self.reserve(other_d->length);
  if (FOG_IS_ERROR(err)) { self.clear(); return ERR_RT_OUT_OF_MEMORY; }

  self_d = self._d;
  sysuint_t length = other_d->length;

  self_d->length = length;

  self_d->flat = other_d->flat;
  self_d->boundingBoxDirty = other_d->boundingBoxDirty;
  self_d->boundingBox = other_d->boundingBox;

  Memory::copy(self_d->commands, other_d->commands, length * sizeof(uint8_t));
  Memory::copy(self_d->vertices, other_d->vertices, length * sizeof(typename PointT<Number>::T));

  return ERR_OK;
}

// ============================================================================
// [Fog::Path - SubPath]
// ============================================================================

template<typename Number>
static Range _G2d_PathT_getSubpathRange(const typename PathT<Number>::T& self, sysuint_t index)
{
  sysuint_t length = self._d->length;
  if (index >= length) return Range(INVALID_INDEX, INVALID_INDEX);

  sysuint_t i = index + 1;
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

template<typename Number>
static err_t _G2d_PathT_moveTo(typename PathT<Number>::T& self, const typename PointT<Number>::T& pt0)
{
  sysuint_t pos = self._add(1);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  typename PointT<Number>::T* vertices = self._d->vertices + pos;

  commands[0] = PATH_CMD_MOVE_TO;
  vertices[0] = pt0;

  self._d->boundingBoxDirty = true;
  return ERR_OK;
}

template<typename Number>
static err_t _G2d_PathT_moveToRel(typename PathT<Number>::T& self, const typename PointT<Number>::T& pt0)
{
  typename PointT<Number>::T tr;
  if (!_G2d_PathT_getLastPoint<Number>(self._d, tr)) return ERR_PATH_NO_RELATIVE;

  return self.moveTo(pt0 + tr);
}

// ============================================================================
// [Fog::Path - LineTo]
// ============================================================================

template<typename Number>
static err_t _G2d_PathT_lineTo(typename PathT<Number>::T& self, const typename PointT<Number>::T& pt1)
{
  PATH_ADD_VERTEX_BEGIN(Number, 1)
    commands[0] = PATH_CMD_LINE_TO;
    vertices[0] = pt1;

    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

template<typename Number>
static err_t _G2d_PathT_lineToRel(typename PathT<Number>::T& self, const typename PointT<Number>::T& pt1)
{
  PATH_ADD_VERTEX_BEGIN(Number, 1)
    typename PointT<Number>::T tr(vertices[-1]);

    commands[0] = PATH_CMD_LINE_TO;
    vertices[0] = pt1 + tr;

    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

template<typename Number>
static err_t _G2d_PathT_hlineTo(typename PathT<Number>::T& self, Number x)
{
  PATH_ADD_VERTEX_BEGIN(Number, 1)
    commands[0] = PATH_CMD_LINE_TO;
    vertices[0].set(x, vertices[-1].y);

    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

template<typename Number>
static err_t _G2d_PathT_hlineToRel(typename PathT<Number>::T& self, Number x)
{
  PATH_ADD_VERTEX_BEGIN(Number, 1)
    commands[0] = PATH_CMD_LINE_TO;
    vertices[0].set(x + vertices[-1].x, vertices[-1].y);

    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

template<typename Number>
static err_t _G2d_PathT_vlineTo(typename PathT<Number>::T& self, Number y)
{
  PATH_ADD_VERTEX_BEGIN(Number, 1)
    commands[0] = PATH_CMD_LINE_TO;
    vertices[0].set(vertices[-1].x, y);

    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

template<typename Number>
static err_t _G2d_PathT_vlineToRel(typename PathT<Number>::T& self, Number y)
{
  PATH_ADD_VERTEX_BEGIN(Number, 1)
    commands[0] = PATH_CMD_LINE_TO;
    vertices[0].set(vertices[-1].x, y + vertices[-1].y);

    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

// ============================================================================
// [Fog::Path - PolyTo]
// ============================================================================

template<typename Number>
static err_t _G2d_PathT_polyTo(typename PathT<Number>::T& self, const typename PointT<Number>::T* pts, sysuint_t count)
{
  if (count == 0) return ERR_OK;
  FOG_ASSERT(pts != NULL);

  PATH_ADD_VERTEX_BEGIN(Number, count)
    sysuint_t i;
    for (i = 0; i < count; i++) commands[i] = PATH_CMD_LINE_TO;
    for (i = 0; i < count; i++) vertices[i] = pts[i];

    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

template<typename Number>
static err_t _G2d_PathT_polyToRel(typename PathT<Number>::T& self, const typename PointT<Number>::T* pts, sysuint_t count)
{
  if (count == 0) return ERR_OK;
  FOG_ASSERT(pts != NULL);

  PATH_ADD_VERTEX_BEGIN(Number, count)
    typename PointT<Number>::T tr(vertices[-1]);

    sysuint_t i;
    for (i = 0; i < count; i++) commands[i] = PATH_CMD_LINE_TO;
    for (i = 0; i < count; i++) vertices[i] = pts[i] + tr;

    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

// ============================================================================
// [Fog::Path - QuadTo]
// ============================================================================

template<typename Number>
static err_t _G2d_PathT_quadTo(
  typename PathT<Number>::T& self, 
  const typename PointT<Number>::T& pt1, 
  const typename PointT<Number>::T& pt2)
{
  PATH_ADD_VERTEX_BEGIN(Number, 2)
    commands[0] = PATH_CMD_QUAD_TO;
    commands[1] = PATH_CMD_QUAD_TO;

    vertices[0] = pt1;
    vertices[1] = pt2;

    self._d->flat = 0;
    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

template<typename Number>
static err_t _G2d_PathT_quadToRel(
  typename PathT<Number>::T& self, 
  const typename PointT<Number>::T& pt1, 
  const typename PointT<Number>::T& pt2)
{
  PATH_ADD_VERTEX_BEGIN(Number, 2)
    typename PointT<Number>::T tr(vertices[-1]);

    commands[0] = PATH_CMD_QUAD_TO;
    commands[1] = PATH_CMD_QUAD_TO;

    vertices[0] = pt1 + tr;
    vertices[1] = pt2 + tr;

    self._d->flat = 0;
    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

template<typename Number>
static err_t _G2d_PathT_smoothQuadTo(
  typename PathT<Number>::T& self, 
  const typename PointT<Number>::T& pt2)
{
  PATH_ADD_VERTEX_BEGIN(Number, 2)
    typename PointT<Number>::T pt1 = vertices[-1];

    commands[0] = PATH_CMD_QUAD_TO;
    commands[1] = PATH_CMD_QUAD_TO;

    if (_length >= 2 && PathCmd::isQuadOrCubicTo(commands[-2]))
    {
      pt1 += pt1;
      pt1 -= vertices[-2];
    }

    vertices[0] = pt1;
    vertices[1] = pt2;

    self._d->flat = 0;
    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

template<typename Number>
static err_t _G2d_PathT_smoothQuadToRel(
  typename PathT<Number>::T& self, 
  const typename PointT<Number>::T& pt2)
{
  PATH_ADD_VERTEX_BEGIN(Number, 2)
    typename PointT<Number>::T pt1(vertices[-1]);

    commands[0] = PATH_CMD_QUAD_TO;
    commands[1] = PATH_CMD_QUAD_TO;
    vertices[1] = pt2 + pt1;

    if (_length >= 2 && PathCmd::isQuadOrCubicTo(commands[-2]))
    {
      pt1 += pt1;
      pt1 -= vertices[-2];
    }

    vertices[0] = pt1;

    self._d->flat = 0;
    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

// ============================================================================
// [Fog::Path - CubicTo]
// ============================================================================

template<typename Number>
static err_t _G2d_PathT_cubicTo(
  typename PathT<Number>::T& self, 
  const typename PointT<Number>::T& pt1, 
  const typename PointT<Number>::T& pt2, 
  const typename PointT<Number>::T& pt3)
{
  PATH_ADD_VERTEX_BEGIN(Number, 3)
    commands[0] = PATH_CMD_CUBIC_TO;
    commands[1] = PATH_CMD_CUBIC_TO;
    commands[2] = PATH_CMD_CUBIC_TO;

    vertices[0] = pt1;
    vertices[1] = pt2;
    vertices[2] = pt3;

    self._d->flat = 0;
    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

template<typename Number>
static err_t _G2d_PathT_cubicToRel(
  typename PathT<Number>::T& self, 
  const typename PointT<Number>::T& pt1, 
  const typename PointT<Number>::T& pt2, 
  const typename PointT<Number>::T& pt3)
{
  PATH_ADD_VERTEX_BEGIN(Number, 3)
    typename PointT<Number>::T tr(vertices[-1]);

    commands[0] = PATH_CMD_CUBIC_TO;
    commands[1] = PATH_CMD_CUBIC_TO;
    commands[2] = PATH_CMD_CUBIC_TO;

    vertices[0] = pt1 + tr;
    vertices[1] = pt2 + tr;
    vertices[2] = pt3 + tr;

    self._d->flat = 0;
    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

template<typename Number>
static err_t _G2d_PathT_smoothCubicTo(
  typename PathT<Number>::T& self,
  const typename PointT<Number>::T& pt2,
  const typename PointT<Number>::T& pt3)
{
  PATH_ADD_VERTEX_BEGIN(Number, 3)
    typename PointT<Number>::T pt1(vertices[-1]);

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

    self._d->flat = 0;
    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

template<typename Number>
static err_t _G2d_PathT_smoothCubicToRel(
  typename PathT<Number>::T& self,
  const typename PointT<Number>::T& pt2,
  const typename PointT<Number>::T& pt3)
{
  PATH_ADD_VERTEX_BEGIN(Number, 3)
    typename PointT<Number>::T pt1(vertices[-1]);

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

    self._d->flat = 0;
    self._d->boundingBoxDirty = true;
  PATH_ADD_VERTEX_END()
}

// ============================================================================
// [Fog::Path - ArcTo]
// ============================================================================

template<typename Number>
static err_t _G2d_PathT_arcTo(
  typename PathT<Number>::T& self, 
  const typename PointT<Number>::T& cp, 
  const typename PointT<Number>::T& rp, 
  Number start, Number sweep, bool startPath)
{
  start = Math::mod(start, (Number)(2.0 * MATH_PI));

  if (sweep > (Number)( 2.0 * MATH_PI)) sweep = (Number)( 2.0 * MATH_PI);
  if (sweep < (Number)(-2.0 * MATH_PI)) sweep = (Number)(-2.0 * MATH_PI);

  uint8_t* commands;
  typename PointT<Number>::T* vertices;

  uint8_t initial = startPath ? PATH_CMD_MOVE_TO : PATH_CMD_LINE_TO;

  // Degenerated.
  if (Math::abs(sweep) < 1e-7)
  {
    Number aSin, aCos;

    sysuint_t pos = self._add(2);
    if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

    commands = self._d->commands + pos;
    vertices = self._d->vertices + pos;

    Math::sincos(start, &aSin, &aCos);
    vertices[0].set(cp.x + rp.x * aCos, cp.y + rp.y * aSin);
    commands[0] = initial;

    if (!(!startPath && pos > 0 && Fuzzy<typename PointT<Number>::T>::eq(vertices[-1], vertices[0])))
    {
      commands++;
      vertices++;
    }

    Math::sincos(start + sweep, &aSin, &aCos);
    commands[0] = PATH_CMD_LINE_TO;
    vertices[0].set(cp.x + rp.x * aCos, cp.y + rp.y * aSin);

    commands++;
    vertices++;
  }
  else
  {
    sysuint_t pos = self._add(13);
    if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

    commands = self._d->commands + pos;
    vertices = self._d->vertices + pos;

    Number totalSweep = Number(0.0);
    Number localSweep = Number(0.0);
    Number prevSweep;

    // 4 cubic curves are the maximum.
    int remain = 4;
    bool isFirst = true;

    do {
      if (sweep < 0.0)
      {
        prevSweep   = totalSweep;
        localSweep  = (Number)(-MATH_PI * 0.5);
        totalSweep -= (Number)( MATH_PI * 0.5);

        if (totalSweep <= sweep + Math2dConst<Number>::getAngleEpsilon())
        {
          localSweep = sweep - prevSweep;
          remain = 1;
        }
      }
      else
      {
        prevSweep   = totalSweep;
        localSweep  = (Number)(MATH_PI * 0.5);
        totalSweep += (Number)(MATH_PI * 0.5);

        if (totalSweep >= sweep - Math2dConst<Number>::getAngleEpsilon())
        {
          localSweep = sweep - prevSweep;
          remain = 1;
        }
      }

      if (isFirst)
      {
        _G2d_PathT_arcToBezier<Number>(cp, rp, start, localSweep, vertices);
        if (!startPath && pos > 0 && Fuzzy<typename PointT<Number>::T>::eq(vertices[-1], vertices[0]))
        {
          vertices[0] = vertices[1];
          vertices[1] = vertices[2];
          vertices[2] = vertices[3];
        }
        else
        {
          commands[0] = initial;
          commands++;
          vertices++;
        }
        isFirst = false;
      }
      else
      {
        _G2d_PathT_arcToBezier<Number>(cp, rp, start, localSweep, vertices - 1);
      }

      commands[0] = PATH_CMD_CUBIC_TO;
      commands[1] = PATH_CMD_CUBIC_TO;
      commands[2] = PATH_CMD_CUBIC_TO;

      commands += 3;
      vertices += 3;

      start += localSweep;
    } while (--remain);

    // The curves were added, the path is no longer flat.
    self._d->flat = 0;
  }

  // Fix path length and invalidate bounding box.
  self._d->length = (sysuint_t)(commands - self._d->commands);
  self._d->boundingBoxDirty = true;

  return ERR_OK;
}

template<typename Number>
static err_t _G2d_PathT_arcToRel(
  typename PathT<Number>::T& self, 
  const typename PointT<Number>::T& cp, 
  const typename PointT<Number>::T& r, 
  Number start, Number sweep, bool startPath)
{
  typename PointT<Number>::T tr;
  if (!_G2d_PathT_getLastPoint<Number>(self._d, tr)) return ERR_PATH_NO_RELATIVE;

  return self.arcTo(cp + tr, r, start, sweep, startPath);
}

template<typename Number>
static err_t _G2d_PathT_svgArcTo(
  typename PathT<Number>::T& self, 
  const typename PointT<Number>::T& rp, 
  Number angle, bool largeArcFlag, bool sweepFlag, 
  const typename PointT<Number>::T& p2)
{
  // Mark current length (will be position where the first bezier would start).
  sysuint_t mark = self._d->length;

  typename PointT<Number>::T p0;
  bool radiiOk = true;

  // Get initial point - p0.
  if (!_G2d_PathT_getLastPoint<Number>(self._d, p0)) return ERR_PATH_NO_RELATIVE;

  // Normalize radius.
  Number rx = (rp.x >= Number(0.0)) ? rp.x : -rp.x;
  Number ry = (rp.y >= Number(0.0)) ? rp.y : -rp.y;

  // Calculate the middle point between the current and the final points.
  Number dx2 = (p0.x - p2.x) * Number(0.5);
  Number dy2 = (p0.y - p2.y) * Number(0.5);

  Number aSin, aCos;
  Math::sincos(angle, &aSin, &aCos);

  // Calculate middle point - p1.
  typename PointT<Number>::T p1;
  p1.x =  aCos * dx2 + aSin * dy2;
  p1.y = -aSin * dx2 + aCos * dy2;

  // Ensure radii are large enough.
  Number rx_2 = rx * rx;
  Number ry_2 = ry * ry;
  Number p1x_2 = p1.x * p1.x;
  Number p1y_2 = p1.y * p1.y;

  // Check that radii are large enough.
  Number radiiCheck = p1x_2 / rx_2 + p1y_2 / ry_2;

  if (radiiCheck > Number(1.0))
  {
    Number s = Math::sqrt(radiiCheck);
    rx *= s;
    ry *= s;
    rx_2 = Math::pow2(rx);
    ry_2 = Math::pow2(ry);
    if (radiiCheck > Number(10.0)) radiiOk = false;
  }

  // Calculate (cx1, cy1).
  Number sign = (largeArcFlag == sweepFlag) ? -Number(1.0) : Number(1.0);
  Number sq   = (rx_2 * ry_2  - rx_2 * p1y_2 - ry_2 * p1x_2) / 
               (rx_2 * p1y_2 + ry_2 * p1x_2);
  Number coef = sign * (sq <= Number(0.0) ? Number(0.0) : Math::sqrt(sq));

  typename PointT<Number>::T cp(coef *  ((rx * p1.y) / ry), coef * -((ry * p1.x) / rx));

  // Calculate (cx, cy) from (cx1, cy1).
  Number sx2 = (p0.x + p2.x) * Number(0.5);
  Number sy2 = (p0.y + p2.y) * Number(0.5);
  Number cx = sx2 + (aCos * cp.x - aSin * cp.y);
  Number cy = sy2 + (aSin * cp.x + aCos * cp.y);

  // Calculate the start_angle (angle1) and the sweep_angle (dangle).
  Number ux = ( p1.x - cp.x) / rx;
  Number uy = ( p1.y - cp.y) / ry;
  Number vx = (-p1.x - cp.x) / rx;
  Number vy = (-p1.y - cp.y) / ry;
  Number p, n;

  // Calculate the angle start.
  n = Math::sqrt(ux * ux + uy * uy);
  p = ux; // (1 * ux) + (0 * uy)
  sign = (uy < Number(0.0)) ? -Number(1.0) : Number(1.0);

  Number v = p / n;
  if (v < -Number(1.0)) v = -Number(1.0);
  if (v >  Number(1.0)) v =  Number(1.0);
  Number startAngle = sign * Math::acos(v);

  // Calculate the sweep angle.
  n = Math::sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
  p = ux * vx + uy * vy;
  sign = (ux * vy - uy * vx < 0) ? -Number(1.0) : Number(1.0);
  v = p / n;
  if (v < -Number(1.0)) v = -Number(1.0);
  if (v >  Number(1.0)) v =  Number(1.0);
  Number sweepAngle = sign * Math::acos(v);

  if (!sweepFlag && sweepAngle > 0)
    sweepAngle -= (Number)(MATH_PI * 2.0);
  else if (sweepFlag && sweepAngle < 0)
    sweepAngle += (Number)(MATH_PI * 2.0);

  FOG_RETURN_ON_ERROR(
    self.arcTo(typename PointT<Number>::T(Number(0.0), Number(0.0)), typename PointT<Number>::T(rx, ry), startAngle, sweepAngle, false)
  );

  // If no error was reported then _arcTo had to add almost two vertices, for
  // matrix transform and fixing the end point we need almost one.
  FOG_ASSERT(self._d->length > 0);

  // We can now transform the resulting arc.
  {
    typename TransformT<Number>::T transform = TransformT<Number>::T::fromRotation(angle);
    transform.translate(typename PointT<Number>::T(cx, cy), MATRIX_ORDER_APPEND);

    typename PointT<Number>::T* pts = self._d->vertices + mark;
    transform.mapPoints(pts, pts, self._d->length - mark);
  }

  // We must make sure that the starting and ending points exactly coincide
  // with the initial p0 and p2.
  {
    typename PointT<Number>::T* vertex = self._d->vertices;
    vertex[mark].x = p0.x;
    vertex[mark].y = p0.y;
    vertex[self._d->length - 1].x = p2.x;
    vertex[self._d->length - 1].y = p2.y;
  }

  // Bounding box is no longer valid.
  self._d->boundingBoxDirty = true;
  return ERR_OK;
}

template<typename Number>
static err_t _G2d_PathT_svgArcToRel(
  typename PathT<Number>::T& self, 
  const typename PointT<Number>::T& rp, 
  Number angle, bool largeArcFlag, bool sweepFlag, 
  const typename PointT<Number>::T& pt)
{
  typename PointT<Number>::T last;
  if (!_G2d_PathT_getLastPoint<Number>(self._d, last)) return ERR_PATH_NO_RELATIVE;

  return self.svgArcTo(rp, angle, largeArcFlag, sweepFlag, pt + last);
}

// ============================================================================
// [Fog::Path - Close]
// ============================================================================

template<typename Number>
static err_t _G2d_PathT_close(typename PathT<Number>::T& self)
{
  PATH_ADD_VERTEX_BEGIN(Number, 1)
    commands[0] = PATH_CMD_CLOSE;
    vertices[0].set(Math::getQNanT<Number>(), Math::getQNanT<Number>());
  PATH_ADD_VERTEX_END()
}

// ============================================================================
// [Fog::Path - Rect / Rects]
// ============================================================================

template<typename Number, typename Param>
static err_t _G2d_PathT_rectT(
  typename PathT<Number>::T& self,
  const typename RectT<Param>::T& r, uint32_t direction)
{
  if (!r.isValid()) return ERR_OK;

  sysuint_t pos = self._add(5);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  typename PointT<Number>::T* vertices = self._d->vertices + pos;

  Number x0 = (Number)r.getX0();
  Number y0 = (Number)r.getY0();
  Number x1 = (Number)r.getX1();
  Number y1 = (Number)r.getY1();

  commands[0] = PATH_CMD_MOVE_TO;
  commands[1] = PATH_CMD_LINE_TO;
  commands[2] = PATH_CMD_LINE_TO;
  commands[3] = PATH_CMD_LINE_TO;
  commands[4] = PATH_CMD_CLOSE;

  vertices[0].x = x0;
  vertices[0].y = y0;
  vertices[2].x = x1;
  vertices[2].y = y1;
  vertices[4].x = Math::getQNanT<Number>();
  vertices[4].y = Math::getQNanT<Number>();

  if (direction == PATH_DIRECTION_CW)
  {
    vertices[1].x = x1;
    vertices[1].y = y0;
    vertices[3].x = x0;
    vertices[3].y = y1;
  }
  else
  {
    vertices[1].x = x0;
    vertices[1].y = y1;
    vertices[3].x = x1;
    vertices[3].y = y0;
  }

  if (!self._d->boundingBoxDirty)
  {
    if (x0 < self._d->boundingBox.x0) self._d->boundingBox.x0 = x0;
    if (x1 > self._d->boundingBox.x1) self._d->boundingBox.x1 = x1;

    if (y0 < self._d->boundingBox.y0) self._d->boundingBox.y0 = y0;
    if (y1 > self._d->boundingBox.y1) self._d->boundingBox.y1 = y1;
  }

  return ERR_OK;
}

template<typename Number, typename Param>
static err_t _G2d_PathT_boxT(
  typename PathT<Number>::T& self,
  const typename BoxT<Param>::T& r, uint32_t direction)
{
  if (!r.isValid()) return ERR_OK;

  sysuint_t pos = self._add(5);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  typename PointT<Number>::T* vertices = self._d->vertices + pos;

  Number x0 = (Number)r.getX0();
  Number y0 = (Number)r.getY0();
  Number x1 = (Number)r.getX1();
  Number y1 = (Number)r.getY1();

  commands[0] = PATH_CMD_MOVE_TO;
  commands[1] = PATH_CMD_LINE_TO;
  commands[2] = PATH_CMD_LINE_TO;
  commands[3] = PATH_CMD_LINE_TO;
  commands[4] = PATH_CMD_CLOSE;

  vertices[0].x = x0;
  vertices[0].y = y0;
  vertices[2].x = x1;
  vertices[2].y = y1;
  vertices[4].x = Math::getQNanT<Number>();
  vertices[4].y = Math::getQNanT<Number>();

  if (direction == PATH_DIRECTION_CW)
  {
    vertices[1].x = x1;
    vertices[1].y = y0;
    vertices[3].x = x0;
    vertices[3].y = y1;
  }
  else
  {
    vertices[1].x = x0;
    vertices[1].y = y1;
    vertices[3].x = x1;
    vertices[3].y = y0;
  }

  if (!self._d->boundingBoxDirty)
  {
    if (x0 < self._d->boundingBox.x0) self._d->boundingBox.x0 = x0;
    if (x1 > self._d->boundingBox.x1) self._d->boundingBox.x1 = x1;

    if (y0 < self._d->boundingBox.y0) self._d->boundingBox.y0 = y0;
    if (y1 > self._d->boundingBox.y1) self._d->boundingBox.y1 = y1;
  }

  return ERR_OK;
}

template<typename Number, typename Param>
static err_t _G2d_PathT_boxesT(
  typename PathT<Number>::T& self,
  const typename BoxT<Param>::T* r, sysuint_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(r);

  sysuint_t pos = self._add(count * 5);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  sysuint_t i;
  sysuint_t added = 0;
  typename PointT<Number>::T* vertices = self._d->vertices + pos;

  if (direction == PATH_DIRECTION_CW)
  {
    for (i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      Number x0 = (Number)r->getX0();
      Number y0 = (Number)r->getY0();
      Number x1 = (Number)r->getX1();
      Number y1 = (Number)r->getY1();

      vertices[0].x = x0;
      vertices[0].y = y0;
      vertices[1].x = x1;
      vertices[1].y = y0;
      vertices[2].x = x1;
      vertices[2].y = y1;
      vertices[3].x = x0;
      vertices[3].y = y1;
      vertices[4].x = Math::getQNanT<Number>();
      vertices[4].y = Math::getQNanT<Number>();

      vertices += 5;
      added++;
    }
  }
  else
  {
    for (i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      Number x0 = (Number)r->getX0();
      Number y0 = (Number)r->getY0();
      Number x1 = (Number)r->getX1();
      Number y1 = (Number)r->getY1();

      vertices[0].x = x0;
      vertices[0].y = y0;
      vertices[1].x = x0;
      vertices[1].y = y1;
      vertices[2].x = x1;
      vertices[2].y = y1;
      vertices[3].x = x1;
      vertices[3].y = y0;
      vertices[4].x = Math::getQNanT<Number>();
      vertices[4].y = Math::getQNanT<Number>();

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

  // Bounding box is no longer valid.
  self._d->boundingBoxDirty = true;

  // Update path length (some rectangles may be invalid) and return.
  self._d->length = (sysuint_t)(commands - self._d->commands);
  return ERR_OK;
}

template<typename Number, typename Param>
static err_t _G2d_PathT_rectsT(
  typename PathT<Number>::T& self,
  const typename RectT<Param>::T* r, sysuint_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(r);

  sysuint_t pos = self._add(count * 5);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  sysuint_t i;
  sysuint_t added = 0;
  typename PointT<Number>::T* vertices = self._d->vertices + pos;

  if (direction == PATH_DIRECTION_CW)
  {
    for (i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      Number x0 = (Number)r->x;
      Number y0 = (Number)r->y;
      Number x1 = x0 + (Number)r->w;
      Number y1 = y0 + (Number)r->h;

      vertices[0].x = x0;
      vertices[0].y = y0;
      vertices[1].x = x1;
      vertices[1].y = y0;
      vertices[2].x = x1;
      vertices[2].y = y1;
      vertices[3].x = x0;
      vertices[3].y = y1;
      vertices[4].x = Math::getQNanT<Number>();
      vertices[4].y = Math::getQNanT<Number>();

      vertices += 5;
      added++;
    }
  }
  else
  {
    for (i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      Number x0 = (Number)r->x;
      Number y0 = (Number)r->y;
      Number x1 = x0 + (Number)r->w;
      Number y1 = y0 + (Number)r->h;

      vertices[0].x = x0;
      vertices[0].y = y0;
      vertices[1].x = x0;
      vertices[1].y = y1;
      vertices[2].x = x1;
      vertices[2].y = y1;
      vertices[3].x = x1;
      vertices[3].y = y0;
      vertices[4].x = Math::getQNanT<Number>();
      vertices[4].y = Math::getQNanT<Number>();

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

  // Bounding box is no longer valid.
  self._d->boundingBoxDirty = true;

  // Update path length (some rectangles may be invalid) and return.
  self._d->length = (sysuint_t)(commands - self._d->commands);
  return ERR_OK;
}

// ============================================================================
// [Fog::Path - Region]
// ============================================================================

template<typename Number>
static err_t _G2d_PathT_region(typename PathT<Number>::T& self, const Region& r, uint32_t direction)
{
  return self.boxes(r.getData(), r.getLength(), direction);
}

// ============================================================================
// [Fog::Path - Polyline / Polygon]
// ============================================================================

template<typename Number>
static err_t _G2d_PathT_polylineI(typename PathT<Number>::T& self, const PointI* pts, sysuint_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(pts);

  sysuint_t i;
  sysuint_t pos = self._add(count);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  typename PointT<Number>::T* vertices = self._d->vertices + pos;

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

  // Bounding box is no longer valid.
  self._d->boundingBoxDirty = true;

  return ERR_OK;
}

template<typename Number>
static err_t _G2d_PathT_polylineT(typename PathT<Number>::T& self, const typename PointT<Number>::T* pts, sysuint_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(pts);

  sysuint_t i;
  sysuint_t pos = self._add(count);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  typename PointT<Number>::T* vertices = self._d->vertices + pos;

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

  // Bounding box is no longer valid.
  self._d->boundingBoxDirty = true;

  return ERR_OK;
}

template<typename Number>
static err_t _G2d_PathT_polygonI(typename PathT<Number>::T& self, const PointI* pts, sysuint_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(pts);

  sysuint_t i;
  sysuint_t pos = self._add(count + 1);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  typename PointT<Number>::T* vertices = self._d->vertices + pos;

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

  vertices[count].set(Math::getQNanT<Number>(),
                      Math::getQNanT<Number>());

  // Bounding box is no longer valid.
  self._d->boundingBoxDirty = true;

  return ERR_OK;
}

template<typename Number>
static err_t _G2d_PathT_polygonT(typename PathT<Number>::T& self, const typename PointT<Number>::T* pts, sysuint_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(pts);

  sysuint_t i;
  sysuint_t pos = self._add(count + 1);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = self._d->commands + pos;
  typename PointT<Number>::T* vertices = self._d->vertices + pos;

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

  vertices[count].set(Math::getQNanT<Number>(), 
                      Math::getQNanT<Number>());

  // Bounding box is no longer valid.
  self._d->boundingBoxDirty = true;

  return ERR_OK;
}

// ============================================================================
// [Fog::Path - Shape]
// ============================================================================

#define KAPPA (4.0 * (MATH_SQRT2 - 1.0) / 3.0)

template<typename Number>
static err_t _G2d_PathT_shape(
  typename PathT<Number>::T& self,
  uint32_t shapeType, const void* shapeData, uint32_t direction, 
  const typename TransformT<Number>::T* tr)
{
  sysuint_t len = self._d->length;
  sysuint_t pos;

  err_t err = ERR_OK;

  switch (shapeType)
  {
    case SHAPE_TYPE_NONE:
    {
      return ERR_OK;
    }

    // --------------------------------------------------------------------------
    // [Unclosed]
    // --------------------------------------------------------------------------

    case SHAPE_TYPE_LINE:
    {
      const LineF* data = reinterpret_cast<const LineF*>(shapeData);
      if ((pos = self._add(2)) == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

      uint8_t* commands = self._d->commands + pos;
      typename PointT<Number>::T* vertices = self._d->vertices + pos;

      commands[0] = PATH_CMD_MOVE_TO;
      commands[1] = PATH_CMD_LINE_TO;

      if (direction == PATH_DIRECTION_CW)
      {
        vertices[0] = data->p[0];
        vertices[1] = data->p[1];
      }
      else
      {
        vertices[0] = data->p[1];
        vertices[1] = data->p[0];
      }

      self._d->boundingBoxDirty = true;
      break;
    }

    case SHAPE_TYPE_QUAD:
    {
      const QuadCurveF* data = reinterpret_cast<const QuadCurveF*>(shapeData);
      if ((pos = self._add(3)) == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

      uint8_t* commands = self._d->commands + pos;
      typename PointT<Number>::T* vertices = self._d->vertices + pos;

      commands[0] = PATH_CMD_MOVE_TO;
      commands[1] = PATH_CMD_QUAD_TO;
      commands[2] = PATH_CMD_QUAD_TO;

      if (direction == PATH_DIRECTION_CW)
      {
        vertices[0] = data->p[0];
        vertices[1] = data->p[1];
        vertices[2] = data->p[2];
      }
      else
      {
        vertices[0] = data->p[2];
        vertices[1] = data->p[1];
        vertices[2] = data->p[0];
      }

      self._d->flat = 0;
      self._d->boundingBoxDirty = true;
      break;
    }

    case SHAPE_TYPE_CUBIC:
    {
      const CubicCurveF* data = reinterpret_cast<const CubicCurveF*>(shapeData);
      if ((pos = self._add(4)) == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

      uint8_t* commands = self._d->commands + pos;
      typename PointT<Number>::T* vertices = self._d->vertices + pos;

      commands[0] = PATH_CMD_MOVE_TO;
      commands[1] = PATH_CMD_CUBIC_TO;
      commands[2] = PATH_CMD_CUBIC_TO;
      commands[3] = PATH_CMD_CUBIC_TO;

      if (direction == PATH_DIRECTION_CW)
      {
        vertices[0] = data->p[0];
        vertices[1] = data->p[1];
        vertices[2] = data->p[2];
        vertices[3] = data->p[3];
      }
      else
      {
        vertices[0] = data->p[3];
        vertices[1] = data->p[2];
        vertices[2] = data->p[1];
        vertices[3] = data->p[0];
      }

      self._d->flat = 0;
      self._d->boundingBoxDirty = true;
      break;
    }

    case SHAPE_TYPE_ARC:
    {
      const ArcF* data = reinterpret_cast<const ArcF*>(shapeData);

      Number start = data->start;
      Number sweep = data->sweep;
      if (direction != PATH_DIRECTION_CW) { start += sweep; sweep = -sweep; }

      if ((err = self.arcTo(data->center, data->radius, start, sweep, true)) != ERR_OK) goto _Fail;
      break;
    }

    // --------------------------------------------------------------------------
    // [Closed]
    // --------------------------------------------------------------------------

    case SHAPE_TYPE_RECT:
    {
_ShapeRect:
      const typename RectT<Number>::T* data = reinterpret_cast<const typename RectT<Number>::T*>(shapeData);
      if ((err = self.rect(*data, direction)) != ERR_OK) goto _Fail;
      break;
    }

    case SHAPE_TYPE_ROUND:
    {
      const typename RoundT<Number>::T* data = reinterpret_cast<const typename RoundT<Number>::T*>(shapeData);

      const typename RectT<Number>::T& r = data->rect;
      if (!r.isValid()) return ERR_OK;

      Number rx = Math::abs(data->radius.x);
      Number ry = Math::abs(data->radius.y);

      Number rxKappaInv = rx * (Number)(1.0 - KAPPA);
      Number ryKappaInv = ry * (Number)(1.0 - KAPPA);

      Number rw2 = r.w * Number(0.5);
      Number rh2 = r.h * Number(0.5);

      if (rx > rw2) rx = rw2;
      if (ry > rh2) ry = rh2;
      if (Math::isFuzzyZero(rx) || Math::isFuzzyZero(ry)) goto _ShapeRect;

      Number x0 = r.x;
      Number y0 = r.y;
      Number x1 = r.x + r.w;
      Number y1 = r.y + r.h;

      bool boundingBoxDirty = self._d->boundingBoxDirty;
      typename BoxT<Number>::T oldBoundingBox = self._d->boundingBox;

      sysuint_t pos = self._add(18);
      if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

      uint8_t* commands = self._d->commands + pos;
      typename PointT<Number>::T* vertices = self._d->vertices + pos;

      if (direction == PATH_DIRECTION_CW)
      {
        commands[ 0] = PATH_CMD_MOVE_TO;

        commands[ 1] = PATH_CMD_LINE_TO;
        commands[ 2] = PATH_CMD_CUBIC_TO;
        commands[ 3] = PATH_CMD_CUBIC_TO;
        commands[ 4] = PATH_CMD_CUBIC_TO;

        commands[ 5] = PATH_CMD_LINE_TO;
        commands[ 6] = PATH_CMD_CUBIC_TO;
        commands[ 7] = PATH_CMD_CUBIC_TO;
        commands[ 8] = PATH_CMD_CUBIC_TO;

        commands[ 9] = PATH_CMD_LINE_TO;
        commands[10] = PATH_CMD_CUBIC_TO;
        commands[11] = PATH_CMD_CUBIC_TO;
        commands[12] = PATH_CMD_CUBIC_TO;

        commands[13] = PATH_CMD_LINE_TO;
        commands[14] = PATH_CMD_CUBIC_TO;
        commands[15] = PATH_CMD_CUBIC_TO;
        commands[16] = PATH_CMD_CUBIC_TO;

        commands[17] = PATH_CMD_CLOSE;

        vertices[ 0].set(x0 + rx        , y0             );

        vertices[ 1].set(x1 - rx        , y0             );
        vertices[ 2].set(x1 - rxKappaInv, y0             );
        vertices[ 3].set(x1             , y0 + ryKappaInv);
        vertices[ 4].set(x1             , y0 + ry        );

        vertices[ 5].set(x1             , y1 - ry        );
        vertices[ 6].set(x1             , y1 - ryKappaInv);
        vertices[ 7].set(x1 - rxKappaInv, y1             );
        vertices[ 8].set(x1 - rx        , y1             );

        vertices[ 9].set(x0 + rx        , y1             );
        vertices[10].set(x0 + rxKappaInv, y1             );
        vertices[11].set(x0             , y1 - ryKappaInv);
        vertices[12].set(x0             , y1 - ry        );

        vertices[13].set(x0             , y0 + ry        );
        vertices[14].set(x0             , y0 + ryKappaInv);
        vertices[15].set(x0 + rxKappaInv, y0             );
        vertices[16].set(x0 + rx        , y0);

        vertices[17].set(Math::getQNanT<Number>(), Math::getQNanT<Number>());
      }
      else
      {
        commands[ 0] = PATH_CMD_MOVE_TO;
        commands[ 1] = PATH_CMD_CUBIC_TO;
        commands[ 2] = PATH_CMD_CUBIC_TO;
        commands[ 3] = PATH_CMD_CUBIC_TO;

        commands[ 4] = PATH_CMD_LINE_TO;
        commands[ 5] = PATH_CMD_CUBIC_TO;
        commands[ 6] = PATH_CMD_CUBIC_TO;
        commands[ 7] = PATH_CMD_CUBIC_TO;

        commands[ 8] = PATH_CMD_LINE_TO;
        commands[ 9] = PATH_CMD_CUBIC_TO;
        commands[10] = PATH_CMD_CUBIC_TO;
        commands[11] = PATH_CMD_CUBIC_TO;

        commands[12] = PATH_CMD_LINE_TO;
        commands[13] = PATH_CMD_CUBIC_TO;
        commands[14] = PATH_CMD_CUBIC_TO;
        commands[15] = PATH_CMD_CUBIC_TO;

        commands[16] = PATH_CMD_CLOSE;

        vertices[ 0].set(x0 + rx        , y0             );
        vertices[ 1].set(x0 + rxKappaInv, y0             );
        vertices[ 2].set(x0             , y0 + ryKappaInv);
        vertices[ 3].set(x0             , y0 + ry        );

        vertices[ 4].set(x0             , y1 - ry        );
        vertices[ 5].set(x0             , y1 - ryKappaInv);
        vertices[ 6].set(x0 + rxKappaInv, y1             );
        vertices[ 7].set(x0 + rx        , y1             );

        vertices[ 8].set(x1 - rx        , y1             );
        vertices[ 9].set(x1 - rxKappaInv, y1             );
        vertices[10].set(x1             , y1 - ryKappaInv);
        vertices[11].set(x1             , y1 - ry        );

        vertices[12].set(x1             , y0 + ry        );
        vertices[13].set(x1             , y0 + ryKappaInv);
        vertices[14].set(x1 - rxKappaInv, y0             );
        vertices[15].set(x1 - rx        , y0             );

        vertices[16].set(Math::getQNanT<Number>(), Math::getQNanT<Number>());
        self._d->length--;
      }

      if (len == 0 || !boundingBoxDirty)
      {
        self._d->boundingBox.set(x0, y0, x1, y1);

        if (len > 0) BoxT<Number>::T::bound(self._d->boundingBox, self._d->boundingBox, oldBoundingBox);
        boundingBoxDirty = false;
      }

      self._d->flat = 0;
      self._d->boundingBoxDirty = boundingBoxDirty;
      break;
    }

    case SHAPE_TYPE_CIRCLE:
    case SHAPE_TYPE_ELLIPSE:
    {
      Number rx, rxKappa;
      Number ry, ryKappa;

      typename PointT<Number>::T c;

      if (shapeType == SHAPE_TYPE_CIRCLE)
      {
        const CircleF* data = reinterpret_cast<const CircleF*>(shapeData);
        c = data->center;
        rx = data->radius;
        ry = Math::abs(rx);
      }
      else
      {
        const EllipseF* data = reinterpret_cast<const EllipseF*>(shapeData);
        c = data->center;
        rx = data->radius.x;
        ry = data->radius.y;
      }

      bool boundingBoxDirty = self._d->boundingBoxDirty;
      typename BoxT<Number>::T oldBoundingBox = self._d->boundingBox;

      sysuint_t pos = self._add(14);
      if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

      uint8_t* commands = self._d->commands + pos;
      typename PointT<Number>::T* vertices  = self._d->vertices + pos;

      if (direction == PATH_DIRECTION_CCW) rx = -rx;

      rxKappa = rx * (Number)(KAPPA);
      ryKappa = ry * (Number)(KAPPA);

      commands[ 0] = PATH_CMD_MOVE_TO;
      commands[ 1] = PATH_CMD_CUBIC_TO;
      commands[ 2] = PATH_CMD_CUBIC_TO;
      commands[ 3] = PATH_CMD_CUBIC_TO;
      commands[ 4] = PATH_CMD_CUBIC_TO;
      commands[ 5] = PATH_CMD_CUBIC_TO;
      commands[ 6] = PATH_CMD_CUBIC_TO;
      commands[ 7] = PATH_CMD_CUBIC_TO;
      commands[ 8] = PATH_CMD_CUBIC_TO;
      commands[ 9] = PATH_CMD_CUBIC_TO;
      commands[10] = PATH_CMD_CUBIC_TO;
      commands[11] = PATH_CMD_CUBIC_TO;
      commands[12] = PATH_CMD_CUBIC_TO;
      commands[13] = PATH_CMD_CLOSE;

      vertices[ 0].set(c.x          , c.y - ry     );
      vertices[ 1].set(c.x + rxKappa, c.y - ry     );
      vertices[ 2].set(c.x + rx     , c.y - ryKappa);
      vertices[ 3].set(c.x + rx     , c.y          );
      vertices[ 4].set(c.x + rx     , c.y + ryKappa);
      vertices[ 5].set(c.x + rxKappa, c.y + ry     );
      vertices[ 6].set(c.x          , c.y + ry     );
      vertices[ 7].set(c.x - rxKappa, c.y + ry     );
      vertices[ 8].set(c.x - rx     , c.y + ryKappa);
      vertices[ 9].set(c.x - rx     , c.y          );
      vertices[10].set(c.x - rx     , c.y - ryKappa);
      vertices[11].set(c.x - rxKappa, c.y - ry     );
      vertices[12].set(c.x          , c.y - ry     );

      vertices[13].set(Math::getQNanT<Number>(), Math::getQNanT<Number>());

      if (len == 0 || !boundingBoxDirty)
      {
        rx = Math::abs(rx);
        ry = Math::abs(ry);

        self._d->boundingBox.set(c.x - rx, c.y - ry, c.x + rx, c.y + ry);

        if (len > 0)
          BoxT<Number>::T::bound(self._d->boundingBox, self._d->boundingBox, oldBoundingBox);
        boundingBoxDirty = false;
      }

      self._d->boundingBoxDirty = boundingBoxDirty;
      break;
    }

    case SHAPE_TYPE_CHORD:
    case SHAPE_TYPE_PIE:
    {
      const ArcF* data = reinterpret_cast<const ArcF*>(shapeData);

      Number start = data->start;
      Number sweep = data->sweep;
      if (direction != PATH_DIRECTION_CW) { start += sweep; sweep = -sweep; }

      bool startPath = shapeType == SHAPE_TYPE_CHORD;
      if (!startPath && (err = self.moveTo(data->center)) != ERR_OK) goto _Fail;

      if ((err = self.arcTo(data->center, data->radius, start, sweep, startPath)) != ERR_OK) goto _Fail;
      if ((err = self.close()) != ERR_OK) goto _Fail;

      self._d->boundingBoxDirty = true;
      break;
    }

    default:
    {
      return ERR_RT_INVALID_ARGUMENT;
    }
  }

  // Transform if matrix provided.
  if (tr)
  {
    tr->mapPoints(self._d->vertices + len, self._d->vertices + len, self._d->length - len);
    self._d->boundingBoxDirty = true;
  }

  _FOG_PATH_VERIFY_BOUNDING_BOX(self);
  return ERR_OK;

_Fail:
  // Restore path to previous state.
  if (self._d->length != len) self._d->length = len;
  return err;
}

// ============================================================================
// [Fog::Path - Append]
// ============================================================================

template<typename Number, typename Param>
static err_t _G2d_PathT_appendPathT(
  typename PathT<Number>::T& self,
  const typename PathT<Param>::T& path,
  const Range* range)
{
  sysuint_t srcPos = 0;
  sysuint_t srcLen = path.getLength();

  if (range != NULL)
  {
    srcPos = range->getStart();
    srcLen = Math::min(range->getEnd(), srcLen);
    if (srcPos > srcLen) return ERR_RT_INVALID_ARGUMENT;
    srcLen -= srcPos;
  }
  if (srcLen == 0) return ERR_OK;

  sysuint_t pos = self._add(srcLen);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  Memory::copy(self._d->commands + pos, path._d->commands + srcPos, srcLen * sizeof(uint8_t));

  if (sizeof(Number) == sizeof(Param))
  {
    Memory::copy(self._d->vertices + pos, path._d->vertices + srcPos, srcLen * sizeof(typename PointT<Number>::T));
  }
  else
  {
    for (sysuint_t i = 0; i < srcLen; i++) self._d->vertices[pos + i] = path._d->vertices[srcPos + i];
  }

  if (range == NULL && !(self._d->boundingBoxDirty | path._d->boundingBoxDirty))
  {
    Number x, y;

    x = (Number)path._d->boundingBox.x0;
    y = (Number)path._d->boundingBox.y0;

    if (x < self._d->boundingBox.x0) self._d->boundingBox.x0 = x;
    if (y < self._d->boundingBox.y0) self._d->boundingBox.y0 = y;

    x = (Number)path._d->boundingBox.x1;
    y = (Number)path._d->boundingBox.y1;

    if (x > self._d->boundingBox.x1) self._d->boundingBox.x1 = x;
    if (y > self._d->boundingBox.y1) self._d->boundingBox.y1 = y;

    self._d->flat = ((self._d->flat | path._d->flat) < 2) ? Math::min(self._d->flat, path._d->flat) : 2;
  }
  else
  {
    self._d->boundingBoxDirty = true;
    self._d->flat = ((self._d->flat | path._d->flat) == 0) ? 0 : 2;
  }

  return ERR_OK;
}

template<typename Number>
static err_t _G2d_PathT_appendTransformedPathT(
  typename PathT<Number>::T& self,
  const typename PathT<Number>::T& path,
  const typename TransformT<Number>::T& tr,
  const Range* range)
{
  sysuint_t srcPos = 0;
  sysuint_t srcLen = path.getLength();

  if (range != NULL)
  {
    srcPos = range->getStart();
    srcLen = Math::min(range->getEnd(), srcLen);
    if (srcPos > srcLen) return ERR_RT_INVALID_ARGUMENT;
    srcLen -= srcPos;
  }
  if (srcLen == 0) return ERR_OK;

  sysuint_t pos = self._add(srcLen);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  Memory::copy(self._d->commands + pos, path._d->commands + srcPos, srcLen * sizeof(uint8_t));
  tr.mapPoints(self._d->vertices + pos, path._d->vertices + srcPos, srcLen);

  if (range == NULL && tr._type <= TRANSFORM_TYPE_SCALING && !(self._d->boundingBoxDirty | path._d->boundingBoxDirty))
  {
    typename BoxT<Number>::T box;
    tr.mapBox(box, path._d->boundingBox);

    if (box.x0 < self._d->boundingBox.x0) self._d->boundingBox.x0 = box.x0;
    if (box.y0 < self._d->boundingBox.y0) self._d->boundingBox.y0 = box.y0;

    if (box.x1 > self._d->boundingBox.x1) self._d->boundingBox.x1 = box.x1;
    if (box.y1 > self._d->boundingBox.y1) self._d->boundingBox.y1 = box.y1;

    self._d->flat = ((self._d->flat | path._d->flat) < 2) ? Math::min(self._d->flat, path._d->flat) : 2;
  }
  else
  {
    self._d->boundingBoxDirty = true;
    self._d->flat = ((self._d->flat | path._d->flat) == 0) ? 0 : 2;
  }
  return ERR_OK;
}

template<typename Number>
static err_t _G2d_PathT_appendTranslatedPathT(
  typename PathT<Number>::T& self,
  const typename PathT<Number>::T& path,
  const typename PointT<Number>::T& pt,
  const Range* range)
{
  // Build transform (fast).
  typename TransformT<Number>::T tr(UNINITIALIZED);
  tr._type = TRANSFORM_TYPE_TRANSLATION;
  tr._20 = pt.x;
  tr._21 = pt.y;

  return _G2d_PathT_appendTransformedPathT<Number>(self, path, tr, range);
}

// ============================================================================
// [Fog::Path - Transform]
// ============================================================================

template<typename Number>
static err_t _G2d_PathT_transform(
  typename PathT<Number>::T& self,
  const typename TransformT<Number>::T& tr, const Range* range)
{
  if (range == NULL)
  {
    FOG_RETURN_ON_ERROR(self.detach());

    tr.mapPoints(self._d->vertices, self._d->vertices, self._d->length);
    self._d->boundingBoxDirty = true;

    return ERR_OK;
  }
  else
  {
    sysuint_t length = self._d->length;

    sysuint_t start = range->getStart();
    sysuint_t end = range->getEnd();

    if (start >= length && start >= end)
      return ERR_RT_INVALID_ARGUMENT;
    if (end > length) end = length;

    FOG_RETURN_ON_ERROR(self.detach());

    tr.mapPoints(self._d->vertices + start, self._d->vertices + start, end - start);
    self._d->boundingBoxDirty = true;

    return ERR_OK;
  }
}

template<typename Number>
static err_t _G2d_PathT_translate(
  typename PathT<Number>::T& self,
  const typename PointT<Number>::T& pt, const Range* range)
{
  // Build transform (fast).
  typename TransformT<Number>::T tr(UNINITIALIZED);
  tr._type = TRANSFORM_TYPE_TRANSLATION;
  tr._20 = pt.x;
  tr._21 = pt.y;

  return _G2d_PathT_transform<Number>(self, tr, range);
}

template<typename Number>
static err_t _G2d_PathT_fitTo(
  typename PathT<Number>::T& self,
  const typename RectT<Number>::T& bounds)
{
  if (!bounds.isValid()) return ERR_RT_INVALID_ARGUMENT;

  typename RectT<Number>::T currentBounds = self.getBoundingRect();
  if (!currentBounds.isValid()) return ERR_OK;

  Number cx = currentBounds.x;
  Number cy = currentBounds.y;

  Number tx = bounds.x;
  Number ty = bounds.y;

  Number sx = bounds.w / currentBounds.w;
  Number sy = bounds.h / currentBounds.h;

  FOG_RETURN_ON_ERROR(self.detach());

  sysuint_t i, length = self._d->length;
  typename PointT<Number>::T* pts = self._d->vertices;

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

template<typename Number>
static err_t _G2d_PathT_scale(
  typename PathT<Number>::T& self,
  const typename PointT<Number>::T& pt, bool keepStartPos)
{
  sysuint_t i, len = self._d->length;
  if (!len) return ERR_OK;

  FOG_RETURN_ON_ERROR(self.detach());
  typename PointT<Number>::T* vertices = self._d->vertices;

  if (keepStartPos)
  {
    uint8_t* commands = self._d->commands;
    Number tx = Number(0.0);
    Number ty = Number(0.0);

    for (i = 0; i < len; i++)
    {
      if (PathCmd::isVertex(commands[i]))
      {
        tx = vertices[i].x;
        ty = vertices[i].y;
      }
    }

    typename PointT<Number>::T tr(tx - tx * pt.x, ty - ty * pt.y);
    for (i = 0; i < len; i++)
    {
      vertices[i].x *= pt.x;
      vertices[i].y *= pt.y;
      vertices[i].x += tr.x;
      vertices[i].y += tr.y;
    }

    if (!self._d->boundingBoxDirty)
    {
      typename BoxT<Number>::T& b = self._d->boundingBox;

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

    if (!self._d->boundingBoxDirty)
    {
      typename BoxT<Number>::T& b = self._d->boundingBox;

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

template<typename Number>
static err_t _G2d_PathT_flipX(
  typename PathT<Number>::T& self, Number x0, Number x1)
{
  sysuint_t i, len = self._d->length;
  if (!len) return ERR_OK;

  FOG_RETURN_ON_ERROR(self.detach());

  Number x = x0 + x1;
  typename PointT<Number>::T* vertices = self._d->vertices;

  for (i = 0; i < len; i++)
  {
    vertices[i].x = x - vertices[i].x;
  }

  if (!self._d->boundingBoxDirty)
  {
    Number xMin = x - self._d->boundingBox.x0;
    Number xMax = x - self._d->boundingBox.x1;
    if (xMax < xMin) swap(xMin, xMax);

    self._d->boundingBox.x0 = xMin;
    self._d->boundingBox.x1 = xMax;
  }

  return ERR_OK;
}

template<typename Number>
static err_t _G2d_PathT_flipY(
  typename PathT<Number>::T& self, Number y0, Number y1)
{
  sysuint_t i, len = self._d->length;
  if (!len) return ERR_OK;

  FOG_RETURN_ON_ERROR(self.detach());

  Number y = y0 + y1;
  typename PointT<Number>::T* vertices = self._d->vertices;

  for (i = 0; i < len; i++)
  {
    vertices[i].y = y - vertices[i].y;
  }

  if (!self._d->boundingBoxDirty)
  {
    Number yMin = y - self._d->boundingBox.y0;
    Number yMax = y - self._d->boundingBox.y1;
    if (yMax < yMin) swap(yMin, yMax);

    self._d->boundingBox.y0 = yMin;
    self._d->boundingBox.y1 = yMax;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Path - Flatten]
// ============================================================================

template<typename Number>
static err_t _G2d_PathT_doFlatten(
  typename PathT<Number>::T& dst, 
  const uint8_t* commands, const typename PointT<Number>::T* vertices, sysuint_t length,
  const typename PathFlattenParamsT<Number>::T& params)
{
  if (!length) return ERR_OK;

  const typename TransformT<Number>::T* tr = params.getTransform();
  Number flatness = params.getFlatness();

  sysuint_t i = 0;
  sysuint_t dstInitial = dst.getLength();

  uint8_t c;
  err_t err = ERR_OK;

  // Minimal count of vertices added to the dst path is 'length'. We assume
  // that there are curves in the source path so length is multiplied by 4
  // and small constant is added to ensure that we don't reallocate if a given
  // path is small, but contains curves.
  sysuint_t predict = dstInitial + (length * 4U) + 128;

  // Overflow?
  if (predict < dstInitial) predict = dstInitial;
  FOG_RETURN_ON_ERROR(dst.reserve(predict));

  // Set destination path as flat.
  dst._d->flat = 1;

  if (tr == NULL || tr->getType() == TRANSFORM_TYPE_IDENTITY)
  {
    for (;;)
    {
      // Collect 'move-to' and 'line-to' commands.
      while (i < length)
      {
        c = commands[i];
        if (PathCmd::isMoveOrLineTo(c)) i++;
        break;
      }

      // Invalid state if 'i' is zero (no 'move-to' or 'line-to').
      if (i == 0) goto _InvalidState;

      // Include 'close' command if used.
      if (PathCmd::isClose(c)) i++;

      // Copy commands and vertices to the destination path.
      {
        sysuint_t pos = dst._add(i);
        if (pos == INVALID_INDEX) goto _OutOfMemory;

        memcpy(dst._d->commands + pos, commands, i * sizeof(uint8_t));
        memcpy(dst._d->vertices + pos, vertices, i * sizeof(typename PointT<Number>::T ));
      }

      // Advance pointers.
      if ((length -= i) == 0) continue;
      commands += i;
      vertices += i;
      i = 0;

      // Closed polygon (no curves?)
      if (PathCmd::isClose(c)) continue;

      // Approximate 'quad-to' or 'cubic-to' commands.
      do {
        c = commands[i];
        if (PathCmd::isQuadTo(c))
        {
          if ((i += 2) > length) goto _InvalidState;

          err = QuadCurveT<Number>::T::flatten(vertices + i - 3, dst, PATH_CMD_LINE_TO, flatness);
          if (FOG_IS_ERROR(err)) goto _Fail;
        }
        else if (PathCmd::isCubicTo(c))
        {
          if ((i += 3) > length) goto _InvalidState;

          err = CubicCurveT<Number>::T::flatten(vertices + i - 4, dst, PATH_CMD_LINE_TO, flatness);
          if (FOG_IS_ERROR(err)) goto _Fail;
        }
        else
        {
          break;
        }
      } while (i < length);

      if (PathCmd::isClose(c)) { dst.close(); i++; }

      // Advance pointers.
      if ((length -= i) == 0) return ERR_OK;
      commands += i;
      vertices += i;
      i = 0;
    }
  }
  else
  {
    typename PointT<Number>::T pts[4];

    for (;;)
    {
      // Collect 'move-to' and 'line-to' commands.
      while (i < length)
      {
        c = commands[i];
        if (PathCmd::isMoveOrLineTo(c)) i++;
        break;
      }

      // Invalid state if 'i' is zero (no 'move-to' or 'line-to').
      if (i == 0) goto _InvalidState;

      // Include 'close' command if used.
      if (PathCmd::isClose(c)) i++;

      // Copy commands and vertices to the destination path.
      {
        sysuint_t pos = dst._add(i);
        if (pos == INVALID_INDEX) goto _OutOfMemory;

        memcpy(dst._d->commands + pos, commands, i * sizeof(uint8_t));
        tr->_mapPoints(dst._d->vertices + pos, vertices, i);
      }

      // Advance pointers.
      if ((length -= i) == 0) continue;
      commands += i;
      vertices += i;
      i = 0;

      // Closed polygon (no curves?)
      if (PathCmd::isClose(c)) continue;

      // Approximate 'quad-to' or 'cubic-to' commands.
      do {
        c = commands[i];
        if (PathCmd::isQuadTo(c))
        {
          if ((i += 2) > length) goto _InvalidState;

          tr->_mapPoints(pts, vertices + i - 3, 3);
          err = QuadCurveT<Number>::T::flatten(pts, dst, PATH_CMD_LINE_TO, flatness);
          if (FOG_IS_ERROR(err)) goto _Fail;
        }
        else if (PathCmd::isCubicTo(c))
        {
          if ((i += 3) > length) goto _InvalidState;

          tr->_mapPoints(pts, vertices + i - 4, 4);
          err = CubicCurveT<Number>::T::flatten(pts,dst, PATH_CMD_LINE_TO, flatness);
          if (FOG_IS_ERROR(err)) goto _Fail;
        }
        else
        {
          break;
        }
      } while (i < length);

      if (PathCmd::isClose(c)) { dst.close(); i++; }

      // Advance pointers.
      if ((length -= i) == 0) return ERR_OK;
      commands += i;
      vertices += i;
      i = 0;
    }
  }
  return ERR_OK;

_OutOfMemory:
  err = ERR_RT_OUT_OF_MEMORY;
  goto _Fail;

_InvalidState:
  err = ERR_PATH_INVALID;
  goto _Fail;

_Fail:
  if (dst._d->length != dstInitial) dst._d->length = dstInitial;
  return err;
}

template<typename Number>
static err_t _G2d_PathT_flatten(
  typename PathT<Number>::T& dst,
  const typename PathT<Number>::T& src,
  const typename PathFlattenParamsT<Number>::T& params,
  const Range* range)
{
  if (range == NULL)
  {
    if (dst._d == src._d)
    {
      typename PathT<Number>::T tmp;

      FOG_RETURN_ON_ERROR(
        _G2d_PathT_doFlatten<Number>(tmp, src.getCommands(), src.getVertices(), src.getLength(), params)
      );
      return dst.setPath(tmp);
    }
    else
    {
      dst.clear();
      return _G2d_PathT_doFlatten<Number>(dst, src.getCommands(), src.getVertices(), src.getLength(), params);
    }
  }
  else
  {
    sysuint_t start = range->getStart();
    sysuint_t end = Math::min(range->getEnd(), src.getLength());

    if (start >= end) return (start == 0) ? (err_t)ERR_OK : (err_t)ERR_RT_INVALID_ARGUMENT;
    sysuint_t len = start - end;

    if (dst._d == src._d)
    {
      typename PathT<Number>::T t;

      FOG_RETURN_ON_ERROR(
        _G2d_PathT_doFlatten<Number>(t, src.getCommands() + start, src.getVertices() + start, len, params)
      );
      return dst.setPath(t);
    }
    else
    {
      dst.clear();

      return _G2d_PathT_doFlatten<Number>(dst, src.getCommands() + start, src.getVertices() + start, len, params);
    }
  }
}

// ============================================================================
// [Fog::typename PathT<Number>::T - Equality]
// ============================================================================

template<typename Number>
static bool _G2d_PathT_eq(const typename PathT<Number>::T& _a, const typename PathT<Number>::T& _b)
{
  const typename PathDataT<Number>::T* a = _a._d;
  const typename PathDataT<Number>::T* b = _b._d;

  if (a == b) return true;

  sysuint_t length = a->length;
  if (length != b->length) return false;

  return memcmp(a->commands, b->commands, length * sizeof(uint8_t)) == 0 &&
         memcmp(a->vertices, b->vertices, length * sizeof(typename PointT<Number>::T )) == 0 ;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_path_init(void)
{
  _g2d.pathf.ctor = _G2d_PathT_ctor<float>;
  _g2d.pathf.ctorCopyF = _G2d_PathT_ctorCopyT<float>;
  _g2d.pathf.dtor = _G2d_PathT_dtor<float>;
  _g2d.pathf.detach = _G2d_PathT_detach<float>;
  _g2d.pathf.reserve = _G2d_PathT_reserve<float>;
  _g2d.pathf.squeeze = _G2d_PathT_squeeze<float>;
  _g2d.pathf.add = _G2d_PathT_add<float>;
  _g2d.pathf.updateFlat = _G2d_PathT_updateFlat<float>;
  _g2d.pathf.updateBoundingBox = _G2d_PathT_updateBoundingBox<float>;
  _g2d.pathf.clear = _G2d_PathT_clear<float>;
  _g2d.pathf.reset = _G2d_PathT_reset<float>;
  _g2d.pathf.setPathF = _G2d_PathT_setPathT<float>;
  _g2d.pathf.setDeepF = _G2d_PathT_setDeepT<float>;
  _g2d.pathf.getSubpathRange = _G2d_PathT_getSubpathRange<float>;
  _g2d.pathf.moveTo = _G2d_PathT_moveTo<float>;
  _g2d.pathf.moveToRel = _G2d_PathT_moveToRel<float>;
  _g2d.pathf.lineTo = _G2d_PathT_lineTo<float>;
  _g2d.pathf.lineToRel = _G2d_PathT_lineToRel<float>;
  _g2d.pathf.hlineTo = _G2d_PathT_hlineTo<float>;
  _g2d.pathf.hlineToRel = _G2d_PathT_hlineToRel<float>;
  _g2d.pathf.vlineTo = _G2d_PathT_vlineTo<float>;
  _g2d.pathf.vlineToRel = _G2d_PathT_vlineToRel<float>;
  _g2d.pathf.polyTo = _G2d_PathT_polyTo<float>;
  _g2d.pathf.polyToRel = _G2d_PathT_polyToRel<float>;
  _g2d.pathf.quadTo = _G2d_PathT_quadTo<float>;
  _g2d.pathf.quadToRel = _G2d_PathT_quadToRel<float>;
  _g2d.pathf.cubicTo = _G2d_PathT_cubicTo<float>;
  _g2d.pathf.cubicToRel = _G2d_PathT_cubicToRel<float>;
  _g2d.pathf.smoothQuadTo = _G2d_PathT_smoothQuadTo<float>;
  _g2d.pathf.smoothQuadToRel = _G2d_PathT_smoothQuadToRel<float>;
  _g2d.pathf.smoothCubicTo = _G2d_PathT_smoothCubicTo<float>;
  _g2d.pathf.smoothCubicToRel = _G2d_PathT_smoothCubicToRel<float>;
  _g2d.pathf.arcTo = _G2d_PathT_arcTo<float>;
  _g2d.pathf.arcToRel = _G2d_PathT_arcToRel<float>;
  _g2d.pathf.svgArcTo = _G2d_PathT_svgArcTo<float>;
  _g2d.pathf.svgArcToRel = _G2d_PathT_svgArcToRel<float>;
  _g2d.pathf.close = _G2d_PathT_close<float>;
  _g2d.pathf.boxI = _G2d_PathT_boxT<float, int>;
  _g2d.pathf.boxF = _G2d_PathT_boxT<float, float>;
  _g2d.pathf.rectI = _G2d_PathT_rectT<float, int>;
  _g2d.pathf.rectF = _G2d_PathT_rectT<float, float>;
  _g2d.pathf.boxesI = _G2d_PathT_boxesT<float, int>;
  _g2d.pathf.boxesF = _G2d_PathT_boxesT<float, float>;
  _g2d.pathf.rectsI = _G2d_PathT_rectsT<float, int>;
  _g2d.pathf.rectsF = _G2d_PathT_rectsT<float, float>;
  _g2d.pathf.region = _G2d_PathT_region<float>;
  _g2d.pathf.polylineI = _G2d_PathT_polylineI<float>;
  _g2d.pathf.polylineF = _G2d_PathT_polylineT<float>;
  _g2d.pathf.polygonI = _G2d_PathT_polygonI<float>;
  _g2d.pathf.polygonF = _G2d_PathT_polygonT<float>;
  _g2d.pathf.shape = _G2d_PathT_shape<float>;
  _g2d.pathf.appendPath = _G2d_PathT_appendPathT<float, float>;
  _g2d.pathf.appendTranslatedPath = _G2d_PathT_appendTranslatedPathT<float>;
  _g2d.pathf.appendTransformedPath = _G2d_PathT_appendTransformedPathT<float>;
  _g2d.pathf.translate = _G2d_PathT_translate<float>;
  _g2d.pathf.transform = _G2d_PathT_transform<float>;
  _g2d.pathf.fitTo = _G2d_PathT_fitTo<float>;
  _g2d.pathf.scale = _G2d_PathT_scale<float>;
  _g2d.pathf.flipX = _G2d_PathT_flipX<float>;
  _g2d.pathf.flipY = _G2d_PathT_flipY<float>;
  _g2d.pathf.flatten = _G2d_PathT_flatten<float>;
  _g2d.pathf.eq = _G2d_PathT_eq<float>;

  _g2d.pathd.ctor = _G2d_PathT_ctor<double>;
  _g2d.pathd.ctorCopyD = _G2d_PathT_ctorCopyT<double>;
  _g2d.pathd.dtor = _G2d_PathT_dtor<double>;
  _g2d.pathd.detach = _G2d_PathT_detach<double>;
  _g2d.pathd.reserve = _G2d_PathT_reserve<double>;
  _g2d.pathd.squeeze = _G2d_PathT_squeeze<double>;
  _g2d.pathd.add = _G2d_PathT_add<double>;
  _g2d.pathd.updateFlat = _G2d_PathT_updateFlat<double>;
  _g2d.pathd.updateBoundingBox = _G2d_PathT_updateBoundingBox<double>;
  _g2d.pathd.clear = _G2d_PathT_clear<double>;
  _g2d.pathd.reset = _G2d_PathT_reset<double>;
  _g2d.pathd.setPathD = _G2d_PathT_setPathT<double>;
  _g2d.pathd.setDeepD = _G2d_PathT_setDeepT<double>;
  _g2d.pathd.getSubpathRange = _G2d_PathT_getSubpathRange<double>;
  _g2d.pathd.moveTo = _G2d_PathT_moveTo<double>;
  _g2d.pathd.moveToRel = _G2d_PathT_moveToRel<double>;
  _g2d.pathd.lineTo = _G2d_PathT_lineTo<double>;
  _g2d.pathd.lineToRel = _G2d_PathT_lineToRel<double>;
  _g2d.pathd.hlineTo = _G2d_PathT_hlineTo<double>;
  _g2d.pathd.hlineToRel = _G2d_PathT_hlineToRel<double>;
  _g2d.pathd.vlineTo = _G2d_PathT_vlineTo<double>;
  _g2d.pathd.vlineToRel = _G2d_PathT_vlineToRel<double>;
  _g2d.pathd.polyTo = _G2d_PathT_polyTo<double>;
  _g2d.pathd.polyToRel = _G2d_PathT_polyToRel<double>;
  _g2d.pathd.quadTo = _G2d_PathT_quadTo<double>;
  _g2d.pathd.quadToRel = _G2d_PathT_quadToRel<double>;
  _g2d.pathd.cubicTo = _G2d_PathT_cubicTo<double>;
  _g2d.pathd.cubicToRel = _G2d_PathT_cubicToRel<double>;
  _g2d.pathd.smoothQuadTo = _G2d_PathT_smoothQuadTo<double>;
  _g2d.pathd.smoothQuadToRel = _G2d_PathT_smoothQuadToRel<double>;
  _g2d.pathd.smoothCubicTo = _G2d_PathT_smoothCubicTo<double>;
  _g2d.pathd.smoothCubicToRel = _G2d_PathT_smoothCubicToRel<double>;
  _g2d.pathd.arcTo = _G2d_PathT_arcTo<double>;
  _g2d.pathd.arcToRel = _G2d_PathT_arcToRel<double>;
  _g2d.pathd.svgArcTo = _G2d_PathT_svgArcTo<double>;
  _g2d.pathd.svgArcToRel = _G2d_PathT_svgArcToRel<double>;
  _g2d.pathd.close = _G2d_PathT_close<double>;
  _g2d.pathd.boxI = _G2d_PathT_boxT<double, int>;
  _g2d.pathd.boxF = _G2d_PathT_boxT<double, float>;
  _g2d.pathd.boxD = _G2d_PathT_boxT<double, double>;
  _g2d.pathd.rectI = _G2d_PathT_rectT<double, int>;
  _g2d.pathd.rectF = _G2d_PathT_rectT<double, float>;
  _g2d.pathd.rectD = _G2d_PathT_rectT<double, double>;
  _g2d.pathd.boxesI = _G2d_PathT_boxesT<double, int>;
  _g2d.pathd.boxesF = _G2d_PathT_boxesT<double, float>;
  _g2d.pathd.boxesD = _G2d_PathT_boxesT<double, double>;
  _g2d.pathd.rectsI = _G2d_PathT_rectsT<double, int>;
  _g2d.pathd.rectsF = _G2d_PathT_rectsT<double, float>;
  _g2d.pathd.rectsD = _G2d_PathT_rectsT<double, double>;
  _g2d.pathd.region = _G2d_PathT_region<double>;
  _g2d.pathd.polylineI = _G2d_PathT_polylineI<double>;
  _g2d.pathd.polylineD = _G2d_PathT_polylineT<double>;
  _g2d.pathd.polygonI = _G2d_PathT_polygonI<double>;
  _g2d.pathd.polygonD = _G2d_PathT_polygonT<double>;
  _g2d.pathd.shape = _G2d_PathT_shape<double>;
  _g2d.pathd.appendPath = _G2d_PathT_appendPathT<double, double>;
  _g2d.pathd.appendTranslatedPath = _G2d_PathT_appendTranslatedPathT<double>;
  _g2d.pathd.appendTransformedPath = _G2d_PathT_appendTransformedPathT<double>;
  _g2d.pathd.translate = _G2d_PathT_translate<double>;
  _g2d.pathd.transform = _G2d_PathT_transform<double>;
  _g2d.pathd.fitTo = _G2d_PathT_fitTo<double>;
  _g2d.pathd.scale = _G2d_PathT_scale<double>;
  _g2d.pathd.flipX = _G2d_PathT_flipX<double>;
  _g2d.pathd.flipY = _G2d_PathT_flipY<double>;
  _g2d.pathd.flatten = _G2d_PathT_flatten<double>;
  _g2d.pathd.eq = _G2d_PathT_eq<double>;

  {
    PathDataF* d = _G2d_PathT_getDNull<float>();

    d->refCount.init(1);
    d->flat = 1;
    d->boundingBoxDirty = true;
    d->capacity = 0;
    d->length = 0;
    d->boundingBox.reset();
  }

  {
    PathDataD* d = _G2d_PathT_getDNull<double>();

    d->refCount.init(1);
    d->flat = 1;
    d->boundingBoxDirty = true;
    d->capacity = 0;
    d->length = 0;
    d->boundingBox.reset();
  }
}

FOG_NO_EXPORT void _g2d_path_fini(void)
{
  _G2d_PathT_getDNull<float>()->refCount.dec();
  _G2d_PathT_getDNull<double>()->refCount.dec();
}

} // Fog namespace
