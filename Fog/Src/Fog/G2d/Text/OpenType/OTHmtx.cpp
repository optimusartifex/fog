// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/G2d/Text/OpenType/OTEnum.h>
#include <Fog/G2d/Text/OpenType/OTFace.h>
#include <Fog/G2d/Text/OpenType/OTHHea.h>
#include <Fog/G2d/Text/OpenType/OTHmtx.h>
#include <Fog/G2d/Text/OpenType/OTMaxp.h>

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
    "Fog::OTHmtx::init() - Not a 'hmtx' table.");

  self->_destroy = (OTTableDestroyFunc)OTHmtx_destroy;
  self->_numberOfHMetrics = 0;
  self->_numberOfLeftSideBearing = 0;

  // --------------------------------------------------------------------------
  // [Header]
  // --------------------------------------------------------------------------

  const OTHmtxHeader* header = self->getHeader();

  OTHHea* hhea = self->getFace()->getHHea();
  OTMaxp* maxp = self->getFace()->getMaxp();

  if (FOG_IS_NULL(hhea))
  {
#if defined(FOG_OT_DEBUG)
  Logger::info("Fog::OTHmtx", "init", 
    "Table 'hmtx' requires 'hhea' table to be present.");
#endif // FOG_OT_DEBUG
    return self->setStatus(ERR_FONT_HMTX_HEADER_WRONG_DATA);
  }

  uint32_t numOfHMetrics = hhea->getHeader()->numberOfHMetrics.getValueU();
  uint32_t numOfLSB = 0;

  if (numOfHMetrics > dataLength / sizeof(OTHmtxMetric))
  {
#if defined(FOG_OT_DEBUG)
    Logger::info("Fog::OTHmtx", "init", 
      "Detected inconsistency: numberOfHMetrics=%u is larger than dataLength=%u (%u divided by %u).",
        numOfHMetrics,
        dataLength,
        dataLength / (uint32_t)sizeof(OTHmtxMetric),
        (uint32_t)sizeof(OTHmtxMetric));
#endif // FOG_OT_DEBUG
    return self->setStatus(ERR_FONT_HMTX_HEADER_WRONG_DATA);
  }

  if (maxp != NULL)
  {
    uint32_t numOfGlyphs = maxp->getNumberOfGlyphs();
    uint32_t remainingDataLength = dataLength - (numOfHMetrics / sizeof(OTHmtxMetric));
    
    if (numOfGlyphs < numOfHMetrics)
    {
#if defined(FOG_OT_DEBUG)
      Logger::info("Fog::OTHmtx", "init", 
        "Detected inconsistency: numberOfHMetrics=%u, numberOfGlyphs=%u.",
          numOfHMetrics,
          numOfGlyphs);
#endif // FOG_OT_DEBUG
      return self->setStatus(ERR_FONT_HMTX_HEADER_WRONG_DATA);
    }

    numOfLSB = numOfGlyphs - numOfHMetrics;

    if (numOfLSB > remainingDataLength / 2)
    {
#if defined(FOG_OT_DEBUG)
      Logger::info("Fog::OTHmtx", "init", 
        "Detected inconsistency: numberOfLeftSideBearing=%u is larger than remaining data length=%u (%u divided by 2).",
          numOfHMetrics,
          remainingDataLength,
          remainingDataLength / 2);
#endif // FOG_OT_DEBUG
      return self->setStatus(ERR_FONT_HMTX_HEADER_WRONG_DATA);
    }
  }

  // --------------------------------------------------------------------------
  // [Finished]
  // --------------------------------------------------------------------------

  self->_numberOfHMetrics = numOfHMetrics;
  self->_numberOfLeftSideBearing = numOfLSB;

#if defined(FOG_OT_DEBUG)
  Logger::info("Fog::OTHmtx", "init", "NumberOfHMetrics=%u.", numOfHMetrics);
  Logger::info("Fog::OTHmtx", "init", "NumberOfLeftSideBearing=%u.", numOfLSB);
#endif // FOG_OT_DEBUG

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
