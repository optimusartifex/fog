// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_CONSTANTS_H
#define _FOG_CORE_GLOBAL_CONSTANTS_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Fog::DETECT_LENGTH / INVALID_INDEX]
// ============================================================================

//! @brief Detect length means that string function should first detect it length.
static const size_t DETECT_LENGTH = SYSUINT_MAX;
//! @brief Returned by functions like indexOf() to indicate that element wasn't
//! found.
static const size_t INVALID_INDEX = SYSUINT_MAX;

static const size_t NO_PRECISION = SYSUINT_MAX;
static const size_t NO_WIDTH     = SYSUINT_MAX;

//! @brief how much to reserve stack for local strings in Fog library or all
//! additional libraries.
static const size_t TEMPORARY_LENGTH = 256;

// ============================================================================
// [Fog::NO_FLAGS_ENUM]
// ============================================================================

//! @brief Convenience value that can be used instead of zero.
enum NO_FLAGS_ENUM
{
  NO_FLAGS = 0
};

// ============================================================================
// [Fog::ALPHA_DISTRIBUTION]
// ============================================================================

//! @brief Distribution of alpha channel values in an image or in a color table.
enum ALPHA_DISTRIBUTION
{
  //! @brief Alpha values are 0.
  ALPHA_DISTRIBUTION_ZERO = 0,
  //! @brief Alpha values are 1.
  ALPHA_DISTRIBUTION_FULL = 1,
  //! @brief Alpha values are 0 (transparent) or 1 (opaque).
  ALPHA_DISTRIBUTION_ZERO_OR_FULL = 2,
  //! @brief Alpha values are variant.
  ALPHA_DISTRIBUTION_VARIANT = 3,

  //! @brief Count of alpha distribution types.
  ALPHA_DISTRIBUTION_COUNT = 4
};

// ============================================================================
// [Fog::ASCII_CLASS]
// ============================================================================

enum ASCII_CLASS
{
  //! @brief Mask for all lowercase characters: <code>abcdefghijklmnopqrstuvwxyz</code>.
  ASCII_CLASS_LOWER = 0x01,
  //! @brief Mask for all uppercase characters: <code>ABCDEFGHIJKLMNOPQRSTUVWXYZ</code>.
  ASCII_CLASS_UPPER = 0x02,
  //! @brief Mask for all hex characters except digits: <code>abcdefABCDEF</code>.
  ASCII_CLASS_XALPHA = 0x04,
  //! @brief Mask for all ASCII digits: <code>0123456789</code>
  ASCII_CLASS_DIGIT = 0x08,
  //! @brief Mask for underscore character: <code>_</code>
  ASCII_CLASS_UNDERSCORE = 0x10,
  //! @brief Mask for all white-spaces: <code>' ', '\\t', '\\n', '\\r', '\\f', '\\v'</code>
  ASCII_CLASS_SPACE = 0x20,
  //! @brief Mask for all punctuation characters.
  ASCII_CLASS_PUNCT = 0x40,
  //! @brief Mask for all blank characters: <code>' ', '\\t'</code>
  ASCII_CLASS_BLANK = 0x80,

  //! @brief Mask for all alpha characters (@c ASCII_CLASS_LOWER | @c ASCII_CLASS_UPPER).
  ASCII_CLASS_ALPHA = ASCII_CLASS_LOWER | ASCII_CLASS_UPPER,
  //! @brief Mask for all alpha characters or underscore (@c ASCII_CLASS_ALPHA | @c ASCII_CLASS_UNDERSCORE).
  ASCII_CLASS_ALPHA_ = ASCII_CLASS_ALPHA | ASCII_CLASS_UNDERSCORE,
  //! @brief Mask for all alpha and numeric characters (@c ASCII_CLASS_ALPHA | @c ASCII_CLASS_DIGIT).
  ASCII_CLASS_ALNUM = ASCII_CLASS_ALPHA | ASCII_CLASS_DIGIT,
  //! @brief Mask for all graphical characters (@c ASCII_CLASS_ALNUM | @c ASCII_CLASS_PUNCT).
  ASCII_CLASS_GRAPH = ASCII_CLASS_ALNUM | ASCII_CLASS_PUNCT,
  //! @brief Mask for all printable characters (@c ASCII_CLASS_GRAPH | @c ASCII_CLASS_BLANK).
  ASCII_CLASS_PRINT = ASCII_CLASS_GRAPH | ASCII_CLASS_BLANK,
  //! @brief Mask for all hex-digit characters (0-9, A-F, a-f).
  ASCII_CLASS_XDIGIT = ASCII_CLASS_DIGIT | ASCII_CLASS_XALPHA
};

// ============================================================================
// [Fog::AXIS]
// ============================================================================

//! @brief Axis.
enum AXIS
{
  //! @brief X axis.
  AXIS_X = 0x1,
  //! @brief Y axis.
  AXIS_Y = 0x2,
  //! @brief Z axis.
  AXIS_Z = 0x4
};

// ============================================================================
// [Fog::CASE_SENSITIVITY]
// ============================================================================

//! @brief Case sensitivity, used by @c String and @c ByteArray classes.
enum CASE_SENSITIVITY
{
  //! @brief Case insensitive (search, replace, indexOf, lastIndexOf, ...).
  CASE_INSENSITIVE = 0,
  //! @brief Case sensitive (search, replace, indexOf, lastIndexOf, ...).
  CASE_SENSITIVE = 1
};

// ============================================================================
// [Fog::CHAR_BIDI]
// ============================================================================

enum CHAR_BIDI
{
  // --------------------------------------------------------------------------
  // [Unicode]
  // --------------------------------------------------------------------------

  //! @brief European number.
  CHAR_BIDI_EN = 0,
  //! @brief European separator.
  CHAR_BIDI_ES = 1,
  //! @brief European terminator.
  CHAR_BIDI_ET = 2,
  //! @brief Arabic number.
  CHAR_BIDI_AN = 3,
  //! @brief Arabic letter.
  CHAR_BIDI_AL = 4,
  //! @brief White space.
  CHAR_BIDI_WS = 5,
  //! @brief Common separator.
  CHAR_BIDI_CS = 6,
  //! @brief Paragraph separator.
  CHAR_BIDI_B = 7,
  //! @brief Segment separator.
  CHAR_BIDI_S = 8,
  //! @brief Left-to-right.
  CHAR_BIDI_L = 9,
  //! @brief Left-to-right embedding.
  CHAR_BIDI_LRE = 10,
  //! @brief Left-to-right override.
  CHAR_BIDI_LRO = 11,
  //! @brief Right-to-left.
  CHAR_BIDI_R = 12,
  //! @brief Right-to-left embedding.
  CHAR_BIDI_RLE = 13,
  //! @brief Right-to-left override.
  CHAR_BIDI_RLO = 14,
  //! @brief Pop directional format.
  CHAR_BIDI_PDF = 15,
  //! @brief Non-spacing mark.
  CHAR_BIDI_NSM = 16,
  //! @brief Boundary neutral.
  CHAR_BIDI_BN = 17,
  //! @brief Other neutral.
  CHAR_BIDI_ON = 18,

  // --------------------------------------------------------------------------
  // [Expanded]
  // --------------------------------------------------------------------------

  //! @brief European number.
  CHAR_BIDI_EUROPEAN_NUMBER = CHAR_BIDI_EN,
  //! @brief European separator.
  CHAR_BIDI_EUROPEAN_SEPARATOR = CHAR_BIDI_ES,
  //! @brief European terminator.
  CHAR_BIDI_EUROPEAN_TERMINATOR = CHAR_BIDI_ET,
  //! @brief Arabic number.
  CHAR_BIDI_ARABIC_NUMBER = CHAR_BIDI_AN,
  //! @brief Arabic letter.
  CHAR_BIDI_ARABIC_LETTER = CHAR_BIDI_AL,
  //! @brief White space.
  CHAR_BIDI_WHITE_SPACE = CHAR_BIDI_WS,
  //! @brief Common separator.
  CHAR_BIDI_COMMON_SEPARATOR = CHAR_BIDI_CS,
  //! @brief Paragraph separator.
  CHAR_BIDI_PARAGRAPH_SEPARATOR = CHAR_BIDI_B,
  //! @brief Segment separator.
  CHAR_BIDI_SEGMENT_SEPARATOR = CHAR_BIDI_S,
  //! @brief Left-to-right.
  CHAR_BIDI_LEFT_TO_RIGHT = CHAR_BIDI_L,
  //! @brief Left-to-right embedding.
  CHAR_BIDI_LEFT_TO_RIGHT_EMBEDDING = CHAR_BIDI_LRE,
  //! @brief Left-to-right override.
  CHAR_BIDI_LEFT_TO_RIGHT_OVERRIDE = CHAR_BIDI_RLO,
  //! @brief Right-to-left.
  CHAR_BIDI_RIGHT_TO_LEFT = CHAR_BIDI_R,
  //! @brief Right-to-left embedding.
  CHAR_BIDI_RIGHT_TO_LEFT_EMBEDDING = CHAR_BIDI_RLE,
  //! @brief Right-to-left override.
  CHAR_BIDI_RIGHT_TO_LEFT_OVERRIDE = CHAR_BIDI_RLO,
  //! @brief Pop directional format.
  CHAR_BIDI_POP_DIRECTIONAL_FORMAT = CHAR_BIDI_PDF,
  //! @brief Non-spacing mark.
  CHAR_BIDI_NON_SPACING_MARK = CHAR_BIDI_NSM,
  //! @brief Boundary neutral.
  CHAR_BIDI_BOUNDARY_NEUTRAL = CHAR_BIDI_BN,
  //! @brief Other neutral.
  CHAR_BIDI_OTHER_NEUTRAL = CHAR_BIDI_ON
};

// ============================================================================
// [Fog::CHAR_CATEGORY]
// ============================================================================

enum CHAR_CATEGORY
{
  // --------------------------------------------------------------------------
  // [Unicode]
  // --------------------------------------------------------------------------

  //! @brief Uppercase letter.
  CHAR_CATEGORY_LU = 0,
  //! @brief Lowercase letter.
  CHAR_CATEGORY_LL = 1,
  //! @brief Titlecase letter.
  CHAR_CATEGORY_LT = 2,
  //! @brief Modifier letter.
  CHAR_CATEGORY_LM = 3,
  //! @brief Other letter.
  CHAR_CATEGORY_LO = 4,
  //! @brief Decimal number.
  CHAR_CATEGORY_ND = 5,
  //! @brief Letter number.
  CHAR_CATEGORY_NL = 6,
  //! @brief Other number.
  CHAR_CATEGORY_NO = 7,
  //! @brief Spacing mark.
  CHAR_CATEGORY_MC = 8,
  //! @brief Enclosing mark.
  CHAR_CATEGORY_ME = 9,
  //! @brief Nonclosing mark.
  CHAR_CATEGORY_MN = 10,
  //! @brief Space separator.
  CHAR_CATEGORY_ZS = 11,
  //! @brief Line separator.
  CHAR_CATEGORY_ZL = 12,
  //! @brief Paragraph separator.
  CHAR_CATEGORY_ZP = 13,
  //! @brief Other control.
  CHAR_CATEGORY_CC = 14,
  //! @brief Other format.
  CHAR_CATEGORY_CF = 15,
  //! @brief Surrogate.
  CHAR_CATEGORY_CS = 16,
  //! @brief Private use.
  CHAR_CATEGORY_CO = 17,
  //! @brief Unassigned.
  CHAR_CATEGORY_CN = 18,
  //! @brief Connector punctuation.
  CHAR_CATEGORY_PC = 19,
  //! @brief Dash punctuation.
  CHAR_CATEGORY_PD = 20,
  //! @brief Open punctuation.
  CHAR_CATEGORY_PS = 21,
  //! @brief close punctuation.
  CHAR_CATEGORY_PE = 22,
  //! @brief Initial quote punctuation.
  CHAR_CATEGORY_PI = 23,
  //! @brief Final quote punctuation.
  CHAR_CATEGORY_PF = 24,
  //! @brief Other punctuation.
  CHAR_CATEGORY_PO = 25,
  //! @brief Math symbol.
  CHAR_CATEGORY_SM = 26,
  //! @brief Currency symbol.
  CHAR_CATEGORY_SC = 27,
  //! @brief Modified symbol.
  CHAR_CATEGORY_SK = 28,
  //! @brief Other symbol.
  CHAR_CATEGORY_SO = 29,

  // --------------------------------------------------------------------------
  // [Expanded]
  // --------------------------------------------------------------------------

  CHAR_CATEGORY_NONE = 0,

  //! @brief Uppercase letter.
  CHAR_CATEGORY_LETTER_UPPERCASE = CHAR_CATEGORY_LU,
  //! @brief Lowercase letter.
  CHAR_CATEGORY_LETTER_LOWERCASE = CHAR_CATEGORY_LL,
  //! @brief Titlecase letter.
  CHAR_CATEGORY_LETTER_TITLECASE = CHAR_CATEGORY_LT,
  //! @brief Modifier letter.
  CHAR_CATEGORY_LETTER_MODIFIER = CHAR_CATEGORY_LM,
  //! @brief Other letter.
  CHAR_CATEGORY_LETTER_OTHER = CHAR_CATEGORY_LO,
  //! @brief Decimal number.
  CHAR_CATEGORY_NUMBER_DECIMAL = CHAR_CATEGORY_ND,
  //! @brief Letter number.
  CHAR_CATEGORY_NUMBER_LETTER = CHAR_CATEGORY_NL,
  //! @brief Other number.
  CHAR_CATEGORY_NUMBER_OTHER = CHAR_CATEGORY_NO,
  //! @brief Spacing mark.
  CHAR_CATEGORY_MARK_SPACING = CHAR_CATEGORY_MC,
  //! @brief Enclosing mark.
  CHAR_CATEGORY_MARK_ENCLOSING = CHAR_CATEGORY_ME,
  //! @brief Non-spacing mark.
  CHAR_CATEGORY_MARK_NONSPACING = CHAR_CATEGORY_MN,
  //! @brief Space separator.
  CHAR_CATEGORY_SEPARATOR_SPACE = CHAR_CATEGORY_ZS,
  //! @brief Line separator.
  CHAR_CATEGORY_SEPARATOR_LINE = CHAR_CATEGORY_ZL,
  //! @brief Paragraph separator.
  CHAR_CATEGORY_SEPARATOR_PARAGRAPH = CHAR_CATEGORY_ZP,
  //! @brief Other format.
  CHAR_CATEGORY_OTHER_FORMAT = CHAR_CATEGORY_CF,
  //! @brief Surrogate.
  CHAR_CATEGORY_OTHER_SURROGATE = CHAR_CATEGORY_CS,
  //! @brief Private use.
  CHAR_CATEGORY_OTHER_PRIVATE = CHAR_CATEGORY_CO,
  //! @brief Other control.
  CHAR_CATEGORY_OTHER_CONTROL = CHAR_CATEGORY_CC,
  //! @brief Unassigned.
  CHAR_CATEGORY_OTHER_UNASSIGNED = CHAR_CATEGORY_CN,
  //! @brief Connector punctuation.
  CHAR_CATEGORY_PUNCT_CONNECTOR = CHAR_CATEGORY_PC,
  //! @brief Dash punctuation.
  CHAR_CATEGORY_PUNCT_DASH = CHAR_CATEGORY_PD,
  //! @brief Open punctuation.
  CHAR_CATEGORY_PUNCT_OPEN = CHAR_CATEGORY_PS,
  //! @brief Close punctuation.
  CHAR_CATEGORY_PUNCT_CLOSE = CHAR_CATEGORY_PE,
  //! @brief Initial quote punctuation.
  CHAR_CATEGORY_PUNCT_INITIAL_QUOTE = CHAR_CATEGORY_PI,
  //! @brief Final quote punctuation.
  CHAR_CATEGORY_PUNCT_FINAL_QUOTE = CHAR_CATEGORY_PF,
  //! @brief Other punctuation.
  CHAR_CATEGORY_PUNCT_OTHER = CHAR_CATEGORY_PO,
  //! @brief Math symbol.
  CHAR_CATEGORY_SYMBOL_MATH = CHAR_CATEGORY_SM,
  //! @brief Currency symbol.
  CHAR_CATEGORY_SYMBOL_CURRENCY = CHAR_CATEGORY_SC,
  //! @brief Modifier symbol.
  CHAR_CATEGORY_SYMBOL_MODIFIER = CHAR_CATEGORY_SK,
  //! @brief Other symbol.
  CHAR_CATEGORY_SYMBOL_OTHER = CHAR_CATEGORY_SO
};

// ============================================================================
// [Fog::CHAR_CODE]
// ============================================================================

//! @brief List of some useful unicode character codes.
enum CHAR_CODE
{
  //! @brief Null terminator.
  CHAR_CODE_NULL = 0x0000,

  //! @brief Line feed (LF).
  CHAR_CODE_LF = 0x000A,
  //! @brief Carriage return (CR).
  CHAR_CODE_CR = 0x000D,

  //! @brief Space
  CHAR_CODE_SPACE = 0x0020,
  //! @brief Non-breaking space.
  CHAR_CODE_NBSP = 0x00A0,
  //! @brief Zero-width space.
  CHAR_CODE_ZWSP = 0x200B,
  //! @brief Narrow non-breaking space.
  CHAR_CODE_NARROW_NBSP = 0x202F,

  //! @brief Left-to-right mark.
  CHAR_CODE_LTR_MARK = 0x200E,
  //! @brief Right-to-left mark.
  CHAR_CODE_RTL_MARK = 0x200F,

  //! @brief Line separator.
  CHAR_CODE_LINE_SEPARATOR = 0x2028,
  //! @brief Paragraph separator.
  CHAR_CODE_PARAGRAPH_SEPARATOR = 0x2029,

  CHAR_CODE_BOM_NATIVE = 0xFEFF,
  CHAR_CODE_REPLACEMENT = 0xFFFD,
  CHAR_CODE_BOM_SWAPPED = 0xFFFE,
};

// ============================================================================
// [Fog::CHAR_COMBINING_CLASS]
// ============================================================================

enum CHAR_COMBINING_CLASS
{
  // --------------------------------------------------------------------------
  // [Unicode]
  // --------------------------------------------------------------------------

  //! @brief Not-reordered.
  //!
  //! Spacing and enclosing marks; also many vowel and consonant signs, even if
  //! nonspacing
  CHAR_COMBINING_NR = 0,
  //! @brief Marks which overlay a base letter or symbol.
  CHAR_COMBINING_OV = 1,
  //! @brief Diacritic nukta marks in Brahmi-derived script.
  CHAR_COMBINING_NK = 7,
  //! @brief Hiragana/Kakataka voicing mark.
  CHAR_COMBINING_KV = 8,
  //! @brief Viramas.
  CHAR_COMBINING_VR = 9,

  //! @brief Marks attached directly below left.
  CHAR_COMBINING_ATBL = 200,
  //! @brief Marks attached directly below.
  CHAR_COMBINING_ATB = 202,
  //! @brief Marks attached directly below right.
  CHAR_COMBINING_ATBR = 204,

  //! @brief Marks attached to the left.
  CHAR_COMBINING_ATL = 208,
  //! @brief Marks attached to the right.
  CHAR_COMBINING_ATR = 210,

  //! @brief Marks attached at the top left.
  CHAR_COMBINING_ATAL = 212,
  //! @brief Marks attached directly above.
  CHAR_COMBINING_ATA = 214,
  //! @brief Marks attached at the top right.
  CHAR_COMBINING_ATAR = 216,

  //! @brief Distinct marks at the bottom left.
  CHAR_COMBINING_BL = 218,
  //! @brief Distinct marks directly below.
  CHAR_COMBINING_B = 220,
  //! @brief Distinct marks at the bottom right.
  CHAR_COMBINING_BR = 222,

  //! @brief Distinct marks to the left.
  CHAR_COMBINING_L = 224,
  //! @brief Distinct marks to the right.
  CHAR_COMBINING_R = 226,

  //! @brief Distinct marks at the top left.
  CHAR_COMBINING_AL = 228,
  //! @brief Distinct marks directly above.
  CHAR_COMBINING_A = 230,
  //! @brief Distinct marks at the top right.
  CHAR_COMBINING_AR = 232,

  //! @brief Distinct marks subtending two bases.
  CHAR_COMBINING_DB = 233,
  //! @brief Distinct marks extending above two bases.
  CHAR_COMBINING_DA = 234,

  //! @brief Greek iota subscript only.
  CHAR_COMBINING_IS = 240,

  // --------------------------------------------------------------------------
  // [Expanded]
  // --------------------------------------------------------------------------

  //! @brief Not-reordered.
  //!
  //! Spacing and enclosing marks; also many vowel and consonant signs, even if
  //! nonspacing
  CHAR_COMBINING_NOT_REORDERED = CHAR_COMBINING_NR,
  //! @brief Marks which overlay a base letter or symbol.
  CHAR_COMBINING_OVERLAY = CHAR_COMBINING_OV,
  //! @brief Diacritic nukta marks in Brahmi-derived scripts.
  CHAR_COMBINING_NUKTA = CHAR_COMBINING_NK,
  //! @brief Hiragana/Kakataka voicing mar.
  CHAR_COMBINING_KATAKANA_VOICING = CHAR_COMBINING_KV,
  //! @brief Viramas.
  CHAR_COMBINING_VIRAMA = CHAR_COMBINING_VR,
  
  //! @brief Start of fixed position classes.
  CHAR_COMBINING_FIXED_START = 10,
  //! @brief End of fixed position classes.
  CHAR_COMBINING_FIXED_LAST = 199,

  //! @brief Marks attached directly below left.
  CHAR_COMBINING_ATTACHED_BELOW_LEFT = CHAR_COMBINING_ATBL,
  //! @brief Marks attached directly below.
  CHAR_COMBINING_ATTACHED_BELOW = CHAR_COMBINING_ATB,
  //! @brief Marks attached at the bottom right.
  CHAR_COMBINING_ATTACHED_BELOW_RIGHT = CHAR_COMBINING_ATBR,

  //! @brief Marks attached to the left.
  CHAR_COMBINING_ATTACHED_LEFT = CHAR_COMBINING_ATL,
  //! @brief Marks attached to the right.
  CHAR_COMBINING_ATTACHED_RIGHT = CHAR_COMBINING_ATR,

  //! @brief Marks attached at the top left.
  CHAR_COMBINING_ATTACHED_ABOVE_LEFT = CHAR_COMBINING_ATAL,
  //! @brief Marks attached directly above.
  CHAR_COMBINING_ATTACHED_ABOVE = CHAR_COMBINING_ATA,
  //! @brief Marks attached at the top right.
  CHAR_COMBINING_ATTACHED_ABOVE_RIGHT = CHAR_COMBINING_ATAR,

  //! @brief Distinct marks at the bottom left.
  CHAR_COMBINING_BELOW_LEFT = CHAR_COMBINING_BL,
  //! @brief Distinct marks directly below.
  CHAR_COMBINING_BELOW = CHAR_COMBINING_B,
  //! @brief Distinct marks at the bottom right.
  CHAR_COMBINING_BELOW_RIGHT = CHAR_COMBINING_BR,

  //! @brief Distinct marks to the left.
  CHAR_COMBINING_LEFT = CHAR_COMBINING_L,
  //! @brief Distinct marks to the right.
  CHAR_COMBINING_RIGHT = CHAR_COMBINING_R,

  //! @brief Distinct marks at the top left.
  CHAR_COMBINING_ABOVE_LEFT = CHAR_COMBINING_AL,
  //! @brief Distinct marks directly above.
  CHAR_COMBINING_ABOVE = CHAR_COMBINING_A,
  //! @brief Distinct marks at the top right.
  CHAR_COMBINING_ABOVE_RIGHT = CHAR_COMBINING_AR,

