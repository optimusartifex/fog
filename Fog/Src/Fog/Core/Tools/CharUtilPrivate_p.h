// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_CHARUTIL_PRIVATE_P_H
#define _FOG_CORE_TOOLS_CHARUTIL_PRIVATE_P_H

// [Dependencies]
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/CharData.h>
#include <Fog/Core/Tools/CharUtil.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::CharUtilPrivate]
// ============================================================================

struct FOG_NO_EXPORT CharUtilPrivate
{
  static FOG_INLINE bool maybeHangul(uint16_t c)
  {
    return Math::isBounded<uint16_t>(c, CHAR_HANGUL_BASE_L, CHAR_HANGUL_BASE_S + CHAR_HANGUL_COUNT_S - 1);
  }

  static FOG_INLINE bool maybeHangul(uint32_t c)
  {
    return Math::isBounded<uint32_t>(c, CHAR_HANGUL_BASE_L, CHAR_HANGUL_BASE_S + CHAR_HANGUL_COUNT_S - 1);
  }

  static FOG_INLINE uint32_t composeBSearch(const CharComposition* data, uint32_t length, uint32_t a)
  {
    for (uint32_t lim = length; lim != 0; lim >>= 1)
    {
      const CharComposition* cur = data + (lim >> 1);

      if (a > cur->getA())
      {
        data = cur + 1;
        lim--;
        continue;
      }

      if (a < cur->getA())
        continue;

      return cur->getAB();
    }

    return 0;
  }
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_TOOLS_CHARUTIL_PRIVATE_P_H
