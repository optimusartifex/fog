// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OPENTYPE_OTENUM_H
#define _FOG_G2D_TEXT_OPENTYPE_OTENUM_H

// [Dependencies]
#include <Fog/G2d/Text/OpenType/OTAPI.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text_OpenType
//! @{

// ============================================================================
// [Fog::OT_NOT_VALIDATED]
// ============================================================================

enum 
{
  OT_NOT_VALIDATED = 0xFFFFFFFFU
};

// ============================================================================
// [Fog::OT_HEAD_FLAG]
// ============================================================================

//! @brief Flags used by OTHeadHeader::flags.
enum OT_HEAD_FLAG
{
  //! @brief Baseline for font at y=0.
  OT_HEAD_FLAG_BASELINE_FOR_Y = 0x0001,
  //! @brief Left sidebearing point at x=0.
  OT_HEAD_FLAG_LEFT_SIDEBEARING_AT_X = 0x0002,
  //! @brief Instructions may depend on point size.
  OT_HEAD_FLAG_INST_MAY_DEPEND_ON_SIZE = 0x0004,
  //! @brief Force ppem to integer values for all internal scaler math; may use
  //! fractional ppem sizes if this bit is clear.
  OT_HEAD_FLAG_FORCE_PPEM_TO_INTEGER = 0x0008,
  //! @brief  Instructions may alter advance width (the advance widths might not
  //! scale linearly).
  OT_HEAD_FLAG_INST_MAY_ALTER_ADVANCE = 0x0010,
  //! @brief Should be set in fonts that are intended to be laid out vertically,
  //! and in which the glyphs have been drawn such that an x-coordinate of 0
  //! corresponds to the desired vertical baseline.
  OT_HEAD_FLAG_VERTICAL_LAYOUT = 0x0020,
  //! @brief Should be set if the font requires layout for correct linguistic
  //! rendering (Arabic fonts).
  OT_HEAD_FLAG_REQUIRE_LAYOUT = 0x0080,
  //! @brief This bit should be set for a GX font which has one or more 
  //! metamorphosis effects designated as happening by default.
  OT_HEAD_FLAG_GX_METAMORPHOSIS = 0x0100,
  //! @brief This bit should be set if the font contains any strong right-to-left
  //! glyphs.
  OT_HEAD_FLAG_SUPPORT_RTL = 0x0200,
  //! @brief This bit should be set if the font contains Indic-style 
  //! rearrangement effects
  OT_HEAD_FLAG_SUPPORT_INDIC = 0x0400,
  //! @brief Font data is 'lossless,' as a result of having been compressed and
  //! decompressed with the Agfa MicroType Express engine.
  OT_HEAD_FLAG_LOSSLESS = 0x0800,
  //! @brief Font converted (produce compatible metrics).
  OT_HEAD_FLAG_CONVERTED = 0x1000,
  //! @brief Font optimized for ClearType.
  //!
  //! @note Fonts that rely on embedded bitmaps (EBDT) for rendering should not 
  //! be considered optimized for ClearType, and therefore should keep this bit
  //! cleared.
  OT_HEAD_FLAG_CLEARTYPE = 0x2000,
  //! @brief Last Resort font.
  //!
  //! If set, indicates that the glyphs encoded in the cmap subtables are simply
  //! generic symbolic representations of code point ranges and don’t truly
  //! represent support for those code points. If unset, indicates that the glyphs
  //! encoded in the cmap subtables represent proper support for those code points.
  OT_HEAD_FLAG_LAST_RESORT_FONT = 0x4000
};

// ============================================================================
// [Fog::OT_HEAD_MAC_STYLE]
// ============================================================================

//! @brief Flags used by OTHeadHeader::macStyle.
enum OT_HEAD_MAC_STYLE
{
  //! @brief Italic.
  OT_HEAD_MAC_STYLE_ITALIC = 0x0002,
  //! @brief Underline.
  OT_HEAD_MAC_STYLE_UNDERLINE = 0x0004,
  //! @brief Outline.
  OT_HEAD_MAC_STYLE_OUTLINE = 0x0008,
  //! @brief Shadow.
  OT_HEAD_MAC_STYLE_SHADOW = 0x0010,
  //! @brief Condensed.
  OT_HEAD_MAC_STYLE_CONDENSED = 0x0020,
  //! @brief Extended.
  OT_HEAD_MAC_STYLE_EXTENDED = 0x0040
};

// ============================================================================
// [Fog::OT_HEAD_INDEX_TO_LOC]
// ============================================================================

//! @brief Flags used by OTHeadHeader::indexToLocFormat.
enum OT_HEAD_INDEX_TO_LOC
{
  OT_HEAD_INDEX_TO_LOC_SHORT = 0,
  OT_HEAD_INDEX_TO_LOC_LONG = 1
};

// ============================================================================
// [Fog::OT_PLATFORM_ID]
// ============================================================================

//! @brief TrueType/OpenType platform ID.
enum OT_PLATFORM_ID
{
  //! @brief Unicode platform.
  //!
  //! @note When used, OTCMapEncoding::specificId indicates unicode version.
  OT_PLATFORM_ID_UNICODE = 0,

  //! @brief Mac platform.
  //!
  //! @note When used, OTCMapEncoding::specificId means mac script-manager code, see
  //! @ref OT_MAC_ID enumeration for possible values.
  OT_PLATFORM_ID_MAC = 1,

  //! @brief Deprecated, originally used by ISO/IEC 10646.
  OT_PLATFORM_ID_RESERVED = 2,

