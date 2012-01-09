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
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Imaging/Filters/FeConvolveSeparable.h>

namespace Fog {

// ============================================================================
// [Fog::FeConvolveSeparable - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FeConvolveSeparable_ctor(FeConvolveSeparable* self)
{
  self->_feType = FE_TYPE_CONVOLVE_MATRIX;
  self->_extendType = FE_EXTEND_COLOR;
  self->_extendColor.init();
  self->_hVector.init();
  self->_vVector.init();
  self->_hScale = 1.0f;
  self->_vScale = 1.0f;
  self->_hBias = 0.0f;
  self->_vBias = 0.0f;
}

static void FOG_CDECL FeConvolveSeparable_ctorCopy(FeConvolveSeparable* self, const FeConvolveSeparable* other)
{
  self->_feType = FE_TYPE_CONVOLVE_MATRIX;
  self->_extendType = other->_extendType;
  self->_extendColor.init(other->_extendColor);
  self->_hVector.initCustom1(other->_hVector());
  self->_vVector.initCustom1(other->_vVector());
  self->_hScale = other->_hScale;
  self->_vScale = other->_vScale;
  self->_hBias = other->_hBias;
  self->_vBias = other->_vBias;
}

static void FOG_CDECL FeConvolveSeparable_dtor(FeConvolveSeparable* self)
{
  self->_hVector.destroy();
  self->_vVector.destroy();
}

// ============================================================================
// [Fog::FeConvolveSeparable - Reset]
// ============================================================================

static void FOG_CDECL FeConvolveSeparable_reset(FeConvolveSeparable* self)
{
  self->_extendType = FE_EXTEND_COLOR;
  self->_extendColor->reset();
  self->_hVector->reset();
  self->_vVector->reset();
  self->_hScale = 1.0f;
  self->_vScale = 1.0f;
  self->_hBias = 0.0f;
  self->_vBias = 0.0f;
}

// ============================================================================
// [Fog::FeConvolveSeparable - Copy]
// ============================================================================

static err_t FOG_CDECL FeConvolveSeparable_copy(FeConvolveSeparable* self, const FeConvolveSeparable* other)
{
  self->_extendType = FE_EXTEND_COLOR;
  self->_extendColor.init();
  self->_hVector() = other->_hVector();
  self->_vVector() = other->_vVector();
  self->_hScale = other->_hScale;
  self->_vScale = other->_vScale;
  self->_hBias = other->_hBias;
  self->_vBias = other->_vBias;

  return ERR_OK;
}

// ============================================================================
// [Fog::FeConvolveSeparable - Eq]
// ============================================================================

static bool FOG_CDECL FeConvolveSeparable_eq(const FeConvolveSeparable* a, const FeConvolveSeparable* b)
{
  return a->_extendType == b->_extendType &&
         a->_extendColor() == b->_extendColor() &&
         a->_hVector() == b->_hVector() &&
         a->_vVector() == b->_vVector() &&
         a->_hScale == b->_hScale &&
         a->_vScale == b->_vScale &&
         a->_hBias == b->_hBias &&
         a->_vBias == b->_vBias;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FeConvolveSeparable_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.feconvolveseparable_ctor = FeConvolveSeparable_ctor;
  fog_api.feconvolveseparable_ctorCopy = FeConvolveSeparable_ctorCopy;
  fog_api.feconvolveseparable_dtor = FeConvolveSeparable_dtor;

  fog_api.feconvolveseparable_reset = FeConvolveSeparable_reset;
  fog_api.feconvolveseparable_copy = FeConvolveSeparable_copy;
  fog_api.feconvolveseparable_eq = FeConvolveSeparable_eq;
}

} // Fog namespace
