// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_G2D_TOOLS_REDUCE_P_H
#define _FOG_G2D_TOOLS_REDUCE_P_H

#include <Fog/Core/Tools/Hash.h>
#include <Fog/G2d/Imaging/Image.h>

namespace Fog {

//! @addtogroup Fog_G2d_Tools
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
struct FOG_NO_EXPORT Reduce
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
  //! @brief Argb32 entity with usage information.
  struct Entity
  {
    uint32_t key;
    uint64_t usage;
  };

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE const Hash<uint32_t, uint8_t>& getIndexes() const { return _indexes; }
  FOG_INLINE const Entity* getEntities() const { return _entities; }
  FOG_INLINE const ImagePalette& getPalette() const { return _palette; }

  FOG_INLINE uint32_t getCount() const { return _count; }
  FOG_INLINE uint32_t getMask() const { return _mask; }

  void reset();
  bool analyze(const Image& image, bool discardAlphaChannel = false);

  uint32_t traslate(uint32_t key) const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief Color to index hash.
  Hash<uint32_t, uint8_t> _indexes;

  //! @brief Entities.
  Entity _entities[256];
  //! @brief Palette.
  ImagePalette _palette;

  //! @brief Count of colors / entities in image or zero.
  uint32_t _count;
  //! @brief Pixel mask. Must be used (BITAND) before the pixel is accessed.
  uint32_t _mask;

private:
  FOG_NO_COPY(Reduce)
};

//! @}

} // Fog namespace

#endif // _FOG_G2D_TOOLS_REDUCE_P_H
