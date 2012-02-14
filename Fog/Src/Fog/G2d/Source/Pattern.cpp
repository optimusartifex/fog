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
  d->pType = PATTERN_TYPE_COLOR;
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
  d->pType = PATTERN_TYPE_COLOR;
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

  d->vType = VAR_TYPE_PATTERN;
  d->pType = PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_F;

  d->transform.initCustom1(tr != NULL ? *tr : TransformF::getIdentityInstance());
  d->texture.initCustom1(*texture);

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

  d->vType = VAR_TYPE_PATTERN;
  d->pType = PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_D;

  d->transform.initCustom1(tr != NULL ? *tr : TransformD::getIdentityInstance());
  d->texture.initCustom1(*texture);

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

  d->vType = VAR_TYPE_PATTERN;
  d->pType = PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_F;

  d->transform.initCustom1(tr != NULL ? *tr : TransformF::getIdentityInstance());
  d->gradient.initCustom1(*gradient);

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

  d->vType = VAR_TYPE_PATTERN;
  d->pType = PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_D;

  d->transform.initCustom1(tr != NULL ? *tr : TransformD::getIdentityInstance());
  d->gradient.initCustom1(*gradient);

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
      newd->pType = d->pType;
      newd->color.initCustom1(static_cast<PatternColorData*>(d)->color());

      atomicPtrXchg(&self->_d, static_cast<PatternData*>(newd))->release();
      return ERR_OK;
    }

    case VAR_TYPE_PATTERN:
    {
      switch (d->pType)
      {
        case PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_F:
        {
          PatternTextureDataF* newd = static_cast<PatternTextureDataF*>(
            fog_api.pattern_dCreate(sizeof(PatternTextureDataF)));

          if (FOG_IS_NULL(newd))
            return ERR_RT_OUT_OF_MEMORY;

          newd->vType = vType;
          newd->pType = d->pType;
          newd->transform.initCustom1(static_cast<PatternTextureDataF*>(d)->transform());
          newd->texture.initCustom1(static_cast<PatternTextureDataF*>(d)->texture());

          atomicPtrXchg(&self->_d, static_cast<PatternData*>(newd))->release();
          return ERR_OK;
        }
        
        case PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_D:
        {
          PatternTextureDataD* newd = static_cast<PatternTextureDataD*>(
            fog_api.pattern_dCreate(sizeof(PatternTextureDataD)));

          if (FOG_IS_NULL(newd))
            return ERR_RT_OUT_OF_MEMORY;

          newd->vType = vType;
          newd->pType = d->pType;
          newd->texture.initCustom1(static_cast<PatternTextureDataD*>(d)->texture());
          newd->transform.initCustom1(static_cast<PatternTextureDataD*>(d)->transform());

          atomicPtrXchg(&self->_d, static_cast<PatternData*>(newd))->release();
          return ERR_OK;
        }

        case PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_F:
        {
          PatternGradientDataF* newd = static_cast<PatternGradientDataF*>(
            fog_api.pattern_dCreate(sizeof(PatternGradientDataF)));

          if (FOG_IS_NULL(newd))
            return ERR_RT_OUT_OF_MEMORY;

          newd->vType = vType;
          newd->pType = d->pType;
          newd->gradient.initCustom1(static_cast<PatternGradientDataF*>(d)->gradient());
          newd->transform.initCustom1(static_cast<PatternGradientDataF*>(d)->transform());

          atomicPtrXchg(&self->_d, static_cast<PatternData*>(newd))->release();
          return ERR_OK;
        }

        case PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_D:
        {
          PatternGradientDataD* newd = static_cast<PatternGradientDataD*>(
            fog_api.pattern_dCreate(sizeof(PatternGradientDataD)));

          if (FOG_IS_NULL(newd))
            return ERR_RT_OUT_OF_MEMORY;

          newd->vType = vType;
          newd->pType = d->pType;
          newd->gradient.initCustom1(static_cast<PatternGradientDataD*>(d)->gradient());
          newd->transform.initCustom1(static_cast<PatternGradientDataD*>(d)->transform());

          atomicPtrXchg(&self->_d, static_cast<PatternData*>(newd))->release();
          return ERR_OK;
        }
      }
    }
  }

  // Shouldn't be reached.
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::Pattern - Accessors]
// ============================================================================

