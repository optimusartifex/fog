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
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Imaging/Filters/FeComponentFunction.h>
#include <Fog/G2d/Imaging/Filters/FeComponentTransfer.h>

namespace Fog {

// ============================================================================
// [Fog::FeComponentTransfer - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FeComponentTransfer_ctor(FeComponentTransfer* self)
{
  FeComponentFunctionData* identity = fog_api.fecomponentfunction_oIdentity->_d;
  identity->reference.add(4);

  self->_feType = FE_TYPE_COMPONENT_TRANSFER;
  self->c[0].initCustom1(identity);
  self->c[1].initCustom1(identity);
  self->c[2].initCustom1(identity);
  self->c[3].initCustom1(identity);
}

static void FOG_CDECL FeComponentTransfer_ctorCopy(FeComponentTransfer* self, const FeComponentTransfer* other)
{
  self->_feType = FE_TYPE_COMPONENT_TRANSFER;
  self->c[0].initCustom1(other->c[0]->_d->addRef());
  self->c[1].initCustom1(other->c[1]->_d->addRef());
  self->c[2].initCustom1(other->c[2]->_d->addRef());
  self->c[3].initCustom1(other->c[3]->_d->addRef());
}

static void FOG_CDECL FeComponentTransfer_dtor(FeComponentTransfer* self)
{
  self->c[0]->_d->release();
  self->c[1]->_d->release();
  self->c[2]->_d->release();
  self->c[3]->_d->release();
}

// ============================================================================
// [Fog::FeComponentTransfer - Reset]
// ============================================================================

static void FOG_CDECL FeComponentTransfer_reset(FeComponentTransfer* self)
{
  FeComponentFunctionData* identity = fog_api.fecomponentfunction_oIdentity->_d;
  identity->reference.add(4);

  atomicPtrXchg(&self->c[0]->_d, identity)->release();
  atomicPtrXchg(&self->c[1]->_d, identity)->release();
  atomicPtrXchg(&self->c[2]->_d, identity)->release();
  atomicPtrXchg(&self->c[3]->_d, identity)->release();
}

// ============================================================================
// [Fog::FeComponentTransfer - Copy]
// ============================================================================

static err_t FOG_CDECL FeComponentTransfer_copy(FeComponentTransfer* self, const FeComponentTransfer* other)
{
  atomicPtrXchg(&self->c[0]->_d, other->c[0]->_d->addRef())->release();
  atomicPtrXchg(&self->c[1]->_d, other->c[1]->_d->addRef())->release();
  atomicPtrXchg(&self->c[2]->_d, other->c[2]->_d->addRef())->release();
  atomicPtrXchg(&self->c[3]->_d, other->c[3]->_d->addRef())->release();

  return ERR_OK;
}

// ============================================================================
// [Fog::FeComponentTransfer - Eq]
// ============================================================================

static bool FOG_CDECL FeComponentTransfer_eq(const FeComponentTransfer* a, const FeComponentTransfer* b)
{
  return a->c[0]() == b->c[0]() &&
         a->c[1]() == b->c[1]() &&
         a->c[2]() == b->c[2]() &&
         a->c[3]() == b->c[3]() ;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FeComponentTransfer_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.fecomponenttransfer_ctor = FeComponentTransfer_ctor;
  fog_api.fecomponenttransfer_ctorCopy = FeComponentTransfer_ctorCopy;
  fog_api.fecomponenttransfer_dtor = FeComponentTransfer_dtor;

  fog_api.fecomponenttransfer_reset = FeComponentTransfer_reset;
  fog_api.fecomponenttransfer_copy = FeComponentTransfer_copy;
  fog_api.fecomponenttransfer_eq = FeComponentTransfer_eq;
}

} // Fog namespace
