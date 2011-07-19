// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/G2d/Source/Pattern.h>

namespace Fog {

// ============================================================================
// [Fog::PatternF - Helpers]
// ============================================================================

static FOG_INLINE PatternDataF* PatternF_dalloc()
{
  PatternDataF* newd = reinterpret_cast<PatternDataF*>(Memory::alloc(sizeof(PatternDataF)));
  if (FOG_UNLIKELY(newd == NULL)) return NULL;

  newd->refCount.init(1);
  newd->transform.reset();

  return newd;
}

static FOG_INLINE PatternDataF* PatternF_dalloc(const TransformF& tr)
{
  PatternDataF* newd = reinterpret_cast<PatternDataF*>(Memory::alloc(sizeof(PatternDataF)));
  if (FOG_UNLIKELY(newd == NULL)) return NULL;

  newd->refCount.init(1);
  newd->transform = tr;

  return newd;
}

static FOG_INLINE void PatternF_dinitColor(PatternDataF* d, const Color& color)
{
  d->type = PATTERN_TYPE_COLOR;
  d->color.init(color);
}

static FOG_INLINE void PatternF_dinitColor(PatternDataF* d, const Argb32& argb32)
{
  d->type = PATTERN_TYPE_COLOR;
  d->color.initCustom1(argb32);
}

static FOG_INLINE void PatternF_dinitTexture(PatternDataF* d, const Texture& tex)
{
  d->type = PATTERN_TYPE_TEXTURE;
  d->texture.init(tex);
}

static FOG_INLINE void PatternF_dinitGradient(PatternDataF* d, const GradientF& gr)
{
  d->type = PATTERN_TYPE_GRADIENT;
  d->gradient.init(gr);
}

static FOG_INLINE void PatternF_dinitGradient(PatternDataF* d, const GradientD& gr)
{
  d->type = PATTERN_TYPE_GRADIENT;
  d->gradient.initCustom1(gr);
}

#define PATTERNF_CHANGE() \
  if (!isDetached()) \
  { \
    PatternDataF* newd = PatternF_dalloc(_d->transform); \
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY; \
    atomicPtrXchg(&_d, newd)->deref(); \
  } \
  else \
  { \
    _d->destroy(); \
  }

// ============================================================================
// [Fog::PatternF - Construction / Destruction]
// ============================================================================

PatternF::PatternF()
{
  _d = _dnull->ref();
}

PatternF::PatternF(const PatternF& other)
{
  _d = other._d->ref();
}

PatternF::PatternF(const PatternD& other)
{
  _d = PatternF_dalloc();
  if (FOG_UNLIKELY(_d == NULL)) { _d = _dnull->ref(); return; }

  setPattern(other);
}

PatternF::PatternF(const Argb32& argb32)
{
  _d = PatternF_dalloc();
  if (FOG_UNLIKELY(_d == NULL)) { _d = _dnull->ref(); return; }

  PatternF_dinitColor(_d, argb32);
}

PatternF::PatternF(const Color& color)
{
  _d = PatternF_dalloc();
  if (FOG_UNLIKELY(_d == NULL)) { _d = _dnull->ref(); return; }

  PatternF_dinitColor(_d, color);
}

PatternF::PatternF(const Texture& texture)
{
  _d = PatternF_dalloc();
  if (FOG_UNLIKELY(_d == NULL)) { _d = _dnull->ref(); return; }

  PatternF_dinitTexture(_d, texture);
}

PatternF::PatternF(const GradientF& gradient)
{
  _d = PatternF_dalloc();
  if (FOG_UNLIKELY(_d == NULL)) { _d = _dnull->ref(); return; }

  PatternF_dinitGradient(_d, gradient);
}

PatternF::PatternF(const GradientD& gradient)
{
  _d = PatternF_dalloc();
  if (FOG_UNLIKELY(_d == NULL)) { _d = _dnull->ref(); return; }

  PatternF_dinitGradient(_d, gradient);
}

PatternF::~PatternF()
{
  _d->deref();
}

// ============================================================================
// [Fog::PatternF - Data]
// ============================================================================

err_t PatternF::_detach()
{
  if (isDetached()) return ERR_OK;

  PatternDataF* newd = PatternF_dalloc(_d->transform);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  switch (_d->type)
  {
    case PATTERN_TYPE_NONE:
      newd->type = PATTERN_TYPE_NONE;
      break;

    case PATTERN_TYPE_COLOR:
      newd->type = PATTERN_TYPE_COLOR;
      newd->color.init(_d->color.instance());
      break;

    case PATTERN_TYPE_GRADIENT:
      newd->gradient.init(_d->gradient.instance());
      break;

    case PATTERN_TYPE_TEXTURE:
      newd->texture.init(_d->texture.instance());
      break;

    default:
      Memory::free(newd);
      return ERR_RT_INVALID_STATE;
  }

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

void PatternF::clear()
{
  if (isDetached())
  {
    _d->destroy();
    _d->type = PATTERN_TYPE_NONE;
  }
  else
  {
    atomicPtrXchg(&_d, _dnull->ref())->deref();
  }
}

void PatternF::reset()
{
  atomicPtrXchg(&_d, _dnull->ref())->deref();
}

// ============================================================================
// [Fog::PatternF - Transform]
// ============================================================================

err_t PatternF::getTransform(TransformF& tr) const
{
  return tr.setTransform(_d->transform);
}

err_t PatternF::getTransform(TransformD& tr) const
{
  return tr.setTransform(_d->transform);
}

err_t PatternF::setTransform(const TransformF& tr)
{
  FOG_RETURN_ON_ERROR(detach());

  return _d->transform.setTransform(tr);
}

err_t PatternF::setTransform(const TransformD& tr)
{
  FOG_RETURN_ON_ERROR(detach());

  return _d->transform.setTransform(tr);
}

err_t PatternF::resetTransform()
{
  FOG_RETURN_ON_ERROR(detach());

  _d->transform.reset();
  return ERR_OK;
}

err_t PatternF::_transform(uint32_t transformOp, const void* params)
{
  FOG_RETURN_ON_ERROR(detach());

  return _api.transformf.transform(_d->transform, transformOp, params);
}

// ============================================================================
// [Fog::PatternF - Color]
// ============================================================================

err_t PatternF::getColor(Color& color) const
{
  if (_d->type != PATTERN_TYPE_COLOR)
  {
    color.reset();
    return ERR_RT_INVALID_STATE;
  }
  else
  {
    color = _d->color.instance();
    return ERR_OK;
  }
}

err_t PatternF::getColor(Argb32& argb32) const
{
  if (_d->type != PATTERN_TYPE_COLOR)
  {
    argb32.reset();
    return ERR_RT_INVALID_STATE;
  }
  else
  {
    argb32 = _d->color.instance()._argb32;
    return ERR_OK;
  }
}

err_t PatternF::setColor(const Color& color)
{
  PATTERNF_CHANGE()

  _d->type = PATTERN_TYPE_COLOR;
  _d->color.init(color);
  return ERR_OK;
}

err_t PatternF::setColor(const Argb32& argb32)
{
  PATTERNF_CHANGE()

  _d->type = PATTERN_TYPE_COLOR;
  _d->color.initCustom1(argb32);
  return ERR_OK;
}

// ============================================================================
// [Fog::PatternF - Texture]
// ============================================================================

err_t PatternF::getTexture(Texture& texture) const
{
  if (_d->type != PATTERN_TYPE_COLOR)
  {
    texture.reset();
    return ERR_RT_INVALID_STATE;
  }
  else
  {
    texture = _d->texture.instance();
    return ERR_OK;
  }
}

err_t PatternF::setTexture(const Texture& texture)
{
  PATTERNF_CHANGE()

  PatternF_dinitTexture(_d, texture);
  return ERR_OK;
}

// ============================================================================
// [Fog::PatternF - Gradient]
// ============================================================================

err_t PatternF::_getGradientF(uint32_t gradientType, GradientF& gr) const
{
  if ((_d->type != PATTERN_TYPE_GRADIENT) ||
      (_d->gradient->getGradientType() != gradientType && gradientType != 0xFFFFFFFF))
  {
    gr.reset();
    return ERR_RT_INVALID_STATE;
  }
  else
  {
    return gr.setGradient(_d->gradient.instance());
  }
}

err_t PatternF::_getGradientD(uint32_t gradientType, GradientD& gr) const
{
  if ((_d->type != PATTERN_TYPE_GRADIENT) ||
      (_d->gradient->getGradientType() != gradientType && gradientType != 0xFFFFFFFF))
  {
    gr.reset();
    return ERR_RT_INVALID_STATE;
  }
  else
  {
    return gr.setGradient(_d->gradient.instance());
  }
}

err_t PatternF::setGradient(const GradientF& gr)
{
  PATTERNF_CHANGE()

  PatternF_dinitGradient(_d, gr);
  return ERR_OK;
}

err_t PatternF::setGradient(const GradientD& gr)
{
  PATTERNF_CHANGE()

  PatternF_dinitGradient(_d, gr);
  return ERR_OK;
}

// ============================================================================
// [Fog::PatternF - Pattern]
// ============================================================================

err_t PatternF::setPattern(const PatternF& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return ERR_OK;
}

err_t PatternF::setPattern(const PatternD& other)
{
  PATTERNF_CHANGE()

  switch (other.getType())
  {
    case PATTERN_TYPE_NONE:
      _d->type = PATTERN_TYPE_NONE;
      break;

    case PATTERN_TYPE_COLOR:
      _d->type = PATTERN_TYPE_COLOR;
      _d->color.init(other._d->color.instance());
      break;

    case PATTERN_TYPE_GRADIENT:
      _d->type = PATTERN_TYPE_GRADIENT;
      _d->gradient.initCustom1(other._d->gradient.instance());
      break;

    case PATTERN_TYPE_TEXTURE:
      _d->type = PATTERN_TYPE_TEXTURE;
      _d->texture.init(other._d->texture.instance());
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::PatternF - Statics]
// ============================================================================

Static<PatternDataF> PatternF::_dnull;

// ============================================================================
// [Fog::PatternD - Helpers]
// ============================================================================

static FOG_INLINE PatternDataD* PatternD_dalloc()
{
  PatternDataD* newd = reinterpret_cast<PatternDataD*>(Memory::alloc(sizeof(PatternDataD)));
  if (FOG_UNLIKELY(newd == NULL)) return NULL;

  newd->refCount.init(1);
  newd->transform.reset();

  return newd;
}

static FOG_INLINE PatternDataD* PatternD_dalloc(const TransformD& tr)
{
  PatternDataD* newd = reinterpret_cast<PatternDataD*>(Memory::alloc(sizeof(PatternDataD)));
  if (FOG_UNLIKELY(newd == NULL)) return NULL;

  newd->refCount.init(1);
  newd->transform = tr;

  return newd;
}

static FOG_INLINE void PatternD_dinitColor(PatternDataD* d, const Color& color)
{
  d->type = PATTERN_TYPE_COLOR;
  d->color.init(color);
}

static FOG_INLINE void PatternD_dinitColor(PatternDataD* d, const Argb32& argb32)
{
  d->type = PATTERN_TYPE_COLOR;
  d->color.initCustom1(argb32);
}

static FOG_INLINE void PatternD_dinitTexture(PatternDataD* d, const Texture& tex)
{
  d->type = PATTERN_TYPE_TEXTURE;
  d->texture.init(tex);
}

static FOG_INLINE void PatternD_dinitGradient(PatternDataD* d, const GradientF& gr)
{
  d->type = PATTERN_TYPE_GRADIENT;
  d->gradient.initCustom1(gr);
}

static FOG_INLINE void PatternD_dinitGradient(PatternDataD* d, const GradientD& gr)
{
  d->type = PATTERN_TYPE_GRADIENT;
  d->gradient.init(gr);
}

#define PATTERND_CHANGE() \
  if (!isDetached()) \
  { \
    PatternDataD* newd = PatternD_dalloc(_d->transform); \
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY; \
    atomicPtrXchg(&_d, newd)->deref(); \
  } \
  else \
  { \
    _d->destroy(); \
  }

// ============================================================================
// [Fog::PatternD - Construction / Destruction]
// ============================================================================

PatternD::PatternD()
{
  _d = _dnull->ref();
}

PatternD::PatternD(const PatternF& other)
{
  _d = PatternD_dalloc();
  if (FOG_UNLIKELY(_d == NULL)) { _d = _dnull->ref(); return; }

  setPattern(other);
}

PatternD::PatternD(const PatternD& other)
{
  _d = other._d->ref();
}

PatternD::PatternD(const Argb32& argb32)
{
  _d = PatternD_dalloc();
  if (FOG_UNLIKELY(_d == NULL)) { _d = _dnull->ref(); return; }

  PatternD_dinitColor(_d, argb32);
}

PatternD::PatternD(const Color& color)
{
  _d = PatternD_dalloc();
  if (FOG_UNLIKELY(_d == NULL)) { _d = _dnull->ref(); return; }

  PatternD_dinitColor(_d, color);
}

PatternD::PatternD(const Texture& texture)
{
  _d = PatternD_dalloc();
  if (FOG_UNLIKELY(_d == NULL)) { _d = _dnull->ref(); return; }

  PatternD_dinitTexture(_d, texture);
}

PatternD::PatternD(const GradientF& gradient)
{
  _d = PatternD_dalloc();
  if (FOG_UNLIKELY(_d == NULL)) { _d = _dnull->ref(); return; }

  PatternD_dinitGradient(_d, gradient);
}

PatternD::PatternD(const GradientD& gradient)
{
  _d = PatternD_dalloc();
  if (FOG_UNLIKELY(_d == NULL)) { _d = _dnull->ref(); return; }

  PatternD_dinitGradient(_d, gradient);
}

PatternD::~PatternD()
{
  _d->deref();
}

// ============================================================================
// [Fog::PatternD - Data]
// ============================================================================

err_t PatternD::_detach()
{
  if (isDetached()) return ERR_OK;

  PatternDataD* newd = PatternD_dalloc(_d->transform);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  switch (_d->type)
  {
    case PATTERN_TYPE_NONE:
      newd->type = PATTERN_TYPE_NONE;
      break;

    case PATTERN_TYPE_COLOR:
      newd->type = PATTERN_TYPE_COLOR;
      newd->color.init(_d->color.instance());
      break;

    case PATTERN_TYPE_GRADIENT:
      newd->gradient.init(_d->gradient.instance());
      break;

    case PATTERN_TYPE_TEXTURE:
      newd->texture.init(_d->texture.instance());
      break;

    default:
      Memory::free(newd);
      return ERR_RT_INVALID_STATE;
  }

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

void PatternD::clear()
{
  if (isDetached())
  {
    _d->destroy();
    _d->type = PATTERN_TYPE_NONE;
  }
  else
  {
    atomicPtrXchg(&_d, _dnull->ref())->deref();
  }
}

void PatternD::reset()
{
  atomicPtrXchg(&_d, _dnull->ref())->deref();
}

// ============================================================================
// [Fog::PatternD - Transform]
// ============================================================================

err_t PatternD::getTransform(TransformF& tr) const
{
  return tr.setTransform(_d->transform);
}

err_t PatternD::getTransform(TransformD& tr) const
{
  return tr.setTransform(_d->transform);
}

err_t PatternD::setTransform(const TransformF& tr)
{
  FOG_RETURN_ON_ERROR(detach());

  return _d->transform.setTransform(tr);
}

err_t PatternD::setTransform(const TransformD& tr)
{
  FOG_RETURN_ON_ERROR(detach());

  return _d->transform.setTransform(tr);
}

err_t PatternD::resetTransform()
{
  FOG_RETURN_ON_ERROR(detach());

  _d->transform.reset();
  return ERR_OK;
}

err_t PatternD::_transform(uint32_t transformOp, const void* params)
{
  FOG_RETURN_ON_ERROR(detach());

  return _api.transformd.transform(_d->transform, transformOp, params);
}

// ============================================================================
// [Fog::PatternD - Color]
// ============================================================================

err_t PatternD::getColor(Color& color) const
{
  if (_d->type != PATTERN_TYPE_COLOR)
  {
    color.reset();
    return ERR_RT_INVALID_STATE;
  }
  else
  {
    color = _d->color.instance();
    return ERR_OK;
  }
}

err_t PatternD::getColor(Argb32& argb32) const
{
  if (_d->type != PATTERN_TYPE_COLOR)
  {
    argb32.reset();
    return ERR_RT_INVALID_STATE;
  }
  else
  {
    argb32 = _d->color.instance()._argb32;
    return ERR_OK;
  }
}

err_t PatternD::setColor(const Color& color)
{
  PATTERND_CHANGE()

  _d->type = PATTERN_TYPE_COLOR;
  _d->color.init(color);
  return ERR_OK;
}

err_t PatternD::setColor(const Argb32& argb32)
{
  PATTERND_CHANGE()

  _d->type = PATTERN_TYPE_COLOR;
  _d->color.initCustom1(argb32);
  return ERR_OK;
}

// ============================================================================
// [Fog::PatternF - Texture]
// ============================================================================

err_t PatternD::getTexture(Texture& texture) const
{
  if (_d->type != PATTERN_TYPE_COLOR)
  {
    texture.reset();
    return ERR_RT_INVALID_STATE;
  }
  else
  {
    texture = _d->texture.instance();
    return ERR_OK;
  }
}

err_t PatternD::setTexture(const Texture& texture)
{
  PATTERND_CHANGE()

  _d->texture.init(texture);
  return ERR_OK;
}

// ============================================================================
// [Fog::PatternD - Gradient]
// ============================================================================

err_t PatternD::_getGradientF(uint32_t gradientType, GradientF& gr) const
{
  if ((_d->type != PATTERN_TYPE_GRADIENT) ||
      (_d->gradient->getGradientType() != gradientType && gradientType != 0xFFFFFFFF))
  {
    gr.reset();
    return ERR_RT_INVALID_STATE;
  }
  else
  {
    return gr.setGradient(_d->gradient.instance());
  }
}

err_t PatternD::_getGradientD(uint32_t gradientType, GradientD& gr) const
{
  if ((_d->type != PATTERN_TYPE_GRADIENT) ||
      (_d->gradient->getGradientType() != gradientType && gradientType != 0xFFFFFFFF))
  {
    gr.reset();
    return ERR_RT_INVALID_STATE;
  }
  else
  {
    return gr.setGradient(_d->gradient.instance());
  }
}

err_t PatternD::setGradient(const GradientF& gr)
{
  PATTERND_CHANGE()

  _d->gradient.initCustom1(gr);
  return ERR_OK;
}

err_t PatternD::setGradient(const GradientD& gr)
{
  PATTERND_CHANGE()

  _d->gradient.init(gr);
  return ERR_OK;
}

// ============================================================================
// [Fog::PatternF - Pattern]
// ============================================================================

err_t PatternD::setPattern(const PatternF& other)
{
  PATTERND_CHANGE()

  switch (other.getType())
  {
    case PATTERN_TYPE_NONE:
      _d->type = PATTERN_TYPE_NONE;
      break;

    case PATTERN_TYPE_COLOR:
      _d->type = PATTERN_TYPE_COLOR;
      _d->color.init(other._d->color.instance());
      break;

    case PATTERN_TYPE_GRADIENT:
      _d->type = PATTERN_TYPE_GRADIENT;
      _d->gradient.initCustom1(other._d->gradient.instance());
      _d->transform = other._d->transform;
      break;

    case PATTERN_TYPE_TEXTURE:
      _d->type = PATTERN_TYPE_TEXTURE;
      _d->texture.init(other._d->texture.instance());
      _d->transform = other._d->transform;
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_OK;
}

err_t PatternD::setPattern(const PatternD& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return ERR_OK;
/*
  if (_d == other._d) return ERR_OK;
  PATTERND_CHANGE()

  switch (other.getType())
  {
    case PATTERN_TYPE_NONE:
      _d->type = PATTERN_TYPE_NONE;
      break;

    case PATTERN_TYPE_COLOR:
      _d->type = PATTERN_TYPE_COLOR;
      _d->color.init(other._d->color.instance());
      break;

    case PATTERN_TYPE_GRADIENT:
      _d->type = PATTERN_TYPE_GRADIENT;
      _d->gradient.init(other._d->gradient.instance());
      _d->transform = other._d->transform;
      break;

    case PATTERN_TYPE_TEXTURE:
      _d->type = PATTERN_TYPE_TEXTURE;
      _d->texture.init(other._d->texture.instance());
      _d->transform = other._d->transform;
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return ERR_OK;
*/
}

// ============================================================================
// [Fog::PatternD - Statics]
// ============================================================================

Static<PatternDataD> PatternD::_dnull;

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Pattern_init(void)
{
  PatternF::_dnull->refCount.init(1);
  PatternF::_dnull->type = PATTERN_TYPE_NONE;
  PatternF::_dnull->transform.reset();

  PatternD::_dnull->refCount.init(1);
  PatternD::_dnull->type = PATTERN_TYPE_NONE;
  PatternD::_dnull->transform.reset();
}

} // Fog namespace
