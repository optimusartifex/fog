// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OPENTYPE_OTUTIL_H
#define _FOG_G2D_TEXT_OPENTYPE_OTUTIL_H

// [Dependencies]
#include <Fog/G2d/Text/OpenType/OTApi.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text_OpenType
//! @{

// ============================================================================
// [Fog::OTUtil]
// ============================================================================

//! @brief TrueType/OpenType utilities.
struct FOG_NO_EXPORT OTUtil
{
  // --------------------------------------------------------------------------
  // [InitAligned]
  // --------------------------------------------------------------------------

  //! @brief Get whether to use special code to work with a table aligned to 16-bits.
  //!
  //! This function returns always false on platforms where unaligned read can
  //! be performed. On platforms where unaligned read is expensive or forbidden
  //! the @a data pointer is checked and return value depends on real alignment.
  static bool FOG_INLINE initAligned16(const void* data)
  {
#if defined(FOG_ARCH_UNALIGNED_ACCESS_16)
    return false;
#else
    return (uintptr_t(data) & 0x1) == 0;
#endif // FOG_ARCH_UNALIGNED_ACCESS_16
  }

  //! @brief Get whether to use special code to work with a table aligned to 32-bits.
  //!
  //! This function returns always false on platforms where unaligned read can
  //! be performed. On platforms where unaligned read is expensive or forbidden
  //! the @a data pointer is checked and return value depends on real alignment.
  static bool FOG_INLINE initAligned32(const void* data)
  {
#if defined(FOG_ARCH_UNALIGNED_ACCESS_32)
    return false;
#else
    return (uintptr_t(data) & 0x3) == 0;
#endif // FOG_ARCH_UNALIGNED_ACCESS_32
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OPENTYPE_OTUTIL_H
