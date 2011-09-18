// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_ENUMCORE_H
#define _FOG_CORE_GLOBAL_ENUMCORE_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Fog::DETECT_LENGTH / INVALID_INDEX]
// ============================================================================

//! @brief Detect length means that string function should first detect it length.
static const size_t DETECT_LENGTH = SIZE_MAX;
//! @brief Returned by functions like indexOf() to indicate that element wasn't
//! found.
static const size_t INVALID_INDEX = SIZE_MAX;

static const size_t NO_PRECISION = SIZE_MAX;
static const size_t NO_WIDTH = SIZE_MAX;

//! @brief how much to reserve stack for local strings in Fog library or all
//! additional libraries.
static const size_t TEMPORARY_LENGTH = 256;

// ============================================================================
// [Fog::NO_FLAGS_ENUM]
// ============================================================================

enum NO_FLAGS_ENUM
{
  //! @brief Convenience value that can be used instead of zero when flag
  //! variable is created, but with all bits set to zero.
  NO_FLAGS = 0
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
// [Fog::CASE_SENSITIVITY]
// ============================================================================

//! @brief Case sensitivity, used by @c StringW and @c StringA classes.
enum CASE_SENSITIVITY
{
  //! @brief Case sensitive (search, replace, indexOf, lastIndexOf, ...).
  CASE_SENSITIVE = 0,
  //! @brief Case insensitive (search, replace, indexOf, lastIndexOf, ...).
  CASE_INSENSITIVE = 1,

  CASE_SENSITIVITY_COUNT = 2
};

// ============================================================================
// [Fog::CHAR_BIDI]
// ============================================================================

//! @brief Character BIDI class.
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
// [Fog::CHAR_CASE_FOLDING]
// ============================================================================

//! @brief Type of case-folding (as defined by the unicode standard).
enum CHAR_CASE_FOLDING
{
  //! @brief Simple case-folding, mapping characters one-to-one.
  CHAR_CASE_FOLDING_SIMPLE = 0,

  //! @brief Full case-folding, mapping characters one-to-many (ligatures).
  //!
  //! @see @c CHAR_CASE_FOLDING_SIMPLE.
  CHAR_CASE_FOLDING_FULL = 1,

  //! @brief Special case-folding for uppercase I and dotted uppercase I.
  //!
  //! - For non-Turkic languages, this mapping is normally not used.
  //! - For Turkic languages (tr, az), this mapping can be used instead of the
  //!   normal mapping for these characters.
  CHAR_CASE_FOLDING_SIMPLE_SPECIAL = 2,

  //! @brief Full+special case-folding (mapping characters one-to-many).
  //!
  //! @see @c CHAR_CASE_FOLDING_SIMPLE_SPECIAL.
  CHAR_CASE_FOLDING_FULL_SPECIAL = 3
};

// ============================================================================
// [Fog::CHAR_CATEGORY]
// ============================================================================

//! @brief Character category.
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
  CHAR_UNICODE_V6_1 = 13,

  //! @brief Default unicode version to use when unassigned.
  CHAR_UNICODE_DEFAULT = CHAR_UNICODE_V6_0
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
  //! @brief Unknown CPU.
  CPU_VENDOR_UNKNOWN = 0,

  //! @brief Intel CPU.
  CPU_VENDOR_INTEL = 1,
  //! @brief AMD CPU.
  CPU_VENDOR_AMD = 2,
  //! @brief National Semiconductor CPU (applies also to Cyrix processors).
  CPU_VENDOR_NSM = 3,
  //! @brief Transmeta CPU.
  CPU_VENDOR_TRANSMETA = 4,
  //! @brief VIA CPU.
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
  //! @brief Decimal double form (compatible to "%f").
  DF_DECIMAL = 0,

  //! @brief Exponent double form (compatible to "%e").
  DF_EXPONENT = 1,

  //! @brief Significant digits (compatible to "%g").
  DF_SIGNIFICANT_DIGITS = 2
};

// ============================================================================
// [Fog::DIR_ENTRY]
// ============================================================================

