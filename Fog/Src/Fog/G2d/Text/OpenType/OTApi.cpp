// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/G2d/Text/OpenType/OTApi.h>

// ============================================================================
// [fog_ot_api]
// ============================================================================

FOG_CVAR_DECLARE Fog::OTApi fog_ot_api;

namespace Fog {

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OTTypes_init(void);
FOG_NO_EXPORT void OTFace_init(void);
FOG_NO_EXPORT void OTCMap_init(void);

FOG_NO_EXPORT void OTApi_init(void)
{
  OTTypes_init();
  OTFace_init();

  // TrueType/OpenType Tables.
  OTCMap_init();
}

} // Fog namespace
