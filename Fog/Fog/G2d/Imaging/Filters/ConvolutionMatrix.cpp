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
#include <Fog/G2d/Imaging/Filters/ConvolutionMatrix.h>

namespace Fog {

// ============================================================================
// [Fog::ConvolutionMatrix - Construction / Destruction]
// ============================================================================

static void FOG_CDECL ConvolutionMatrix_ctor(ConvolutionMatrix* self)
{
  self->_filterType = IMAGE_FILTER_TYPE_CONVOLUTION_MATRIX;
  self->_extendType = IMAGE_FILTER_EXTEND_COLOR;
  self->_extendColor.init();
  self->_matrix.init();
  self->_scale = 1.0f;
  self->_bias = 0.0f;
}

static void FOG_CDECL ConvolutionMatrix_ctorCopy(ConvolutionMatrix* self, const ConvolutionMatrix* other)
{
  self->_filterType = IMAGE_FILTER_TYPE_CONVOLUTION_MATRIX;
  self->_extendType = other->_extendType;
  self->_extendColor.init(other->_extendColor);
  self->_matrix.initCustom1(other->_matrix());
}

static void FOG_CDECL ConvolutionMatrix_dtor(ConvolutionMatrix* self)
{
  self->_matrix.destroy();
}

// ============================================================================
// [Fog::ConvolutionMatrix - Reset]
// ============================================================================

static void FOG_CDECL ConvolutionMatrix_reset(ConvolutionMatrix* self)
{
  self->_extendType = IMAGE_FILTER_EXTEND_COLOR;
  self->_extendColor->reset();
  self->_matrix->reset();
  self->_scale = 1.0f;
  self->_bias = 0.0f;
}

// ============================================================================
// [Fog::ConvolutionMatrix - Copy]
// ============================================================================

static err_t FOG_CDECL ConvolutionMatrix_copy(ConvolutionMatrix* self, const ConvolutionMatrix* other)
{
  self->_extendType = IMAGE_FILTER_EXTEND_COLOR;
  self->_extendColor.init();
  self->_matrix() = other->_matrix();
  self->_scale = other->_scale;
  self->_bias = other->_bias;

  return ERR_OK;
}

// ============================================================================
// [Fog::ConvolutionMatrix - Eq]
// ============================================================================

static bool FOG_CDECL ConvolutionMatrix_eq(const ConvolutionMatrix* a, const ConvolutionMatrix* b)
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

FOG_NO_EXPORT void ConvolutionMatrix_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.convolutionmatrix_ctor = ConvolutionMatrix_ctor;
  _api.convolutionmatrix_ctorCopy = ConvolutionMatrix_ctorCopy;
  _api.convolutionmatrix_dtor = ConvolutionMatrix_dtor;

  _api.convolutionmatrix_reset = ConvolutionMatrix_reset;
  _api.convolutionmatrix_copy = ConvolutionMatrix_copy;
  _api.convolutionmatrix_eq = ConvolutionMatrix_eq;
}

} // Fog namespace
