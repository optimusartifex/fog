// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Pattern.h>

namespace Fog {

// ============================================================================
// [Fog::Pattern]
// ============================================================================

Static<PatternData> Pattern::sharedNull;

Pattern::Pattern() : 
  _d(sharedNull->ref())
{
}

Pattern::Pattern(const Pattern& other) :
  _d(other._d->ref())
{
}

Pattern::~Pattern()
{
  _d->deref();
}

err_t Pattern::_detach()
{
  if (_d->refCount.get() == 1) return ERR_OK;

  PatternData* newd = new(std::nothrow) PatternData(*_d);
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

void Pattern::free()
{
  atomicPtrXchg(&_d, sharedNull->ref())->deref();
}

err_t Pattern::setType(uint32_t type)
{
  if (_d->type == type) return ERR_OK;

  if (!((uint)type <= 0x2U || (type >= PATTERN_LINEAR_GRADIENT && type <= PATTERN_CONICAL_GRADIENT)))
    return ERR_RT_INVALID_ARGUMENT;

  // Optimize gradient switching.
  if ((_d->type & PATTERN_GRADIENT_MASK) != 0 && (type & PATTERN_GRADIENT_MASK) != 0)
  {
    FOG_RETURN_ON_ERROR(detach());
    _d->type = type;
    return ERR_OK;
  }

  // Optimize switching to different pattern type by using previously allocated
  // memory if it's not shared between another Pattern.
  if (_d->refCount.get() == 1)
  {
    _d->deleteResources();
    _d->type = type;

    if (type == PATTERN_TEXTURE)
      _d->obj.texture.init();
    else if (type & PATTERN_GRADIENT_MASK)
      _d->obj.stops.init();
  }
  else
  {
    PatternData* newd = new(std::nothrow) PatternData();
    if (!newd) return ERR_RT_OUT_OF_MEMORY;

    newd->type = type;
    newd->spread = _d->spread;
    newd->points[0] = _d->points[0];
    newd->points[1] = _d->points[1];
    newd->points[2] = _d->points[2];
    newd->radius = 0.0;
    newd->matrix = _d->matrix;
    
    if (type == PATTERN_TEXTURE)
      newd->obj.texture.init();
    else if (type & PATTERN_GRADIENT_MASK)
      newd->obj.stops.init();

    atomicPtrXchg(&_d, newd)->deref();
  }
  return ERR_OK;
}

void Pattern::reset()
{
  if (_d->refCount.get() > 1)
  {
    atomicPtrXchg(&_d, sharedNull->ref())->deref();
  }
  else
  {
    _d->deleteResources();
    _d->type = PATTERN_NULL;
    _d->spread = SPREAD_PAD;
    _d->points[0].set(0.0, 0.0);
    _d->points[1].set(0.0, 0.0);
    _d->points[2].set(0.0, 0.0);
    _d->radius = 0.0;
    _d->matrix.reset();
    _d->obj.argb.instance().set(0x00000000);
  }
}

err_t Pattern::setSpread(uint32_t spread)
{
  if (_d->spread == spread) return ERR_OK;
  if ((uint)spread >= SPREAD_COUNT) return ERR_RT_INVALID_ARGUMENT;

  FOG_RETURN_ON_ERROR(detach());

  _d->spread = spread;
  return ERR_OK;
}

err_t Pattern::setMatrix(const FloatMatrix& matrix)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->matrix = matrix;
  return ERR_OK;
}

err_t Pattern::setMatrix(const DoubleMatrix& matrix)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->matrix = matrix;
  return ERR_OK;
}

err_t Pattern::resetMatrix()
{
  FOG_RETURN_ON_ERROR(detach());

  _d->matrix.reset();
  return ERR_OK;
}

err_t Pattern::translate(float x, float y, uint32_t order)
{
  return translate((double)x, (double)y, order);
}

err_t Pattern::translate(double x, double y, uint32_t order)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->matrix.translate(x, y, order);
  return ERR_OK;
}

err_t Pattern::rotate(float a, uint32_t order)
{
  return rotate((double)a, order);
}

err_t Pattern::rotate(double a, uint32_t order)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->matrix.rotate(a, order);
  return ERR_OK;
}

err_t Pattern::scale(float x, float y, uint32_t order)
{
  return scale((double)x, (double)y, order);

}
err_t Pattern::scale(double x, double y, uint32_t order)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->matrix.scale(x, y, order);
  return ERR_OK;
}

