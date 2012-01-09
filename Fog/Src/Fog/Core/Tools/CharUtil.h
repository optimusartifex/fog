// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_CHARUTIL_H
#define _FOG_CORE_TOOLS_CHARUTIL_H

// [Dependencies]
#include <Fog/Core/Tools/CharData.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::CharUtil]
// ============================================================================

struct FOG_NO_EXPORT CharUtil
{
  // --------------------------------------------------------------------------
  // [ISO-15924]
  // --------------------------------------------------------------------------
  
  //! @brief Normalize ISO15924 Form of text script into "Xxxx" form.
  //!
  //! @note This kind of normalization is tricky and expects that the @a iso15924
  //! value is in valid form.
  static FOG_INLINE uint32_t getNormalizedISO15924Name(uint32_t iso15924)
  {
    return (iso15924 & 0xDFDFDFDF) | 0x00202020;
  }

  //! @brief Translate TEXT_SCRIPT value into the ISO15924 form stored as uint32_t.
  static FOG_INLINE uint32_t getIso15924FromTextScript(uint32_t script)
  {
    return fog_api.charutil_getIso15924FromTextScript(script);
  }

  //! @brief Translate ISO15924 code stored in uint32_t to TEXT_SCRIPT.
  static FOG_INLINE uint32_t getTextScriptFromIso15924(uint32_t iso15924)
  {
    return fog_api.charutil_getTextScriptFromIso15924(iso15924);
  }
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_TOOLS_CHARUTIL_H
