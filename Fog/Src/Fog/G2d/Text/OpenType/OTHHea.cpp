// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/G2d/Text/OpenType/OTEnum.h>
#include <Fog/G2d/Text/OpenType/OTHHea.h>

namespace Fog {

// ============================================================================
// [Fog::OTHHea - Init / Destroy]
// ============================================================================

static void FOG_CDECL OTHHea_destroy(OTHHea* self)
{
  // This results in crash in case that destroy is called twice by accident.
  self->_destroy = NULL;
}

static err_t FOG_CDECL OTHHea_init(OTHHea* self)
{
  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  const uint8_t* data = self->getData();
  uint32_t dataLength = self->getDataLength();

#if defined(FOG_OT_DEBUG)
  Logger::info("Fog::OTHHea", "init", 
    "Initializing 'hhea' table (%u bytes).", dataLength);
#endif // FOG_OT_DEBUG

  FOG_ASSERT_X(self->_tag == FOG_OT_TAG('h', 'h', 'e', 'a'),
    "Fog::OTHHea::init() - Not a 'head' table.");

  self->_destroy = (OTTableDestroyFunc)OTHHea_destroy;

  // --------------------------------------------------------------------------
  // [Header]
  // --------------------------------------------------------------------------

  const OTHHeaHeader* header = self->getHeader();

  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OTHHea_init(void)
{
  OTApi& api = fog_ot_api;

  // --------------------------------------------------------------------------
  // [OTHHea]
  // --------------------------------------------------------------------------
  
  api.othhea_init = OTHHea_init;
}

} // Fog namespace
