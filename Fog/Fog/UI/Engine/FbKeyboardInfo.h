// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_FBKEYBOARDINFO_H
#define _FOG_UI_ENGINE_FBKEYBOARDINFO_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Point.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::FbKeyboardInfo]
// ============================================================================

//! @brief Frame-buffer mouse information.
struct FOG_NO_EXPORT FbKeyboardInfo
{
  FOG_INLINE FbKeyboardInfo() :
    _devicesCount(0)
  {
  }

  explicit FOG_INLINE FbKeyboardInfo(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------
  
  FOG_INLINE bool hasKeyboard() const { return _devicesCount > 0; }

  FOG_INLINE uint32_t getDevicesCount() const { return _devicesCount; }
  FOG_INLINE TimeDelta getRepeatingDelay() const { return _repeatingDelay; }
  FOG_INLINE TimeDelta getRepeatingInterval() const { return _repeatingInterval; }

  FOG_INLINE void setDevicesCount(uint32_t devicesCount) { _devicesCount = devicesCount; }
  FOG_INLINE void setRepeatingDelay(const TimeDelta& delta) { _repeatingDelay = delta; }
  FOG_INLINE void setRepeatingInterval(const TimeDelta& delta) { _repeatingInterval = delta; }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FbKeyboardInfo& other) const
  {
    return _devicesCount      == other._devicesCount      &&
           _repeatingDelay    == other._repeatingDelay    && 
           _repeatingInterval == other._repeatingInterval ;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FbKeyboardInfo& operator=(const FbKeyboardInfo& other)
  {
    _devicesCount = other._devicesCount;
    _repeatingDelay = other._repeatingDelay;
    _repeatingInterval = other._repeatingInterval;

    return *this;
  }
  
  FOG_INLINE bool operator==(const FbKeyboardInfo& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FbKeyboardInfo& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Count of keyboard devices (default 1).
  //!
  //! Always set to 0 or 1 at this time. It's not supported to have more than
  //! 1 keyboard at this time.
  uint32_t _devicesCount;

  //! @brief Key-press repeating delay (between the first and second repeat).
  TimeDelta _repeatingDelay;

  //! @brief Key-press repeating interval.
  TimeDelta _repeatingInterval;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBKEYBOARDINFO_H
