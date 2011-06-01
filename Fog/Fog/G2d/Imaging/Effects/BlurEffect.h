// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_EFFECTS_BLUREFFECT_H
#define _FOG_G2D_IMAGING_EFFECTS_BLUREFFECT_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Memory/MemoryOps.h>
#include <Fog/G2d/Imaging/Effects/BorderEffect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::Blur]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
struct FOG_NO_EXPORT BlurEffect : public BorderEffect
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE BlurEffect() :
    BorderEffect(),
    _blurType(BLUR_FX_DEFAULT),
    _hRadius(1.0f),
    _vRadius(1.0f)
  {
  }

  FOG_INLINE BlurEffect(const BlurEffect& other) :
    BorderEffect(other._extendType, other._extendColor),
    _blurType(other._blurType),
    _hRadius(other._hRadius),
    _vRadius(other._vRadius)
  {
  }

  FOG_INLINE BlurEffect(uint32_t blurType, float hRadius, float vRadius) :
    BorderEffect(),
    _blurType(blurType),
    _hRadius(hRadius),
    _vRadius(vRadius)
  {
  }

  FOG_INLINE BlurEffect(uint32_t blurType, float hRadius, float vRadius, uint32_t extendType, const Color& extendColor) :
    BorderEffect(extendType, extendColor),
    _blurType(blurType),
    _hRadius(hRadius),
    _vRadius(vRadius)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getBlurType() const { return _blurType; }
  FOG_INLINE void setBlurType(uint32_t blurType) { _blurType = blurType; }

  FOG_INLINE float getHorizontalRadius() const { return _hRadius; }
  FOG_INLINE void setHorizontalRadius(float radius) { _hRadius = radius; }

  FOG_INLINE float getVerticalRadius() const { return _vRadius; }
  FOG_INLINE void setVerticalRadius(float radius) { _vRadius = radius; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE BlurEffect& operator=(const BlurEffect& other)
  {
    Memory::copy_t<BlurEffect>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The blur type, see @c BLUR_FX_TYPE.
  uint32_t _blurType;

  //! @brief The horizontal radius.
  float _hRadius;
  //! @brief The vertical radius.
  float _vRadius;
};
#include <Fog/Core/Pack/PackRestore.h>

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::BlurEffect, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_G2D_IMAGING_EFFECTS_BLUREFFECT_H
