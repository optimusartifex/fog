// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_GLYPHIMAGE_H
#define _FOG_G2D_TEXT_GLYPHIMAGE_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Text/GlyphMetrics.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::GlyphBitmapData]
// ============================================================================

struct FOG_NO_EXPORT GlyphBitmapData
{
  //! @brief Reference count.
  mutable Atomic<size_t> refCount;

  //! @brief Metrics.
  GlyphMetricsF metrics;

  //! @brief Outline.
  Static<Image> bitmap[3];
};

// ============================================================================
// [Fog::GlyphBitmap]
// ============================================================================

struct FOG_API GlyphBitmap
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GlyphBitmap();
  GlyphBitmap(const GlyphBitmap& other);
  ~GlyphBitmap();

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  err_t create(const GlyphMetricsF& metrics, const Image& bitmap);

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool _isNull() const { return _d == NULL; }

  FOG_INLINE const GlyphMetricsF& getMetrics() const
  {
    return _d->metrics;
  }

  FOG_INLINE const Image& getBitmap() const
  {
    return _d->bitmap[0].instance();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  GlyphBitmap& operator=(const GlyphBitmap& other);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(GlyphBitmapData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::GlyphBitmap, Fog::TYPEINFO_MOVABLE)

// ============================================================================
// [Fog::Swap<>]
// ============================================================================

_FOG_SWAP_D(Fog::GlyphBitmap)

// [Guard]
#endif // _FOG_G2D_TEXT_GLYPHIMAGE_H
