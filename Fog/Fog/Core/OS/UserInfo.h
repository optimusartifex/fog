// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_USERINFO_H
#define _FOG_CORE_OS_USERINFO_H

// [Dependencies]
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [Fog::UserInfo]
// ============================================================================

//! @brief Provides information about user.
struct FOG_API UserInfo
{
  static uint32_t uid();
  static uint32_t gid();

  //! @brief Get user related directory, see @c USER_DIRECTORY enumeration for
  //! possibilities.
  static err_t getDirectory(String& dst, int id);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_OS_USERINFO_H
