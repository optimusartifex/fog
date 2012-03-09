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
FOG_NO_EXPORT void OTHHea_init(void);
FOG_NO_EXPORT void OTHead_init(void);
FOG_NO_EXPORT void OTHmtx_init(void);
FOG_NO_EXPORT void OTKern_init(void);
FOG_NO_EXPORT void OTMaxp_init(void);
FOG_NO_EXPORT void OTName_init(void);

FOG_NO_EXPORT void OTApi_init(void)
{
  OTTypes_init();
  OTFace_init();

  OTHead_init();
  OTName_init();
  OTHHea_init();
  OTHmtx_init();
  OTCMap_init();
  OTKern_init();
  OTMaxp_init();
}

} // Fog namespace