  //! @brief Distinct marks subtending two bases.
  CHAR_COMBINING_DOUBLE_BELOW = CHAR_COMBINING_DB,
  //! @brief Distinct marks extending above two bases.
  CHAR_COMBINING_DOUBLE_ABOVE = CHAR_COMBINING_DA,

  //! @brief Greek iota subscript only.
  CHAR_COMBINING_IOTA_SUBSCRIPT = CHAR_COMBINING_IS
};

// ============================================================================
// [Fog::CHAR_DECOMPOSITION]
// ============================================================================

enum CHAR_DECOMPOSITION
{
  CHAR_DECOMPOSITION_NONE = 0,
  //! @brief Canonical decomposition.
  CHAR_DECOMPOSITION_CANONICAL = 1,
  //! @brief Font variant.
  CHAR_DECOMPOSITION_FONT = 2,
  //! @brief No-break version of a space or hyphen.
  CHAR_DECOMPOSITION_NOBREAK = 3,
  //! @brief Initial presentation form (Arabic).
  CHAR_DECOMPOSITION_INITIAL = 4,
  //! @brief Medial presentation form (Arabic).
  CHAR_DECOMPOSITION_MEDIAL = 5,
  //! @brief Final presentation form (Arabic).
  CHAR_DECOMPOSITION_FINAL = 6,
  //! @brief Isolated presentation form (Arabic).
  CHAR_DECOMPOSITION_ISOLATED = 7,
  //! @brief Encircled form.
  CHAR_DECOMPOSITION_CIRCLE = 8,
  //! @brief Superscript form.
  CHAR_DECOMPOSITION_SUPER = 9,
  //! @brief Subscript form.
  CHAR_DECOMPOSITION_SUB = 10,
  //! @brief Vertical layout presentation form.
  CHAR_DECOMPOSITION_VERTICAL = 11,
  //! @brief Wide (or zenkaku) compatibility character.
  CHAR_DECOMPOSITION_WIDE = 12,
  //! @brief Narrow (or hankaku) compatibility character.
  CHAR_DECOMPOSITION_NARROW = 13,
  //! @brief Small variant form (CNS compatibility).
  CHAR_DECOMPOSITION_SMALL = 14,
  //! @brief CJK squared font variant.
  CHAR_DECOMPOSITION_SQUARE = 15,
  //! @brief Vulgar fraction form.
  CHAR_DECOMPOSITION_FRACTION = 16,
  //! @brief Otherwise unspecified compatibility character.
  CHAR_DECOMPOSITION_COMPAT = 17,
};

// ============================================================================
// [Fog::CHAR_EAW]
// ============================================================================

//! @brief Character East Asian width.
enum CHAR_EAW
{
  // --------------------------------------------------------------------------
  // [Unicode]
  // --------------------------------------------------------------------------

  //! @brief Ambiguous.
  CHAR_EAW_A = 0,
  //! @brief Neutral.
  CHAR_EAW_N = 1,
  //! @brief Full width.
  CHAR_EAW_F = 2,
  //! @brief Half width.
  CHAR_EAW_H = 3,
  //! @brief Narrow.
  CHAR_EAW_NA = 4,
  //! @brief Wide.
  CHAR_EAW_W = 5,

  // --------------------------------------------------------------------------
  // [Expanded]
  // --------------------------------------------------------------------------

  //! @brief Alias to @c CHAR_EAW_A.
  CHAR_EAW_AMBIGUOUS = CHAR_EAW_A,
  //! @brief Alias to @c CHAR_EAW_N.
  CHAR_EAW_NEUTRAL = CHAR_EAW_N,
  //! @brief Alias to @c CHAR_EAW_F.
  CHAR_EAW_FULL_WIDTH = CHAR_EAW_F,
  //! @brief Alias to @c CHAR_EAW_H.
  CHAR_EAW_HALF_WIDTH = CHAR_EAW_H,
  //! @brief Alias to @c CHAR_EAW_NA.
  CHAR_EAW_NARROW = CHAR_EAW_NA,
  //! @brief Alias to @c CHAR_EAW_W.
  CHAR_EAW_WIDE = CHAR_EAW_W
};

// ============================================================================
// [Fog::CHAR_GRAPHEME_BREAK]
// ============================================================================

enum CHAR_GRAPHEME_BREAK
{
  // --------------------------------------------------------------------------
  // [Unicode]
  // --------------------------------------------------------------------------

  //! @brief Other.
  CHAR_GRAPHEME_BREAK_XX = 0,
  //! @brief CR.
  CHAR_GRAPHEME_BREAK_CR = 1,
  //! @brief LF.
  CHAR_GRAPHEME_BREAK_LF = 2,
  //! @brief Control.
  CHAR_GRAPHEME_BREAK_CN = 3,
  //! @brief Extend.
  CHAR_GRAPHEME_BREAK_EX = 4,
  //! @brief Prepend
  CHAR_GRAPHEME_BREAK_PP = 5,
  //! @brief Spacing-mark.
  CHAR_GRAPHEME_BREAK_SM = 6,
  // @brief Hungul jamo L.
  CHAR_GRAPHEME_BREAK_L = 7,
  // @brief Hungul jamo V.
  CHAR_GRAPHEME_BREAK_V = 8,
  // @brief Hungul jamo T.
  CHAR_GRAPHEME_BREAK_T = 9,
  // @brief Hungul syllable LV.
  CHAR_GRAPHEME_BREAK_LV = 10,
  // @brief Hungul syllable LVT.
  CHAR_GRAPHEME_BREAK_LVT = 11,

  // --------------------------------------------------------------------------
  // [Expanded]
  // --------------------------------------------------------------------------

  //! @brief Other.
  CHAR_GRAPHEME_BREAK_OTHER = CHAR_GRAPHEME_BREAK_XX,
  //! @brief Control.
  CHAR_GRAPHEME_BREAK_CONTROL = CHAR_GRAPHEME_BREAK_CN,
  //! @brief Extend.
  CHAR_GRAPHEME_BREAK_EXTEND = CHAR_GRAPHEME_BREAK_EX,
  //! @brief Prepend
  CHAR_GRAPHEME_BREAK_PREPEND = CHAR_GRAPHEME_BREAK_PP,
  //! @brief Spacing-mark.
  CHAR_GRAPHEME_BREAK_SPACING_MARK = CHAR_GRAPHEME_BREAK_SM
};

// ============================================================================
// [Fog::CHAR_JOINING]
// ============================================================================

enum CHAR_JOINING
{
  // --------------------------------------------------------------------------
  // [Unicode]
  // --------------------------------------------------------------------------

  //! @brief Non-joining.
  CHAR_JOINING_U = 0,
  //! @brief Left joining.
  CHAR_JOINING_L = 1,
  //! @brief Right joining.
  CHAR_JOINING_R = 2,
  //! @brief Dual joining.
  CHAR_JOINING_D = 3,
  //! @brief Join causing.
  CHAR_JOINING_C = 4,
  //! @brief Transparent.
  CHAR_JOINING_T = 5,

  // --------------------------------------------------------------------------
  // [Expanded]
  // --------------------------------------------------------------------------

  //! @brief Non-joining.
  CHAR_JOINING_NONE = CHAR_JOINING_U,
  //! @brief Left joining.
  CHAR_JOINING_LEFT = CHAR_JOINING_L,
  //! @brief Right joining.
  CHAR_JOINING_RIGHT = CHAR_JOINING_R,
  //! @brief Dual joining.
  CHAR_JOINING_DUAL = CHAR_JOINING_D,
  //! @brief Join causing.
  CHAR_JOINING_CAUSING = CHAR_JOINING_C,
  //! @brief Transparent.
  CHAR_JOINING_TRANSPARENT = CHAR_JOINING_T
};

// ============================================================================
// [Fog::CHAR_LINE_BREAK]
// ============================================================================

enum CHAR_LINE_BREAK
{
  // --------------------------------------------------------------------------
  // [Unicode]
  // --------------------------------------------------------------------------

  //! @brief Mandatory break.
  CHAR_LINE_BREAK_BK = 0,
  //! @brief Carriage return.
  CHAR_LINE_BREAK_CR = 1,
  //! @brief Line feed.
  CHAR_LINE_BREAK_LF = 2,
  //! @brief Combining mark.
  CHAR_LINE_BREAK_CM = 3,
  //! @brief Surrogate.
  CHAR_LINE_BREAK_SG = 4,
  //! @brief Glue.
  CHAR_LINE_BREAK_GL = 5,
  //! @brief Contingent break.
  CHAR_LINE_BREAK_CB = 6,
  //! @brief Space.
  CHAR_LINE_BREAK_SP = 7,
  //! @brief Zero-width space.
  CHAR_LINE_BREAK_ZW = 8,
  //! @brief Next line.
  CHAR_LINE_BREAK_NL = 9,
  //! @brief Word joiner.
  CHAR_LINE_BREAK_WJ = 10,
  //! @brief JL.
  CHAR_LINE_BREAK_JL = 11,
  //! @brief JV.
  CHAR_LINE_BREAK_JV = 12,
  //! @brief JT.
  CHAR_LINE_BREAK_JT = 13,
  //! @brief H2.
  CHAR_LINE_BREAK_H2 = 14,
  //! @brief H3.
  CHAR_LINE_BREAK_H3 = 15,

  //! @brief Unknown.
  CHAR_LINE_BREAK_XX = 16,
  //! @brief Open punctuation.
  CHAR_LINE_BREAK_OP = 17,
  //! @brief Close punctuation.
  CHAR_LINE_BREAK_CL = 18,
  //! @brief Close parenthesis.
  CHAR_LINE_BREAK_CP = 19,
  //! @brief Quotation.
  CHAR_LINE_BREAK_QU = 20,
  //! @brief Nonstarter.
  CHAR_LINE_BREAK_NS = 21,
  //! @brief Exclamation.
  CHAR_LINE_BREAK_EX = 22,
  //! @brief Break symbols.
  CHAR_LINE_BREAK_SY = 23,
  //! @brief Infix numeric.
  CHAR_LINE_BREAK_IS = 24,
  //! @brief Prefix numeric.
  CHAR_LINE_BREAK_PR = 25,
  //! @brief Postfix numeric.
  CHAR_LINE_BREAK_PO = 26,
  //! @brief Numeric.
  CHAR_LINE_BREAK_NU = 27,
  //! @brief Alphabetic.
  CHAR_LINE_BREAK_AL = 28,
  //! @brief Ideographic.
  CHAR_LINE_BREAK_ID = 29,
  //! @brief Inseparable.
  CHAR_LINE_BREAK_IN = 30,
  //! @brief Hyphen.
  CHAR_LINE_BREAK_HY = 31,
  //! @brief Break before.
  CHAR_LINE_BREAK_BB = 32,
  //! @brief Break after.
  CHAR_LINE_BREAK_BA = 33,
  //! @brief Complex context.
  CHAR_LINE_BREAK_SA = 34,
  //! @brief Ambiguous.
  CHAR_LINE_BREAK_AI = 35,
  //! @brief Break both.
  CHAR_LINE_BREAK_B2 = 36,

  // --------------------------------------------------------------------------
  // [Expanded]
  // --------------------------------------------------------------------------

  //! @brief Mandatory break.
  CHAR_LINE_BREAK_MANDATORY = CHAR_LINE_BREAK_BK,
  //! @brief Combining mark.
  CHAR_LINE_BREAK_COMBINING_MARK = CHAR_LINE_BREAK_CM,
  //! @brief Surrogate.
  CHAR_LINE_BREAK_SURROGATE = CHAR_LINE_BREAK_SG,
  //! @brief Glue.
  CHAR_LINE_BREAK_GLUE = CHAR_LINE_BREAK_GL,
  //! @brief Contingent break.
  CHAR_LINE_BREAK_CONTINGENT = CHAR_LINE_BREAK_CB,
  //! @brief Space.
  CHAR_LINE_BREAK_SPACE = CHAR_LINE_BREAK_SP,
  //! @brief Zero-width space.
  CHAR_LINE_BREAK_ZWSP = CHAR_LINE_BREAK_ZW,
  //! @brief Next line.
  CHAR_LINE_BREAK_NEXT_LINE = CHAR_LINE_BREAK_NL,
  //! @brief Word joiner.
  CHAR_LINE_BREAK_WORD_JOINER = CHAR_LINE_BREAK_WJ,

  //! @brief Unknown.
  CHAR_LINE_BREAK_UNKNOWN = CHAR_LINE_BREAK_XX,
  //! @brief Open punctuation.
  CHAR_LINE_BREAK_OPEN_PUNCT = CHAR_LINE_BREAK_OP,
  //! @brief Close punctuation.
  CHAR_LINE_BREAK_CLOSE_PUNCT = CHAR_LINE_BREAK_CL,
  //! @brief Close parenthesis.
  CHAR_LINE_BREAK_CLOSE_PARENTHESIS = CHAR_LINE_BREAK_CP,
  //! @brief Quotation.
  CHAR_LINE_BREAK_QUOTATION = CHAR_LINE_BREAK_QU,
  //! @brief Nonstarter.
  CHAR_LINE_BREAK_NONSTARTER = CHAR_LINE_BREAK_NS,
  //! @brief Exclamation.
  CHAR_LINE_BREAK_EXCLAMATION = CHAR_LINE_BREAK_EX,
  //! @brief Break symbols.
  CHAR_LINE_BREAK_SYMBOLS = CHAR_LINE_BREAK_SY,
  //! @brief Infix numeric.
  CHAR_LINE_BREAK_INFIX_NUMERIC = CHAR_LINE_BREAK_IS,
  //! @brief Prefix numeric.
  CHAR_LINE_BREAK_PREFIX_NUMERIC = CHAR_LINE_BREAK_PR,
  //! @brief Postfix numeric.
  CHAR_LINE_BREAK_POSTFIX_NUMERIC = CHAR_LINE_BREAK_PO,
  //! @brief Numeric.
  CHAR_LINE_BREAK_NUMERIC = CHAR_LINE_BREAK_NU,
  //! @brief Alphabetic.
  CHAR_LINE_BREAK_ALPHABETIC = CHAR_LINE_BREAK_AL,
  //! @brief Ideographic.
  CHAR_LINE_BREAK_IDEOGRAPHIC = CHAR_LINE_BREAK_ID,
  //! @brief Inseparable.
  CHAR_LINE_BREAK_INSEPARABLE= CHAR_LINE_BREAK_IN,
  //! @brief Hyphen.
  CHAR_LINE_BREAK_HYPHEN = CHAR_LINE_BREAK_HY,
  //! @brief Break before.
  CHAR_LINE_BREAK_BEFORE = CHAR_LINE_BREAK_BB,
  //! @brief Break after.
  CHAR_LINE_BREAK_AFTER = CHAR_LINE_BREAK_BA,
  //! @brief Complex context.
  CHAR_LINE_BREAK_COMPLEX_CONTENT = CHAR_LINE_BREAK_SA,
  //! @brief Ambiguous.
  CHAR_LINE_BREAK_AMBIGUOUS = CHAR_LINE_BREAK_AI,
  //! @brief Break both.
  CHAR_LINE_BREAK_BREAK_BOTH = CHAR_LINE_BREAK_B2
};

// ============================================================================
// [Fog::CHAR_MAPPING]
// ============================================================================

//! @brief The meaning of CharProperty::mappingData member.
enum CHAR_MAPPING
{
  //! @brief No mapping, mappingData is zero.
  CHAR_MAPPING_NONE = 0,
  //! @brief Lowercase mapping character (from uppercase to lowercase).
  CHAR_MAPPING_LOWERCASE = 1,
  //! @brief Uppercase mapping character (from lowercase to uppercase/titlecase).
  CHAR_MAPPING_UPPERCASE = 2,
  //! @brief Mirror character (RTL mirroring).
  CHAR_MAPPING_MIRROR = 3,
  //! @brief Mirror character (RTL mirroring).
  CHAR_MAPPING_DIGIT = 4,
  //! @brief Index to a special table CharSpecial for complex mapping.
  CHAR_MAPPING_SPECIAL = 5
};

// ============================================================================
// [Fog::CHAR_NORMALIZATION]
// ============================================================================

//! @brief Unicode text normalization form.
enum CHAR_NORMALIZATION_FORM
{
  //! @brief Decomposed.
  CHAR_NFD = 0,
  //! @brief Composed.
  CHAR_NFC = 1,
  //! @brief Decomposed (compatibility).
  CHAR_NFKD = 2,
  //! @brief Composed (compatibility).
  CHAR_NFKC = 3
};

// ============================================================================
// [Fog::CHAR_QUICK_CHECK]
// ============================================================================

//! @brief Unicode character quick-check (used by the unicode normalization).
enum CHAR_QUICK_CHECK
{
  //! @brief Character is not allowed in the normalized form.
  CHAR_QUICK_CHECK_NO = 0,
  //! @brief Character is allowed in the normalized form.
  CHAR_QUICK_CHECK_YES = 1,
  //! @brief The occurence of character in the normalized form depends on the
  //! context.
  CHAR_QUICK_CHECK_MAYBE = 2
};

// ============================================================================
// [Fog::CHAR_SENTENCE_BREAK]
// ============================================================================

enum CHAR_SENTENCE_BREAK
{
  // --------------------------------------------------------------------------
  // [Unicode]
  // --------------------------------------------------------------------------

  //! @brief Other.
  CHAR_SENTENCE_BREAK_XX = 0,
  //! @brief CR.
  CHAR_SENTENCE_BREAK_CR = 1,
  //! @brief LF.
  CHAR_SENTENCE_BREAK_LF = 2,
  //! @brief Extend.
  CHAR_SENTENCE_BREAK_EX = 3,
  //! @brief Sep.
  CHAR_SENTENCE_BREAK_SE = 4,
  //! @brief Format.
  CHAR_SENTENCE_BREAK_FO = 5,
  //! @brief SP.
  CHAR_SENTENCE_BREAK_SP = 6,
  //! @brief Lower.
  CHAR_SENTENCE_BREAK_LO = 7,
  //! @brief Upper.
  CHAR_SENTENCE_BREAK_UP = 8,
  //! @brief OLetter.
  CHAR_SENTENCE_BREAK_LE = 9,
  //! @brief Numeric.
  CHAR_SENTENCE_BREAK_NU = 10,
  //! @brief ATerm.
  CHAR_SENTENCE_BREAK_AT = 11,
  //! @brief STerm.
  CHAR_SENTENCE_BREAK_ST = 12,
  //! @brief Close.
  CHAR_SENTENCE_BREAK_CL = 13,
  //! @brief SContinue.
  CHAR_SENTENCE_BREAK_SC = 14,

  // --------------------------------------------------------------------------
  // [Expanded]
  // --------------------------------------------------------------------------

  //! @brief Other.
  CHAR_SENTENCE_BREAK_OTHER = CHAR_SENTENCE_BREAK_XX,
  //! @brief Extend.
  CHAR_SENTENCE_BREAK_EXTEND = CHAR_SENTENCE_BREAK_EX,
  //! @brief Sep.
  CHAR_SENTENCE_BREAK_SEPARATOR = CHAR_SENTENCE_BREAK_SE,
  //! @brief Format.
  CHAR_SENTENCE_BREAK_FORMAT = CHAR_SENTENCE_BREAK_FO,
  //! @brief Lower.
  CHAR_SENTENCE_BREAK_LOWER = CHAR_SENTENCE_BREAK_LO,
  //! @brief Upper.
  CHAR_SENTENCE_BREAK_UPPER = CHAR_SENTENCE_BREAK_UP,
  //! @brief OLetter.
  CHAR_SENTENCE_BREAK_OLETTER = CHAR_SENTENCE_BREAK_LE,
  //! @brief Numeric.
  CHAR_SENTENCE_BREAK_NUMERIC = CHAR_SENTENCE_BREAK_NU,
  //! @brief ATerm.
  CHAR_SENTENCE_BREAK_ATERM = CHAR_SENTENCE_BREAK_AT,
  //! @brief STerm.
  CHAR_SENTENCE_BREAK_STERM = CHAR_SENTENCE_BREAK_ST,
  //! @brief Close.
  CHAR_SENTENCE_BREAK_CLOSE = CHAR_SENTENCE_BREAK_CL,
  //! @brief SContinue.
  CHAR_SENTENCE_BREAK_SCONTINUE = CHAR_SENTENCE_BREAK_SC
};

// ============================================================================
// [Fog::CHAR_UNICODE_VERSION]
// ============================================================================

enum CHAR_UNICODE_VERSION
{
  //! @brief Unassigned.
  CHAR_UNICODE_UNASSIGNED = 0,
  //! @brief Unicode version 1.1.
  CHAR_UNICODE_V1_1 = 1,
  //! @brief Unicode version 2.0.
  CHAR_UNICODE_V2_0 = 2,
  //! @brief Unicode version 2.1.
  CHAR_UNICODE_V2_1 = 3,
  //! @brief Unicode version 3.0.
  CHAR_UNICODE_V3_0 = 4,
  //! @brief Unicode version 3.1.
  CHAR_UNICODE_V3_1 = 5,
  //! @brief Unicode version 3.2.
  CHAR_UNICODE_V3_2 = 6,
  //! @brief Unicode version 4.0.
  CHAR_UNICODE_V4_0 = 7,
  //! @brief Unicode version 4.1.
  CHAR_UNICODE_V4_1 = 8,
  //! @brief Unicode version 5.0.
  CHAR_UNICODE_V5_0 = 9,
  //! @brief Unicode version 5.1.
  CHAR_UNICODE_V5_1 = 10,
  //! @brief Unicode version 5.2.
  CHAR_UNICODE_V5_2 = 11,
  //! @brief Unicode version 6.0.
  CHAR_UNICODE_V6_0 = 12,
  //! @brief Unicode version 6.1.
  CHAR_UNICODE_V6_1 = 13
};

// ============================================================================
// [Fog::CHAR_WORD_BREAK]
// ============================================================================

enum CHAR_WORD_BREAK
{
  // --------------------------------------------------------------------------
  // [Unicode]
  // --------------------------------------------------------------------------

  //! @brief Other word-break.
  CHAR_WORD_BREAK_XX = 0,
  //! @brief Carriage return.
  CHAR_WORD_BREAK_CR = 1,
  //! @brief Line feed.
  CHAR_WORD_BREAK_LF = 2,
  //! @brief Newline.
  CHAR_WORD_BREAK_NL = 3,
  //! @brief Extend.
  CHAR_WORD_BREAK_EXTEND = 4,
  //! @brief Format.
  CHAR_WORD_BREAK_FO = 5,
  //! @brief Katakana.
  CHAR_WORD_BREAK_KA = 6,
  //! @brief ALetter.
  CHAR_WORD_BREAK_LE = 7,
  //! @brief MidLetter.
  CHAR_WORD_BREAK_ML = 8,
  //! @brief MidNum.
  CHAR_WORD_BREAK_MN = 9,
  //! @brief MidNumLet.
  CHAR_WORD_BREAK_MB = 10,
  //! @brief Numeric.
  CHAR_WORD_BREAK_NU = 11,
  //! @brief ExtendNumLet.
  CHAR_WORD_BREAK_EX = 12,

  // --------------------------------------------------------------------------
  // [Expanded]
  // --------------------------------------------------------------------------

