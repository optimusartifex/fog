// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_IMAGEPIXELS_H
#define _FOG_GRAPHICS_IMAGEPIXELS_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Palette.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Imaging
//! @{

// ============================================================================
// [Fog::ImagePixels]
// ============================================================================

//! @brief Structure that holds information about raster image buffer.
//!
//! Use it together with @c Image::adopt() or @c Painter::begin() methods.
struct FOG_API ImagePixels
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ImagePixels();
  virtual ~ImagePixels();

  // --------------------------------------------------------------------------
  // [Valid]
  // --------------------------------------------------------------------------

  //! @brief Get whether the image pixels instance contains valid data.
  //!
  //! Valid data are data that can be used to represent image at least 1x1
  //! dimensions using Fog supported pixel format.
  FOG_INLINE bool isValid() const
  {
    return _size.isValid() && _format < IMAGE_FORMAT_COUNT && _data != NULL;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get size of the buffer.
  FOG_INLINE IntSize getSize() const { return _size; }
  //! @brief Get width of image pixels buffer.
  FOG_INLINE int getWidth() const { return _size.getWidth(); }
  //! @brief Get height of image pixels buffer.
  FOG_INLINE int getHeight() const { return _size.getHeight(); }

  //! @brief Get format of image pixels buffer.
  FOG_INLINE uint32_t getFormat() const { return _format; }
  //! @brief Get stride of image pixels buffer.
  FOG_INLINE sysint_t getStride() const { return _stride; }

  //! @brief Get raw data.
  FOG_INLINE uint8_t* getData() const { return _data; }

  // --------------------------------------------------------------------------
  // [Clear / Ready]
  // --------------------------------------------------------------------------

protected:
  virtual void _clear();
  virtual err_t _ready();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Image pixels size (width and height).
  IntSize _size;
  //! @brief Image pixels format.
  uint32_t _format;
  //! @brief Image pixels flags.
  uint32_t _flags;
  //! @brief Image pixels stride (bytes per line).
  sysint_t _stride;
  //! @brief Pointer to the first image scanline.
  uint8_t* _data;

  //! @brief Image that owns the image pixels stored here, can be @c NULL.
  //!
  //! @note The class is never instantiated using create() / destroy(). It's
  //! here only to hold @c Image::_d pointer (@c ImageData) and to be able to
  //! unlock the pixels in destructor (or to perform platform specific stuff).
  Static<Image> _relatedImage;

private:
  FOG_DISABLE_COPY(ImagePixels)

  friend struct Image;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_IMAGEPIXELS_H
