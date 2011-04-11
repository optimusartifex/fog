// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_FONT_GLYPHCACHE_H
#define _FOG_G2D_FONT_GLYPHCACHE_H

// [Dependencies]
#include <Fog/G2d/Font/Glyph.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::GlyphCache]
// ============================================================================

//! @brief Glyph cache.
//!
//! Glyphs are stored in sparse Nx256 array. First pointer points to second
//! array where are stored real glyphs. This cache is per face and glyph
//! attribute. The most optimized method is @c get() for retrieving the glyph.
//!
//! Address for glyph 'uc' is row[uc >> 8][uc & 255].
struct FOG_API GlyphCache
{
  typedef GlyphData* Entity;

  GlyphCache();
  ~GlyphCache();

  FOG_INLINE Entity get(uint32_t uc) const
  {
    Entity* row;
    uint32_t ucRow = uc >> 8;
    return ucRow < _count && (row = _rows[ucRow]) ? row[uc & 0xFF] : 0;
  }

  bool set(uint32_t uc, Entity data);

  void reset();

protected:
  //! @brief Array of pointers to glyphs.
  Entity** _rows;
  //! @brief Count of first rows pointer. Initial value is 0.
  sysuint_t _count;

private:
  FOG_DISABLE_COPY(GlyphCache)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_FONT_GLYPHCACHE_H
