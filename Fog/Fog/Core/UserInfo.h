// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_USERINFO_H
#define _FOG_CORE_USERINFO_H

// [Dependencies]
#include <Fog/Core/String.h>

//! @addtogroup Fog_Core
//! @{

namespace Fog {

// ============================================================================
// [Fog::UserInfo]
// ============================================================================

//! @brief Provides information about user.
struct FOG_API UserInfo
{
  enum DIRECTORY_ID
  {
    DIRECTORY_HOME = 0,
    DIRECTORY_DESKTOP = 1,
    DIRECTORY_DOCUMENTS = 2,
    DIRECTORY_MUSIC = 3,
    DIRECTORY_PICTURES = 4,
    DIRECTORY_VIDEOS = 5,

    DIRECTORY_INVALID
  };

  static uint32_t uid();
  static uint32_t gid();

  //! @brief Get user related directory, see @c DIRECTORY_ID enumeration for
  //! possibilities.
  static err_t getDirectory(String& dst, int id);
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_CORE_USERINFO_H