//! @brief Directory-entry type.
enum DIR_ENTRY
{
  DIR_ENTRY_UNKNOWN = 0,
  DIR_ENTRY_FILE = 0x0001,
  DIR_ENTRY_DIRECTORY = 0x0002,
  DIR_ENTRY_CHAR_DEVICE = 0x0004,
  DIR_ENTRY_BLOCK_DEVICE = 0x0008,
  DIR_ENTRY_FIFO = 0x0010,
  DIR_ENTRY_LINK = 0x0020,
  DIR_ENTRY_SOCKET = 0x0040
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
// [Fog::LIBRARY_PATH]
// ============================================================================

enum LIBRARY_PATH
{
  LIBRARY_PATH_PREPEND = 0,
  LIBRARY_PATH_APPEND = 1
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
  MATH_SOLVE_QUARTIC = 2,

  MATH_SOLVE_COUNT = 3
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
// [Fog::OS_WIN_VERSION]
// ============================================================================

enum OS_WIN_VERSION
{
  //! @brief Unknown windows version (probably too old).
  OS_WIN_VERSION_UNKNOWN = 0,

  //! @brief Set if system is WinNT.
  OS_WIN_VERSION_NT = 1,
  //! @brief Set if system is WinNT 3.x.
  OS_WIN_VERSION_NT_3 = 2,
  //! @brief Set if system is WinNT 4.x.
  OS_WIN_VERSION_NT_4 = 3,
  //! @brief Set if system is Windows 2000.
  OS_WIN_VERSION_2000 = 4,
  //! @brief Set if system is Windows XP.
  OS_WIN_VERSION_XP = 5,
  //! @brief Set if system is Windows Server 2003.
  OS_WIN_VERSION_2003 = 6,
  //! @brief Set if system is Windows Server 2003-R2.
  OS_WIN_VERSION_2003_R2 = 7,
  //! @brief Set if system is Windows Vista.
  OS_WIN_VERSION_VISTA = 8,
  //! @brief Set if system is Windows Server 2008.
  OS_WIN_VERSION_2008 = 9,
  //! @brief Set if system is Windows Server 2008-R2.
  OS_WIN_VERSION_2008_R2 = 10,
  //! @brief Set if system is Windows 7.
  OS_WIN_VERSION_7 = 11,

  OS_WIN_VERSION_FUTURE = 0xFF
};

// ============================================================================
// [Fog::REGEXP_TYPE]
// ============================================================================

enum REGEXP_TYPE
{
  REGEXP_TYPE_NONE = 0,
  REGEXP_TYPE_PATTERN = 1,
  REGEXP_TYPE_WILDCARD = 2,
  REGEXP_TYPE_REGEXP = 3,

  REGEXP_TYPE_COUNT = 4
};
// ============================================================================
// [Fog::SLASH_FORM]
// ============================================================================

enum SLASH_FORM
{
  SLASH_FORM_FORWARD = 0,
  SLASH_FORM_BACKWARD = 1,

  SLASH_FORM_UNIX = SLASH_FORM_FORWARD,
  SLASH_FORM_WINDOWS = SLASH_FORM_BACKWARD,

  SLASH_FORM_NATIVE =
#if defined(FOG_OS_WINDOWS)
    SLASH_FORM_WINDOWS,
#else
    SLASH_FORM_UNIX,
#endif // FOG_OS_WINDOWS

  SLASH_FORM_COUNT = 2
};

// ============================================================================
// [Fog::SORT_ORDER]
// ============================================================================

enum SORT_ORDER
{
  SORT_ORDER_ASCENDING = 0,
  SORT_ORDER_DESCENDING = 1
};

// ============================================================================
// [Fog::SPLIT_BEHAVIOR]
// ============================================================================

enum SPLIT_BEHAVIOR
{
  SPLIT_KEEP_EMPTY = 0,
  SPLIT_REMOVE_EMPTY = 1
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
// [Fog::STRING_FORMAT]
// ============================================================================

//! @brief StringW formatting flags.
enum STRING_FORMAT
{
  //! @brief Use alternate form (sprintf '#' flag).
  STRING_FORMAT_ALTERNATE = 0x0001,

  //! @brief Zero padded (sprintf '0' flag).
  STRING_FORMAT_ZERO_PAD = 0x0002,

  //! @brief Align to left (sprintf '-' flag).
  STRING_FORMAT_LEFT = 0x0004,

  //! @brief Blank character instead of positive sign (sprintf ' ' flag).
  FORMAT_FORMAT_BLANK = 0x0008,

  //! @brief Always show sign (sprintf '+' flag).
  STRING_FORMAT_SIGN = 0x0010,

  //! @brief Capitalize number output.
  STRING_FORMAT_CAPITALIZE = 0x0020,

