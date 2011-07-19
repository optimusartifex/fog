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
    data(NULL),
    size(0, 0),
    format(IMAGE_FORMAT_NULL),
    stride(0)
  {
  }

  FOG_INLINE ImageBits(const ImageBits& other) :
    data(other.data),
    size(other.size),
    format(other.format),
    stride(other.stride)
  {
  }

  FOG_INLINE ImageBits(uint8_t* data, const SizeI& size, uint32_t format, sysint_t stride) :
    data(data),
    size(size),
    format(format),
    stride(stride)
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
    return data != NULL &&
           size.isValid() &&
           format != IMAGE_FORMAT_NULL &&
           format < IMAGE_FORMAT_COUNT;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE void setRaw(const ImageBits* raw)
  {
    this->data = raw->data;
    this->size = raw->size;
    this->format = raw->format;
    this->stride = raw->stride;
  }

  FOG_INLINE void setRaw(uint8_t* data, const SizeI& size, uint32_t format, sysint_t stride)
  {
    this->data = data;
    this->size = size;
    this->format = format;
    this->stride = stride;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    data = NULL;
    size.reset();
    format = IMAGE_FORMAT_NULL;
    stride = 0;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Pointer to the first image scanline.
  uint8_t* data;
  //! @brief Image buffer size.
  SizeI size;
  //! @brief Image buffer format.
  uint32_t format;
  //! @brief Image buffer stride (bytes per line).
  sysint_t stride;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::ImageBits, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEBITS_H