  //! @brief MS platform.
  //!
  //! @note When used, OTCMapEncoding::specificId indicates microsoft encoding.
  OT_PLATFORM_ID_MS = 3
};

// ============================================================================
// [Fog::OT_UNICODE_ID]
// ============================================================================

enum OT_UNICODE_ID
{
  //! @brief Default semantics.
  OT_UNICODE_ID_DEFAULT = 0,

  //! @brief Unicode version 1.1 semantics.
  OT_UNICODE_ID_V1_1 = 1,

  //! @brief Deprecated, used by ISO 10646 1993 semantics.
  OT_UNICODE_ID_RESERVED = 2,

  //! @brief Unicode version 2.0 or later semantics.
  OT_UNICODE_ID_V2_0 = 3,

  //! @brief Unicode version 3.2 or later semantics.
  OT_UNICODE_ID_V3_2 = 4
};

// ============================================================================
// [Fog::OT_MAC_ID]
// ============================================================================

enum OT_MAC_ID
{
  OT_MAC_ID_ANY = 0,

  OT_MAC_ID_ROMAN = 0,
  OT_MAC_ID_JAPANESE = 1,
  OT_MAC_ID_TRADITIONAL_CHINESE = 2,
  OT_MAC_ID_KOREAN = 3,
  OT_MAC_ID_ARABIC = 4,
  OT_MAC_ID_HEBREW = 5,
  OT_MAC_ID_GREEK = 6,
  OT_MAC_ID_RUSSIAN = 7,
  OT_MAC_ID_RSYMBOL = 8,
  OT_MAC_ID_DEVANAGARI = 9,
  OT_MAC_ID_GURMUKHI = 10,
  OT_MAC_ID_GUJARATI = 11,
  OT_MAC_ID_ORIYA = 12,
  OT_MAC_ID_BENGALI = 13,
  OT_MAC_ID_TAMIL = 14,
  OT_MAC_ID_TELUGU = 15,
  OT_MAC_ID_KANNADA = 16,
  OT_MAC_ID_MALAYALAM = 17,
  OT_MAC_ID_SINHALESE = 18,
  OT_MAC_ID_BURMESE = 19,
  OT_MAC_ID_KHMER = 20,
  OT_MAC_ID_THAI = 21,
  OT_MAC_ID_LAOTIAN = 22,
  OT_MAC_ID_GEORGIAN = 23,
  OT_MAC_ID_ARMENIAN = 24,
  OT_MAC_ID_SIMPLIFIED_CHINESE = 25,
  OT_MAC_ID_TIBETAN = 26,
  OT_MAC_ID_MONGOLIAN = 27,
  OT_MAC_ID_GEEZ = 28,
  OT_MAC_ID_SLAVIC = 29,
  OT_MAC_ID_VIETNAMESE = 30,
  OT_MAC_ID_SINDHI = 31,
  OT_MAC_ID_UNINTERPRETED = 32
};

// ============================================================================
// [Fog::OT_MS_ID]
// ============================================================================

enum OT_MS_ID
{
  //! @brief Microsoft symbol encoding.
  OT_MS_ID_SYMBOL = 0,
  //! @brief Microsoft WGL4 charmap, matching unicode.
  OT_MS_ID_UNICODE = 1,
  //! @brief SJIS Japanese encoding.
  OT_MS_ID_SJIS = 2,
  //! @brief Simplified Chinese encoding.
  OT_MS_ID_GB2312 = 3,
  //! @brief Traditional Chinese encoding.
  OT_MS_ID_BIG5 = 4,
  //! @brief Korean Wansung encoding.
  OT_MS_ID_WANSUNG = 5,
  //! @brief Johab encoding.
  OT_MS_ID_JOHAB = 6,
  //! @brief UCS-4 charmaps.
  OT_MS_ID_UCS4 = 10
};

// ============================================================================
// [Fog::OT_ENCODING_ID]
// ============================================================================

enum OT_ENCODING_ID
{
  // NOTE: Tag values are designed to be compatible with FreeType library, but
  // the table can be extended to support more values than FreeType can.

  OT_ENCODING_ID_NONE = FOG_OT_TAG_NONE,
  OT_ENCODING_ID_UNICODE = FOG_OT_TAG('u', 'n', 'i', 'c'),

  OT_ENCODING_ID_SJIS = FOG_OT_TAG('s', 'j', 'i', 's'),
  OT_ENCODING_ID_GB2312 = FOG_OT_TAG('g', 'b', ' ', ' '),
  OT_ENCODING_ID_BIG5 = FOG_OT_TAG('b', 'i', 'g', '5'),
  OT_ENCODING_ID_WANSUNG = FOG_OT_TAG('w', 'a', 'n', 's'),
  OT_ENCODING_ID_JOHAB = FOG_OT_TAG('j', 'o', 'h', 'a'),

  OT_ENCODING_ID_ADOBE_LATIN1 = FOG_OT_TAG('l', 'a', 't', '1'),
  OT_ENCODING_ID_ADOBE_STANDARD = FOG_OT_TAG('A', 'D', 'O', 'B'),
  OT_ENCODING_ID_ADOBE_EXPERT = FOG_OT_TAG('A', 'D', 'B', 'E'),
  OT_ENCODING_ID_ADOBE_CUSTOM = FOG_OT_TAG('A', 'D', 'B', 'C'),

  OT_ENCODING_ID_MS_SYMBOL  = FOG_OT_TAG('s', 'y', 'm', 'b'),
  OT_ENCODING_ID_MAC_ROMAN = FOG_OT_TAG('a', 'r', 'm', 'n')
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OPENTYPE_OTENUM_H
