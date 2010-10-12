// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_GRAPHICS_REDUCE_P_H
#define _FOG_GRAPHICS_REDUCE_P_H

#include <Fog/Core/Hash.h>
#include <Fog/Graphics/Image.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::Reduce]
// ============================================================================

//! @internal
//!
//! @brief Class used to get information about colors in an image.
//!
//! It's designed for tasks where it's needed or optional to reduce color
//! depth, so this class can calculate count of colors and generate palette 
//! for you.
//!
//! If count of colors is larger than 256, operation is stopped and count
//! of colors is set to zero. Zero means "not able to reduce depth without
//! quantization".
struct FOG_HIDDEN Reduce
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Reduce();
  ~Reduce();

  // --------------------------------------------------------------------------
  // [Entity]
  // --------------------------------------------------------------------------

  //! @internal
  //!
  //! @brief ArgbI entity with usage information.
  struct Entity
  {
    uint32_t key;
    uint64_t usage;
  };

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE const UnorderedHash<uint32_t, uint8_t>& getIndexes() const { return _indexes; }
  FOG_INLINE const Entity* getEntities() const { return _entities; }
  FOG_INLINE uint32_t getCount() const { return _count; }

  void clear();
  bool analyze(const Image& image, bool discardAlpha = false);

  uint32_t traslate(uint32_t key) const;

  Palette toPalette();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  //! @brief Color to index hash.
  UnorderedHash<uint32_t, uint8_t> _indexes;

  //! @brief Entities.
  Entity _entities[256];

  //! @brief Count of colors / entities in image or zero.
  uint32_t _count;

  FOG_DISABLE_COPY(Reduce)
};

//! @}

} // Fog namespace

#endif // _FOG_GRAPHICS_REDUCE_P_H
