// [Fog/Graphics Library - C++ API]
//
// [Licence] 
// MIT, See COPYING file in package

// [Precompiled headers]
#ifdef FOG_PRECOMP
#include FOG_PRECOMP
#endif

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Error.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Std.h>
#include <Fog/Graphics/AffineMatrix.h>
#include <Fog/Graphics/Error.h>
#include <Fog/Graphics/Path.h>

// [Antigrain]
#include "agg_basics.h"
#include "agg_conv_dash.h"
#include "agg_conv_stroke.h"
#include "agg_vcgen_dash.h"

namespace Fog {

// ============================================================================
// [AntiGrain Wrappers]
// ============================================================================

// Wraps Fog::Path to antigrain like vertex storage.
struct FOG_HIDDEN AggPath
{
  FOG_INLINE AggPath(const Path& path)
  {
    d = path._d;
    rewind(0);
  }

  FOG_INLINE ~AggPath()
  {
  }

  FOG_INLINE void rewind(unsigned index)
  {
    vCur = d->data + index;
    vEnd = d->data + d->length;
  }

  FOG_INLINE unsigned vertex(double* x, double* y)
  {
    if (vCur == vEnd) return Path::CmdStop;

    *x = vCur->x;
    *y = vCur->y;

    uint command = vCur->cmd.cmd();
    vCur++;
    return command;
  }

private:
  const Path::Data* d;
  const Path::Vertex* vCur;
  const Path::Vertex* vEnd;
};

template<typename VertexStorage>
static err_t concatToPath(Path& dst, VertexStorage& src, unsigned path_id = 0)
{
  sysuint_t i, len = dst.length();
  sysuint_t step = 1024;

  Path::Vertex* v;
  err_t err;

  src.rewind(path_id);

  for (;;)
  {
    if ( (err = dst.reserve(len + step)) ) return err;
    v = dst._d->data + len;

    // Concat vertexes.
    for (i = step; i; i--, v++)
    {
      if ((v->cmd._cmd = src.vertex(&v->x, &v->y)) == Path::CmdStop)
        goto done;
    }

    // If we are here it's needed to alloc more memory (really big path).
    len += step;
    dst._d->length = len;

    // Double step until we reach 1MB.
    if (step < 1024*1024) step <<= 1;
  }

done:
  dst._d->length = (sysuint_t)(v - dst._d->data);
  return Error::Ok;
}

// ============================================================================
// [Fog::Path::Data]
// ============================================================================

Static<Path::Data> Path::sharedNull;

Path::Data* Path::Data::ref() const
{
  if (flags & IsSharable)
  {
    refCount.inc();
    return const_cast<Data*>(this);
  }
  else
  {
    return copy();
  }
}

void Path::Data::deref()
{
  if (refCount.deref() && (flags & IsDynamic) != 0) 
  {
    Memory::free((void*)this);
  }
}

Path::Data* Path::Data::copy() const
{
  if (!length) return refAlways();

  Data* d = alloc(length);
  if (!d) return NULL;

  d->length = length;
  d->type = type;
  memcpy(d->data, data, sizeof(Path::Vertex) * length);

  return d;
}

Path::Data* Path::Data::alloc(sysuint_t capacity)
{
  sysuint_t dsize = 
    sizeof(Data) - sizeof(Vertex) + capacity * sizeof(Vertex);

  Data* d = reinterpret_cast<Data*>(Memory::alloc(dsize));
  if (!d) return NULL;

  d->refCount.init(1);
  d->flags = IsDynamic | IsSharable;
  d->type = Path::LineType;
  d->capacity = capacity;
  d->length = 0;

  return d;
}

Path::Data* Path::Data::realloc(Data* d, sysuint_t capacity)
{
  FOG_ASSERT(d->length <= capacity);

  if (d->flags & IsDynamic)
  {
    sysuint_t dsize = 
      sizeof(Data) - sizeof(Vertex) + capacity * sizeof(Vertex);

    Data* newd = reinterpret_cast<Data*>(Memory::realloc((void*)d, dsize));
    if (!newd) return NULL;

    newd->capacity = capacity;
    return newd;
  }
  else
  {
    Data* newd = alloc(capacity);

    newd->length = d->length;
    newd->type = d->type;
    memcpy(newd->data, d->data, d->length * sizeof(Vertex));
    d->deref();
    return newd;
  }
}

// ============================================================================
// [Fog::Path]
// ============================================================================

static FOG_INLINE double calcDistance(double x1, double y1, double x2, double y2)
{
  double dx = x2 - x1;
  double dy = y2 - y1;
  return sqrt((dx * dx) + (dy * dy));
}

static FOG_INLINE Path::Cmd lastCmd(Path::Data* d)
{
  return Path::Cmd(d->length
    ? Path::Cmd(d->data[d->length-1].cmd)
    : Path::Cmd(Path::CmdEndPoly));
}

static FOG_INLINE double lastX(Path::Data* d)
{
  return d->length ? d->data[d->length-1].x : 0.0;
}

static FOG_INLINE double lastY(Path::Data* d)
{
  return d->length ? d->data[d->length-1].y : 0.0;
}

static FOG_INLINE void relToAbsInline(Path::Data* d, double* x, double* y)
{
  Path::Vertex* v = d->data + d->length;

  if (d->length && v[-1].cmd.isVertex())
  {
    *x += v[-1].x;
    *y += v[-1].y;
  }
}

static FOG_INLINE void relToAbsInline(Path::Data* d, double* x0, double* y0, double* x1, double* y1)
{
  Path::Vertex* v = d->data + d->length;

  if (d->length && v[-1].cmd.isVertex())
  {
    *x0 += v[-1].x;
    *y0 += v[-1].y;
    *x1 += v[-1].x;
    *y1 += v[-1].y;
  }
}

static FOG_INLINE void relToAbsInline(Path::Data* d, double* x0, double* y0, double* x1, double* y1, double* x2, double* y2)
{
  Path::Vertex* v = d->data + d->length;

  if (d->length && v[-1].cmd.isVertex())
  {
    *x0 += v[-1].x;
    *y0 += v[-1].y;
    *x1 += v[-1].x;
    *y1 += v[-1].y;
    *x2 += v[-1].x;
    *y2 += v[-1].y;
  }
}

template<class VertexStorage>
static err_t aggJoinPath(Path* self, VertexStorage& a)
{
  sysuint_t i, len = a.num_vertices();
  Path::Vertex* v = self->_add(len);
  if (!v) return Error::OutOfMemory;

  a.rewind(0);
  for (i = 0; i < len; i++)
  {
    v[i].cmd = a.vertex(&v[i].x, &v[i].y);
  }

  return Error::Ok;
}

// ============================================================================
// [Fog::Path - Construction / Destruction]
// ============================================================================

Path::Path()
{
  _d = sharedNull->refAlways();
}

Path::Path(Data* d)
{
  _d = d;
}

Path::Path(const Path& other)
{
  _d = other._d->ref();
}

Path::~Path()
{
  _d->deref();
}

// ============================================================================
// [Fog::Path - Type]
// ============================================================================

uint32_t Path::type() const
{
  uint32_t t = _d->type;
  if (t != 0) return t;

  // Detection
  const Vertex* v = _d->data;
  sysuint_t len = _d->length;

  t = LineType;
  for (sysuint_t i = 0; i < len; i++, v++)
  {
    if (v->cmd.cmd() > CmdLineTo && v->cmd.cmd() < CmdMask)
    {
      t = CurveType;
      break;
    }
  }

  _d->type = t;
  return t;
}

// ============================================================================
// [Fog::Path - Data]
// ============================================================================

err_t Path::reserve(sysuint_t capacity)
{
  if (_d->refCount.get() == 1 && _d->capacity >= capacity) return Error::Ok;

  Data* newd = Data::alloc(capacity);
  if (!newd) return Error::OutOfMemory;

  newd->length = _d->length;
  memcpy(newd->data, _d->data, _d->length * sizeof(Vertex));

  AtomicBase::ptr_setXchg(&_d, newd)->deref();
  return Error::Ok;
}

Path::Vertex* Path::_add(sysuint_t count)
{
  sysuint_t length = _d->length;
  sysuint_t remain = _d->capacity - length;

  if (_d->refCount.get() == 1 && count <= remain)
  {
    _d->length += count;
    return _d->data + length;
  }
  else
  {
    sysuint_t optimalCapacity = 
      Std::calcOptimalCapacity(sizeof(Data), sizeof(Vertex), _d->length, _d->length + count);

    Data* newd = Data::alloc(optimalCapacity);
    if (!newd) return NULL;

    newd->length = length + count;
    memcpy(newd->data, _d->data, length * sizeof(Vertex));

    AtomicBase::ptr_setXchg(&_d, newd)->deref();
    return newd->data + length;
  }
}

err_t Path::_detach()
{
  if (isDetached()) return Error::Ok;

  Data* newd = _d->copy();
  if (!newd) return Error::OutOfMemory;

  AtomicBase::ptr_setXchg(&_d, newd)->deref();
  return Error::Ok;
}

err_t Path::set(const Path& other)
{
  Data* self_d = _d;
  Data* other_d = other._d;
  if (self_d == other_d) return Error::Ok;

  if ((self_d->flags & Data::IsStrong) != 0 || 
      (other_d->flags & Data::IsSharable) == 0)
  {
    return setDeep(other);
  }
  else
  {
    AtomicBase::ptr_setXchg(&_d, other_d->ref())->deref();
    return Error::Ok;
  }
}

err_t Path::setDeep(const Path& other)
{
  Data* self_d = _d;
  Data* other_d = other._d;

  if (self_d == other_d) return Error::Ok;
  if (other_d->length == 0) { clear(); return Error::Ok; }

  err_t err = reserve(other_d->length);
  if (err) { clear(); return Error::OutOfMemory; }

  self_d = _d;
  sysuint_t len = other_d->length;

  self_d->length = len;
  self_d->type = other_d->type;
  memcpy(self_d->data, other_d->data, len * sizeof(Vertex));

  return Error::Ok;
}

void Path::clear()
{
  if (_d->refCount.get() > 1)
  {
    AtomicBase::ptr_setXchg(&_d, sharedNull->refAlways())->deref();
  }
  else
  {
    _d->length = 0;
    _d->type = LineType;
  }
}

void Path::free()
{
  AtomicBase::ptr_setXchg(&_d, sharedNull->refAlways())->deref();
}

// ============================================================================
// [Fog::Path - Start / End]
// ============================================================================

err_t Path::start(sysuint_t* index)
{
  if (_d->length && !_d->data[_d->length-1].cmd.isStop())
  {
    Vertex* v = _add(1);
    if (!v) return Error::OutOfMemory;

    v->cmd = CmdStop;
    v->x = 0.0;
    v->y = 0.0;
  }

  if (index) *index = _d->length;
  return Error::Ok;
}

err_t Path::endPoly(uint32_t cmdflags)
{
  if (_d->length && _d->data[_d->length-1].cmd.isVertex())
  {
    Vertex* v = _add(1);
    if (!v) return Error::OutOfMemory;

    v->cmd = cmdflags | CmdEndPoly;
    v->x = 0.0;
    v->y = 0.0;
  }

  return Error::Ok;
}

err_t Path::closePolygon(uint32_t cmdflags)
{
  return endPoly(cmdflags | CFlagClose);
}

// ============================================================================
// [Fog::Path - MoveTo]
// ============================================================================

err_t Path::moveTo(double x, double y)
{
  Vertex* v = _add(1);
  if (!v) return Error::OutOfMemory;

  v->cmd = CmdMoveTo;
  v->x = x;
  v->y = y;

  return Error::Ok;
}

err_t Path::moveToRel(double dx, double dy)
{
  relToAbsInline(_d, &dx, &dy);
  return moveTo(dx, dy);
}

// ============================================================================
// [Fog::Path - LineTo]
// ============================================================================

err_t Path::lineTo(double x, double y)
{
  Vertex* v = _add(1);
  if (!v) return Error::OutOfMemory;

  v->cmd = CmdLineTo;
  v->x = x;
  v->y = y;

  return Error::Ok;
}

err_t Path::lineToRel(double dx, double dy)
{
  relToAbsInline(_d, &dx, &dy);
  return lineTo(dx, dy);
}

err_t Path::lineTo(const double* x, const double* y, sysuint_t count)
{
  Vertex* v = _add(count);
  if (!v) return Error::OutOfMemory;

  for (sysuint_t i = 0; i < count; i++)
  {
    v[i].cmd = CmdLineTo;
    v[i].x = x[i];
    v[i].y = y[i];
  }

  return Error::Ok;
}

err_t Path::lineTo(const PointF* pts, sysuint_t count)
{
  Vertex* v = _add(count);
  if (!v) return Error::OutOfMemory;

  for (sysuint_t i = 0; i < count; i++)
  {
    v[i].cmd = CmdLineTo;
    v[i].x = pts[i].x();
    v[i].y = pts[i].y();
  }

  return Error::Ok;
}

err_t Path::hlineTo(double x)
{
  return lineTo(x, lastY(_d));
}

err_t Path::hlineToRel(double dx)
{
  double dy = 0.0;
  relToAbsInline(_d, &dx, &dy);
  return lineTo(dx, dy);
}

err_t Path::vlineTo(double y)
{
  return lineTo(lastX(_d), y);
}

err_t Path::vlineToRel(double dy)
{
  double dx = 0.0;
  relToAbsInline(_d, &dx, &dy);
  return lineTo(dx, dy);
}

// ============================================================================
// [Fog::Path - ArcTo]
// ============================================================================

// This epsilon is used to prevent us from adding degenerate curves
// (converging to a single point).
// The value isn't very critical. Function arc_to_bezier() has a limit
// of the sweep_angle. If fabs(sweep_angle) exceeds pi/2 the curve
// becomes inaccurate. But slight exceeding is quite appropriate.
static const double bezier_arc_angle_epsilon = 0.01;

static void arc_to_bezier(
  double cx, double cy,
  double rx, double ry,
  double start_angle,
  double sweep_angle,
  Path::Vertex* dst)
{
  sweep_angle /= 2.0;

  double x0 = cos(sweep_angle);
  double y0 = sin(sweep_angle);
  double tx = (1.0 - x0) * (4.0 / 3.0);
  double ty = y0 - tx * x0 / y0;
  double px[4];
  double py[4];

  px[0] =  x0;
  py[0] = -y0;
  px[1] =  x0 + tx;
  py[1] = -ty;
  px[2] =  x0 + tx;
  py[2] =  ty;
  px[3] =  x0;
  py[3] =  y0;

  double sn = sin(start_angle + sweep_angle);
  double cs = cos(start_angle + sweep_angle);

  for (sysuint_t i = 0; i < 4; i++)
  {
    dst[i].cmd = Path::CmdCurve4;
    dst[i].x = cx + rx * (px[i] * cs - py[i] * sn);
    dst[i].y = cy + ry * (px[i] * sn + py[i] * cs);
  }
}

err_t Path::_arcTo(double cx, double cy, double rx, double ry, double start, double sweep, uint initialCommand, bool closePath)
{
  start = fmod(start, 2.0 * M_PI);

  if (sweep >=  2.0 * M_PI) sweep =  2.0 * M_PI;
  if (sweep <= -2.0 * M_PI) sweep = -2.0 * M_PI;

  // Degenerated.
  if (fabs(sweep) < 1e-10)
  {
    Vertex* v = _add(2);
    if (!v) return Error::OutOfMemory;

    v[0].cmd = initialCommand;
    v[0].x = cx + rx * cos(start);
    v[0].y = cx + ry * sin(start);
    v[1].cmd = CmdLineTo;
    v[1].x = cx + rx * cos(start + sweep);
    v[1].y = cx + ry * sin(start + sweep);
  }
  else
  {
    Vertex* v = _add(13);
    if (!v) return Error::OutOfMemory;

    Vertex* vstart = v;
    Vertex* vend = v + 13;

    double total_sweep = 0.0;
    double local_sweep = 0.0;
    double prev_sweep;
    bool done = false;

    v++;

    do {
      if (sweep < 0.0)
      {
        prev_sweep   = total_sweep;
        local_sweep  = -M_PI * 0.5;
        total_sweep -=  M_PI * 0.5;

        if (total_sweep <= sweep + bezier_arc_angle_epsilon)
        {
          local_sweep = sweep - prev_sweep;
          done = true;
        }
      }
      else
      {
        prev_sweep   = total_sweep;
        local_sweep  = M_PI * 0.5;
        total_sweep += M_PI * 0.5;

        if (total_sweep >= sweep - bezier_arc_angle_epsilon)
        {
          local_sweep = sweep - prev_sweep;
          done = true;
        }
      }

      arc_to_bezier(cx, cy, rx, ry, start, local_sweep, v-1);
      v += 3;
      start += local_sweep;
    } while (!done && v < vend);

    // Setup initial command, path length and set type to CurveType.
    vstart[0].cmd = initialCommand;
    _d->length = (sysuint_t)(v - _d->data);
    _d->type = CurveType;
  }

  if (closePath) return closePolygon();
}

err_t Path::arcTo(double cx, double cy, double rx, double ry, double start, double sweep)
{
  return _arcTo(cx, cy, rx, ry, start, sweep, CmdLineTo, false);
}

err_t Path::arcToRel(double cx, double cy, double rx, double ry, double start, double sweep)
{
  relToAbsInline(_d, &cx, &cy);
  return _arcTo(cx, cy, rx, ry, start, sweep, CmdLineTo, false);
}

// ============================================================================
// [Fog::Path - BezierTo]
// ============================================================================

err_t Path::curveTo(double cx, double cy, double tx, double ty)
{
  Vertex* v = _add(2);
  if (!v) return Error::OutOfMemory;

  v[0].cmd = CmdCurve3;
  v[0].x = cx;
  v[0].y = cy;
  v[1].cmd = CmdCurve3;
  v[1].x = tx;
  v[1].y = ty;

  _d->type = CurveType;
  return Error::Ok;
}

err_t Path::curveToRel(double cx, double cy, double tx, double ty)
{
  relToAbsInline(_d, &cx, &cy, &tx, &ty);
  return curveTo(cx, cy, tx, ty);
}

err_t Path::curveTo(double tx, double ty)
{
  Vertex* endv = _d->data + _d->length;

  if (_d->length && endv[-1].cmd.isVertex())
  {
    double cx = endv[-1].x;
    double cy = endv[-1].y;

    if (_d->length >= 2 && endv[-2].cmd.isCurve())
    {
      cx += endv[-1].x - endv[-2].x;
      cy += endv[-1].y - endv[-2].y;
    }

    return curveTo(cx, cy, tx, ty);
  }
  else
  {
    return Error::Ok;
  }
}

err_t Path::curveToRel(double tx, double ty)
{
  relToAbsInline(_d, &tx, &ty);
  return curveTo(tx, ty);
}

// ============================================================================
// [Fog::Path - CubicTo]
// ============================================================================

err_t Path::cubicTo(double cx1, double cy1, double cx2, double cy2, double tx, double ty)
{
  Vertex* v = _add(3);
  if (!v) return Error::OutOfMemory;

  v[0].cmd = CmdCurve4;
  v[0].x = cx1;
  v[0].y = cy1;
  v[1].cmd = CmdCurve4;
  v[1].x = cx2;
  v[1].y = cy2;
  v[2].cmd = CmdCurve4;
  v[2].x = tx;
  v[2].y = ty;

  _d->type = CurveType;
  return Error::Ok;
}

err_t Path::cubicToRel(double cx1, double cy1, double cx2, double cy2, double tx, double ty)
{
  relToAbsInline(_d, &cx1, &cy1, &cx2, &cy2, &tx, &ty);
  return cubicTo(cx1, cy1, cx2, cy2, tx, ty);
}

err_t Path::cubicTo(double cx2, double cy2, double tx, double ty)
{
  Vertex* endv = _d->data + _d->length;

  if (_d->length && endv[-1].cmd.isVertex())
  {
    double cx1 = endv[-1].x;
    double cy1 = endv[-1].y;

    if (_d->length >= 2 && endv[-2].cmd.isCurve())
    {
      cx1 += endv[-1].x - endv[-2].x;
      cy1 += endv[-1].y - endv[-2].y;
    }

    return cubicTo(cx1, cy1, cx2, cy2, tx, ty);
  }
  else
  {
    return Error::Ok;
  }
}

err_t Path::cubicToRel(double cx2, double cy2, double tx, double ty)
{
  relToAbsInline(_d, &cx2, &cy2, &tx, &ty);
  return cubicTo(cx2, cy2, tx, ty);
}

// ============================================================================
// [Fog::Path - FlipX / FlipY]
// ============================================================================

err_t Path::flipX(double x1, double x2)
{
  if (!_d->length) return Error::Ok;

  err_t err = detach();
  if (err) return err;

  sysuint_t i, len = _d->length;
  Vertex* v = _d->data;
  
  for (i = 0; i < len; i++)
  {
    if (v[i].cmd.isVertex()) v[i].x = x2 - v[i].x + x1;
  }

  return Error::Ok;
}

err_t Path::flipY(double y1, double y2)
{
  if (!_d->length) return Error::Ok;

  err_t err = detach();
  if (err) return err;

  sysuint_t i, len = _d->length;
  Vertex* v = _d->data;
  
  for (i = 0; i < len; i++)
  {
    if (v[i].cmd.isVertex()) v[i].y = y2 - v[i].y + y1;
  }

  return Error::Ok;
}

// ============================================================================
// [Fog::Path - Translate]
// ============================================================================

err_t Path::translate(double dx, double dy)
{
  if (!_d->length) return Error::Ok;

  err_t err = detach();
  if (err) return err;
  
  sysuint_t i, len = _d->length;
  Vertex* v = _d->data;

  for (i = 0; i < len; i++)
  {
    if (v[i].cmd.isVertex())
    {
      v[i].x += dx;
      v[i].y += dy;
    }
  }

  return Error::Ok;
}

err_t Path::translate(double dx, double dy, sysuint_t pathId)
{
  if (!_d->length) return Error::Ok;

  err_t err = detach();
  if (err) return err;

  sysuint_t i, len = _d->length;
  Vertex* v = _d->data;

  for (i = pathId; i < len; i++)
  {
    if (v[i].cmd.isStop()) break;
    if (v[i].cmd.isVertex())
    {
      v[i].x += dx;
      v[i].y += dy;
    }
  }

  return Error::Ok;
}

// ============================================================================
// [Fog::Path - Scale]
// ============================================================================

err_t Path::scale(double sx, double sy, bool keepStartPos)
{
  if (!_d->length) return Error::Ok;

  err_t err = detach();
  if (err) return err;

  sysuint_t i, len = _d->length;
  Vertex* v = _d->data;

  if (keepStartPos)
  {
    double px = v[0].x;
    double py = v[0].y;

    for (i = 1; i < len; i++)
    {
      if (v[i].cmd.isVertex())
      {
        if (v[i].x < px) px = v[i].x;
        if (v[i].y < py) py = v[i].y;
      }
    }

    for (i = 0; i < len; i++)
    {
      if (v[i].cmd.isVertex())
      {
        v[i].x = (v[i].x - px) * sx + px;
        v[i].y = (v[i].y - py) * sy + py;
      }
    }
  }
  else
  {
    for (i = 0; i < len; i++)
    {
      if (v[i].cmd.isVertex())
      {
        v[i].x *= sx;
        v[i].y *= sy;
      }
    }
  }

  return Error::Ok;
}

// ============================================================================
// [Fog::Path - ApplyMatrix]
// ============================================================================

err_t Path::applyMatrix(const AffineMatrix& matrix)
{
  if (!_d->length) return Error::Ok;

  err_t err = detach();
  if (err) return err;

  sysuint_t i, len = _d->length;
  Vertex* v = _d->data;

  for (i = 0; i < len; i++)
  {
    if (v[i].cmd.isVertex()) matrix.transform(&v[i].x, &v[i].y);
  }

  return Error::Ok;
}

// ============================================================================
// [Fog::Path - Add]
// ============================================================================

err_t Path::addRect(const RectF& r)
{
  if (!r.isValid()) return Error::Ok;

  Vertex* v = _add(5);
  if (!v) return Error::OutOfMemory;

  v[0].cmd = CmdMoveTo;
  v[0].x = r.x1();
  v[0].y = r.y1();
  v[1].cmd = CmdLineTo;
  v[1].x = r.x2();
  v[1].y = r.y1();
  v[2].cmd = CmdLineTo;
  v[2].x = r.x2();
  v[2].y = r.y2();
  v[3].cmd = CmdLineTo;
  v[3].x = r.x1();
  v[3].y = r.y2();
  v[4].cmd = CmdEndPoly | CFlagClose;
  v[4].x = 0.0;
  v[4].y = 0.0;

  return Error::Ok;
}

err_t Path::addRects(const RectF* r, sysuint_t count)
{
  if (!count) return Error::Ok;
  FOG_ASSERT(r);

  Vertex* v = _add(count * 5);
  if (!v) return Error::OutOfMemory;

  for (sysuint_t i = 0; i < count; i++, r++)
  {
    if (!r->isValid()) continue;

    v[0].cmd = CmdMoveTo;
    v[0].x = r->x1();
    v[0].y = r->y1();
    v[1].cmd = CmdLineTo;
    v[1].x = r->x2();
    v[1].y = r->y1();
    v[2].cmd = CmdLineTo;
    v[2].x = r->x2();
    v[2].y = r->y2();
    v[3].cmd = CmdLineTo;
    v[3].x = r->x1();
    v[3].y = r->y2();
    v[4].cmd = CmdEndPoly | CFlagClose;
    v[4].x = 0.0;
    v[4].y = 0.0;

    v += 5;
  }

  // Return and update path length.
  _d->length = (sysuint_t)(v - _d->data);
  return Error::Ok;
}

err_t Path::addRound(const RectF& r, const PointF& radius)
{
  if (!r.isValid()) return Error::Ok;

  double rw2 = r.w() / 2.0;
  double rh2 = r.h() / 2.0;

  double rx = fabs(radius.x());
  double ry = fabs(radius.y());

  if (rx > rw2) rx = rw2;
  if (ry > rh2) ry = rh2;

  if (rx == 0 || ry == 0)
    return addRect(r);

  double x1 = r.x();
  double y1 = r.y();
  double x2 = r.x() + r.width();
  double y2 = r.y() + r.height();

  err_t err = Error::Ok;

  err |= moveTo(x1 + rx, y1);
  err |= lineTo(x2 - rx, y1);
  err |= arcTo(x2 - rx, y1 + ry, rx, ry, M_PI * 1.5, M_PI * 0.5);

  err |= lineTo(x2, y2 - ry);
  err |= arcTo(x2 - rx, y2 - ry, rx, ry, M_PI * 0.0, M_PI * 0.5);

  err |= lineTo(x1 + rx, y2);
  err |= arcTo(x1 + rx, y2 - ry, rx, ry, M_PI * 0.5, M_PI * 0.5);

  err |= lineTo(x1, y1 + ry);
  err |= arcTo(x1 + rx, y1 + ry, rx, ry, M_PI * 1.0, M_PI * 0.5);

  err |= closePolygon();

  return err;
}

err_t Path::addEllipse(const RectF& r)
{
  if (!r.isValid()) return Error::Ok;

  double rx = r.width() / 2.0;
  double ry = r.height() / 2.0;
  double cx = r.x() + rx;
  double cy = r.y() + ry;

  return _arcTo(cx, cy, rx, ry, 0.0, 2.0 * M_PI, CmdMoveTo, true);
}

err_t Path::addEllipse(const PointF& cp, const PointF& r)
{
  return _arcTo(cp.x(), cp.y(), r.x(), r.y(), 0.0, 2.0 * M_PI, CmdMoveTo, true);
}

err_t Path::addArc(const RectF& r, double start, double sweep)
{
  if (!r.isValid()) return Error::Ok;

  double rx = r.width() / 2.0;
  double ry = r.height() / 2.0;
  double cx = r.x() + rx;
  double cy = r.y() + ry;

  return _arcTo(cx, cy, rx, ry, start, sweep, CmdMoveTo, false);
}

err_t Path::addArc(const PointF& cp, const PointF& r, double start, double sweep)
{
  return _arcTo(cp.x(), cp.y(), r.x(), r.y(), start, sweep, CmdMoveTo, false);
}

err_t Path::addChord(const RectF& r, double start, double sweep)
{
  if (!r.isValid()) return Error::Ok;

  double rx = r.width() / 2.0;
  double ry = r.height() / 2.0;
  double cx = r.x() + rx;
  double cy = r.y() + ry;

  return _arcTo(cx, cy, rx, ry, start, sweep, CmdMoveTo, true);
}

err_t Path::addChord(const PointF& cp, const PointF& r, double start, double sweep)
{
  return _arcTo(cp.x(), cp.y(), r.x(), r.y(), start, sweep, CmdMoveTo, true);
}

err_t Path::addPie(const RectF& r, double start, double sweep)
{
  if (!r.isValid()) return Error::Ok;

  double rx = r.width() / 2.0;
  double ry = r.height() / 2.0;
  double cx = r.x() + rx;
  double cy = r.y() + ry;

  return addPie(PointF(cx, cy), PointF(rx, ry), start, sweep);
}

err_t Path::addPie(const PointF& cp, const PointF& r, double start, double sweep)
{
  if (sweep >= M_PI*2.0) return addEllipse(cp, r);

  start = fmod(start, M_PI*2.0);
  if (start < 0) start += M_PI*2.0;

  err_t err;

  if ( (err = moveTo(cp.x(), cp.y())) ) return err;
  if ( (err = _arcTo(cp.x(), cp.y(), r.x(), r.y(), start, sweep, CmdLineTo, true)) ) return err;

  return Error::Ok;
}

err_t Path::addPath(const Path& path)
{
  sysuint_t count = path.length();
  if (count == 0) return Error::Ok;

  uint32_t t = fog_max(type(), path.type());

  Vertex* v = _add(count);
  if (!v) return Error::OutOfMemory;

  const Vertex* src = path.cData();

  for (sysuint_t i = 0; i < count; i++, v++, src++)
  {
    v->cmd = src->cmd;
    v->x   = src->x;
    v->y   = src->y;
  }
  _d->type = t;

  return Error::Ok;
}

// ============================================================================
// [Fog::Path - Curves Approximation]
// ============================================================================

#define ADD_VERTEX(_cmd, _x, _y) { v->x = _x; v->y = _y; v->cmd = _cmd; v++; }

#define APPROXIMATE_CURVE3_RECURSION_LIMIT 32

static const double curve_distance_epsilon = 1e-30;
static const double curve_collinearity_epsilon = 1e-30;
static const double curve_angle_tolerance_epsilon = 0.01;

static FOG_INLINE double squareDistance(double x1, double y1, double x2, double y2)
{
  double dx = x2 - x1;
  double dy = y2 - y1;
  return dx * dx + dy * dy;
}

struct ApproximateCurve3Data
{
  double x1;
  double y1;
  double x2;
  double y2;
  double x3;
  double y3;
};

struct ApproximateCurve4Data
{
  double x1;
  double y1;
  double x2;
  double y2;
  double x3;
  double y3;
  double x4;
  double y4;
};

static err_t approximateCurve3(
  Path& dst,
  double x1, double y1,
  double x2, double y2,
  double x3, double y3,
  double approximationScale,
  double angleTolerance = 0.0)
{
  double distanceToleranceSquare = 0.5 / approximationScale;
  distanceToleranceSquare *= distanceToleranceSquare;

  sysuint_t level = 0;
  ApproximateCurve3Data stack[APPROXIMATE_CURVE3_RECURSION_LIMIT];

  Path::Vertex* v = dst._add(APPROXIMATE_CURVE3_RECURSION_LIMIT * 2 + 1);
  if (!v) return Error::OutOfMemory;

  for (;;)
  {
    // Calculate all the mid-points of the line segments
    double x12   = (x1 + x2) / 2.0;
    double y12   = (y1 + y2) / 2.0;
    double x23   = (x2 + x3) / 2.0;
    double y23   = (y2 + y3) / 2.0;
    double x123  = (x12 + x23) / 2.0;
    double y123  = (y12 + y23) / 2.0;

    double dx = x3-x1;
    double dy = y3-y1;
    double d = fabs(((x2 - x3) * dy - (y2 - y3) * dx));
    double da;

    if (d > curve_collinearity_epsilon)
    {
      // Regular case
      if (d * d <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        // If the curvature doesn't exceed the distance_tolerance value
        // we tend to finish subdivisions.
        if (angleTolerance < curve_angle_tolerance_epsilon)
        {
          ADD_VERTEX(Path::CmdLineTo, x123, y123);
          goto ret;
        }

        // Angle & Cusp Condition
        da = fabs(atan2(y3 - y2, x3 - x2) - atan2(y2 - y1, x2 - x1));
        if (da >= M_PI) da = 2.0 * M_PI - da;

        if (da < angleTolerance)
        {
          // Finally we can stop the recursion
          ADD_VERTEX(Path::CmdLineTo, x123, y123);
          goto ret;
        }
      }
    }
    else
    {
      // Collinear case
      da = dx*dx + dy*dy;
      if (da == 0)
      {
        d = squareDistance(x1, y1, x2, y2);
      }
      else
      {
        d = ((x2 - x1)*dx + (y2 - y1)*dy) / da;

        if (d > 0 && d < 1)
        {
          // Simple collinear case, 1---2---3
          // We can leave just two endpoints
          goto ret;
        }

        if (d <= 0)
          d = squareDistance(x2, y2, x1, y1);
        else if (d >= 1)
          d = squareDistance(x2, y2, x3, y3);
        else
          d = squareDistance(x2, y2, x1 + d*dx, y1 + d*dy);
      }
      if (d < distanceToleranceSquare)
      {
        ADD_VERTEX(Path::CmdLineTo, x2, y2);
        goto ret;
      }
    }

    // Continue subdivision
    //
    // Original code from antigrain was:
    //   recursive_bezier(x1, y1, x12, y12, x123, y123, level + 1);
    //   recursive_bezier(x123, y123, x23, y23, x3, y3, level + 1);
    //
    // First recursive subdivision will be set into x1, y1, x2, y2, x3, y3,
    // second subdivision will be added into stack.

    if (level < APPROXIMATE_CURVE3_RECURSION_LIMIT)
    {
      stack[level].x1 = x123;
      stack[level].y1 = y123;
      stack[level].x2 = x23;
      stack[level].y2 = y23;
      stack[level].x3 = x3;
      stack[level].y3 = y3;
      level++;

      x2 = x12;
      y2 = y12;
      x3 = x123;
      y3 = y123;

      continue;
    }

ret:
    if (level == 0) break;

    level--;
    x1 = stack[level].x1;
    y1 = stack[level].y1;
    x2 = stack[level].x2;
    y2 = stack[level].y2;
    x3 = stack[level].x3;
    y3 = stack[level].y3;
  }

  // Add end point.
  ADD_VERTEX(Path::CmdLineTo, x3, y3);

  dst._d->length = (sysuint_t)(v - dst._d->data);
  return Error::Ok;
}

static err_t approximateCurve4(
  Path& dst,
  double x1, double y1,
  double x2, double y2,
  double x3, double y3,
  double x4, double y4,
  double approximationScale,
  double angleTolerance = 0.0,
  double cuspLimit = 0.0)
{
  double distanceToleranceSquare = 0.5 / approximationScale;
  distanceToleranceSquare *= distanceToleranceSquare;

  sysuint_t level = 0;
  ApproximateCurve4Data stack[APPROXIMATE_CURVE3_RECURSION_LIMIT];

  Path::Vertex* v = dst._add(APPROXIMATE_CURVE3_RECURSION_LIMIT * 4 + 1);
  if (!v) return Error::OutOfMemory;

  for (;;)
  {
    // Calculate all the mid-points of the line segments
    double x12   = (x1 + x2) / 2.0;
    double y12   = (y1 + y2) / 2.0;
    double x23   = (x2 + x3) / 2.0;
    double y23   = (y2 + y3) / 2.0;
    double x34   = (x3 + x4) / 2.0;
    double y34   = (y3 + y4) / 2.0;
    double x123  = (x12 + x23) / 2.0;
    double y123  = (y12 + y23) / 2.0;
    double x234  = (x23 + x34) / 2.0;
    double y234  = (y23 + y34) / 2.0;
    double x1234 = (x123 + x234) / 2.0;
    double y1234 = (y123 + y234) / 2.0;

    // Try to approximate the full cubic curve by a single straight line
    double dx = x4-x1;
    double dy = y4-y1;

    double d2 = fabs(((x2 - x4) * dy - (y2 - y4) * dx));
    double d3 = fabs(((x3 - x4) * dy - (y3 - y4) * dx));
    double da1, da2, k;

    switch ((int(d2 > curve_collinearity_epsilon) << 1) +
             int(d3 > curve_collinearity_epsilon))
    {
      // All collinear OR p1==p4
      case 0:
        k = dx*dx + dy*dy;
        if (k == 0)
        {
          d2 = squareDistance(x1, y1, x2, y2);
          d3 = squareDistance(x4, y4, x3, y3);
        }
        else
        {
          k   = 1 / k;
          da1 = x2 - x1;
          da2 = y2 - y1;
          d2  = k * (da1 * dx + da2 * dy);
          da1 = x3 - x1;
          da2 = y3 - y1;
          d3  = k * (da1 * dx + da2 * dy);

          if (d2 > 0 && d2 < 1 && d3 > 0 && d3 < 1)
          {
            // Simple collinear case, 1---2---3---4
            // We can leave just two endpoints
            goto ret;
          }

          if (d2 <= 0)
            d2 = squareDistance(x2, y2, x1, y1);
          else if (d2 >= 1)
            d2 = squareDistance(x2, y2, x4, y4);
          else
            d2 = squareDistance(x2, y2, x1 + d2*dx, y1 + d2*dy);

          if (d3 <= 0)
            d3 = squareDistance(x3, y3, x1, y1);
          else if (d3 >= 1)
            d3 = squareDistance(x3, y3, x4, y4);
          else
            d3 = squareDistance(x3, y3, x1 + d3*dx, y1 + d3*dy);
        }

        if (d2 > d3)
        {
          if (d2 < distanceToleranceSquare)
          {
            ADD_VERTEX(Path::CmdLineTo, x2, y2);
            goto ret;
          }
        }
        else
        {
          if (d3 < distanceToleranceSquare)
          {
            ADD_VERTEX(Path::CmdLineTo, x3, y3);
            goto ret;
          }
        }
        break;

    // p1,p2,p4 are collinear, p3 is significant
    case 1:
      if (d3 * d3 <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        if (angleTolerance < curve_angle_tolerance_epsilon)
        {
          ADD_VERTEX(Path::CmdLineTo, x23, y23);
          goto ret;
        }

        // Angle Condition
        da1 = fabs(atan2(y4 - y3, x4 - x3) - atan2(y3 - y2, x3 - x2));
        if (da1 >= M_PI) da1 = 2.0 * M_PI - da1;

        if (da1 < angleTolerance)
        {
          ADD_VERTEX(Path::CmdLineTo, x2, y2);
          ADD_VERTEX(Path::CmdLineTo, x3, y3);
          goto ret;
        }

        if (cuspLimit != 0.0)
        {
          if (da1 > cuspLimit)
          {
            ADD_VERTEX(Path::CmdLineTo, x3, y3);
            goto ret;
          }
        }
      }
      break;

    // p1,p3,p4 are collinear, p2 is significant
    case 2:
      if (d2 * d2 <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        if (angleTolerance < curve_angle_tolerance_epsilon)
        {
          ADD_VERTEX(Path::CmdLineTo, x23, y23);
          goto ret;
        }

        // Angle Condition
        da1 = fabs(atan2(y3 - y2, x3 - x2) - atan2(y2 - y1, x2 - x1));
        if (da1 >= M_PI) da1 = 2.0 * M_PI - da1;

        if (da1 < angleTolerance)
        {
          ADD_VERTEX(Path::CmdLineTo, x2, y2);
          ADD_VERTEX(Path::CmdLineTo, x3, y3);
          goto ret;
        }

        if (cuspLimit != 0.0)
        {
          if (da1 > cuspLimit)
          {
            ADD_VERTEX(Path::CmdLineTo, x3, y3);
            goto ret;
          }
        }
      }
      break;

    // Regular case
    case 3:
      if ((d2 + d3)*(d2 + d3) <= distanceToleranceSquare * (dx*dx + dy*dy))
      {
        // If the curvature doesn't exceed the distance_tolerance value
        // we tend to finish subdivisions.
        if (angleTolerance < curve_angle_tolerance_epsilon)
        {
          ADD_VERTEX(Path::CmdLineTo, x23, y23);
          goto ret;
        }

        // Angle & Cusp Condition
        k   = atan2(y3 - y2, x3 - x2);
        da1 = fabs(k - atan2(y2 - y1, x2 - x1));
        da2 = fabs(atan2(y4 - y3, x4 - x3) - k);
        if (da1 >= M_PI) da1 = 2.0 * M_PI - da1;
        if (da2 >= M_PI) da2 = 2.0 * M_PI - da2;

        if (da1 + da2 < angleTolerance)
        {
          // Finally we can stop the recursion
          ADD_VERTEX(Path::CmdLineTo, x23, y23);
          goto ret;
        }

        if (cuspLimit != 0.0)
        {
          if (da1 > cuspLimit)
          {
            ADD_VERTEX(Path::CmdLineTo, x2, y2);
            goto ret;
          }

          if (da2 > cuspLimit)
          {
            ADD_VERTEX(Path::CmdLineTo, x3, y3);
            goto ret;
          }
        }
      }
      break;
    }

    // Continue subdivision
    //
    // Original antigrain code:
    //   recursive_bezier(x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1);
    //   recursive_bezier(x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1);
    //
    // First recursive subdivision will be set into x1, y1, x2, y2, x3, y3,
    // second subdivision will be added into stack.
    if (level < APPROXIMATE_CURVE3_RECURSION_LIMIT)
    {
      stack[level].x1 = x1234;
      stack[level].y1 = y1234;
      stack[level].x2 = x234;
      stack[level].y2 = y234;
      stack[level].x3 = x34;
      stack[level].y3 = y34;
      stack[level].x4 = x4;
      stack[level].y4 = y4;
      level++;

      x2 = x12;
      y2 = y12;
      x3 = x123;
      y3 = y123;
      x4 = x1234;
      y4 = y1234;

      continue;
    }

ret:
    if (level == 0) break;

    level--;
    x1 = stack[level].x1;
    y1 = stack[level].y1;
    x2 = stack[level].x2;
    y2 = stack[level].y2;
    x3 = stack[level].x3;
    y3 = stack[level].y3;
    x4 = stack[level].x4;
    y4 = stack[level].y4;
  }

  // Add end point.
  ADD_VERTEX(Path::CmdLineTo, x4, y4);

  dst._d->length = (sysuint_t)(v - dst._d->data);
  return Error::Ok;
}

// ============================================================================
// [Fog::Path - Flatten]
// ============================================================================

err_t Path::flatten()
{
  return flatten(NULL, 1.0);
}

err_t Path::flatten(const AffineMatrix* matrix, double approximationScale)
{
  if (type() == LineType) return Error::Ok;
  return flattenTo(*this, matrix, approximationScale);
}

err_t Path::flattenTo(Path& dst, const AffineMatrix* matrix, double approximationScale) const
{
  // --------------------------------------------------------------------------
  // Contains only lines (already flattened)
  // --------------------------------------------------------------------------

  if (type() == LineType)
  {
    if (this == &dst)
    {
      if (matrix != NULL) dst.applyMatrix(*matrix);
      return Error::Ok;
    }
    else
    {
      dst = *this;
      if (matrix != NULL) dst.applyMatrix(*matrix);
      return Error::Ok;
    }
  }

  // --------------------------------------------------------------------------
  // Contains curves
  // --------------------------------------------------------------------------

  // The dst argument is here mainly if we need to flatten path into different
  // one. If paths are equal, we will simply create second path and assign
  // result to first one.
  if (this == &dst)
  {
    Path tmp;
    err_t err = flattenTo(tmp, matrix, approximationScale);
    dst = tmp;
    return err;
  }

  dst.clear();

  sysuint_t n = length();
  if (n == 0) return Error::Ok;
  if (dst.reserve(n * 8)) return Error::OutOfMemory;

  double lastx = 0.0;
  double lasty = 0.0;

  const Vertex* v = cData();
  Vertex* dstv;
  err_t err;

ensureSpace:
  dstv = dst._add(n);
  if (!dstv) return Error::OutOfMemory;

  do {
    switch (v->cmd.cmd())
    {
      case CmdMoveTo:
      case CmdLineTo:
        dstv->x = lastx = v->x;
        dstv->y = lasty = v->y;
        dstv->cmd = v->cmd;

        v++;
        dstv++;
        n--;

        break;

      case CmdCurve3:
        if (n <= 1) goto invalid;
        if (v[1].cmd.cmd() != CmdCurve3) goto invalid;

        // Finalize path
        dst._d->length = (sysuint_t)(dstv - dst._d->data);

        // Approximate curve.
        err = approximateCurve3(dst, lastx, lasty, v[0].x, v[0].y, v[1].x, v[1].y, approximationScale);
        if (err) return err;

        lastx = v[1].x;
        lasty = v[1].y;

        v += 2;
        n -= 2;

        if (n == 0)
          return Error::Ok;
        else
          goto ensureSpace;

      case CmdCurve4:
        if (n <= 2) goto invalid;
        if (v[1].cmd.cmd() != CmdCurve4 ||
            v[2].cmd.cmd() != CmdCurve4) goto invalid;

        // Finalize path
        dst._d->length = (sysuint_t)(dstv - dst._d->data);

        // Approximate curve.
        err = approximateCurve4(dst, lastx, lasty, v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y, approximationScale);
        if (err) return err;

        lastx = v[2].x;
        lasty = v[2].y;

        v += 3;
        n -= 3;

        if (n == 0)
          return Error::Ok;
        else
          goto ensureSpace;

      case CmdCatrom:
      {
        if (n <= 2) goto invalid;
        if (v[1].cmd.cmd() != CmdCatrom ||
            v[2].cmd.cmd() != CmdCatrom) goto invalid;

        // Finalize path
        dst._d->length = (sysuint_t)(dstv - dst._d->data);

        double x1 = lastx;
        double y1 = lasty;
        double x2 = v[0].x;
        double y2 = v[0].y;
        double x3 = v[1].x;
        double y3 = v[1].y;
        double x4 = v[2].x;
        double y4 = v[2].y;

        // Trans. matrix Catmull-Rom to Bezier
        //
        //  0       1       0       0
        //  -1/6    1       1/6     0
        //  0       1/6     1       -1/6
        //  0       0       1       0
        err = approximateCurve4(dst,
          x2,
          y2,
          (-x1 + 6*x2 + x3) / 6,
          (-y1 + 6*y2 + y3) / 6,
          ( x2 + 6*x3 - x4) / 6,
          ( y2 + 6*y3 - y4) / 6,
          x3,
          y3,
          approximationScale);

        lastx = x4;
        lasty = y4;

        v += 3;
        n -= 3;

        if (n == 0)
          return Error::Ok;
        else
          goto ensureSpace;
      }

      case CmdUBSpline:
      {
        if (n <= 2) goto invalid;
        if (v[1].cmd.cmd() != CmdUBSpline ||
            v[2].cmd.cmd() != CmdUBSpline) goto invalid;

        // Finalize path
        dst._d->length = (sysuint_t)(dstv - dst._d->data);

        double x1 = lastx;
        double y1 = lasty;
        double x2 = v[0].x;
        double y2 = v[0].y;
        double x3 = v[1].x;
        double y3 = v[1].y;
        double x4 = v[2].x;
        double y4 = v[2].y;

        lastx = (    x2 + 4.0 * x3 + x4) / 6.0;
        lasty = (    y2 + 4.0 * y3 + y4) / 6.0;

        // Trans. matrix Uniform BSpline to Bezier
        //
        //  1/6     4/6     1/6     0
        //  0       4/6     2/6     0
        //  0       2/6     4/6     0
        //  0       1/6     4/6     1/6
        err = approximateCurve4(dst,
          (    x1 + 4.0 * x2 + x3) / 6.0,
          (    y1 + 4.0 * y2 + y3) / 6.0,
          (4 * x2 + 2.0 * x3     ) / 6.0,
          (4 * y2 + 2.0 * y3     ) / 6.0,
          (2 * x2 + 4.0 * x3     ) / 6.0,
          (2 * y2 + 4.0 * y3     ) / 6.0,
          lastx, lasty,
          approximationScale);

        v += 3;
        n -= 3;

        if (n == 0)
          return Error::Ok;
        else
          goto ensureSpace;
      }

      default:
        dstv->x = lastx = 0.0;
        dstv->y = lasty = 0.0;
        dstv->cmd = v->cmd;

        v++;
        dstv++;
        n--;

        break;
    }
  } while(n);

  dst._d->length = (sysuint_t)(dstv - dst._d->data);
  dst._d->type = LineType;
  if (matrix) dst.applyMatrix(*matrix);
  return Error::Ok;

invalid:
  dst._d->length = 0;
  dst._d->type = LineType;
  return Error::InvalidPath;
}

// ============================================================================
// [Fog::Path - Dash]
// ============================================================================

err_t Path::dash(const Vector<double>& dashes, double startOffset, double approximationScale)
{
  return dashTo(*this, dashes, startOffset, approximationScale);
}

err_t Path::dashTo(Path& dst, const Vector<double>& dashes, double startOffset, double approximationScale)
{
  if (type() != LineType)
  {
    Path tmp;
    flattenTo(tmp, NULL, approximationScale);
    return tmp.dashTo(dst, dashes, startOffset, approximationScale);
  }
  else
  {
    AggPath src(*this);

    agg::conv_dash<AggPath, agg::vcgen_dash> dasher(src);

    Vector<double>::ConstIterator it(dashes);
    for (;;)
    {
      double d1 = it.value(); it.toNext();
      if (!it.isValid()) break;
      double d2 = it.value(); it.toNext();
      dasher.add_dash(d1, d2);
      if (!it.isValid()) break;
    }
    dasher.dash_start(startOffset);

    if (this == &dst)
    {
      Path tmp;
      err_t err = concatToPath(tmp, dasher);
      if (err) return err;
      return dst.set(tmp);
    }
    else
    {
      dst.clear();
      return concatToPath(dst, dasher);
    }
  }
}

// ============================================================================
// [Fog::Path - Stroke]
// ============================================================================

err_t Path::stroke(const StrokeParams& strokeParams, double approximationScale)
{
  return strokeTo(*this, strokeParams);
}

err_t Path::strokeTo(Path& dst, const StrokeParams& strokeParams, double approximationScale) const
{
  if (type() != LineType)
  {
    Path tmp;
    flattenTo(tmp, NULL, approximationScale);
    return tmp.strokeTo(dst, strokeParams, approximationScale);
  }
  else
  {
    AggPath src(*this);

    agg::conv_stroke<AggPath> stroker(src);
    stroker.width(strokeParams.lineWidth);
    stroker.miter_limit(strokeParams.miterLimit);
    stroker.line_join(static_cast<agg::line_join_e>(strokeParams.lineJoin));
    stroker.line_cap(static_cast<agg::line_cap_e>(strokeParams.lineCap));
    stroker.approximation_scale(approximationScale);

    if (this == &dst)
    {
      Path tmp;
      err_t err = concatToPath(tmp, stroker);
      if (err) return err;
      return dst.set(tmp);
    }
    else
    {
      dst.clear();
      return concatToPath(dst, stroker);
    }
  }
}

// ============================================================================
// [Fog::Path - Operator Overload]
// ============================================================================

Path& Path::operator=(const Path& other)
{
  set(other);
  return *this;
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_path_init(void)
{
  Fog::Path::Data* d = Fog::Path::sharedNull.instancep();

  d->refCount.init(1);
  d->flags |= Fog::Path::Data::IsNull | Fog::Path::Data::IsSharable;
  d->type = Fog::Path::LineType;
  d->capacity = 0;
  d->length = 0;

  d->data[0].cmd._cmd = Fog::Path::CmdStop;
  d->data[0].x = 0.0;
  d->data[0].y = 0.0;

  return Error::Ok;
}

FOG_INIT_DECLARE void fog_path_shutdown(void)
{
  Fog::Path::sharedNull->refCount.dec();
}
