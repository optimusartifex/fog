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
#include <Fog/Core/Tools/Var.h>
#include <Fog/G2d/Source/Pattern.h>

namespace Fog {

// ============================================================================
// [Fog::Pattern - Helpers]
// ============================================================================

// ${GRADIENT_TYPE:BEGIN}
static const uint32_t Pattern_vTypeFromGradientTableF[GRADIENT_TYPE_COUNT] = 
{
  /* 00: GRADIENT_TYPE_LINEAR      */ VAR_TYPE_LINEAR_GRADIENTF,
  /* 01: GRADIENT_TYPE_RADIAL      */ VAR_TYPE_RADIAL_GRADIENTF,
  /* 02: GRADIENT_TYPE_CONICAL     */ VAR_TYPE_CONICAL_GRADIENTF,
  /* 03: GRADIENT_TYPE_RECTANGULAR */ VAR_TYPE_RECTANGULAR_GRADIENTF
};
// ${GRADIENT_TYPE:END}

// ${GRADIENT_TYPE:BEGIN}
static const uint32_t Pattern_vTypeFromGradientTableD[GRADIENT_TYPE_COUNT] = 
{
  /* 00: GRADIENT_TYPE_LINEAR      */ VAR_TYPE_LINEAR_GRADIENTF,
  /* 01: GRADIENT_TYPE_RADIAL      */ VAR_TYPE_RADIAL_GRADIENTF,
  /* 02: GRADIENT_TYPE_CONICAL     */ VAR_TYPE_CONICAL_GRADIENTF,
  /* 03: GRADIENT_TYPE_RECTANGULAR */ VAR_TYPE_RECTANGULAR_GRADIENTF
};
// ${GRADIENT_TYPE:END}

static FOG_INLINE uint32_t Pattern_getVarTypeFromGradientF(uint32_t gradientType)
{
  FOG_ASSERT(gradientType < GRADIENT_TYPE_COUNT);
  return Pattern_vTypeFromGradientTableF[gradientType];
}

static FOG_INLINE uint32_t Pattern_getVarTypeFromGradientD(uint32_t gradientType)
{
  FOG_ASSERT(gradientType < GRADIENT_TYPE_COUNT);
  return Pattern_vTypeFromGradientTableD[gradientType];
}

static FOG_INLINE uint32_t Pattern_getGradientType(PatternData* d)
{
  // GradientF and GradientD share common values (gradientType and stops), but
  // there is no single structure which these two are based on. So instead of
  // doing this magic across the Pattern implementation it's only here. There is
  // no difference between PatternGradientDataF and PatternGradientDataD from
  // the gradientType perspective.
  return reinterpret_cast<PatternGradientDataF*>(d)->gradient().getGradientType();
}

// ============================================================================
// [Fog::Pattern - Construction / Destruction]
// ============================================================================

static void FOG_CDECL Pattern_ctor(Pattern* self)
{
  self->_d = _api.pattern.oNull->_d;
}

static void FOG_CDECL Pattern_ctorCopy(Pattern* self, const Pattern* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL Pattern_ctorArgb32(Pattern* self, const ArgbBase32* argb32)
{
  PatternColorData* d = reinterpret_cast<PatternColorData*>(
    _api.pattern.dCreate(sizeof(PatternColorData)));

  if (FOG_IS_NULL(d))
  {
    self->_d = _api.pattern.oNull->_d;
    return;
  }

  d->vType = VAR_TYPE_COLOR;
  d->patternType = PATTERN_TYPE_COLOR;
  d->color.initCustom1(*argb32);

  self->_d = d;
}

static void FOG_CDECL Pattern_ctorColor(Pattern* self, const Color* color)
{
  PatternColorData* d = reinterpret_cast<PatternColorData*>(
    _api.pattern.dCreate(sizeof(PatternColorData)));

  if (FOG_IS_NULL(d))
  {
    self->_d = _api.pattern.oNull->_d;
    return;
  }

  d->vType = VAR_TYPE_COLOR;
  d->patternType = PATTERN_TYPE_COLOR;
  d->color.initCustom1(*color);

  self->_d = d;
}

static void FOG_CDECL Pattern_ctorTextureF(Pattern* self, const Texture* texture, const TransformF* tr)
{
  PatternTextureDataF* d = reinterpret_cast<PatternTextureDataF*>(
    _api.pattern.dCreate(sizeof(PatternTextureDataF)));

  if (FOG_IS_NULL(d))
  {
    self->_d = _api.pattern.oNull->_d;
    return;
  }

  d->vType = VAR_TYPE_TEXTUREF;
  d->patternType = PATTERN_TYPE_TEXTURE;
  d->texture.initCustom1(*texture);
  d->transform.initCustom1(tr != NULL ? *tr : TransformF::identity());

  self->_d = d;
}

static void FOG_CDECL Pattern_ctorTextureD(Pattern* self, const Texture* texture, const TransformD* tr)
{
  if (texture->getImage().isEmpty())
  {
    self->_d = _api.pattern.oNull->_d;
    return;
  }

  PatternTextureDataD* d = reinterpret_cast<PatternTextureDataD*>(
    _api.pattern.dCreate(sizeof(PatternTextureDataD)));

  if (FOG_IS_NULL(d))
  {
    self->_d = _api.pattern.oNull->_d;
    return;
  }

  d->vType = VAR_TYPE_TEXTURED;
  d->patternType = PATTERN_TYPE_TEXTURE;
  d->texture.initCustom1(*texture);
  d->transform.initCustom1(tr != NULL ? *tr : TransformD::identity());

  self->_d = d;
}

static void FOG_CDECL Pattern_ctorGradientF(Pattern* self, const GradientF* gradient, const TransformF* tr)
{
  uint32_t gradientType = gradient->getGradientType();

  // Pattern disallows to use invalid (uninitialized) gradient.
  if (gradientType >= GRADIENT_TYPE_COUNT)
  {
    self->_d = _api.pattern.oNull->_d;
    return;
  }

  PatternGradientDataF* d = reinterpret_cast<PatternGradientDataF*>(
    _api.pattern.dCreate(sizeof(PatternGradientDataF)));

  if (FOG_IS_NULL(d))
  {
    self->_d = _api.pattern.oNull->_d;
    return;
  }

  d->vType = Pattern_getVarTypeFromGradientF(gradientType);
  d->patternType = PATTERN_TYPE_GRADIENT;
  d->gradient.initCustom1(*gradient);
  d->transform.initCustom1(tr != NULL ? *tr : TransformF::identity());

  self->_d = d;
}

static void FOG_CDECL Pattern_ctorGradientD(Pattern* self, const GradientD* gradient, const TransformD* tr)
{
  uint32_t gradientType = gradient->getGradientType();

  // Pattern disallows to use invalid (uninitialized) gradient.
  if (gradientType >= GRADIENT_TYPE_COUNT)
  {
    self->_d = _api.pattern.oNull->_d;
    return;
  }

  PatternGradientDataD* d = reinterpret_cast<PatternGradientDataD*>(
    _api.pattern.dCreate(sizeof(PatternGradientDataD)));

  if (FOG_IS_NULL(d))
  {
    self->_d = _api.pattern.oNull->_d;
    return;
  }

  d->vType = Pattern_getVarTypeFromGradientD(gradientType);
  d->patternType = PATTERN_TYPE_GRADIENT;
  d->gradient.initCustom1(*gradient);
  d->transform.initCustom1(tr != NULL ? *tr : TransformD::identity());

  self->_d = d;
}

static void FOG_CDECL Pattern_dtor(Pattern* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::Pattern - Sharing]
// ============================================================================

static err_t FOG_CDECL Pattern_detach(Pattern* self)
{
  PatternData* d = self->_d;
  if (d->reference.get() == 1)
    return ERR_OK;

  uint32_t vType = d->vType & VAR_TYPE_MASK;
  switch (vType)
  {
    case VAR_TYPE_NULL:
    {
      return ERR_OK;
    }

    case VAR_TYPE_COLOR:
    {
      PatternColorData* newd = reinterpret_cast<PatternColorData*>(
        _api.pattern.dCreate(sizeof(PatternColorData)));

      if (FOG_IS_NULL(newd))
        return ERR_RT_OUT_OF_MEMORY;

      newd->vType = vType;
      newd->patternType = d->patternType;
      newd->color.initCustom1(reinterpret_cast<PatternColorData*>(d)->color());

      atomicPtrXchg(&self->_d, reinterpret_cast<PatternData*>(newd))->release();
      return ERR_OK;
    }

    case VAR_TYPE_TEXTUREF:
    {
      PatternTextureDataF* newd = reinterpret_cast<PatternTextureDataF*>(
        _api.pattern.dCreate(sizeof(PatternTextureDataF)));

      if (FOG_IS_NULL(newd))
        return ERR_RT_OUT_OF_MEMORY;

      newd->vType = vType;
      newd->patternType = d->patternType;
      newd->texture.initCustom1(reinterpret_cast<PatternTextureDataF*>(d)->texture());
      newd->transform.initCustom1(reinterpret_cast<PatternTextureDataF*>(d)->transform());

      atomicPtrXchg(&self->_d, reinterpret_cast<PatternData*>(newd))->release();
      return ERR_OK;
    }

    case VAR_TYPE_TEXTURED:
    {
      PatternTextureDataD* newd = reinterpret_cast<PatternTextureDataD*>(
        _api.pattern.dCreate(sizeof(PatternTextureDataD)));

      if (FOG_IS_NULL(newd))
        return ERR_RT_OUT_OF_MEMORY;

      newd->vType = vType;
      newd->patternType = d->patternType;
      newd->texture.initCustom1(reinterpret_cast<PatternTextureDataD*>(d)->texture());
      newd->transform.initCustom1(reinterpret_cast<PatternTextureDataD*>(d)->transform());

      atomicPtrXchg(&self->_d, reinterpret_cast<PatternData*>(newd))->release();
      return ERR_OK;
    }

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
    {
      PatternGradientDataF* newd = reinterpret_cast<PatternGradientDataF*>(
        _api.pattern.dCreate(sizeof(PatternGradientDataF)));

      if (FOG_IS_NULL(newd))
        return ERR_RT_OUT_OF_MEMORY;

      newd->vType = vType;
      newd->patternType = d->patternType;
      newd->gradient.initCustom1(reinterpret_cast<PatternGradientDataF*>(d)->gradient());
      newd->transform.initCustom1(reinterpret_cast<PatternGradientDataF*>(d)->transform());

      atomicPtrXchg(&self->_d, reinterpret_cast<PatternData*>(newd))->release();
      return ERR_OK;
    }

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
    {
      PatternGradientDataD* newd = reinterpret_cast<PatternGradientDataD*>(
        _api.pattern.dCreate(sizeof(PatternGradientDataD)));

      if (FOG_IS_NULL(newd))
        return ERR_RT_OUT_OF_MEMORY;

      newd->vType = vType;
      newd->patternType = d->patternType;
      newd->gradient.initCustom1(reinterpret_cast<PatternGradientDataD*>(d)->gradient());
      newd->transform.initCustom1(reinterpret_cast<PatternGradientDataD*>(d)->transform());

      atomicPtrXchg(&self->_d, reinterpret_cast<PatternData*>(newd))->release();
      return ERR_OK;
    }

    default:
      // Shouldn't be reached.
      return ERR_RT_INVALID_STATE;
  }
}

// ============================================================================
// [Fog::Pattern - Accessors]
// ============================================================================

static err_t FOG_CDECL Pattern_getArgb32(const Pattern* self, ArgbBase32* argb32)
{
  PatternColorData* d = reinterpret_cast<PatternColorData*>(self->_d);

  if (d->patternType != PATTERN_TYPE_COLOR)
    return ERR_RT_INVALID_STATE;

  *argb32 = d->color->getArgb32();
  return ERR_OK;
}

static err_t FOG_CDECL Pattern_getColor(const Pattern* self, Color* color)
{
  PatternColorData* d = reinterpret_cast<PatternColorData*>(self->_d);

  if (d->patternType != PATTERN_TYPE_COLOR)
    return ERR_RT_INVALID_STATE;

  *color = d->color();
  return ERR_OK;
}

static err_t FOG_CDECL Pattern_getTexture(const Pattern* self, Texture* texture)
{
  PatternTextureDataF* d = reinterpret_cast<PatternTextureDataF*>(self->_d);

  if (d->patternType != PATTERN_TYPE_COLOR)
    return ERR_RT_INVALID_STATE;

  texture->setTexture(d->texture());
  return ERR_OK;
}

static err_t FOG_CDECL Pattern_getGradientF(const Pattern* self, uint32_t targetType, GradientF* gradient)
{
  PatternData* d = self->_d;

  if (d->patternType != PATTERN_TYPE_GRADIENT)
    return ERR_RT_INVALID_STATE;

  if (targetType != 0xFFFFFFFF && Pattern_getGradientType(d) != targetType)
    return ERR_RT_INVALID_STATE;

  switch (d->vType & VAR_TYPE_MASK)
  {
    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      return gradient->setGradient(reinterpret_cast<PatternGradientDataF*>(d)->gradient());

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      return gradient->setGradient(reinterpret_cast<PatternGradientDataD*>(d)->gradient());

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

static err_t FOG_CDECL Pattern_getGradientD(const Pattern* self, uint32_t targetType, GradientD* gradient)
{
  PatternData* d = self->_d;

  if (d->patternType != PATTERN_TYPE_GRADIENT)
    return ERR_RT_INVALID_STATE;

  if (targetType != 0xFFFFFFFF && Pattern_getGradientType(d) != targetType)
    return ERR_RT_INVALID_STATE;

  switch (d->vType & VAR_TYPE_MASK)
  {
    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      return gradient->setGradient(reinterpret_cast<PatternGradientDataF*>(d)->gradient());

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      return gradient->setGradient(reinterpret_cast<PatternGradientDataD*>(d)->gradient());

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

static err_t FOG_CDECL Pattern_getTransformF(const Pattern* self, TransformF* tr)
{
  PatternData* d = self->_d;

  switch (d->vType & VAR_TYPE_MASK)
  {
    // Null and color pattern has no transform, but it's special kind of pattern
    // so we return ERR_OK instead of reporting an invalid state.
    case VAR_TYPE_NULL:
    case VAR_TYPE_COLOR:
      tr->reset();
      return ERR_OK;

    case VAR_TYPE_TEXTUREF:
      *tr = reinterpret_cast<PatternTextureDataF*>(d)->transform();
      return ERR_OK;

    case VAR_TYPE_TEXTURED:
      *tr = reinterpret_cast<PatternTextureDataD*>(d)->transform();
      return ERR_OK;

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      *tr = reinterpret_cast<PatternGradientDataF*>(d)->transform();
      return ERR_OK;

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      *tr = reinterpret_cast<PatternGradientDataD*>(d)->transform();
      return ERR_OK;

    default:
      return ERR_RT_INVALID_STATE;
  }
}

static err_t FOG_CDECL Pattern_getTransformD(const Pattern* self, TransformD* tr)
{
  PatternData* d = self->_d;

  switch (d->vType & VAR_TYPE_MASK)
  {
    // Null and color pattern has no transform, but it's special kind of pattern
    // so we return ERR_OK instead of reporting an invalid state.
    case VAR_TYPE_NULL:
    case VAR_TYPE_COLOR:
      tr->reset();
      return ERR_OK;

    case VAR_TYPE_TEXTUREF:
      *tr = reinterpret_cast<PatternTextureDataF*>(d)->transform();
      return ERR_OK;

    case VAR_TYPE_TEXTURED:
      *tr = reinterpret_cast<PatternTextureDataD*>(d)->transform();
      return ERR_OK;

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      *tr = reinterpret_cast<PatternGradientDataF*>(d)->transform();
      return ERR_OK;

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      *tr = reinterpret_cast<PatternGradientDataD*>(d)->transform();
      return ERR_OK;

    default:
      return ERR_RT_INVALID_STATE;
  }
}

// ============================================================================
// [Fog::Pattern - Create]
// ============================================================================

static err_t FOG_CDECL Pattern_createArgb32(Pattern* self, const ArgbBase32* argb32)
{
  PatternColorData* d = reinterpret_cast<PatternColorData*>(self->_d);

  if (d->patternType != PATTERN_TYPE_COLOR || d->reference.get() != 1)
  {
    d = reinterpret_cast<PatternColorData*>(
      _api.pattern.dCreate(sizeof(PatternColorData)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_COLOR;
    d->patternType = PATTERN_TYPE_COLOR;

    self->_d->release();
    self->_d = d;
  }

  d->color.initCustom1(*argb32);
  return ERR_OK;
}

static err_t FOG_CDECL Pattern_createColor(Pattern* self, const Color* color)
{
  PatternColorData* d = reinterpret_cast<PatternColorData*>(self->_d);

  if (d->patternType != PATTERN_TYPE_COLOR || d->reference.get() != 1)
  {
    d = reinterpret_cast<PatternColorData*>(
      _api.pattern.dCreate(sizeof(PatternColorData)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_COLOR;
    d->patternType = PATTERN_TYPE_COLOR;

    self->_d->release();
    self->_d = d;
  }

  d->color.initCustom1(*color);
  return ERR_OK;
}

static err_t FOG_CDECL Pattern_createTextureF(Pattern* self, const Texture* texture, const TransformF* tr)
{
  PatternTextureDataF* d = reinterpret_cast<PatternTextureDataF*>(self->_d);

  if ((d->vType & VAR_TYPE_MASK) != VAR_TYPE_TEXTUREF || d->reference.get() != 1)
  {
    d = reinterpret_cast<PatternTextureDataF*>(
      _api.pattern.dCreate(sizeof(PatternTextureDataF)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_TEXTUREF;
    d->patternType = PATTERN_TYPE_TEXTURE;
    d->texture.initCustom1(*texture);
    d->transform.initCustom1(tr != NULL ? *tr : TransformF::identity());

    self->_d->release();
    self->_d = d;
    return ERR_OK;
  }
  else
  {
    d->texture() = *texture;
    d->transform() = tr != NULL ? *tr : TransformF::identity();
    return ERR_OK;
  }
}

static err_t FOG_CDECL Pattern_createTextureD(Pattern* self, const Texture* texture, const TransformD* tr)
{
  PatternTextureDataD* d = reinterpret_cast<PatternTextureDataD*>(self->_d);

  if ((d->vType & VAR_TYPE_MASK) != VAR_TYPE_TEXTUREF || d->reference.get() != 1)
  {
    d = reinterpret_cast<PatternTextureDataD*>(
      _api.pattern.dCreate(sizeof(PatternTextureDataD)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_TEXTUREF;
    d->patternType = PATTERN_TYPE_TEXTURE;
    d->texture.initCustom1(*texture);
    d->transform.initCustom1(tr != NULL ? *tr : TransformD::identity());

    self->_d->release();
    self->_d = d;
    return ERR_OK;
  }
  else
  {
    d->texture() = *texture;
    d->transform() = tr != NULL ? *tr : TransformD::identity();
    return ERR_OK;
  }
}

static err_t FOG_CDECL Pattern_createGradientF(Pattern* self, const GradientF* gradient, const TransformF* tr)
{
  // Pattern disallows to use invalid (uninitialized) gradient.
  if (gradient->getGradientType() >= GRADIENT_TYPE_COUNT)
  {
    _api.pattern.reset(self);
    return ERR_OK;
  }

  PatternGradientDataF* d = reinterpret_cast<PatternGradientDataF*>(self->_d);

  if ((d->vType & VAR_TYPE_MASK) != VAR_TYPE_TEXTUREF || d->reference.get() != 1)
  {
    d = reinterpret_cast<PatternGradientDataF*>(
      _api.pattern.dCreate(sizeof(PatternGradientDataF)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_TEXTUREF;
    d->patternType = PATTERN_TYPE_TEXTURE;
    d->gradient.initCustom1(*gradient);
    d->transform.initCustom1(tr != NULL ? *tr : TransformF::identity());

    self->_d->release();
    self->_d = d;
    return ERR_OK;
  }
  else
  {
    d->gradient().setGradient(*gradient);
    d->transform() = tr != NULL ? *tr : TransformF::identity();
    return ERR_OK;
  }
}

static err_t FOG_CDECL Pattern_createGradientD(Pattern* self, const GradientD* gradient, const TransformD* tr)
{
  // Pattern disallows to use invalid (uninitialized) gradient.
  if (gradient->getGradientType() >= GRADIENT_TYPE_COUNT)
  {
    _api.pattern.reset(self);
    return ERR_OK;
  }

  PatternGradientDataD* d = reinterpret_cast<PatternGradientDataD*>(self->_d);

  if ((d->vType & VAR_TYPE_MASK) != VAR_TYPE_TEXTUREF || d->reference.get() != 1)
  {
    d = reinterpret_cast<PatternGradientDataD*>(
      _api.pattern.dCreate(sizeof(PatternGradientDataD)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_TEXTUREF;
    d->patternType = PATTERN_TYPE_TEXTURE;
    d->gradient.initCustom1(*gradient);
    d->transform.initCustom1(tr != NULL ? *tr : TransformD::identity());

    self->_d->release();
    self->_d = d;
    return ERR_OK;
  }
  else
  {
    d->gradient().setGradient(*gradient);
    d->transform() = tr != NULL ? *tr : TransformD::identity();
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::Pattern - Trasnform]
// ============================================================================

static err_t FOG_CDECL Pattern_setTransformF(Pattern* self, const TransformF* tr)
{
  PatternData* d = self->_d;

  if (!Math::isBounded<uint32_t>(d->patternType, PATTERN_TYPE_TEXTURE, PATTERN_TYPE_GRADIENT))
    return ERR_RT_INVALID_STATE;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(self->detach());
    d = self->_d;
  }

  TransformF* dstf = NULL;
  TransformD* dstd = NULL;

  switch (d->vType & VAR_TYPE_MASK)
  {
    case VAR_TYPE_TEXTUREF:
      dstf = &reinterpret_cast<PatternTextureDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_TEXTURED:
      dstd = &reinterpret_cast<PatternTextureDataD*>(d)->transform;
      goto _DoTransformD;

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      dstf = &reinterpret_cast<PatternGradientDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      dstd = &reinterpret_cast<PatternGradientDataD*>(d)->transform;
      goto _DoTransformD;
  }

  return ERR_RT_INVALID_STATE;

_DoTransformF:
  *dstf = *tr;
  return ERR_OK;

_DoTransformD:
  *dstd = *tr;
  return ERR_OK;
}

static err_t FOG_CDECL Pattern_setTransformD(Pattern* self, const TransformD* tr)
{
  PatternData* d = self->_d;

  if (!Math::isBounded<uint32_t>(d->patternType, PATTERN_TYPE_TEXTURE, PATTERN_TYPE_GRADIENT))
    return ERR_RT_INVALID_STATE;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(self->detach());
    d = self->_d;
  }

  TransformF* dstf = NULL;
  TransformD* dstd = NULL;

  switch (d->vType & VAR_TYPE_MASK)
  {
    case VAR_TYPE_TEXTUREF:
      dstf = &reinterpret_cast<PatternTextureDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_TEXTURED:
      dstd = &reinterpret_cast<PatternTextureDataD*>(d)->transform;
      goto _DoTransformD;

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      dstf = &reinterpret_cast<PatternGradientDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      dstd = &reinterpret_cast<PatternGradientDataD*>(d)->transform;
      goto _DoTransformD;
  }

  return ERR_RT_INVALID_STATE;

_DoTransformF:
  *dstf = *tr;
  return ERR_OK;

_DoTransformD:
  *dstd = *tr;
  return ERR_OK;
}

static err_t FOG_CDECL Pattern_applyTransform(Pattern* self, uint32_t transformOp, const void* params)
{
  PatternData* d = self->_d;

  if (!Math::isBounded<uint32_t>(d->patternType, PATTERN_TYPE_TEXTURE, PATTERN_TYPE_GRADIENT))
    return ERR_RT_INVALID_STATE;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(self->detach());
    d = self->_d;
  }

  TransformF* dstf = NULL;
  TransformD* dstd = NULL;

  switch (d->vType & VAR_TYPE_MASK)
  {
    case VAR_TYPE_TEXTUREF:
      dstf = &reinterpret_cast<PatternTextureDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_TEXTURED:
      dstd = &reinterpret_cast<PatternTextureDataD*>(d)->transform;
      goto _DoTransformD;

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      dstf = &reinterpret_cast<PatternGradientDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      dstd = &reinterpret_cast<PatternGradientDataD*>(d)->transform;
      goto _DoTransformD;
  }

  return ERR_RT_INVALID_STATE;

_DoTransformF:
  return dstf->_transform(transformOp, params);

_DoTransformD:
  return dstd->_transform(transformOp, params);
}

static err_t FOG_CDECL Pattern_resetTransform(Pattern* self)
{
  PatternData* d = self->_d;

  if (!Math::isBounded<uint32_t>(d->patternType, PATTERN_TYPE_TEXTURE, PATTERN_TYPE_GRADIENT))
    return ERR_RT_INVALID_STATE;

  if (d->reference.get() != 1)
  {
    FOG_RETURN_ON_ERROR(self->detach());
    d = self->_d;
  }

  TransformF* dstf = NULL;
  TransformD* dstd = NULL;

  switch (d->vType & VAR_TYPE_MASK)
  {
    case VAR_TYPE_TEXTUREF:
      dstf = &reinterpret_cast<PatternTextureDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_TEXTURED:
      dstd = &reinterpret_cast<PatternTextureDataD*>(d)->transform;
      goto _DoTransformD;

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      dstf = &reinterpret_cast<PatternGradientDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      dstd = &reinterpret_cast<PatternGradientDataD*>(d)->transform;
      goto _DoTransformD;
  }

  return ERR_RT_INVALID_STATE;

_DoTransformF:
  dstf->reset();
  return ERR_OK;

_DoTransformD:
  dstd->reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::Pattern - Reset]
// ============================================================================

static void FOG_CDECL Pattern_reset(Pattern* self)
{
  atomicPtrXchg(&self->_d, _api.pattern.oNull->_d)->release();
}

// ============================================================================
// [Fog::Pattern - Copy]
// ============================================================================

static err_t FOG_CDECL Pattern_copy(Pattern* self, const Pattern* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Pattern - Equality]
// ============================================================================

static bool FOG_CDECL Pattern_eq(const Pattern* a, const Pattern* b)
{
  const PatternData* a_d = a->_d;
  const PatternData* b_d = b->_d;

  if (a_d == b_d)
    return true;

  uint32_t patternType = a_d->patternType;
  if (patternType != b_d->patternType)
    return false;

  // TODO: Pattern comparison.
  return false;
}

// ============================================================================
// [Fog::Pattern - PatternData]
// ============================================================================

static PatternData* FOG_CDECL Pattern_dCreate(size_t size)
{
  PatternData* newd = reinterpret_cast<PatternData*>(MemMgr::alloc(size));

  if (FOG_IS_NULL(newd))
    return NULL;

  newd->reference.init(1);
  return newd;
}

static void FOG_CDECL Pattern_dFree(PatternData* d)
{
  uint32_t patternType = d->patternType;

  if (patternType == PATTERN_TYPE_TEXTURE)
    reinterpret_cast<PatternTextureDataF*>(d)->texture.destroy();
  else if (patternType == PATTERN_TYPE_GRADIENT)
    reinterpret_cast<PatternGradientDataF*>(d)->gradient->_stops.destroy();

  if ((d->vType & VAR_FLAG_STATIC) == 0)
    MemMgr::free(d);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Pattern_init(void)
{
  _api.pattern.ctor = Pattern_ctor;
  _api.pattern.ctorCopy = Pattern_ctorCopy;
  _api.pattern.ctorArgb32 = Pattern_ctorArgb32;
  _api.pattern.ctorColor = Pattern_ctorColor;
  _api.pattern.ctorTextureF = Pattern_ctorTextureF;
  _api.pattern.ctorTextureD = Pattern_ctorTextureD;
  _api.pattern.ctorGradientF = Pattern_ctorGradientF;
  _api.pattern.ctorGradientD = Pattern_ctorGradientD;
  _api.pattern.dtor = Pattern_dtor;

  _api.pattern.detach = Pattern_detach;

  _api.pattern.getArgb32 = Pattern_getArgb32;
  _api.pattern.getColor = Pattern_getColor;
  _api.pattern.getTexture = Pattern_getTexture;
  _api.pattern.getGradientF = Pattern_getGradientF;
  _api.pattern.getGradientD = Pattern_getGradientD;
  _api.pattern.getTransformF = Pattern_getTransformF;
  _api.pattern.getTransformD = Pattern_getTransformD;

  _api.pattern.createArgb32 = Pattern_createArgb32;
  _api.pattern.createColor = Pattern_createColor;
  _api.pattern.createTextureF = Pattern_createTextureF;
  _api.pattern.createTextureD = Pattern_createTextureD;
  _api.pattern.createGradientF = Pattern_createGradientF;
  _api.pattern.createGradientD = Pattern_createGradientD;

  _api.pattern.setTransformF = Pattern_setTransformF;
  _api.pattern.setTransformD = Pattern_setTransformD;
  _api.pattern.applyTransform = Pattern_applyTransform;
  _api.pattern.resetTransform = Pattern_resetTransform;

  _api.pattern.reset = Pattern_reset;
  _api.pattern.copy = Pattern_copy;
  _api.pattern.eq = Pattern_eq;

  _api.pattern.dCreate = Pattern_dCreate;
  _api.pattern.dFree = Pattern_dFree;

  _api.pattern.oNull = reinterpret_cast<Pattern*>(_api.var.oNull);
}

} // Fog namespace
