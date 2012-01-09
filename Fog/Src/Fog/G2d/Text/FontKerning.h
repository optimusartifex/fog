// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_FONTKERNING_H
#define _FOG_G2D_TEXT_FONTKERNING_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/Algorithm.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::FontKerningChar]
// ============================================================================

//! @brief Union which container two characters used by @c FontKerningPairI
//! and @c FontKerningPairF.
//!
//! Use @c comparable member to get value which can be used by bsearch or qsort.
union FOG_NO_EXPORT FontKerningChars
{
  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static int FOG_CDECL compareFunc(const void* a, const void* b)
  {
    uint32_t aComparable = reinterpret_cast<const FontKerningChars*>(a)->comparable;
    uint32_t bComparable = reinterpret_cast<const FontKerningChars*>(b)->comparable;

    if (aComparable < bComparable)
      return -1;
    else if (aComparable > bComparable)
      return 1;
    else
      return 0;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // [00 00] - First
  // [11 11] - Second
  //
  // [11 11 00 00] - Little-endian.
  // [00 00 11 11] - Big-endian.

  struct
  {
#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    //! @brief Second character.
    uint16_t second;
    //! @brief First character.
    uint16_t first;
#else
    //! @brief First character.
    uint16_t first;
    //! @brief Second character.
    uint16_t second;
#endif // FOG_BYTE_ORDER
  };

  //! @brief Packed first and second characters.
  uint32_t comparable;
};

// ============================================================================
// [Fog::FontKerningPairI]
// ============================================================================

//! @brief Kerning pair (integer).
#include <Fog/Core/C++/PackDWord.h>
struct FOG_NO_EXPORT FontKerningPairI
{
  //! @brief Kerning characters.
  FontKerningChars chars;
  //! @brief Kerning value (usually negative).
  int amount;
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::FontKerningPairF]
// ============================================================================

//! @brief Kerning pair (float).
#include <Fog/Core/C++/PackDWord.h>
struct FOG_NO_EXPORT FontKerningPairF
{
  //! @brief Kerning characters.
  FontKerningChars chars;
  //! @brief Kerning value (usually negative).
  float amount;
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::FontKerningTableI]
// ============================================================================

struct FOG_NO_EXPORT FontKerningTableI
{
  // --------------------------------------------------------------------------
  // [Find]
  // --------------------------------------------------------------------------

  FOG_INLINE const FontKerningPairI* find(uint32_t first, uint32_t second) const
  {
    FontKerningChars chars;
    chars.first = (uint16_t)first;
    chars.second = (uint16_t)second;

    // BSearch.
    const FontKerningPairI* base = pairs;
    for (uint32_t lim = length; lim != 0; lim >>= 1)
    {
      const FontKerningPairI* cur = base + (lim >> 1);

      if (chars.comparable > cur->chars.comparable)
      {
        base = cur + 1;
        lim--;
        continue;
      }

      if (chars.comparable < cur->chars.comparable)
        continue;

      return cur;
    }

    return NULL;
  }

  // --------------------------------------------------------------------------
  // [Sort]
  // --------------------------------------------------------------------------

  FOG_INLINE void sort()
  {
    Algorithm::qsort(pairs, length, sizeof(FontKerningPairI), FontKerningChars::compareFunc);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE size_t getSizeOf(size_t length)
  {
    return sizeof(FontKerningTableI) - sizeof(FontKerningPairI) +
      length * sizeof(FontKerningPairI);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Length of kerning-pair array.
  uint32_t length;

  //! @brief Kerning-pair array.
  FontKerningPairI pairs[1];
};

// ============================================================================
// [Fog::FontKerningTableF]
// ============================================================================

struct FOG_NO_EXPORT FontKerningTableF
{
  // --------------------------------------------------------------------------
  // [Find]
  // --------------------------------------------------------------------------

  FOG_INLINE const FontKerningPairF* find(uint32_t first, uint32_t second) const
  {
    FontKerningChars chars;
    chars.first = (uint16_t)first;
    chars.second = (uint16_t)second;

    // BSearch.
    const FontKerningPairF* base = pairs;
    for (uint32_t lim = length; lim != 0; lim >>= 1)
    {
      const FontKerningPairF* cur = base + (lim >> 1);

      if (chars.comparable > cur->chars.comparable)
      {
        base = cur + 1;
        lim--;
        continue;
      }

      if (chars.comparable < cur->chars.comparable)
        continue;

      return cur;
    }

    return NULL;
  }

  // --------------------------------------------------------------------------
  // [Sort]
  // --------------------------------------------------------------------------

  FOG_INLINE void sort()
  {
    Algorithm::qsort(pairs, length, sizeof(FontKerningPairF), FontKerningChars::compareFunc);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE size_t getSizeOf(size_t length)
  {
    return sizeof(FontKerningTableF) - sizeof(FontKerningPairF) +
      length * sizeof(FontKerningPairF);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Length of kerning-pair table.
  uint32_t length;

  //! @brief Kerning-pair array.
  FontKerningPairF pairs[1];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_FONTKERNING_H
