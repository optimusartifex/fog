// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Imaging/Filters/FeCompositingFunction.h>

namespace Fog {

// ============================================================================
// [Fog::FeCompositingFunctionData - Construction / Destruction]
// ============================================================================

FeCompositingFunctionData::FeCompositingFunctionData()
{
  reference.init(1);
}

FeCompositingFunctionData::~FeCompositingFunctionData()
{
}

// ============================================================================
// [Fog::FeCompositingFunctionData - AddRef / Release]
// ============================================================================

FeCompositingFunctionData* FeCompositingFunctionData::addRef() const
{
  reference.inc();
  return const_cast<FeCompositingFunctionData*>(this);
}

void FeCompositingFunctionData::release()
{
  if (reference.deref())
    delete this;
}

// ============================================================================
// [Fog::FeCompositingFunctionNull - Declaration]
// ============================================================================

struct FOG_NO_EXPORT FeCompositingFunctionNull : public FeCompositingFunctionData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FeCompositingFunctionNull();
  virtual ~FeCompositingFunctionNull();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual FeCompositingFunctionData* clone() const;

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual FeCompositingFunctionFloatFunc* getFloatFunc() const;
};

// ============================================================================
// [Fog::FeCompositingFunctionNull - Construction / Destruction]
// ============================================================================

FeCompositingFunctionNull::FeCompositingFunctionNull() {}
FeCompositingFunctionNull::~FeCompositingFunctionNull() {}

// ============================================================================
// [Fog::FeCompositingFunctionNull - Clone]
// ============================================================================

FeCompositingFunctionData* FeCompositingFunctionNull::clone() const
{
  return addRef();
}

// ============================================================================
// [Fog::FeCompositingFunctionNull - Interface]
// ============================================================================

FeCompositingFunctionFloatFunc* FeCompositingFunctionNull::getFloatFunc() const
{
  return NULL;
}

static Static<FeCompositingFunctionNull> FeCompositingFunctionNull_oInstance;

// ============================================================================
// [Fog::FeCompositingFunction - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FeCompositingFunction_ctor(FeCompositingFunction* self)
{
  self->_d = FeCompositingFunctionNull_oInstance->addRef();
}

static void FOG_CDECL FeCompositingFunction_ctorCopy(FeCompositingFunction* self, const FeCompositingFunction* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL FeCompositingFunction_dtor(FeCompositingFunction* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::FeCompositingFunction - Reset]
// ============================================================================

static void FOG_CDECL FeCompositingFunction_reset(FeCompositingFunction* self)
{
  atomicPtrXchg(&self->_d, FeCompositingFunctionNull_oInstance->addRef())->release();
}

// ============================================================================
// [Fog::FeCompositingFunction - Copy]
// ============================================================================

static err_t FOG_CDECL FeCompositingFunction_copy(FeCompositingFunction* self, const FeCompositingFunction* other)
{
  FeCompositingFunctionData* other_d = other->_d->addRef();
  if (other_d == NULL)
    return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&self->_d, other_d)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FeCompositingFunction - Eq]
// ============================================================================

static bool FOG_CDECL FeCompositingFunction_eq(const FeCompositingFunction* a, const FeCompositingFunction* b)
{
  return a->_d == b->_d;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FeCompositingFunction_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  FeCompositingFunctionNull_oInstance.init();

  fog_api.fecompositingfunction_ctor = FeCompositingFunction_ctor;
  fog_api.fecompositingfunction_ctorCopy = FeCompositingFunction_ctorCopy;
  fog_api.fecompositingfunction_dtor = FeCompositingFunction_dtor;

  fog_api.fecompositingfunction_reset = FeCompositingFunction_reset;
  fog_api.fecompositingfunction_copy = FeCompositingFunction_copy;
  fog_api.fecompositingfunction_eq = FeCompositingFunction_eq;
}

} // Fog namespace
