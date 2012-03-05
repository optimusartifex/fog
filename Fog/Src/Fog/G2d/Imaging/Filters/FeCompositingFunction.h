// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_FECOMPOSITINGFUNCTION_H
#define _FOG_G2D_IMAGING_FILTERS_FECOMPOSITINGFUNCTION_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/CoreObj.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::FeCompositingFunctionData]
// ============================================================================

struct FOG_API FeCompositingFunctionData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FeCompositingFunctionData();
  virtual ~FeCompositingFunctionData();

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  virtual FeCompositingFunctionData* addRef() const;
  virtual void release();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual FeCompositingFunctionData* clone() const = 0;

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual FeCompositingFunctionPrgb32Func getPrgb32Func() const = 0;
  virtual FeCompositingFunctionPrgb64Func getPrgb64Func() const = 0;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t vType;
  uint32_t fType;

  mutable Atomic<size_t> reference;
  
private:
  FOG_NO_COPY(FeCompositingFunctionData)
};

// ============================================================================
// [Fog::FeCompositingFunction]
// ============================================================================

struct FOG_NO_EXPORT FeCompositingFunction
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeCompositingFunction()
  {
    fog_api.fecompositingfunction_ctor(this);
  }

  FOG_INLINE FeCompositingFunction(const FeCompositingFunction& other)
  {
    fog_api.fecompositingfunction_ctorCopy(this, &other);
  }

  explicit FOG_INLINE FeCompositingFunction(FeCompositingFunctionData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~FeCompositingFunction()
  {
    fog_api.fecompositingfunction_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE uint32_t getFunctionType() const { return _d->fType; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.fecompositingfunction_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeCompositingFunction& other) const
  {
    return fog_api.fecompositingfunction_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FeCompositingFunction& operator=(const FeCompositingFunction& other)
  {
    fog_api.fecompositingfunction_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeCompositingFunction& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeCompositingFunction& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FeCompositingFunctionData* _d;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_FECOMPOSITINGFUNCTION_H
