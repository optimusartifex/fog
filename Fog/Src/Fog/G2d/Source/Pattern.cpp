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
  /* 00: GRADIENT_TYPE_LINEAR      */ VAR_TYPE_LINEAR_GRADIENTD,
  /* 01: GRADIENT_TYPE_RADIAL      */ VAR_TYPE_RADIAL_GRADIENTD,
  /* 02: GRADIENT_TYPE_CONICAL     */ VAR_TYPE_CONICAL_GRADIENTD,
  /* 03: GRADIENT_TYPE_RECTANGULAR */ VAR_TYPE_RECTANGULAR_GRADIENTD
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
  return static_cast<PatternGradientDataF*>(d)->gradient().getGradientType();
}

// ============================================================================
// [Fog::Pattern - Construction / Destruction]
// ============================================================================

static void FOG_CDECL Pattern_ctor(Pattern* self)
{
  self->_d = fog_api.pattern_oNull->_d;
}

static void FOG_CDECL Pattern_ctorCopy(Pattern* self, const Pattern* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL Pattern_ctorArgb32(Pattern* self, const ArgbBase32* argb32)
{
  PatternColorData* d = static_cast<PatternColorData*>(
    fog_api.pattern_dCreate(sizeof(PatternColorData)));

  if (FOG_IS_NULL(d))
  {
    self->_d = fog_api.pattern_oNull->_d;
    return;
  }

  d->vType = VAR_TYPE_COLOR;
  d->patternType = PATTERN_TYPE_COLOR;
  d->color.initCustom1(*argb32);

  self->_d = d;
}

static void FOG_CDECL Pattern_ctorColor(Pattern* self, const Color* color)
{
  PatternColorData* d = static_cast<PatternColorData*>(
    fog_api.pattern_dCreate(sizeof(PatternColorData)));

  if (FOG_IS_NULL(d))
  {
    self->_d = fog_api.pattern_oNull->_d;
    return;
  }

  d->vType = VAR_TYPE_COLOR;
  d->patternType = PATTERN_TYPE_COLOR;
  d->color.initCustom1(*color);

  self->_d = d;
}

static void FOG_CDECL Pattern_ctorTextureF(Pattern* self, const Texture* texture, const TransformF* tr)
{
  PatternTextureDataF* d = static_cast<PatternTextureDataF*>(
    fog_api.pattern_dCreate(sizeof(PatternTextureDataF)));

  if (FOG_IS_NULL(d))
  {
    self->_d = fog_api.pattern_oNull->_d;
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
    self->_d = fog_api.pattern_oNull->_d;
    return;
  }

  PatternTextureDataD* d = static_cast<PatternTextureDataD*>(
    fog_api.pattern_dCreate(sizeof(PatternTextureDataD)));

  if (FOG_IS_NULL(d))
  {
    self->_d = fog_api.pattern_oNull->_d;
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
    self->_d = fog_api.pattern_oNull->_d;
    return;
  }

  PatternGradientDataF* d = static_cast<PatternGradientDataF*>(
    fog_api.pattern_dCreate(sizeof(PatternGradientDataF)));

  if (FOG_IS_NULL(d))
  {
    self->_d = fog_api.pattern_oNull->_d;
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
    self->_d = fog_api.pattern_oNull->_d;
    return;
  }

  PatternGradientDataD* d = static_cast<PatternGradientDataD*>(
    fog_api.pattern_dCreate(sizeof(PatternGradientDataD)));

  if (FOG_IS_NULL(d))
  {
    self->_d = fog_api.pattern_oNull->_d;
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
  PatternData* d = self->_d;

  if (d != NULL)
    d->release();
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
      PatternColorData* newd = static_cast<PatternColorData*>(
        fog_api.pattern_dCreate(sizeof(PatternColorData)));

      if (FOG_IS_NULL(newd))
        return ERR_RT_OUT_OF_MEMORY;

      newd->vType = vType;
      newd->patternType = d->patternType;
      newd->color.initCustom1(static_cast<PatternColorData*>(d)->color());

      atomicPtrXchg(&self->_d, static_cast<PatternData*>(newd))->release();
      return ERR_OK;
    }

    case VAR_TYPE_TEXTUREF:
    {
      PatternTextureDataF* newd = static_cast<PatternTextureDataF*>(
        fog_api.pattern_dCreate(sizeof(PatternTextureDataF)));

      if (FOG_IS_NULL(newd))
        return ERR_RT_OUT_OF_MEMORY;

      newd->vType = vType;
      newd->patternType = d->patternType;
      newd->texture.initCustom1(static_cast<PatternTextureDataF*>(d)->texture());
      newd->transform.initCustom1(static_cast<PatternTextureDataF*>(d)->transform());

      atomicPtrXchg(&self->_d, static_cast<PatternData*>(newd))->release();
      return ERR_OK;
    }

    case VAR_TYPE_TEXTURED:
    {
      PatternTextureDataD* newd = static_cast<PatternTextureDataD*>(
        fog_api.pattern_dCreate(sizeof(PatternTextureDataD)));

      if (FOG_IS_NULL(newd))
        return ERR_RT_OUT_OF_MEMORY;

      newd->vType = vType;
      newd->patternType = d->patternType;
      newd->texture.initCustom1(static_cast<PatternTextureDataD*>(d)->texture());
      newd->transform.initCustom1(static_cast<PatternTextureDataD*>(d)->transform());

      atomicPtrXchg(&self->_d, static_cast<PatternData*>(newd))->release();
      return ERR_OK;
    }

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
    {
      PatternGradientDataF* newd = static_cast<PatternGradientDataF*>(
        fog_api.pattern_dCreate(sizeof(PatternGradientDataF)));

      if (FOG_IS_NULL(newd))
        return ERR_RT_OUT_OF_MEMORY;

      newd->vType = vType;
      newd->patternType = d->patternType;
      newd->gradient.initCustom1(static_cast<PatternGradientDataF*>(d)->gradient());
      newd->transform.initCustom1(static_cast<PatternGradientDataF*>(d)->transform());

      atomicPtrXchg(&self->_d, static_cast<PatternData*>(newd))->release();
      return ERR_OK;
    }

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
    {
      PatternGradientDataD* newd = static_cast<PatternGradientDataD*>(
        fog_api.pattern_dCreate(sizeof(PatternGradientDataD)));

      if (FOG_IS_NULL(newd))
        return ERR_RT_OUT_OF_MEMORY;

      newd->vType = vType;
      newd->patternType = d->patternType;
      newd->gradient.initCustom1(static_cast<PatternGradientDataD*>(d)->gradient());
      newd->transform.initCustom1(static_cast<PatternGradientDataD*>(d)->transform());

      atomicPtrXchg(&self->_d, static_cast<PatternData*>(newd))->release();
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
  PatternColorData* d = static_cast<PatternColorData*>(self->_d);

  if (d->patternType != PATTERN_TYPE_COLOR)
    return ERR_RT_INVALID_STATE;

  *argb32 = d->color->getArgb32();
  return ERR_OK;
}

static err_t FOG_CDECL Pattern_getColor(const Pattern* self, Color* color)
{
  PatternColorData* d = static_cast<PatternColorData*>(self->_d);

  if (d->patternType != PATTERN_TYPE_COLOR)
    return ERR_RT_INVALID_STATE;

  *color = d->color();
  return ERR_OK;
}

static err_t FOG_CDECL Pattern_getTexture(const Pattern* self, Texture* texture)
{
  PatternTextureDataF* d = static_cast<PatternTextureDataF*>(self->_d);

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
      return gradient->setGradient(static_cast<PatternGradientDataF*>(d)->gradient());

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      return gradient->setGradient(static_cast<PatternGradientDataD*>(d)->gradient());

    default:
      return ERR_RT_INVALID_STATE;
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
      return gradient->setGradient(static_cast<PatternGradientDataF*>(d)->gradient());

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      return gradient->setGradient(static_cast<PatternGradientDataD*>(d)->gradient());

    default:
      return ERR_RT_INVALID_STATE;
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
      *tr = static_cast<PatternTextureDataF*>(d)->transform();
      return ERR_OK;

    case VAR_TYPE_TEXTURED:
      *tr = static_cast<PatternTextureDataD*>(d)->transform();
      return ERR_OK;

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      *tr = static_cast<PatternGradientDataF*>(d)->transform();
      return ERR_OK;

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      *tr = static_cast<PatternGradientDataD*>(d)->transform();
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
      *tr = static_cast<PatternTextureDataF*>(d)->transform();
      return ERR_OK;

    case VAR_TYPE_TEXTURED:
      *tr = static_cast<PatternTextureDataD*>(d)->transform();
      return ERR_OK;

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      *tr = static_cast<PatternGradientDataF*>(d)->transform();
      return ERR_OK;

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      *tr = static_cast<PatternGradientDataD*>(d)->transform();
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
  PatternColorData* d = static_cast<PatternColorData*>(self->_d);

  if (d->patternType == PATTERN_TYPE_COLOR && d->reference.get() == 1)
  {
    d->color.initCustom1(*argb32);
    return ERR_OK;
  }
  else
  {
    d = static_cast<PatternColorData*>(
      fog_api.pattern_dCreate(sizeof(PatternColorData)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_COLOR;
    d->patternType = PATTERN_TYPE_COLOR;
    d->color.initCustom1(*argb32);

    self->_d->release();
    self->_d = d;
    return ERR_OK;
  }
}

static err_t FOG_CDECL Pattern_createColor(Pattern* self, const Color* color)
{
  PatternColorData* d = static_cast<PatternColorData*>(self->_d);

  if (d->patternType == PATTERN_TYPE_COLOR && d->reference.get() == 1)
  {
    d->color.initCustom1(*color);
    return ERR_OK;
  }
  else
  {
    d = static_cast<PatternColorData*>(
      fog_api.pattern_dCreate(sizeof(PatternColorData)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_COLOR;
    d->patternType = PATTERN_TYPE_COLOR;
    d->color.initCustom1(*color);

    self->_d->release();
    self->_d = d;
    return ERR_OK;
  }
}

static err_t FOG_CDECL Pattern_createTextureF(Pattern* self, const Texture* texture, const TransformF* tr)
{
  PatternTextureDataF* d = static_cast<PatternTextureDataF*>(self->_d);
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  if (vType == VAR_TYPE_TEXTUREF && d->reference.get() == 1)
  {
    d->texture() = *texture;
    d->transform() = tr != NULL ? *tr : TransformF::identity();
    return ERR_OK;
  }
  else
  {
    d = static_cast<PatternTextureDataF*>(
      fog_api.pattern_dCreate(sizeof(PatternTextureDataF)));

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
}

static err_t FOG_CDECL Pattern_createTextureD(Pattern* self, const Texture* texture, const TransformD* tr)
{
  PatternTextureDataD* d = static_cast<PatternTextureDataD*>(self->_d);
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  if (vType == VAR_TYPE_TEXTURED && d->reference.get() == 1)
  {
    d->texture() = *texture;
    d->transform() = tr != NULL ? *tr : TransformD::identity();
    return ERR_OK;
  }
  else
  {
    d = static_cast<PatternTextureDataD*>(
      fog_api.pattern_dCreate(sizeof(PatternTextureDataD)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_TEXTURED;
    d->patternType = PATTERN_TYPE_TEXTURE;
    d->texture.initCustom1(*texture);
    d->transform.initCustom1(tr != NULL ? *tr : TransformD::identity());

    self->_d->release();
    self->_d = d;
    return ERR_OK;
  }
}

static err_t FOG_CDECL Pattern_createGradientF(Pattern* self, const GradientF* gradient, const TransformF* tr)
{
  // Pattern doesn't allow to use invalid (uninitialized) gradient.
  uint32_t gradientType = gradient->getGradientType();
  if (gradientType >= GRADIENT_TYPE_COUNT)
  {
    fog_api.pattern_reset(self);
    return ERR_OK;
  }

  PatternGradientDataF* d = static_cast<PatternGradientDataF*>(self->_d);
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  if ((vType == VAR_TYPE_LINEAR_GRADIENTF  || vType == VAR_TYPE_RADIAL_GRADIENTF       ||
       vType == VAR_TYPE_CONICAL_GRADIENTF || vType == VAR_TYPE_RECTANGULAR_GRADIENTF) &&
      d->reference.get() == 1)
  {
    d->vType = Pattern_vTypeFromGradientTableF[gradientType] | (d->vType & ~VAR_TYPE_MASK);
    d->gradient().setGradient(*gradient);
    d->transform() = tr != NULL ? *tr : TransformF::identity();
    return ERR_OK;
  }
  else
  {
    d = static_cast<PatternGradientDataF*>(
      fog_api.pattern_dCreate(sizeof(PatternGradientDataF)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = Pattern_vTypeFromGradientTableF[gradientType];
    d->patternType = PATTERN_TYPE_GRADIENT;
    d->gradient.initCustom1(*gradient);
    d->transform.initCustom1(tr != NULL ? *tr : TransformF::identity());

    self->_d->release();
    self->_d = d;
    return ERR_OK;
  }
}

static err_t FOG_CDECL Pattern_createGradientD(Pattern* self, const GradientD* gradient, const TransformD* tr)
{
  // Pattern doesn't allow to use invalid (uninitialized) gradient.
  uint32_t gradientType = gradient->getGradientType();
  if (gradientType >= GRADIENT_TYPE_COUNT)
  {
    fog_api.pattern_reset(self);
    return ERR_OK;
  }

  PatternGradientDataD* d = static_cast<PatternGradientDataD*>(self->_d);
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  if ((vType == VAR_TYPE_LINEAR_GRADIENTD  || vType == VAR_TYPE_RADIAL_GRADIENTD       ||
       vType == VAR_TYPE_CONICAL_GRADIENTD || vType == VAR_TYPE_RECTANGULAR_GRADIENTD) &&
      d->reference.get() == 1)
  {
    d->vType = Pattern_vTypeFromGradientTableD[gradientType] | (d->vType & ~VAR_TYPE_MASK);
    d->gradient().setGradient(*gradient);
    d->transform() = tr != NULL ? *tr : TransformD::identity();
    return ERR_OK;
  }
  else
  {
    d = static_cast<PatternGradientDataD*>(
      fog_api.pattern_dCreate(sizeof(PatternGradientDataD)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = Pattern_vTypeFromGradientTableD[gradientType];
    d->patternType = PATTERN_TYPE_GRADIENT;
    d->gradient.initCustom1(*gradient);
    d->transform.initCustom1(tr != NULL ? *tr : TransformD::identity());

    self->_d->release();
    self->_d = d;
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
      dstf = &static_cast<PatternTextureDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_TEXTURED:
      dstd = &static_cast<PatternTextureDataD*>(d)->transform;
      goto _DoTransformD;

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      dstf = &static_cast<PatternGradientDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      dstd = &static_cast<PatternGradientDataD*>(d)->transform;
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
      dstf = &static_cast<PatternTextureDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_TEXTURED:
      dstd = &static_cast<PatternTextureDataD*>(d)->transform;
      goto _DoTransformD;

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      dstf = &static_cast<PatternGradientDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      dstd = &static_cast<PatternGradientDataD*>(d)->transform;
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
      dstf = &static_cast<PatternTextureDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_TEXTURED:
      dstd = &static_cast<PatternTextureDataD*>(d)->transform;
      goto _DoTransformD;

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      dstf = &static_cast<PatternGradientDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      dstd = &static_cast<PatternGradientDataD*>(d)->transform;
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
      dstf = &static_cast<PatternTextureDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_TEXTURED:
      dstd = &static_cast<PatternTextureDataD*>(d)->transform;
      goto _DoTransformD;

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
      dstf = &static_cast<PatternGradientDataF*>(d)->transform;
      goto _DoTransformF;

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
      dstd = &static_cast<PatternGradientDataD*>(d)->transform;
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
  atomicPtrXchg(&self->_d, fog_api.pattern_oNull->_d)->release();
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

  uint32_t vType = a_d->vType;
  if (vType != b_d->vType)
    return false;

  switch (vType)
  {
    // Redundant check, because in such case a_d has to be equal with b_d.
    case VAR_TYPE_NULL:
      return true;

    case VAR_TYPE_COLOR:
    {
      const PatternColorData* a_data = static_cast<const PatternColorData*>(a_d);
      const PatternColorData* b_data = static_cast<const PatternColorData*>(b_d);

      return a_data->color() == b_data->color();
    }

    case VAR_TYPE_TEXTUREF:
    {
      const PatternTextureDataF* a_data = static_cast<const PatternTextureDataF*>(a_d);
      const PatternTextureDataF* b_data = static_cast<const PatternTextureDataF*>(b_d);

      return a_data->texture() == b_data->texture() &&
             a_data->transform() == b_data->transform();
    }

    case VAR_TYPE_TEXTURED:
    {
      const PatternTextureDataD* a_data = static_cast<const PatternTextureDataD*>(a_d);
      const PatternTextureDataD* b_data = static_cast<const PatternTextureDataD*>(b_d);

      return a_data->texture() == b_data->texture() &&
             a_data->transform() == b_data->transform();
    }

    case VAR_TYPE_LINEAR_GRADIENTF:
    case VAR_TYPE_RADIAL_GRADIENTF:
    case VAR_TYPE_CONICAL_GRADIENTF:
    case VAR_TYPE_RECTANGULAR_GRADIENTF:
    {
      const PatternGradientDataF* a_data = static_cast<const PatternGradientDataF*>(a_d);
      const PatternGradientDataF* b_data = static_cast<const PatternGradientDataF*>(b_d);

      return a_data->gradient() == b_data->gradient() &&
             a_data->transform() == b_data->transform();
    }

    case VAR_TYPE_LINEAR_GRADIENTD:
    case VAR_TYPE_RADIAL_GRADIENTD:
    case VAR_TYPE_CONICAL_GRADIENTD:
    case VAR_TYPE_RECTANGULAR_GRADIENTD:
    {
      const PatternGradientDataD* a_data = static_cast<const PatternGradientDataD*>(a_d);
      const PatternGradientDataD* b_data = static_cast<const PatternGradientDataD*>(b_d);

      return a_data->gradient() == b_data->gradient() &&
             a_data->transform() == b_data->transform();
    }
    
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return false;
}

// ============================================================================
// [Fog::Pattern - PatternData]
// ============================================================================

static PatternData* FOG_CDECL Pattern_dCreate(size_t size)
{
  PatternData* newd = static_cast<PatternData*>(MemMgr::alloc(size));

  if (FOG_IS_NULL(newd))
    return NULL;

  newd->reference.init(1);
  return newd;
}

static void FOG_CDECL Pattern_dFree(PatternData* d)
{
  uint32_t patternType = d->patternType;

  if (patternType == PATTERN_TYPE_TEXTURE)
    static_cast<PatternTextureDataF*>(d)->texture.destroy();
  else if (patternType == PATTERN_TYPE_GRADIENT)
    static_cast<PatternGradientDataF*>(d)->gradient->_stops.destroy();

  if ((d->vType & VAR_FLAG_STATIC) == 0)
    MemMgr::free(d);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Pattern_init(void)
{
  fog_api.pattern_ctor = Pattern_ctor;
  fog_api.pattern_ctorCopy = Pattern_ctorCopy;
  fog_api.pattern_ctorArgb32 = Pattern_ctorArgb32;
  fog_api.pattern_ctorColor = Pattern_ctorColor;
  fog_api.pattern_ctorTextureF = Pattern_ctorTextureF;
  fog_api.pattern_ctorTextureD = Pattern_ctorTextureD;
  fog_api.pattern_ctorGradientF = Pattern_ctorGradientF;
  fog_api.pattern_ctorGradientD = Pattern_ctorGradientD;
  fog_api.pattern_dtor = Pattern_dtor;

  fog_api.pattern_detach = Pattern_detach;

  fog_api.pattern_getArgb32 = Pattern_getArgb32;
  fog_api.pattern_getColor = Pattern_getColor;
  fog_api.pattern_getTexture = Pattern_getTexture;
  fog_api.pattern_getGradientF = Pattern_getGradientF;
  fog_api.pattern_getGradientD = Pattern_getGradientD;
  fog_api.pattern_getTransformF = Pattern_getTransformF;
  fog_api.pattern_getTransformD = Pattern_getTransformD;

  fog_api.pattern_createArgb32 = Pattern_createArgb32;
  fog_api.pattern_createColor = Pattern_createColor;
  fog_api.pattern_createTextureF = Pattern_createTextureF;
  fog_api.pattern_createTextureD = Pattern_createTextureD;
  fog_api.pattern_createGradientF = Pattern_createGradientF;
  fog_api.pattern_createGradientD = Pattern_createGradientD;

  fog_api.pattern_setTransformF = Pattern_setTransformF;
  fog_api.pattern_setTransformD = Pattern_setTransformD;
  fog_api.pattern_applyTransform = Pattern_applyTransform;
  fog_api.pattern_resetTransform = Pattern_resetTransform;

  fog_api.pattern_reset = Pattern_reset;
  fog_api.pattern_copy = Pattern_copy;
  fog_api.pattern_eq = Pattern_eq;

  fog_api.pattern_dCreate = Pattern_dCreate;
  fog_api.pattern_dFree = Pattern_dFree;

  fog_api.pattern_oNull = reinterpret_cast<Pattern*>(fog_api.var_oNull);
}

} // Fog namespace
