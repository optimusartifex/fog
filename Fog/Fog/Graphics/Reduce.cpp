// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Hash.h>
#include <Fog/Core/Memory.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Graphics/Reduce_p.h>

#include <stdlib.h>

namespace Fog {

// ============================================================================
// [Fog::Reduce]
// ============================================================================

static int cmp_color_ascent(const void* _a, const void* _b)
{
  const Reduce::Entity* a = (const Reduce::Entity *)_a;
  const Reduce::Entity* b = (const Reduce::Entity *)_b;

  if (a->usage < b->usage)
    return -1;
  else if (a->usage > b->usage)
    return 1;
  else
    return 0;
}

/*
static int cmp_color_descent(const void* _a, const void* _b)
{
  const Reduce::Entity* a = (const Reduce::Entity *)_a;
  const Reduce::Entity* b = (const Reduce::Entity *)_b;

  if (a->usage < b->usage)
    return 1;
  else if (a->usage > b->usage)
    return -1;
  else
    return 0;
}
*/

Reduce::Reduce()
{
  clear();
}

Reduce::~Reduce()
{
}

void Reduce::clear()
{
  _indexes.clear();
  Memory::zero(_entities, sizeof(_entities));
  _count = 0;
}

bool Reduce::analyze(const Image& image, bool discardAlpha)
{
  // first clear values
  clear();

  int x;
  int y;
  int w = image.getWidth();
  int h = image.getHeight();
  int i;
  const uint8_t* p;

  Entity* e = _entities;

  if (image.getDepth() == 8)
  {
    // It's guaranted maximum numbers of colors (256). So to calculate count
    // of colors is not hard. Entities are used directly.
    for (i = 0; i < 256; i++) e[i].key = i;

    for (y = 0; y < h; y++)
    {
      for (x = w, p = (const uint8_t*)image.getScanline(y); x; x--, p++)
      {
        e[p[0]].usage++;
      }
    }
  }
  else
  {
    UnorderedHash<uint32_t, uint64_t> hash;
    bool fillalpha = (image.getFormat() == IMAGE_FORMAT_XRGB32) || discardAlpha;

    for (y = 0; y < h; y++)
    {
      for (x = 0, p = (const uint8_t*)image.getScanline(y); x < w; x++, p += 4)
      {
        uint32_t c = *(const uint32_t *)p;
        if (fillalpha) c |= ARGB32_AMASK;

        // Increase count of 'c' if hash already contains it
        if (hash.contains(c))
          (*hash[c])++;
        // Create new node if there are not 256 colors already
        else if (hash.getLength() < 256)
          hash.put(c, 1);
        // Finished, no color reduction possible
        else
          return false;
      }
    }

    // If we are here, color reduction is possible, setup tables ...
    i = 0;

    UnorderedHash<uint32_t, uint64_t>::ConstIterator it(hash);
    for (it.toStart(); it.isValid(); it.toNext(), i++)
    {
      e[i].key   = it.key();
      e[i].usage = it.value();
    }

    // Count items in hash table means count of colors used
    _count = hash.getLength();
  }

  // sort, optimizes for PCX and all formats that are dependent to 
  // color indexes (most used colors go first and unused last)
  qsort(e, _count, sizeof(Entity), cmp_color_ascent);

  // fix black and white images, it's usual that black color
  // is at index 0 
  if (_count == 2)
  {
    if (_entities[1].key == 0xFF000000)
    {
      Memory::xchg(&e[0], &e[1], sizeof(Entity));
    }
  }

  // create fast index table
  for (i = 0; i < (int)_count; i++) _indexes.put(e[i].key, i);

  return true;
}

uint32_t Reduce::traslate(uint32_t key) const
{
  return _indexes.value(key, 255);
}

Palette Reduce::toPalette()
{
  Palette palette;
  uint32_t colors[256];

  FOG_ASSERT(_count < 256);

  for (uint32_t i = 0; i < _count; i++) colors[i] = _entities[i].key;
  palette.setArgb32(0, (Argb*)colors, _count);

  return palette;
}

} // Fog namespace