  //! @brief Capitalize E (floats) or X (hexadecimal) in number output.
  STRING_FORMAT_CAPITALIZE_E_OR_X = 0x0040,

  //! @brief Show locale specific group separators (sprintf ''' flag).
  //!
  //! @note This flag is always ignored when using formatting on @c StringA
  //! destination.
  STRING_FORMAT_GROUP = 0x0080
};

// ============================================================================
// [Fog::STRING_PARSED]
// ============================================================================

//! @brief Flags which can be returned by various string-parsing classes.
enum STRING_PARSED
{
  //! @brief Some spaces was parsed and skipped in input.
  STRING_PARSED_SPACES = (1 << 1),
  //! @brief Sign was parsed.
  STRING_PARSED_SIGN = (1 << 2),
  //! @brief @c "0" octal prefix was parsed.
  STRING_PARSED_OCTAL_PREFIX = (1 << 3),
  //! @brief @c "0x" or @c "0X" hexadecimal prefix was parsed.
  STRING_PARSED_HEX_PREFIX = (1 << 4),
  //! @brief Decimal point was parsed.
  STRING_PARSED_DECIMAL_POINT = (1 << 5),
  //! @brief Exponent (@c "E" or @c "e") was parsed.
  STRING_PARSED_EXPONENT = (1 << 6)
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
// [Fog::TEXT_ENCODING_FLAG]
// ============================================================================

//! @brief Text-codec flags.
enum TEXT_ENCODING_FLAG
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
// [Fog::TYPE_CATEGORY]
// ============================================================================

//! @brief Type category, see @ref Fog::TypeInfo<>.
enum TYPE_CATEGORY
{
  //! @brief Simple type.
  //!
  //! Simple type can be created, copied, moved, and freed, without calling
  //! constructor, copy operator, or destructor. All primitive types like
  //! @c int, @ref Date, @c Time, ... are simple.
  TYPE_CATEGORY_SIMPLE = 0,

  //! @brief Movable type.
  //!
  //! Movable type can be moved in memory without calling constructor,
  //! copy-operator, and destructor, but can't be copied or freed without
  //! calling constructor or destructor. All implicitly shared containers used
  //! by Fog-Framework are movable. Types like @c StringA, @c StringW, @c
  //! Image, and many others are movable.
  TYPE_CATEGORY_MOVABLE = 1,

  //! @brief Complex type.
  //!
  //! Unknown type which can't be moved, copied, or freed, without calling the
  //! constructor, copy-operator, and destructor. This is the default type for
  //! any unknown class which is not specified by @c _FOG_TYPE_DECLARE() macro.
  TYPE_CATEGORY_COMPLEX = 2,

  //! @brief Category mask.
  TYPE_CATEGORY_MASK = 0xFF
};

// ============================================================================
// [Fog::TYPE_FLAG]
// ============================================================================

//! @brief Additional flags for @c _FOG_TYPE_DECLARE. All flags are initially
//! unset when used @c _FOG_TYPE_DECLARE to declare information about a type.
enum TYPE_FLAG
{
  //! @brief Type is POD.
  TYPE_FLAG_POD = 0x00000100,
  //! @brief Type is pointer.
  TYPE_FLAG_POINTER = 0x00000200,
  //! @brief Type is class which contains implicitly shared data aka 'D'.
  TYPE_FLAG_IMPLICIT = 0x00000400,
  //! @brief Type is class that is not initialized by its constructor.
  //!
  //! @note Uninitialized can be only @c TYPE_CATEGORY_SIMPLE.
  TYPE_FLAG_UNINITIALIZED = 0x00000800,

  //! @brief Type is integer.
  TYPE_FLAG_INTEGER = 0x00001000,
  //! @brief Type is single-precision floating-point.
  TYPE_FLAG_FLOAT = 0x00002000,
  //! @brief Type is double-precision floating-point.
  TYPE_FLAG_DOUBLE = 0x00004000,

  //! @brief Type doesn't contain equality method or operator.
  TYPE_FLAG_NO_EQ = 0x00010000,
  //! @brief Type provides it's own equality implementation through Type::eq().
  TYPE_FLAG_OWN_EQ = 0x00020000,
  //! @brief Type can be binary-compared for equality (optimization for
  //! containers).
  TYPE_FLAG_BIN_EQ = 0x00040000,

  //! @brief Type doesn't contain comparison methods and operators.
  TYPE_FLAG_NO_CMP = 0x00100000,
  //! @brief Type provides it's own equality implementation through Type::compare().
  TYPE_FLAG_OWN_CMP = 0x00200000,

