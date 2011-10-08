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
#include <Fog/G2d/Imaging/Filters/ColorLut.h>
#include <Fog/G2d/Imaging/Filters/ColorLutArray.h>

namespace Fog {

// ============================================================================
// [Fog::ColorLut - Construction / Destruction]
// ============================================================================

static void FOG_CDECL ColorLut_ctor(ColorLut* self)
{
  ColorLutArrayData* identity = _api.colorlutarray_oIdentity->_d;
  identity->reference.add(4);

  self->_filterType = IMAGE_FILTER_TYPE_COLOR_LUT;
  self->c[0].initCustom1(identity);
  self->c[1].initCustom1(identity);
  self->c[2].initCustom1(identity);
  self->c[3].initCustom1(identity);
}

static void FOG_CDECL ColorLut_ctorCopy(ColorLut* self, const ColorLut* other)
{
  self->_filterType = IMAGE_FILTER_TYPE_COLOR_LUT;
  self->c[0].initCustom1(other->c[0]->_d->addRef());
  self->c[1].initCustom1(other->c[1]->_d->addRef());
  self->c[2].initCustom1(other->c[2]->_d->addRef());
  self->c[3].initCustom1(other->c[3]->_d->addRef());
}

static void FOG_CDECL ColorLut_dtor(ColorLut* self)
{
  self->c[0]->_d->release();
  self->c[1]->_d->release();
  self->c[2]->_d->release();
  self->c[3]->_d->release();
}

// ============================================================================
// [Fog::ColorLut - Reset]
// ============================================================================

static void FOG_CDECL ColorLut_reset(ColorLut* self)
{
  ColorLutArrayData* identity = _api.colorlutarray_oIdentity->_d;
  identity->reference.add(4);

  atomicPtrXchg(&self->c[0]->_d, identity)->release();
  atomicPtrXchg(&self->c[1]->_d, identity)->release();
  atomicPtrXchg(&self->c[2]->_d, identity)->release();
  atomicPtrXchg(&self->c[3]->_d, identity)->release();
}

// ============================================================================
// [Fog::ColorLut - Copy]
// ============================================================================

static err_t FOG_CDECL ColorLut_copy(ColorLut* self, const ColorLut* other)
{
  atomicPtrXchg(&self->c[0]->_d, other->c[0]->_d->addRef())->release();
  atomicPtrXchg(&self->c[1]->_d, other->c[1]->_d->addRef())->release();
  atomicPtrXchg(&self->c[2]->_d, other->c[2]->_d->addRef())->release();
  atomicPtrXchg(&self->c[3]->_d, other->c[3]->_d->addRef())->release();

  return ERR_OK;
}

// ============================================================================
// [Fog::ColorLut - Eq]
// ============================================================================

static bool FOG_CDECL ColorLut_eq(const ColorLut* a, const ColorLut* b)
{
  return a->c[0]() == b->c[0]() &&
         a->c[1]() == b->c[1]() &&
         a->c[2]() == b->c[2]() &&
         a->c[3]() == b->c[3]() ;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ColorLut_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.colorlut_ctor = ColorLut_ctor;
  _api.colorlut_ctorCopy = ColorLut_ctorCopy;
  _api.colorlut_dtor = ColorLut_dtor;

  _api.colorlut_reset = ColorLut_reset;
  _api.colorlut_copy = ColorLut_copy;
  _api.colorlut_eq = ColorLut_eq;
}

} // Fog namespace
