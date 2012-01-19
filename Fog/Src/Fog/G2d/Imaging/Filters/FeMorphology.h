// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_FEMORPHOLOGY_H
#define _FOG_G2D_IMAGING_FILTERS_FEMORPHOLOGY_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Imaging/Filters/FeBorder.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::Blur]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
//! @brief Morphology.
struct FOG_NO_EXPORT FeMorphology : public FeBorder
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeMorphology()
  {
    fog_api.femorphology_ctor(this);
  }

  FOG_INLINE FeMorphology(const FeMorphology& other)
  {
    fog_api.femorphology_ctorCopy(this, &other);
  }

  FOG_INLINE FeMorphology(uint32_t morphologyType, float radius)
  {
    fog_api.femorphology_ctor(this);

    _morphologyType = morphologyType;
    _hRadius = radius;
    _vRadius = radius;
  }

  FOG_INLINE FeMorphology(uint32_t morphologyType, float hRadius, float vRadius)
  {
    fog_api.femorphology_ctor(this);

    _morphologyType = morphologyType;
    _hRadius = hRadius;
    _vRadius = vRadius;
  }
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getMorphologyType() const { return _morphologyType; }
  FOG_INLINE float getHorizontalRadius() const { return _hRadius; }
  FOG_INLINE float getVerticalRadius() const { return _vRadius; }

  FOG_INLINE err_t setMorphologyType(uint32_t morphologyType)
  {
    if (morphologyType >= FE_MORPHOLOGY_TYPE_COUNT)
      return ERR_RT_INVALID_ARGUMENT;

    _morphologyType = morphologyType;
    return ERR_OK;
  }

  FOG_INLINE void setHorizontalRadius(float radius)
  {
    _hRadius = radius;
  }

  FOG_INLINE void setVerticalRadius(float radius)
  {
    _vRadius = radius;
  }

  FOG_INLINE void setRadius(float radius)
  {
    _hRadius = radius;
    _vRadius = radius;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeMorphology& other) const
  {
    return fog_api.femorphology_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FeMorphology& operator=(const FeMorphology& other)
  {
    fog_api.femorphology_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeMorphology& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeMorphology& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FeMorphology* a, const FeMorphology* b)
  {
    return fog_api.femorphology_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.femorphology_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The morphology type, see @c FE_MORPHOLOGY_TYPE.
  uint32_t _morphologyType;

  //! @brief The horizontal radius.
  float _hRadius;
  //! @brief The vertical radius.
  float _vRadius;
};
#include <Fog/Core/C++/PackRestore.h>

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_FEMORPHOLOGY_H
