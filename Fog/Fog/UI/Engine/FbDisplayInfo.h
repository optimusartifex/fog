// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_FBDISPLAYINFO_H
#define _FOG_UI_ENGINE_FBDISPLAYINFO_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Size.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::FbDisplayInfo]
// ============================================================================

//! @brief Frame-buffer display info.
struct FOG_NO_EXPORT FbDisplayInfo
{
  FOG_INLINE FbDisplayInfo() :
    size(0, 0),
    depth(0),
    rMask(0x00000000),
    gMask(0x00000000),
    bMask(0x00000000),
    is16BitSwapped(0)
  {
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FbDisplayInfo& other) const
  {
    return MemOps::eq(this, &other, sizeof(FbDisplayInfo));
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator=(const FbDisplayInfo& other)
  {
    MemOps::copy(this, &other, sizeof(FbDisplayInfo));
    return *this;
  }

  FOG_INLINE bool operator==(const FbDisplayInfo& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FbDisplayInfo& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Screen width.
  SizeI size;
  //! @brief Screen depth.
  uint32_t depth;

  //! @brief Screen red mask.
  uint32_t rMask;
  //! @brief Screen green mask.
  uint32_t gMask;
  //! @brief Screen blue mask.
  uint32_t bMask;
  //! @brief If true, 16 bit depth is byteswapped (X Server). In other depths
  //! are byteswapped instead rMask, gMask and bMask values.
  uint32_t is16BitSwapped;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBDISPLAYINFO_H
