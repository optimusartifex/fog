// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/G2d/Text/OpenType/OTCMap.h>
#include <Fog/G2d/Text/OpenType/OTEnum.h>
#include <Fog/G2d/Text/OpenType/OTUtil.h>

namespace Fog {

// ============================================================================
// [Fog::OTCMapFormat0]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMapFormat2]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMapFormat4]
// ============================================================================

template<bool IsAligned>
static FOG_INLINE const uint8_t* OTCMapContext4_bsearch(
  uint32_t& start,
  uint32_t& end,
  const uint8_t* pEnd,
  uint32_t numSeg,
  uint32_t uc)
{
  uint32_t base = 0;

  for (uint32_t lim = numSeg; lim != 0; lim >>= 1)
  {
    const uint8_t* pCur = pEnd + (lim & ~1);

    end = FOG_OT_UINT16(pCur)->getValueU();
    if (end < uc)
    {
      pEnd = pCur + 2;
      lim--;
      continue;
    }

    start = FOG_OT_UINT16(pCur + 2 + numSeg * 2)->getValueU();
    if (start <= uc)
      return pCur;
  }

  return NULL;
}

template<bool IsAligned>
static size_t FOG_CDECL OTCMapContext4_getGlyphPlacement(OTCMapContext* cctx,
  uint32_t* glyphList, size_t glyphAdvance, const uint16_t* sData, size_t sLength)
{
  if (sLength == 0)
    return 0;

  const uint8_t* data = static_cast<const uint8_t*>(cctx->_data);
  const CMapFormat4* tab = reinterpret_cast<const CMapFormat4*>(data);

  FOG_ASSERT(tab->format.getValueT<IsAligned>() == 4);

  uint32_t length = tab->length.getValueT<IsAligned>();
  uint32_t numSeg = tab->numSegX2.getValueT<IsAligned>() >> 1;

  const uint16_t* sMark = sData;
  const uint16_t* sEnd = sData + sLength;
/*
  {
    uint j;
    for (j = 0; j < numSeg; j++)
    {
      Logger::info("Fog::OTCMapContext4", "getGlyphPlacement",
        "#%02d start=%d, end=%d, delta=%d, offset=%d\n",
          j,
          FOG_OT_UINT16(data + 16 + numSeg * 2 + j * 2)->getValueU(),
          FOG_OT_UINT16(data + 14 + j * 2)->getValueU(),
          FOG_OT_UINT16(data + 16 + numSeg * 4 + j * 2)->getValueU(),
          FOG_OT_UINT16(data + 16 + numSeg * 6 + j * 2)->getValueU());
    }
  }
*/

  uint32_t uc = sData[0];
  for (;;)
  {
    uint32_t start, end, offset;
    const uint8_t* pEnd;
    const uint8_t* pOffset;

    if (CharW::isSurrogate(uc))
      goto _MissingGlyph;

    pEnd = OTCMapContext4_bsearch<IsAligned>(start, end, data + 14, numSeg, uc);
    if (pEnd == NULL)
      goto _MissingGlyph;

    // 'data + 14' points to the 'endChar' table. There are two bytes reserved
    // which should be zero, so we need to skip them. After the skip pEnd points
    // to location L at index I, which can be used to access:
    //
    //   - endChar[I]   == pEnd[-2]
    //   - startChar[I] == pEnd[numSeg * 2] 
    //   - delta[I]     == pEnd[numSeg * 4]
    //   - offset[I]    == pEnd[numSeg * 6]

    pEnd += 2;
    pOffset = pEnd + numSeg * 6;

    FOG_ASSERT(pOffset < data + length);
    offset = FOG_OT_UINT16(pOffset)->getValueT<IsAligned>();

_Repeat:
    // According to the specification:
    //
    // If the 'offset' value for the segment is not 0, the mapping of character
    // codes relies on glyphIdArray. The character code offset from 'startChar'
    // is added to the 'offset' value. This sum is used as an offset from the
    // current location within 'offset' itself to index out the correct 
    // glyphIdArray value.
    if (offset != 0)
    {
      uc -= start;

      FOG_ASSERT(pOffset + offset + uc * 2 < data + length);
      uc = FOG_OT_UINT16(pOffset + offset + uc * 2)->getValueT<IsAligned>();

      if (uc == 0)
        goto _MissingGlyph;
    }

    uc += FOG_OT_UINT16(pEnd + numSeg * 4)->getValueT<IsAligned>();
    uc &= 0xFFFF;

    glyphList[0] = uc;
    glyphList = reinterpret_cast<uint32_t*>((uint8_t*)glyphList + glyphAdvance);

    if (++sData == sEnd)
      break;

    // We are processing a string, so if characters are close to each other,
    // we can try to skip a binary search if the next character is in the same
    // range list as the previous character. This optimization is always benefical
    // for latin text and for large range lists.
    uc = sData[0];

    if (uc >= start && uc <= end)
      goto _Repeat;
    continue;

_MissingGlyph:
    glyphList[0] = 0;
    glyphList = reinterpret_cast<uint32_t*>((uint8_t*)glyphList + glyphAdvance);
  }

  return (size_t)(sData - sMark);
}

// ============================================================================
// [Fog::OTCMapFormat6]
// ============================================================================

template<bool IsAligned>
static size_t FOG_CDECL OTCMapContext6_getGlyphPlacement(OTCMapContext* cctx,
  uint32_t* glyphList, size_t glyphAdvance, const uint16_t* sData, size_t sLength)
{
  if (sLength == 0)
    return 0;

  const uint8_t* data = static_cast<const uint8_t*>(cctx->_data);
  const CMapFormat6* tab = reinterpret_cast<const CMapFormat6*>(data);

  FOG_ASSERT(tab->format.getValueT<IsAligned>() == 6);

  uint32_t first = tab->first.getValueT<IsAligned>();
  uint32_t count = tab->count.getValueT<IsAligned>();

  const OTUInt16* glyphIdArray = tab->glyphIdArray;
  const uint16_t* sMark = sData;
  const uint16_t* sEnd = sData + sLength;

  for (;;)
  {
    uint32_t uc = sData[0];
    uint32_t glyphId = 0;

    if (CharW::isSurrogate(uc))
      goto _MissingGlyph;

    uc -= first;
    if (uc < count)
      glyphId = glyphIdArray[uc].getValueT<IsAligned>();

_MissingGlyph:
    glyphList[0] = glyphId;
    glyphList = reinterpret_cast<uint32_t*>((uint8_t*)glyphList + glyphAdvance);

    if (++sData == sEnd)
      break;
  }

  return (size_t)(sData - sMark);
}

// ============================================================================
// [Fog::OTCMapFormat8]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMapFormat10]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMapFormat12]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMapFormat13]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMapFormat14]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMap - Init / Destroy]
// ============================================================================

static err_t OTCMapContext_init(OTCMapContext* cctx, const OTCMap* cmap, uint32_t language);

static void FOG_CDECL OTCMap_destroy(OTCMap* self)
{
  if (self->_items)
  {
    MemMgr::free(self->_items);
    self->_items = NULL;
  }

  // This results in crash in case that destroy is called twice by accident.
  self->_destroy = NULL;
}

static err_t FOG_CDECL OTCMap_init(OTCMap* self)
{
  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  const uint8_t* data = self->getData();
  uint32_t dataLength = self->getDataLength();

#if defined(FOG_OT_DEBUG)
  Logger::info("Fog::OTCMap", "init", 
    "Initializing 'cmap' table (%u bytes).", dataLength);
#endif // FOG_OT_DEBUG

  FOG_ASSERT_X(self->_tag == FOG_OT_TAG('c', 'm', 'a', 'p'),
    "Fog::OTCMap::init() - Not a 'cmap' table.");

  self->_count = 0;
  self->_items = NULL;
  self->_destroy = (OTTableDestroyFunc)OTCMap_destroy;

  // --------------------------------------------------------------------------
  // [Header]
  // --------------------------------------------------------------------------

  const OTCMapHeader* header = self->getHeader();
  if (header->version.getRawU() != 0x0000)
  {
#if defined(FOG_OT_DEBUG)
    Logger::info("Fog::OTCMap", "init",
      "Unsupported header version %d.", header->version.getValueU());
#endif // FOG_OT_DEBUG

    return self->setStatus(ERR_FONT_CMAP_HEADER_WRONG_VERSION);
  }

  // --------------------------------------------------------------------------
  // [Encoding]
  // --------------------------------------------------------------------------

  uint32_t count = header->count.getValueU();
  if (count == 0 || count > dataLength / sizeof(OTCMapEncoding))
  {
#if defined(FOG_OT_DEBUG)
    Logger::info("Fog::OTCMap", "init",
      "Corrupted 'cmap' table, count of subtables reported to by %u.", count);
#endif // FOG_OT_DEBUG

    return self->setStatus(ERR_FONT_CMAP_HEADER_WRONG_DATA);
  }

  const OTCMapEncoding* encTable = reinterpret_cast<const OTCMapEncoding*>(
    data + sizeof(OTCMapHeader));

  OTCMapItem* items = static_cast<OTCMapItem*>(
    MemMgr::alloc(count * sizeof(OTCMapItem)));

  if (FOG_IS_NULL(items))
    return self->setStatus(ERR_RT_OUT_OF_MEMORY);

  uint32_t unicodeEncodingIndex = 0xFFFFFFFF;
  uint32_t unicodeEncodingPriority = 0xFFFFFFFF;

  uint32_t minOffsetValue = sizeof(OTCMapHeader) + count * sizeof(OTCMapEncoding);

  self->_count = count;
  self->_items = items;

  for (uint32_t i = 0; i < count; i++, items++, encTable++)
  {
    uint16_t platformId = encTable->platformId.getValueU();
    uint16_t specificId = encTable->specificId.getValueU();
    uint32_t offset     = encTable->offset.getValueU();

    // This is our encodingId which can match several platformId/specificId
    // combinations. The purpose is to simplify matching of subtables we are
    // interested it. Generally we are interested mainly of unicode tables.
    uint32_t encodingId = OT_ENCODING_ID_NONE;

    // The priority of encodingId when matching it against encodingId in tables.
    // Higher priority is set by subtables with more features. For example MS
    // UCS4 encoding has higher priority than UNICODE.
    uint32_t priority = 0;

    switch (platformId)
    {
      case OT_PLATFORM_ID_UNICODE:
        encodingId = OT_ENCODING_ID_UNICODE;
        break;

      case OT_PLATFORM_ID_MAC:
        if (specificId == OT_MAC_ID_ROMAN)
          encodingId = OT_ENCODING_ID_MAC_ROMAN;
        break;

      case OT_PLATFORM_ID_MS:
        switch (specificId)
        {
          case OT_MS_ID_SYMBOL : encodingId = OT_ENCODING_ID_MS_SYMBOL; break;
          case OT_MS_ID_UNICODE: encodingId = OT_ENCODING_ID_UNICODE  ; priority = 1; break;
          case OT_MS_ID_SJIS   : encodingId = OT_ENCODING_ID_SJIS     ; break;
          case OT_MS_ID_GB2312 : encodingId = OT_ENCODING_ID_GB2312   ; break;
          case OT_MS_ID_BIG5   : encodingId = OT_ENCODING_ID_BIG5     ; break;
          case OT_MS_ID_WANSUNG: encodingId = OT_ENCODING_ID_WANSUNG  ; break;
          case OT_MS_ID_JOHAB  : encodingId = OT_ENCODING_ID_JOHAB    ; break;
          case OT_MS_ID_UCS4   : encodingId = OT_ENCODING_ID_UNICODE  ; break;
          default: break;
        }
        break;

      default:
        // Unknown.
        break;
    }

    if ((encodingId == FOG_OT_TAG('u', 'n', 'i', 'c')) &&
        (unicodeEncodingIndex == 0xFFFFFFFF || unicodeEncodingPriority < priority))
    {
      unicodeEncodingIndex = i;
      unicodeEncodingPriority = priority;
    }

    items->encodingId = encodingId;
    items->priority = priority;
    items->status = OT_NOT_VALIDATED;

    if (offset < minOffsetValue || offset >= dataLength || (offset & 0x1) == 0x1)
    {
#if defined(FOG_OT_DEBUG)
      Logger::info("Fog::OTCMap", "init", 
        "#%02u - corrupted offset=%u", i, offset);
#endif // FOG_OT_DEBUG
      items->status = ERR_FONT_CMAP_TABLE_WRONG_DATA;
    }
    else
    {
#if defined(FOG_OT_DEBUG)
      Logger::info("Fog::OTCMap", "init", 
        "#%02u - pId=%u, sId=%u, fmt=%u, offset=%u => [enc='%c%c%c%c', priority=%u]",
        i,
        platformId,
        specificId,
        (uint32_t)(reinterpret_cast<const OTUInt16*>(data + offset)->getValueU()),
        offset,
        (encodingId >> 24) & 0xFF,
        (encodingId >> 16) & 0xFF,
        (encodingId >>  8) & 0xFF,
        (encodingId      ) & 0xFF,
        priority);
#endif // FOG_OT_DEBUG
    }
  }

  self->_unicodeEncodingIndex = unicodeEncodingIndex;
  self->_initContext = OTCMapContext_init;

  return ERR_OK;
}

// ============================================================================
// [Fog::OTCMapContext]
// ============================================================================

static err_t FOG_CDECL OTCMapContext_init(OTCMapContext* cctx, const OTCMap* cmap, uint32_t encodingId)
{
  uint32_t index = 0xFFFFFFFF;

  const OTCMapItem* items = cmap->getItems();
  uint32_t count = cmap->getCount();

  if (encodingId == FOG_OT_TAG('u', 'n', 'i', 'c'))
  {
    index = cmap->getUnicodeEncodingIndex();
  }
  else
  {
    uint32_t priority = 0xFFFFFFFF;

    for (uint32_t i = 0; i < count; i++)
    {
      if (items[i].encodingId == encodingId)
      {
        if (index == 0xFFFFFFFF || priority < items[i].priority)
        {
          index = i;
          priority = items[i].priority;
        }
      }
    }
  }

  if (index == 0xFFFFFFFF)
  {
    cctx->_getGlyphPlacementFunc = NULL;
    cctx->_data = NULL;
    return ERR_FONT_CMAP_TABLE_NOT_FOUND;
  }

  FOG_ASSERT(index < count);

  const uint8_t* data = cmap->getData();
  const OTCMapEncoding* encTable = reinterpret_cast<const OTCMapEncoding*>(
    data + sizeof(OTCMapHeader) + index * sizeof(OTCMapEncoding));

  uint32_t offset = encTable->offset.getValueU();
  FOG_ASSERT(offset < cmap->getDataLength());

  cctx->_data = data + offset;

  // Format of the table is the first UInt16 data entry in that offset.
  uint16_t format = reinterpret_cast<const OTUInt16*>(data + offset)->getValueU();
  switch (format)
  {
    case 4:
      if (OTUtil::initAligned16(cctx->_data))
        cctx->_getGlyphPlacementFunc = OTCMapContext4_getGlyphPlacement<true>;
      else
        cctx->_getGlyphPlacementFunc = OTCMapContext4_getGlyphPlacement<false>;
      return ERR_OK;

    case 6:
      if (OTUtil::initAligned16(cctx->_data))
        cctx->_getGlyphPlacementFunc = OTCMapContext6_getGlyphPlacement<true>;
      else
        cctx->_getGlyphPlacementFunc = OTCMapContext6_getGlyphPlacement<false>;
      return ERR_OK;

    default:
      cctx->_data = NULL;
      cctx->_getGlyphPlacementFunc = NULL;
      return ERR_FONT_CMAP_TABLE_WRONG_FORMAT;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OTCMap_init(void)
{
  OTApi& api = fog_ot_api;

  // --------------------------------------------------------------------------
  // [OTCMap]
  // --------------------------------------------------------------------------
  
  api.otcmap_init = OTCMap_init;
}

} // Fog namespace
