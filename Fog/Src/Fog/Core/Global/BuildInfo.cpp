// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>

// ============================================================================
// [Fog::BuildInfo]
// ============================================================================

static const Fog::BuildInfo _fog_build_info_data =
{
  // Major.
  0,
  // Minor.
  0,
  // Patch.
  0,
  // BuildType.
#if defined(FOG_DEBUG) || defined(DEBUG) || defined(_DEBUG)
  Fog::BUILD_TYPE_DEBUG
#else
  Fog::BUILD_TYPE_RELEASE
#endif
};

FOG_CAPI_EXTERN const Fog::BuildInfo* _fog_build_info(void)
{
  return &_fog_build_info_data;
}
