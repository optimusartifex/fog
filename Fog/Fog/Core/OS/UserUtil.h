// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_USERUTIL_H
#define _FOG_CORE_OS_USERUTIL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [Fog::UserUtil]
// ============================================================================

//! @brief Provides information about user.
struct FOG_NO_EXPORT UserUtil
{
  //! @brief Get the current user ID.
  static FOG_INLINE uint32_t getUid()
  {
    return fog_api.userutil_getUid();
  }

  //! @brief Get the current group ID.
  static FOG_INLINE uint32_t getGid()
  {
    return fog_api.userutil_getGid();
  }

  //! @brief Get the user related directory, see @c USER_DIRECTORY enumeration
  //! for possibilities.
  //!
  //! @note This function can fail if there is no such directory associated for
  //! the current used. In such case the HOME directory should be used. The HOME
  //! directory can point into /tmp in UNIX based systems if the application is
  //! run in restricted environment (sandbox for example).
  static FOG_INLINE err_t getUserDirectory(StringW& dst, uint32_t userDirectory)
  {
    return fog_api.userutil_getUserDirectory(&dst, userDirectory);
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_OS_USERUTIL_H
