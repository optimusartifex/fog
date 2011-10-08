// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_CONVOLUTIONMATRIX_H
#define _FOG_G2D_IMAGING_FILTERS_CONVOLUTIONMATRIX_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Imaging/BorderFilterTag.h>
#include <Fog/G2d/Tools/Matrix.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ConvolutionMatrix]
// ============================================================================

//! @brief Component transfer.
struct FOG_NO_EXPORT ConvolutionMatrix : public BorderFilterTag
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ConvolutionMatrix()
  {
    _api.convolutionmatrix_ctor(this);
  }

  FOG_INLINE ConvolutionMatrix(const ConvolutionMatrix& other)
  {
    _api.convolutionmatrix_ctorCopy(this, &other);
  }

  FOG_INLINE ~ConvolutionMatrix()
  {
    _api.convolutionmatrix_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getScale() const { return _scale; }
  FOG_INLINE float getBias() const { return _bias; }
  FOG_INLINE const MatrixF& getMatrix() const { return _matrix(); }

  FOG_INLINE void setScale(float scale) { _scale = scale; }
  FOG_INLINE void setBias(float bias) { _bias = bias; }
  FOG_INLINE void setMatrix(const MatrixF& matrix) { _matrix() = matrix; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _api.convolutionmatrix_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const ConvolutionMatrix& other) const
  {
    return _api.convolutionmatrix_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE ConvolutionMatrix& operator=(const ConvolutionMatrix& other)
  {
    _api.convolutionmatrix_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const ConvolutionMatrix& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ConvolutionMatrix& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const ConvolutionMatrix* a, const ConvolutionMatrix* b)
  {
    return _api.convolutionmatrix_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.convolutionmatrix_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<MatrixF> _matrix;
  float _scale;
  float _bias;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_CONVOLUTIONMATRIX_H
