// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_TOOLS_KEYUTIL_H
#define _FOG_UI_TOOLS_KEYUTIL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Face/FaceC.h>

namespace Fog {

//! @addtogroup Fog_UI_Tools
//! @{

// ============================================================================
// [Fog::UIUtil]
// ============================================================================

//! @brief Key code/modifiers helpers.
struct FOG_NO_EXPORT UIUtil
{
  static FOG_INLINE bool isClearMod   (uint32_t mod) { return (mod & MODIFIER_MASK  ) == 0; }
  static FOG_INLINE bool isAltMod     (uint32_t mod) { return (mod & MODIFIER_ALT   ) != 0; }
  static FOG_INLINE bool isAltOnly    (uint32_t mod) { return (mod & MODIFIER_MASK  ) == (mod & MODIFIER_ALT); }
  static FOG_INLINE bool isShiftMod   (uint32_t mod) { return (mod & MODIFIER_SHIFT ) != 0; }
  static FOG_INLINE bool isShiftOnly  (uint32_t mod) { return (mod & MODIFIER_MASK  ) == (mod & MODIFIER_SHIFT); }
  static FOG_INLINE bool isCtrlMod    (uint32_t mod) { return (mod & MODIFIER_CTRL  ) != 0; }
  static FOG_INLINE bool isCtrlOnly   (uint32_t mod) { return (mod & MODIFIER_MASK  ) == (mod & MODIFIER_CTRL); }
  static FOG_INLINE bool isCapsLockMod(uint32_t mod) { return (mod & MODIFIER_CAPS  ) != 0; }
  static FOG_INLINE bool isNumLockMod (uint32_t mod) { return (mod & MODIFIER_NUM   ) != 0; }
  static FOG_INLINE bool isModeMod    (uint32_t mod) { return (mod & MODIFIER_MODE  ) != 0; }

  static FOG_INLINE uint32_t getButtonIndexFromCode(uint32_t button)
  {
    FOG_ASSERT(button != 0);

    uint32_t result;
    Face::p32CTZ(result, button);
    return result;
  }

  static FOG_INLINE uint32_t getButtonCodeFromIndex(uint32_t index)
  {
    FOG_ASSERT(index <= 31);
    return (1 << index);
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_TOOLS_KEYUTIL_H
