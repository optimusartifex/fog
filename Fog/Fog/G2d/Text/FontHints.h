// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_FONTHINTS_H
#define _FOG_G2D_TEXT_FONTHINTS_H

// [Dependencies]
#include <Fog/Core/Collection/HashUtil.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/Ops.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::FontHints]
// ============================================================================

//! @brief Font options.
struct FOG_NO_EXPORT FontHints
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FontHints()
  {
    reset();
  }

  FOG_INLINE FontHints(
    uint8_t weight,
    uint8_t style,
    uint8_t variant,
    uint8_t decoration,
    uint8_t kerning = FONT_KERNING_DETECT,
    uint8_t hinting = FONT_HINTING_DETECT,
    uint8_t alignMode = FONT_ALIGN_MODE_DETECT,
    uint8_t quality = FONT_QUALITY_DETECT)
  {
    _weight = weight;
    _style = style;
    _variant = variant;
    _decoration = decoration;

    _kerning = kerning;
    _hinting = hinting;
    _alignMode = alignMode;
    _quality = quality;
  }

  FOG_INLINE FontHints(const FontHints& other)
  {
    Memory::copy_t<FontHints>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t getWeight() const { return _weight; }
  FOG_INLINE uint8_t getStyle() const { return _style; }
  FOG_INLINE uint8_t getVariant() const { return _variant; }
  FOG_INLINE uint8_t getDecoration() const { return _decoration; }

  FOG_INLINE uint8_t getKerning() const { return _kerning; }
  FOG_INLINE uint8_t getHinting() const { return _hinting; }
  FOG_INLINE uint8_t getAlignMode() const { return _alignMode; }
  FOG_INLINE uint8_t getQuality() const { return _quality; }

  FOG_INLINE void setWeight(uint32_t weight) { _weight = (uint8_t)weight; }
  FOG_INLINE void setStyle(uint32_t style) { _style = (uint8_t)style; }
  FOG_INLINE void setVariant(uint32_t variant) { _variant = (uint8_t)variant; }
  FOG_INLINE void setDecoration(uint32_t decoration) { _decoration = (uint8_t)decoration; }

  FOG_INLINE void setKerning(uint32_t kerning) { _kerning = (uint8_t)kerning; }
  FOG_INLINE void setHinting(uint32_t hinting) { _hinting = (uint8_t)hinting; }
  FOG_INLINE void setAlignMode(uint32_t alignMode) { _alignMode = (uint8_t)alignMode; }
  FOG_INLINE void setQuality(uint32_t quality) { _quality = (uint8_t)quality; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _weight = FONT_WEIGHT_NORMAL;
    _style = FONT_STYLE_NORMAL;
    _decoration = FONT_DECORATION_NONE;
    _variant = FONT_VARIANT_NORMAL;

    _kerning = FONT_KERNING_DETECT;
    _hinting = FONT_HINTING_DETECT;
    _alignMode = FONT_ALIGN_MODE_DETECT;
    _quality = FONT_QUALITY_DETECT;
  }

  // --------------------------------------------------------------------------
  // [HashCode]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getHashCode() const
  {
    return HashUtil::combineHash(
      _data[0],
      _data[1]);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FontHints& operator=(const FontHints& other)
  {
    Memory::copy_t<FontHints>(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FontHints& other) const
  {
    return Memory::eq_t<FontHints>(this, &other);
  }

  FOG_INLINE bool operator!=(const FontHints& other) const
  {
    return !operator==(other);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union
  {
    struct
    {
      //! @brief Font weight.
      uint8_t _weight;
      //! @brief Font style.
      uint8_t _style;
      //! @brief Font decoration.
      uint8_t _decoration;
      //! @brief Font variant.
      uint8_t _variant;

      //! @brief Font kerning.
      uint8_t _kerning;
      //! @brief Font hinting.
      uint8_t _hinting;
      //! @brief Font align-mode.
      uint8_t _alignMode;
      //! @brief Font quality.
      uint8_t _quality;
    };

    //! @brief Data as 4-BYTE (DWORD) array (for fast compare/copy/set).
    uint32_t _data[2];
  };
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_FONTHINTS_H