  TYPE_FLAG_MASK = 0xFFFFFF00
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
// [Fog::VAR_TYPE]
// ============================================================================

//! @brief Variable type ID, used by @c Var and all binary compatible classes.
//!
//! @note Variable type ID is immutable. It is assigned when the instance is
//! created and immutable until the instance is destroyed.
enum VAR_TYPE
{
  VAR_TYPE_NULL = 0,
  VAR_TYPE_UNKNOWN = 0,

  VAR_TYPE_BOOL = 1,
  VAR_TYPE_CHAR = 2,
  VAR_TYPE_INT32 = 3,
  VAR_TYPE_UINT32 = 4,
  VAR_TYPE_INT64 = 5,
  VAR_TYPE_UINT64 = 6,
  VAR_TYPE_FLOAT = 7,
  VAR_TYPE_DOUBLE = 8,

  VAR_TYPE_STRINGA = 9,
  VAR_TYPE_STRINGW = 10,

  VAR_TYPE_LIST_STRINGA = 11,
  VAR_TYPE_LIST_STRINGW = 12,
  VAR_TYPE_LIST_VAR = 13,

  VAR_TYPE_HASH_STRINGA_STRINGA = 14,
  VAR_TYPE_HASH_STRINGA_VAR = 15,
  VAR_TYPE_HASH_STRINGW_STRINGW = 16,
  VAR_TYPE_HASH_STRINGW_VAR = 17,

  VAR_TYPE_REGEXPA = 18,
  VAR_TYPE_REGEXPW = 19,

  VAR_TYPE_LOCALE = 20,

  VAR_TYPE_DATE = 21,
  VAR_TYPE_TIME = 22,

  VAR_TYPE_POINTI = 32,
  VAR_TYPE_POINTF = 33,
  VAR_TYPE_POINTD = 34,
  VAR_TYPE_SIZEI = 35,
  VAR_TYPE_SIZEF = 36,
  VAR_TYPE_SIZED = 37,
  VAR_TYPE_BOXI = 38,
  VAR_TYPE_BOXF = 39,
  VAR_TYPE_BOXD = 40,
  VAR_TYPE_RECTI = 41,
  VAR_TYPE_RECTF = 42,
  VAR_TYPE_RECTD = 43,
  // Reserved for VAR_TYPE_LINEI.
  VAR_TYPE_LINEF = 45,
  VAR_TYPE_LINED = 46,
  VAR_TYPE_QBEZIERF = 47,
  VAR_TYPE_QBEZIERD = 48,
  VAR_TYPE_CBEZIERF = 49,
  VAR_TYPE_CBEZIERD = 50,
  VAR_TYPE_TRIANGLEF = 51,
  VAR_TYPE_TRIANGLED = 52,
  VAR_TYPE_ROUNDF = 53,
  VAR_TYPE_ROUNDD = 54,
  VAR_TYPE_CIRCLEF = 55,
  VAR_TYPE_CIRCLED = 56,
  VAR_TYPE_ELLIPSEF = 57,
  VAR_TYPE_ELLIPSED = 58,
  VAR_TYPE_ARCF = 59,
  VAR_TYPE_ARCD = 60,
  VAR_TYPE_CHORDF = 61,
  VAR_TYPE_CHORDD = 62,
  VAR_TYPE_PIEF = 63,
  VAR_TYPE_PIED = 64,
  VAR_TYPE_POLYGONF = 65,
  VAR_TYPE_POLYGOND = 66,
  VAR_TYPE_PATHF = 67,
  VAR_TYPE_PATHD = 68,
  VAR_TYPE_REGION = 69,

  VAR_TYPE_TRANSFORMF = 70,
  VAR_TYPE_TRANSFORMD = 71,

  VAR_TYPE_MATRIXF = 72,
  VAR_TYPE_MATRIXD = 73,

  VAR_TYPE_COLOR = 74,
  VAR_TYPE_COLOR_STOP = 75,
  VAR_TYPE_COLOR_STOP_LIST = 76,

  VAR_TYPE_TEXTURE,
  VAR_TYPE_GRADIENTF,
  VAR_TYPE_GRADIENTD,
  VAR_TYPE_PATTERN,

  VAR_TYPE_IMAGE,
  VAR_TYPE_IMAGE_PALETTE,

  VAR_TYPE_FONT,

