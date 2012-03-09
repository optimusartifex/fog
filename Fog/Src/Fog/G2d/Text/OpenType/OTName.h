// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OPENTYPE_OTNAME_H
#define _FOG_G2D_TEXT_OPENTYPE_OTNAME_H

// [Dependencies]
#include <Fog/Core/Memory/MemZoneAllocator.h>
#include <Fog/G2d/Text/OpenType/OTApi.h>
#include <Fog/G2d/Text/OpenType/OTTypes.h>

namespace Fog {

// [Byte-Pack]
#include <Fog/Core/C++/PackByte.h>

//! @addtogroup Fog_G2d_Text_OpenType
//! @{

// ============================================================================
// [Fog::OTNameRecord]
// ============================================================================

//! @brief TrueType/OpenType 'name' - Name record.
//!
//! Following are the descriptions of the four kinds of ID. The specific values
//! listed here are predefined; new ones may be added in the future. Similar to
//! the character encoding table, the @ref OTNameRecord(s) are sorted by 
//! @c platformId, then @c specificId, then @c languageId, and then by @c nameId.
struct FOG_NO_EXPORT OTNameRecord
{
  //! @brief Platform id.
  OTUInt16 platformId;
  //! @brief Specific id (platform-specific id, or encoding id by MS).
  OTUInt16 specificId;
  //! @brief Language id.
  //!
  //! Language IDs refer to a value that identifies the language in which a 
  //! particular string is written. Values less than 0x8000 are defined on a 
  //! platform-specific basis. Values greater than or equal to 0x8000 are 
  //! associated with language-tag records, as described above. Not all platforms
  //! have platform-specific language IDs, and not all platforms support 
  //! language-tag records.
  OTUInt16 languageId;
  //! @brief Name id.
  OTUInt16 nameId;
  //! @brief String length in bytes.
  OTUInt16 length;
  //! @brief String offset from start of storage area in bytes.
  OTUInt16 offset;
};

// ============================================================================
// [Fog::OTNameLangTagRecord]
// ============================================================================

//! @brief TrueType/OpenType 'name' - Language tag record.
//!
//! Language-tag strings stored in the Naming table must be encoded in UTF-16BE.
//! The language tags must conform to IETF specification BCP 47. This provides
//! tags such as "en", "fr-CA" and "zh-Hant" to identify languages, including
//! dialects, written form and other variations.
//!
//! The language-tag records are associated sequentially with language IDs 
//! starting with 0x8000. Each language-tag record corresponds to a language ID
//! one greater than that for the previous language-tag record. Thus, language
//! IDs associated with language-tag records must be within the range 0x8000 to 
//! 0x8000 + langTagCount - 1. If a name record uses a language ID that is 
//! greater than this, the identify of the language is unknown; such name records
//! should not be used.
//! 
//! For example, suppose a font has two language-tag records referencing strings 
//! in the storage: the first references the string "en", and the second 
//! references the string "zh-Hant-HK" In this case, the language ID 0x8000 is 
//! used in name records to index English-language strings. The language ID 
//! 0x8001 is used in name records to index strings in Traditional Chinese as 
//! used in Hong Kong.
struct FOG_NO_EXPORT OTNameLangTagRecord
{
  //! @brief Language-tag string length (in bytes).
  OTUInt16 length;
  //! @brief Language-tag string offset from start of storage area (in bytes).
  OTUInt16 offset;
};

// ============================================================================
// [Fog::OTNameHeader]
// ============================================================================

//! @brief TrueType/OpenType 'name' - Naming header.
struct FOG_NO_EXPORT OTNameHeader
{
  //! @brief Format (supported formats are 0 and 1).
  OTUInt16 format;
  //! @brief Number of name records.
  OTUInt16 count;
  //! @brief Offset to start of string storage.
  OTUInt16 stringOffset;
};


// ============================================================================
// [Fog::OTName]
// ============================================================================

//! @brief TrueType/OpenType 'name' - Naming table.
//!
//! Specification:
//!   - http://www.microsoft.com/typography/otspec/name.htm
//!   - https://developer.apple.com/fonts/ttrefman/RM06/Chap6name.html
struct FOG_NO_EXPORT OTName : public OTTable
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const OTNameHeader* getHeader() const { return reinterpret_cast<OTNameHeader*>(_data); }
};

//! @}

// [Byte-Pack]
#include <Fog/Core/C++/PackRestore.h>

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OPENTYPE_OTNAME_H
