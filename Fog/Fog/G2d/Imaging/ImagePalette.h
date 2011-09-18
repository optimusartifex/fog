// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEPALETTE_H
#define _FOG_G2D_IMAGING_IMAGEPALETTE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ImagePaletteData]
// ============================================================================

struct FOG_NO_EXPORT ImagePaletteData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE ImagePaletteData* addRef() const
  {
    reference.inc();
    return const_cast<ImagePaletteData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      MemMgr::free(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

  //! @brief Count of palette entries used.
  uint32_t length;

  //! @brief Palette data.
  Argb32 data[256];
};

// ============================================================================
// [Fog::ImagePalette]
// ============================================================================

//! @brief The ImagePalette is array of 256 ARGB32 color elements used to represent
//! colors in 8-bit indexed image (the pixel value is index in the palette).
//!
//! @note Using alpha-channel is non-portable, because there is no API on
//! target operating system which is possible to handle it. However, Fog API
//! can handle transparency like @c IMAGE_FORMAT_ARGB32 or @c IMAGE_FORMAT_PRGB32
//! formats.
struct FOG_API ImagePalette
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ImagePalette();
  ImagePalette(const ImagePalette& other);

  explicit FOG_INLINE ImagePalette(ImagePaletteData* d) : _d(d)
  {
  }

  ~ImagePalette();

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  //! @copydoc Doxygen::Implicit::_detach().
  err_t _detach();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Returns a const pointer to the palette data in PRGB32 format.
  FOG_INLINE const Argb32* getData() const { return _d->data; }

  //! @brief Returns a mutable pointer to the palette data in PRGB32 format
  //! without calling detach().
  FOG_INLINE Argb32* getDataX()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::ImagePalette::getDataX() - Not detached.");

    return reinterpret_cast<Argb32*>(_d->data);
  }

  //! @brief Set palette data to @a other, making their implicit copy.
  err_t setData(const ImagePalette& other);
  //! @brief Set palette data to @a other, making their deep copy.
  err_t setDeep(const ImagePalette& other);

  //! @brief Replace palette entries at @a range by @a entities.
  err_t setData(const Range& range, const Argb32* entries);

  //! @brief Get the palette length.
  FOG_INLINE size_t getLength() const { return _d->length; }
  //! @brief Set the palette length.
  err_t setLength(size_t length);

  // --------------------------------------------------------------------------
  // [At]
  // --------------------------------------------------------------------------

  FOG_INLINE Argb32 getAt(size_t index) const
  {
    FOG_ASSERT_X(index < 256,
      "Fog::ImagePalette::at() - Index out of range.");

    return _d->data[index];
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  void clear();
  void reset();

  // --------------------------------------------------------------------------
  // [Find]
  // --------------------------------------------------------------------------

  uint8_t findColor(uint8_t r, uint8_t g, uint8_t b) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const ImagePalette& operator=(const ImagePalette& other)
  {
    setData(other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Static<ImagePaletteData> _dnull;

  static ImagePalette fromGreyscale(uint32_t count);
  static ImagePalette fromCube(uint32_t r, uint32_t g, uint32_t b);

  static bool isGreyscale(const Argb32* data, size_t count);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ImagePaletteData);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEPALETTE_H