  //! @brief Other word-break.
  CHAR_WORD_BREAK_OTHER = CHAR_WORD_BREAK_XX,
  //! @brief Newline.
  CHAR_WORD_BREAK_NEWLINE = CHAR_WORD_BREAK_NL,
  //! @brief Format.
  CHAR_WORD_BREAK_FORMAT = CHAR_WORD_BREAK_FO,
  //! @brief Katakana.
  CHAR_WORD_BREAK_KATAKANA = CHAR_WORD_BREAK_KA,
  //! @brief ALetter.
  CHAR_WORD_BREAK_ALETTER = CHAR_WORD_BREAK_LE,
  //! @brief MidLetter.
  CHAR_WORD_BREAK_MIDLETTER = CHAR_WORD_BREAK_ML,
  //! @brief MidNum.
  CHAR_WORD_BREAK_MIDNUM = CHAR_WORD_BREAK_MN,
  //! @brief MidNumLet.
  CHAR_WORD_BREAK_MIDNUMLET = CHAR_WORD_BREAK_MB,
  //! @brief Numeric.
  CHAR_WORD_BREAK_NUMERIC = CHAR_WORD_BREAK_NU,
  //! @brief ExtendNumLet.
  CHAR_WORD_BREAK_EXTENDNUMLET = CHAR_WORD_BREAK_EX
};

// ============================================================================
// [Fog::CLIP_OP]
// ============================================================================

//! @brief Clip operation used by @c Painter::clip() and PaintEngine::clip()
//! methods.
enum CLIP_OP
{
  //! @brief Replace the current clipping area by the filled shape.
  CLIP_OP_REPLACE = 0x0,
  //! @brief Intersect the current clipping area by the filled shape.
  CLIP_OP_INTERSECT = 0x1,
  //! @brief Stroke instead of fill (invalid when clipping to masks).
  CLIP_OP_STROKE = 0x2,

  //! @brief Count of clip operators.
  CLIP_OP_COUNT = 4
};

// ============================================================================
// [Fog::CLIP_SHIFT]
// ============================================================================

enum CLIP_SHIFT
{
  CLIP_SHIFT_X0 = 2,
  CLIP_SHIFT_Y0 = 3,
  CLIP_SHIFT_X1 = 0,
  CLIP_SHIFT_Y1 = 1
};

// ============================================================================
// [Fog::CLIP_SIDE]
// ============================================================================

enum CLIP_SIDE
{
  CLIP_SIDE_NONE   = 0x00,

  CLIP_SIDE_X0     = 1 << CLIP_SHIFT_X0,
  CLIP_SIDE_Y0     = 1 << CLIP_SHIFT_Y0,
  CLIP_SIDE_X1     = 1 << CLIP_SHIFT_X1,
  CLIP_SIDE_Y1     = 1 << CLIP_SHIFT_Y1,

  CLIP_SIDE_X      = CLIP_SIDE_X0 | CLIP_SIDE_X1,
  CLIP_SIDE_Y      = CLIP_SIDE_Y0 | CLIP_SIDE_Y1,

  CLIP_SIDE_LEFT   = CLIP_SIDE_X0,
  CLIP_SIDE_TOP    = CLIP_SIDE_Y0,
  CLIP_SIDE_RIGHT  = CLIP_SIDE_X1,
  CLIP_SIDE_BOTTOM = CLIP_SIDE_Y1,

  CLIP_SIDE_LT     = CLIP_SIDE_LEFT | CLIP_SIDE_TOP,
  CLIP_SIDE_RT     = CLIP_SIDE_RIGHT | CLIP_SIDE_TOP,

  CLIP_SIDE_LB     = CLIP_SIDE_LEFT | CLIP_SIDE_BOTTOM,
  CLIP_SIDE_RB     = CLIP_SIDE_RIGHT | CLIP_SIDE_BOTTOM
};

// ============================================================================
// [Fog::COLOR_ADJUST_OP]
// ============================================================================

enum COLOR_ADJUST_OP
{
  COLOR_ADJUST_OP_NONE = 0,
  COLOR_ADJUST_OP_LIGHTEN = 1,
  COLOR_ADJUST_OP_DARKEN = 2,
  COLOR_ADJUST_OP_HUE = 3,

  COLOR_ADJUST_OP_COUNT = 4
};

// ============================================================================
// [Fog::COLOR_CHANNEL]
// ============================================================================

//! @brief Argb32 color channels.
enum COLOR_CHANNEL
{
  COLOR_CHANNEL_NONE  = 0x0,

  COLOR_CHANNEL_ALPHA = 0x1,
  COLOR_CHANNEL_RED   = 0x2,
  COLOR_CHANNEL_GREEN = 0x4,
  COLOR_CHANNEL_BLUE  = 0x8,

  COLOR_CHANNEL_RGB   = COLOR_CHANNEL_RED | COLOR_CHANNEL_GREEN | COLOR_CHANNEL_BLUE,
  COLOR_CHANNEL_ARGB  = COLOR_CHANNEL_RGB | COLOR_CHANNEL_ALPHA,

  COLOR_CHANNEL_COUNT = COLOR_CHANNEL_ARGB + 1
};

// ============================================================================
// [Fog::COLOR_MODEL]
// ============================================================================

enum COLOR_MODEL
{
  // --------------------------------------------------------------------------
  // [Public]
  // --------------------------------------------------------------------------

  COLOR_MODEL_NONE = 0,
  COLOR_MODEL_ARGB = 1,
  COLOR_MODEL_AHSV = 2,
  COLOR_MODEL_AHSL = 3,
  COLOR_MODEL_ACMYK = 4,

  COLOR_MODEL_COUNT = 5,

  // --------------------------------------------------------------------------
  // [Private]
  // --------------------------------------------------------------------------

  // Used internally by the Fog::Color conversion classes, reserved one color
  // model for the future binary compatibility.

  //! @internal
  _COLOR_MODEL_ARGB32 = 6,
  //! @internal
  _COLOR_MODEL_ARGB64 = 7,
  //! @internal
  _COLOR_MODEL_COUNT = 8
};

// ============================================================================
// [Fog::COLOR_NAME]
// ============================================================================

enum COLOR_NAME
{
  //! @brief Color in the CSS-RGB form "rgb(R, G, B)".
  COLOR_NAME_CSS_RGB = 0x00000001,
  //! @brief Color in the CSS-RGBA form "rgba(R, G, B, A)".
  COLOR_NAME_CSS_RGBA = 0x00000002,
  //! @brief Color in the CSS-HSL form "hsl(H, S, L)".
  COLOR_NAME_CSS_HSL = 0x00000004,
  //! @brief Color in the CSS-HSLA form "hsla(H, S, L, A)".
  COLOR_NAME_CSS_HSLA = 0x00000008,

  //! @brief Color in the CSS-HEX form "#RRGGBB" or "#RGB".
  COLOR_NAME_CSS_HEX = 0x00000010,

  //! @brief Color that matches the CSS color keywords.
  COLOR_NAME_CSS_KEYWORD = 0x00000020,
  //! @brief Color that matches the 'transparent' keyword.
  COLOR_NAME_CSS_TRANSPARENT = 0x00000040,

  //! @brief Strict mode.
  //!
  //! Strict mode does following:
  //!   - Error is returned if the given string starts with white-space
  //!   - Error is returned if the given string not ends with the color
  //!     keyword or data (continues).
  //!   - Error is returned for an non-css compliant color combination,
  //!     for example rgb(255, 100%, 100%) is invalid color string,
  //!     because percent notation is mixed with integer notation.
  COLOR_NAME_STRICT = 0x00001000,
  //! @brief Ignore case of the color string.
  COLOR_NAME_IGNORE_CASE = 0x00002000,

  //! @brief Use all rules to parse the color in CSS format.
  COLOR_NAME_CSS =
    COLOR_NAME_CSS_RGB         |
    COLOR_NAME_CSS_RGBA        |
    COLOR_NAME_CSS_HSL         |
    COLOR_NAME_CSS_HSLA        |
    COLOR_NAME_CSS_HEX         |
    COLOR_NAME_CSS_KEYWORD     |
    COLOR_NAME_CSS_TRANSPARENT ,

  //! @brief Use all rules to parse the color in ANY format.
  COLOR_NAME_ANY =
    COLOR_NAME_CSS             |
    COLOR_NAME_IGNORE_CASE
};

// ============================================================================
// [Fog::COLOR_MIX_ALPHA]
// ============================================================================

enum COLOR_MIX_ALPHA
{
  //! @brief Preserve the destination alpha and do not use the source alpha.
  COLOR_MIX_ALPHA_PRESERVE = 0,

  //! @brief Preserve the destination alpha and multiply the source alpha with
  //! the mask.
  COLOR_MIX_ALPHA_MASK = 1,

  //! @brief Treat the alpha as a RGB component, using the same mix function.
  COLOR_MIX_ALPHA_COMPONENT = 2
};

// ============================================================================
// [Fog::COLOR_MIX_OP]
// ============================================================================

enum COLOR_MIX_OP
{
  COLOR_MIX_OP_NONE = 0,
  COLOR_MIX_OP_BLEND = 1,
  COLOR_MIX_OP_ADD = 2,
  COLOR_MIX_OP_SUBTRACT = 3,
  COLOR_MIX_OP_MULTIPLY = 4,
  COLOR_MIX_OP_DIVIDE = 5,
  COLOR_MIX_OP_SCREEN = 6,
  COLOR_MIX_OP_OVERLAY = 7,
  COLOR_MIX_OP_DARKEN = 8,
  COLOR_MIX_OP_LIGHTEN = 9,
  COLOR_MIX_OP_DODGE = 10,
  COLOR_MIX_OP_BURN = 11,
  COLOR_MIX_OP_DIFFERENCE = 12,

  COLOR_MIX_OP_COUNT = 13
};

// ============================================================================
// [Fog::COMPOSITE_OP]
// ============================================================================

//! @brief The compositing operator.
enum COMPOSITE_OP
{
  //! @brief The source is copied to the destination.
  //!
  //! The source pixel is copied to destination pixel. If destination pixel
  //! format not supports alpha channel, the source alpha value is ignored.
  //! If there is alpha channel mask the composition is done using LERP
  //! operator.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC):
  //!   Dca' = Sca.Da + Sca.(1 - Da)
  //!        = Sca
  //!   Da'  = Sa.Da + Sa.(1 - Da)
  //!        = Sa
  //! @endverbatim
  COMPOSITE_SRC = 0,

  //! @brief The source is composited over the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_OVER):
  //!   Dca' = Sca.Da + Sca.(1 - Da) + Dca.(1 - Sa)
  //!        = Sca + Dca.(1 - Sa)
  //!   Da'  = Sa.Da + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa + Da.(1 - Sa)
  //!        = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_SRC_OVER = 1,

  //! @brief The part of the source lying inside of the destination replaces
  //! the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_IN):
  //!   Dca' = Sca.Da
  //!   Da'  = Sa.Da
  //! @endverbatim
  COMPOSITE_SRC_IN = 2,

  //! @brief The part of the source lying outside of the destination replaces
  //! the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_OUT):
  //!   Dca' = Sca.(1 - Da)
  //!   Da'  = Sa.(1 - Da)
  //! @endverbatim
  COMPOSITE_SRC_OUT = 3,

  //! @brief The part of the source lying inside of the destination is
  //! composited onto the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SRC_ATOP):
  //!   Dca' = Sca.Da + Dca.(1 - Sa)
  //!   Da'  = Sa.Da + Da.(1 - Sa)
  //!        = Sa.Da + Da - Da.Sa
  //!        = Da
  //! @endverbatim
  COMPOSITE_SRC_ATOP = 4,

  //! @brief The destination is left untouched.
  //!
  //! Destination pixels remains unchanged.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (NOP):
  //!   Dca' = Dca
  //!   Da'  = Da
  //! @endverbatim
  COMPOSITE_DST = 5,

  //! @brief The destination is composited over the source and the result
  //! replaces the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DST_OVER):
  //!   Dca' = Dca.Sa + Sca.(1 - Da) + Dca.(1 - Sa)
  //!        = Dca + Sca.(1 - Da)
  //!   Da'  = Da.Sa + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Da + Sa.(1 - Da)
  //!        = Da + Sa - Da.Sa
  //! @endverbatim
  COMPOSITE_DST_OVER = 6,

  //! @brief The part of the destination lying inside of the source replaces
  //! the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DST_IN):
  //!   Dca' = Dca.Sa
  //!   Da'  = Da.Sa
  //! @endverbatim
  COMPOSITE_DST_IN = 7,

  //! @brief The part of the destination lying outside of the source replaces
  //! the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DST_OUT):
  //!   Dca' = Dca.(1 - Sa)
  //!   Da'  = Da.(1 - Sa)
  //! @endverbatim
  COMPOSITE_DST_OUT = 8,

  //! @brief The part of the destination lying inside of the source is
  //! composited over the source and replaces the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DST_ATOP):
  //!   Dca' = Dca.Sa + Sca.(1 - Da)
  //!   Da'  = Da.Sa + Sa.(1 - Da)
  //!        = Sa.(Da + 1 - Da)
  //!        = Sa
  //! @endverbatim
  COMPOSITE_DST_ATOP = 9,

  //! @brief The part of the source that lies outside of the destination is
  //! combined with the part of the destination that lies outside of the source.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (XOR):
  //!   Dca' = Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa + Da - 2.Sa.Da
  //! @endverbatim
  COMPOSITE_XOR = 10,

  //! @brief Clear the destination not using the source.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (CLEAR):
  //!   Dca' = 0
  //!   Da'  = 0
  //! @endverbatim
  COMPOSITE_CLEAR = 11,

  //! @brief The source is added to the destination and replaces the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (ADD):
  //!   Dca' = Sca.Da + Dca.Sa + Sca.(1 - Da) + Dca.(1 - Sa)
  //!        = Dca + Sca
  //!   Da'  = Sa.Da + Da.Sa + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Da + Sa
  //! @endverbatim
  COMPOSITE_ADD = 12,

  //! @brief The source is subtracted from the destination and replaces
  //! the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SUBTRACT):
  //!   Dca' = Dca - Sca
  //!   Da'  = 1 - (1 - Sa).(1 - Da)
  //!        = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_SUBTRACT = 13,

  //! @brief The source is multiplied by the destination and replaces
  //! the destination.
  //!
  //! The resultant color is always at least as dark as either of the two
  //! constituent colors. Multiplying any color with black produces black.
  //! Multiplying any color with white leaves the original color unchanged.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (MULTIPLY):
  //!   Dca' = Sca.Dca + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa.Da + Sa.(1 - Da) + Da.(1 - Sa)
  //! @endverbatim
  COMPOSITE_MULTIPLY = 14,

  //! @brief The source and destination are complemented and then multiplied
  //! and then replace the destination.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SCREEN):
  //!   Dca' = Sca + Dca.(1 - Sca)
  //!   Da'  = Sa + Da.(1 - Sa)
  //! @endverbatim
  COMPOSITE_SCREEN = 15,

  //! @brief Multiplies or screens the colors, dependent on the destination
  //! color.
  //!
  //! Source colors overlay the destination whilst preserving its highlights
  //! and shadows. The destination color is not replaced, but is mixed with
  //! the source color to reflect the lightness or darkness of the destination.
  //!
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (OVERLAY):
  //!
  //! @verbatim
  //! Tca = Sca.(1 - Da) + Dca.(1 - Sa)
  //! Ta  = Sa .(1 - Da) + Da .(1 - Sa)
  //!
  //! if (2.Dca < Da)
  //!   Dca' = 2.Sca.Dca + Tca
  //!   Da'  = Sa.Da     + Ta
  //!        = Sa + Da - Sa.Da
  //! else
  //!   Dca' = Sa.Da - 2.(Da - Dca).(Sa - Sca) + Tca
  //!   Da'  = Sa.Da - 2.(Da - Da ).(Sa - Sa ) + Ta
  //!        = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_OVERLAY = 16,

  //! @brief Selects the darker of the destination and source colors.
  //!
  //! The destination is replaced with the source when the source is
  //! darker, otherwise it is left unchanged.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DARKEN):
  //!   Dca' = min(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = min(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa.Da + Sa - Sa.Da + Da - Sa.Da
  //!        = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_DARKEN = 17,

  //! @brief Selects the lighter of the destination and source colors.
  //!
  //! The destination is replaced with the source when the source is
  //! lighter, otherwise it is left unchanged.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (LIGHTEN):
  //!   Dca' = max(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = max(Sa.Da, Da.Sa) + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa.Da + Sa - Sa.Da + Da - Sa.Da
  //!        = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_LIGHTEN = 18,

  //! @brief Brightens the destination color to reflect the source color.
  //!
  //! Painting with black produces no change.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (COLOR_DODGE):
  //!
  //! if (Sca.Da + Dca.Sa >= Sa.Da)
  //!   Dca' = Sa.Da + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! else
  //!   Dca' = Dca.Sa / (1 - Sca / Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Dca' = [Dca.(Sa - Sca.(1 - Sa)) + Sca((Sa - Sca).(1 - Da))] / [Sa - Sca]
  //!   Da'  = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_COLOR_DODGE = 19,

  //! @brief Darkens the destination color to reflect the source color.
  //! Painting with white produces no change.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (COLOR_BURN):
  //!
  //! if (Sca.Da + Dca.Sa <= Sa.Da)
  //!   Dca' = Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! else
  //!   Dca' = Sa.(Sca.Da + Dca.Sa - Sa.Da)/Sca + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_COLOR_BURN = 20,

  //! @brief Darkens or lightens the colors, dependent on the source color value.
  //!
  //! If the source color is lighter than 0.5, the destination is lightened. If
  //! the source color is darker than 0.5, the destination is darkened, as if
  //! it were burned in. The degree of darkening or lightening is proportional
  //! to the difference between the source color and 0.5. If it is equal to 0.5,
  //! the destination is unchanged. Painting with pure black or white produces
  //! a distinctly darker or lighter area, but does not result in pure black or
  //! white.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (SOFT_LIGHT):
  //!
  //! if (2.Sca <= Sa)
  //!   Dca' = Dca.Sa - (Sa.Da - 2.Sca.Da).Dca.Sa.(Sa.Da - Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! else if (4.Dca <= Da)
  //!   Dca' = Dca.Sa + (2.Sca.Da - Sa.Da).((((16.Dsa.Sa - 12).Dsa.Sa + 4).Dsa.Da) - Dsa.Da) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! else
  //!   Dca' = Dca.Sa + (2.Sca.Da - Sa.Da).((Dca.Sa)^0.5 - Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_SOFT_LIGHT = 21,

  //! @brief Multiplies or screens the colors, dependent on the source color value.
  //!
  //! If the source color is lighter than 0.5, the destination is lightened as
  //! if it were screened. If the source color is darker than 0.5, the destination
  //! is darkened, as if it were multiplied. The degree of lightening or darkening
  //! is proportional to the difference between the source color and 0.5. If it is
  //! equal to 0.5 the destination is unchanged. Painting with pure black or white
  //! produces black or white.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (HARD_LIGHT):
  //!
  //! if (2.Sca <= Sa)
  //!   Dca' = 2.Sca.Dca + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! else
  //!   Dca' = Sa.Da - 2.(Da - Dca).(Sa - Sca) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!   Da'  = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_HARD_LIGHT = 22,

  //! @brief Subtracts the darker of the two constituent colors from the
  //! lighter.
  //!
  //! Painting with white inverts the destination color. Painting with
  //! black produces no change.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (DIFFERENCE):
  //!   Dca' = abs(Dca.Sa - Sca.Da) + Sca.(1 - Da) + Dca.(1 - Sa)
  //!        = Dca + Sca - 2.min(Sca.Da, Dca.Sa)
  //!   Da'  = abs(Da.Sa - Sa.Da) + Sa.(1 - Da) + Da.(1 - Sa)
  //!        = Sa + Da - min(Sa.Da, Da.Sa)
  //!        = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_DIFFERENCE = 23,

  //! @brief Produces an effect similar to that of 'difference', but appears
  //! as lower contrast.
  //!
  //! Painting with white inverts the destination color. Painting with black
  //! produces no change.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (EXCLUSION):
  //!   Dca' = Sca.Da + Dca - 2.Sca.Dca
  //!   Da'  = Sa + Da - Sa.Da
  //! @endverbatim
  COMPOSITE_EXCLUSION = 24,

  //! @brief Invert.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (INVERT):
  //!   Dca' = (Da - Dca) * Sa + Dca.(1 - Sa)
  //!   Da'  = (1) * Sa + Da.(1 - Sa)
  //! @endverbatim
  COMPOSITE_INVERT = 25,

  //! @brief Invert RGB.
  //!
  //! @verbatim
  //! Formulas for PRGB(dst), PRGB(src) colorspaces (INVERT_RGB):
  //!   Dca' = (Da - Dca) * Sca + Dca.(1 - Sa)
  //!   Da'  = (1) * Sa + Da.(1 - Sa)
  //! @endverbatim
  COMPOSITE_INVERT_RGB = 26,

  //! @brief Count of compositing operators (for error checking).
  COMPOSITE_COUNT = 27
};

// ============================================================================
// [Fog::CONTAINER_DATA]
// ============================================================================

//! @brief Container data flags, used by many Fog-Framework classes.
enum CONTAINER_DATA
{
  //! @brief Container data is adopted from other container / custom memory
  //! location.
  //!
  //! There are some templates that allows to allocate some container data
  //! structures on the stack (for better performance). There are also
  //! containers which allows to adopt another data pointer to be used with
  //! the container instance (for example @c Image class).
  CONTAINER_DATA_STATIC = 0x01,

  //! @brief Container data is read-only.
  CONTAINER_DATA_READ_ONLY = 0x02,

  // TODO: REMOVE.

  //! @brief Data allocated by the container are private. Private data can't
  //! be weak-referenced so any time the reference is requested the deep-copy
  //! is created.
  CONTAINER_DATA_PRIVATE = 0x08
};

// ============================================================================
// [Fog::CONTAINER_OP]
// ============================================================================

//! @brief Container operation.
//!
//! Container operation is useful when working with two containers (same or
//! different type). The operation can be transformation of data in the source
//! container to target container. Sometimes it's useful
enum CONTAINER_OP
{
  //! @brief Replace operation.
  CONTAINER_OP_REPLACE = 0,
  //! @brief Append operation.
  CONTAINER_OP_APPEND = 1
};

// ============================================================================
// [Fog::CPU_BUG]
// ============================================================================

enum CPU_BUG
{
  CPU_BUG_AMD_LOCK_MB = 1U << 0
};

// ============================================================================
// [Fog::CPU_FEATURE]
// ============================================================================

//! @brief CPU features.
enum CPU_FEATURE
{
  // --------------------------------------------------------------------------
  // [X86, AMD64]
  // --------------------------------------------------------------------------

