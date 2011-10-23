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
#include <Fog/G2d/Imaging/Filters/FeColorLut.h>
#include <Fog/G2d/Imaging/Filters/FeColorLutArray.h>

namespace Fog {

// ============================================================================
// [Fog::FeColorLut - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FeColorLut_ctor(FeColorLut* self)
{
  FeColorLutArrayData* identity = fog_api.fecolorlutarray_oIdentity->_d;
  identity->reference.add(4);

  self->_feType = FE_TYPE_COLOR_LUT;
  self->c[0].initCustom1(identity);
  self->c[1].initCustom1(identity);
  self->c[2].initCustom1(identity);
  self->c[3].initCustom1(identity);
}

static void FOG_CDECL FeColorLut_ctorCopy(FeColorLut* self, const FeColorLut* other)
{
  self->_feType = FE_TYPE_COLOR_LUT;
  self->c[0].initCustom1(other->c[0]->_d->addRef());
  self->c[1].initCustom1(other->c[1]->_d->addRef());
  self->c[2].initCustom1(other->c[2]->_d->addRef());
  self->c[3].initCustom1(other->c[3]->_d->addRef());
}

static void FOG_CDECL FeColorLut_dtor(FeColorLut* self)
{
  self->c[0]->_d->release();
  self->c[1]->_d->release();
  self->c[2]->_d->release();
  self->c[3]->_d->release();
}

// ============================================================================
// [Fog::FeColorLut - Reset]
// ============================================================================

static void FOG_CDECL FeColorLut_reset(FeColorLut* self)
{
  FeColorLutArrayData* identity = fog_api.fecolorlutarray_oIdentity->_d;
  identity->reference.add(4);

  atomicPtrXchg(&self->c[0]->_d, identity)->release();
  atomicPtrXchg(&self->c[1]->_d, identity)->release();
  atomicPtrXchg(&self->c[2]->_d, identity)->release();
  atomicPtrXchg(&self->c[3]->_d, identity)->release();
}

// ============================================================================
// [Fog::FeColorLut - Copy]
// ============================================================================

static err_t FOG_CDECL FeColorLut_copy(FeColorLut* self, const FeColorLut* other)
{
  atomicPtrXchg(&self->c[0]->_d, other->c[0]->_d->addRef())->release();
  atomicPtrXchg(&self->c[1]->_d, other->c[1]->_d->addRef())->release();
  atomicPtrXchg(&self->c[2]->_d, other->c[2]->_d->addRef())->release();
  atomicPtrXchg(&self->c[3]->_d, other->c[3]->_d->addRef())->release();

  return ERR_OK;
}

// ============================================================================
// [Fog::FeColorLut - Eq]
// ============================================================================

static bool FOG_CDECL FeColorLut_eq(const FeColorLut* a, const FeColorLut* b)
{
  return a->c[0]() == b->c[0]() &&
         a->c[1]() == b->c[1]() &&
         a->c[2]() == b->c[2]() &&
         a->c[3]() == b->c[3]() ;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FeColorLut_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.fecolorlut_ctor = FeColorLut_ctor;
  fog_api.fecolorlut_ctorCopy = FeColorLut_ctorCopy;
  fog_api.fecolorlut_dtor = FeColorLut_dtor;

  fog_api.fecolorlut_reset = FeColorLut_reset;
  fog_api.fecolorlut_copy = FeColorLut_copy;
  fog_api.fecolorlut_eq = FeColorLut_eq;
}

} // Fog namespace
