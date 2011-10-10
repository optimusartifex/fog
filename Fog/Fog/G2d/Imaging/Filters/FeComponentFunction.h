// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_FECOMPONENTFUNCTION_H
#define _FOG_G2D_IMAGING_FILTERS_FECOMPONENTFUNCTION_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/List.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::FeComponentFunctionLinear]
// ============================================================================

struct FOG_NO_EXPORT FeComponentFunctionLinear
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeComponentFunctionLinear(float slope = 1.0f, float intercept = 0.0f) :
    _slope(slope),
    _intercept(intercept)
  {
  }

  FOG_INLINE FeComponentFunctionLinear(const FeComponentFunctionLinear& other)
  {
    MemOps::copy_t<FeComponentFunctionLinear>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getSlope() const { return _slope; }
  FOG_INLINE float getIntercept() const { return _intercept; }

  FOG_INLINE void setSlope(float slope) { _slope = slope; }
  FOG_INLINE void setIntercept(float intercept) { _intercept = intercept; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _slope = 1.0f;
    _intercept = 0.0f;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeComponentFunctionLinear& other) const
  {
    return MemOps::eq_t<FeComponentFunctionLinear>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FeComponentFunctionLinear& operator=(const FeComponentFunctionLinear& other)
  {
    MemOps::copy_t<FeComponentFunctionLinear>(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeComponentFunctionLinear& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeComponentFunctionLinear& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _slope;
  float _intercept;
};

// ============================================================================
// [Fog::FeComponentFunctionGamma]
// ============================================================================

struct FOG_NO_EXPORT FeComponentFunctionGamma
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeComponentFunctionGamma(float amplitude = 1.0f, float exponent = 1.0f, float offset = 0.0f) :
    _amplitude(amplitude),
    _exponent(exponent),
    _offset(offset),
    _padding(0)
  {
  }

  FOG_INLINE FeComponentFunctionGamma(const FeComponentFunctionGamma& other)
  {
    MemOps::copy_t<FeComponentFunctionGamma>(this, &other);
  }
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getAmplitude() const { return _amplitude; }
  FOG_INLINE float getExponent() const { return _exponent; }
  FOG_INLINE float getOffset() const { return _offset; }

  FOG_INLINE void setAmplitude(float amplitude) { _amplitude = amplitude; }
  FOG_INLINE void setExponent(float exponent) { _exponent = exponent; }
  FOG_INLINE void setOffset(float offset) { _offset = offset; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _amplitude = 1.0f;
    _exponent = 1.0f;
    _offset = 0.0f;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeComponentFunctionGamma& other) const
  {
    return MemOps::eq_t<FeComponentFunctionGamma>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FeComponentFunctionGamma& operator=(const FeComponentFunctionGamma& other)
  {
    MemOps::copy_t<FeComponentFunctionGamma>(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeComponentFunctionGamma& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeComponentFunctionGamma& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _amplitude;
  float _exponent;
  float _offset;
  uint32_t _padding;
};

// ============================================================================
// [Fog::FeComponentFunctionData]
// ============================================================================

struct FOG_NO_EXPORT FeComponentFunctionData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE FeComponentFunctionData* addRef() const
  {
    reference.inc();
    return const_cast<FeComponentFunctionData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      _api.fecomponentfunction_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
  
  //! @brief Reference.
  mutable Atomic<size_t> reference;

  uint32_t vType;
  uint32_t functionType;

  union
  {
    Static< List<float> > table;
    Static< FeComponentFunctionLinear > linear;
    Static< FeComponentFunctionGamma > gamma;
  };
};

// ============================================================================
// [Fog::FeComponentFunction]
// ============================================================================

struct FOG_NO_EXPORT FeComponentFunction
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeComponentFunction()
  {
    _api.fecomponentfunction_ctor(this);
  }

  FOG_INLINE FeComponentFunction(const FeComponentFunction& other)
  {
    _api.fecomponentfunction_ctorCopy(this, &other);
  }

  explicit FOG_INLINE FeComponentFunction(FeComponentFunctionData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~FeComponentFunction()
  {
    _api.fecomponentfunction_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getFunctionType() const { return _d->functionType; }
  FOG_INLINE bool resultsInIdentity() const { return _api.fecomponentfunction_resultsInIdentity(this); }

  FOG_INLINE err_t _getData(uint32_t functionType, void* functionData) const
  {
    return _api.fecomponentfunction_getData(this, functionType, functionData);
  }

  FOG_INLINE err_t getTable(List<float>& dst) const
  {
    return _getData(FE_COMPONENT_FUNCTION_TABLE, &dst);
  }

  FOG_INLINE err_t getDiscrete(List<float>& dst) const
  {
    return _getData(FE_COMPONENT_FUNCTION_DISCRETE, &dst);
  }

  FOG_INLINE err_t getLinear(FeComponentFunctionLinear& dst) const
  {
    return _getData(FE_COMPONENT_FUNCTION_LINEAR, &dst);
  }

  FOG_INLINE err_t getGamma(FeComponentFunctionGamma& dst) const
  {
    return _getData(FE_COMPONENT_FUNCTION_GAMMA, &dst);
  }

  FOG_INLINE err_t setIdentity()
  {
    _api.fecomponentfunction_reset(this);
    return ERR_OK;
  }

  FOG_INLINE err_t _setData(uint32_t functionType, const void* functionData)
  {
    return _api.fecomponentfunction_setData(this, functionType, functionData);
  }

  FOG_INLINE err_t setTable(const List<float>& dst)
  {
    return _setData(FE_COMPONENT_FUNCTION_TABLE, &dst);
  }

  FOG_INLINE err_t setDiscrete(const List<float>& dst)
  {
    return _setData(FE_COMPONENT_FUNCTION_DISCRETE, &dst);
  }

  FOG_INLINE err_t setLinear(const FeComponentFunctionLinear& dst)
  {
    return _setData(FE_COMPONENT_FUNCTION_LINEAR, &dst);
  }

  FOG_INLINE err_t setGamma(const FeComponentFunctionGamma& dst)
  {
    return _setData(FE_COMPONENT_FUNCTION_GAMMA, &dst);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _api.fecomponentfunction_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeComponentFunction& other) const
  {
    return _api.fecomponentfunction_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE FeComponentFunction& operator=(const FeComponentFunction& other)
  {
    _api.fecomponentfunction_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeComponentFunction& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeComponentFunction& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const FeComponentFunction& identity()
  {
    return *_api.fecomponentfunction_oIdentity;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FeComponentFunction* a, const FeComponentFunction* b)
  {
    return _api.fecomponentfunction_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.fecomponentfunction_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(FeComponentFunctionData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_FECOMPONENTFUNCTION_H