  VAR_TYPE_OBJECT_REF = 0x00FFFFFF,

  VAR_TYPE_MASK = 0x00FFFFFF,

  _VAR_TYPE_INTEGER_START = 2,
  _VAR_TYPE_INTEGER_END = 6,

  _VAR_TYPE_NUMBER_START = 2,
  _VAR_TYPE_NUMBER_END = 8,

  _VAR_TYPE_GEOMETRY_START = VAR_TYPE_POINTI,
  _VAR_TYPE_GEOMETRY_END = VAR_TYPE_REGION
};

// ============================================================================
// [Fog::VAR_FLAG]
// ============================================================================

//! @brief Variable type flags, used by @c Var and all binary compatible classes.
//!
//! @note Variable type flags ara immutable. They are assigned when the instance
//! is created and immutable until the instance is destroyed. There are only few
//! special cases which break this rule - containers which support read-only data
//! (@c VAR_FLAG_READ_ONLY) contains adopt() member which is used to adopt the
//! data. In this case it's not required that the instance is destroyed, it can
//! be reused instead to adopt the data to prevent calling MemMgr::free() 
//! followed by MemMgr::alloc().
enum VAR_FLAG
{
  //! @brief Convenience to highlight that no variable flag is used in type
  //! definition.
  VAR_FLAG_NONE = 0x00000000,

  //! @brief Variable data is adopted from other variable or custom memory
  //! location.
  //!
  //! This flag means that the variable destructor can't free memory using
  //! standard MemMgr::free() call. The internal data structures allocated
  //! on the heap are freed normally.
  //!
  //! Not all containers support this flag. It's mainly designed for strings
  //! to prevent dynamic memory allocation for very short lived strings, 
  //! typically used as a buffer when converting ansi-string to wide-string
  //! or vica-versa to use a native operating system API.
  //!
  //! @note This flag doesn't mean that the object destruction can't be called,
  //! it means that IF the object destructor is called then it's used to
  //! prevent it to free itself. Please look at StringT_dFree() method to see
  //! how this flag is used in StringA/StringW data destructor.
  VAR_FLAG_STATIC = 0x01000000,

  //! @brief Variable is read-only.
  //!
  //! @note This flag is currently used only by @c Image.
  VAR_FLAG_READ_ONLY = 0x02000000,

  //! @brief Reserved, can be used for specific purposes by any type.
  VAR_FLAG_RESERVED_1 = 0x04000000,

  //! @brief Reserved, can be used for specific purposes by any type.
  VAR_FLAG_RESERVED_2 = 0x08000000,

  //! @brief Reserved, can be used for specific purposes by any type.
  VAR_FLAG_RESERVED_3 = 0x10000000,

  //! @brief Reserved, can be used for specific purposes by any type.
  VAR_FLAG_RESERVED_4 = 0x20000000,

  //! @brief Reserved, can be used for specific purposes by any type.
  VAR_FLAG_RESERVED_5 = 0x40000000,

  //! @brief Reserved, can be used for specific purposes by any type.
  VAR_FLAG_RESERVED_6 = 0x80000000,

  //! @brief Mask for all variable flags.
  VAR_FLAG_MASK = 0xFF000000
};

// ============================================================================
// [Fog::OBJECT_FLAG]
// ============================================================================

//! @brief @ref Object flags.
enum OBJECT_FLAG
{
  // --------------------------------------------------------------------------
  // [Core]
  // --------------------------------------------------------------------------

  //! @brief Object was created and create event was received.
  OBJECT_FLAG_CREATED = VAR_FLAG_RESERVED_1,
  //! @brief Object will be destroyed, method @c destroyLater() was called.
  OBJECT_FLAG_DESTROY_LATER = VAR_FLAG_RESERVED_2,

  // --------------------------------------------------------------------------
  // [Cast]
  // --------------------------------------------------------------------------

  // These flags were designed for fast object identification in a Fog-Gui
  // library. Generally all flags above 1 << 16 are used by Fog-Gui.

  OBJECT_FLAG_IS_COMPONENT = VAR_FLAG_RESERVED_4,
  //! @brief The @ref Object instance is @ref Widget.
  OBJECT_FLAG_IS_WIDGET = VAR_FLAG_RESERVED_5,
  //! @brief The @ref Object instance is @ref Layout.
  OBJECT_FLAG_IS_LAYOUT = VAR_FLAG_RESERVED_6
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

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_ENUMCORE_H
