// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_FECONVOLVEMATRIX_H
#define _FOG_G2D_IMAGING_FILTERS_FECONVOLVEMATRIX_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Imaging/Filters/FeBorder.h>
#include <Fog/G2d/Tools/Matrix.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::FeConvolveMatrix]
// ============================================================================

//! @brief Convolve matrix.
struct FOG_NO_EXPORT FeConvolveMatrix : public FeBorder
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeConvolveMatrix()
  {
    fog_api.feconvolvematrix_ctor(this);
  }

  FOG_INLINE FeConvolveMatrix(const FeConvolveMatrix& other)
  {
    fog_api.feconvolvematrix_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE FeConvolveMatrix(FeConvolveMatrix&& other)
  {
    _matrix->_d = other._matrix->_d;
    other._matrix->_d = NULL;

    _scale = other._scale;
    _bias = other._bias;
  }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE ~FeConvolveMatrix()
  {
    fog_api.feconvolvematrix_dtor(this);
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
    fog_api.feconvolvematrix_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeConvolveMatrix& other) const
  {
    return fog_api.feconvolvematrix_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE FeConvolveMatrix& operator=(const FeConvolveMatrix& other)
  {
    fog_api.feconvolvematrix_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeConvolveMatrix& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeConvolveMatrix& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FeConvolveMatrix* a, const FeConvolveMatrix* b)
  {
    return fog_api.feconvolvematrix_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.feconvolvematrix_eq;
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
#endif // _FOG_G2D_IMAGING_FILTERS_FECONVOLVEMATRIX_H
