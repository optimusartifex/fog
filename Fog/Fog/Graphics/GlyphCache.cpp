// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/GlyphCache.h>

namespace Fog {

// ============================================================================
// [Fog::GlyphCache]
// ============================================================================

GlyphCache::GlyphCache() :
  _rows(NULL),
  _count(0)
{
}

GlyphCache::~GlyphCache()
{
  reset();
}

bool GlyphCache::set(uint32_t uc, Entity data)
{
  // First reject too high character
  if (uc > 65535) return false;

  uint32_t ucRow = uc >> 8;
  uint32_t ucCol = uc & 0xFF;

  // Realloc ROWS array if needed.
  if (FOG_UNLIKELY(ucRow >= _count))
  {
    Entity** newRows = (Entity**)Memory::realloc(_rows, (ucRow + 1) * sizeof(Entity));
    if (!newRows) return false;

    _rows = newRows;
    Memory::zero(_rows + _count, (ucRow - _count + 1) * sizeof(Entity));
    _count = ucRow + 1;
  }

  // Alloc COL array if needed.
  if (FOG_UNLIKELY(_rows[ucRow] == NULL))
  {
    _rows[ucRow] = (Entity*)Memory::calloc(256 * sizeof(Entity));
    if (!_rows[ucRow]) return false;
  }

  _rows[ucRow][ucCol] = data;
  return true;
}

void GlyphCache::reset()
{
  sysuint_t i, j;
  sysuint_t count = _count;
  Entity** rows = _rows;
  Entity* row;

  for (i = 0; i != count; i++, rows++)
  {
    if ((row = *rows) != NULL)
    {
      for (j = 0; j != 256; j++)
      {
        if (row[j]) row[j]->deref();
      }
      Memory::free(row);
    }
  }
  Memory::free(_rows);

  _rows = 0;
  _count = 0;
}

} // Fog namespace