  //! @brief Cpu has RDTSC instruction.
  CPU_FEATURE_RDTSC = 1U << 0,
  //! @brief Cpu has RDTSCP instruction.
  CPU_FEATURE_RDTSCP = 1U << 1,
  //! @brief Cpu has CMOV instruction (conditional move)
  CPU_FEATURE_CMOV = 1U << 2,
  //! @brief Cpu has CMPXCHG8B instruction
  CPU_FEATURE_CMPXCHG8B = 1U << 3,
  //! @brief Cpu has CMPXCHG16B instruction (64 bit processors)
  CPU_FEATURE_CMPXCHG16B = 1U << 4,
  //! @brief Cpu has CLFUSH instruction
  CPU_FEATURE_CLFLUSH = 1U << 5,
  //! @brief Cpu has PREFETCH instruction
  CPU_FEATURE_PREFETCH = 1U << 6,
  //! @brief Cpu supports LAHF and SAHF instrictions.
  CPU_FEATURE_LAHF_SAHF = 1U << 7,
  //! @brief Cpu supports FXSAVE and FXRSTOR instructions.
  CPU_FEATURE_FXSR = 1U << 8,
  //! @brief Cpu supports FXSAVE and FXRSTOR instruction optimizations (FFXSR).
  CPU_FEATURE_FFXSR = 1U << 9,
  //! @brief Cpu has MMX.
  CPU_FEATURE_MMX = 1U << 10,
  //! @brief Cpu has extended MMX.
  CPU_FEATURE_MMX_EXT = 1U << 11,
  //! @brief Cpu has 3dNow!
  CPU_FEATURE_3DNOW = 1U << 12,
  //! @brief Cpu has enchanced 3dNow!
  CPU_FEATURE_3DNOW_EXT = 1U << 13,
  //! @brief Cpu has SSE.
  CPU_FEATURE_SSE = 1U << 14,
  //! @brief Cpu has SSE2.
  CPU_FEATURE_SSE2 = 1U << 15,
  //! @brief Cpu has SSE3.
  CPU_FEATURE_SSE3 = 1U << 16,
  //! @brief Cpu has Supplemental SSE3 (SSSE3).
  CPU_FEATURE_SSSE3 = 1U << 17,
  //! @brief Cpu has SSE4.A.
  CPU_FEATURE_SSE4_A = 1U << 18,
  //! @brief Cpu has SSE4.1.
  CPU_FEATURE_SSE4_1 = 1U << 19,
  //! @brief Cpu has SSE4.2.
  CPU_FEATURE_SSE4_2 = 1U << 20,
  //! @brief Cpu has AVX.
  CPU_FEATURE_AVX = 1U << 22,
  //! @brief Cpu has Misaligned SSE (MSSE).
  CPU_FEATURE_MSSE = 1U << 23,
  //! @brief Cpu supports MONITOR and MWAIT instructions.
  CPU_FEATURE_MONITOR_MWAIT = 1U << 24,
  //! @brief Cpu supports MOVBE instruction.
  CPU_FEATURE_MOVBE = 1U << 25,
  //! @brief Cpu supports POPCNT instruction.
  CPU_FEATURE_POPCNT = 1U << 26,
  //! @brief Cpu supports LZCNT instruction.
  CPU_FEATURE_LZCNT  = 1U << 27,
  //! @brief Cpu supports PCLMULDQ set of instructions.
  CPU_FEATURE_PCLMULDQ  = 1U << 28,
  //! @brief Cpu supports multithreading.
  CPU_FEATURE_MULTITHREADING = 1U << 29,
  //! @brief Cpu supports execute disable bit (execute protection).
  CPU_FEATURE_EXECUTE_DISABLE_BIT = 1U << 30,
  //! @brief Cpu supports 64 bits.
  CPU_FEATURE_64_BIT = 1U << 31
};

// ============================================================================
// [Fog::CPU_VENDOR]
// ============================================================================

enum CPU_VENDOR
{
  //! @brief Unknown CPU vendor.
  CPU_VENDOR_UNKNOWN = 0,

  //! @brief Intel CPU vendor.
  CPU_VENDOR_INTEL = 1,
  //! @brief AMD CPU vendor.
  CPU_VENDOR_AMD = 2,
  //! @brief National Semiconductor CPU vendor (applies also to Cyrix processors).
  CPU_VENDOR_NSM = 3,
  //! @brief Transmeta CPU vendor.
  CPU_VENDOR_TRANSMETA = 4,
  //! @brief VIA CPU vendor.
  CPU_VENDOR_VIA = 5
};

// ============================================================================
// [Fog::DATE_DAY]
// ============================================================================

enum DATE_DAY
{
  DATE_DAY_SUNDAY = 0,
  DATE_DAY_MONDAY = 1,
  DATE_DAY_TUESDAY = 2,
  DATE_DAY_WEDNESDAY = 3,
  DATE_DAY_THURSDAY = 4,
  DATE_DAY_FRIDAY = 5,
  DATE_DAY_SATURDAY = 6,

  DATE_DAY_COUNT = 7
};

// ============================================================================
// [Fog::DATE_MONTH]
// ============================================================================

enum DATE_MONTH
{
  DATE_MONTH_JANUARY = 0,
  DATE_MONTH_FEBRUARY = 1,
  DATE_MONTH_MARCH = 2,
  DATE_MONTH_APRIL = 3,
  DATE_MONTH_MAY = 4,
  DATE_MONTH_JUNE = 5,
  DATE_MONTH_JULY = 6,
  DATE_MONTH_AUGUST = 7,
  DATE_MONTH_SEPTEMBER = 8,
  DATE_MONTH_OCTOBER = 9,
  DATE_MONTH_NOVEMBER = 10,
  DATE_MONTH_DECEMBER = 11,

  DATE_MONTH_COUNT = 12
};

// ============================================================================
// [Fog::DATE_VALUE]
// ============================================================================

enum DATE_VALUE
{
  //! @brief Year.
  DATE_VALUE_YEAR = 0,
  //! @brief Month.
  DATE_VALUE_MONTH = 1,
  //! @brief Day.
  DATE_VALUE_DAY = 2,
  //! @brief Day of week (see @c DATE_DAY).
  DATE_VALUE_DAY_OF_WEEK = 3,
  //! @brief Hour.
  DATE_VALUE_HOUR = 4,
  //! @brief Minute.
  DATE_VALUE_MINUTE = 5,
  //! @brief Second.
  DATE_VALUE_SECOND = 6,
  //! @brief Millisecond.
  DATE_VALUE_MS = 7,
  //! @brief Microsecond.
  DATE_VALUE_US = 8
};

// ============================================================================
// [Fog::DF_FORM]
// ============================================================================

//! @brief Double-format form.
enum DF_FORM
{
  //! @brief Decimal double form (printf %f compatible).
  DF_DECIMAL = 0,

  //! @brief Exponent double form (printf %e compatible).
  DF_EXPONENT = 1,

  //! @brief Significant digits (printf %g compatible).
  DF_SIGNIFICANT_DIGITS = 2
};

// ============================================================================
// [Fog::DITHER_TYPE]
// ============================================================================

enum DITHER_TYPE
{
  DITHER_TYPE_NONE = 0,
  DITHER_TYPE_PATTERN = 1
};

// ============================================================================
// [Fog::FILL_RULE]
// ============================================================================

//! @brief Fill rule.
enum FILL_RULE
{
  //! @brief Fill using non-zero rule.
  FILL_RULE_NON_ZERO = 0,
  //! @brief Fill using even-odd rule.
  FILL_RULE_EVEN_ODD = 1,

  //! @brief Default fill-rule.
  FILL_RULE_DEFAULT = FILL_RULE_EVEN_ODD,
  //! @brief Used to catch invalid arguments.
  FILL_RULE_COUNT = 2
};

// ============================================================================
// [Fog::FONT_ALIGN_MODE]
// ============================================================================

enum FONT_ALIGN_MODE
{
  FONT_ALIGN_MODE_NONE = 0x00,
  FONT_ALIGN_MODE_X = 0x01,
  FONT_ALIGN_MODE_Y = 0x02,
  FONT_ALIGN_MODE_XY = 0x03,
  FONT_ALIGN_MODE_HEIGHT = 0x04,

  FONT_ALIGN_MODE_DETECT = 0xFF
};

// ============================================================================
// [Fog::FONT_DATA]
// ============================================================================

enum FONT_DATA
{
  //! @brief The font properties were resolved - the height is in pixels and
  //! all properties were successfully detected (hinting).
  //!
  //! Physical font can be used by the layout manager and the result matches
  //! the device pixels and other requirements to layout the font-glyphs
  //! correctly (for example using quantized hinting and kerning).
  FONT_DATA_IS_PHYSICAL = 0x01,

  //! @brief The size of font-face is aligned.
  FONT_DATA_IS_ALIGNED = 0x02,

  //! @brief The font properties contains custom letter-spacing.
  FONT_DATA_HAS_LETTER_SPACING = 0x04,

  //! @brief The font properties contains custom word-spacing.
  FONT_DATA_HAS_WORD_SPACING = 0x08,

  //! @brief The font properties contains user transform.
  FONT_DATA_HAS_TRANSFORM = 0x10
};

// ============================================================================
// [Fog::FONT_DECORATION]
// ============================================================================

enum FONT_DECORATION
{
  FONT_DECORATION_NONE = 0x00,
  FONT_DECORATION_UNDERLINE = 0x01,
  FONT_DECORATION_STRIKE_THROUGH = 0x02
};

// ============================================================================
// [Fog::FONT_FEATURE]
// ============================================================================

enum FONT_FEATURE
{
  //! @brief Font contains raster-based glyphs.
  //!
  //! @note May be combined with @c FONT_FEATURE_OUTLINE.
  FONT_FEATURE_RASTER = 0x00000001,

  //! @brief Font contains outlined glyphs.
  //!
  //! @note May be combined with @c FONT_FEATURE_RASTER.
  FONT_FEATURE_OUTLINE = 0x00000002,

  //! @brief Font supports kerning.
  FONT_FEATURE_KERNING = 0x00000004,

  //! @brief Font supports hinting.
  FONT_FEATURE_HINTING = 0x00000008,

  //! @brief Font supports LCD quality of rendering.
  FONT_FEATURE_LCD_QUALITY = 0x00000010
};

// ============================================================================
// [Fog::FONT_FACE]
// ============================================================================

//! @brief Type of font-face.
enum FONT_FACE
{
  //! @brief Null font-face (dummy face that is used if something failed or on-demand).
  FONT_FACE_NULL = 0,
  //! @brief Windows font-face (HFONT instance).
  FONT_FACE_WINDOWS = 1,
  //! @brief Mac font-face.
  FONT_FACE_MAC = 2,
  //! @brief Freetype font-face (FT_Face).
  FONT_FACE_FREETYPE = 3,

  //! @brief Count of font-faces.
  FONT_FACE_COUNT = 4
};

// ============================================================================
// [Fog::FONT_FAMILY]
// ============================================================================

//! @brief Standard font family IDs (defined by CSS).
enum FONT_FAMILY
{
  //! @brief "serif" font.
  //!
  //! Glyphs of serif fonts, as the term is used in CSS, tend to have finishing
  //! strokes, flared or tapering ends, or have actual serifed endings (including
  //! slab serifs). Serif fonts are typically proportionately-spaced. They often
  //! display a greater variation between thick and thin strokes than fonts from
  //! the "sans-serif" generic font family. CSS uses the term "serif" to apply to
  //! a font for any script, although other names may be more familiar for
  //! particular scripts, such as Mincho (Japanese), Sung or Song (Chinese),
  //! Totum or Kodig (Korean). Any font that is so described may be used to
  //! represent the generic "serif" family.
  //!
  //! Latin fonts
  //!   - Times New Roman,
  //!   - Bodoni, Garamond,
  //!   - Minion Web,
  //!   - ITC Stone Serif,
  //!   - MS Georgia,
  //!   - Bitstream Cyberbit.
  //!
  //! Greek fonts
  //!   - Bitstream Cyberbit.
  //!
  //! Cyrillic fonts
  //!   - Adobe Minion Cyrillic,
  //!   - Excelsior Cyrillic Upright,
  //!   - Monotype Albion 70,
  //!   - Bitstream Cyberbit,
  //!   - ER Bukinist.
  //!
  //! Hebrew fonts
  //!   - New Peninim,
  //!   - Raanana,
  //!   - Bitstream Cyberbit.
  //!
  //! Japanese fonts
  //!   - Ryumin Light-KL,
  //!   - Kyokasho ICA,
  //!   - Futo Min A101.
  //!
  //! Arabic fonts
  //!   - Bitstream Cyberbit.
  FONT_FAMILY_SERIF = 0,

  //! @brief "sans-serif" font.
  //!
  //! Glyphs in sans-serif fonts, as the term is used in CSS, tend to have
  //! stroke endings that are plain - with little or no flaring, cross stroke,
  //! or other ornamentation. Sans-serif fonts are typically proportionately-
  //! spaced. They often have little variation between thick and thin strokes,
  //! compared to fonts from the "serif" family. CSS uses the term 'sans-serif'
  //! to apply to a font for any script, although other names may be more
  //! familiar for particular scripts, such as Gothic (Japanese), Kai (Chinese),
  //! or Pathang (Korean). Any font that is so described may be used to
  //! represent the generic ?sans-serif" family.
  //!
  //! Latin fonts:
  //!   - MS Trebuchet,
  //!   - ITC Avant Garde Gothic,
  //!   - MS Arial,
  //!   - MS Verdana,
  //!   - Univers,
  //!   - Futura,
  //!   - ITC Stone Sans,
  //!   - Gill Sans,
  //!   - Akzidenz Grotesk,
  //!   - Helvetica.
  //!
  //! Greek fonts
  //!   - Attika,
  //!   - Typiko New Era,
  //!   - MS Tahoma,
  //!   - Monotype Gill Sans 571,
  //!   - Helvetica Greek.
  //!
  //! Cyrillic fonts
  //!   - Helvetica Cyrillic,
  //!   - ER Univers,
  //!   - Lucida Sans Unicode,
  //!   - Bastion.
  //!
  //! Hebrew fonts
  //!   - Arial Hebrew,
  //!   - MS Tahoma.
  //!
  //! Japanese fonts
  //!   - Shin Go,
  //!   - Heisei Kaku Gothic W5.
  //!
  //! Arabic fonts
  //!   - MS Tahoma.
  FONT_FAMILY_SANS_SERIF = 1,

  //! @brief "cursive" font.
  //!
  //! Glyphs in cursive fonts, as the term is used in CSS, generally have either
  //! joining strokes or other cursive characteristics beyond those of italic
  //! typefaces. The glyphs are partially or completely connected, and the
  //! result looks more like handwritten pen or brush writing than printed
  //! letterwork. Fonts for some scripts, such as Arabic, are almost always
  //! cursive. CSS uses the term 'cursive' to apply to a font for any script,
  //! although other names such as Chancery, Brush, Swing and Script are also
  //! used in font names.
  //!
  //! Latin fonts:
  //!   - Caflisch Script,
  //!   - Adobe Poetica,
  //!   - Sanvito,
  //!   - Ex Ponto,
  //!   - Snell Roundhand,
  //!   - Zapf-Chancery.
  //!
  //! Cyrillic fonts:
  //!   - ER Architekt.
  //!
  //! Hebrew fonts:
  //!   - Corsiva.
  //!
  //! Arabic fonts:
  //!   - DecoType Naskh,
  //!   - Monotype Urdu 507.
  FONT_FAMILY_CUSRIVE = 2,

  //! @brief "fantasy" font.
  //!
  //! Fantasy fonts, as used in CSS, are primarily decorative while still
  //! containing representations of characters (as opposed to Pi or Picture
  //! fonts, which do not represent characters).
  //!
  //! Latin fonts:
  //! - Alpha Geometrique,
  //! - Critter,
  //! - Cottonwood,
  //! - FB Reactor,
  //! - Studz.
  FONT_FAMILY_FANTASY = 3,

  //! @brief "monospace" font.
  //!
  //! The sole criterion of a monospace font is that all glyphs have the same
  //! fixed width. (This can make some scripts, such as Arabic, look most
  //! peculiar.) The effect is similar to a manual typewriter, and is often
  //! used to set samples of computer code.
  //!
  //! Latin fonts:
  //!   - Courier,
  //!   - Courier New,
  //!   - Lucida Console,
  //!   - Monaco.
  //!
  //! Greek fonts:
  //!   - MS Courier New,
  //!   - Everson Mono.
  //!
  //! Cyrillic fonts:
  //!   - ER Kurier, Everson Mono.
  //!
  //! Japanese fonts:
  //!   - Osaka Monospaced.
  FONT_FAMILY_MONOSPACE = 4,

  //! @brief Count of font-family IDs.
  FONT_FAMILY_COUNT = 5,

  //! @brief Unknown font (not categorized family).
  FONT_FAMILY_UNKNOWN = 0xFF
};

// ============================================================================
// [Fog::FONT_HINTING]
// ============================================================================

//! @brief Font-hinting mode.
enum FONT_HINTING
{
  //! @brief Font-hinting is disabled.
  FONT_HINTING_DISABLED = 0,
  //! @brief Font-hinting is enabled.
  FONT_HINTING_ENABLED = 1,

  //! @brief Detect font-hinting.
  FONT_HINTING_DETECT = 0xFF
};

// ============================================================================
// [Fog::FONT_KERNING]
// ============================================================================

//! @brief Font-kerning mode.
enum FONT_KERNING
{
  //! @brief Disable the use of kerning.
  FONT_KERNING_DISABLED = 0,
  //! @brief Enable the use of kerning (default).
  FONT_KERNING_ENABLED = 1,

  //! @brief Detect font-kerning.
  FONT_KERNING_DETECT = 0xFF
};

// ============================================================================
// [Fog::FONT_ORDER]
// ============================================================================

//! @brief Font-provider order.
enum FONT_ORDER
{
  //! @brief Prepend the provider (add it as the first, taking the highest
  //! relevance).
  FONT_ORDER_FIRST = 0,

  //! @brief Append the provider (add it as the last item, taking the lowest
  //! relevance).
  FONT_ORDER_LAST = 1
};

// ============================================================================
// [Fog::FONT_PROVIDER]
// ============================================================================

//! @brief Font-provider IDs.
enum FONT_PROVIDER
{
  //! @brief Null font-provider (only for compatibility with @c FONT_FACE, never created).
  FONT_PROVIDER_NULL = 0,
  //! @brief Windows font-provider.
  FONT_PROVIDER_WINDOWS = 1,
  //! @brief Mac font-provider.
  FONT_PROVIDER_MAC = 2,
  //! @brief Freetype font-provider which use fontconfig.
  FONT_PROVIDER_FT_FONTCONFIG = 3,
  //! @brief Freetype font-provider which use own, minimalist provider.
  FONT_PROVIDER_FT_MINIMALIST = 4,
  //! @brief Freetype font-provider (including fontconfig support if available).
  //! @brief Custom font-provider (SVG/CSS/Others...).
  FONT_PROVIDER_CUSTOM = 5,

  //! @brief Count of font-providers.
  FONT_PROVIDER_COUNT = 6
};

// ============================================================================
// [Fog::FONT_QUALITY]
// ============================================================================

//! @brief Font quality.
enum FONT_QUALITY
{
  //! @brief No antialiasing.
  FONT_QUALITY_ALIASED = 0,
  //! @brief Grey antialiasing.
  FONT_QUALITY_GREY = 1,
  //! @brief LCD subpixel antialiasing (only enabled for the LCD output device).
  FONT_QUALITY_LCD = 2,

  //! @brief Use default font quality.
  FONT_QUALITY_DETECT = 0xFF,
  //! @brief Count of font-quality options.
  FONT_QUALITY_COUNT = 3
};

// ============================================================================
// [Fog::FONT_SPACING_MODE]
// ============================================================================

//! @brief Font spacing mode.
enum FONT_SPACING_MODE
{
  //! @brief Spacing is percentage (in Fog 0.0 to 1.0, inclusive) of the glyph
  //! spacing.
  FONT_SPACING_MODE_PERCENTAGE = 0,

  //! @brief Spacing is absolute, in font units.
  FONT_SPACING_MODE_ABSOLUTE = 1,

  //! @brief Count of font spacing modes.
  FONT_SPACING_MODE_COUNT = 2
};

// ============================================================================
// [Fog::FONT_STYLE]
// ============================================================================

//! @brief Font style.
enum FONT_STYLE
{
  //! @brief Normal style.
  FONT_STYLE_NORMAL = 0,

  //! @brief Oblique (slanted, sloped) style.
  //!
  //! Form of type that slants slightly to the right, using shearing transform
  //! of original glyph-set.
  FONT_STYLE_OBLIQUE = 1,

  //! @brief Italic style.
  //!
  //! Form of type that slants slightly to the right, using different glyph-set.
  //! If glyph-set for italic is not available, the @c FONT_STYLE_OBLIQUE is
  //! used instead.
  FONT_STYLE_ITALIC = 2,

  //! @brief Count of font styles.
  FONT_STYLE_COUNT = 3
};

// ============================================================================
// [Fog::FONT_VARIANT]
// ============================================================================

//! @brief Font variant.
enum FONT_VARIANT
{
  //! @brief Normal font-variant.
  FONT_VARIANT_NORMAL = 0,
  //! @brief Small letters are capitalized, but their size is lowered.
  FONT_VARIANT_SMALL_CAPS = 1,

  FONT_VARIANT_COUNT = 2
};

// ============================================================================
// [Fog::FONT_WEIGHT]
// ============================================================================

enum FONT_WEIGHT
{
  FONT_WEIGHT_100 = 1,
  FONT_WEIGHT_200 = 2,
  FONT_WEIGHT_300 = 3,
  FONT_WEIGHT_400 = 4,
  FONT_WEIGHT_500 = 5,
  FONT_WEIGHT_600 = 6,
  FONT_WEIGHT_700 = 7,
  FONT_WEIGHT_800 = 8,
  FONT_WEIGHT_900 = 9,

  FONT_WEIGHT_THIN       = FONT_WEIGHT_100,
  FONT_WEIGHT_EXTRALIGHT = FONT_WEIGHT_200,
  FONT_WEIGHT_LIGHT      = FONT_WEIGHT_300,
  FONT_WEIGHT_NORMAL     = FONT_WEIGHT_400,
  FONT_WEIGHT_MEDIUM     = FONT_WEIGHT_500,
  FONT_WEIGHT_SEMIBOLD   = FONT_WEIGHT_600,
  FONT_WEIGHT_BOLD       = FONT_WEIGHT_700,
  FONT_WEIGHT_EXTRABOLD  = FONT_WEIGHT_800,
  FONT_WEIGHT_BLACK      = FONT_WEIGHT_900
};

// ============================================================================
// [Fog::FORMAT_FLAGS]
// ============================================================================

//! @brief String formatting flags.
enum FORMAT_FLAGS
{
  //! @brief Show group separators (sprintf ' flag).
  FORMAT_THOUSANDS_GROUP = 0x0001,

  //! @brief Align to left (sprintf '-' flag).
  FORMAT_LEFT_ADJUSTED = 0x0002,

  //! @brief Always show sign (sprintf '+' flag).
  FORMAT_SHOW_SIGN = 0x0004,

  //! @brief Blank character instead of positive sign (sprintf ' ' flag).
  FORMAT_BLANK_POSITIVE = 0x0008,

  //! @brief Use alternate form (sprintf '#' flag).
  FORMAT_ALTERNATE_FORM = 0x0010,

  //! @brief Zero padded (sprintf '0' flag).
  FORMAT_ZERO_PADDED = 0x0020,

  //! @brief Capitalize number output.
  FORMAT_CAPITALIZE = 0x0040,

  //! @brief Capitalize E (floats) or X (hexadecimal) in number output.
  FORMAT_CAPITALIZE_E_OR_X = 0x0080
};

// ============================================================================
// [Fog::GEOMETRIC_PRECISION]
// ============================================================================

//! @brief The geometic precision hint.
enum GEOMETRIC_PRECISION
{
  //! @brief Standard geometric precision (default).
  //!
  //! Standard geometric precision uses single-precision or double-precision
  //! floating points. The usage of single-precision floating points is
  //! determined by input parameters. For example if shape that is being
  //! painted is given in single-precision floating point vectors, then
  //! single-precision floating point operations will be used to do the
  //! transformations, clipping and final painting.
  GEOMETRIC_PRECISION_NORMAL = 0,

  //! @brief High geometric precision.
  //!
  //! High geometric precision uses always double-precision floating points
  //! regardless of input arguments.
  //!
  //! @note High geometric precision may be very expensive when running on CPU
  //! without double-precision floating point unit.
  GEOMETRIC_PRECISION_HIGH = 1,

