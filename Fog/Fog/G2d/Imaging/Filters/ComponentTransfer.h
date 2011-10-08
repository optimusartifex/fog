// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_COMPONENTTRANSFER_H
#define _FOG_G2D_IMAGING_FILTERS_COMPONENTTRANSFER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Imaging/ImageFilterTag.h>
#include <Fog/G2d/Imaging/Filters/ComponentTransferFunction.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ComponentTransfer]
// ============================================================================

//! @brief Component transfer.
struct FOG_NO_EXPORT ComponentTransfer : public ImageFilterTag
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ComponentTransfer()
  {
    _api.componenttransfer_ctor(this);
  }

  FOG_INLINE ComponentTransfer(const ComponentTransfer& other)
  {
    _api.componenttransfer_ctorCopy(this, &other);
  }

  FOG_INLINE ~ComponentTransfer()
  {
    _api.componenttransfer_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const ComponentTransferFunction& getComponentFunction(uint32_t component) const
  {
    FOG_ASSERT(component < COLOR_INDEX_COUNT);
    return c[component]();
  }

  FOG_INLINE const ComponentTransferFunction& getAlphaFunction() const { return getComponentFunction(COLOR_INDEX_ALPHA); }
  FOG_INLINE const ComponentTransferFunction& getRedFunction() const { return getComponentFunction(COLOR_INDEX_RED); }
  FOG_INLINE const ComponentTransferFunction& getGreenFunction() const { return getComponentFunction(COLOR_INDEX_GREEN); }
  FOG_INLINE const ComponentTransferFunction& getBlueFunction() const { return getComponentFunction(COLOR_INDEX_BLUE); }

  FOG_INLINE err_t setComponentFunction(uint32_t component, const ComponentTransferFunction& function)
  {
    FOG_ASSERT(component < COLOR_INDEX_COUNT);
    c[component]() = function;

    return ERR_OK;
  }

  FOG_INLINE err_t setAlphaFunction(const ComponentTransferFunction& func) { return setComponentFunction(COLOR_INDEX_ALPHA, func); }
  FOG_INLINE err_t setRedFunction(const ComponentTransferFunction& func) { return setComponentFunction(COLOR_INDEX_RED, func); }
  FOG_INLINE err_t setGreenFunction(const ComponentTransferFunction& func) { return setComponentFunction(COLOR_INDEX_GREEN, func); }
  FOG_INLINE err_t setBlueFunction(const ComponentTransferFunction& func) { return setComponentFunction(COLOR_INDEX_BLUE, func); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _api.componenttransfer_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const ComponentTransfer& other) const
  {
    return _api.componenttransfer_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE ComponentTransfer& operator=(const ComponentTransfer& other)
  {
    _api.componenttransfer_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const ComponentTransfer& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ComponentTransfer& other) const { return !eq(other); }

  FOG_INLINE const ComponentTransferFunction& operator[](uint32_t index) const
  {
    FOG_ASSERT(index < COLOR_INDEX_COUNT);
    return c[index];
  }

  FOG_INLINE ComponentTransferFunction& operator[](uint32_t index)
  {
    FOG_ASSERT(index < COLOR_INDEX_COUNT);
    return c[index];
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const ComponentTransfer* a, const ComponentTransfer* b)
  {
    return _api.componenttransfer_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.componenttransfer_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<ComponentTransferFunction> c[COLOR_INDEX_COUNT];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_COMPONENTTRANSFER_H
