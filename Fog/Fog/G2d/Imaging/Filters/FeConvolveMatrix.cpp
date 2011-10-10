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
#include <Fog/G2d/Imaging/Filters/FeConvolveMatrix.h>

namespace Fog {

// ============================================================================
// [Fog::FeConvolveMatrix - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FeConvolveMatrix_ctor(FeConvolveMatrix* self)
{
  self->_filterType = IMAGE_FILTER_TYPE_CONVOLVE_MATRIX;
  self->_extendType = IMAGE_FILTER_EXTEND_COLOR;
  self->_extendColor.init();
  self->_matrix.init();
  self->_scale = 1.0f;
  self->_bias = 0.0f;
}

static void FOG_CDECL FeConvolveMatrix_ctorCopy(FeConvolveMatrix* self, const FeConvolveMatrix* other)
{
  self->_filterType = IMAGE_FILTER_TYPE_CONVOLVE_MATRIX;
  self->_extendType = other->_extendType;
  self->_extendColor.init(other->_extendColor);
  self->_matrix.initCustom1(other->_matrix());
}

static void FOG_CDECL FeConvolveMatrix_dtor(FeConvolveMatrix* self)
{
  self->_matrix.destroy();
}

// ============================================================================
// [Fog::FeConvolveMatrix - Reset]
// ============================================================================

static void FOG_CDECL FeConvolveMatrix_reset(FeConvolveMatrix* self)
{
  self->_extendType = IMAGE_FILTER_EXTEND_COLOR;
  self->_extendColor->reset();
  self->_matrix->reset();
  self->_scale = 1.0f;
  self->_bias = 0.0f;
}

// ============================================================================
// [Fog::FeConvolveMatrix - Copy]
// ============================================================================

static err_t FOG_CDECL FeConvolveMatrix_copy(FeConvolveMatrix* self, const FeConvolveMatrix* other)
{
  self->_extendType = IMAGE_FILTER_EXTEND_COLOR;
  self->_extendColor.init();
  self->_matrix() = other->_matrix();
  self->_scale = other->_scale;
  self->_bias = other->_bias;

  return ERR_OK;
}

// ============================================================================
// [Fog::FeConvolveMatrix - Eq]
// ============================================================================

static bool FOG_CDECL FeConvolveMatrix_eq(const FeConvolveMatrix* a, const FeConvolveMatrix* b)
{
  return a->_extendType == b->_extendType &&
         a->_extendColor() == b->_extendColor() &&
         a->_matrix() == b->_matrix() &&
         a->_scale == b->_scale &&
         b->_bias == b->_bias;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FeConvolveMatrix_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.feconvolvematrix_ctor = FeConvolveMatrix_ctor;
  _api.feconvolvematrix_ctorCopy = FeConvolveMatrix_ctorCopy;
  _api.feconvolvematrix_dtor = FeConvolveMatrix_dtor;

  _api.feconvolvematrix_reset = FeConvolveMatrix_reset;
  _api.feconvolvematrix_copy = FeConvolveMatrix_copy;
  _api.feconvolvematrix_eq = FeConvolveMatrix_eq;
}

} // Fog namespace
