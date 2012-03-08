// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/G2d/Text/OpenType/OTEnum.h>
#include <Fog/G2d/Text/OpenType/OTHead.h>

namespace Fog {

// ============================================================================
// [Fog::OTHead - Init / Destroy]
// ============================================================================

static void FOG_CDECL OTHead_destroy(OTHead* self)
{
  // This results in crash in case that destroy is called twice by accident.
  self->_destroy = NULL;
}

static err_t FOG_CDECL OTHead_init(OTHead* self)
{
  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  const uint8_t* data = self->getData();
  uint32_t dataLength = self->getDataLength();

#if defined(FOG_OT_DEBUG)
  Logger::info("Fog::OTHead", "init", 
    "Initializing 'head' table (%u bytes).", dataLength);
#endif // FOG_OT_DEBUG

  FOG_ASSERT_X(self->_tag == FOG_OT_TAG('h', 'e', 'a', 'd'),
    "Fog::OTHead::init() - Not a 'head' table.");

  self->_destroy = (OTTableDestroyFunc)OTHead_destroy;
  self->_unitsPerEM = 0;

  // --------------------------------------------------------------------------
  // [Header]
  // --------------------------------------------------------------------------

  const OTHeadHeader* header = self->getHeader();

  // MagicNumber.
  if (header->magicNumber.getValueA() != 0x5F0F3CF5)
  {
#if defined(FOG_OT_DEBUG)
    Logger::info("Fog::OTHead", "init", 
      "Magic number do not match (%08X).", header->magicNumber.getValueA());
#endif // FOG_OT_DEBUG

    return self->setStatus(ERR_FONT_HEAD_HEADER_WRONG_DATA);
  }

  // Version.
  if (header->version.getValueA() < 0x00010000)
  {
#if defined(FOG_OT_DEBUG)
    Logger::info("Fog::OTHead", "init", 
      "Version of the table is not supported (%d.%d).",
        header->version.getValueA() >> 16,
        header->version.getValueA() & 0xFFFF);
#endif // FOG_OT_DEBUG

    return self->setStatus(ERR_FONT_HEAD_HEADER_WRONG_DATA);
  }

  // If unitsPerEM is not set or invalid, assume 1000, which matches typical
  // Type1 usage.
  self->_unitsPerEM = header->unitsPerEm.getValueA();
  if (!Math::isBounded<uint16_t>(self->_unitsPerEM, 16, 16384));
    self->_unitsPerEM = 1000;

  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OTHead_init(void)
{
  OTApi& api = fog_ot_api;

  // --------------------------------------------------------------------------
  // [OTHead]
  // --------------------------------------------------------------------------
  
  api.othead_init = OTHead_init;
}

} // Fog namespace