err_t Pattern::skew(float x, float y, uint32_t order)
{
  return skew((double)x, (double)y, order);
}

err_t Pattern::skew(double x, double y, uint32_t order)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->matrix.skew(x, y, order);
  return ERR_OK;
}

err_t Pattern::transform(const FloatMatrix& m, uint32_t order)
{
  return transform(m.toDoubleMatrix(), order);
}

err_t Pattern::transform(const DoubleMatrix& m, uint32_t order)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->matrix.multiply(m, order);
  return ERR_OK;
}

err_t Pattern::setStartPoint(const IntPoint& pt)
{
  return setStartPoint(pt.toDoublePoint());
}

err_t Pattern::setStartPoint(const FloatPoint& pt)
{
  return setStartPoint(pt.toDoublePoint());

}
err_t Pattern::setStartPoint(const DoublePoint& pt)
{
  if (_d->points[0] == pt) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->points[0] = pt;
  return ERR_OK;
}

err_t Pattern::setEndPoint(const IntPoint& pt)
{
  return setEndPoint(pt.toDoublePoint());
}

err_t Pattern::setEndPoint(const FloatPoint& pt)
{
  return setEndPoint(pt.toDoublePoint());
}

err_t Pattern::setEndPoint(const DoublePoint& pt)
{
  if (_d->points[1] == pt) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->points[1] = pt;
  return ERR_OK;
}

err_t Pattern::setPoints(const IntPoint& startPt, const IntPoint& endPt)
{
  return setPoints(startPt.toDoublePoint(), endPt.toDoublePoint());
}

err_t Pattern::setPoints(const FloatPoint& startPt, const FloatPoint& endPt)
{
  return setPoints(startPt.toDoublePoint(), endPt.toDoublePoint());

}
err_t Pattern::setPoints(const DoublePoint& startPt, const DoublePoint& endPt)
{
  if (_d->points[0] == startPt && _d->points[1] == endPt) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->points[0] = startPt;
  _d->points[1] = endPt;
  return ERR_OK;
}

// ============================================================================
// [Fog::Pattern - Solid]
// ============================================================================

Argb Pattern::getColor() const
{
  if (isSolid())
    return _d->obj.argb.instance();
  else
    return Argb(0x00000000);
}

