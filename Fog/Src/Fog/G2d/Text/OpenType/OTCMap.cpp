// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/G2d/Text/OpenType/OTCMap.h>

namespace Fog {

// [Byte-Pack]
#include <Fog/Core/C++/PackByte.h>

// ============================================================================
// [Fog::OTCMapIndex]
// ============================================================================

//! @brief 'cmap' index table.
struct FOG_NO_EXPORT OTCMapIndex
{
  //! @brief Version of 'cmap' header.
  OTUInt16 version;
  //! @brief Count of 'cmap' (sub)tables.
  OTUInt16 count;
};

// ============================================================================
// [Fog::OTCMapEncoding]
// ============================================================================

struct FOG_NO_EXPORT OTCMapEncoding
{
  //! @brief Platform identifier.
  OTUInt16 platformID;
  //! @brief Plaform-specific encoding identifier.
  OTUInt16 platformSpecificID;
  //! @brief Offset of the mapping table.
  OTUInt32 offset;
};

// ============================================================================
// [Fog::OTCMapFormat0]
// ============================================================================

// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt16       | 0            | format               | Format number is set to 0.
// UInt16       | 2            | length               | Table length in bytes.
// UInt16       | 4            | MacLanguageCode      | Mac language code.
// UInt8[256]   | 6            | glyphIdArray         | Glyph index array (256).

// ============================================================================
// [Fog::OTCMapFormat2]
// ============================================================================

// ============================================================================
// [Fog::OTCMapFormat4]
// ============================================================================

// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt16       | 0            | format               | Format number is set to 4.
// UInt16       | 2            | length               | Table length in bytes.
// UInt16       | 4            | MacLanguageCode      | Mac language code.
// UInt16       | 6            | nSegs*2 (N)          | Number of segments * 2.
// UInt16       | 8            | searchRange          | (2^floor(log2(N))) * 2   == 2*1(LogSegs).
// UInt16       | 10           | entrySelector        | log2(searchRange / 2)    == LogSegs.
// UInt16       | 12           | rangeShift           | N*2 - searchRange.
// UInt16[N]    | 14           | endCount             | End characterCode for each segment, last=0xFFFF.
// UInt16       | 14+N*2       | reservedPad          | Padding, set to 0.
// UInt16[N]    | 16+N*2       | startCount           | Start character code for each segment.
// UInt16[N]    | 16+N*4       | idDelta              | Delta for all character codes in segment.
// UInt16[N]    | 16+N*6       | idRangeOffset        | Offsets into glyphIdArray or 0.
// UInt16[]     | 16+N*8       | glyphIdArray         | Glyph index array (arbitrary length).

// ============================================================================
// [Fog::OTCMapFormat6]
// ============================================================================

// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt16       | 0            | format               | Format number is set to 6.
// UInt16       | 2            | length               | Table length in bytes.
// UInt16       | 4            | MacLanguageCode      | Mac language code.
// UInt16       | 6            | first                | First segment code.
// UInt16       | 8            | count (N)            | Segment size in bytes.
// UInt16[N]    | 10           | glyphIdArray         | Glyph index array.

// ============================================================================
// [Fog::OTCMapFormat8]
// ============================================================================

// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt16       | 0            | format               | Format number is set to 8.
// UInt16       | 2            | reserved             | Reserved.
// UInt32       | 4            | length               | Table length in bytes.
// UInt32       | 8            | macLanguageCode      | Mac language code.
// UInt8[8192]  | 12           | is32                 | Tightly packed array of bits indicating
//              |              |                      | whether the particular 16-bit (index) value
//              |              |                      | is the start of a 32-bit character code.
// UInt32       | 8204         | count (N)            | Number of groupings which follow.
//
// The header is followed by N groups in the following format:
//
// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt32       | 0            | startChar            | Start character code.
// UInt32       | 4            | lastChar             | Last character code.
// UInt32       | 8            | startGlyph           | Start glyph ID for the group.

// ============================================================================
// [Fog::OTCMapFormat10]
// ============================================================================

// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt16       | 0            | format               | Format number is set to 10.
// UInt16       | 2            | reserved             | Reserved.
// UInt32       | 4            | length               | Table length in bytes.
// UInt32       | 8            | macLanguageCode      | Mac language code.
// UInt32       | 12           | start                | First character in range.
// UInt32       | 16           | count (N)            | Number of characters in range.
// UInt16[N]    | 20           | glyphIdArray         | Glyph index array.

// ============================================================================
// [Fog::OTCMapFormat12]
// ============================================================================

// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt16       | 0            | format               | Format number is set to 12.
// UInt16       | 2            | reserved             | Reserved.
// UInt32       | 4            | length               | Table length in bytes.
// UInt32       | 8            | macLanguageCode      | Mac language code.
// UInt32       | 12           | count (N)            | Number of groups.
//
// The header is followed by N groups in the following format:
//
// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt32       | 0            | start                | First character code.
// UInt32       | 4            | end                  | Last character code.
// UInt32       | 8            | startId              | Start glyph ID for the group.

// ============================================================================
// [Fog::OTCMapFormat13]
// ============================================================================

// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt16       | 0            | format               | Format number is set to 13.
// UInt16       | 2            | reserved             | Reserved.
// UInt32       | 4            | length               | Table length in bytes.
// UInt32       | 8            | macLanguageCode      | Mac language code.
// UInt32       | 12           | count (N)            | Number of groups.
//
// The header is followed by N groups in the following format:
//
// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt32       | 0            | start                | First character code.
// UInt32       | 4            | end                  | Last character code.
// UInt32       | 8            | glyphId              | Glyph ID for the whole group.

// ============================================================================
// [Fog::OTCMapFormat14]
// ============================================================================

// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt16       | 0            | format               | Format number is set to 14.
// UInt32       | 2            | length               | Table length in bytes.
// UInt32       | 8            | numSelector (N)      | Number of variation selection records.
//
// The header is followed by N selection records in the following format:
//
// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt24       | 0            | varSelector          | Unicode code-point of selection.
// UInt32       | 3            | defaultOffset        | Offset to a default UVS table.
// UInt32       | 7            | nonDefaultOffset     | Offset to a non-default UVS table.
//
// Selectors are sorted by code-point.
//
// A default Unicode Variation Selector (UVS) subtable is just a list of ranges
// of code-points which are to be found in the standard cmap. No glyph IDs here:
//
// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt32       | 0            | numRanges            | Number of ranges following.
// -------------+--------------+----------------------+------------------------
// UInt24       | 0            | uniStart             | First code-point in range.
// UInt8        | 3            | additionalCount      | Count of additional characters (can be zero).
//
// Ranges are sorted by uniStart:
//
// Type         | Offset       | Name                 | Description
// -------------+--------------+----------------------+------------------------
// UInt32       | 0            | numMappings          | Number of mappings.
// -------------+--------------+----------------------+------------------------
// UInt24       | 0            | uniStart             | First code-point in range.
// UInt16       | 3            | glyphID              | Glyph ID of the first character.

// [Byte-Pack]
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::OTCMapTable]
// ============================================================================

static err_t FOG_CDECL OTCMapTable_init(OTCMapTable* self)
{
  FOG_ASSERT_X(self->_tag == FOG_OT_TAG('c', 'm', 'a', 'p'),
    "Fog::OTCMapTable::init() - Called on invalid table.");

  

  return ERR_OK;
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
  
  api.otcmaptable_init = OTCMapTable_init;
}

} // Fog namespace
