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
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Imaging/Filters/Blur.h>

namespace Fog {

// ============================================================================
// [Fog::Blur - Construction / Destruction]
// ============================================================================

static void FOG_CDECL Blur_ctor(Blur* self)
{
  self->_filterType = IMAGE_FILTER_TYPE_BLUR;
  self->_extendType = IMAGE_FILTER_EXTEND_COLOR;
  self->_extendColor.init();

  self->_blurType = BLUR_TYPE_DEFAULT;
  self->_hRadius = 0.0f;
  self->_vRadius = 0.0f;
}

static void FOG_CDECL Blur_ctorCopy(Blur* self, const Blur* other)
{
  MemOps::copy_t<Blur>(self, other);
}

// ============================================================================
// [Fog::Blur - Reset]
// ============================================================================

static void FOG_CDECL Blur_reset(Blur* self)
{
  self->_extendType = IMAGE_FILTER_EXTEND_COLOR;
  self->_extendColor.init();

  self->_blurType = BLUR_TYPE_DEFAULT;
  self->_hRadius = 0.0f;
  self->_vRadius = 0.0f;
}

// ============================================================================
// [Fog::Blur - Copy]
// ============================================================================

static err_t FOG_CDECL Blur_copy(Blur* self, const Blur* other)
{
  MemOps::copy_t<Blur>(self, other);
  return ERR_OK;
}

// ============================================================================
// [Fog::Blur - Eq]
// ============================================================================

static bool FOG_CDECL Blur_eq(const Blur* a, const Blur* b)
{
  return a->_extendType == b->_extendType &&
         a->_extendColor() == b->_extendColor() &&
         a->_blurType == b->_blurType &&
         a->_hRadius == b->_hRadius &&
         a->_vRadius == b->_vRadius;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Blur_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.blur_ctor = Blur_ctor;
  _api.blur_ctorCopy = Blur_ctorCopy;

  _api.blur_reset = Blur_reset;
  _api.blur_copy = Blur_copy;
  _api.blur_eq = Blur_eq;
}

} // Fog namespace
