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
#include <Fog/G2d/Imaging/Filters/FeTurbulence.h>

namespace Fog {

// ============================================================================
// [Fog::FeTurbulence - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FeTurbulence_ctor(FeTurbulence* self)
{
  self->_feType = FE_TYPE_TURBULENCE;

  self->_turbulenceType = FE_TURBULENCE_TYPE_DEFAULT;
  self->_numOctaves = 1;
  self->_stitchTitles = 0;
  self->_seed = 0;
  self->_hBaseFrequency = 0.0f;
  self->_vBaseFrequency = 0.0f;
}

static void FOG_CDECL FeTurbulence_ctorCopy(FeTurbulence* self, const FeTurbulence* other)
{
  MemOps::copy_t<FeTurbulence>(self, other);
}

// ============================================================================
// [Fog::FeTurbulence - Reset]
// ============================================================================

static void FOG_CDECL FeTurbulence_reset(FeTurbulence* self)
{
  self->_turbulenceType = FE_TURBULENCE_TYPE_DEFAULT;
  self->_numOctaves = 1;
  self->_stitchTitles = 0;
  self->_seed = 0;
  self->_hBaseFrequency = 0.0f;
  self->_vBaseFrequency = 0.0f;
}

// ============================================================================
// [Fog::FeTurbulence - Copy]
// ============================================================================

static err_t FOG_CDECL FeTurbulence_copy(FeTurbulence* self, const FeTurbulence* other)
{
  MemOps::copy_t<FeTurbulence>(self, other);
  return ERR_OK;
}

// ============================================================================
// [Fog::FeTurbulence - Eq]
// ============================================================================

static bool FOG_CDECL FeTurbulence_eq(const FeTurbulence* a, const FeTurbulence* b)
{
  return a->_turbulenceType == b->_turbulenceType &&
         a->_numOctaves == b->_numOctaves &&
         a->_stitchTitles == b->_stitchTitles &&
         a->_seed == b->_seed &&
         a->_hBaseFrequency == b->_hBaseFrequency &&
         a->_vBaseFrequency == b->_vBaseFrequency;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FeTurbulence_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.feturbulence_ctor = FeTurbulence_ctor;
  fog_api.feturbulence_ctorCopy = FeTurbulence_ctorCopy;

  fog_api.feturbulence_reset = FeTurbulence_reset;
  fog_api.feturbulence_copy = FeTurbulence_copy;
  fog_api.feturbulence_eq = FeTurbulence_eq;
}

} // Fog namespace
