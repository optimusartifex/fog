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
#include <Fog/G2d/Imaging/Filters/ComponentTransfer.h>
#include <Fog/G2d/Imaging/Filters/ComponentTransferFunction.h>

namespace Fog {

// ============================================================================
// [Fog::ComponentTransfer - Construction / Destruction]
// ============================================================================

static void FOG_CDECL ComponentTransfer_ctor(ComponentTransfer* self)
{
  ComponentTransferFunctionData* identity = _api.componenttransferfunction_oIdentity->_d;
  identity->reference.add(4);

  self->_filterType = IMAGE_FILTER_TYPE_COMPONENT_TRANSFER;
  self->c[0].initCustom1(identity);
  self->c[1].initCustom1(identity);
  self->c[2].initCustom1(identity);
  self->c[3].initCustom1(identity);
}

static void FOG_CDECL ComponentTransfer_ctorCopy(ComponentTransfer* self, const ComponentTransfer* other)
{
  self->_filterType = IMAGE_FILTER_TYPE_COMPONENT_TRANSFER;
  self->c[0].initCustom1(other->c[0]->_d->addRef());
  self->c[1].initCustom1(other->c[1]->_d->addRef());
  self->c[2].initCustom1(other->c[2]->_d->addRef());
  self->c[3].initCustom1(other->c[3]->_d->addRef());
}

static void FOG_CDECL ComponentTransfer_dtor(ComponentTransfer* self)
{
  self->c[0]->_d->release();
  self->c[1]->_d->release();
  self->c[2]->_d->release();
  self->c[3]->_d->release();
}

// ============================================================================
// [Fog::ComponentTransfer - Reset]
// ============================================================================

static void FOG_CDECL ComponentTransfer_reset(ComponentTransfer* self)
{
  ComponentTransferFunctionData* identity = _api.componenttransferfunction_oIdentity->_d;
  identity->reference.add(4);

  atomicPtrXchg(&self->c[0]->_d, identity)->release();
  atomicPtrXchg(&self->c[1]->_d, identity)->release();
  atomicPtrXchg(&self->c[2]->_d, identity)->release();
  atomicPtrXchg(&self->c[3]->_d, identity)->release();
}

// ============================================================================
// [Fog::ComponentTransfer - Copy]
// ============================================================================

static err_t FOG_CDECL ComponentTransfer_copy(ComponentTransfer* self, const ComponentTransfer* other)
{
  atomicPtrXchg(&self->c[0]->_d, other->c[0]->_d->addRef())->release();
  atomicPtrXchg(&self->c[1]->_d, other->c[1]->_d->addRef())->release();
  atomicPtrXchg(&self->c[2]->_d, other->c[2]->_d->addRef())->release();
  atomicPtrXchg(&self->c[3]->_d, other->c[3]->_d->addRef())->release();

  return ERR_OK;
}

// ============================================================================
// [Fog::ComponentTransfer - Eq]
// ============================================================================

static bool FOG_CDECL ComponentTransfer_eq(const ComponentTransfer* a, const ComponentTransfer* b)
{
  return a->c[0]() == b->c[0]() &&
         a->c[1]() == b->c[1]() &&
         a->c[2]() == b->c[2]() &&
         a->c[3]() == b->c[3]() ;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ComponentTransfer_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.componenttransfer_ctor = ComponentTransfer_ctor;
  _api.componenttransfer_ctorCopy = ComponentTransfer_ctorCopy;
  _api.componenttransfer_dtor = ComponentTransfer_dtor;

  _api.componenttransfer_reset = ComponentTransfer_reset;
  _api.componenttransfer_copy = ComponentTransfer_copy;
  _api.componenttransfer_eq = ComponentTransfer_eq;
}

} // Fog namespace
