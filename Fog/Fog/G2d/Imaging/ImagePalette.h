// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEPALETTE_H
#define _FOG_G2D_IMAGING_IMAGEPALETTE_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Memory/Memory.h>
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
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  FOG_INLINE ImagePaletteData* ref() const
  {
    refCount.inc();
    return const_cast<ImagePaletteData*>(this);
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref()) Memory::free(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  mutable Atomic<sysuint_t> refCount;

  uint32_t length;
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
  explicit FOG_INLINE ImagePalette(ImagePaletteData* d) : _d(d) {}
  ~ImagePalette();

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getRefCount().
  FOG_INLINE sysuint_t getRefCount() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getRefCount() == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  //! @copydoc Doxygen::Implicit::_detach().
  err_t _detach();

  //! @brief Returns a const pointer to the palette data in PRGB32 format.
  FOG_INLINE const Argb32* getData() const { return _d->data; }

  //! @brief Returns a mutable pointer to the palette data in PRGB32 format
  //! without calling detach().
  FOG_INLINE Argb32* getDataX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::ImagePalette::getDataX() - Called on non-detached object.");
    return reinterpret_cast<Argb32*>(_d->data);
  }

  //! @brief Set palette data to @a other, making their implicit copy.
  err_t setData(const ImagePalette& other);
  //! @brief Set palette data to @a other, making their deep copy.
  err_t setDeep(const ImagePalette& other);

  //! @brief Set @a count of palette entries from @a index to @a pal.
  err_t setData(const Range& range, const Argb32* entries);

  //! @brief Get the palette length.
  FOG_INLINE sysuint_t getLength() const { return _d->length; }
  //! @brief Set the palette length.
  err_t setLength(sysuint_t length);

  // --------------------------------------------------------------------------
  // [At]
  // --------------------------------------------------------------------------

  FOG_INLINE Argb32 getAt(sysuint_t index) const
  {
    FOG_ASSERT_X(index < 256, "Fog::ImagePalette::at() - Index out of range.");
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

  static bool isGreyscale(const Argb32* data, sysuint_t count);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ImagePaletteData);
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::ImagePalette, Fog::TYPEINFO_MOVABLE)

// ============================================================================
// [Fog::Swap]
// ============================================================================

_FOG_SWAP_D(Fog::ImagePalette)

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEPALETTE_H
