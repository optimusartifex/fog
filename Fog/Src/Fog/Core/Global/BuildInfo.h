// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_BUILDINFO_H
#define _FOG_CORE_GLOBAL_BUILDINFO_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>
#include <Fog/Core/Global/TypeDefs.h>

//! @brief Get Type of build used to create Fog-Framework shared or static library.
FOG_CAPI_EXTERN const Fog::BuildInfo* _fog_build_info(void);

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Fog::BuildInfo]
// ============================================================================

struct FOG_NO_EXPORT BuildInfo
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getMajor() const { return _major; }
  FOG_INLINE uint32_t getMinor() const { return _minor; }
  FOG_INLINE uint32_t getPatch() const { return _patch; }

  FOG_INLINE uint32_t getBuildType() const { return _buildType; }
  FOG_INLINE bool isDebugVersion() const { return _buildType != BUILD_TYPE_RELEASE; }
  FOG_INLINE bool isReleaseVersion() const { return _buildType == BUILD_TYPE_RELEASE; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE const BuildInfo* get() { return _fog_build_info(); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _major;
  uint32_t _minor;
  uint32_t _patch;
  uint32_t _buildType;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_BUILDINFO_H