  GEOMETRIC_PRECISION_DEFAULT = GEOMETRIC_PRECISION_NORMAL,
  GEOMETRIC_PRECISION_COUNT = 2
};

// ============================================================================
// [Fog::GRADIENT_QUALITY]
// ============================================================================

//! @brief The color interpolation type, used by @c Painter.
enum GRADIENT_QUALITY
{
  //! @brief Normal interpolation quality (good for the most purposes).
  GRADIENT_QUALITY_NORMAL = 0,
  //! @brief High interpolation quality (slower, but more precise).
  GRADIENT_QUALITY_HIGH = 1,

  //! @brief Default interpolation quality, @c GRADIENT_QUALITY_NORMAL.
  GRADIENT_QUALITY_DEFAULT = GRADIENT_QUALITY_NORMAL,
  //! @brief Count of interpolation quality options.
  GRADIENT_QUALITY_COUNT = 2
};

// ============================================================================
// [Fog::GRADIENT_SPREAD]
// ============================================================================

//! @brief Spread type.
enum GRADIENT_SPREAD
{
  //! @brief Pad spread (area outside the shape continues using border color).
  GRADIENT_SPREAD_PAD = 0,
  //! @brief Releat spread (pattern is repeated).
  GRADIENT_SPREAD_REPEAT = 1,
  //! @brief Reflect spread (pattern is reflected and then repeated).
  GRADIENT_SPREAD_REFLECT = 2,

  //! @brief Default spread.
  GRADIENT_SPREAD_DEFAULT = GRADIENT_SPREAD_PAD,
  //! @brief Count of spread types.
  GRADIENT_SPREAD_COUNT = 3
};

// ============================================================================
// [Fog::GRADIENT_TYPE]
// ============================================================================

enum GRADIENT_TYPE
{
  //! @brief Linear gradient type.
  GRADIENT_TYPE_LINEAR = 0,
  //! @brief Radial gradient type.
  GRADIENT_TYPE_RADIAL = 1,
  //! @brief Conical gradient type.
  GRADIENT_TYPE_CONICAL = 2,
  //! @brief Rectangular gradient type.
  GRADIENT_TYPE_RECTANGULAR = 3,

  //! @brief Invalid gradient type, used internally.
  GRADIENT_TYPE_INVALID = 4,
  //! @brief Count of gradient types.
  GRADIENT_TYPE_COUNT = 4
};

// ============================================================================
// [Fog::IMAGE_ADOPT]
// ============================================================================

//! @brief Image adopt flags.
enum IMAGE_ADOPT
{
  //! @brief Standard adopt behavior
  IMAGE_ADOPT_DEFAULT = 0x00,
  //! @brief Adopted image will be read-only.
  IMAGE_ATOPT_READ_ONLY = 0x01,
  //! @brief Adopted image data are from bottom-to-top.
  //!
  //! Useful flag for Windows-DIB adoption.
  IMAGE_ADOPT_REVERSED = 0x02
};

// ============================================================================
// [Fog::IMAGE_BUFFER]
// ============================================================================

//! @brief Image type.
enum IMAGE_BUFFER
{
  //! @brief The image is a platform independent memory buffer.
  //!
  //! @note This is the default image type.
  IMAGE_BUFFER_MEMORY = 0,

  //! @brief The image is Windows-Dib (DIBSECTION).
  //!
  //! @note This is Windows-only image type.
  IMAGE_BUFFER_WIN_DIB = 1,

  //! @brief Count of image types.
  IMAGE_BUFFER_COUNT = 2,

  //! @brief Ignore image type (used by some functions inside @c Image).
  IMAGE_BUFFER_IGNORE = 0xFF
};

// ============================================================================
// [Fog::IMAGE_COMPONENT]
// ============================================================================

//! @brief Image components.
enum IMAGE_COMPONENT
{
  //! @brief No components.
  IMAGE_COMPONENT_NONE = 0x00000000,

  //! @brief Component mask.
  IMAGE_COMPONENT_MASK = 0x00000003,

  //! @brief Alpha component available.
  //!
  //! Related pixel formats:
  //!   - @c IMAGE_FORMAT_A8.
  //!   - @c IMAGE_FORMAT_A16.
  IMAGE_COMPONENT_ALPHA = 0x00000001,

  //! @brief RGB components available.
  //!
  //! Related pixel formats:
  //!   - @c IMAGE_FORMAT_XRGB32.
  //!   - @c IMAGE_FORMAT_RGB24.
  //!   - @c IMAGE_FORMAT_RGB48.
  IMAGE_COMPONENT_RGB = 0x00000002,

  //! @brief Image contains ARGB entities (RGB and ALPHA).
  //!
  //! Related pixel formats:
  //!   - @c IMAGE_FORMAT_PRGB32.
  //!   - @c IMAGE_FORMAT_PRGB64.
  IMAGE_COMPONENT_ARGB = IMAGE_COMPONENT_ALPHA | IMAGE_COMPONENT_RGB
};

// ============================================================================
// [Fog::IMAGE_CODEC]
// ============================================================================

//! @brief Type of @c ImageCodec class.
enum IMAGE_CODEC
{
  //! @brief None, null codec or non-initialized (shouldn't be used in public code).
  IMAGE_CODEC_NONE = 0x0,
  //! @brief Image encoder.
  IMAGE_CODEC_ENCODER = 0x1,
  //! @brief Image decoder.
  IMAGE_CODEC_DECODER = 0x2,
  //! @brief Image decoder and encoder.
  IMAGE_CODEC_BOTH = 0x3
};

// ============================================================================
// [Fog::IMAGE_DATA]
// ============================================================================

//! @brief @c Image / @c ImageData flags.
enum IMAGE_DATA
{
  // --------------------------------------------------------------------------
  // [Core Flags]
  // --------------------------------------------------------------------------

  //! @brief Image data was adopted.
  IMAGE_DATA_STATIC = CONTAINER_DATA_STATIC,

  //! @brief Image is read-only. Fog must create a copy when write operation
  //! is performed (creating @c Painter instance, locking pixels, etc...)
  IMAGE_DATA_READ_ONLY = CONTAINER_DATA_READ_ONLY,

  // --------------------------------------------------------------------------
  // [Extended Flags]
  // --------------------------------------------------------------------------

  //! @brief Image is reversed (from bottom-to-top).
  //!
  //! @note This flag should be set only when adopting other image or
  //! DIBSECTION (Windows only). Fog itselt shouldn't create reversed images.
  IMAGE_DATA_REVERSED = 0x10
};

// ============================================================================
// [Fog::IMAGE_FD_FLAGS]
// ============================================================================

enum IMAGE_FD_FLAGS
{
  IMAGE_FD_NONE,

  IMAGE_FD_IS_PREMULTIPLIED = 0x01,
  IMAGE_FD_IS_BYTESWAPPED = 0x02,
  IMAGE_FD_FILL_UNUSED_BITS = 0x04
};

// ============================================================================
// [Fog::IMAGE_FORMAT]
// ============================================================================

//! @brief Pixel format.
//!
//! @note The memory representation of nearly all pixel formats depends on the
//! CPU endianness.
//!
//! @c IMAGE_FORMAT_PRGB32, @c IMAGE_FORMAT_XRGB32:
//! - Memory       : 00|01|02|03
//! - Little endian: BB|GG|RR|AA
//! - Big endian   : AA|RR|GG|BB
//!
//! @c IMAGE_FORMAT_RGB24:
//! - Memory       : 00|01|02
//! - Little endian: BB|GG|RR
//! - Big endian   : RR|GG|BB
//!
//! @c IMAGE_FORMAT_A8:
//! - No difference: AA (8-bit alpha value, indexes to palette are also valid).
//!
//! @c IMAGE_FORMAT_I8:
//! - No difference: II (8-bit index value to palette)
//!
//! @c IMAGE_FORMAT_PRGB64
//! - Memory       : 00|01|02|03|04|05|06|07
//! - Little endian: B0|B1|G0|G1|R0|R1|A0|A1
//! - Big endian   : A1|A0|R1|R0|G1|G0|B1|B0
//!
//! @c IMAGE_FORMAT_RGB48
//! - Memory       : 00|01|02|03|04|05
//! - Little endian: B0|B1|G0|G1|R0|R1
//! - Big endian   : R1|R0|G1|G0|B1|B0
//!
//! @c IMAGE_FORMAT_A16
//! - Memory       : 00|01|
//! - Little endian: A0|A1|
//! - Big endian   : A1|A0|
//!
//! @note The terminology used for position is the same as in the @c Fog-Face
//! framework:
//!
//! @verbatim
//! - Byte0: (Value >>  0) & 0xFF
//! - Byte1: (Value >>  8) & 0xFF
//! - Byte2: (Value >> 16) & 0xFF
//! - Byte3: (Value >> 24) & 0xFF
//! @endverbatim
enum IMAGE_FORMAT
{
  // --------------------------------------------------------------------------
  // NOTE: When changing order, removing or adding image format please match
  // ${IMAGE_FORMAT:BEGIN} -> ${IMAGE_FORMAT:END} and
  // fix the code that depends on image-format order and their meaning.
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [8-bit per component or less]
  // --------------------------------------------------------------------------

  //! @brief 32-bit ARGB (8 bits per component), premultiplied.
  IMAGE_FORMAT_PRGB32 = 0,
  //! @brief 32-bit RGB (8 bits per component).
  IMAGE_FORMAT_XRGB32 = 1,
  //! @brief 24-bit RGB (8 bits per component).
  IMAGE_FORMAT_RGB24 = 2,

  //! @brief 8-bit ALPHA.
  IMAGE_FORMAT_A8 = 3,
  //! @brief 8-bit INDEXED (RGB32 palette).
  IMAGE_FORMAT_I8 = 4,

  // --------------------------------------------------------------------------
  // [16-bit per component]
  // --------------------------------------------------------------------------

  //! @brief 64-bit ARGB (16 bits per component), premultiplied.
  IMAGE_FORMAT_PRGB64 = 5,
  //! @brief 48-bit RGB (16 bits per component).
  IMAGE_FORMAT_RGB48 = 6,
  //! @brief 16-bit ALPHA.
  IMAGE_FORMAT_A16 = 7,

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  //! @brief Count of image formats.
  IMAGE_FORMAT_COUNT = 8,
  //! @brief Null image format (used only by empty images).
  IMAGE_FORMAT_NULL = IMAGE_FORMAT_COUNT,
  //! @brief Ignore image format (used by some functions inside the @c Image).
  IMAGE_FORMAT_IGNORE = IMAGE_FORMAT_COUNT
};

// ============================================================================
// [Fog::IMAGE_LIMITS]
// ============================================================================

//! @brief Image limits.
enum IMAGE_LIMITS
{
  //! @brief Maximum image width (65536 in pixels).
  IMAGE_MAX_WIDTH = 0x10000,
  //! @brief Maximum image height (65536 in pixels).
  IMAGE_MAX_HEIGHT = 0x10000
};

// ============================================================================
// [Fog::IMAGE_MIRROR_MODE]
// ============================================================================

//! @brief Mirror modes used together with @c Image::mirror().
enum IMAGE_MIRROR_MODE
{
  IMAGE_MIRROR_NONE       = 0x00,
  IMAGE_MIRROR_HORIZONTAL = 0x01,
  IMAGE_MIRROR_VERTICAL   = 0x02,
  IMAGE_MIRROR_BOTH       = 0x03
};

// ============================================================================
// [Fog::IMAGE_PRECISION]
// ============================================================================

enum IMAGE_PRECISION
{
  IMAGE_PRECISION_BYTE = 0,
  IMAGE_PRECISION_WORD = 1,

  IMAGE_PRECISION_COUNT = 2
};

// ============================================================================
// [Fog::IMAGE_ROTATE_MODE]
// ============================================================================

//! @brief Rotate modes used together with @c Image::rotate() methods.
enum IMAGE_ROTATE_MODE
{
  IMAGE_ROTATE_0   = 0x00,
  IMAGE_ROTATE_90  = 0x01,
  IMAGE_ROTATE_180 = 0x02,
  IMAGE_ROTATE_270 = 0x03
};

// ============================================================================
// [Fog::IMAGE_STREAM]
// ============================================================================

//! @brief Image file type.
enum IMAGE_STREAM
{
  IMAGE_STREAM_NONE = 0,

  IMAGE_STREAM_ANI,
  IMAGE_STREAM_APNG,
  IMAGE_STREAM_BMP,
  IMAGE_STREAM_FLI,
  IMAGE_STREAM_FLC,
  IMAGE_STREAM_GIF,
  IMAGE_STREAM_ICO,
  IMAGE_STREAM_JPEG,
  IMAGE_STREAM_LBM,
  IMAGE_STREAM_MNG,
  IMAGE_STREAM_PCX,
  IMAGE_STREAM_PNG,
  IMAGE_STREAM_PNM,
  IMAGE_STREAM_TGA,
  IMAGE_STREAM_TIFF,
  IMAGE_STREAM_XBM,
  IMAGE_STREAM_XPM,

  IMAGE_STREAM_CUSTOM = 65536
};

// ============================================================================
// [Fog::IMAGE_QUALITY]
// ============================================================================

//! @brief Image interpolation type, used by @c Painter or @c Image::scale().
enum IMAGE_QUALITY
{
  IMAGE_QUALITY_NEAREST = 0,
  IMAGE_QUALITY_BILINEAR = 1,
  IMAGE_QUALITY_BILINEAR_HQ = 2,
  IMAGE_QUALITY_BICUBIC = 3,
  IMAGE_QUALITY_BICUBIC_HQ = 4,

  IMAGE_QUALITY_DEFAULT = IMAGE_QUALITY_BILINEAR,
  IMAGE_QUALITY_COUNT = 5
};

// ============================================================================
// [Fog::LCD_ORDER]
// ============================================================================

//! @brief Order of RGB components of LCD display.
enum LCD_ORDER
{
  //! @brief Order is unknown or the device is not the LCD display.
  LCD_ORDER_NONE = 0,
  //! @brief Horizontal R-G-B order.
  LCD_ORDER_HRGB = 1,
  //! @brief Horizontal B-G-R order.
  LCD_ORDER_HBGR = 2,
  //! @brief Vertical R-G-B order.
  LCD_ORDER_VRGB = 3,
  //! @brief Vertical B-G-R order.
  LCD_ORDER_VBGR = 4,

  //! @brief Count of LCD order options.
  LCD_ORDER_COUNT = 5
};

// ============================================================================
// [Fog::LIBRARY_OPEN]
// ============================================================================

//! @brief Library open flags used in @c Library::open().
enum LIBRARY_OPEN
{
  //! @brief Don't use any flags.
  LIBRARY_OPEN_NO_FLAGS = 0,

  //! @brief Open library with system prefix (default @c true).
  //!
  //! System prefix is mainly for systems like unix, where library prefix
  //! is usually 'lib', but plugins are usually linked without this prefix,
  //! but default is to use system prefix.
  LIBRARY_OPEN_SYSTEM_PREFIX = 0x0001,

  //! @brief Open libray with system suffix (default @c true).
  //!
  //! System suffix = dot + library extension, for example in MS Windows
  //! this suffix is equal to '.dll', on unix like machines this suffix
  //! is '.so'. If you need to specify your own suffix, don't set this
  //! flag, because if you not use open flags argument. This flag is
  //! default.
  LIBRARY_OPEN_SYSTEM_SUFFIX = 0x0002,

  //! @brief Default open flags for Library constructors and @c Library::open()
  //! methods.
  //!
  //! Default is to use @c OpenSystemPrefix with @c OpenSystemSuffix
  LIBRARY_OPEN_DEFAULT = 0x0003
};

// ============================================================================
// [Fog::LINE_CAP]
// ============================================================================

//! @brief Line cap.
enum LINE_CAP
{
  LINE_CAP_BUTT = 0,
  LINE_CAP_SQUARE = 1,
  LINE_CAP_ROUND = 2,
  LINE_CAP_ROUND_REVERT = 3,
  LINE_CAP_TRIANGLE = 4,
  LINE_CAP_TRIANGLE_REVERT = 5,

  //! @brief Default line-cap type.
  LINE_CAP_DEFAULT = LINE_CAP_BUTT,
  //! @brief Used to catch invalid arguments.
  LINE_CAP_COUNT = 6
};

// ============================================================================
// [Fog::LINE_INTERSECTION]
// ============================================================================

//! @brief Line intersection result.
enum LINE_INTERSECTION
{
  LINE_INTERSECTION_NONE = 0,
  LINE_INTERSECTION_BOUNDED = 1,
  LINE_INTERSECTION_UNBOUNDED = 2
};

// ============================================================================
// [Fog::LINE_JOIN]
// ============================================================================

//! @brief Line join.
enum LINE_JOIN
{
  LINE_JOIN_MITER = 0,
  LINE_JOIN_ROUND = 1,
  LINE_JOIN_BEVEL = 2,
  LINE_JOIN_MITER_REVERT = 3,
  LINE_JOIN_MITER_ROUND = 4,

  //! @brief Default line-join type.
  LINE_JOIN_DEFAULT = LINE_JOIN_MITER,
  //! @brief Used to catch invalid arguments.
  LINE_JOIN_COUNT = 5
};

// ============================================================================
// [Fog::LOCALE_CHAR]
// ============================================================================

enum LOCALE_CHAR
{
  LOCALE_CHAR_DECIMAL_POINT = 0,
  LOCALE_CHAR_THOUSANDS_GROUP = 1,
  LOCALE_CHAR_ZERO = 2,
  LOCALE_CHAR_PLUS = 3,
  LOCALE_CHAR_MINUS = 4,
  LOCALE_CHAR_SPACE = 5,
  LOCALE_CHAR_EXPONENTIAL = 6,
  LOCALE_CHAR_FIRST_THOUSANDS_GROUP = 7,
  LOCALE_CHAR_NEXT_THOUSANDS_GROUP = 8,
  LOCALE_CHAR_RESERVED = 9,

  LOCALE_CHAR_COUNT = 10
};

// ============================================================================
// [Fog::MASK_OP]
// ============================================================================

enum MASK_OP
{
  //! @brief Copy the source to the destination (SRC).
  MASK_OP_REPLACE = 0,
  //! @brief Intersect the source with the destination (MULTIPLY).
  MASK_OP_INTERSECT = 1,

  //! @brief Count of mask operators.
  MASK_OP_COUNT = 2
};

// ============================================================================
// [Fog::MATH_INTEGRATION_METHOD]
// ============================================================================

//! @brief Type of integration methods implemented by Fog::Math::integrate().
enum MATH_INTEGRATION_METHOD
{
  //! @brief Gauss-Legendre numerical integration.
  MATH_INTEGRATION_METHOD_GAUSS = 0,

  //! @brief Count of integration methods.
  MATH_INTEGRATION_METHOD_COUNT = 1
};

// ============================================================================
// [Fog::MATH_SOLVE]
// ============================================================================

enum MATH_SOLVE
{
  MATH_SOLVE_QUADRATIC = 0,
  MATH_SOLVE_CUBIC = 1,

  MATH_SOLVE_COUNT = 2
};

// ============================================================================
// [Fog::MATRIX_ORDER]
// ============================================================================

//! @brief Matrix multiply ordering.
enum MATRIX_ORDER
{
  //! @brief The second matrix which is multiplied with the primary matrix is
  //! on the left (default for all graphics / color matrix operations).
  MATRIX_ORDER_PREPEND = 0,

  //! @brief The second matrix which is multiplied with the primary matrix is
  //! on the right.
  MATRIX_ORDER_APPEND = 1
};

// ============================================================================
// [Fog::OBJECT_EVENT_HANDLER_PROTOTYPE]
// ============================================================================

//! @brief Object event handler prototype
enum OBJECT_EVENT_HANDLER_PROTOTYPE
{
  //! @brief Event handler not accepts arguments.
  OBJECT_EVENT_HANDLER_VOID = 0,
  //! @brief Event handler accepts <code>Event*</code> argument.
  OBJECT_EVENT_HANDLER_EVENTPTR = 1
};

// ============================================================================
// [Fog::OBJECT_EVENT_POLICY]
// ============================================================================

//! @brief @ref Object event handler behavior.
//!
//! @sa @ref Object.
enum OBJECT_EVENT_HANDLER_BEHAVIOR
{
  OBJECT_EVENT_HANDLER_REVERSE = 0,
  OBJECT_EVENT_HANDLER_CASCADE = 1,
  OBJECT_EVENT_HANDLER_OVERRIDE = 2
};

// ============================================================================
// [Fog::OBJECT_FLAG]
// ============================================================================

//! @brief @ref Object flags.
enum OBJECT_FLAG
{
  //! @brief Object instance can be deleted by @c delete operator (this is the
  //! default).
  OBJECT_FLAG_CAN_DELETE = (1 << 0),

  // OBJECT TODO: Remove?
  //HasChildAddedEventHandler = (1 << 2),
  //HasChildRemovedEventHandler = (1 << 3),
  //HasParentChangedEventHandler = (1 << 4),

  //! @brief Object was created and create event was received.
  OBJECT_FLAG_CREATED = (1 << 5),
  //! @brief Object will be destroyed, method @c destroyLater() was called.
  OBJECT_FLAG_DESTROY_LATER = (1 << 6),

  // --------------------------------------------------------------------------
  // [Class Identification]
  // --------------------------------------------------------------------------

  // These flags were designed for fast object identification in a Fog-Gui
  // library. Generally all flags above 1 << 16 are used by Fog-Gui.

  //! @brief The @ref Object instance is @ref Widget.
  OBJECT_FLAG_IS_WIDGET = (1 << 16),
  //! @brief The @ref Object instance is @ref Layout.
  OBJECT_FLAG_IS_LAYOUT = (1 << 17)
};

// ============================================================================
// [Fog::PAINTER_FLUSH]
// ============================================================================

//! @brief Painter flush flags.
enum PAINTER_FLUSH
{
  //! @brief Flush all painter commands and wait for completition. Use this
  //! command if you want to access image data after the @c flush() call.
  PAINTER_FLUSH_SYNC = 0x00000001
};

// ============================================================================
// [Fog::PAINTER_INIT]
// ============================================================================

//! @brief Painter initialization flags.
enum PAINTER_INIT
{
  //! @brief Crear the content of the painter to transparent color if target
  //! buffer contains alpha channel or to black (if target buffer not contains
  //! alpha channel).
  //!
  //! Using this flag can lead to very optimized painting and it's generally
  //! faster than setting painter to @c COMPOSITE_SRC and clearing the content
  //! by using @c Painter::fillAll() or @c Painter::fillRect() methods. This
  //! method is efficient, because the painter can mark an image region as
  //! transparent and then use that hint to perform fast source-copy
  //! compositing on that region, instead of doing regular composition.
  PAINTER_INIT_CLEAR = 0x00000001,

  //! @brief Initialize multithreading if it makes sense.
  //!
  //! If this option is true, painter first check if image size is not too
  //! small (painting to small images are singlethreaded by default). Then
  //! CPU detection is used to check if machine contains more CPUs or cores.
  PAINTER_INIT_MT = 0x00000002
};

// ============================================================================
// [Fog::PAINTER_MAP]
// ============================================================================

//! @brief Painter point options.
enum PAINTER_MAP
{
  //! @brief Map user coordinates to device coordinates.
  PAINTER_MAP_USER_TO_DEVICE = 0,
  //! @brief Map device coordinates to user coordinates.
  PAINTER_MAP_DEVICE_TO_USER = 1,

  //! @brief Count of map operations.
  PAINTER_MAP_COUNT = 2
};

// ============================================================================
// [Fog::PAINT_DEVICE
// ============================================================================

