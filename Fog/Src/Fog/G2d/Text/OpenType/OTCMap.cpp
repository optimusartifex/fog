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
// [Fog::OTCMap0]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMap2]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMap4 - Validate]
// ============================================================================

template<bool IsAligned>
static err_t OTCMap4_validate(OTCMapItem* item, const uint8_t* data, uint32_t dataLength)
{
  // Minimum length of 'cmap' table format 4 is 16 + 8.
  static const uint32_t minLength = 16 + 8;
 
  if (dataLength < minLength)
    return ERR_FONT_CMAP_TABLE_WRONG_DATA;

  const OTCMapFormat4Header* head = reinterpret_cast<const OTCMapFormat4Header*>(data);
  FOG_ASSERT(head->format.getValueT<IsAligned>() == 4);

  uint32_t length = head->length.getValueT<IsAligned>();
  if (length < minLength || length > dataLength)
    return ERR_FONT_CMAP_TABLE_WRONG_LENGTH;

  uint32_t numSegX2      = head->numSegX2.getValueT<IsAligned>();
  uint32_t searchRange   = head->searchRange.getValueT<IsAligned>();
  uint32_t entrySelector = head->entrySelector.getValueT<IsAligned>();
  uint32_t rangeShift    = head->rangeShift.getValueT<IsAligned>();

  // Validate 'numSegX2' - Can't be odd and there must be at least one segment.
  if ((numSegX2 & 0x1) == 0x1 || numSegX2 < 2)
    return ERR_FONT_CMAP_TABLE_WRONG_DATA;

  uint32_t numSeg = numSegX2 / 2;
  uint32_t numLog2Seg = 0;

  while (1U << (numLog2Seg + 1) <= numSeg)
    numLog2Seg++;

  // Validate 'searchRange' - (2^floor(log2(N))) * 2.
  if (searchRange != (1U << numLog2Seg) * 2)
    return ERR_FONT_CMAP_TABLE_WRONG_DATA;

  // Validate 'entrySelector'.
  if (entrySelector != numLog2Seg)
    return ERR_FONT_CMAP_TABLE_WRONG_DATA;

  // Validate 'rangeShift'.
  if (rangeShift != numSegX2 - searchRange)
    return ERR_FONT_CMAP_TABLE_WRONG_DATA;

  // Validate table length against numSeg.
  if (length < 16 + numSeg * 8)
    return ERR_FONT_CMAP_TABLE_WRONG_DATA;

  const OTUInt16* pEnd    = reinterpret_cast<const OTUInt16*>(data + 14);
  const OTUInt16* pStart  = reinterpret_cast<const OTUInt16*>(data + 16 + numSeg * 2);
  const OTUInt16* pDelta  = reinterpret_cast<const OTUInt16*>(data + 16 + numSeg * 4);
  const OTUInt16* pOffset = reinterpret_cast<const OTUInt16*>(data + 16 + numSeg * 6);

  uint32_t i;
  uint32_t previousEnd = 0;
  uint32_t numSegToBSearch = 0;

  for (i = 0; i < numSeg; i++)
  {
    uint32_t start  = pStart [i].getValueT<IsAligned>();
    uint32_t end    = pEnd   [i].getValueT<IsAligned>();
    uint32_t delta  = pDelta [i].getValueT<IsAligned>();
    uint32_t offset = pOffset[i].getValueT<IsAligned>();

    // Logger::info("Fog::OTCMap4", "validate",
    //   "start=%d, end=%d, delta=%d, offset=%d\n", start, end, delta, offset);

    if (start > end)
      return ERR_FONT_CMAP_TABLE_WRONG_GROUP;

    if (i != 0 && start <= previousEnd)
    {
      // Some fonts contain multiple ending marks. According to specification
      // it's not allowed, but we simply pass, because this can't cause error
      // in our 'getGlyphPlacement' implementation.
      if (start == 0xFFFF && end == 0xFFFF && previousEnd == 0xFFFF)
      {
#if defined(FOG_OT_DEBUG)
        Logger::warning("Fog::OTCMap4", "validate",
          "Detected multiple ending marks, it's against specification.", i);
#endif // FOG_OT_DEBUG
      }
      else
      {
        return ERR_FONT_CMAP_TABLE_WRONG_GROUP;
      }
    }

    if (start == 0xFFFF && end == 0xFFFF)
    {
      // If this is the first ending mark then we set the number of segments
      // for binary-search to skip this and all additional ending marks. I
      // think that this is Fog-Framework specific, becuase 'getGlyphPlacement'
      // doesn't use 'searchRange', 'entrySelector', and 'rangeShift' members
      // stored in cmap format-4 header.
      if (numSegToBSearch == 0)
        numSegToBSearch = i;
    }
    else
    {
      if (offset != 0)
      {
        // Offset to 16-bit data is always even.
        if ((offset & 0x1) == 0x1)
          return ERR_FONT_CMAP_TABLE_WRONG_DATA;

        // We don't need to validate this table in char-to-char basis, we just
        // check whether the last character mapping is within the data range.
        uint32_t numChars = end - start + 1;
        uint32_t indexInTable = 16 + numSeg * 6 + offset + numChars * 2;

        if (indexInTable > length)
          return ERR_FONT_CMAP_TABLE_WRONG_DATA;
      }
    }
  }

  item->specificData = numSegToBSearch;
  return ERR_OK;
}

