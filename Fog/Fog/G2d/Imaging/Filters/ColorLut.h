// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_COLORLUT_H
#define _FOG_G2D_IMAGING_FILTERS_COLORLUT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Imaging/ImageFilterTag.h>
#include <Fog/G2d/Imaging/Filters/ColorLutArray.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ColorLut]
// ============================================================================

struct FOG_NO_EXPORT ColorLut : public ImageFilterTag
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorLut()
  {
    _api.colorlut_ctor(this);
  }

  FOG_INLINE ColorLut(const ColorLut& other)
  {
    _api.colorlut_ctorCopy(this, &other);
  }

  FOG_INLINE ~ColorLut()
  {
    _api.colorlut_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const ColorLutArray& getComponentArray(uint32_t component) const
  {
    FOG_ASSERT(component < COLOR_INDEX_COUNT);
    return c[component]();
  }

  FOG_INLINE const ColorLutArray& getAlphaFunction() const { return getComponentArray(COLOR_INDEX_ALPHA); }
  FOG_INLINE const ColorLutArray& getRedFunction() const { return getComponentArray(COLOR_INDEX_RED); }
  FOG_INLINE const ColorLutArray& getGreenFunction() const { return getComponentArray(COLOR_INDEX_GREEN); }
  FOG_INLINE const ColorLutArray& getBlueFunction() const { return getComponentArray(COLOR_INDEX_BLUE); }

  FOG_INLINE err_t setComponentArray(uint32_t component, const ColorLutArray& lutArray)
  {
    FOG_ASSERT(component < COLOR_INDEX_COUNT);
    c[component]() = lutArray;

    return ERR_OK;
  }

  FOG_INLINE err_t setAlphaFunction(const ColorLutArray& lutArray) { return setComponentArray(COLOR_INDEX_ALPHA, lutArray); }
  FOG_INLINE err_t setRedFunction(const ColorLutArray& lutArray) { return setComponentArray(COLOR_INDEX_RED, lutArray); }
  FOG_INLINE err_t setGreenFunction(const ColorLutArray& lutArray) { return setComponentArray(COLOR_INDEX_GREEN, lutArray); }
  FOG_INLINE err_t setBlueFunction(const ColorLutArray& lutArray) { return setComponentArray(COLOR_INDEX_BLUE, lutArray); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _api.colorlut_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const ColorLut& other) const
  {
    return _api.colorlut_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE ColorLut& operator=(const ColorLut& other)
  {
    _api.colorlut_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const ColorLut& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ColorLut& other) const { return !eq(other); }

  FOG_INLINE const ColorLutArray& operator[](uint32_t index) const
  {
    FOG_ASSERT(index < COLOR_INDEX_COUNT);
    return c[index];
  }

  FOG_INLINE ColorLutArray& operator[](uint32_t index)
  {
    FOG_ASSERT(index < COLOR_INDEX_COUNT);
    return c[index];
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const ColorLut* a, const ColorLut* b)
  {
    return _api.colorlut_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.colorlut_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<ColorLutArray> c[COLOR_INDEX_COUNT];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_COLORLUT_H