err_t Pattern::setColor(const Argb& argb)
{
  if (isSolid())
  {
    if (_d->obj.argb.instance() == argb) return ERR_OK;
    FOG_RETURN_ON_ERROR(detach());

    _d->obj.argb->set(argb);
    return ERR_OK;
  }
  else
  {
    PatternData* newd = new(std::nothrow) PatternData();
    if (!newd) return ERR_RT_OUT_OF_MEMORY;

    newd->type = PATTERN_SOLID;
    newd->spread = _d->spread;
    newd->points[0] = _d->points[0];
    newd->points[1] = _d->points[1];
    newd->points[2] = _d->points[2];
    newd->radius = 0.0;
    newd->matrix = _d->matrix;
    newd->obj.argb->set(argb);

    atomicPtrXchg(&_d, newd)->deref();
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::Pattern - Texture]
// ============================================================================

Image Pattern::getTexture() const
{
  if (isTexture())
    return _d->obj.texture.instance();
  else
    return Image();
}

err_t Pattern::setTexture(const Image& texture)
{
  if (_d->refCount.get() == 1)
  {
    _d->deleteResources();
    _d->obj.texture.init(texture);

    return ERR_OK;
  }
  else
  {
    PatternData* newd = new(std::nothrow) PatternData();
    if (!newd) return ERR_RT_OUT_OF_MEMORY;

    newd->type = PATTERN_TEXTURE;
    newd->spread = _d->spread;
    newd->points[0] = _d->points[0];
    newd->points[1] = _d->points[1];
    newd->points[2] = _d->points[2];
    newd->radius = 0.0;
    newd->matrix = _d->matrix;
    newd->obj.texture.init(texture);

    atomicPtrXchg(&_d, newd)->deref();
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::Pattern - Gradient]
// ============================================================================

Gradient Pattern::getGradient() const
{
  PatternData* d = _d;

  if (d->type & PATTERN_GRADIENT_MASK)
  {
    return Gradient(d->type, d->spread, d->points[0], d->points[1], d->obj.stops.instance());
  }
  else
  {
    return Gradient();
  }
}

err_t Pattern::setGradient(const Gradient& gradient)
{
  if (isGradient())
  {
    FOG_RETURN_ON_ERROR(detach());

    _d->type = gradient._type;
    _d->spread = gradient._spread;
    _d->points[0] = gradient._start;
    _d->points[1] = gradient._end;
    _d->radius = gradient._radius;
    _d->obj.stops.instance() = gradient.getStops();

    return ERR_OK;
  }
  else
  {
    PatternData* newd = new(std::nothrow) PatternData();
    if (!newd) return ERR_RT_OUT_OF_MEMORY;

    newd->type = gradient._type;
    newd->spread = gradient._spread;
    newd->points[0] = gradient._start;
    newd->points[1] = gradient._end;
    newd->radius = gradient._radius;
    newd->obj.stops.init(gradient._stops);

    atomicPtrXchg(&_d, newd)->deref();
    return ERR_OK;
  }
}

err_t Pattern::setRadius(float r)
{
  return setRadius((double)r);
}

err_t Pattern::setRadius(double r)
{
  if (!isGradient()) return ERR_RT_INVALID_CONTEXT;
  if (_d->radius == r) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->radius = r;
  return ERR_OK;
}

List<ArgbStop> Pattern::getStops() const
{
  List<ArgbStop> stops;
  if (isGradient()) stops = _d->obj.stops.instance();
  return stops;
}

err_t Pattern::setStops(const List<ArgbStop>& stops)
{
  if (!isGradient()) return ERR_RT_INVALID_CONTEXT;
  if (_d->obj.stops->_d == stops._d) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->obj.stops.instance() = stops;
  return _d->obj.stops->sort();
}

err_t Pattern::resetStops()
{
  if (!isGradient()) return ERR_RT_INVALID_CONTEXT;

  _d->obj.stops->free();
  return ERR_OK;
}

err_t Pattern::addStop(const ArgbStop& stop)
{
  if (!isGradient()) return ERR_RT_INVALID_CONTEXT;

  FOG_RETURN_ON_ERROR(detach());

  ArgbStop s(stop);
  s.normalize();

  List<ArgbStop>::MutableIterator it(_d->obj.stops.instance());
  for (it.toStart(); it.isValid(); it.toNext())
  {
    if (it.value().offset == s.offset)
    {
      it.value().argb = s.argb;
      return ERR_OK;
    }
    else if (it.value().offset > s.offset)
    {
      break;
    }
  }

  return _d->obj.stops->insert(it.index(), s);
}

// [Operator Overload]

Pattern& Pattern::operator=(const Pattern& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return *this;
}

Pattern& Pattern::operator=(const Argb& rgba)
{
  setColor(rgba);
  return *this;
}

// ============================================================================
// [Fog::PatternData]
// ============================================================================

PatternData::PatternData()
{
  refCount.init(1);
  type = PATTERN_NULL;
  spread = SPREAD_PAD;
  points[0].set(0.0, 0.0);
  points[1].set(0.0, 0.0);
  points[2].set(0.0, 0.0);
  radius = 0.0;
  obj.argb->set(0x00000000);
}

PatternData::PatternData(const PatternData& other) :
  type(other.type),
  spread(other.spread),
  radius(other.radius),
  matrix(other.matrix)
{
  refCount.init(1);
  points[0] = other.points[0];
  points[1] = other.points[1];
  points[2] = other.points[2];

  switch (type)
  {
    case PATTERN_SOLID:
      obj.argb.init(other.obj.argb.instance());
      break;
    case PATTERN_TEXTURE:
      obj.texture.init(other.obj.texture.instance());
      break;
    case PATTERN_LINEAR_GRADIENT:
    case PATTERN_RADIAL_GRADIENT:
    case PATTERN_CONICAL_GRADIENT:
      obj.stops.init(other.obj.stops.instance());
      break;
    default:
      break;
  }
}

PatternData::~PatternData()
{
  deleteResources();
}

PatternData* PatternData::copy()
{
  return new(std::nothrow) PatternData(*this);
}

void PatternData::deleteResources()
{
  if (type == PATTERN_TEXTURE)
  {
    obj.texture.destroy();
  }
  else if (type & PATTERN_GRADIENT_MASK)
  {
    obj.stops.destroy();
  }
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_pattern_init(void)
{
  using namespace Fog;

  Pattern::sharedNull.init();
  return ERR_OK;
}

FOG_INIT_DECLARE void fog_pattern_shutdown(void)
{
  using namespace Fog;

  Pattern::sharedNull.destroy();
}
