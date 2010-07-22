// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Std.h>
#include <Fog/Core/Swap.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/PathUtil.h>
#include <Fog/Graphics/Region.h>

namespace Fog {

// ============================================================================
// [Fog::Path - Construction / Destruction]
// ============================================================================

DoublePath::DoublePath()
{
  _d = _dnull->ref();
}

DoublePath::DoublePath(const DoublePath& other)
{
  _d = other._d->ref();
}

DoublePath::~DoublePath()
{
  _d->deref();
}

// ============================================================================
// [Fog::Path - Implicit Sharing]
// ============================================================================

Static<DoublePathData> DoublePath::_dnull;

static FOG_INLINE sysuint_t _getPathDataSize(sysuint_t capacity)
{
  sysuint_t s = sizeof(DoublePathData);

  s += capacity * sizeof(uint8_t);
  s += capacity * sizeof(DoublePoint);

  return s;
}

static FOG_INLINE void _updatePathDataPointers(DoublePathData* d, sysuint_t capacity)
{
  d->vertices = reinterpret_cast<DoublePoint*>(
    d->commands + (((capacity + 15) & ~15) * sizeof(uint8_t)));
}

DoublePathData* DoublePath::_allocData(sysuint_t capacity)
{
  sysuint_t dsize = _getPathDataSize(capacity);

  DoublePathData* d = reinterpret_cast<DoublePathData*>(Memory::alloc(dsize));
  if (!d) return NULL;

  d->refCount.init(1);
  d->flat = 1;
  d->capacity = capacity;
  d->length = 0;
  _updatePathDataPointers(d, capacity);

  return d;
}

DoublePathData* DoublePath::_reallocData(DoublePathData* d, sysuint_t capacity)
{
  FOG_ASSERT(d->length <= capacity);
  sysuint_t dsize = _getPathDataSize(capacity);

  DoublePathData* newd = reinterpret_cast<DoublePathData*>(Memory::alloc(dsize));
  if (!newd) return NULL;

  sysuint_t length = d->length;

  newd->refCount.init(1);

  newd->flat = d->flat;
  newd->boundingBoxDirty = d->boundingBoxDirty;
  newd->boundingBoxMin = d->boundingBoxMin;
  newd->boundingBoxMax = d->boundingBoxMax;

  newd->capacity = capacity;
  newd->length = length;
  _updatePathDataPointers(newd, capacity);

  Memory::copy(newd->commands, d->commands, length * sizeof(uint8_t));
  Memory::copy(newd->vertices, d->vertices, length * sizeof(DoublePoint));

  d->deref();
  return newd;
}

DoublePathData* DoublePath::_copyData(const DoublePathData* d)
{
  sysuint_t length = d->length;
  if (!length) return _dnull->ref();

  DoublePathData* newd = _allocData(length);
  if (!newd) return NULL;

  newd->length = length;

  newd->flat = d->flat;
  newd->boundingBoxDirty = d->boundingBoxDirty;
  newd->boundingBoxMin = d->boundingBoxMin;
  newd->boundingBoxMax = d->boundingBoxMax;

  Memory::copy(newd->commands, d->commands, length * sizeof(uint8_t));
  Memory::copy(newd->vertices, d->vertices, length * sizeof(DoublePoint));

  return newd;
}

// ============================================================================
// [Fog::Path - Helpers]
// ============================================================================

static FOG_INLINE void _relativeToAbsolute(DoublePathData* d, double* x, double* y)
{
  sysuint_t last = d->length;
  if (!last) return;

  last--;

  uint8_t cmd = d->commands[last];
  if (!PathCmd::isVertex(cmd)) return;

  const DoublePoint& pt = d->vertices[last];
  *x += pt.x;
  *y += pt.y;
}

static FOG_INLINE void _relativeToAbsolute(DoublePathData* d, double* x0, double* y0, double* x1, double* y1)
{
  sysuint_t last = d->length;
  if (!last) return;

  last--;

  uint8_t cmd = d->commands[last];
  if (!PathCmd::isVertex(cmd)) return;

  const DoublePoint& pt = d->vertices[last];
  *x0 += pt.x;
  *y0 += pt.y;
  *x1 += pt.x;
  *y1 += pt.y;
}

static FOG_INLINE void _relativeToAbsolute(DoublePathData* d, double* x0, double* y0, double* x1, double* y1, double* x2, double* y2)
{
  sysuint_t last = d->length;
  if (!last) return;

  last--;

  uint8_t cmd = d->commands[last];
  if (!PathCmd::isVertex(cmd)) return;

  const DoublePoint& pt = d->vertices[last];

  *x0 += pt.x;
  *y0 += pt.y;
  *x1 += pt.x;
  *y1 += pt.y;
  *x2 += pt.x;
  *y2 += pt.y;
}

// ============================================================================
// [Fog::Path - Data]
// ============================================================================

err_t DoublePath::reserve(sysuint_t capacity)
{
  if (_d->refCount.get() == 1 && _d->capacity >= capacity) return ERR_OK;

  sysuint_t length = _d->length;
  if (capacity < length) capacity = length;

  DoublePathData* newd = _allocData(capacity);
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  newd->length = length;

  newd->flat = _d->flat;
  newd->boundingBoxDirty = _d->boundingBoxDirty;
  newd->boundingBoxMin = _d->boundingBoxMin;
  newd->boundingBoxMax = _d->boundingBoxMax;

  Memory::copy(newd->commands, _d->commands, length * sizeof(uint8_t));
  Memory::copy(newd->vertices, _d->vertices, length * sizeof(DoublePoint));

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

void DoublePath::squeeze()
{
  if (_d->length == _d->capacity) return;

  if (_d->refCount.get() == 1)
  {
    DoublePathData* newd = _reallocData(_d, _d->length);
    if (!newd) return;

    atomicPtrXchg(&_d, newd);
  }
  else
  {
    DoublePathData* newd = _copyData(_d);
    if (!newd) return;

    atomicPtrXchg(&_d, newd)->deref();
  }
}

sysuint_t DoublePath::_add(sysuint_t count)
{
  sysuint_t length = _d->length;
  sysuint_t remain = _d->capacity - length;

  if (_d->refCount.get() == 1 && count <= remain)
  {
    _d->length += count;
    return length;
  }
  else
  {
    sysuint_t optimalCapacity =
      Std::calcOptimalCapacity(sizeof(DoublePathData), sizeof(DoublePoint) + sizeof(uint8_t), length, length + count);

    DoublePathData* newd = _allocData(optimalCapacity);
    if (!newd) return INVALID_INDEX;

    newd->length = length + count;

    newd->flat = _d->flat;
    newd->boundingBoxDirty = _d->boundingBoxDirty;
    newd->boundingBoxMin = _d->boundingBoxMin;
    newd->boundingBoxMax = _d->boundingBoxMax;

    Memory::copy(newd->commands, _d->commands, length * sizeof(uint8_t));
    Memory::copy(newd->vertices, _d->vertices, length * sizeof(DoublePoint));

    atomicPtrXchg(&_d, newd)->deref();
    return length;
  }
}

err_t DoublePath::_detach()
{
  if (isDetached()) return ERR_OK;

  DoublePathData* newd = _copyData(_d);
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

err_t DoublePath::set(const DoublePath& other)
{
  if (_d == other._d) return ERR_OK;

  atomicPtrXchg(&_d, other._d->ref())->deref();
  return ERR_OK;
}

err_t DoublePath::setDeep(const DoublePath& other)
{
  DoublePathData* self_d = _d;
  DoublePathData* other_d = other._d;

  if (self_d == other_d) return ERR_OK;
  if (other_d->length == 0) { clear(); return ERR_OK; }

  err_t err = reserve(other_d->length);
  if (err) { clear(); return ERR_RT_OUT_OF_MEMORY; }

  self_d = _d;
  sysuint_t length = other_d->length;

  self_d->length = length;
  self_d->flat = other_d->flat;

  self_d->boundingBoxDirty = other_d->boundingBoxDirty;
  self_d->boundingBoxMin = other_d->boundingBoxMin;
  self_d->boundingBoxMax = other_d->boundingBoxMax;

  Memory::copy(self_d->commands, other_d->commands, length * sizeof(uint8_t));
  Memory::copy(self_d->vertices, other_d->vertices, length * sizeof(DoublePoint));

  return ERR_OK;
}

void DoublePath::clear()
{
  if (_d->refCount.get() > 1)
  {
    atomicPtrXchg(&_d, _dnull->ref())->deref();
  }
  else
  {
    _d->length = 0;

    _d->flat = 1;
    _d->boundingBoxDirty = true;
    _d->boundingBoxMin.clear();
    _d->boundingBoxMax.clear();
  }
}

void DoublePath::free()
{
  atomicPtrXchg(&_d, _dnull->ref())->deref();
}

// ============================================================================
// [Fog::Path - BoundingRect / FitTo]
// ============================================================================

// Based on the Graphics-Gems.

// Calculate the quadratic bezier curve coefficients at (t).
//
// a = (1-t)^2
// b = 2 * (1-t) * t
// c = t^2
#define DOUBLE_QUAD_COEFF_AT_T(t, a, b, c) \
  { \
    double inv_t = 1.0 - t; \
    \
    a = inv_t * inv_t; \
    b = 2.0 * inv_t * t; \
    c = t * t; \
  }

#define DOUBLE_QUAD_MERGE_AT_T(t) \
  { \
    if (t > 0.0 && t < 1.0) \
    { \
      DOUBLE_QUAD_COEFF_AT_T(t, a, b, c) \
      \
      DoublePoint __p(a * startPoint.x + b * pts[0].x + c * pts[1].x, \
                      a * startPoint.y + b * pts[0].y + c * pts[1].y); \
      \
      if (__p.x < pMin.x) pMin.x = __p.x; else if (__p.x > pMax.x) pMax.x = __p.x; \
      if (__p.y < pMin.y) pMin.y = __p.y; else if (__p.y > pMax.y) pMax.y = __p.y; \
    } \
  }

static void DoublePath_calcQuadExtrema(
  const DoublePoint& startPoint, const DoublePoint* pts, DoublePoint& pMin, DoublePoint& pMax)
{
  // Merge end point.
  if (pts[1].x < pMin.x) pMin.x = pts[1].x;
  if (pts[1].y < pMin.y) pMin.y = pts[1].y;

  if (pts[1].x > pMax.x) pMax.x = pts[1].x;
  if (pts[1].y > pMax.y) pMax.y = pts[1].y;

  double a, b, c;

  // X/Y extrema.
  double t0 = (startPoint.x - pts[0].x) / (startPoint.x - 2.0 * pts[0].x + pts[1].x);
  double t1 = (startPoint.y - pts[0].y) / (startPoint.y - 2.0 * pts[0].y + pts[1].y);

  DOUBLE_QUAD_MERGE_AT_T(t0)
  DOUBLE_QUAD_MERGE_AT_T(t1)
}

// Calculate the cubic bezier curve coefficients at (t).
//
// a = (1-t)^3
// b = 3 * (1-t)^2 * t
// c = 3 * (1-t) * t^2
// d = (t)^3
#define DOUBLE_CUBIC_COEFF_AT_T(t, a, b, c, d) \
  { \
    double inv_t = 1.0 - t; \
    double t_2 = t * t; \
    double inv_t_2 = inv_t * inv_t; \
    \
    a = inv_t * inv_t_2; \
    b = 3.0 * inv_t_2 * t; \
    c = 3.0 * inv_t * t_2; \
    d = t * t_2; \
  }

#define DOUBLE_CUBIC_MERGE_AT_T(t) \
  { \
    if (t > 0.0 && t < 1.0) \
    { \
      DOUBLE_CUBIC_COEFF_AT_T(t, a, b, c, d) \
      \
      DoublePoint __p(a * startPoint.x + b * pts[0].x + c * pts[1].x + d * pts[2].x, \
                      a * startPoint.y + b * pts[0].y + c * pts[1].y + d * pts[2].y); \
      \
      if (__p.x < pMin.x) pMin.x = __p.x; else if (__p.x > pMax.x) pMax.x = __p.x; \
      if (__p.y < pMin.y) pMin.y = __p.y; else if (__p.y > pMax.y) pMax.y = __p.y; \
    } \
  }

static void DoublePath_calcCubicExtrema(
  const DoublePoint& startPoint, const DoublePoint* pts, DoublePoint& pMin, DoublePoint& pMax)
{
  // Merge end point.
  if (pts[2].x < pMin.x) pMin.x = pts[2].x;
  if (pts[2].y < pMin.y) pMin.y = pts[2].y;

  if (pts[2].x > pMax.x) pMax.x = pts[2].x;
  if (pts[2].y > pMax.y) pMax.y = pts[2].y;

  double a;
  double b;
  double c;

  // X extrema.
  a = 3.0 * (-startPoint.x + 3.0 * pts[0].x - 3.0 * pts[1].x + pts[2].x);
  b = 6.0 * ( startPoint.x - 2.0 * pts[0].x +       pts[1].x           );
  c = 3.0 * (-startPoint.x +       pts[0].x                            );

  for (int i = 0;;)
  {
    if (Math::feq(a, 0.0))
    {
      if (!Math::feq(b, 0.0))
      {
        // Simple case (t = -c / b).
        double t0 = -c / b;
        double d;

        DOUBLE_CUBIC_MERGE_AT_T(t0)
      }
    }
    else
    {
      // Calculate roots (t = b^2 - 4ac).
      double t = b * b - 4.0 * a * c;
      if (t > 0.0)
      {
        double tsqrt = Math::sqrt(t);
        double recip = 1.0 / (2.0 * a);

        double t0 = (-b + tsqrt) * recip;
        double t1 = (-b - tsqrt) * recip;
        double d;

        DOUBLE_CUBIC_MERGE_AT_T(t0)
        DOUBLE_CUBIC_MERGE_AT_T(t1)
      }
    }

    if (++i == 2) break;

    // Y extrema.
    a = 3.0 * (-startPoint.y + 3.0 * pts[0].y - 3.0 * pts[1].y + pts[2].y);
    b = 6.0 * ( startPoint.y - 2.0 * pts[0].y +       pts[1].y           );
    c = 3.0 * (-startPoint.y +       pts[0].y                            );
  }
}

DoubleRect DoublePath::getBoundingRect() const
{
  if (_d->boundingBoxDirty)
  {
    sysuint_t i = _d->length;

    const uint8_t* cmd = _d->commands;
    const DoublePoint* pts = _d->vertices;

    bool first = false;

    DoublePoint pMin(0.0, 0.0);
    DoublePoint pMax(0.0, 0.0);
    DoublePoint pLast(0.0, 0.0);

    if (i > 0)
    {
      // Find first CMD_MOVE_TO. If there is no moveTo then we assume first point
      // as [0, 0].
      do {
        uint c = cmd[0] & PATH_CMD_TYPE_MASK;

        if (c == PATH_CMD_MOVE_TO)
        {
          pMin = pts[0];
          pMax = pts[0];
          pLast = pts[0];

          i--;
          cmd++;
          pts++;
          break;
        }
        else if (c == PATH_CMD_STOP)
        {
          i--;
          cmd++;
          pts++;
          continue;
        }
        else
        {
          break;
        }
      } while (i);

      // Iterate over the path / sub-paths.
      while (i)
      {
        switch (cmd[0] & PATH_CMD_TYPE_MASK)
        {
          case PATH_CMD_MOVE_TO:
          case PATH_CMD_LINE_TO:
            if (pts[0].x < pMin.x) pMin.x = pts[0].x; else if (pts[0].x > pMax.x) pMax.x = pts[0].x;
            if (pts[0].y < pMin.y) pMin.y = pts[0].y; else if (pts[0].y > pMax.y) pMax.y = pts[0].y;

            pLast = pts[0];

            i--;
            cmd++;
            pts++;
            break;

          case PATH_CMD_CURVE_3:
            FOG_ASSERT(i >= 2);
            if (FOG_UNLIKELY(i < 2)) break;

            DoublePath_calcQuadExtrema(pLast, pts, pMin, pMax);
            pLast = pts[1];

            i -= 2;
            cmd += 2;
            pts += 2;
            break;

          case PATH_CMD_CURVE_4:
            FOG_ASSERT(i >= 3);
            if (FOG_UNLIKELY(i < 3)) break;

            DoublePath_calcCubicExtrema(pLast, pts, pMin, pMax);
            pLast = pts[2];

            i -= 3;
            cmd += 3;
            pts += 3;
            break;

          case PATH_CMD_STOP:
          case PATH_CMD_END:
            pLast.clear();

            i--;
            cmd++;
            pts++;
            break;
        }
      }
    }

    _d->boundingBoxDirty = false;
    _d->boundingBoxMin = pMin;
    _d->boundingBoxMax = pMax;
  }
  
  return DoubleRect(
    _d->boundingBoxMin.x,
    _d->boundingBoxMin.y,
    _d->boundingBoxMax.x - _d->boundingBoxMin.x,
    _d->boundingBoxMax.y - _d->boundingBoxMin.y);
}

err_t DoublePath::fitTo(const DoubleRect& toRect)
{
  if (!toRect.isValid()) return ERR_RT_INVALID_ARGUMENT;

  DoubleRect currentRect = getBoundingRect();
  if (!currentRect.isValid()) return ERR_OK;

  double cx = currentRect.x;
  double cy = currentRect.y;

  double tx = toRect.x;
  double ty = toRect.y;

  double sx = toRect.w / currentRect.w;
  double sy = toRect.h / currentRect.h;

  FOG_RETURN_ON_ERROR(detach());

  sysuint_t i, length = _d->length;
  DoublePoint* pts = _d->vertices;

  for (i = 0; i < length; i++, pts++)
  {
    pts[0].set((pts[0].x - cx) * sx + tx,
               (pts[0].y - cy) * sy + ty);
  }

  _d->boundingBoxMin.set((_d->boundingBoxMin.x - cx) * sx + tx,
                         (_d->boundingBoxMin.y - cy) * sy + ty);
  _d->boundingBoxMax.set((_d->boundingBoxMax.x - cx) * sx + tx,
                         (_d->boundingBoxMax.y - cy) * sy + ty);

  return ERR_OK;
}

// ============================================================================
// [Fog::Path - SubPath]
// ============================================================================

sysuint_t DoublePath::getSubPathLength(sysuint_t subPathId) const
{
  sysuint_t length = _d->length;
  if (subPathId >= length) return 0;

  sysuint_t i = length - subPathId;

  const uint8_t* commands = _d->commands + subPathId;
  const DoublePoint* vertices = _d->vertices + subPathId;

  do {
    if (PathCmd::isMoveTo(commands[0])) break;

    commands++;
    vertices++;
  } while (++i < length);

  return i - subPathId;
}

// ============================================================================
// [Fog::Path - Start / End]
// ============================================================================

err_t DoublePath::start(sysuint_t* index)
{
  if (_d->length && !PathCmd::isStop(_d->commands[_d->length-1]))
  {
    sysuint_t pos = _add(1);
    if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

    uint8_t* commands = _d->commands + pos;
    DoublePoint* vertices = _d->vertices + pos;

    commands[0] = PATH_CMD_STOP;
    vertices[0].set(NAN, NAN);
  }

  if (index) *index = _d->length;
  return ERR_OK;
}

err_t DoublePath::endPoly(uint32_t flags)
{
  if (_d->length && PathCmd::isVertex(_d->commands[_d->length-1]))
  {
    sysuint_t pos = _add(1);
    if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

    uint8_t* commands = _d->commands + pos;
    DoublePoint* vertices = _d->vertices + pos;

    commands[0] = PATH_CMD_END | flags;
    vertices[0].set(NAN, NAN);
  }

  return ERR_OK;
}

err_t DoublePath::closePolygon(uint32_t flags)
{
  return endPoly(flags | PATH_CMD_FLAG_CLOSE);
}

// ============================================================================
// [Fog::Path - MoveTo]
// ============================================================================

err_t DoublePath::moveTo(double x, double y)
{
  sysuint_t pos = _add(1);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = _d->commands + pos;
  DoublePoint* vertices = _d->vertices + pos;

  commands[0] = PATH_CMD_MOVE_TO;
  vertices[0].set(x, y);

  // Bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  return ERR_OK;
}

err_t DoublePath::moveRel(double dx, double dy)
{
  _relativeToAbsolute(_d, &dx, &dy);
  return moveTo(dx, dy);
}

// ============================================================================
// [Fog::Path - LineTo]
// ============================================================================

err_t DoublePath::lineTo(double x, double y)
{
  sysuint_t pos = _add(1);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = _d->commands + pos;
  DoublePoint* vertices = _d->vertices + pos;

  commands[0] = PATH_CMD_LINE_TO;
  vertices[0].set(x, y);

  // Bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  return ERR_OK;
}

err_t DoublePath::lineRel(double dx, double dy)
{
  _relativeToAbsolute(_d, &dx, &dy);
  return lineTo(dx, dy);
}

err_t DoublePath::lineTo(const double* x, const double* y, sysuint_t count)
{
  sysuint_t pos = _add(count);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = _d->commands + pos;
  DoublePoint* vertices = _d->vertices + pos;

  for (sysuint_t i = 0; i < count; i++)
  {
    commands[i] = PATH_CMD_LINE_TO;
    vertices[i].set(x[i], y[i]);
  }

  // Bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  return ERR_OK;
}

err_t DoublePath::lineTo(const DoublePoint* pts, sysuint_t count)
{
  sysuint_t pos = _add(count);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = _d->commands + pos;
  DoublePoint* vertices = _d->vertices + pos;

  for (sysuint_t i = 0; i < count; i++)
  {
    commands[i] = PATH_CMD_LINE_TO;
    vertices[i].set(pts[i]);
  }

  // Bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  return ERR_OK;
}

err_t DoublePath::hlineTo(double x)
{
  sysuint_t last = _d->length;

  double y = 0.0;
  if (last > 0 && PathCmd::isVertex(_d->commands[--last])) y += _d->vertices[last].y;

  return lineTo(x, y);
}

err_t DoublePath::hlineRel(double dx)
{
  double dy = 0.0;
  _relativeToAbsolute(_d, &dx, &dy);

  return lineTo(dx, dy);
}

err_t DoublePath::vlineTo(double y)
{
  sysuint_t last = _d->length;

  double x = 0.0;
  if (last > 0 && PathCmd::isVertex(_d->commands[--last])) x += _d->vertices[last].x;

  return lineTo(x, y);
}

err_t DoublePath::vlineRel(double dy)
{
  double dx = 0.0;
  _relativeToAbsolute(_d, &dx, &dy);

  return lineTo(dx, dy);
}

// ============================================================================
// [Fog::Path - ArcTo]
// ============================================================================

static void _arcToBezier(
  double cx, double cy,
  double rx, double ry,
  double start,
  double sweep,
  DoublePoint* dst)
{
  sweep *= 0.5;

  double x0;
  double y0;
  Math::sincos(sweep, &y0, &x0);

  double tx = (1.0 - x0) * (4.0 / 3.0);
  double ty = y0 - tx * x0 / y0;
  double px[4];
  double py[4];

  double sn;
  double cs;
  Math::sincos(start + sweep, &sn, &cs);

  px[0] =  x0;
  py[0] = -y0;
  px[1] =  x0 + tx;
  py[1] = -ty;
  px[2] =  x0 + tx;
  py[2] =  ty;
  px[3] =  x0;
  py[3] =  y0;

  for (sysuint_t i = 0; i < 4; i++)
  {
    dst[i].set(cx + rx * (px[i] * cs - py[i] * sn),
               cy + ry * (px[i] * sn + py[i] * cs));
  }
}

err_t DoublePath::_arcTo(double cx, double cy, double rx, double ry, double start, double sweep, uint8_t initialCommand, bool closePath)
{
  start = fmod(start, 2.0 * M_PI);

  if (sweep >=  2.0 * M_PI) sweep =  2.0 * M_PI;
  if (sweep <= -2.0 * M_PI) sweep = -2.0 * M_PI;

  // Degenerated.
  if (fabs(sweep) < 1e-10)
  {
    sysuint_t pos = _add(2);
    if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

    uint8_t* commands = _d->commands + pos;
    DoublePoint* vertices = _d->vertices + pos;

    double aSin;
    double aCos;

    Math::sincos(start, &aSin, &aCos);
    commands[0] = initialCommand;
    vertices[0].set(cx + rx * aCos, cy + ry * aSin);

    Math::sincos(start + sweep, &aSin, &aCos);
    commands[1] = PATH_CMD_LINE_TO;
    vertices[1].set(cx + rx * aCos, cy + ry * aSin);
  }
  else
  {
    sysuint_t pos = _add(13);
    if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

    uint8_t* commands = _d->commands + pos;
    DoublePoint* vertices = _d->vertices + pos;

    double totalSweep = 0.0;
    double localSweep = 0.0;
    double prevSweep;
    bool done = false;

    commands[0] = initialCommand;

    commands++;
    vertices++;

    // 4 arcs is the maximum.
    int remain = 4;

    do {
      if (sweep < 0.0)
      {
        prevSweep   = totalSweep;
        localSweep  = -M_PI * 0.5;
        totalSweep -=  M_PI * 0.5;

        if (totalSweep <= sweep + PathUtil::BEZIER_ARC_ANGLE_EPSILON)
        {
          localSweep = sweep - prevSweep;
          done = true;
        }
      }
      else
      {
        prevSweep   = totalSweep;
        localSweep  = M_PI * 0.5;
        totalSweep += M_PI * 0.5;

        if (totalSweep >= sweep - PathUtil::BEZIER_ARC_ANGLE_EPSILON)
        {
          localSweep = sweep - prevSweep;
          done = true;
        }
      }

      _arcToBezier(cx, cy, rx, ry, start, localSweep, vertices - 1);
      commands[0] = PATH_CMD_CURVE_4;
      commands[1] = PATH_CMD_CURVE_4;
      commands[2] = PATH_CMD_CURVE_4;

      commands += 3;
      vertices += 3;

      start += localSweep;
    } while (!done && --remain);

    // Fix path length and set flat to false.
    _d->length = (sysuint_t)(commands - _d->commands);
    _d->flat = 0;
  }

  // Bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  if (closePath) closePolygon();
  return ERR_OK;
}

err_t DoublePath::_svgArcTo(
  double rx, double ry,
  double angle,
  bool largeArcFlag,
  bool sweepFlag,
  double x2, double y2,
  uint8_t initialCommand, bool closePath)
{
  // Mark current length (will be position where the first bezier would start).
  sysuint_t mark = _d->length;

  bool radiiOk = true;
  double x0 = 0.0, y0 = 0.0;

  // Get initial (x0, y0).
  if (mark)
  {
    uint8_t cmd = _d->commands[mark - 1];
    if (PathCmd::isVertex(cmd))
    {
      const DoublePoint* vertex = _d->vertices + mark - 1;
      x0 = vertex[0].x;
      y0 = vertex[0].y;

      // Skip last vertex, because initial command duplicates it. This is
      // sometimes called as a degenerate case.
      if (initialCommand != PATH_CMD_MOVE_TO)
      {
        FOG_RETURN_ON_ERROR(detach());
        initialCommand = cmd;
        _d->length--;
        mark--;
      }
    }
  }

  // Normalize radius.
  if (rx < 0.0) rx = -rx;
  if (ry < 0.0) ry = -ry;

  // Calculate the middle point between the current and the final points.
  double dx2 = (x0 - x2) / 2.0;
  double dy2 = (y0 - y2) / 2.0;

  double aSin;
  double aCos;
  Math::sincos(angle, &aSin, &aCos);

  // Calculate (x1, y1).
  double x1 =  aCos * dx2 + aSin * dy2;
  double y1 = -aSin * dx2 + aCos * dy2;

  // Ensure radii are large enough.
  double prx = rx * rx;
  double pry = ry * ry;
  double px1 = x1 * x1;
  double py1 = y1 * y1;

  // Check that radii are large enough.
  double radiiCheck = px1 / prx + py1 / pry;

  if (radiiCheck > 1.0)
  {
    rx = Math::sqrt(radiiCheck) * rx;
    ry = Math::sqrt(radiiCheck) * ry;
    prx = rx * rx;
    pry = ry * ry;
    if (radiiCheck > 10.0) radiiOk = false;
  }

  // Calculate (cx1, cy1).
  double sign = (largeArcFlag == sweepFlag) ? -1.0 : 1.0;
  double sq   = (prx*pry - prx*py1 - pry*px1) / (prx*py1 + pry*px1);
  double coef = sign * Math::sqrt((sq < 0) ? 0 : sq);
  double cx1  = coef *  ((rx * y1) / ry);
  double cy1  = coef * -((ry * x1) / rx);

  // Calculate (cx, cy) from (cx1, cy1).
  double sx2 = (x0 + x2) / 2.0;
  double sy2 = (y0 + y2) / 2.0;
  double cx = sx2 + (aCos * cx1 - aSin * cy1);
  double cy = sy2 + (aSin * cx1 + aCos * cy1);

  // Calculate the start_angle (angle1) and the sweep_angle (dangle).
  double ux =  (x1 - cx1) / rx;
  double uy =  (y1 - cy1) / ry;
  double vx = (-x1 - cx1) / rx;
  double vy = (-y1 - cy1) / ry;
  double p, n;

  // Calculate the angle start.
  n = Math::sqrt(ux*ux + uy*uy);
  p = ux; // (1 * ux) + (0 * uy)
  sign = (uy < 0) ? -1.0 : 1.0;
  double v = p / n;
  if (v < -1.0) v = -1.0;
  if (v >  1.0) v =  1.0;
  double startAngle = sign * acos(v);

  // Calculate the sweep angle.
  n = Math::sqrt((ux*ux + uy*uy) * (vx*vx + vy*vy));
  p = ux * vx + uy * vy;
  sign = (ux * vy - uy * vx < 0) ? -1.0 : 1.0;
  v = p / n;
  if (v < -1.0) v = -1.0;
  if (v >  1.0) v =  1.0;
  double sweepAngle = sign * acos(v);

  if (!sweepFlag && sweepAngle > 0)
    sweepAngle -= M_PI * 2.0;
  else if (sweepFlag && sweepAngle < 0)
    sweepAngle += M_PI * 2.0;

  err_t err = _arcTo(0.0, 0.0, rx, ry, startAngle, sweepAngle, initialCommand, false);
  if (err) return err;

  // If no error was reported then _arcTo had to add almost two vertices, for
  // matrix transform and fixing the end point we need almost one.
  FOG_ASSERT(_d->length > 0);

  // We can now build and transform the resulting arc.
  {
    DoubleMatrix matrix = DoubleMatrix::fromRotation(angle);
    matrix.translate(cx, cy, MATRIX_APPEND);
    PathUtil::transformPoints(_d->vertices + mark, _d->length - mark, &matrix);
  }

  // We must make sure that the starting and ending points exactly coincide
  // with the initial (x0, y0) and (x2, y2).
  {
    DoublePoint* vertex = _d->vertices;
    vertex[mark].x = x0;
    vertex[mark].y = y0;
    vertex[_d->length - 1].x = x2;
    vertex[_d->length - 1].y = y2;
  }

  // Bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  if (closePath) err = closePolygon();
  return err;
}

err_t DoublePath::arcTo(double cx, double cy, double rx, double ry, double start, double sweep)
{
  return _arcTo(cx, cy, rx, ry, start, sweep, PATH_CMD_LINE_TO, false);
}

err_t DoublePath::arcRel(double cx, double cy, double rx, double ry, double start, double sweep)
{
  _relativeToAbsolute(_d, &cx, &cy);
  return _arcTo(cx, cy, rx, ry, start, sweep, PATH_CMD_LINE_TO, false);
}

err_t DoublePath::svgArcTo(
  double rx, double ry,
  double angle,
  bool largeArcFlag,
  bool sweepFlag,
  double x2, double y2)
{
  return _svgArcTo(rx, ry, angle, largeArcFlag, sweepFlag, x2, y2, PATH_CMD_LINE_TO, false);
}

err_t DoublePath::svgArcRel(
  double rx, double ry,
  double angle,
  bool largeArcFlag,
  bool sweepFlag,
  double x2, double y2)
{
  _relativeToAbsolute(_d, &x2, &y2);
  return _svgArcTo(rx, ry, angle, largeArcFlag, sweepFlag, x2, y2, PATH_CMD_LINE_TO, false);
}

// ============================================================================
// [Fog::Path - CurveTo]
// ============================================================================

err_t DoublePath::curveTo(double cx, double cy, double tx, double ty)
{
  sysuint_t pos = _add(2);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = _d->commands + pos;
  DoublePoint* vertices = _d->vertices + pos;

  commands[0] = PATH_CMD_CURVE_3;
  commands[1] = PATH_CMD_CURVE_3;

  vertices[0].set(cx, cy);
  vertices[1].set(tx, ty);

  // Path is no longer flat.
  _d->flat = 0;
  // The bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  return ERR_OK;
}

err_t DoublePath::curveRel(double cx, double cy, double tx, double ty)
{
  _relativeToAbsolute(_d, &cx, &cy, &tx, &ty);
  return curveTo(cx, cy, tx, ty);
}

err_t DoublePath::curveTo(double tx, double ty)
{
  sysuint_t length = _d->length;
  if (!length || !PathCmd::isVertex(_d->commands[--length])) return ERR_OK;

  const DoublePoint* vertex = _d->vertices + length;

  double cx = vertex[0].x;
  double cy = vertex[0].y;

  if (length && PathCmd::isCurve(_d->commands[--length]))
  {
    cx += cx;
    cy += cy;
    cx -= vertex[-1].x;
    cy -= vertex[-1].y;
  }

  return curveTo(cx, cy, tx, ty);
}

err_t DoublePath::curveRel(double tx, double ty)
{
  _relativeToAbsolute(_d, &tx, &ty);
  return curveTo(tx, ty);
}

// ============================================================================
// [Fog::Path - CubicTo]
// ============================================================================

err_t DoublePath::cubicTo(double cx1, double cy1, double cx2, double cy2, double tx, double ty)
{
  sysuint_t pos = _add(3);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = _d->commands + pos;
  DoublePoint* vertices = _d->vertices + pos;

  commands[0] = PATH_CMD_CURVE_4;
  commands[1] = PATH_CMD_CURVE_4;
  commands[2] = PATH_CMD_CURVE_4;

  vertices[0].set(cx1, cy1);
  vertices[1].set(cx2, cy2);
  vertices[2].set(tx, ty);

  // Path is no longer flat.
  _d->flat = 0;
  // The bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  return ERR_OK;
}

err_t DoublePath::cubicRel(double cx1, double cy1, double cx2, double cy2, double tx, double ty)
{
  _relativeToAbsolute(_d, &cx1, &cy1, &cx2, &cy2, &tx, &ty);
  return cubicTo(cx1, cy1, cx2, cy2, tx, ty);
}

err_t DoublePath::cubicTo(double cx2, double cy2, double tx, double ty)
{
  sysuint_t length = _d->length;
  if (!length || !PathCmd::isVertex(_d->commands[--length])) return ERR_OK;

  const DoublePoint* vertex = _d->vertices + length;

  double cx1 = vertex[0].x;
  double cy1 = vertex[0].y;

  if (length && PathCmd::isCurve(_d->commands[--length]))
  {
    cx1 += cx1;
    cy1 += cy1;
    cx1 -= vertex[-1].x;
    cy1 -= vertex[-1].y;
  }

  return cubicTo(cx1, cy1, cx2, cy2, tx, ty);
}

err_t DoublePath::cubicRel(double cx2, double cy2, double tx, double ty)
{
  _relativeToAbsolute(_d, &cx2, &cy2, &tx, &ty);
  return cubicTo(cx2, cy2, tx, ty);
}

// ============================================================================
// [Fog::Path - FlipX / FlipY]
// ============================================================================

err_t DoublePath::flipX(double x1, double x2)
{
  sysuint_t i = _d->length;
  if (!i) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  double x = x1 + x2;
  DoublePoint* vertices = _d->vertices;

  do {
    vertices[0].x = x - vertices[0].x;
    vertices++;
  } while (--i);

  if (!_d->boundingBoxDirty)
  {
    double xMin = x - _d->boundingBoxMin.x;
    double xMax = x - _d->boundingBoxMax.x;
    if (xMax < xMin) swap(xMin, xMax);

    _d->boundingBoxMin.x = xMin;
    _d->boundingBoxMax.x = xMax;
  }

  return ERR_OK;
}

err_t DoublePath::flipY(double y1, double y2)
{
  sysuint_t i = _d->length;
  if (!i) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  double y = y1 + y2;
  DoublePoint* vertices = _d->vertices;

  do {
    vertices[0].y = y - vertices[0].y;
    vertices++;
  } while (--i);

  if (!_d->boundingBoxDirty)
  {
    double yMin = y - _d->boundingBoxMin.y;
    double yMax = y - _d->boundingBoxMax.y;
    if (yMax < yMin) swap(yMin, yMax);

    _d->boundingBoxMin.y = yMin;
    _d->boundingBoxMax.y = yMax;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Path - Translate]
// ============================================================================

err_t DoublePath::translate(double dx, double dy)
{
  sysuint_t i = _d->length;
  if (!i) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  DoublePoint* vertices = _d->vertices;

  do {
    vertices[0].translate(dx, dy);
    vertices++;
  } while (--i);

  if (!_d->boundingBoxDirty)
  {
    _d->boundingBoxMin.translate(dx, dy);
    _d->boundingBoxMax.translate(dx, dy);
  }

  return ERR_OK;
}

err_t DoublePath::translateSubPath(sysuint_t subPathId, double dx, double dy)
{
  if (subPathId >= _d->length) return ERR_RT_INVALID_ARGUMENT;

  sysuint_t i = _d->length;
  if (!i) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  uint8_t* commands = _d->commands;
  DoublePoint* vertices = _d->vertices;

  do {
    if (PathCmd::isStop(commands[0])) break;
    vertices[0].translate(dx, dy);

    commands++;
    vertices++;
  } while (--i);

  // Bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  return ERR_OK;
}

// ============================================================================
// [Fog::Path - Scale]
// ============================================================================

err_t DoublePath::scale(double sx, double sy, bool keepStartPos)
{
  sysuint_t i = _d->length;
  if (!i) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  DoublePoint* vertices = _d->vertices;

  if (keepStartPos)
  {
    uint8_t* commands = _d->commands;
    double tx = 0.0;
    double ty = 0.0;

    do {
      if (PathCmd::isVertex(commands[0])) break;

      commands++;
      vertices++;
    } while (--i);

    if (!i) return ERR_OK;

    double dx = tx - tx * sx;
    double dy = ty - ty * sy;

    do {
      vertices[0].x *= sx;
      vertices[0].y *= sy;
      vertices[0].x += dx;
      vertices[0].y += dy;
      vertices++;
    } while (--i);

    if (!_d->boundingBoxDirty)
    {
      DoublePoint& bMin = _d->boundingBoxMin;
      DoublePoint& bMax = _d->boundingBoxMin;

      bMin.x *= sx;
      bMin.y *= sy;
      bMin.x += dx;
      bMin.y += dy;

      bMax.x *= sx;
      bMax.y *= sy;
      bMax.x += dx;
      bMax.y += dy;

      if (bMin.x > bMax.x) swap(bMin.x, bMax.x);
      if (bMin.y > bMax.y) swap(bMin.y, bMax.y);
    }
  }
  else
  {
    do {
      vertices[0].x *= sx;
      vertices[0].y *= sy;
      vertices++;
    } while (--i);

    if (!_d->boundingBoxDirty)
    {
      DoublePoint& bMin = _d->boundingBoxMin;
      DoublePoint& bMax = _d->boundingBoxMin;

      bMin.x *= sx;
      bMin.y *= sy;

      bMax.x *= sx;
      bMax.y *= sy;

      if (bMin.x > bMax.x) swap(bMin.x, bMax.x);
      if (bMin.y > bMax.y) swap(bMin.y, bMax.y);
    }
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::Path - ApplyMatrix]
// ============================================================================

err_t DoublePath::applyMatrix(const DoubleMatrix& matrix)
{
  if (!_d->length) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  PathUtil::transformPoints(_d->vertices, _d->length, &matrix);

  // Bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  return ERR_OK;
}

err_t DoublePath::applyMatrix(const DoubleMatrix& matrix, const Range& range)
{
  sysuint_t length = _d->length;

  sysuint_t rstart = range.getStart();
  sysuint_t rend = range.getEnd();

  if (rend > length) rend = length;
  if (rstart >= rend) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());
  PathUtil::transformPoints(_d->vertices + range.getStart(), rend - rstart, &matrix);

  // Bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  return ERR_OK;
}

// ============================================================================
// [Fog::Path - Add]
// ============================================================================

err_t DoublePath::addRect(const IntRect& r, uint32_t direction)
{
  return addRect(DoubleRect(r.x, r.y, r.w, r.h), direction);
}

err_t DoublePath::addRect(const DoubleRect& r, uint32_t direction)
{
  if (!r.isValid()) return ERR_OK;

  sysuint_t pos = _add(5);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = _d->commands + pos;
  DoublePoint* vertices = _d->vertices + pos;

  double x0 = r.x;
  double y0 = r.y;
  double x1 = r.x + r.w;
  double y1 = r.y + r.h;

  if (direction == PATH_DIRECTION_CW)
  {
    commands[0] = PATH_CMD_MOVE_TO;
    commands[1] = PATH_CMD_LINE_TO;
    commands[2] = PATH_CMD_LINE_TO;
    commands[3] = PATH_CMD_LINE_TO;
    commands[4] = PATH_CMD_END | PATH_CMD_FLAG_CLOSE;

    vertices[0].x = x0;
    vertices[0].y = y0;
    vertices[1].x = x1;
    vertices[1].y = y0;
    vertices[2].x = x1;
    vertices[2].y = y1;
    vertices[3].x = x0;
    vertices[3].y = y1;
    vertices[4].x = NAN;
    vertices[4].y = NAN;
  }
  else
  {
    commands[0] = PATH_CMD_MOVE_TO;
    commands[1] = PATH_CMD_LINE_TO;
    commands[2] = PATH_CMD_LINE_TO;
    commands[3] = PATH_CMD_LINE_TO;
    commands[4] = PATH_CMD_END | PATH_CMD_FLAG_CLOSE;

    vertices[0].x = x0;
    vertices[0].y = y0;
    vertices[1].x = x0;
    vertices[1].y = y1;
    vertices[2].x = x1;
    vertices[2].y = y1;
    vertices[3].x = x1;
    vertices[3].y = y0;
    vertices[4].x = NAN;
    vertices[4].y = NAN;
  }

  if (!_d->boundingBoxDirty)
  {
    if (x0 < _d->boundingBoxMin.x) _d->boundingBoxMin.x = x0;
    if (x1 > _d->boundingBoxMax.x) _d->boundingBoxMax.x = x1;

    if (y0 < _d->boundingBoxMin.y) _d->boundingBoxMin.y = y0;
    if (y1 > _d->boundingBoxMax.y) _d->boundingBoxMax.y = y1;
  }

  return ERR_OK;
}

static err_t DoublePath_addPoly(DoublePath* path, const IntPoint* pts, sysuint_t count, bool closePath)
{
  sysuint_t pos = path->_add(count + closePath);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = path->_d->commands + pos;
  DoublePoint* vertices = path->_d->vertices + pos;

  commands[0] = PATH_CMD_MOVE_TO;
  memset(&commands[1], PATH_CMD_LINE_TO, count - 1);

  for (sysuint_t i = 0; i < count; i++)
  {
    vertices[i] = pts[i];
  }

  if (closePath)
  {
    commands[count] = PATH_CMD_END | PATH_CMD_FLAG_CLOSE;
    vertices[count].set(NAN, NAN);
  }

  return ERR_OK;
}

static err_t DoublePath_addPoly(DoublePath* path, const DoublePoint* pts, sysuint_t count, bool closePath)
{
  sysuint_t pos = path->_add(count + closePath);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = path->_d->commands + pos;
  DoublePoint* vertices = path->_d->vertices + pos;

  commands[0] = PATH_CMD_MOVE_TO;
  memset(&commands[1], PATH_CMD_LINE_TO, count - 1);

  for (sysuint_t i = 0; i < count; i++)
  {
    vertices[i] = pts[i];
  }

  if (closePath)
  {
    commands[count] = PATH_CMD_END | PATH_CMD_FLAG_CLOSE;
    vertices[count].set(NAN, NAN);
  }

  // Bounding box is no longer valid.
  path->_d->boundingBoxDirty = true;

  return ERR_OK;
}

err_t DoublePath::addPolygon(const IntPoint* pts, sysuint_t count)
{
  if (count < 3 || pts == NULL) return ERR_RT_INVALID_ARGUMENT;
  return DoublePath_addPoly(this, pts, count, true);
}

err_t DoublePath::addPolygon(const DoublePoint* pts, sysuint_t count)
{
  if (count < 3 || pts == NULL) return ERR_RT_INVALID_ARGUMENT;
  return DoublePath_addPoly(this, pts, count, true);
}

err_t DoublePath::addPolyLine(const IntPoint* pts, sysuint_t count)
{
  if (count < 2) return ERR_RT_INVALID_ARGUMENT;
  return DoublePath_addPoly(this, pts, count, false);
}

err_t DoublePath::addPolyLine(const DoublePoint* pts, sysuint_t count)
{
  if (count < 2) return ERR_RT_INVALID_ARGUMENT;
  return DoublePath_addPoly(this, pts, count, false);
}

err_t DoublePath::addRegion(const Region& r, uint32_t direction)
{
  return addRects(r.getData(), r.getLength(), direction);
}

err_t DoublePath::addRects(const IntRect* r, sysuint_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(r);

  sysuint_t pos = _add(count * 5);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = _d->commands + pos;
  DoublePoint* vertices = _d->vertices + pos;

  if (direction == PATH_DIRECTION_CW)
  {
    for (sysuint_t i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      commands[0] = PATH_CMD_MOVE_TO;
      commands[1] = PATH_CMD_LINE_TO;
      commands[2] = PATH_CMD_LINE_TO;
      commands[3] = PATH_CMD_LINE_TO;
      commands[4] = PATH_CMD_END | PATH_CMD_FLAG_CLOSE;

      vertices[0].x = (double)(r->x);
      vertices[0].y = (double)(r->y);
      vertices[1].x = (double)(r->x + r->w);
      vertices[1].y = (double)(r->y);
      vertices[2].x = (double)(r->x + r->w);
      vertices[2].y = (double)(r->y + r->h);
      vertices[3].x = (double)(r->x);
      vertices[3].y = (double)(r->y + r->h);
      vertices[4].x = NAN;
      vertices[4].y = NAN;

      commands += 5;
      vertices += 5;
    }
  }
  else
  {
    for (sysuint_t i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      commands[0] = PATH_CMD_MOVE_TO;
      commands[1] = PATH_CMD_LINE_TO;
      commands[2] = PATH_CMD_LINE_TO;
      commands[3] = PATH_CMD_LINE_TO;
      commands[4] = PATH_CMD_END | PATH_CMD_FLAG_CLOSE;

      vertices[0].x = (double)(r->x);
      vertices[0].y = (double)(r->y);
      vertices[1].x = (double)(r->x);
      vertices[1].y = (double)(r->y + r->h);
      vertices[2].x = (double)(r->x + r->w);
      vertices[2].y = (double)(r->y + r->h);
      vertices[3].x = (double)(r->x + r->w);
      vertices[3].y = (double)(r->y);
      vertices[4].x = NAN;
      vertices[4].y = NAN;
    }
  }

  // Bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  // Return and update path length.
  _d->length = (sysuint_t)(commands - _d->commands);
  return ERR_OK;
}

err_t DoublePath::addRects(const IntBox* r, sysuint_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(r);

  sysuint_t pos = _add(count * 5);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = _d->commands + pos;
  DoublePoint* vertices = _d->vertices + pos;

  if (direction == PATH_DIRECTION_CW)
  {
    for (sysuint_t i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      commands[0] = PATH_CMD_MOVE_TO;
      commands[1] = PATH_CMD_LINE_TO;
      commands[2] = PATH_CMD_LINE_TO;
      commands[3] = PATH_CMD_LINE_TO;
      commands[4] = PATH_CMD_END | PATH_CMD_FLAG_CLOSE;

      vertices[0].x = (double)(r->x1);
      vertices[0].y = (double)(r->y1);
      vertices[1].x = (double)(r->x2);
      vertices[1].y = (double)(r->y1);
      vertices[2].x = (double)(r->x2);
      vertices[2].y = (double)(r->y2);
      vertices[3].x = (double)(r->x1);
      vertices[3].y = (double)(r->y2);
      vertices[4].x = NAN;
      vertices[4].y = NAN;

      commands += 5;
      vertices += 5;
    }
  }
  else
  {
    for (sysuint_t i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      commands[0] = PATH_CMD_MOVE_TO;
      commands[1] = PATH_CMD_LINE_TO;
      commands[2] = PATH_CMD_LINE_TO;
      commands[3] = PATH_CMD_LINE_TO;
      commands[4] = PATH_CMD_END | PATH_CMD_FLAG_CLOSE;

      vertices[0].x = (double)(r->x1);
      vertices[0].y = (double)(r->y1);
      vertices[1].x = (double)(r->x1);
      vertices[1].y = (double)(r->y2);
      vertices[2].x = (double)(r->x2);
      vertices[2].y = (double)(r->y2);
      vertices[3].x = (double)(r->x2);
      vertices[3].y = (double)(r->y1);
      vertices[4].x = NAN;
      vertices[4].y = NAN;
    }
  }

  // Bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  // Return and update path length.
  _d->length = (sysuint_t)(commands - _d->commands);
  return ERR_OK;
}

err_t DoublePath::addRects(const DoubleRect* r, sysuint_t count, uint32_t direction)
{
  if (!count) return ERR_OK;
  FOG_ASSERT(r);

  sysuint_t pos = _add(count * 5);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* commands = _d->commands + pos;
  DoublePoint* vertices = _d->vertices + pos;

  if (direction == PATH_DIRECTION_CW)
  {
    for (sysuint_t i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      commands[0] = PATH_CMD_MOVE_TO;
      commands[1] = PATH_CMD_LINE_TO;
      commands[2] = PATH_CMD_LINE_TO;
      commands[3] = PATH_CMD_LINE_TO;
      commands[4] = PATH_CMD_END | PATH_CMD_FLAG_CLOSE;

      vertices[0].x = r->x;
      vertices[0].y = r->y;
      vertices[1].x = r->x + r->w;
      vertices[1].y = r->y;
      vertices[2].x = r->x + r->w;
      vertices[2].y = r->y + r->h;
      vertices[3].x = r->x;
      vertices[3].y = r->y + r->h;
      vertices[4].x = NAN;
      vertices[4].y = NAN;

      commands += 5;
      vertices += 5;
    }
  }
  else
  {
    for (sysuint_t i = 0; i < count; i++, r++)
    {
      if (!r->isValid()) continue;

      commands[0] = PATH_CMD_MOVE_TO;
      commands[1] = PATH_CMD_LINE_TO;
      commands[2] = PATH_CMD_LINE_TO;
      commands[3] = PATH_CMD_LINE_TO;
      commands[4] = PATH_CMD_END | PATH_CMD_FLAG_CLOSE;

      vertices[0].x = r->x;
      vertices[0].y = r->y;
      vertices[1].x = r->x;
      vertices[1].y = r->y + r->h;
      vertices[2].x = r->x + r->w;
      vertices[2].y = r->y + r->h;
      vertices[3].x = r->x + r->w;
      vertices[3].y = r->y;
      vertices[4].x = NAN;
      vertices[4].y = NAN;
    }
  }

  // Bounding box is no longer valid.
  _d->boundingBoxDirty = true;

  // Return and update path length.
  _d->length = (sysuint_t)(commands - _d->commands);
  return ERR_OK;
}

err_t DoublePath::addRound(const DoubleRect& r, const DoublePoint& radius, uint32_t direction)
{
  if (!r.isValid()) return ERR_OK;

  double rw2 = r.w / 2.0;
  double rh2 = r.h / 2.0;

  double rx = fabs(radius.x);
  double ry = fabs(radius.y);

  if (rx > rw2) rx = rw2;
  if (ry > rh2) ry = rh2;

  if (rx == 0 || ry == 0) return addRect(r, direction);

  double x0 = r.x;
  double y0 = r.y;
  double x1 = r.x + r.w;
  double y1 = r.y + r.h;

  err_t err = ERR_OK;

  if (direction == PATH_DIRECTION_CW)
  {
    err |= moveTo(x0 + rx, y0);

    err |= lineTo(x1 - rx, y0);
    err |= arcTo(x1 - rx, y0 + ry, rx, ry, M_PI * 1.5, M_PI * 0.5);

    err |= lineTo(x1, y1 - ry);
    err |= arcTo(x1 - rx, y1 - ry, rx, ry, M_PI * 0.0, M_PI * 0.5);

    err |= lineTo(x0 + rx, y1);
    err |= arcTo(x0 + rx, y1 - ry, rx, ry, M_PI * 0.5, M_PI * 0.5);

    err |= lineTo(x0, y0 + ry);
    err |= arcTo(x0 + rx, y0 + ry, rx, ry, M_PI * 1.0, M_PI * 0.5);
  }
  else
  {
    err |= moveTo(x0 + rx, y0);

    err |= arcTo(x0 + rx, y0 + ry, rx, ry, M_PI * 1.5, M_PI * -0.5);
    err |= lineTo(x0, y1 - ry);

    err |= arcTo(x0 + rx, y1 - ry, rx, ry, M_PI * 1.0, M_PI * -0.5);
    err |= lineTo(x1 - rx, y1);

    err |= arcTo(x1 - rx, y1 - ry, rx, ry, M_PI * 0.5, M_PI * -0.5);
    err |= lineTo(x1, y0 + ry);

    err |= arcTo(x1 - rx, y0 + ry, rx, ry, M_PI * 0.0, M_PI * -0.5);
  }

  err |= closePolygon();


  if (err == ERR_OK && !_d->boundingBoxDirty)
  {
    if (x0 < _d->boundingBoxMin.x) _d->boundingBoxMin.x = x0;
    if (x1 > _d->boundingBoxMax.x) _d->boundingBoxMax.x = y0;

    if (y0 < _d->boundingBoxMin.y) _d->boundingBoxMin.y = x1;
    if (y1 > _d->boundingBoxMax.y) _d->boundingBoxMax.y = y1;
  }

  return err;
}

err_t DoublePath::addEllipse(const DoubleRect& r, uint32_t direction)
{
  if (!r.isValid()) return ERR_OK;

  double rx = r.w / 2.0;
  double ry = r.h / 2.0;
  double cx = r.x + rx;
  double cy = r.y + ry;

  if (direction == PATH_DIRECTION_CW)
  {
    return _arcTo(cx, cy, rx, ry, 0.0, M_PI * 2.0, PATH_CMD_MOVE_TO, true);
  }
  else
  {
    return _arcTo(cx, cy, rx, ry, 0.0, M_PI * -2.0, PATH_CMD_MOVE_TO, true);
  }
}

err_t DoublePath::addEllipse(const DoublePoint& cp, const DoublePoint& r, uint32_t direction)
{
  if (direction == PATH_DIRECTION_CW)
  {
    return _arcTo(cp.x, cp.y, r.x, r.y, 0.0, M_PI * 2.0, PATH_CMD_MOVE_TO, true);
  }
  else
  {
    return _arcTo(cp.x, cp.y, r.x, r.y, 0.0, M_PI * -2.0, PATH_CMD_MOVE_TO, true);
  }
}

err_t DoublePath::addArc(const DoubleRect& r, double start, double sweep, uint32_t direction)
{
  if (!r.isValid()) return ERR_OK;

  double rx = r.w / 2.0;
  double ry = r.h / 2.0;
  double cx = r.x + rx;
  double cy = r.y + ry;

  if (direction == PATH_DIRECTION_CCW) { start += sweep; sweep = -sweep; }
  return _arcTo(cx, cy, rx, ry, start, sweep, PATH_CMD_MOVE_TO, false);
}

err_t DoublePath::addArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep, uint32_t direction)
{
  if (direction == PATH_DIRECTION_CCW) { start += sweep; sweep = -sweep; }
  return _arcTo(cp.x, cp.y, r.x, r.y, start, sweep, PATH_CMD_MOVE_TO, false);
}

err_t DoublePath::addChord(const DoubleRect& r, double start, double sweep, uint32_t direction)
{
  if (!r.isValid()) return ERR_OK;

  double rx = r.w / 2.0;
  double ry = r.h / 2.0;
  double cx = r.x + rx;
  double cy = r.y + ry;

  if (direction == PATH_DIRECTION_CCW) { start += sweep; sweep = -sweep; }
  return _arcTo(cx, cy, rx, ry, start, sweep, PATH_CMD_MOVE_TO, true);
}

err_t DoublePath::addChord(const DoublePoint& cp, const DoublePoint& r, double start, double sweep, uint32_t direction)
{
  if (direction == PATH_DIRECTION_CCW) { start += sweep; sweep = -sweep; }
  return _arcTo(cp.x, cp.y, r.x, r.y, start, sweep, PATH_CMD_MOVE_TO, true);
}

err_t DoublePath::addPie(const DoubleRect& r, double start, double sweep, uint32_t direction)
{
  if (!r.isValid()) return ERR_OK;

  double rx = r.w / 2.0;
  double ry = r.h / 2.0;
  double cx = r.x + rx;
  double cy = r.y + ry;

  return addPie(DoublePoint(cx, cy), DoublePoint(rx, ry), start, sweep);
}

err_t DoublePath::addPie(const DoublePoint& cp, const DoublePoint& r, double start, double sweep, uint32_t direction)
{
  if (sweep >= M_PI*2.0) return addEllipse(cp, r, direction);

  if (direction == PATH_DIRECTION_CCW) { start += sweep; sweep = -sweep; }

  start = fmod(start, M_PI * 2.0);
  if (start < 0) start += M_PI * 2.0;

  err_t err;

  if ( (err = moveTo(cp.x, cp.y)) ) return err;
  if ( (err = _arcTo(cp.x, cp.y, r.x, r.y, start, sweep, PATH_CMD_LINE_TO, true)) ) return err;

  return ERR_OK;
}

err_t DoublePath::addPath(const DoublePath& path)
{
  sysuint_t count = path.getLength();
  if (count == 0) return ERR_OK;

  int flat = isFlat() & path.isFlat();

  sysuint_t pos = _add(count);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  Memory::copy(_d->commands + pos, path._d->commands, count * sizeof(uint8_t));
  Memory::copy(_d->vertices + pos, path._d->vertices, count * sizeof(DoublePoint));

  _d->flat = flat;

  if (!_d->boundingBoxDirty && !path._d->boundingBoxDirty)
  {
    double x0 = path._d->boundingBoxMin.x;
    double y0 = path._d->boundingBoxMin.y;
    double x1 = path._d->boundingBoxMax.x;
    double y1 = path._d->boundingBoxMax.y;

    if (x0 < _d->boundingBoxMin.x) _d->boundingBoxMin.x = x0;
    if (x1 > _d->boundingBoxMax.x) _d->boundingBoxMax.x = x0;

    if (y0 < _d->boundingBoxMin.y) _d->boundingBoxMin.y = y0;
    if (y1 > _d->boundingBoxMax.y) _d->boundingBoxMax.y = y1;
  }

  return ERR_OK;
}

err_t DoublePath::addPath(const DoublePath& path, const DoublePoint& pt)
{
  sysuint_t count = path.getLength();
  if (count == 0) return ERR_OK;

  int flat = isFlat() & path.isFlat();

  sysuint_t pos = _add(count);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  Memory::copy(_d->commands + pos, path._d->commands, count * sizeof(uint8_t));
  PathUtil::translatePoints(_d->vertices + pos, path._d->vertices, count, &pt);

  _d->flat = flat;

  if (!_d->boundingBoxDirty && !path._d->boundingBoxDirty)
  {
    double x0 = path._d->boundingBoxMin.x + pt.x;
    double y0 = path._d->boundingBoxMin.y + pt.y;
    double x1 = path._d->boundingBoxMax.x + pt.x;
    double y1 = path._d->boundingBoxMax.y + pt.y;

    if (x0 < _d->boundingBoxMin.x) _d->boundingBoxMin.x = x0;
    if (x1 > _d->boundingBoxMax.x) _d->boundingBoxMax.x = x0;

    if (y0 < _d->boundingBoxMin.y) _d->boundingBoxMin.y = y0;
    if (y1 > _d->boundingBoxMax.y) _d->boundingBoxMax.y = y1;
  }

  return ERR_OK;
}

err_t DoublePath::addPath(const DoublePath& path, const DoubleMatrix& matrix)
{
  sysuint_t count = path.getLength();
  if (count == 0) return ERR_OK;

  int flat = isFlat() & path.isFlat();

  sysuint_t pos = _add(count);
  if (pos == INVALID_INDEX) return ERR_RT_OUT_OF_MEMORY;

  Memory::copy(_d->commands + pos, path._d->commands, count * sizeof(uint8_t));
  PathUtil::transformPoints(_d->vertices + pos, path._d->vertices, count, &matrix);

  _d->flat = flat;

  return ERR_OK;
}

// ============================================================================
// [Fog::Path - Flatten]
// ============================================================================

bool DoublePath::isFlat() const
{
  uint flat = _d->flat;
  if (flat != 0xFF) return (bool)flat;

  flat = 1;

  const uint8_t* commands = _d->commands;
  for (sysuint_t i = _d->length; i; i--, commands++)
  {
    if (PathCmd::isCurve(commands[0])) { flat = 0; break; }
  }

  _d->flat = flat;
  return (bool)flat;
}

err_t DoublePath::flatten()
{
  return flattenTo(*this, NULL, 1.0);
}

err_t DoublePath::flatten(const DoubleMatrix* matrix, double approximationScale)
{
  return flattenTo(*this, NULL, 1.0);
}

static err_t DoublePath_flattenData(
  DoublePath& dst,
  const uint8_t* srcCommands, const DoublePoint* srcVertices, sysuint_t srcCount,
  const DoubleMatrix* matrix, double approximationScale)
{
  dst.clear();
  if (srcCount == 0) return ERR_OK;

  err_t err = ERR_OK;
  sysuint_t grow = srcCount * 4;

  if (grow < srcCount) return ERR_RT_OUT_OF_MEMORY;
  if (dst.reserve(grow)) return ERR_RT_OUT_OF_MEMORY;

  uint8_t* dstCommands;
  DoublePoint* dstVertices;

  double lastx = 0.0;
  double lasty = 0.0;

  // Encuse space and update dstCommands / dstVertices pointers.
ensureSpace:
  {
    sysuint_t pos = dst._add(srcCount);

    if (pos == INVALID_INDEX)
    {
      err = ERR_RT_OUT_OF_MEMORY;
      goto error;
    }

    dstCommands = dst._d->commands + pos;
    dstVertices = dst._d->vertices + pos;
  }

  do {
    switch (srcCommands[0])
    {
      case PATH_CMD_MOVE_TO:
      case PATH_CMD_LINE_TO:
      {
        dstCommands[0] = srcCommands[0];
        dstVertices[0].x = lastx = srcVertices[0].x;
        dstVertices[0].y = lasty = srcVertices[0].y;

        dstCommands++;
        dstVertices++;

        srcCommands++;
        srcVertices++;
        srcCount--;

        break;
      }

      case PATH_CMD_CURVE_3:
      {
        if (srcCount <= 1) goto invalid;
        if (srcCommands[1] != PATH_CMD_CURVE_3) goto invalid;

        sysuint_t dstLength = (sysuint_t)(dstCommands - dst._d->commands);
        uint8_t initialCommand = PATH_CMD_LINE_TO;

        // Get the last command that will be set as first command by
        // approximation function.
        if (dstLength > 0 && PathCmd::isVertex(dstCommands[-1]))
        {
          //initialCommand = dstCommands[-1];
          //dstLength--;
        }
        else
        {
          initialCommand = PATH_CMD_MOVE_TO;
        }

        // Finalize path.
        dst._d->length = dstLength;

        // Approximate curve.
        err = PathUtil::functionMap.approximateCurve3(
          dst,
          lastx, lasty,
          srcVertices[0].x, srcVertices[0].y,
          srcVertices[1].x, srcVertices[1].y,
          initialCommand,
          approximationScale, 0.0);
        if (err) goto error;

        lastx = srcVertices[1].x;
        lasty = srcVertices[1].y;

        srcCommands += 2;
        srcVertices += 2;
        srcCount -= 2;

        if (srcCount == 0)
          goto end;
        else
          goto ensureSpace;
      }

      case PATH_CMD_CURVE_4:
      {
        if (srcCount <= 2) goto invalid;
        if (srcCommands[1] != PATH_CMD_CURVE_4 ||
            srcCommands[2] != PATH_CMD_CURVE_4) goto invalid;

        sysuint_t dstLength = (sysuint_t)(dstCommands - dst._d->commands);
        uint8_t initialCommand = PATH_CMD_LINE_TO;

        // Get the last command that will be set as first command by
        // approximation function.
        if (dstLength > 0 && PathCmd::isVertex(dstCommands[-1]))
        {
          //initialCommand = dstCommands[-1];
          //dstLength--;
        }
        else
        {
          initialCommand = PATH_CMD_MOVE_TO;
        }

        // Finalize path.
        dst._d->length = dstLength;

        // Approximate curve.
        err = PathUtil::functionMap.approximateCurve4(
          dst,
          lastx, lasty,
          srcVertices[0].x, srcVertices[0].y,
          srcVertices[1].x, srcVertices[1].y,
          srcVertices[2].x, srcVertices[2].y,
          initialCommand,
          approximationScale, 0.0, 0.0);
        if (err) goto error;

        lastx = srcVertices[2].x;
        lasty = srcVertices[2].y;

        srcCommands += 3;
        srcVertices += 3;
        srcCount -= 3;

        if (srcCount == 0)
          goto end;
        else
          goto ensureSpace;
      }

      default:
      {
        dstCommands[0] = srcCommands[0];
        dstVertices[0].x = srcVertices[0].x;
        dstVertices[0].y = srcVertices[0].y;

        dstCommands++;
        dstVertices++;

        srcCommands++;
        srcVertices++;
        srcCount--;

        break;
      }
    }
  } while(srcCount);

  dst._d->length = (sysuint_t)(dstCommands - dst._d->commands);
end:
  if (matrix) dst.applyMatrix(*matrix);
  return err;

invalid:
  err = ERR_PATH_INVALID;

error:
  dst._d->length = 0;
  return err;
}

err_t DoublePath::flattenTo(DoublePath& dst, const DoubleMatrix* matrix, double approximationScale) const
{
  // --------------------------------------------------------------------------
  // Contains only lines (flat path).
  // --------------------------------------------------------------------------

  if (isFlat())
  {
    if (this == &dst)
    {
      if (!matrix) return ERR_OK;
      return dst.applyMatrix(*matrix);
    }
    else
    {
      if (!matrix) return dst.set(*this);

      dst.clear();
      return dst.addPath(*this, *matrix);
    }
  }

  // --------------------------------------------------------------------------
  // Contains curves.
  // --------------------------------------------------------------------------

  // The dst argument is here mainly if we need to flatten path into different
  // one. If paths are equal, we will simply create second path and assign
  // result to first one.
  if (this == &dst)
  {
    DoublePath tmp(*this);

    return DoublePath_flattenData(dst,
      tmp.getCommands(),
      tmp.getVertices(),
      tmp.getLength(), matrix, approximationScale);
  }
  else
  {
    return DoublePath_flattenData(dst,
      getCommands(),
      getVertices(),
      getLength(), matrix, approximationScale);
  }
}

err_t DoublePath::flattenSubPathTo(DoublePath& dst, sysuint_t subPathId, const DoubleMatrix* matrix, double approximationScale) const
{
  sysuint_t len = getSubPathLength(subPathId);

  // --------------------------------------------------------------------------
  // No sub-path.
  // --------------------------------------------------------------------------

  if (!len)
  {
    if (this != &dst) dst.clear();
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // Contains only lines (flat path).
  // --------------------------------------------------------------------------

  if (isFlat())
  {
    if (this == &dst)
    {
      if (!matrix) return ERR_OK;
      return dst.applyMatrix(*matrix);
    }
    else
    {
      if (!matrix) return dst.set(*this);

      dst.clear();
      return dst.addPath(*this, *matrix);
    }
  }

  // --------------------------------------------------------------------------
  // Contains curves.
  // --------------------------------------------------------------------------

  // The dst argument is here mainly if we need to flatten path into different
  // one. If paths are equal, we will simply create second path and assign
  // result to first one.
  if (this == &dst)
  {
    DoublePath tmp(*this);

    return DoublePath_flattenData(dst,
      tmp.getCommands() + subPathId,
      tmp.getVertices() + subPathId,
      len, matrix, approximationScale);
  }
  else
  {
    return DoublePath_flattenData(dst,
      getCommands() + subPathId,
      getVertices() + subPathId,
      len, matrix, approximationScale);
  }
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_path_init(void)
{
  using namespace Fog;

  DoublePathData* d = DoublePath::_dnull.instancep();

  d->refCount.init(1);

  d->flat = true;
  d->boundingBoxDirty = true;
  d->boundingBoxMin.clear();
  d->boundingBoxMax.clear();

  d->capacity = 0;
  d->length = 0;

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_path_shutdown(void)
{
  using namespace Fog;

  DoublePath::_dnull->refCount.dec();
}
