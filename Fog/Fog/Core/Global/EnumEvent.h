// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_ENUMEVENT_H
#define _FOG_CORE_GLOBAL_ENUMEVENT_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Fog::EVENT_CORE]
// ============================================================================

//! @brief Core event IDs.
enum EVENT_CORE_ENUM
{
  // --------------------------------------------------------------------------
  // [Null]
  // --------------------------------------------------------------------------

  //! @brief Null event code.
  //!
  //! Reserved, shouldn't be used.
  EVENT_NULL = 0,

  // --------------------------------------------------------------------------
  // [Object - Create / Destroy]
  // --------------------------------------------------------------------------

  EVENT_CREATE,
  EVENT_DESTROY,
  EVENT_DELETE,

  // --------------------------------------------------------------------------
  // [Object - Properties]
  // --------------------------------------------------------------------------

  EVENT_PROPERTY,

  // --------------------------------------------------------------------------
  // [Object - Hierarchy]
  // --------------------------------------------------------------------------

  EVENT_CHILD_ADD,
  EVENT_CHILD_REMOVE,

  // --------------------------------------------------------------------------
  // [Timer]
  // --------------------------------------------------------------------------

  EVENT_TIMER,

  // --------------------------------------------------------------------------
  // [User]
  // --------------------------------------------------------------------------

  //! @brief First user event.
  EVENT_USER = 65536,

  // --------------------------------------------------------------------------
  // [UID]
  // --------------------------------------------------------------------------

  //! @brief Last event that can be used by all libraries.
  //!
  //! This number is first retrieved by Fog::Event::uid() and incremented
  //! each time the function is called.
  EVENT_UID = 10000000
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_ENUMEVENT_H
