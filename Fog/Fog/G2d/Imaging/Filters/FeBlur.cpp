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
#include <Fog/G2d/Imaging/Filters/FeBlur.h>

namespace Fog {

// ============================================================================
// [Fog::FeBlur - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FeBlur_ctor(FeBlur* self)
{
  self->_filterType = IMAGE_FILTER_TYPE_BLUR;
  self->_extendType = IMAGE_FILTER_EXTEND_COLOR;
  self->_extendColor.init();

  self->_blurType = FE_BLUR_TYPE_DEFAULT;
  self->_hRadius = 0.0f;
  self->_vRadius = 0.0f;
}

static void FOG_CDECL FeBlur_ctorCopy(FeBlur* self, const FeBlur* other)
{
  MemOps::copy_t<FeBlur>(self, other);
}

// ============================================================================
// [Fog::FeBlur - Reset]
// ============================================================================

static void FOG_CDECL FeBlur_reset(FeBlur* self)
{
  self->_extendType = IMAGE_FILTER_EXTEND_COLOR;
  self->_extendColor.init();

  self->_blurType = FE_BLUR_TYPE_DEFAULT;
  self->_hRadius = 0.0f;
  self->_vRadius = 0.0f;
}

// ============================================================================
// [Fog::FeBlur - Copy]
// ============================================================================

static err_t FOG_CDECL FeBlur_copy(FeBlur* self, const FeBlur* other)
{
  MemOps::copy_t<FeBlur>(self, other);
  return ERR_OK;
}

// ============================================================================
// [Fog::FeBlur - Eq]
// ============================================================================

static bool FOG_CDECL FeBlur_eq(const FeBlur* a, const FeBlur* b)
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

FOG_NO_EXPORT void FeBlur_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.feblur_ctor = FeBlur_ctor;
  fog_api.feblur_ctorCopy = FeBlur_ctorCopy;

  fog_api.feblur_reset = FeBlur_reset;
  fog_api.feblur_copy = FeBlur_copy;
  fog_api.feblur_eq = FeBlur_eq;
}

} // Fog namespace