//! @brief Type of the paint-device.
enum PAINT_DEVICE
{
  //! @brief Null paint-device (not initialized or invalid).
  PAINT_DEVICE_NULL = 0,
  //! @brief Same as @c PAINT_DEVICE_NULL.
  PAINT_DEVICE_UNKNOWN = 0,

  //! @brief Raster paint-device.
  PAINT_DEVICE_RASTER = 1,

  //! @brief Count of paint-device IDs.
  PAINT_DEVICE_COUNT = 2
};

// ============================================================================
// [Fog::PAINTER_PARAMETER]
// ============================================================================

enum PAINTER_PARAMETER
{
  // --------------------------------------------------------------------------
  // [Engine]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_SIZE_I = 0,
  PAINTER_PARAMETER_SIZE_F = 1,
  PAINTER_PARAMETER_SIZE_D = 2,

  PAINTER_PARAMETER_FORMAT_I = 3,
  PAINTER_PARAMETER_DEVICE_I = 4,

  // --------------------------------------------------------------------------
  // [Multithreading]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_MULTITHREADED_I = 5,
  PAINTER_PARAMETER_MAX_THREADS_I = 6,

  // --------------------------------------------------------------------------
  // [Paint Params]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_PARAMS_F = 7,
  PAINTER_PARAMETER_PARAMS_D = 8,

  // --------------------------------------------------------------------------
  // [Paint Hints]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_PAINT_HINTS = 9,

  //! @brief Compositing operator.
  PAINTER_PARAMETER_COMPOSITING_OPERATOR_I = 10,

  //! @brief Render quality, see @c RENDER_QUALITY.
  PAINTER_PARAMETER_RENDER_QUALITY_I = 11,

  //! @brief Image interpolation quality, see @c IMAGE_QUALITY.
  PAINTER_PARAMETER_IMAGE_QUALITY_I = 12,

  //! @brief Gradient interpolation quality, see @c GRADIENT_QUALITY.
  PAINTER_PARAMETER_GRADIENT_QUALITY_I = 13,

  //! @brief Whether to render text using path-outlines only.
  PAINTER_PARAMETER_OUTLINED_TEXT_I = 14,

  //! @brief Whether to maximize the performance of rendering lines, possibly
  //! degrading their visual quality.
  //!
  //! @note The quality of rendered lines must be related to antialiasing quality.
  //! This means that using fast-lines flag never turns antialiasing off, only
  //! the mask computation can be simplified.
  PAINTER_PARAMETER_FAST_LINE_I = 15,

  //! @brief Whether to maximize the geometric precision of vector coordinates
  //! and transformations.
  PAINTER_PARAMETER_GEOMETRIC_PRECISION_I = 16,

  // --------------------------------------------------------------------------
  // [Paint Opacity]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_OPACITY_F = 17,
  PAINTER_PARAMETER_OPACITY_D = 18,

  // --------------------------------------------------------------------------
  // [Fill Params]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_FILL_RULE_I = 19,

  // --------------------------------------------------------------------------
  // [Stroke Params]
  // --------------------------------------------------------------------------

  PAINTER_PARAMETER_STROKE_PARAMS_F = 20,
  PAINTER_PARAMETER_STROKE_PARAMS_D = 21,

  PAINTER_PARAMETER_LINE_WIDTH_F = 22,
  PAINTER_PARAMETER_LINE_WIDTH_D = 23,

  PAINTER_PARAMETER_LINE_JOIN_I = 24,
  PAINTER_PARAMETER_START_CAP_I = 25,
  PAINTER_PARAMETER_END_CAP_I = 26,
  PAINTER_PARAMETER_LINE_CAPS_I = 27,

  PAINTER_PARAMETER_MITER_LIMIT_F = 28,
  PAINTER_PARAMETER_MITER_LIMIT_D = 29,

  PAINTER_PARAMETER_DASH_OFFSET_F = 30,
  PAINTER_PARAMETER_DASH_OFFSET_D = 31,

  PAINTER_PARAMETER_DASH_LIST_F = 32,
  PAINTER_PARAMETER_DASH_LIST_D = 33,

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  //! @brief Count of painter parameters.
  PAINTER_PARAMETER_COUNT = 34
};

// ============================================================================
// [Fog::PAINTER_SOURCE]
// ============================================================================

enum PAINTER_SOURCE
{
  PAINTER_SOURCE_TEXTURE_F = 0,
  PAINTER_SOURCE_TEXTURE_D = 1,
  PAINTER_SOURCE_GRADIENT_F = 2,
  PAINTER_SOURCE_GRADIENT_D = 3,
  PAINTER_SOURCE_COUNT = 4
};

// ============================================================================
// [Fog::PATH_CLIPPER_STATUS]
// ============================================================================

enum PATH_CLIPPER_STATUS
{
  PATH_CLIPPER_STATUS_CLIPPED = 0,
  PATH_CLIPPER_STATUS_MUST_CLIP = 1,
  PATH_CLIPPER_STATUS_INVALID = 2
};

// ============================================================================
// [Fog::PATH_CMD]
// ============================================================================

//! @brief Path commands.
enum PATH_CMD
{
  //! @brief Move-to command.
  PATH_CMD_MOVE_TO = 0,
  //! @brief Line-to command.
  PATH_CMD_LINE_TO = 1,
  //! @brief Quad-to command.
  PATH_CMD_QUAD_TO = 2,
  //! @brief Cubic-to command.
  PATH_CMD_CUBIC_TO = 3,
  // TODO: Not implemented yet!
  //
  //! @brief Command used to distunguish between the additional data needed for
  //! @c PATH_CMD_QUAD_TO and @c PATH_CMD_CUBIC_TO.
  PATH_CMD_DATA = 4,
  //! @brief Close command.
  PATH_CMD_CLOSE = 5
};

// ============================================================================
// [Fog::PATH_DATA]
// ============================================================================

enum PATH_DATA
{
  PATH_DATA_STATIC = CONTAINER_DATA_STATIC,

  PATH_DATA_DIRTY_BBOX = 0x0010,
  PATH_DATA_DIRTY_CMD = 0x0020,

  PATH_DATA_HAS_BBOX = 0x0040,
  PATH_DATA_HAS_QBEZIER = 0x0080,
  PATH_DATA_HAS_CBEZIER = 0x0100,

  PATH_DATA_OWN_FLAGS =
    PATH_DATA_DIRTY_BBOX  |
    PATH_DATA_DIRTY_CMD   |
    PATH_DATA_HAS_BBOX    |
    PATH_DATA_HAS_QBEZIER |
    PATH_DATA_HAS_CBEZIER
};

// ============================================================================
// [Fog::PATH_DIRECTION]
// ============================================================================

//! @brief Sub-path direction.
enum PATH_DIRECTION
{
  //! @brief No direction specified.
  PATH_DIRECTION_NONE = 0,
  //! @brief Clockwise direction.
  PATH_DIRECTION_CW = 1,
  //! @brief Counter-clockwise direction.
  PATH_DIRECTION_CCW = 2
};

// ============================================================================
// [Fog::PATTERN_TYPE]
// ============================================================================

//! @brief Type of pattern in the @c PatternF or @c PatternD instance.
enum PATTERN_TYPE
{
  //! @brief Null pattern (nothing will be paint using this pattern).
  PATTERN_TYPE_NONE = 0,
  //! @brief Solid color pattern.
  PATTERN_TYPE_COLOR = 1,
  //! @brief Texture pattern (@c Texture).
  PATTERN_TYPE_TEXTURE = 2,
  //! @brief Gradient pattern (@c GradientF or @c GradientD).
  PATTERN_TYPE_GRADIENT = 3
};

// ============================================================================
// [Fog::PIXEL_ARGB32_POS]
// ============================================================================

enum PIXEL_ARGB32_POS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  PIXEL_ARGB32_POS_A = 3,
  PIXEL_ARGB32_POS_R = 2,
  PIXEL_ARGB32_POS_G = 1,
  PIXEL_ARGB32_POS_B = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  PIXEL_ARGB32_POS_A = 0,
  PIXEL_ARGB32_POS_R = 1,
  PIXEL_ARGB32_POS_G = 2,
  PIXEL_ARGB32_POS_B = 3
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::PIXEL_ARGB32_MASK]
// ============================================================================

static const uint32_t PIXEL_ARGB32_MASK_A = 0xFF000000U;
static const uint32_t PIXEL_ARGB32_MASK_R = 0x00FF0000U;
static const uint32_t PIXEL_ARGB32_MASK_G = 0x0000FF00U;
static const uint32_t PIXEL_ARGB32_MASK_B = 0x000000FFU;

// ============================================================================
// [Fog::PIXEL_ARGB32_SHIFT]
// ============================================================================

enum PIXEL_ARGB32_SHIFT
{
  PIXEL_ARGB32_SHIFT_A = 24U,
  PIXEL_ARGB32_SHIFT_R = 16U,
  PIXEL_ARGB32_SHIFT_G =  8U,
  PIXEL_ARGB32_SHIFT_B =  0U
};

// ============================================================================
// [Fog::PIXEL_ARGB64_POS]
// ============================================================================

enum PIXEL_ARGB64_POS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  PIXEL_ARGB64_POS_A = 3,
  PIXEL_ARGB64_POS_R = 2,
  PIXEL_ARGB64_POS_G = 1,
  PIXEL_ARGB64_POS_B = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  PIXEL_ARGB64_POS_A = 0,
  PIXEL_ARGB64_POS_R = 1,
  PIXEL_ARGB64_POS_G = 2,
  PIXEL_ARGB64_POS_B = 3
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::PIXEL_ARGB64_MASK]
// ============================================================================

static const uint64_t PIXEL_ARGB64_MASK_A = FOG_UINT64_C(0xFFFF000000000000);
static const uint64_t PIXEL_ARGB64_MASK_R = FOG_UINT64_C(0x0000FFFF00000000);
static const uint64_t PIXEL_ARGB64_MASK_G = FOG_UINT64_C(0x00000000FFFF0000);
static const uint64_t PIXEL_ARGB64_MASK_B = FOG_UINT64_C(0x000000000000FFFF);

// ============================================================================
// [Fog::PIXEL_ARGB64_SHIFT]
// ============================================================================

enum PIXEL_ARGB64_SHIFT
{
  PIXEL_ARGB64_SHIFT_A = 48U,
  PIXEL_ARGB64_SHIFT_R = 32U,
  PIXEL_ARGB64_SHIFT_G = 16U,
  PIXEL_ARGB64_SHIFT_B =  0U
};

// ============================================================================
// [Fog::PIXEL_RGB24_POS]
// ============================================================================

enum PIXEL_RGB24_POS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  PIXEL_RGB24_POS_R = 2,
  PIXEL_RGB24_POS_G = 1,
  PIXEL_RGB24_POS_B = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  PIXEL_RGB24_POS_R = 0,
  PIXEL_RGB24_POS_G = 1,
  PIXEL_RGB24_POS_B = 2
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::PIXEL_RGB48_POS]
// ============================================================================

enum PIXEL_RGB48_POS
{
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  PIXEL_RGB48_POS_R = 2,
  PIXEL_RGB48_POS_G = 1,
  PIXEL_RGB48_POS_B = 0
#else // FOG_BYTE_ORDER == FOG_BIG_ENDIAN
  PIXEL_RGB48_POS_R = 0,
  PIXEL_RGB48_POS_G = 1,
  PIXEL_RGB48_POS_B = 2
#endif // FOG_BYTE_ORDER
};

// ============================================================================
// [Fog::REGION_HIT_TEST]
// ============================================================================

//! @brief Region hit-test result.
enum REGION_HIT_TEST
{
  //! @brief Object isn't in region (point, rectangle or another region).
  REGION_HIT_OUT = 0,
  //! @brief Object is in region (point, rectangle or another region).
  REGION_HIT_IN = 1,
  //! @brief Object is partially in region (point, rectangle or another region).
  REGION_HIT_PART = 2
};

// ============================================================================
// [Fog::REGION_OP]
// ============================================================================

//! @brief Region or clip combining operators.
enum REGION_OP
{
  //! @brief Replace (COPY).
  REGION_OP_REPLACE = 0,
  //! @brief Intersection (AND).
  REGION_OP_INTERSECT = 1,
  //! @brief Union (OR)
  REGION_OP_UNION = 2,
  //! @brief eXclusive or (XOR).
  REGION_OP_XOR = 3,
  //! @brief Subtraction (DIFF).
  REGION_OP_SUBTRACT = 4,

  //! @brief Count of region operators.
  REGION_OP_COUNT = 5
};

// ============================================================================
// [Fog::REGION_TYPE]
// ============================================================================

//! @brief Type of @c Region.
enum REGION_TYPE
{
  // NOTE: Never change value of REGION_TYPE_EMPTY and REGION_TYPE_SIMPLE
  // constants, see Region::getType() method in Region.cpp file.

  //! @brief Region is empty.
  REGION_TYPE_EMPTY = 0,
  //! @brief Region has only one rectangle (rectangular).
  REGION_TYPE_SIMPLE = 1,
  //! @brief Region has more YX sorted rectangles.
  REGION_TYPE_COMPLEX = 2,
  //! @brief Region is infinite (special region type).
  REGION_TYPE_INFINITE = 3
};

// ============================================================================
// [Fog::RENDER_QUALITY]
// ============================================================================

//! @brief Render quality.
enum RENDER_QUALITY
{
  //! @brief Aliased (disabled antialiasing).
  RENDER_QUALITY_ALIASED = 0,

  //! @brief Use at least 4 shades of grey for antialiasing.
  //!
  //! This is only recommendation for paint-engine. It can use higher render
  //! quality if this option is not supported.
  RENDER_QUALITY_GREY_4 = 1,

  //! @brief Use at least 8 shades of grey for antialiasing.
  //!
  //! This is only recommendation for paint-engine. It can use higher or lower
  //! render quality if this option is not supported.
  RENDER_QUALITY_GREY_8 = 2,

  //! @brief Use at least 16 shades of grey for antialiasing.
  //!
  //! This is only recommendation for paint-engine. It can use higher or lower
  //! render quality if this option is not supported.
  RENDER_QUALITY_GREY_16 = 3,

  //! @brief Use at least 32 shades of grey for antialiasing.
  //!
  //! This is only recommendation for paint-engine. It can use higher or lower
  //! render quality if this option is not supported.
  RENDER_QUALITY_GREY_32 = 4,

  //! @brief Use at least 64 shades of grey for antialiasing.
  //!
  //! This is only recommendation for paint-engine. It can use higher or lower
  //! render quality if this option is not supported.
  RENDER_QUALITY_GREY_64 = 5,

  //! @brief Use at least 256 shades of grey for antialiasing when rendering
  //! for 8-bit target and 65536 shades of grey when rendering for 16-bit
  //! target.
  //!
  //! This is only recommendation for paint-engine. It can use higher or lower
  //! render quality if this option is not supported.
  RENDER_QUALITY_GREY_HQ = 6,

  //! @brief LCD subpixel antialiasing.
  //!
  //! This type of antialiasing is usually only implemented for font rendering.
  RENDER_QUALITY_LCD = 7,

  //! @brief Count of render quality settings (for error checking).
  RENDER_QUALITY_COUNT = 8,

  //! @brief Default rendering quality (synonym to @c RENDER_QUALITY_GREY_16).
  RENDER_QUALITY_DEFAULT = RENDER_QUALITY_GREY_16
};

// ============================================================================
// [Fog::SHAPE_TYPE]
// ============================================================================

//! @brief Type of primitive shape, see @c ShapeF and @c ShapeD classes.
enum SHAPE_TYPE
{
  SHAPE_TYPE_NONE = 0,

  // --------------------------------------------------------------------------
  // [Unclosed]
  // --------------------------------------------------------------------------

  SHAPE_TYPE_LINE = 1,
  SHAPE_TYPE_QBEZIER = 2,
  SHAPE_TYPE_CBEZIER = 3,
  SHAPE_TYPE_ARC = 4,

  // --------------------------------------------------------------------------
  // [Closed]
  // --------------------------------------------------------------------------

  SHAPE_TYPE_RECT = 5,
  SHAPE_TYPE_ROUND = 6,
  SHAPE_TYPE_CIRCLE = 7,
  SHAPE_TYPE_ELLIPSE = 8,
  SHAPE_TYPE_CHORD = 9,
  SHAPE_TYPE_PIE = 10,
  SHAPE_TYPE_TRIANGLE = 11,

  // --------------------------------------------------------------------------
  // [Count]
  // --------------------------------------------------------------------------

  SHAPE_TYPE_COUNT = 12
};

// ============================================================================
// [Fog::SORT_ORDER]
// ============================================================================

enum SORT_ORDER
{
  SORT_ORDER_ASCENT = 0,
  SORT_ORDER_DESCENT = 1
};

// ============================================================================
// [Fog::SPAN]
// ============================================================================

//! @brief Type of @c Span.
enum SPAN
{
  // --------------------------------------------------------------------------
  // NOTE: When changing these constants, please make sure that the span methods
  // like isConst()/isVariant() are also changed. There are some optimizations
  // which are based on order of these constants.
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Const]
  // --------------------------------------------------------------------------

  //! @brief Span is a const-mask.
  SPAN_C = 0,

  // --------------------------------------------------------------------------
  // [Variant]
  // --------------------------------------------------------------------------

  //! @brief Start of variant-alpha span types (may be used by asserts).
  //!
  //! @note This constant is only shadow to valid mask type, don't use this
  //! value in switch() {}.
  SPAN_V_BEGIN = 1,

  //! @brief Span is a variable-alpha mask (8-bit, A8).
  //!
  //! @sa @c SPAN_AX_EXTRA.
  SPAN_A8_GLYPH = 1,

  //! @brief Span is a variable-alpha mask (target bit-depth, A8, A16 or A32).
  //!
  //! @sa @c SPAN_AX_EXTRA.
  SPAN_AX_GLYPH = 2,

  //! @brief Span is an extended variable-alpha mask (target bit-depth + 1 bit
  //! for exact scaling).
  SPAN_AX_EXTRA = 3,

  //! @brief Span is a variable-argb mask (8-bit, PRGB32).
  SPAN_ARGB32_GLYPH = 4,

  //! @brief Span is a variable-argb mask (target bit-depth, PRGB32 or PRGB64).
  SPAN_ARGBXX_GLYPH = 5,

  //! @brief The count of span types.
  SPAN_COUNT = 6
};

// ============================================================================
// [Fog::SPLIT_BEHAVIOR]
// ============================================================================

enum SPLIT_BEHAVIOR
{
  SPLIT_KEEP_EMPTY_PARTS = 0,
  SPLIT_REMOVE_EMPTY_PARTS = 1
};

// ============================================================================
// [Fog::STREAM_DEVICE_FLAGS]
// ============================================================================

//! @brief Flags describing @c StreamDevice.
enum STREAM_DEVICE_FLAGS
{
  STREAM_IS_OPEN     = (1 << 0),
  STREAM_IS_SEEKABLE = (1 << 1),
  STREAM_IS_READABLE = (1 << 2),
  STREAM_IS_WRITABLE = (1 << 3),
  STREAM_IS_CLOSABLE = (1 << 4),

  STREAM_IS_HFILE    = (1 << 16),
  STREAM_IS_FD       = (1 << 17),
  STREAM_IS_MEMORY   = (1 << 18),
  STREAM_IS_GROWABLE = (1 << 19)
};

// ============================================================================
// [Fog::STREAM_OPEN_FLAGS]
// ============================================================================

//! @brief Stream open flags.
enum STREAM_OPEN_FLAGS
{
  STREAM_OPEN_READ = (1 << 0),
  STREAM_OPEN_WRITE = (1 << 1),
  STREAM_OPEN_RW = STREAM_OPEN_READ | STREAM_OPEN_WRITE,
  STREAM_OPEN_TRUNCATE = (1 << 2),
  STREAM_OPEN_APPEND = (1 << 3),
  STREAM_OPEN_CREATE = (1 << 4),
  STREAM_OPEN_CREATE_PATH = (1 << 5),
  STREAM_OPEN_CREATE_ONLY = (1 << 6)
};

// ============================================================================
// [Fog::STREAM_SEEK_MODE]
// ============================================================================

//! @brief Stream seek mode.
enum STREAM_SEEK_MODE
{
  STREAM_SEEK_SET = 0,
  STREAM_SEEK_CUR = 1,
  STREAM_SEEK_END = 2
};

// ============================================================================
// [Fog::TEXT_ALIGN]
// ============================================================================

//! @brief Text alignment
enum TEXT_ALIGN
{
  TEXT_ALIGN_LEFT        = 0x01,
  TEXT_ALIGN_RIGHT       = 0x02,
  TEXT_ALIGN_HCENTER     = 0x03,
  TEXT_ALIGN_HMASK       = 0x03,

  TEXT_ALIGN_TOP         = 0x10,
  TEXT_ALIGN_BOTTOM      = 0x20,
  TEXT_ALIGN_VCENTER     = 0x30,
  TEXT_ALIGN_VMASK       = 0x30,

  TEXT_ALIGN_CENTER      = TEXT_ALIGN_VCENTER | TEXT_ALIGN_HCENTER
};

// ============================================================================
// [Fog::TEXT_CASE]
// ============================================================================

enum TEXT_CASE
{
  TEXT_CASE_LOWER = 0,
  TEXT_CASE_UPPER = 1
};

// ============================================================================
// [Fog::TEXT_CODEC_CACHE]
// ============================================================================

enum TEXT_CODEC_CACHE
{
  TEXT_CODEC_CACHE_NONE  = 0,
  TEXT_CODEC_CACHE_ASCII = 1,
  TEXT_CODEC_CACHE_LOCAL = 2,
  TEXT_CODEC_CACHE_UTF8  = 3,
  TEXT_CODEC_CACHE_UTF16 = 4,
  TEXT_CODEC_CACHE_UTF32 = 5,
  TEXT_CODEC_CACHE_WCHAR = sizeof(wchar_t) == 2 ? TEXT_CODEC_CACHE_UTF16 : TEXT_CODEC_CACHE_UTF32,

  TEXT_CODEC_CACHE_COUNT = 6
};

// ============================================================================
// [Fog::TEXT_ENCODING]
// ============================================================================

//! @brief Text-codec codes.
enum TEXT_ENCODING
{
  //! @brief None, not initialized or signalizes error.
  TEXT_ENCODING_NONE = 0,

  //! @brief ISO-8859-1 text-codec.
  TEXT_ENCODING_ISO8859_1 = 1,
  //! @brief ISO-8859-2 text-codec.
  TEXT_ENCODING_ISO8859_2 = 2,
  //! @brief ISO-8859-3 text-codec.
  TEXT_ENCODING_ISO8859_3 = 3,
  //! @brief ISO-8859-4 text-codec.
  TEXT_ENCODING_ISO8859_4 = 4,
  //! @brief ISO-8859-5 text-codec.
  TEXT_ENCODING_ISO8859_5 = 5,
  //! @brief ISO-8859-6 text-codec.
  TEXT_ENCODING_ISO8859_6 = 6,
  //! @brief ISO-8859-7 text-codec.
  TEXT_ENCODING_ISO8859_7 = 7,
  //! @brief ISO-8859-8 text-codec.
  TEXT_ENCODING_ISO8859_8 = 8,
  //! @brief ISO-8859-9 text-codec.
  TEXT_ENCODING_ISO8859_9 = 9,
  //! @brief ISO-8859-10 text-codec.
  TEXT_ENCODING_ISO8859_10 = 10,
  //! @brief ISO-8859-11 text-codec.
  TEXT_ENCODING_ISO8859_11 = 11,
  //! @brief ISO-8859-13 text-codec.
  TEXT_ENCODING_ISO8859_13 = 12,
  //! @brief ISO-8859-14 text-codec.
  TEXT_ENCODING_ISO8859_14 = 13,
  //! @brief ISO-8859-14 text-codec.
  TEXT_ENCODING_ISO8859_15 = 14,
  //! @brief ISO-8859-16 text-codec.
  TEXT_ENCODING_ISO8859_16 = 15,

