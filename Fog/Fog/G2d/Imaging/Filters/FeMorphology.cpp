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
#include <Fog/G2d/Imaging/Filters/FeMorphology.h>

namespace Fog {

// ============================================================================
// [Fog::FeMorphology - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FeMorphology_ctor(FeMorphology* self)
{
  self->_filterType = IMAGE_FILTER_TYPE_BLUR;
  self->_extendType = IMAGE_FILTER_EXTEND_COLOR;
  self->_extendColor.init();

  self->_morphologyType = FE_MORPHOLOGY_TYPE_DEFAULT;
  self->_hRadius = 0.0f;
  self->_vRadius = 0.0f;
}

static void FOG_CDECL FeMorphology_ctorCopy(FeMorphology* self, const FeMorphology* other)
{
  MemOps::copy_t<FeMorphology>(self, other);
}

// ============================================================================
// [Fog::FeMorphology - Reset]
// ============================================================================

static void FOG_CDECL FeMorphology_reset(FeMorphology* self)
{
  self->_extendType = IMAGE_FILTER_EXTEND_COLOR;
  self->_extendColor.init();

  self->_morphologyType = FE_MORPHOLOGY_TYPE_DEFAULT;
  self->_hRadius = 0.0f;
  self->_vRadius = 0.0f;
}

// ============================================================================
// [Fog::FeMorphology - Copy]
// ============================================================================

static err_t FOG_CDECL FeMorphology_copy(FeMorphology* self, const FeMorphology* other)
{
  MemOps::copy_t<FeMorphology>(self, other);
  return ERR_OK;
}

// ============================================================================
// [Fog::FeMorphology - Eq]
// ============================================================================

static bool FOG_CDECL FeMorphology_eq(const FeMorphology* a, const FeMorphology* b)
{
  return a->_extendType == b->_extendType &&
         a->_extendColor() == b->_extendColor() &&
         a->_morphologyType == b->_morphologyType &&
         a->_hRadius == b->_hRadius &&
         a->_vRadius == b->_vRadius;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FeMorphology_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.femorphology_ctor = FeMorphology_ctor;
  _api.femorphology_ctorCopy = FeMorphology_ctorCopy;

  _api.femorphology_reset = FeMorphology_reset;
  _api.femorphology_copy = FeMorphology_copy;
  _api.femorphology_eq = FeMorphology_eq;
}

} // Fog namespace