// ============================================================================
// [Fog::OTCMap4 - BSearch]
// ============================================================================

template<bool IsAligned>
static FOG_INLINE const uint8_t* OTCMap4_bsearch(
  uint32_t& start,
  uint32_t& end,
  const uint8_t* pEnd,
  uint32_t numSegToBSearch,
  uint32_t numSegTotal,
  uint32_t uc)
{
  uint32_t base = 0;

  for (uint32_t lim = numSegToBSearch; lim != 0; lim >>= 1)
  {
    const uint8_t* pCur = pEnd + (lim & ~1);

    end = FOG_OT_UINT16(pCur)->getValueU();
    if (end < uc)
    {
      pEnd = pCur + 2;
      lim--;
      continue;
    }

    start = FOG_OT_UINT16(pCur + 2 + numSegTotal * 2)->getValueU();
    if (start <= uc)
      return pCur;
  }

  return NULL;
}

// ============================================================================
// [Fog::OTCMap4 - GetGlyphPlacement]
// ============================================================================

template<bool IsAligned>
static size_t FOG_CDECL OTCMap4_getGlyphPlacement(OTCMapContext* cctx,
  uint32_t* glyphList, size_t glyphAdvance, const uint16_t* sData, size_t sLength)
{
  if (sLength == 0)
    return 0;

  const uint8_t* data = static_cast<const uint8_t*>(cctx->_data);
  const OTCMapFormat4Header* head = reinterpret_cast<const OTCMapFormat4Header*>(data);

  FOG_ASSERT(head->format.getValueT<IsAligned>() == 4);

  uint32_t length = head->length.getValueT<IsAligned>();
  uint32_t numSeg = head->numSegX2.getValueT<IsAligned>() >> 1;
  uint32_t numSegToBSearch = cctx->_item->specificData;

  const uint16_t* sMark = sData;
  const uint16_t* sEnd = sData + sLength;

  uint32_t uc = sData[0];
  for (;;)
  {
    uint32_t start, end, offset;
    const uint8_t* pEnd;
    const uint8_t* pOffset;

    if (CharW::isSurrogate(uc))
      goto _MissingGlyph;

    pEnd = OTCMap4_bsearch<IsAligned>(start, end, data + 14, numSegToBSearch, numSeg, uc);
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

    // We are processing a string, so if characters are close to each other we
    // can try to skip a binary search if the next character is in the same range
    // list as the previous character. This optimization is always benefical for
    // latin text and for large range runs.
    uc = sData[0];

    if (uc >= start && uc <= end)
      goto _Repeat;
    continue;

_MissingGlyph:
    glyphList[0] = 0;
    glyphList = reinterpret_cast<uint32_t*>((uint8_t*)glyphList + glyphAdvance);

    if (++sData == sEnd)
      break;
    uc = sData[0];
  }

  return (size_t)(sData - sMark);
}

// ============================================================================
// [Fog::OTCMap6]
// ============================================================================

