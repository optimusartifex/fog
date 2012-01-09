// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_FECOLORLUT_H
#define _FOG_G2D_IMAGING_FILTERS_FECOLORLUT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Imaging/Filters/FeBase.h>
#include <Fog/G2d/Imaging/Filters/FeColorLutArray.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::FeColorLut]
// ============================================================================

struct FOG_NO_EXPORT FeColorLut : public FeBase
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeColorLut()
  {
    fog_api.fecolorlut_ctor(this);
  }

  FOG_INLINE FeColorLut(const FeColorLut& other)
  {
    fog_api.fecolorlut_ctorCopy(this, &other);
  }

  FOG_INLINE ~FeColorLut()
  {
    fog_api.fecolorlut_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const FeColorLutArray& getComponentArray(uint32_t component) const
  {
    FOG_ASSERT(component < COLOR_INDEX_COUNT);
    return c[component]();
  }

  FOG_INLINE const FeColorLutArray& getAlphaArray() const { return getComponentArray(COLOR_INDEX_ALPHA); }
  FOG_INLINE const FeColorLutArray& getRedArray() const { return getComponentArray(COLOR_INDEX_RED); }
  FOG_INLINE const FeColorLutArray& getGreenArray() const { return getComponentArray(COLOR_INDEX_GREEN); }
  FOG_INLINE const FeColorLutArray& getBlueArray() const { return getComponentArray(COLOR_INDEX_BLUE); }

  FOG_INLINE err_t setComponentArray(uint32_t component, const FeColorLutArray& lutArray)
  {
    FOG_ASSERT(component < COLOR_INDEX_COUNT);
    c[component]() = lutArray;

    return ERR_OK;
  }

  FOG_INLINE err_t setAlphaArray(const FeColorLutArray& lutArray) { return setComponentArray(COLOR_INDEX_ALPHA, lutArray); }
  FOG_INLINE err_t setRedArray(const FeColorLutArray& lutArray) { return setComponentArray(COLOR_INDEX_RED, lutArray); }
  FOG_INLINE err_t setGreenArray(const FeColorLutArray& lutArray) { return setComponentArray(COLOR_INDEX_GREEN, lutArray); }
  FOG_INLINE err_t setBlueArray(const FeColorLutArray& lutArray) { return setComponentArray(COLOR_INDEX_BLUE, lutArray); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.fecolorlut_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeColorLut& other) const
  {
    return fog_api.fecolorlut_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE FeColorLut& operator=(const FeColorLut& other)
  {
    fog_api.fecolorlut_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeColorLut& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeColorLut& other) const { return !eq(other); }

  FOG_INLINE const FeColorLutArray& operator[](uint32_t index) const
  {
    FOG_ASSERT(index < COLOR_INDEX_COUNT);
    return c[index];
  }

  FOG_INLINE FeColorLutArray& operator[](uint32_t index)
  {
    FOG_ASSERT(index < COLOR_INDEX_COUNT);
    return c[index];
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FeColorLut* a, const FeColorLut* b)
  {
    return fog_api.fecolorlut_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.fecolorlut_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<FeColorLutArray> c[COLOR_INDEX_COUNT];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_FECOLORLUT_H