static err_t FOG_CDECL Pattern_getArgb32(const Pattern* self, ArgbBase32* argb32)
{
  PatternColorData* d = static_cast<PatternColorData*>(self->_d);

  if (d->pType == PATTERN_TYPE_COLOR)
  {
    *argb32 = d->color->getArgb32();
    return ERR_OK;
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL Pattern_getColor(const Pattern* self, Color* color)
{
  PatternColorData* d = static_cast<PatternColorData*>(self->_d);

  if (d->pType == PATTERN_TYPE_COLOR)
  {
    *color = d->color();
    return ERR_OK;
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL Pattern_getTexture(const Pattern* self, Texture* texture)
{
  PatternData* d = static_cast<PatternData*>(self->_d);

  if (d->pType == (PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_F))
  {
    texture->setTexture(static_cast<PatternTextureDataF*>(d)->texture());
    return ERR_OK;
  }

  if (d->pType == (PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_D))
  {
    texture->setTexture(static_cast<PatternTextureDataD*>(d)->texture());
    return ERR_OK;
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL Pattern_getGradientF(const Pattern* self, uint32_t targetType, GradientF* gradient)
{
  PatternData* d = self->_d;

  if (d->pType == (PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_F))
  {
    PatternGradientDataF* p_gradient = reinterpret_cast<PatternGradientDataF*>(d);
    if (targetType != 0xFFFFFFFF && p_gradient->gradient().getGradientType() != targetType)
      return ERR_RT_INVALID_STATE;

    return gradient->setGradient(p_gradient->gradient());
  }

  if (d->pType == (PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_D))
  {
    PatternGradientDataD* p_gradient = reinterpret_cast<PatternGradientDataD*>(d);
    if (targetType != 0xFFFFFFFF && p_gradient->gradient().getGradientType() != targetType)
      return ERR_RT_INVALID_STATE;

    return gradient->setGradient(p_gradient->gradient());
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL Pattern_getGradientD(const Pattern* self, uint32_t targetType, GradientD* gradient)
{
  PatternData* d = self->_d;

  if (d->pType == (PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_F))
  {
    PatternGradientDataF* p_gradient = reinterpret_cast<PatternGradientDataF*>(d);
    if (targetType != 0xFFFFFFFF && p_gradient->gradient().getGradientType() != targetType)
      return ERR_RT_INVALID_STATE;

    return gradient->setGradient(p_gradient->gradient());
  }

  if (d->pType == (PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_D))
  {
    PatternGradientDataD* p_gradient = reinterpret_cast<PatternGradientDataD*>(d);
    if (targetType != 0xFFFFFFFF && p_gradient->gradient().getGradientType() != targetType)
      return ERR_RT_INVALID_STATE;

    return gradient->setGradient(p_gradient->gradient());
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL Pattern_getTransformF(const Pattern* self, TransformF* tr)
{
  PatternData* d = self->_d;

  if ((d->pType & PATTERN_PRECISION_MASK) == PATTERN_PRECISION_F)
  {
    *tr = static_cast<PatternBaseDataF*>(d)->transform();
    return ERR_OK;
  }

  if ((d->pType & PATTERN_PRECISION_MASK) == PATTERN_PRECISION_D)
  {
    *tr = static_cast<PatternBaseDataD*>(d)->transform();
    return ERR_OK;
  }

  tr->reset();
  return ERR_OK;
}

static err_t FOG_CDECL Pattern_getTransformD(const Pattern* self, TransformD* tr)
{
  PatternData* d = self->_d;

  if ((d->pType & PATTERN_PRECISION_MASK) == PATTERN_PRECISION_F)
  {
    *tr = static_cast<PatternBaseDataF*>(d)->transform();
    return ERR_OK;
  }

  if ((d->pType & PATTERN_PRECISION_MASK) == PATTERN_PRECISION_D)
  {
    *tr = static_cast<PatternBaseDataD*>(d)->transform();
    return ERR_OK;
  }

  tr->reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::Pattern - Create]
// ============================================================================

static err_t FOG_CDECL Pattern_createArgb32(Pattern* self, const ArgbBase32* argb32)
{
  PatternColorData* d = static_cast<PatternColorData*>(self->_d);

  if (d->pType == PATTERN_TYPE_COLOR && d->reference.get() == 1)
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
    d->pType = PATTERN_TYPE_COLOR;
    d->color.initCustom1(*argb32);

    self->_d->release();
    self->_d = d;
    return ERR_OK;
  }
}

static err_t FOG_CDECL Pattern_createColor(Pattern* self, const Color* color)
{
  PatternColorData* d = static_cast<PatternColorData*>(self->_d);

  if (d->pType == PATTERN_TYPE_COLOR && d->reference.get() == 1)
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
    d->pType = PATTERN_TYPE_COLOR;
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
  uint32_t pType = d->pType;

  if (vType == VAR_TYPE_PATTERN && 
      pType == (PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_F) &&
      d->reference.get() == 1)
  {
    d->texture() = *texture;
    d->transform() = tr != NULL ? *tr : TransformF::getIdentityInstance();
    return ERR_OK;
  }
  else
  {
    d = static_cast<PatternTextureDataF*>(
      fog_api.pattern_dCreate(sizeof(PatternTextureDataF)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_PATTERN;
    d->pType = PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_F;
    d->transform.initCustom1(tr != NULL ? *tr : TransformF::getIdentityInstance());
    d->texture.initCustom1(*texture);

    self->_d->release();
    self->_d = d;
    return ERR_OK;
  }
}

static err_t FOG_CDECL Pattern_createTextureD(Pattern* self, const Texture* texture, const TransformD* tr)
{
  PatternTextureDataD* d = static_cast<PatternTextureDataD*>(self->_d);
  uint32_t vType = d->vType & VAR_TYPE_MASK;
  uint32_t pType = d->pType;

  if (vType == VAR_TYPE_PATTERN && 
      pType == (PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_D) &&
      d->reference.get() == 1)
  {
    d->texture() = *texture;
    d->transform() = tr != NULL ? *tr : TransformD::getIdentityInstance();
    return ERR_OK;
  }
  else
  {
    d = static_cast<PatternTextureDataD*>(
      fog_api.pattern_dCreate(sizeof(PatternTextureDataD)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_PATTERN;
    d->pType = PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_D;
    d->transform.initCustom1(tr != NULL ? *tr : TransformD::getIdentityInstance());
    d->texture.initCustom1(*texture);

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
  uint32_t pType = d->pType;

  if (vType == VAR_TYPE_PATTERN && 
      pType == (PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_F) &&
      d->reference.get() == 1)
  {
    d->transform() = tr != NULL ? *tr : TransformF::getIdentityInstance();
    d->gradient().setGradient(*gradient);
    return ERR_OK;
  }
  else
  {
    d = static_cast<PatternGradientDataF*>(
      fog_api.pattern_dCreate(sizeof(PatternGradientDataF)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_PATTERN;
    d->pType = PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_F;
    d->transform.initCustom1(tr != NULL ? *tr : TransformF::getIdentityInstance());
    d->gradient.initCustom1(*gradient);

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
  uint32_t pType = d->pType;

  if (vType == VAR_TYPE_PATTERN && 
      pType == (PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_D) &&
      d->reference.get() == 1)
  {
    d->transform() = tr != NULL ? *tr : TransformD::getIdentityInstance();
    d->gradient().setGradient(*gradient);
    return ERR_OK;
  }
  else
  {
    d = static_cast<PatternGradientDataD*>(
      fog_api.pattern_dCreate(sizeof(PatternGradientDataD)));

    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;

    d->vType = VAR_TYPE_PATTERN;
    d->pType = PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_D;
    d->transform.initCustom1(tr != NULL ? *tr : TransformD::getIdentityInstance());
    d->gradient.initCustom1(*gradient);

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

  if (d->pType & PATTERN_PRECISION_F)
  {
    if (d->reference.get() != 1)
    {
      FOG_RETURN_ON_ERROR(self->detach());
      d = self->_d;
    }
    
    static_cast<PatternBaseDataF*>(d)->transform() = *tr;
    return ERR_OK;
  }
  
  if (d->pType & PATTERN_PRECISION_D)
  {
    if (d->reference.get() != 1)
    {
      FOG_RETURN_ON_ERROR(self->detach());
      d = self->_d;
    }

    static_cast<PatternBaseDataD*>(d)->transform() = *tr;
    return ERR_OK;
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL Pattern_setTransformD(Pattern* self, const TransformD* tr)
{
  PatternData* d = self->_d;

  if (d->pType & PATTERN_PRECISION_F)
  {
    if (d->reference.get() != 1)
    {
      FOG_RETURN_ON_ERROR(self->detach());
      d = self->_d;
    }
    
    static_cast<PatternBaseDataF*>(d)->transform() = *tr;
    return ERR_OK;
  }
  
  if (d->pType & PATTERN_PRECISION_D)
  {
    if (d->reference.get() != 1)
    {
      FOG_RETURN_ON_ERROR(self->detach());
      d = self->_d;
    }

    static_cast<PatternBaseDataD*>(d)->transform() = *tr;
    return ERR_OK;
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL Pattern_applyTransform(Pattern* self, uint32_t transformOp, const void* params)
{
  PatternData* d = self->_d;

  if (d->pType & PATTERN_PRECISION_F)
  {
    if (d->reference.get() != 1)
    {
      FOG_RETURN_ON_ERROR(self->detach());
      d = self->_d;
    }
    
    return static_cast<PatternBaseDataF*>(d)->transform()._transform(transformOp, params);
  }
  
  if (d->pType & PATTERN_PRECISION_D)
  {
    if (d->reference.get() != 1)
    {
      FOG_RETURN_ON_ERROR(self->detach());
      d = self->_d;
    }

    return static_cast<PatternBaseDataD*>(d)->transform()._transform(transformOp, params);
  }

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL Pattern_resetTransform(Pattern* self)
{
  PatternData* d = self->_d;

  if (d->pType & PATTERN_PRECISION_F)
  {
    if (d->reference.get() != 1)
    {
      if (static_cast<PatternBaseDataF*>(d)->transform().isIdentity())
        return ERR_OK;
      FOG_RETURN_ON_ERROR(self->detach());
      d = self->_d;
    }
    
    static_cast<PatternBaseDataF*>(d)->transform().reset();
    return ERR_OK;
  }
  
  if (d->pType & PATTERN_PRECISION_D)
  {
    if (d->reference.get() != 1)
    {
      if (static_cast<PatternBaseDataD*>(d)->transform().isIdentity())
        return ERR_OK;
      FOG_RETURN_ON_ERROR(self->detach());
      d = self->_d;
    }

    static_cast<PatternBaseDataD*>(d)->transform().reset();
    return ERR_OK;
  }

  return ERR_RT_INVALID_STATE;
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

    case VAR_TYPE_PATTERN:
    {
      if (a_d->pType != b_d->pType)
        return false;

      switch (a_d->pType)
      {
        case PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_F:
        {
          const PatternTextureDataF* a_data = static_cast<const PatternTextureDataF*>(a_d);
          const PatternTextureDataF* b_data = static_cast<const PatternTextureDataF*>(b_d);

          return a_data->transform() == b_data->transform() &&
                 a_data->texture() == b_data->texture();
        }

        case PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_D:
        {
          const PatternTextureDataD* a_data = static_cast<const PatternTextureDataD*>(a_d);
          const PatternTextureDataD* b_data = static_cast<const PatternTextureDataD*>(b_d);

          return a_data->transform() == b_data->transform() &&
                 a_data->texture() == b_data->texture();
        }

        case PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_F:
        {
          const PatternGradientDataF* a_data = static_cast<const PatternGradientDataF*>(a_d);
          const PatternGradientDataF* b_data = static_cast<const PatternGradientDataF*>(b_d);

          return a_data->transform() == b_data->transform() &&
                 a_data->gradient() == b_data->gradient();
        }

        case PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_D:
        {
          const PatternGradientDataD* a_data = static_cast<const PatternGradientDataD*>(a_d);
          const PatternGradientDataD* b_data = static_cast<const PatternGradientDataD*>(b_d);

          return a_data->transform() == b_data->transform() &&
                 a_data->gradient() == b_data->gradient();
        }
      }
      break;
    }
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
  switch (d->pType)
  {
    case PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_F:
      static_cast<PatternTextureDataF*>(d)->texture.destroy();
      break;

    case PATTERN_TYPE_TEXTURE | PATTERN_PRECISION_D:
      static_cast<PatternTextureDataD*>(d)->texture.destroy();
      break;

    case PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_F:
      static_cast<PatternGradientDataF*>(d)->gradient.destroy();
      break;

    case PATTERN_TYPE_GRADIENT | PATTERN_PRECISION_D:
      static_cast<PatternGradientDataD*>(d)->gradient.destroy();
      break;
  }

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
