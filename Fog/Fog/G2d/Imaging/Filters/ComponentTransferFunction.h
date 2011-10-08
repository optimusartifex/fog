// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_COMPONENTTRANSFERFUNCTION_H
#define _FOG_G2D_IMAGING_FILTERS_COMPONENTTRANSFERFUNCTION_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/List.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ComponentTransferFunctionLinear]
// ============================================================================

struct FOG_NO_EXPORT ComponentTransferFunctionLinear
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ComponentTransferFunctionLinear(float slope = 1.0f, float intercept = 0.0f) :
    _slope(slope),
    _intercept(intercept)
  {
  }

  FOG_INLINE ComponentTransferFunctionLinear(const ComponentTransferFunctionLinear& other)
  {
    MemOps::copy_t<ComponentTransferFunctionLinear>(this, &other);
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

  FOG_INLINE bool eq(const ComponentTransferFunctionLinear& other) const
  {
    return MemOps::eq_t<ComponentTransferFunctionLinear>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ComponentTransferFunctionLinear& operator=(const ComponentTransferFunctionLinear& other)
  {
    MemOps::copy_t<ComponentTransferFunctionLinear>(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const ComponentTransferFunctionLinear& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ComponentTransferFunctionLinear& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _slope;
  float _intercept;
};

// ============================================================================
// [Fog::ComponentTransferFunctionGamma]
// ============================================================================

struct FOG_NO_EXPORT ComponentTransferFunctionGamma
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ComponentTransferFunctionGamma(float amplitude = 1.0f, float exponent = 1.0f, float offset = 0.0f) :
    _amplitude(amplitude),
    _exponent(exponent),
    _offset(offset),
    _padding(0)
  {
  }

  FOG_INLINE ComponentTransferFunctionGamma(const ComponentTransferFunctionGamma& other)
  {
    MemOps::copy_t<ComponentTransferFunctionGamma>(this, &other);
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

  FOG_INLINE bool eq(const ComponentTransferFunctionGamma& other) const
  {
    return MemOps::eq_t<ComponentTransferFunctionGamma>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ComponentTransferFunctionGamma& operator=(const ComponentTransferFunctionGamma& other)
  {
    MemOps::copy_t<ComponentTransferFunctionGamma>(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const ComponentTransferFunctionGamma& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ComponentTransferFunctionGamma& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _amplitude;
  float _exponent;
  float _offset;
  uint32_t _padding;
};

// ============================================================================
// [Fog::ComponentTransferFunctionData]
// ============================================================================

struct FOG_NO_EXPORT ComponentTransferFunctionData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE ComponentTransferFunctionData* addRef() const
  {
    reference.inc();
    return const_cast<ComponentTransferFunctionData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      _api.componenttransferfunction_dFree(this);
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
    Static< ComponentTransferFunctionLinear > linear;
    Static< ComponentTransferFunctionGamma > gamma;
  };
};

// ============================================================================
// [Fog::ComponentTransferFunction]
// ============================================================================

struct FOG_NO_EXPORT ComponentTransferFunction
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ComponentTransferFunction()
  {
    _api.componenttransferfunction_ctor(this);
  }

  FOG_INLINE ComponentTransferFunction(const ComponentTransferFunction& other)
  {
    _api.componenttransferfunction_ctorCopy(this, &other);
  }

  explicit FOG_INLINE ComponentTransferFunction(ComponentTransferFunctionData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~ComponentTransferFunction()
  {
    _api.componenttransferfunction_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getFunctionType() const { return _d->functionType; }
  FOG_INLINE bool resultsInIdentity() const { return _api.componenttransferfunction_resultsInIdentity(this); }

  FOG_INLINE err_t _getData(uint32_t functionType, void* functionData) const
  {
    return _api.componenttransferfunction_getData(this, functionType, functionData);
  }

  FOG_INLINE err_t getTable(List<float>& dst) const
  {
    return _getData(COMPONENT_TRANSFER_FUNCTION_TABLE, &dst);
  }

  FOG_INLINE err_t getDiscrete(List<float>& dst) const
  {
    return _getData(COMPONENT_TRANSFER_FUNCTION_DISCRETE, &dst);
  }

  FOG_INLINE err_t getLinear(ComponentTransferFunctionLinear& dst) const
  {
    return _getData(COMPONENT_TRANSFER_FUNCTION_LINEAR, &dst);
  }

  FOG_INLINE err_t getGamma(ComponentTransferFunctionGamma& dst) const
  {
    return _getData(COMPONENT_TRANSFER_FUNCTION_GAMMA, &dst);
  }

  FOG_INLINE err_t setIdentity()
  {
    _api.componenttransferfunction_reset(this);
    return ERR_OK;
  }

  FOG_INLINE err_t _setData(uint32_t functionType, const void* functionData)
  {
    return _api.componenttransferfunction_setData(this, functionType, functionData);
  }

  FOG_INLINE err_t setTable(const List<float>& dst)
  {
    return _setData(COMPONENT_TRANSFER_FUNCTION_TABLE, &dst);
  }

  FOG_INLINE err_t setDiscrete(const List<float>& dst)
  {
    return _setData(COMPONENT_TRANSFER_FUNCTION_DISCRETE, &dst);
  }

  FOG_INLINE err_t setLinear(const ComponentTransferFunctionLinear& dst)
  {
    return _setData(COMPONENT_TRANSFER_FUNCTION_LINEAR, &dst);
  }

  FOG_INLINE err_t setGamma(const ComponentTransferFunctionGamma& dst)
  {
    return _setData(COMPONENT_TRANSFER_FUNCTION_GAMMA, &dst);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _api.componenttransferfunction_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const ComponentTransferFunction& other) const
  {
    return _api.componenttransferfunction_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE ComponentTransferFunction& operator=(const ComponentTransferFunction& other)
  {
    _api.componenttransferfunction_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const ComponentTransferFunction& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ComponentTransferFunction& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const ComponentTransferFunction& identity()
  {
    return *_api.componenttransferfunction_oIdentity;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const ComponentTransferFunction* a, const ComponentTransferFunction* b)
  {
    return _api.componenttransferfunction_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.componenttransferfunction_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ComponentTransferFunctionData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_COMPONENTTRANSFERFUNCTION_H