  //! @brief CP-850 text-codec.
  TEXT_ENCODING_CP850 = 16,
  //! @brief CP-866 text-codec.
  TEXT_ENCODING_CP866 = 17,
  //! @brief CP-874 text-codec.
  TEXT_ENCODING_CP874 = 18,
  //! @brief CP-1250 text-codec.
  TEXT_ENCODING_CP1250 = 19,
  //! @brief CP-1251 text-codec.
  TEXT_ENCODING_CP1251 = 20,
  //! @brief CP-1252 text-codec.
  TEXT_ENCODING_CP1252 = 21,
  //! @brief CP-1253 text-codec.
  TEXT_ENCODING_CP1253 = 22,
  //! @brief CP-1254 text-codec.
  TEXT_ENCODING_CP1254 = 23,
  //! @brief CP-1255 text-codec.
  TEXT_ENCODING_CP1255 = 24,
  //! @brief CP-1256 text-codec.
  TEXT_ENCODING_CP1256 = 25,
  //! @brief CP-1257 text-codec.
  TEXT_ENCODING_CP1257 = 26,
  //! @brief CP-1258 text-codec.
  TEXT_ENCODING_CP1258 = 27,

  //! @brief MAC-ROMAN codec.
  TEXT_ENCODING_MAC_ROMAN = 28,

  //! @brief KOI8R text-codec.
  TEXT_ENCODING_KOI8R = 29,
  //! @brief KOI8U text-codec.
  TEXT_ENCODING_KOI8U = 30,

  //! @brief WINSAMI-2 text-codec.
  TEXT_ENCODING_WINSAMI2 = 31,

  //! @brief ROMAN-8 text-codec.
  TEXT_ENCODING_ROMAN8 = 32,

  //! @brief ARMSCII-8 text-codec.
  TEXT_ENCODING_ARMSCII8 = 33,

  //! @brief GEORGIAN-ACADEMY text-codec.
  TEXT_ENCODING_GEORGIAN_ACADEMY = 34,
  //! @brief GEORGIAN-PS text-codec.
  TEXT_ENCODING_GEORGIAN_PS = 35,

  //! @brief UTF-8 text-codec.
  TEXT_ENCODING_UTF8 = 36,

  //! @brief UTF-16 text-codec (little-endian).
  TEXT_ENCODING_UTF16_LE = 37,
  //! @brief UTF-16 text-codec (big-endian).
  TEXT_ENCODING_UTF16_BE = 38,

  //! @brief UTF-32 text-codec (little-endian).
  TEXT_ENCODING_UTF32_LE = 39,
  //! @brief UTF-32 text-codec (big-endian).
  TEXT_ENCODING_UTF32_BE = 40,

  //! @brief UCS-2 text-codec (little-endian).
  TEXT_ENCODING_UCS2_LE = 41,
  //! @brief UCS-2 text-codec (big-endian).
  TEXT_ENCODING_UCS2_BE = 42,

  //! @brief UTF-16 text-codec (native-endian).
  TEXT_ENCODING_UTF16 = FOG_BYTE_ORDER_CHOICE(TEXT_ENCODING_UTF16_LE, TEXT_ENCODING_UTF16_BE),
  //! @brief UTF-16 text-codec (swapped-endian).
  TEXT_ENCODING_UTF16_SWAPPED = FOG_BYTE_ORDER_CHOICE(TEXT_ENCODING_UTF16_BE, TEXT_ENCODING_UTF16_LE),

  //! @brief UTF-32 text-codec (native-endian).
  TEXT_ENCODING_UTF32 = FOG_BYTE_ORDER_CHOICE(TEXT_ENCODING_UTF32_LE, TEXT_ENCODING_UTF32_BE),
  //! @brief UTF-32 text-codec (swapped-endian).
  TEXT_ENCODING_UTF32_SWAPPED = FOG_BYTE_ORDER_CHOICE(TEXT_ENCODING_UTF32_BE, TEXT_ENCODING_UTF32_LE),

  //! @brief UCS-2 text-codec (native-endian).
  TEXT_ENCODING_UCS2 = FOG_BYTE_ORDER_CHOICE(TEXT_ENCODING_UCS2_LE, TEXT_ENCODING_UCS2_BE),
  //! @brief UCS-2 text-codec (swapped-endian).
  TEXT_ENCODING_UCS2_SWAPPED = FOG_BYTE_ORDER_CHOICE(TEXT_ENCODING_UCS2_BE, TEXT_ENCODING_UCS2_LE),

  //! @brief wchar_t text-codec (native-endian)
  TEXT_ENCODING_WCHAR_T = sizeof(wchar_t) == 2 ? TEXT_ENCODING_UTF16 : TEXT_ENCODING_UTF32,

  //! @brief Count of text-codecs.
  TEXT_ENCODING_COUNT = 43
};

// ============================================================================
// [Fog::TEXT_ENCODING_FLAGS]
// ============================================================================

//! @brief Text-codec flags.
enum TEXT_ENCODING_FLAGS
{
  //! @brief Text-codec is table based.
  TEXT_ENCODING_IS_TABLE = 0x00000001,
  //! @brief Text-codec is unicode based (UTF-8, UTF-16, UTF-32 or UCS-2).
  TEXT_ENCODING_IS_UNICODE = 0x00000002,

  //! @brief Text-codec base is little-endian.
  TEXT_ENCODING_IS_LE = 0x00000004,
  //! @brief Text-codec base is big-endian.
  TEXT_ENCODING_IS_BE = 0x00000008,

  //! @brief Text-codec is 8-bit.
  TEXT_ENCODING_IS_8BIT = 0x00000010,
  //! @brief Text-codec is 16-bit.
  TEXT_ENCODING_IS_16BIT = 0x00000020,
  //! @brief Text-codec is 32-bit.
  TEXT_ENCODING_IS_32BIT = 0x00000040,

  //! @brief Text-codec is variable-length (UTF-8 and UTF-16 codecs).
  TEXT_ENCODING_IS_VARLEN = 0x00000080
};

// ============================================================================
// [Fog::TEXT_JUSTIFY]
// ============================================================================

enum TEXT_JUSTIFY
{
  TEXT_JUSTIFY_LEFT = 0x1,
  TEXT_JUSTIFY_RIGHT = 0x2,
  TEXT_JUSTIFY_CENTER = 0x3
};

// ============================================================================
// [Fog::TEXT_HINT]
// ============================================================================

enum TEXT_HINT
{
  TEXT_HINT_PRECISE = 0,
  TEXT_HINT_ALIGNED_VERTICAL = 1,
  TEXT_HINT_ALIGNED_BOTH = 2
};

// ============================================================================
// [Fog::TEXT_SCRIPT]
// ============================================================================

enum TEXT_SCRIPT
{
  // ${TEXT_SCRIPT_ENUM:BEGIN}
  // --- Auto-generated by GenUnicode.py (Unicode 6.0.0) ---

  TEXT_SCRIPT_UNKNOWN = 0,
  TEXT_SCRIPT_COMMON = 1,
  TEXT_SCRIPT_INHERITED = 2,
  TEXT_SCRIPT_LATIN = 3,
  TEXT_SCRIPT_ARABIC = 4,
  TEXT_SCRIPT_ARMENIAN = 5,
  TEXT_SCRIPT_AVESTAN = 6,
  TEXT_SCRIPT_BALINESE = 7,
  TEXT_SCRIPT_BAMUM = 8,
  TEXT_SCRIPT_BATAK = 9,
  TEXT_SCRIPT_BENGALI = 10,
  TEXT_SCRIPT_BOPOMOFO = 11,
  TEXT_SCRIPT_BRAHMI = 12,
  TEXT_SCRIPT_BRAILLE = 13,
  TEXT_SCRIPT_BUGINESE = 14,
  TEXT_SCRIPT_BUHID = 15,
  TEXT_SCRIPT_CANADIAN_ABORIGINAL = 16,
  TEXT_SCRIPT_CARIAN = 17,
  TEXT_SCRIPT_CHAM = 18,
  TEXT_SCRIPT_CHEROKEE = 19,
  TEXT_SCRIPT_COPTIC = 20,
  TEXT_SCRIPT_CUNEIFORM = 21,
  TEXT_SCRIPT_CYPRIOT = 22,
  TEXT_SCRIPT_CYRILLIC = 23,
  TEXT_SCRIPT_DEVANAGARI = 24,
  TEXT_SCRIPT_DESERET = 25,
  TEXT_SCRIPT_EGYPTIAN_HIEROGLYPHS = 26,
  TEXT_SCRIPT_ETHIOPIC = 27,
  TEXT_SCRIPT_GEORGIAN = 28,
  TEXT_SCRIPT_GLAGOLITIC = 29,
  TEXT_SCRIPT_GOTHIC = 30,
  TEXT_SCRIPT_GREEK = 31,
  TEXT_SCRIPT_GUJARATI = 32,
  TEXT_SCRIPT_GURMUKHI = 33,
  TEXT_SCRIPT_HAN = 34,
  TEXT_SCRIPT_HANGUL = 35,
  TEXT_SCRIPT_HANUNOO = 36,
  TEXT_SCRIPT_HEBREW = 37,
  TEXT_SCRIPT_HIRAGANA = 38,
  TEXT_SCRIPT_IMPERIAL_ARAMAIC = 39,
  TEXT_SCRIPT_INSCRIPTIONAL_PAHLAVI = 40,
  TEXT_SCRIPT_INSCRIPTIONAL_PARTHIAN = 41,
  TEXT_SCRIPT_JAVANESE = 42,
  TEXT_SCRIPT_KAITHI = 43,
  TEXT_SCRIPT_KANNADA = 44,
  TEXT_SCRIPT_KATAKANA = 45,
  TEXT_SCRIPT_KAYAH_LI = 46,
  TEXT_SCRIPT_KHAROSHTHI = 47,
  TEXT_SCRIPT_KHMER = 48,
  TEXT_SCRIPT_LAO = 49,
  TEXT_SCRIPT_LEPCHA = 50,
  TEXT_SCRIPT_LIMBU = 51,
  TEXT_SCRIPT_LINEAR_B = 52,
  TEXT_SCRIPT_LISU = 53,
  TEXT_SCRIPT_LYCIAN = 54,
  TEXT_SCRIPT_LYDIAN = 55,
  TEXT_SCRIPT_MALAYALAM = 56,
  TEXT_SCRIPT_MANDAIC = 57,
  TEXT_SCRIPT_MEETEI_MAYEK = 58,
  TEXT_SCRIPT_MONGOLIAN = 59,
  TEXT_SCRIPT_MAYANMAR = 60,
  TEXT_SCRIPT_NEW_TAI_LUE = 61,
  TEXT_SCRIPT_NKO = 62,
  TEXT_SCRIPT_OGHAM = 63,
  TEXT_SCRIPT_OL_CHIKI = 64,
  TEXT_SCRIPT_OLD_ITALIC = 65,
  TEXT_SCRIPT_OLD_PERSIAN = 66,
  TEXT_SCRIPT_OLD_SOUTH_ARABIAN = 67,
  TEXT_SCRIPT_OLD_TURKIC = 68,
  TEXT_SCRIPT_ORIYA = 69,
  TEXT_SCRIPT_OSMANYA = 70,
  TEXT_SCRIPT_PHAGS_PA = 71,
  TEXT_SCRIPT_PHOENICIAN = 72,
  TEXT_SCRIPT_REJANG = 73,
  TEXT_SCRIPT_RUNIC = 74,
  TEXT_SCRIPT_SAMARITAN = 75,
  TEXT_SCRIPT_SAURASHTRA = 76,
  TEXT_SCRIPT_SHAVIAN = 77,
  TEXT_SCRIPT_SINHALA = 78,
  TEXT_SCRIPT_SUNDANESE = 79,
  TEXT_SCRIPT_SYLOTI_NAGRI = 80,
  TEXT_SCRIPT_SYRIAC = 81,
  TEXT_SCRIPT_TAGALOG = 82,
  TEXT_SCRIPT_TAGBANWA = 83,
  TEXT_SCRIPT_TAI_LE = 84,
  TEXT_SCRIPT_TAI_THAM = 85,
  TEXT_SCRIPT_TAI_VIET = 86,
  TEXT_SCRIPT_TAMIL = 87,
  TEXT_SCRIPT_TELUGU = 88,
  TEXT_SCRIPT_THAANA = 89,
  TEXT_SCRIPT_THAI = 90,
  TEXT_SCRIPT_TIBETAN = 91,
  TEXT_SCRIPT_TIFINAGH = 92,
  TEXT_SCRIPT_UGARITIC = 93,
  TEXT_SCRIPT_VAI = 94,
  TEXT_SCRIPT_YI = 95,

  TEXT_SCRIPT_COUNT = 96

  // --- Auto-generated by GenUnicode.py (Unicode 6.0.0) ---
  // ${TEXT_SCRIPT_ENUM:END}
};

// ============================================================================
// [Fog::TEXTURE_TILE]
// ============================================================================

//! @brief Texture tiling mode (see @c Texture).
enum TEXTURE_TILE
{
  TEXTURE_TILE_PAD = 0,
  TEXTURE_TILE_REPEAT = 1,
  TEXTURE_TILE_REFLECT = 2,
  TEXTURE_TILE_CLAMP = 3,

  TEXTURE_TILE_DEFAULT = TEXTURE_TILE_REPEAT,
  TEXTURE_TILE_COUNT = 4
};

// ============================================================================
// [Fog::TICKS_PRECISION]
// ============================================================================

enum TICKS_PRECISION
{
  TICKS_PRECISION_LOW = 0,
  TICKS_PRECISION_HIGH = 1
};

// ============================================================================
// [Fog::TIME_MS_PER_... / TIME_US_PER_...]
// ============================================================================

static const int64_t TIME_MS_PER_SECOND = FOG_INT64_C(        1000);
static const int64_t TIME_MS_PER_MINUTE = FOG_INT64_C(       60000);
static const int64_t TIME_MS_PER_HOUR   = FOG_INT64_C(     3600000);
static const int64_t TIME_MS_PER_DAY    = FOG_INT64_C(    86400000);
static const int64_t TIME_MS_PER_WEEK   = FOG_INT64_C(   604800000);

static const int64_t TIME_US_PER_MS     = FOG_INT64_C(        1000);
static const int64_t TIME_US_PER_SECOND = FOG_INT64_C(     1000000);
static const int64_t TIME_US_PER_MINUTE = FOG_INT64_C(    60000000);
static const int64_t TIME_US_PER_HOUR   = FOG_INT64_C(  3600000000);
static const int64_t TIME_US_PER_DAY    = FOG_INT64_C( 86400000000);
static const int64_t TIME_US_PER_WEEK   = FOG_INT64_C(604800000000);

// ============================================================================
// [Fog::TIME_ZONE]
// ============================================================================

//! @brief Time zone used by @c Date class.
enum TIME_ZONE
{
  //! @brief Universal time zone.
  TIME_ZONE_UTC = 0,
  //! @brief Local time zone.
  TIME_ZONE_LOCAL = 1,

  //! @brief Count of time zones.
  TIME_ZONE_COUNT = 2
};

// ============================================================================
// [Fog::TRANSFORM_CREATE]
// ============================================================================

//! @brief Type of matrix to create.
enum TRANSFORM_CREATE
{
  TRANSFORM_CREATE_IDENTITY = 0,
  TRANSFORM_CREATE_TRANSLATION = 1,
  TRANSFORM_CREATE_SCALING = 2,
  TRANSFORM_CREATE_ROTATION = 3,
  TRANSFORM_CREATE_SKEWING = 4,
  TRANSFORM_CREATE_LINE_SEGMENT = 5,
  TRANSFORM_CREATE_REFLECTION_U = 6,
  TRANSFORM_CREATE_REFLECTION_XY = 7,
  TRANSFORM_CREATE_REFLECTION_UNIT = 8,
  TRANSFORM_CREATE_PARALLELOGRAM = 9,
  TRANSFORM_CREATE_QUAD_TO_QUAD = 10,

  TRANSFORM_CREATE_COUNT = 11
};

// ============================================================================
// [Fog::TRANSFORM_OP]
// ============================================================================

//! @brief Type of transform operation.
enum TRANSFORM_OP
{
  //! @brief Translate matrix.
  TRANSFORM_OP_TRANSLATE = 0,
  //! @brief Scale matrix.
  TRANSFORM_OP_SCALE = 1,
  //! @brief Rotate matrix.
  TRANSFORM_OP_ROTATE = 2,
  //! @brief Rotate matrix (about a point).
  TRANSFORM_OP_ROTATE_PT = 3,
  //! @brief Skew matrix.
  TRANSFORM_OP_SKEW = 4,
  //! @brief Flip matrix.
  TRANSFORM_OP_FLIP = 5,
  //! @brief Multiply with other matrix.
  TRANSFORM_OP_MULTIPLY = 6,
  //! @brief Multiply with other matrix, but invert it before multiplication.
  TRANSFORM_OP_MULTIPLY_INV = 7,

  //! @brief Count of matrix transform operations.
  TRANSFORM_OP_COUNT = 8
};

// ============================================================================
// [Fog::TRANSFORM_TYPE]
// ============================================================================

//! @brief Type of transform.
enum TRANSFORM_TYPE
{
  //! @brief Transform type is identity (all zeros, 1 at diagonals).
  TRANSFORM_TYPE_IDENTITY = 0,
  //! @brief Transform type is translation (_20, _21 elements are used).
  TRANSFORM_TYPE_TRANSLATION = 1,
  //! @brief Transform type is scaling (_00, _11, _20, _21 elements are used).
  TRANSFORM_TYPE_SCALING = 2,
  //! @brief Transform type is swap (_01, _10, _20, _21 elements are used).
  TRANSFORM_TYPE_SWAP = 3,
  //! @brief Transform type is rotation (affine part is used).
  TRANSFORM_TYPE_ROTATION = 4,
  //! @brief Transform type is affine.
  TRANSFORM_TYPE_AFFINE = 5,
  //! @brief Transform type is projection.
  TRANSFORM_TYPE_PROJECTION = 6,
  //! @brief Transform type is degenerate (same as projection, but degenerated).
  TRANSFORM_TYPE_DEGENERATE = 7,

  //! @brief Count of transform types (for asserts, ...).
  TRANSFORM_TYPE_COUNT = 8,

  //! @brief Matrix is dirty.
  TRANSFORM_TYPE_DIRTY = 0x8
};

// ============================================================================
// [Fog::TYPEINFO_TYPE]
// ============================================================================

//! @brief Types for @c Fog::TypeInfo, use together with @c _FOG_TYPEINFO_DECLARE
//! macro.
enum TYPEINFO_TYPE
{
  //! @brief Primitive type, for example @c int, @c float, @c Date,
  //! @c Time, ...
  //!
  //! Simple data can be copied, moved, and destroyed without calling
  //! constructor, copy operator, or destructor.
  TYPEINFO_PRIMITIVE = 0,

  //! @brief Movable type, for example @c String, @c Image, ...
  //!
  //! Movable class can be moved, without calling constructor/copy-operator,
  //! and destructor, but can't be copied. Generally all classes used by Fog
  //! container must be primitive or movable.
  TYPEINFO_MOVABLE = 1,

  //! @brief Complex type.
  //!
  //! Unknown type can't be moved, copied, or freed, without calling the
  //! constructor, copy-operator, or destructor.
  TYPEINFO_COMPLEX = 2
};

// ============================================================================
// [Fog::TYPEINFO_FLAGS]
// ============================================================================

// TODO: Refactor:
// TYPEINFO_IS_COMPARABLE

//! @brief Additional flags for @c _FOG_TYPEINFO_DECLARE. All flags are initially
//! unset when used @c _FOG_TYPEINFO_DECLARE to declare information about a type.
enum TYPEINFO_FLAGS
{
  TYPEINFO_IS_POD_TYPE    = 0x00000100,
  TYPEINFO_IS_FLOAT_TYPE  = 0x00000200,
  TYPEINFO_IS_DOUBLE_TYPE = 0x00000400,
  TYPEINFO_HAS_COMPARE    = 0x00000800,
  TYPEINFO_HAS_EQ         = 0x00001000,
  TYPEINFO_MASK           = 0xFFFFFF00
};

// ============================================================================
// [Fog::UNICODE_CHAR]
// ============================================================================

enum UNICODE_CHAR
{
  // --------------------------------------------------------------------------
  // [UTF16_BOM]
  // --------------------------------------------------------------------------

  //! @brief UTF-16 Native BOM Mark.
  UTF16_BOM_MARK = 0xFEFF,
  //! @brief UTF-16 Byte-Swapped BOM Mark.
  UTF16_BOM_SWAP = 0xFFFE,

  // --------------------------------------------------------------------------
  // [UTF32_BOM]
  // --------------------------------------------------------------------------

  //! @brief UTF-32 Native BOM (Byte-Order-Mark).
  UTF32_BOM_NATIVE = 0x0000FEFFU,
  //! @brief UTF-32 Swapped BOM (Byte-Order-Mark).
  UTF32_BOM_SWAPPED = 0xFFFE0000U,

  // --------------------------------------------------------------------------
  // [UNICODE_MAX]
  // --------------------------------------------------------------------------

  //! @brief Maximum valid unicode code-point.
  UNICODE_MAX = 0x0010FFFF
};

// ============================================================================
// [Fog::UNIT]
// ============================================================================

//! @brief Coordinate units that can be used by the @c Dpi and @c Font classes.
//!
//! Coordinate units can be used to create display independent graphics, keeping
//! the coordinates in device independent units and translating them into
//! device pixel by Fog-G2d engine.
enum UNIT
{
  //! @brief No unit (compatible to @c UNIT_PX).
  UNIT_NONE = 0,
  //! @brief Pixel.
  UNIT_PX,

  //! @brief Point, 1 [pt] == 1/72 [in].
  UNIT_PT,
  //! @brief Pica, 1 [pc] == 12 [pt].
  UNIT_PC,

  //! @brief Inch, 1 [in] == 2.54 [cm].
  UNIT_IN,

  //! @brief Millimeter.
  UNIT_MM,
  //! @brief Centimeter.
  UNIT_CM,

  //! @brief Used for coordinates which depends on the object bounding box.
  UNIT_PERCENTAGE,

  //! @brief The font-size of the relevant font (see @c Font).
  UNIT_EM,
  //! @brief The x-height of the relevant font (see @c Font).
  UNIT_EX,

  //! @brief Count of coord units.
  UNIT_COUNT
};

// ============================================================================
// [Fog::USER_DIRECTORY]
// ============================================================================

enum USER_DIRECTORY
{
  USER_DIRECTORY_HOME = 0,
  USER_DIRECTORY_DESKTOP = 1,
  USER_DIRECTORY_DOCUMENTS = 2,
  USER_DIRECTORY_MUSIC = 3,
  USER_DIRECTORY_PICTURES = 4,
  USER_DIRECTORY_VIDEOS = 5,

  USER_DIRECTORY_COUNT = 6
};

// ============================================================================
// [Fog::VALUE_TYPE]
// ============================================================================

//! @brief Value type id.
enum VALUE_TYPE
{
  //! @brief Value is null.
  VALUE_TYPE_NULL = 0,
  //! @brief Value is signed 32-bit integer.
  VALUE_TYPE_INTEGER = 1,
  //! @brief Value is double-precision floating point.
  VALUE_TYPE_DOUBLE = 2,
  //! @brief Value is string.
  VALUE_TYPE_STRING = 3
};

