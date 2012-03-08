// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/G2d/Text/OpenType/OTEnum.h>
#include <Fog/G2d/Text/OpenType/OTHmtx.h>

namespace Fog {

// ============================================================================
// [Fog::OTHead - Init / Destroy]
// ============================================================================

static void FOG_CDECL OTHmtx_destroy(OTHmtx* self)
{
  // This results in crash in case that destroy is called twice by accident.
  self->_destroy = NULL;
}

static err_t FOG_CDECL OTHmtx_init(OTHmtx* self)
{
  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  const uint8_t* data = self->getData();
  uint32_t dataLength = self->getDataLength();

#if defined(FOG_OT_DEBUG)
  Logger::info("Fog::OTHmtx", "init", 
    "Initializing 'hmtx' table (%u bytes).", dataLength);
#endif // FOG_OT_DEBUG

  FOG_ASSERT_X(self->_tag == FOG_OT_TAG('h', 'm', 't', 'x'),
    "Fog::OTHmtx::init() - Not a 'head' table.");

  self->_destroy = (OTTableDestroyFunc)OTHmtx_destroy;

  // --------------------------------------------------------------------------
  // [Header]
  // --------------------------------------------------------------------------

  const OTHmtxHeader* header = self->getHeader();

  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OTHmtx_init(void)
{
  OTApi& api = fog_ot_api;

  // --------------------------------------------------------------------------
  // [OTHmtx]
  // --------------------------------------------------------------------------
  
  api.othmtx_init = OTHmtx_init;
}

} // Fog namespace
