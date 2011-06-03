// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_GLYPHOUTLINECACHE_H
#define _FOG_G2D_TEXT_GLYPHOUTLINECACHE_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/G2d/Text/GlyphOutline.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::GlyphOutlineCache]
// ============================================================================

struct FOG_API GlyphOutlineCache
{
  // --------------------------------------------------------------------------
  // [Structs]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT Table
  {
    Static<GlyphOutline> glyphs[256];
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GlyphOutlineCache();
  ~GlyphOutlineCache();

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    return _tableData != NULL;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE const GlyphOutline& get(uint32_t uc) const
  {
    FOG_ASSERT(_tableData != NULL);

    uint32_t tRow = uc >> 8;
    uint32_t tCol = uc & 0xFF;

    if (tRow >= _tableSize || _tableData[tRow] == NULL)
    {
      tRow = 0;
      tCol = 0;
    }

    return _tableData[tRow]->glyphs[tCol].instance();
  }

  err_t put(uint32_t uc, const GlyphOutline& glyph);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Number of bytes used by the cache.
  size_t _usedBytes;

  //! @brief Count of rows in cache-table.
  uint32_t _tableSize;
  //! @brief Cache-table.
  Table** _tableData;

private:
  _FOG_CLASS_NO_COPY(GlyphOutlineCache)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_GLYPHOUTLINECACHE_H
