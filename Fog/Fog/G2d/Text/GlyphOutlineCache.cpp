// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/Alloc.h>
#include <Fog/G2d/Text/GlyphOutlineCache.h>

namespace Fog {

// ============================================================================
// [Fog::GlyphOutlineCache - Helpers]
// ============================================================================

static FOG_INLINE size_t _GlyphOutlineCache_getUsedBytes(const GlyphOutline& glyph)
{
  return sizeof(PathDataF) + (
    (glyph.getOutline().getCapacity() * (sizeof(PointF) + 1) + 15 ) & ~15
  );
}

// ============================================================================
// [Fog::GlyphOutlineCache - Construction / Destruction]
// ============================================================================

GlyphOutlineCache::GlyphOutlineCache()
{
  _usedBytes = 0;

  _tableSize = 16;
  _tableData = reinterpret_cast<Table**>(Memory::calloc(_tableSize * sizeof(Table*)));
  if (FOG_IS_NULL(_tableData)) goto _Fail;

  _tableData[0] = reinterpret_cast<Table*>(Memory::calloc(sizeof(Table)));
  if (FOG_IS_NULL(_tableData[0])) goto _FailFreeTable;

  // Success.
  return;

_FailFreeTable:
  Memory::free(_tableData);
  _tableData = NULL;

_Fail:
  _tableSize = 0;
}

GlyphOutlineCache::~GlyphOutlineCache()
{
  if (FOG_IS_NULL(_tableData)) return;

  reset();
  Memory::free(_tableData);
}

// ============================================================================
// [Fog::GlyphOutlineCache - Reset]
// ============================================================================

void GlyphOutlineCache::reset()
{
  FOG_ASSERT(_tableData != NULL);

  Table** table = _tableData;
  uint32_t tRow, size = _tableSize;

  for (tRow = 0; tRow < size; tRow++)
  {
    Table* t = table[tRow];

    if (t != NULL)
    {
      for (uint32_t tCol = 0; tCol < 256; tCol++)
      {
        if (!t->glyphs[tCol]->_isNull()) t->glyphs[tCol].destroy();
      }

      Memory::free(t);
      table[tRow] = NULL;
    }
  }

  _usedBytes = 0;
}

// ============================================================================
// [Fog::GlyphOutlineCache - Methods]
// ============================================================================

err_t GlyphOutlineCache::put(uint32_t uc, const GlyphOutline& glyph)
{
  FOG_ASSERT(_tableData != NULL);

  uint32_t tRow = uc >> 8;
  uint32_t tCol = uc & 0xFF;

  // Realloc table rows if needed (and align the count of rows to 32).
  if (FOG_UNLIKELY(tRow >= _tableSize))
  {
    // Make sure 32 is added, because tRow needs to be valid after realloc!
    uint32_t s = (tRow + 32) & ~31;
    Table** newt = reinterpret_cast<Table**>(Memory::realloc(_tableData, s * sizeof(Table*)));

    if (FOG_IS_NULL(newt)) return ERR_RT_OUT_OF_MEMORY;
    Memory::zero(newt + _tableSize, (s - _tableSize) * sizeof(Table*));

    _tableSize = s;
    _tableData = newt;
  }

  // Alloc table column if needed.
  Table* t = _tableData[tRow];
  if (FOG_IS_NULL(t))
  {
    t = reinterpret_cast<Table*>(Memory::calloc(sizeof(Table)));
    if (FOG_IS_NULL(t)) return ERR_RT_OUT_OF_MEMORY;

    _tableData[tRow] = t;
  }

  // Put the glyph into the cache, ignoring the state that it's already there
  if (t->glyphs[tCol]->_isNull())
  {
    glyph._d->outline->squeeze();
    t->glyphs[tCol].initCustom1(glyph);

    // Update the statistics.
    _usedBytes += _GlyphOutlineCache_getUsedBytes(glyph);
  }

  return ERR_OK;
}

} // Fog namespace
