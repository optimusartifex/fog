// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_EFFECTS_BORDEREFFECT_H
#define _FOG_G2D_IMAGING_EFFECTS_BORDEREFFECT_H

// [Dependencies]
#include <Fog/Core/Memory/MemoryOps.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::BorderEffect]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
struct FOG_NO_EXPORT BorderEffect
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

protected:
  FOG_INLINE BorderEffect() :
    _extendType(BORDER_EXTEND_DEFAULT),
    _extendColor()
  {
    // This should be the smallest/fastest possible way to set extend color
    // to ARGB and to transparent black.
    _extendColor._model = COLOR_MODEL_ARGB;
  }

  FOG_INLINE BorderEffect(uint32_t extendType, const Color& extendColor) :
    _extendType(extendType),
    _extendColor(extendColor)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

public:
  FOG_INLINE uint32_t getExtendType() const { return _extendType; }
  FOG_INLINE const Color& getExtendColor() const { return _extendColor; }

  FOG_INLINE void setExtendType(uint32_t extendType) { _extendType = extendType; }
  FOG_INLINE void setExtendColor(const Color& extendColor) { _extendColor = extendColor; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

public:
  //! @brief The border extend type, see @c BORDER_EXTEND_TYPE.
  uint32_t _extendType;
  //! @brief The border extend color.
  Color _extendColor;
};
#include <Fog/Core/Pack/PackRestore.h>

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_EFFECTS_BORDEREFFECT_H
