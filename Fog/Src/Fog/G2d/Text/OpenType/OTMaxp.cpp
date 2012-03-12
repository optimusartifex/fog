// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/G2d/Text/OpenType/OTEnum.h>
#include <Fog/G2d/Text/OpenType/OTMaxp.h>

namespace Fog {

// ============================================================================
// [Fog::OTMaxp - Init / Destroy]
// ============================================================================

static void FOG_CDECL OTMaxp_destroy(OTMaxp* self)
{
  // This results in crash in case that destroy is called twice by accident.
  self->_destroy = NULL;
}

static err_t FOG_CDECL OTMaxp_init(OTMaxp* self)
{
  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  const uint8_t* data = self->getData();
  uint32_t dataLength = self->getDataLength();

#if defined(FOG_OT_DEBUG)
  Logger::info("Fog::OTMaxp", "init", 
    "Initializing 'maxp' table (%u bytes).", dataLength);
#endif // FOG_OT_DEBUG

  FOG_ASSERT_X(self->_tag == FOG_OT_TAG('m', 'a', 'x', 'p'),
    "Fog::OTMaxp::init() - Not a 'maxp' table.");

  self->_destroy = (OTTableDestroyFunc)OTMaxp_destroy;

  // --------------------------------------------------------------------------
  // [Header]
  // --------------------------------------------------------------------------

  if (self->getVersion() < 0x00005000)
  {
#if defined(FOG_OT_DEBUG)
    Logger::info("Fog::OTMaxp", "init", 
      "Unsupported header version (%u.%u bytes).",
        self->getVersion() >> 16,
        self->getVersion() & 0xFFFF);
#endif // FOG_OT_DEBUG
    return self->setStatus(ERR_FONT_MAXP_HEADER_WRONG_VERSION);    
  }

  const OTMaxpHeaderV0_5* header = self->getHeaderV0_5();
  uint32_t numOfGlyphs = header->numGlyphs.getValueU();

  if (numOfGlyphs == 0)
  {
#if defined(FOG_OT_DEBUG)
    Logger::info("Fog::OTMaxp", "init", 
      "Number of glyphs is zero.");
#endif // FOG_OT_DEBUG
    return self->setStatus(ERR_FONT_MAXP_HEADER_WRONG_DATA);    
  }

  // --------------------------------------------------------------------------
  // [Finished]
  // --------------------------------------------------------------------------

  self->_numberOfGlyphs = numOfGlyphs;

#if defined(FOG_OT_DEBUG)
  Logger::info("Fog::OTMaxp", "init", "NumberOfGlyphs=%u.", numOfGlyphs);
#endif // FOG_OT_DEBUG

  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OTMaxp_init(void)
{
  OTApi& api = fog_ot_api;

  // --------------------------------------------------------------------------
  // [OTMaxp]
  // --------------------------------------------------------------------------
  
  api.otmaxp_init = OTMaxp_init;
}

} // Fog namespace
