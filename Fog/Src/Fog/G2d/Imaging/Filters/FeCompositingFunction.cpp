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
#include <Fog/G2d/Source/Argb.h>

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
    fog_delete(this);
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
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  virtual FeCompositingFunctionData* addRef() const override;
  virtual void release() override;

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual FeCompositingFunctionData* clone() const override;

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual FeCompositingFunctionPrgb32Func getPrgb32Func() const override;
  virtual FeCompositingFunctionPrgb64Func getPrgb64Func() const override;
};

// ============================================================================
// [Fog::FeCompositingFunctionNull - Construction / Destruction]
// ============================================================================

FeCompositingFunctionNull::FeCompositingFunctionNull() {}
FeCompositingFunctionNull::~FeCompositingFunctionNull() {}

// ============================================================================
// [Fog::FeCompositingFunctionNull - AddRef / Release]
// ============================================================================

FeCompositingFunctionData* FeCompositingFunctionNull::addRef() const
{
  return const_cast<FeCompositingFunctionNull*>(this);
}

void FeCompositingFunctionNull::release()
{
}

// ============================================================================
// [Fog::FeCompositingFunctionNull - Clone]
// ============================================================================

FeCompositingFunctionData* FeCompositingFunctionNull::clone() const
{
  return const_cast<FeCompositingFunctionNull*>(this);
}

// ============================================================================
// [Fog::FeCompositingFunctionNull - Interface]
// ============================================================================

static void FOG_CDECL FeCompositingFunctionNull_prgb32(
  const FeCompositingFunctionData* self, Prgb32* dst, const Prgb32* a, const Prgb32* b, size_t length)
{
  if (dst != a)
    MemOps::copy(dst, a, length * sizeof(Prgb32));
}

static void FOG_CDECL FeCompositingFunctionNull_prgb64(
  const FeCompositingFunctionData* self, Prgb64* dst, const Prgb64* a, const Prgb64* b, size_t length)
{
  if (dst != a)
    MemOps::copy(dst, a, length * sizeof(Prgb64));
}

FeCompositingFunctionPrgb32Func FeCompositingFunctionNull::getPrgb32Func() const
{
  return FeCompositingFunctionNull_prgb32;
}

FeCompositingFunctionPrgb64Func FeCompositingFunctionNull::getPrgb64Func() const
{
  return FeCompositingFunctionNull_prgb64;
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
