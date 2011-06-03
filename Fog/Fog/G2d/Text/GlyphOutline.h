// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_GLYPHOUTLINE_H
#define _FOG_G2D_TEXT_GLYPHOUTLINE_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Text/GlyphMetrics.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::GlyphOutlineData]
// ============================================================================

struct FOG_NO_EXPORT GlyphOutlineData
{
  //! @brief Reference count.
  mutable Atomic<size_t> refCount;

  //! @brief Metrics.
  GlyphMetricsF metrics;

  //! @brief Outline.
  Static<PathF> outline;
};

// ============================================================================
// [Fog::GlyphOutline]
// ============================================================================

struct FOG_API GlyphOutline
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GlyphOutline();
  GlyphOutline(const GlyphOutline& other);
  ~GlyphOutline();

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  err_t create(const GlyphMetricsF& metrics, const PathF& outline);

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool _isNull() const { return _d == NULL; }

  FOG_INLINE const GlyphMetricsF& getMetrics() const
  {
    return _d->metrics;
  }

  FOG_INLINE const PathF& getOutline() const
  {
    return _d->outline.instance();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  GlyphOutline& operator=(const GlyphOutline& other);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(GlyphOutlineData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::GlyphOutline, Fog::TYPEINFO_MOVABLE)

// ============================================================================
// [Fog::Swap<>]
// ============================================================================

_FOG_SWAP_D(Fog::GlyphOutline)

// [Guard]
#endif // _FOG_G2D_TEXT_GLYPHOUTLINE_H
