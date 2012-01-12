// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEBITS_H
#define _FOG_G2D_IMAGING_IMAGEBITS_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Geometry/Size.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ImageBits]
// ============================================================================

//! @brief Raw-image buffer structure.
//!
//! Use it together with @c Image::adopt() or @c Painter::begin() methods.
struct FOG_NO_EXPORT ImageBits
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageBits() :
    _size(0, 0),
    _format(IMAGE_FORMAT_NULL),
#if FOG_ARCH_BITS >= 64
    _padding_0_32(0),
#endif // FOG_ARCH_BITS
    _stride(0),
    _data(NULL)
  {
  }

  FOG_INLINE ImageBits(const ImageBits& other) :
    _size(other._size),
    _format(other._format),
#if FOG_ARCH_BITS >= 64
    _padding_0_32(0),
#endif // FOG_ARCH_BITS
    _stride(other._stride),
    _data(other._data)
  {
  }

  FOG_INLINE ImageBits(const SizeI& size, uint32_t format, ssize_t stride, uint8_t* data) :
    _size(size),
    _format(format),
#if FOG_ARCH_BITS >= 64
    _padding_0_32(0),
#endif // FOG_ARCH_BITS
    _stride(stride),
    _data(data)
  {
  }

  explicit FOG_INLINE ImageBits(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  //! @brief Get whether the @c ImageBits structure data are valid.
  //!
  //! Valid data are data that can be used to represent image at least 1x1
  //! dimensions using Fog supported pixel format.
  FOG_INLINE bool isValid() const
  {
    return _size.isValid() &&
           _data != NULL &&
           _format != IMAGE_FORMAT_NULL &&
           _format < IMAGE_FORMAT_COUNT;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const SizeI& getSize() const { return _size; }
  FOG_INLINE uint32_t getFormat() const { return _format; }
  FOG_INLINE ssize_t getStride() const { return _stride; }
  FOG_INLINE uint8_t* getData() const { return _data; }

  FOG_INLINE void setData(const ImageBits& other)
  {
    _size = other._size;
    _format = other._format;
    _stride = other._stride;
    _data = other._data;
  }

  FOG_INLINE void setData(const SizeI& size, uint32_t format, ssize_t stride, uint8_t* data)
  {
    _size = size;
    _format = format;
    _stride = stride;
    _data = data;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _size.reset();
    _format = IMAGE_FORMAT_NULL;
    _stride = 0;
    _data = NULL;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Image buffer size.
  SizeI _size;
  //! @brief Image buffer format.
  uint32_t _format;

#if FOG_ARCH_BITS >= 64
  uint32_t _padding_0_32;
#endif // FOG_ARCH_BITS

  //! @brief Image buffer stride (bytes per line).
  ssize_t _stride;
  //! @brief Pointer to the first image scanline.
  uint8_t* _data;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEBITS_H
