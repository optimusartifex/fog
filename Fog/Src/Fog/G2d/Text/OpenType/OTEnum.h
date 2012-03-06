// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OTENUM_H
#define _FOG_G2D_TEXT_OTENUM_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text_OpenType
//! @{

// ============================================================================
// [Fog::OT_PLATFORM_ID]
// ============================================================================

//! @brief TrueType/OpenType platform ID.
enum OT_PLATFORM_ID
{
  //! @brief Unicode platform.
  //!
  //! @note When used, platformSpecificId indicates unicode version.
  OT_PLATFORM_ID_UNICODE = 0,

  //! @brief Mac platform.
  //!
  //! @note When used, platformSpecificId means mac script-manager code, see
  //! @ref OT_MAC_ID enumeration for possible values.
  OT_PLATFORM_ID_MAC = 1,

  //! @brief Deprecated, originally used by ISO/IEC 10646.
  OT_PLATFORM_ID_RESERVED = 2,

  //! @brief MS platform.
  //!
  //! @note When used, platformSpecificId indicates microsoft encoding.
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
  OT_MS_ID_BIG_5 = 4,
  //! @brief Korean Wansung encoding.
  OT_MS_ID_WANSUNG = 5,
  //! @brief Johab encoding.
  OT_MS_ID_JOHAB = 6,
  //! @brief UCS-4 charmaps.
  OT_MS_ID_UCS_4 = 10
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OTENUM_H
