// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_FECOMPONENTTRANSFER_H
#define _FOG_G2D_IMAGING_FILTERS_FECOMPONENTTRANSFER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Imaging/Filters/FeBase.h>
#include <Fog/G2d/Imaging/Filters/FeComponentFunction.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::FeComponentTransfer]
// ============================================================================

//! @brief Component transfer.
struct FOG_NO_EXPORT FeComponentTransfer : public FeBase
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeComponentTransfer()
  {
    _api.fecomponenttransfer_ctor(this);
  }

  FOG_INLINE FeComponentTransfer(const FeComponentTransfer& other)
  {
    _api.fecomponenttransfer_ctorCopy(this, &other);
  }

  FOG_INLINE ~FeComponentTransfer()
  {
    _api.fecomponenttransfer_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const FeComponentFunction& getComponentFunction(uint32_t component) const
  {
    FOG_ASSERT(component < COLOR_INDEX_COUNT);
    return c[component]();
  }

  FOG_INLINE const FeComponentFunction& getAlphaFunction() const { return getComponentFunction(COLOR_INDEX_ALPHA); }
  FOG_INLINE const FeComponentFunction& getRedFunction() const { return getComponentFunction(COLOR_INDEX_RED); }
  FOG_INLINE const FeComponentFunction& getGreenFunction() const { return getComponentFunction(COLOR_INDEX_GREEN); }
  FOG_INLINE const FeComponentFunction& getBlueFunction() const { return getComponentFunction(COLOR_INDEX_BLUE); }

  FOG_INLINE err_t setComponentFunction(uint32_t component, const FeComponentFunction& function)
  {
    FOG_ASSERT(component < COLOR_INDEX_COUNT);
    c[component]() = function;

    return ERR_OK;
  }

  FOG_INLINE err_t setAlphaFunction(const FeComponentFunction& func) { return setComponentFunction(COLOR_INDEX_ALPHA, func); }
  FOG_INLINE err_t setRedFunction(const FeComponentFunction& func) { return setComponentFunction(COLOR_INDEX_RED, func); }
  FOG_INLINE err_t setGreenFunction(const FeComponentFunction& func) { return setComponentFunction(COLOR_INDEX_GREEN, func); }
  FOG_INLINE err_t setBlueFunction(const FeComponentFunction& func) { return setComponentFunction(COLOR_INDEX_BLUE, func); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _api.fecomponenttransfer_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeComponentTransfer& other) const
  {
    return _api.fecomponenttransfer_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE FeComponentTransfer& operator=(const FeComponentTransfer& other)
  {
    _api.fecomponenttransfer_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeComponentTransfer& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeComponentTransfer& other) const { return !eq(other); }

  FOG_INLINE const FeComponentFunction& operator[](uint32_t index) const
  {
    FOG_ASSERT(index < COLOR_INDEX_COUNT);
    return c[index];
  }

  FOG_INLINE FeComponentFunction& operator[](uint32_t index)
  {
    FOG_ASSERT(index < COLOR_INDEX_COUNT);
    return c[index];
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FeComponentTransfer* a, const FeComponentTransfer* b)
  {
    return _api.fecomponenttransfer_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.fecomponenttransfer_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<FeComponentFunction> c[COLOR_INDEX_COUNT];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_FECOMPONENTTRANSFER_H
