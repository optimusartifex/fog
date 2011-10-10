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

//! @brief Component transfer.
struct FOG_NO_EXPORT FeConvolveMatrix : public FeBorder
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeConvolveMatrix()
  {
    _api.feconvolvematrix_ctor(this);
  }

  FOG_INLINE FeConvolveMatrix(const FeConvolveMatrix& other)
  {
    _api.feconvolvematrix_ctorCopy(this, &other);
  }

  FOG_INLINE ~FeConvolveMatrix()
  {
    _api.feconvolvematrix_dtor(this);
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
    _api.feconvolvematrix_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeConvolveMatrix& other) const
  {
    return _api.feconvolvematrix_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE FeConvolveMatrix& operator=(const FeConvolveMatrix& other)
  {
    _api.feconvolvematrix_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeConvolveMatrix& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeConvolveMatrix& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FeConvolveMatrix* a, const FeConvolveMatrix* b)
  {
    return _api.feconvolvematrix_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.feconvolvematrix_eq;
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
