// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_UIENGINEMOUSEINFO_H
#define _FOG_UI_ENGINE_UIENGINEMOUSEINFO_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Point.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::UIEngineMouseInfo]
// ============================================================================

//! @brief Frame-buffer mouse information.
struct FOG_NO_EXPORT UIEngineMouseInfo
{
  FOG_INLINE UIEngineMouseInfo() :
    _devicesCount(0),
    _wheelLines(0),
    _doubleClickInterval()
  {
  }

  explicit FOG_INLINE UIEngineMouseInfo(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasMouse() const { return _devicesCount > 0; }

  FOG_INLINE uint32_t getDevicesCount() const { return _devicesCount; }
  FOG_INLINE uint32_t getWheelLines() const { return _wheelLines; }
  FOG_INLINE TimeDelta getDoubleClickInterval() const { return _doubleClickInterval; }

  FOG_INLINE void setDevicesCount(uint32_t devicesCount) { _devicesCount = devicesCount; }
  FOG_INLINE void setWheelLines(uint32_t wheelLines) { _wheelLines = wheelLines; }
  FOG_INLINE void setDoubleClickInterval(const TimeDelta& delta) { _doubleClickInterval = delta; }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const UIEngineMouseInfo& other) const
  {
    return _devicesCount        == other._devicesCount        &&
           _wheelLines          == other._wheelLines          &&
           _doubleClickInterval == other._doubleClickInterval ;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineMouseInfo& operator=(const UIEngineMouseInfo& other)
  {
    _devicesCount = other._devicesCount;
    _wheelLines = other._wheelLines;
    _doubleClickInterval = other._doubleClickInterval;

    return *this;
  }
  
  FOG_INLINE bool operator==(const UIEngineMouseInfo& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const UIEngineMouseInfo& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Count of mouse devices (default 1).
  uint32_t _devicesCount;

  //! @brief Count of lines to scroll through mouse wheel.
  uint32_t _wheelLines;

  //! @brief Double-click interval.
  TimeDelta _doubleClickInterval;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_UIENGINEMOUSEINFO_H
