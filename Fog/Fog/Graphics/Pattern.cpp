// [Fog-Graphics]
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

static FOG_INLINE void Pattern_initData(PatternData* d)
{
  uint32_t type = d->type;

  if (type == PATTERN_TEXTURE)
  {
    d->obj.texture.init();
    d->data.texture->area.reset();
  }
  else if (type & PATTERN_IS_GRADIENT)
  {
    d->obj.stops.init();
    d->data.gradient->points[0].reset();
    d->data.gradient->points[1].reset();
    d->data.gradient->radius = 0.0;
  }
}

static FOG_INLINE void Pattern_freeData(PatternData* d)
{
  uint32_t type = d->type;

  if (type == PATTERN_TEXTURE)
  {
    d->obj.texture.destroy();
  }
  else if (type & PATTERN_IS_GRADIENT)
  {
    d->obj.stops.destroy();
  }
}

Static<PatternData> Pattern::_dnull;

Pattern::Pattern() : 
  _d(_dnull->ref())
{
}

Pattern::Pattern(const Pattern& other) :
  _d(other._d->ref())
{
}

Pattern::Pattern(const ArgbI& argb)
{
  _d = fog_new PatternData();
  if (!_d) { _d = _dnull->ref(); return; }

  _d->type = PATTERN_SOLID;
  _d->obj.argb->setArgb(argb);
}

Pattern::~Pattern()
{
  _d->deref();
}

err_t Pattern::_detach()
{
  if (_d->refCount.get() == 1) return ERR_OK;

  PatternData* newd = fog_new PatternData(*_d);
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

void Pattern::free()
{
  atomicPtrXchg(&_d, _dnull->ref())->deref();
}

err_t Pattern::setType(uint32_t type)
{
  if (_d->type == type) return ERR_OK;

  if (!((uint)type <= 0x2U || (type >= PATTERN_LINEAR_GRADIENT && type <= PATTERN_CONICAL_GRADIENT)))
    return ERR_RT_INVALID_ARGUMENT;

  // Optimize [Gradient -> Gradient] type switching.
  if ((_d->type & PATTERN_IS_GRADIENT) != 0 && (type & PATTERN_IS_GRADIENT) != 0)
  {
    FOG_RETURN_ON_ERROR(detach());
    _d->type = type;
    return ERR_OK;
  }

  // Optimize switching to different pattern type by using previously allocated
  // memory if it's not shared between another Pattern.
  if (_d->refCount.get() == 1)
  {
    Pattern_freeData(_d);
    _d->deleteResources();
    _d->type = type;

    Pattern_initData(_d);
  }
  else
  {
    PatternData* newd = fog_new PatternData();
    if (!newd) return ERR_RT_OUT_OF_MEMORY;

    newd->type = type;
    newd->spread = _d->spread;
    newd->transform = _d->transform;

    Pattern_initData(newd);
    atomicPtrXchg(&_d, newd)->deref();
  }

  return ERR_OK;
}

void Pattern::reset()
{
  if (_d->refCount.get() > 1)
  {
    atomicPtrXchg(&_d, _dnull->ref())->deref();
  }
  else
  {
    _d->deleteResources();
    _d->type = PATTERN_NONE;
    _d->spread = PATTERN_SPREAD_PAD;
    _d->transform.reset();
  }
}

err_t Pattern::setSpread(uint32_t spread)
{
  if (_d->spread == spread) return ERR_OK;
  if (FOG_UNLIKELY(spread >= PATTERN_SPREAD_COUNT)) return ERR_RT_INVALID_ARGUMENT;

  FOG_RETURN_ON_ERROR(detach());

  _d->spread = spread;
  return ERR_OK;
}

err_t Pattern::setTransform(const TransformF& transform)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->transform = transform;
  return ERR_OK;
}

err_t Pattern::setTransform(const TransformD& transform)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->transform = transform;
  return ERR_OK;
}

err_t Pattern::resetTransform()
{
  FOG_RETURN_ON_ERROR(detach());

  _d->transform.reset();
  return ERR_OK;
}

err_t Pattern::translate(float x, float y, uint32_t order)
{
  return translate((double)x, (double)y, order);
}

err_t Pattern::translate(double x, double y, uint32_t order)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->transform.translate(x, y, order);
  return ERR_OK;
}

err_t Pattern::rotate(float a, uint32_t order)
{
  return rotate((double)a, order);
}

err_t Pattern::rotate(double a, uint32_t order)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->transform.rotate(a, order);
  return ERR_OK;
}

err_t Pattern::scale(float x, float y, uint32_t order)
{
  return scale((double)x, (double)y, order);

}
err_t Pattern::scale(double x, double y, uint32_t order)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->transform.scale(x, y, order);
  return ERR_OK;
}