// ============================================================================
// [Fog::XML_ELEMENT]
// ============================================================================

//! @brief Xml element type.
enum XML_ELEMENT
{
  XML_ELEMENT_BASE = 0x01,
  XML_ELEMENT_TEXT = 0x03,
  XML_ELEMENT_CDATA = 0x04,
  XML_ELEMENT_PI = 0x07,
  XML_ELEMENT_COMMENT = 0x08,
  XML_ELEMENT_DOCUMENT = 0x09,

  XML_ELEMENT_MASK = 0x0F,

  // Svg support.
  SVG_ELEMENT_MASK = 0x10,
  SVG_ELEMENT_BASE = XML_ELEMENT_BASE | SVG_ELEMENT_MASK,
  SVG_ELEMENT_DOCUMENT = XML_ELEMENT_DOCUMENT | SVG_ELEMENT_MASK
};

// ============================================================================
// [Fog::XML_FLAGS]
// ============================================================================

enum XML_FLAGS
{
  //! @brief Whether element can be manipulated (DOM).
  XML_ALLOWED_DOM_MANIPULATION = 0x01,
  //! @brief Whether element tag name can be changed.
  XML_ALLOWED_TAG = 0x02,
  //! @brief Whether element supports attributes.
  XML_ALLOWED_ATTRIBUTES = 0x04
};

// ============================================================================
// [Fog::EVENT_CORE]
// ============================================================================

//! @brief Core event IDs.
enum EVENT_CORE_ENUM
{
  // --------------------------------------------------------------------------
  // [Null]
  // --------------------------------------------------------------------------

  //! @brief Null event code.
  //!
  //! Reserved, shouldn't be used.
  EVENT_NULL = 0,

  // --------------------------------------------------------------------------
  // [Object - Create / Destroy]
  // --------------------------------------------------------------------------

  EVENT_CREATE,
  EVENT_DESTROY,
  EVENT_DELETE,

  // --------------------------------------------------------------------------
  // [Object - Properties]
  // --------------------------------------------------------------------------

  EVENT_PROPERTY,

  // --------------------------------------------------------------------------
  // [Object - Hierarchy]
  // --------------------------------------------------------------------------

  EVENT_CHILD_ADD,
  EVENT_CHILD_REMOVE,

  // --------------------------------------------------------------------------
  // [Timer]
  // --------------------------------------------------------------------------

  EVENT_TIMER,

  // --------------------------------------------------------------------------
  // [User]
  // --------------------------------------------------------------------------

  //! @brief First user event.
  EVENT_USER = 65536,

  // --------------------------------------------------------------------------
  // [UID]
  // --------------------------------------------------------------------------

  //! @brief Last event that can be used by all libraries.
  //!
  //! This number is first retrieved by Fog::Event::uid() and incremented
  //! each time the function is called.
  EVENT_UID = 10000000
};

// ============================================================================
// [Fog::ERR_ENUM]
// ============================================================================

//! @brief Error codes used by Fog-Framework.
enum ERR_ENUM
{
  // --------------------------------------------------------------------------
  // [Global/Ok]
  // --------------------------------------------------------------------------

  //! @brief Function proceed without errors (success).
  ERR_OK = 0,

  // --------------------------------------------------------------------------
  // [Global/Range]
  // --------------------------------------------------------------------------

  //! @brief First error code that can be used by Fog-Framework.
  _ERR_RANGE_FIRST = 0x00010000,
  //! @brief Last error code that can be used by Fog-Framework.
  _ERR_RANGE_LAST  = 0x0001FFFF,

  // --------------------------------------------------------------------------
  // [Global/Run-Time]
  // --------------------------------------------------------------------------

  //! @brief Failed to allocate memory.
  ERR_RT_OUT_OF_MEMORY = _ERR_RANGE_FIRST,

  //! @brief Failed to create thread (or to get thread from a thread pool).
  ERR_RT_OUT_OF_THREADS,

  //! @brief Assertion failure when running in release-mode.
  //!
  //! Please use issue tracker if you get this error, because it can be caused
  //! by your code or Fog-Framework itself. Fog never return this error code
  //! when compiled in debug-mode, instead the assertion is raised.
  ERR_RT_ASSERTION_FAILURE,

  //! @brief Some code you wanted to use is not implemented yet.
  //!
  //! Please use issue tracker if feature you need is not implemented yet.
  ERR_RT_NOT_IMPLEMENTED,

  //! @brief Invalid argument passed to the Fog-Framework function.
  //!
  //! If you encounter such error then it's problem on the your side and you
  //! should fix your code.
  ERR_RT_INVALID_ARGUMENT,

  //! @brief Invalid context means that member method you called can't do the
  //! job, because instance state not allows it to do it.
  ERR_RT_INVALID_OBJECT,

  //! @brief Invalid state.
  ERR_RT_INVALID_STATE,

  //! @brief Overflow in integer or floating point arithmetic.
  ERR_RT_OVERFLOW,

  ERR_RT_BUSY,

  ERR_RT_INVALID_HANDLE,
  ERR_RT_OBJECT_NOT_FOUND,
  ERR_RT_OBJECT_ALREADY_EXISTS,

  // --------------------------------------------------------------------------
  // [Core/DateTime]
  // --------------------------------------------------------------------------

  ERR_DATE_INVALID,

  // --------------------------------------------------------------------------
  // [Core/IO]
  // --------------------------------------------------------------------------

  // TODO: What is difference between ERR_IO_TOO_BIG and ERR_IO_FILE_TOO_BIG.

  ERR_IO_TOO_BIG,
  ERR_IO_NOT_A_FILE,
  ERR_IO_NOT_A_DIRECTORY,
  ERR_IO_FILE_IS_EMPTY,

  ERR_IO_FILE_TOO_BIG,

  ERR_IO_CANT_READ,
  ERR_IO_CANT_WRITE,
  ERR_IO_CANT_SEEK,
  ERR_IO_CANT_RESIZE,
  ERR_IO_CANT_TRUNCATE,

  ERR_IO_FILE_NOT_EXISTS,
  ERR_IO_DIR_ALREADY_EXISTS,

  // --------------------------------------------------------------------------
  // [Core/Library]
  // --------------------------------------------------------------------------

  ERR_LIBRARY_LOAD_FAILED,
  ERR_LIBRARY_NO_SYMBOL,

  // --------------------------------------------------------------------------
  // [Core/Locale]
  // --------------------------------------------------------------------------

  ERR_LOCALE_NOT_MATCHED,

  // --------------------------------------------------------------------------
  // [Core/Environment]
  // --------------------------------------------------------------------------

  ERR_ENV_GET_FAILED,
  ERR_ENV_SET_FAILED,

  // --------------------------------------------------------------------------
  // [Core/Object]
  // --------------------------------------------------------------------------

  //! @brief Object is not part of a hierarchy.
  //!
  //! Tried to remove object from a bad ascendant.
  //!
  //! @note This is very likely a runtime error and should be reported.
  ERR_OBJECT_NOT_PART_OF_HIERARCHY,

  //! @brief Object has already part of a different hierarchy.
  //!
  //! Tried to add object to another, but the object was already added to
  //! another else. You must first remove it from its current parent.
  //!
  //! @note This is very likely a runtime error and should be reported.
  ERR_OBJECT_ALREADY_PART_OF_HIERARCHY,

  //! @brief Property not exists.
  ERR_OBJECT_INVALID_PROPERTY,
  //! @brief Property is read-only.
  ERR_OBJECT_READ_ONLY_PROPERTY,

  // TODO: Shouldn't we use ERR_RT_INVALID_ARGUMENT instead?
  ERR_OBJECT_INVALID_VALUE,

  // --------------------------------------------------------------------------
  // [Core/Text]
  // --------------------------------------------------------------------------

  //! @brief Invalid text input (converting strings to numbers).
  ERR_STRING_INVALID_INPUT,

  //! @brief Invalid text codec (Null) used to encode / decode string.
  ERR_STRING_INVALID_CODEC,

  //! @brief Invalid UTF-8 sequence.
  ERR_STRING_INVALID_UTF8,

  //! @brief Invalid UTF-16 sequence (surrogate pair error).
  ERR_STRING_INVALID_UTF16,

  //! @brief Invalid UCS-2 sequence (surrogate pair in UCS-2).
  ERR_STRING_INVALID_UCS2,

  //! @brief Invalid Unicode character.
  ERR_STRING_INVALID_CHAR,

  //! @brief Truncated (incomplete) input buffer.
  //!
  //! This error can be returned if you passed UTF-16 string to some function
  //! and the string ends with UTF-16 lead surrogate character.
  ERR_STRING_TRUNCATED,

  //! @brief Some characters lost during conversion (replaced by '?' or a given replacer).
  //!
  //! This can only happen when converting unicode to non-unicode encoding.
  ERR_STRING_LOST,


  // --------------------------------------------------------------------------
  // [Core/Thread]
  // --------------------------------------------------------------------------

  //! @brien Invalid TLS index catched by @c ThreadLocal.
  ERR_THREAD_TLS_INVALID,
  //! @brief TLS indexes exhausted.
  ERR_THREAD_TLS_EXHAUSTED,

  // --------------------------------------------------------------------------
  // [Core/User]
  // --------------------------------------------------------------------------

  ERR_USER_NO_HOME_DIRECTORY,
  ERR_USER_NO_XDG_DIRECTORY,

  // --------------------------------------------------------------------------
  // [Core/Xml]
  // --------------------------------------------------------------------------

  ERR_XML_INTERNAL,

  // XmlDom Errors.
  ERR_XML_DOCUMENT_INVALID_CHILD,
  ERR_XML_MANUPULATION_NOT_ALLOWED,
  ERR_XML_TAG_CHANGE_NOT_ALLOWED,
  ERR_XML_ATTRIBUTES_NOT_ALLOWED,
  ERR_XML_NOT_A_TEXT_NODE,
  ERR_XML_ATTRIBUTE_NOT_EXISTS,
  ERR_XML_ATTRIBUTE_CANT_BE_REMOVED,
  ERR_XML_INVALID_ATTRIBUTE,
  ERR_XML_CYCLIC,
  ERR_XML_INVALID_CHILD,
  ERR_XML_INVALID_TAG_NAME,
  ERR_XML_DOCUMENT_HAS_ALREADY_ROOT,

  // XmlSaxReader Errors.
  ERR_XML_NO_DOCUMENT,
  ERR_XML_MISSING_ROOT_TAG,
  ERR_XML_MISSING_TAG,
  ERR_XML_MISSING_ATTRIBUTE,
  ERR_XML_UNMATCHED_CLOSING_TAG,
  ERR_XML_UNCLOSED_CDATA,
  ERR_XML_UNCLOSED_PI,
  ERR_XML_UNCLOSED_COMMENT,
  ERR_XML_UNCLOSED_DOCTYPE,
  ERR_XML_SYNTAX_ERROR,

  ERR_XML_INVALID_CLOSING_TAG,

  // --------------------------------------------------------------------------
  // [G2d/Imaging]
  // --------------------------------------------------------------------------

  //! @brief Image size is invalid or zero.
  ERR_IMAGE_INVALID_SIZE,

  //! @brief Image format is invalid.
  ERR_IMAGE_INVALID_FORMAT,

  // TODO: Remove
  ERR_IMAGE_UNSUPPORTED_FORMAT,

  ERR_IMAGEIO_INTERNAL_ERROR,

  ERR_IMAGE_NO_DECODER,
  ERR_IMAGE_NO_ENCODER,

  ERR_IMAGEIO_UNSUPPORTED_FORMAT,
  ERR_IMAGE_TERMINATED,
  ERR_IMAGE_TRUNCATED,

  ERR_IMAGE_MIME_NOT_MATCH,
  ERR_IMAGE_MALFORMED_HEADER,
  ERR_IMAGE_MALFORMED_RLE,
  ERR_IMAGE_NO_MORE_FRAMES,

  ERR_IMAGE_LIBJPEG_NOT_LOADED,
  ERR_IMAGE_LIBJPEG_ERROR,

  ERR_IMAGE_LIBPNG_NOT_LOADED,
  ERR_IMAGE_LIBPNG_ERROR,

  //! @brief Failed to load Gdi+ library (Windows).
  ERR_IMAGE_GDIPLUS_NOT_LOADED,

  //! @brief Call to Gdi+ library failed and resulting error can't be
  //! translated to the Fog one. This is a generic error.
  ERR_IMAGE_GDIPLUS_ERROR,

  // --------------------------------------------------------------------------
  // [G2d/Geometry]
  // --------------------------------------------------------------------------

  //! @brief No shape to work with.
  //!
  //! This may happen when some method is requested on an empty path or shape
  //! that is @c SHAPE_TYPE_NONE. If this error is returned then the output
  //! of the functions shouldn't be considered as correct. For example the
  //! bounding-box of shape which is invalid will be [0, 0, 0, 0], but it
  //! shouldn't be used/merged or analyzed, because the shape doesn't exist.
  ERR_GEOMETRY_NONE,

  //! @brief Invalid shape.
  //!
  //! This may happen if @c PathF or @c PathD contains invalid data (although
  //! this might be considered as the run-time error) or if some basic shape
  //! is incorrectly given (for example if @c RectF or @c RectD with or height
  //! is negative).
  ERR_GEOMETRY_INVALID,

  //! @brief The transform to be used is degenerated.
  //!
  //! The degenerated transform can't be used in geometry, because the result
  //! is simply nothing - for example rectange might degenerate to rectangle
  //! with zero width or height and it won't be painted. To prevent errors
  //! caused by degenerate transform use @c TransformF::isDegenerate() and
  //! @c TransformD::isDegenerate().
  //!
  //! @note This error is always related to trasnform.
  ERR_GEOMETRY_DEGENERATE,

  //! @brief Can't stroke the path or shape.
  ERR_GEOMETRY_CANT_STROKE,

  //! @brief It is required that a previous path command is a vertex,
  //!
  //! To add @c PATH_CMD_LINE_TO, @c PATH_CMD_QUAD_TO, @c PATH_CMD_CUBIC_TO or
  //! @c PATH_CMD_CLOSE the previous command must be a vertex. The only command
  //! that do not require such condition is @c PATH_CMD_MOVE_TO.
  ERR_PATH_NO_VERTEX,

  //! @brief The relative command can't be added, because the previous command
  //! is not a vertex.
  ERR_PATH_NO_RELATIVE,

  // --------------------------------------------------------------------------
  // [G2d/Painting]
  // --------------------------------------------------------------------------

  //! @brief Image format is not supported by the paint engine.
  //!
  //! The error can be misleading, but it can be returned by @c Painter::begin()
  //! where the image argument points to image that is using @c IMAGE_FORMAT_I8.
  //!
  //! Painter can be used only to paint into supported pixel formats (all RGB,
  //! ARGB and ALPHA-only pixel formats are supported).
  ERR_PAINTER_UNSUPPORTED_FORMAT,

  //! @brief Error that can be returned by painter if it's illegal to change
  //! paint engine at this time.
  //!
  //! This can happen if multithreaded or hardware accelerated painter is used
  //! and you want to switch to a different paint engine.
  //!
  //! @sa @c Painter::setEngine().
  ERR_PAINTER_NOT_ALLOWED,

  //! @brief There is not state to restore, might be caused by
  //! @ref Painter::restore().
  ERR_PAINTER_NO_STATE,

  // --------------------------------------------------------------------------
  // [G2d/Text]
  // --------------------------------------------------------------------------

  ERR_FONT_NOT_MATCHED,
  ERR_FONT_INVALID_FACE,
  ERR_FONT_INVALID_DATA,
  ERR_FONT_CANT_LOAD_DEFAULT_FACE,
  ERR_FONT_CANT_GET_OUTLINE,

  ERR_FONT_FONTCONFIG_NOT_LOADED,
  ERR_FONT_FONTCONFIG_INIT_FAILED,

  ERR_FONT_FREETYPE_NOT_LOADED,
  ERR_FONT_FREETYPE_INIT_FAILED,

  ERR_FONT_INTERNAL,

  // --------------------------------------------------------------------------
  // [G2d/Tools]
  // --------------------------------------------------------------------------

  //! @brief Can't do binary algebra using infinite region as a source or
  //! destination.
  //!
  //! The algebra is possible, but resulting region could be very large and
  //! all other operations can be insecure. This can happen if you want to
  //! XOR simple or complex region by infinite one.
  ERR_REGION_INFINITE
};










// TODO: Unfinished.

// ============================================================================
// [Fog::IMAGE_EFFECT]
// ============================================================================

//! @brief Type of image filter, see @c ImageFxFilter and @c ColorFilter classes.
enum IMAGE_EFFECT
{
  //! @brief Image effect is none (COPY).
  IMAGE_EFFECT_NONE = 0,

  //! @brief @c Color adjust image effect.
  IMAGE_EFFECT_COLOR_ADJUST = 1,
  //! @brief @c Color LUT image effect.
  IMAGE_EFFECT_COLOR_LUT = 2,
  //! @brief @c Color matrix image effect.
  IMAGE_EFFECT_COLOR_MATRIX = 3,

  //! @brief Image filter is box blur.
  IMAGE_EFFECT_BLUR = 4,
  //! @brief Image filter is convolution (using convolution matrix).
  IMAGE_EFFECT_CONVOLVE_MATRIX = 5,
  //! @brief Image filter is convolution (using convolution vectors - horizontal and vertical).
  IMAGE_EFFECT_CONVOLVE_VECTOR = 6,

  // TODO: Image effects.
  IMAGE_EFFECT_EMBOSS = 7,
  IMAGE_EFFECT_MORPHOLOGY = 8,
  IMAGE_EFFECT_SHARPEN = 9,
  IMAGE_EFFECT_SHADOW = 10,

  IMAGE_EFFECT_COUNT = 11
};

// ============================================================================
// [Fog::IMAGE_EFFECT_CHAR]
// ============================================================================

//! @brief Characteristics of image filter.
//!
//! Characteristics can be used to improve performance of filters by @c Painter.
enum IMAGE_EFFECT_CHAR
{
  //! @brief Image filter does only color transformations.
  //!
  //! This flag must set all color filter, because it's very useful hint that
  //! enables very good code optimizations inside @c Painter and @c Image
  //! classes.
  IMAGE_EFFECT_CHAR_COLOR_TRANSFORM = 0x0001,

  //! @brief Image filter can extend image boundary (blur and convolution
  //! filters).
  IMAGE_EFFECT_CHAR_CAN_EXTEND = 0x0002,

  //! @brief Image filter constains standard processing mechanism - one pass.
  IMAGE_EFFECT_CHAR_ENTIRE_PROCESSING = 0x0004,

  //! @brief When doing entire processing the destination and source buffers
  //! can be shared (dst and src pointers can point to same location).
  IMAGE_EFFECT_CHAR_ENTIRE_MEM_EQUAL = 0x0008,

  //! @brief Image filter does vertical processing of image.
  //!
  //! This bit is set for all blur/convolution filters. Performance of filter
  //! is usually degraded, because filter processing function needs to access
  //!  pixels in different scanlines (cache misses, etc...).
  //!
  //! @note Vertical processing can be combined with horizontal processing and
  //! painter tries to make this combination efficient.
  IMAGE_EFFECT_CHAR_VERT_PROCESSING = 0x0010,

  //! @brief When doing vertical processing the destination and source buffers
  //! can be shared (dst and src pointers can point to same location).
  IMAGE_EFFECT_CHAR_VERT_MEM_EQUAL = 0x0020,

  //! @brief Image filter does horizontal processing of image.
  //!
  //! If filter needs only horizontal (no IMAGE_EFFECT_VERT_PROCESSING bit is
  //! set) then processing it can be very efficient in multithreaded painter
  //! engine.
  IMAGE_EFFECT_CHAR_HORZ_PROCESSING = 0x0040,

  //! @brief When doing vertical processing the destination and source buffers
  //! can be shared (dst and src pointers can point to same location).
  IMAGE_EFFECT_CHAR_HORZ_MEM_EQUAL = 0x0080,

  //! @brief Contains both, @c IMAGE_EFFECT_VERT_PROCESSING and
  //! @c IMAGE_EFFECT_HORZ_PROCESSING flags.
  IMAGE_EFFECT_CHAR_HV_PROCESSING =
    IMAGE_EFFECT_CHAR_VERT_PROCESSING |
    IMAGE_EFFECT_CHAR_HORZ_PROCESSING ,

  //! @brief Image filter supports @c IMAGE_FORMAT_PRGB32.
  IMAGE_EFFECT_CHAR_SUPPORTS_PRGB32 = 0x0100,

  //! @brief Image filter supports @c IMAGE_FORMAT_XRGB32.
  //!
  //! @note This flag should be always set!
  IMAGE_EFFECT_CHAR_SUPPORTS_XRGB32 = 0x0400,

  //! @brief Image filter supports @c IMAGE_FORMAT_A8.
  //!
  //! @note This flag should be always set!
  IMAGE_EFFECT_CHAR_SUPPORTS_A8 = 0x0800,

  //! @brief Image filter supports alpha-channel promotion. This means that
  //! source image without alpha-channel can be converted to image with
  //! alpha-channel.
  //!
  //! This operation is supported by all blur-filters (and should be supported
  //! generally by all filters that extend image boundary).
  IMAGE_EFFECT_CHAR_PROMOTE_ALPHA = 0x1000
};

// ============================================================================
// [Fog::BLUR_FX_TYPE]
// ============================================================================

//! @brief Type of blur, see @c ImageFxFilter.
enum BLUR_FX_TYPE
{
  //! @brief The linear-blur effect (default).
  //!
  //! The Linear-blur effect quality is between box-blur and gaussian-blur.
  //! The result and performance of this effect is optimal for the most
  //! operations.
  BLUR_FX_LINEAR = 0,

  //! @brief The box-blur effect.
  //!
  //! The box-blur effect is low-level quality blur, but very efficient.
  BLUR_FX_BOX = 1,

  //! @brief The gaussian-blur type.
  //!
  //! The gaussian-blur effect is high-quality blur, but computation intensive
  //! (poor performance).
  BLUR_FX_GAUSSIAN = 2,

  //! @brief The default blur type.
  BLUR_FX_DEFAULT = BLUR_FX_LINEAR,

  //! @brief Count of blur effects.
  BLUR_FX_COUNT = 3
};

//! @brief Maximum blur radius.
static const float BLUR_FX_MAX_RADIUS = 255.0f;

// ============================================================================
// [Fog::IMAGE_EFFECT_ALPHA_MODE]
// ============================================================================

enum IMAGE_EFFECT_ALPHA_MODE
{
  IMAGE_EFFECT_ALPHA_NORMAL,
  IMAGE_EFFECT_ALPHA_INCLUDE
};

// ============================================================================
// [Fog::BORDER_EXTEND_TYPE]
// ============================================================================

//! @brief Border extend mode used by image effects (convolution and blurs).
enum BORDER_EXTEND_TYPE
{
  //! @brief Borders are extended by a color.
  BORDER_EXTEND_COLOR = 0,

  //! @brief Borders are extended using pad.
  BORDER_EXTEND_PAD = 1,

  //! @brief Borders are extended using repead.
  BORDER_EXTEND_REPEAT = 2,

  //! @brief Borders are extended using reflect.
  BORDER_EXTEND_REFLECT = 3,

  //! @brief Default border extend type.
  BORDER_EXTEND_DEFAULT = BORDER_EXTEND_COLOR,

  //! @brief Count of border extend types (for error checking).
  BORDER_EXTEND_COUNT = 4
};











//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_CONSTANTS_H
