// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_FETURBULENCE_H
#define _FOG_G2D_IMAGING_FILTERS_FETURBULENCE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Imaging/Filters/FeBorder.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::FeTurbulence]
// ============================================================================

#include <Fog/Core/C++/PackDWord.h>
//! @brief Turbulence.
struct FOG_NO_EXPORT FeTurbulence : public FeBase
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeTurbulence()
  {
    fog_api.feturbulence_ctor(this);
  }

  FOG_INLINE FeTurbulence(const FeTurbulence& other)
  {
    fog_api.feturbulence_ctorCopy(this, &other);
  }

  FOG_INLINE FeTurbulence(uint32_t turbulenceType,
    uint32_t numOctaves, uint32_t stitchTitles, uint32_t seed,
    float baseFrequency)
  {
    fog_api.feturbulence_ctor(this);

    _turbulenceType = turbulenceType < FE_TURBULENCE_TYPE_COUNT
      ? turbulenceType
      : FE_TURBULENCE_TYPE_DEFAULT;
    _numOctaves = numOctaves;
    _stitchTitles = !!stitchTitles;
    _seed = seed;

    _hBaseFrequency = baseFrequency;
    _vBaseFrequency = baseFrequency;
  }

  FOG_INLINE FeTurbulence(uint32_t turbulenceType,
    uint32_t numOctaves, uint32_t stitchTitles, uint32_t seed,
    float hBaseFrequency, float vBaseFrequency)
  {
    fog_api.feturbulence_ctor(this);

    _turbulenceType = turbulenceType < FE_TURBULENCE_TYPE_COUNT
      ? turbulenceType
      : FE_TURBULENCE_TYPE_DEFAULT;
    _numOctaves = numOctaves;
    _stitchTitles = stitchTitles;
    _seed = seed;

    _hBaseFrequency = vBaseFrequency;
    _vBaseFrequency = hBaseFrequency;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getTurbulenceType() const { return _turbulenceType; }
  FOG_INLINE uint32_t getNumOctaves() const { return _numOctaves; }
  FOG_INLINE uint32_t getStitchTitles() const { return _stitchTitles; }
  FOG_INLINE int32_t getSeed() const { return _seed; }

  FOG_INLINE float getHorizontalBaseFrequency() const { return _hBaseFrequency; }
  FOG_INLINE float getVerticalBaseFrequency() const { return _vBaseFrequency; }

  FOG_INLINE err_t setTurbulenceType(uint32_t turbulenceType)
  {
    if (turbulenceType >= FE_TURBULENCE_TYPE_COUNT)
      return ERR_RT_INVALID_ARGUMENT;

    _turbulenceType = turbulenceType;
    return ERR_OK;
  }
  
  FOG_INLINE err_t setNumOctaves(uint32_t numOctaves)
  {
    if (numOctaves > 10)
      return ERR_RT_INVALID_ARGUMENT;

    _numOctaves = numOctaves;
    return ERR_OK;
  }

  FOG_INLINE err_t setStitchTitles(uint32_t stitchTitles)
  {
    if (stitchTitles > 1)
      return ERR_RT_INVALID_ARGUMENT;

    _stitchTitles = stitchTitles;
    return ERR_OK;
  }

  FOG_INLINE err_t setSeed(int32_t seed)
  {
    _seed = seed;
    return ERR_OK;
  }

  FOG_INLINE err_t setHorizontalBaseFrequency(float hBaseFrequency)
  {
    _hBaseFrequency = hBaseFrequency;
    return ERR_OK;
  }

  FOG_INLINE err_t setVerticalBaseFrequency(float vBaseFrequency)
  {
    _vBaseFrequency = vBaseFrequency;
    return ERR_OK;
  }

  FOG_INLINE err_t setBaseFrequency(float baseFrequency)
  {
    _hBaseFrequency = baseFrequency;
    _vBaseFrequency = baseFrequency;
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeTurbulence& other) const
  {
    return fog_api.feturbulence_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FeTurbulence& operator=(const FeTurbulence& other)
  {
    fog_api.feturbulence_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeTurbulence& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeTurbulence& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FeTurbulence* a, const FeTurbulence* b)
  {
    return fog_api.feturbulence_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.feturbulence_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The turbulence type, see @c FE_TURBULENCE_TYPE.
  uint32_t _turbulenceType : 16;
  //! @brief Number of octaves.
  uint32_t _numOctaves : 15;
  //! @brief Stitch titles.
  uint32_t _stitchTitles : 1;
  //! @brief The turbulence seed of pseudo random number generator.
  int32_t _seed;

  //! @brief Horizontal base frequency.
  float _hBaseFrequency;
  //! @brief Vertical base frequency.
  float _vBaseFrequency;
};
#include <Fog/Core/C++/PackRestore.h>

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_FETURBULENCE_H
