// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_BORDERFILTERTAG_H
#define _FOG_G2D_IMAGING_BORDERFILTERTAG_H

// [Dependencies]
#include <Fog/G2d/Imaging/ImageFilterTag.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::BorderFilter]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
struct FOG_NO_EXPORT BorderFilterTag : public ImageFilterTag
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getExtendType() const { return _extendType; }
  FOG_INLINE const Color& getExtendColor() const { return _extendColor(); }

  FOG_INLINE void setExtendType(uint32_t type) { _extendType = type; }
  FOG_INLINE void setExtendColor(const Color& color) { _extendColor() = color; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The border extend type, see @c BORDER_EXTEND_TYPE.
  uint32_t _extendType;
  //! @brief The border extend color.
  Static<Color> _extendColor;
};
#include <Fog/Core/C++/PackRestore.h>

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_BORDERFILTERTAG_H
