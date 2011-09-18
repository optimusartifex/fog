// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_FONTFACE_H
#define _FOG_G2D_TEXT_FONTFACE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Text/FontKerning.h>
#include <Fog/G2d/Text/FontMetrics.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct GlyphMetricsF;
struct GlyphOutlineCache;
struct FontData;
struct FontProvider;
struct TextExtents;

// ============================================================================
// [Fog::FontFace]
// ============================================================================

//! @brief Font face.
struct FOG_API FontFace
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FontFace();
  virtual ~FontFace();

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE FontFace* addRef() const
  {
    reference.inc();
    return const_cast<FontFace*>(this);
  }

  FOG_INLINE void deref()
  {
    if (reference.deref()) fog_delete(this);
  }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual err_t getTextOutline(PathF& dst, const FontData* d, const PointF& pt, const StubW& str) = 0;
  virtual err_t getTextOutline(PathD& dst, const FontData* d, const PointD& pt, const StubW& str) = 0;

  virtual err_t getTextExtents(TextExtents& extents, const FontData* d, const StubW& str) = 0;

  // virtual GlyphOutlineCache* getOutlineCache() = 0;
  virtual FontKerningTableF* getKerningTable(const FontData* d) = 0;

  // --------------------------------------------------------------------------
  // [Private]
  // --------------------------------------------------------------------------

  virtual err_t _renderGlyphOutline(PathF& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx) = 0;
  virtual err_t _renderGlyphOutline(PathD& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx) = 0;

  err_t _getTextOutline(PathF& dst, GlyphOutlineCache* outlineCache, const FontData* d, const PointF& pt, const StubW& str, void* ctx);
  err_t _getTextOutline(PathD& dst, GlyphOutlineCache* outlineCache, const FontData* d, const PointD& pt, const StubW& str, void* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Font-face id.
  uint32_t id;

  //! @brief Font-face features.
  uint32_t features;

  //! @brief Font-face family.
  StringW family;

  //! @brief Design EM square
  //!
  //! Ideally in integers, but can be scaled if the exact information can't be
  //! fetched (Windows).
  float designEm;

  //! @brief Design metrics.
  //!
  //! Ideally in integers, but can be scaled if the exact information can't be
  //! fetched (happens under Windows).
  FontMetricsF designMetrics;

private:
  _FOG_NO_COPY(FontFace)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_FONTFACE_H