template<bool IsAligned>
static size_t FOG_CDECL OTCMap6_getGlyphPlacement(OTCMapContext* cctx,
  uint32_t* glyphList, size_t glyphAdvance, const uint16_t* sData, size_t sLength)
{
  if (sLength == 0)
    return 0;

  const uint8_t* data = static_cast<const uint8_t*>(cctx->_data);
  const OTCMapFormat6Header* head = reinterpret_cast<const OTCMapFormat6Header*>(data);

  FOG_ASSERT(head->format.getValueT<IsAligned>() == 6);

  uint32_t first = head->first.getValueT<IsAligned>();
  uint32_t count = head->count.getValueT<IsAligned>();

  const OTUInt16* glyphIdArray = head->glyphIdArray;
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
// [Fog::OTCMap8]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMap10]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMap12]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMap13]
// ============================================================================

// TODO:

// ============================================================================
// [Fog::OTCMap14]
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

    uint32_t format;
    uint32_t status = ERR_OK;

    // ------------------------------------------------------------------------
    // [EncodingId / Priority]
    // ------------------------------------------------------------------------

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

    items->encodingId = encodingId;
    items->priority = priority;
    items->specificData = 0;

    // ------------------------------------------------------------------------
    // [Validation]
    // ------------------------------------------------------------------------

    if (offset < minOffsetValue || offset >= dataLength || (offset & 0x1) == 0x1)
    {
#if defined(FOG_OT_DEBUG)
      Logger::info("Fog::OTCMap", "init", 
        "#%02u - Inconsistency - Offset=%u.", i, offset);
#endif // FOG_OT_DEBUG

      items->status = ERR_FONT_CMAP_TABLE_WRONG_DATA;
      continue;
    }

    format = (uint32_t)(reinterpret_cast<const OTUInt16*>(data + offset)->getValueU());
    switch (format)
    {
      case 0:
        // TODO: OpenType 'cmap'.
        break;

      case 2:
        // TODO: OpenType 'cmap'.
        break;

      case 4:
        if (OTUtil::initAligned16(data + offset))
          status = OTCMap4_validate<true>(items, data + offset, dataLength - offset);
        else
          status = OTCMap4_validate<false>(items, data + offset, dataLength - offset);
        break;

      case 6:
        // TODO: OpenType 'cmap'.
        break;

      case 8:
        // TODO: OpenType 'cmap'.
        break;

      case 10:
        // TODO: OpenType 'cmap'.
        break;

      case 12:
        // TODO: OpenType 'cmap'.
        break;

      case 13:
        // TODO: OpenType 'cmap'.
        break;

      case 14:
        // TODO: OpenType 'cmap'.
        break;

      default:
#if defined(FOG_OT_DEBUG)
        Logger::info("Fog::OTCMap", "init", 
          "#%02u - Inconsistency - Format=%u.", i, format);
#endif // FOG_OT_DEBUG

        items->status = ERR_FONT_CMAP_TABLE_WRONG_DATA;
        continue;
    }

    items->status = status;

#if defined(FOG_OT_DEBUG)
    Logger::info("Fog::OTCMap", "init", 
      "#%02u - Okay - pId=%u, sId=%u, Format=%u, Offset=%u => [Encoding='%c%c%c%c', Priority=%u].", i,
      platformId,
      specificId,
      format,
      offset,
      encodingId ? (encodingId >> 24) & 0xFF : ' ',
      encodingId ? (encodingId >> 16) & 0xFF : ' ',
      encodingId ? (encodingId >>  8) & 0xFF : ' ',
      encodingId ? (encodingId      ) & 0xFF : ' ',
      priority);
#endif // FOG_OT_DEBUG

    if (status != ERR_OK)
    {
#if defined(FOG_OT_DEBUG)
      Logger::info("Fog::OTCMap", "init",
        "#%02u - Inconsistency - Validation error.", i);
#endif // FOG_OT_DEBUG
    }
    else
    {
      // Only set preferred unicode encoding if the table has been validated.
      if ((encodingId == FOG_OT_TAG('u', 'n', 'i', 'c')) &&
          (unicodeEncodingIndex == 0xFFFFFFFF || unicodeEncodingPriority < priority))
      {
        unicodeEncodingIndex = i;
        unicodeEncodingPriority = priority;
      }
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
  cctx->_item = &items[index];

  // Format of the table is the first UInt16 data entry in that offset.
  uint16_t format = reinterpret_cast<const OTUInt16*>(data + offset)->getValueU();
  switch (format)
  {
/*
    case 0:
      // TODO: OpenType 'cmap'.
      break;

    case 2:
      // TODO: OpenType 'cmap'.
      break;
*/
    case 4:
      if (OTUtil::initAligned16(cctx->_data))
        cctx->_getGlyphPlacementFunc = OTCMap4_getGlyphPlacement<true>;
      else
        cctx->_getGlyphPlacementFunc = OTCMap4_getGlyphPlacement<false>;
      return ERR_OK;

    case 6:
      if (OTUtil::initAligned16(cctx->_data))
        cctx->_getGlyphPlacementFunc = OTCMap6_getGlyphPlacement<true>;
      else
        cctx->_getGlyphPlacementFunc = OTCMap6_getGlyphPlacement<false>;
      return ERR_OK;
/*
    case 8:
      // TODO: OpenType 'cmap'.
      break;

    case 10:
      // TODO: OpenType 'cmap'.
      break;

    case 12:
      // TODO: OpenType 'cmap'.
      break;

    case 13:
      // TODO: OpenType 'cmap'.
      break;

    case 14:
      // TODO: OpenType 'cmap'.
      break;
*/
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
