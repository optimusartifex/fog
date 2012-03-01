// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Acc/AccC.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Tools/Reduce_p.h>

#include <stdlib.h>

namespace Fog {

// ============================================================================
// [Fog::Reduce - Helpers]
// ============================================================================

static int _Reduce_compareAscent(const void* _a, const void* _b)
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

// ============================================================================
// [Fog::Reduce - Construction / Destruction]
// ============================================================================

Reduce::Reduce()
{
  reset();
}

Reduce::~Reduce()
{
}

// ============================================================================
// [Fog::Reduce - Reset]
// ============================================================================

void Reduce::reset()
{
  _indexes.clear();

  MemOps::zero(_entities, sizeof(_entities));
  _count = 0;
}

// ============================================================================
// [Fog::Reduce - Analyze]
// ============================================================================

bool Reduce::analyze(const Image& image, bool discardAlphaChannel)
{
  reset();

  int w = image.getWidth();
  int h = image.getHeight();

  int x;
  int y;
  int i;

  const uint8_t* p = image.getFirst();
  size_t stride = image.getStride() - w * image.getBytesPerPixel();

  uint32_t depth = image.getDepth();
  Entity* e = _entities;

  switch (depth)
  {
    case 8:
    {
      // It's guaranted that the maximum count of colors is 256, entities are
      // used directly.
      for (i = 0; i < 256; i++) e[i].key = i;

      for (y = 0; y < h; y++, p += stride)
      {
        for (x = 0; x < w; x++, p++) e[p[0]].usage++;
      }
      break;
    }

    case 16:
    case 24:
    case 32:
    {
      Hash<uint32_t, uint64_t> hash;

      uint32_t mask = image.getFormatDescription().getUsedBits32();
      if (discardAlphaChannel) mask ^= image.getFormatDescription().getAMask32();

#define _FOG_REDUCE_LOOP(_BytesPerPixel_, _Load_) \
      FOG_MACRO_BEGIN \
        for (y = 0; y < h; y++, p += stride) \
        { \
          for (x = 0; x < w; x++, p += _BytesPerPixel_) \
          { \
            uint32_t c; \
            _Load_(c, p); \
            Acc::p32And(c, c, mask); \
            \
            /* Increase the count of 'c' if it is already in the hash-table. */ \
            if (hash.contains(c)) \
              (*hash.usePtr(c))++; \
            /* Create new node if sum of the created nodes is smaller than 256. */ \
            else if (hash.getLength() < 256) \
              if (FOG_IS_ERROR(hash.put(c, 1))) return false; \
            /* Finished, the color reduction isn't possible. */ \
            else \
              return false; \
          } \
        } \
      FOG_MACRO_END

      if (depth == 16)
        _FOG_REDUCE_LOOP(2, Acc::p32Load2a);
      else if (depth == 24)
        _FOG_REDUCE_LOOP(3, Acc::p32Load3b);
      else if (depth == 32)
        _FOG_REDUCE_LOOP(4, Acc::p32Load4a);

      // If we are here, the color reduction is possible.
      i = 0;

      HashIterator<uint32_t, uint64_t> it(hash);
      while (it.isValid())
      {
        e[i].key   = it.getKey();
        e[i].usage = it.getItem();

        it.next();
      }

      // The count of items in the hash table means the count of colors used.
      _count = (uint32_t)hash.getLength();
    }

    default:
      return false;
  }

  // Sort, optimize for PCX and all formats where compression depends on the
  // pixel value (the most used colors go first).
  qsort(e, _count, sizeof(Entity), _Reduce_compareAscent);

  // Fix the B&W images, it's usual that black color is at [0].
  if (_count == 2 && e[1].key == 0) MemOps::xchg_t<Entity>(&e[0], &e[1]);

  // Create a fast index table.
  for (i = 0; i < (int)_count; i++)
  {
    if (FOG_IS_ERROR(_indexes.put(e[i].key, i))) goto _Fail;
  }

  // Build palette.
  {
    if (FOG_IS_ERROR(_palette.setLength(_count))) goto _Fail;
    if (FOG_IS_ERROR(_palette.detach())) goto _Fail;

    Argb32* pal = _palette.getDataX();

    //! ${IMAGE_FORMAT:BEGIN}
    switch (image.getFormat())
    {
      case IMAGE_FORMAT_PRGB32:
        if (!discardAlphaChannel)
        {
          for (uint32_t i = 0; i < _count; i++)
            pal[i] = _entities[i].key;
          break;
        }
        // ... Fall through ...

      case IMAGE_FORMAT_RGB24:
        for (uint32_t i = 0; i < _count; i++)
          pal[i] = _entities[i].key | 0xFF000000;
        break;

      case IMAGE_FORMAT_A8:
        for (uint32_t i = 0; i < _count; i++)
          pal[i] = _entities[i].key * 0x01010101;
        break;

      case IMAGE_FORMAT_I8:
      {
        const Argb32* srcPal = image.getPalette().getData();

        for (uint32_t i = 0; i < _count; i++)
          pal[i] = srcPal[_entities[i].key];
        break;
      }

      default:
        goto _Fail;
    }
    //! ${IMAGE_FORMAT:END}
  }

  // Finished.
  return true;

_Fail:
  reset();
  return false;
}

// ============================================================================
// [Fog::Reduce - Translate]
// ============================================================================

uint32_t Reduce::traslate(uint32_t key) const
{
  return _indexes.get(key, 255);
}

} // Fog namespace