err_t Pattern::skew(float x, float y, uint32_t order)
{
  return skew((double)x, (double)y, order);
}

err_t Pattern::skew(double x, double y, uint32_t order)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->transform.skew(x, y, order);
  return ERR_OK;
}

err_t Pattern::transform(const TransformF& m, uint32_t order)
{
  return transform(m.toTransformD(), order);
}

err_t Pattern::transform(const TransformD& m, uint32_t order)
{
  FOG_RETURN_ON_ERROR(detach());

  _d->transform.transform(m, order);
  return ERR_OK;
}

// ============================================================================
// [Fog::Pattern - Solid]
// ============================================================================

ArgbI Pattern::getColor() const
{
  if (isSolid())
    return _d->obj.argb.instance();
  else
    return ArgbI(0x00000000);
}

err_t Pattern::setColor(const ArgbI& argb)
{
  if (isSolid())
  {
    if (_d->obj.argb.instance() == argb) return ERR_OK;
    FOG_RETURN_ON_ERROR(detach());

    _d->obj.argb->setArgb(argb);
    return ERR_OK;
  }
  else
  {
    PatternData* newd = fog_new PatternData();
    if (!newd) return ERR_RT_OUT_OF_MEMORY;

    newd->type = PATTERN_SOLID;
    newd->spread = _d->spread;
    newd->transform = _d->transform;

    newd->obj.argb->setArgb(argb);

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

RectI Pattern::getTextureArea() const
{
  if (isTexture())
    return _d->data.texture->area;
  else
    return RectI(0, 0, 0, 0);
}

static err_t Pattern_setTexturePrivate(Pattern* self, const Image& texture, const RectI& textureArea)
{
  if (self->_d->refCount.get() == 1)
  {
    self->_d->deleteResources();
    self->_d->type = PATTERN_TEXTURE;

    self->_d->obj.texture.init(texture);
    self->_d->data.texture->area = textureArea;

    return ERR_OK;
  }
  else
  {
    PatternData* newd = fog_new PatternData();
    if (!newd) return ERR_RT_OUT_OF_MEMORY;

    newd->type = PATTERN_TEXTURE;
    newd->spread = self->_d->spread;
    newd->transform = self->_d->transform;

    newd->obj.texture.init(texture);
    newd->data.texture->area = textureArea;

    atomicPtrXchg(&self->_d, newd)->deref();
    return ERR_OK;
  }
}

err_t Pattern::setTexture(const Image& texture)
{
  return Pattern_setTexturePrivate(this, 
    texture, RectI(0, 0, texture.getWidth(), texture.getHeight()));
}

err_t Pattern::setTexture(const Image& texture, const RectI& textureArea)
{
  int x1 = textureArea.x;
  int y1 = textureArea.y;
  int x2 = x1 + textureArea.w;
  int y2 = y1 + textureArea.h;

  if (x1 < 0 || y1 > 0 || x2 >= texture.getWidth() || y2 >= texture.getHeight()) 
    return ERR_RT_INVALID_ARGUMENT;

  if ((x1 >= x2 || y1 >= y2) && !texture.isEmpty())
    return ERR_RT_INVALID_ARGUMENT;

  return Pattern_setTexturePrivate(this, 
    texture, RectI(x1, y1, x2 - x1, y2 - y1));
}

// ============================================================================
// [Fog::Pattern - Gradient]
// ============================================================================

Gradient Pattern::getGradient() const
{
  PatternData* d = _d;

  if (d->type & PATTERN_IS_GRADIENT)
  {
    return Gradient(
      d->type,
      d->spread,
      d->data.gradient->points[0],
      d->data.gradient->points[1],
      d->data.gradient->radius,
      d->obj.stops.instance());
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

    _d->obj.stops.instance() = gradient.getStops();
    _d->data.gradient->points[0] = gradient._start;
    _d->data.gradient->points[1] = gradient._end;
    _d->data.gradient->radius = gradient._radius;

    return ERR_OK;
  }
  else
  {
    PatternData* newd = fog_new PatternData();
    if (!newd) return ERR_RT_OUT_OF_MEMORY;

    newd->type = gradient._type;
    newd->spread = gradient._spread;

    newd->obj.stops.init(gradient._stops);
    newd->data.gradient->points[0] = gradient._start;
    newd->data.gradient->points[1] = gradient._end;
    newd->data.gradient->radius = gradient._radius;

    atomicPtrXchg(&_d, newd)->deref();
    return ERR_OK;
  }
}

PointD Pattern::getStartPoint() const
{
  if (isGradient())
    return _d->data.gradient->points[0];
  else
    return PointD(0.0, 0.0);
}

PointD Pattern::getEndPoint() const
{
  if (isGradient())
    return _d->data.gradient->points[1];
  else
    return PointD(0.0, 0.0);
}

err_t Pattern::setStartPoint(const PointI& pt)
{
  return setStartPoint(pt.toPointD());
}

err_t Pattern::setStartPoint(const PointF& pt)
{
  return setStartPoint(pt.toPointD());

}
err_t Pattern::setStartPoint(const PointD& pt)
{
  if (!isGradient()) return ERR_RT_INVALID_OBJECT;
  if (_d->data.gradient->points[0] == pt) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->data.gradient->points[0] = pt;
  return ERR_OK;
}

err_t Pattern::setEndPoint(const PointI& pt)
{
  return setEndPoint(pt.toPointD());
}

err_t Pattern::setEndPoint(const PointF& pt)
{
  return setEndPoint(pt.toPointD());
}

err_t Pattern::setEndPoint(const PointD& pt)
{
  if (!isGradient()) return ERR_RT_INVALID_OBJECT;
  if (_d->data.gradient->points[1] == pt) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->data.gradient->points[1] = pt;
  return ERR_OK;
}

err_t Pattern::setPoints(const PointI& startPt, const PointI& endPt)
{
  return setPoints(startPt.toPointD(), endPt.toPointD());
}

err_t Pattern::setPoints(const PointF& startPt, const PointF& endPt)
{
  return setPoints(startPt.toPointD(), endPt.toPointD());
}

err_t Pattern::setPoints(const PointD& startPt, const PointD& endPt)
{
  if (!isGradient()) return ERR_RT_INVALID_OBJECT;

  if (_d->data.gradient->points[0] == startPt && 
      _d->data.gradient->points[1] == endPt) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->data.gradient->points[0] = startPt;
  _d->data.gradient->points[1] = endPt;
  return ERR_OK;
}

double Pattern::getRadius() const
{
  if (isGradient())
    return _d->data.gradient->radius;
  else
    return 0.0;
}

err_t Pattern::setRadius(float r)
{
  return setRadius((double)r);
}

err_t Pattern::setRadius(double r)
{
  if (!isGradient()) return ERR_RT_INVALID_OBJECT;
  if (_d->data.gradient->radius == r) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->data.gradient->radius = r;
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
  if (!isGradient()) return ERR_RT_INVALID_OBJECT;
  if (_d->obj.stops->_d == stops._d) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->obj.stops.instance() = stops;
  return _d->obj.stops->sort();
}

err_t Pattern::resetStops()
{
  if (!isGradient()) return ERR_RT_INVALID_OBJECT;

  _d->obj.stops->free();
  return ERR_OK;
}

err_t Pattern::addStop(const ArgbStop& stop)
{
  if (!isGradient()) return ERR_RT_INVALID_OBJECT;

  FOG_RETURN_ON_ERROR(detach());

  ArgbStop s(stop);
  s.normalize();

  List<ArgbStop>::MutableIterator it(_d->obj.stops.instance());
  for (it.toStart(); it.isValid(); it.toNext())
  {
    if (it.value().getOffset() == s.getOffset())
    {
      it.value().setArgb(s.getArgbI());
      return ERR_OK;
    }
    else if (it.value().getOffset() > s.getOffset())
    {
      break;
    }
  }

  return _d->obj.stops->insert(it.index(), s);
}

// ============================================================================
// [Fog::Pattern - Operator Overload]
// ============================================================================

Pattern& Pattern::operator=(const Pattern& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return *this;
}

Pattern& Pattern::operator=(const ArgbI& rgba)
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
  type = PATTERN_NONE;
  spread = PATTERN_SPREAD_PAD;
}

PatternData::PatternData(const PatternData& other) :
  type(other.type),
  spread(other.spread),
  transform(other.transform)
{
  refCount.init(1);

  switch (type)
  {
    case PATTERN_SOLID:
      obj.argb.init(other.obj.argb.instance());
      break;
    case PATTERN_TEXTURE:
      obj.texture.init(other.obj.texture.instance());
      data.texture->area = other.data.texture->area;
      break;
    case PATTERN_LINEAR_GRADIENT:
    case PATTERN_RADIAL_GRADIENT:
    case PATTERN_CONICAL_GRADIENT:
      obj.stops.init(other.obj.stops.instance());
      data.gradient->points[0] = other.data.gradient->points[0];
      data.gradient->points[1] = other.data.gradient->points[1];
      data.gradient->radius = other.data.gradient->radius;
      break;
    default:
      break;
  }
}

PatternData::~PatternData()
{
  deleteResources();
}

void PatternData::deleteResources()
{
  Pattern_freeData(this);
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_pattern_init(void)
{
  using namespace Fog;

  Pattern::_dnull.init();
  return ERR_OK;
}

FOG_INIT_DECLARE void fog_pattern_shutdown(void)
{
  using namespace Fog;

  Pattern::_dnull.destroy();
}
