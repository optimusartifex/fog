// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_IMAGEFILTERENGINE_H
#define _FOG_GRAPHICS_IMAGEFILTERENGINE_H

// [Dependencies]
#include <Fog/Core/Memory.h>
#include <Fog/Core/Object.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Value.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/ConvolutionMatrix.h>
#include <Fog/Graphics/Image.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Effects
//! @{

// ============================================================================
// [Typedefs]
// ============================================================================

//! @brief The color filter function prototype.
//!
//! This function is used to filter individual pixels. Each color filter have
//! functions for all major pixel formats. Context pointer is pointer returned
//! by @c ImageFilterEngine::getContext() method.
typedef void (FOG_FASTCALL *ColorFilterFn)(
  const void* context,
  uint8_t* dst,
  const uint8_t* src,
  sysuint_t w);

//! @brief The image filter function prototype.
//!
//! This function is used to filter entire image data. Each image filter have
//! functions for all major pixel formats. Context pointer is pointer returned
//! by @c ImageFilterEngine::getContext() method.
typedef void (FOG_FASTCALL *ImageFilterFn)(
  const void* context,
  uint8_t* dst, sysint_t dstStride,
  const uint8_t* src, sysint_t srcStride,
  sysuint_t w, sysuint_t h, sysint_t offset);

// ============================================================================
// [Fog::ImageFilterEngine]
// ============================================================================

//! @brief Image filter engine.
//!
//! Purpose of this class is to enable sharing of image filtering concepts
//! between color filters and image filters (but there are big differences
//! anyway).
struct FOG_API ImageFilterEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ImageFilterEngine(uint32_t type);
  virtual ~ImageFilterEngine();

  // --------------------------------------------------------------------------
  // [Interface - Basics]
  // --------------------------------------------------------------------------

  //! @brief Get whether current filter combination is NOP (no operation).
  virtual bool isNop() const;

  //! @brief Extend a given rectangle by number of pixels needed to successfully
  //! apply this filter to an image. This is mainly for convolution type filters
  //! like blurs where resulting image can be larger than source.
  //!
  //! @note For color filters this is always NOP (no extends).
  virtual err_t getExtendedRect(IntRect& rect) const;

  // --------------------------------------------------------------------------
  // [Interface - Context]
  // --------------------------------------------------------------------------

  //! @brief Get or create context that will be used as an argument for filter
  //! functions. You must release it using @c releaseContext() method.
  virtual const void* getContext() const;

  //! @brief Release context returned by @c getContext() method.
  virtual void releaseContext(const void* context) const;

  // --------------------------------------------------------------------------
  // [Interface - Filtering]
  // --------------------------------------------------------------------------

  //! @brief Get color filter function for a given @a format.
  //!
  //! @param format Pixel format of data that will be processed.
  //!
  //! @note This is valid only for color filters, otherwise NULL is returned.
  virtual ColorFilterFn getColorFilterFn(uint32_t format) const;

  //! @brief Get image filter function for a given @a format.
  //!
  //! @param format Pixel format of data that will be processed.
  //! @param processing Processing flag, see @c IMAGE_FILTER_CHAR.
  //!
  //! Possible processing values are:
  //! - @c IMAGE_FILTER_ENTIRE_PROCESSING - Entire processing (default).
  //! - @c IMAGE_FILTER_VERT_PROCESSING - Vertical processing.
  //! - @c IMAGE_FILTER_HORZ_PROCESSING - Horizontal processing.
  //!
  //! @note This is valid only for image filters, otherwise NULL is returned.
  virtual ImageFilterFn getImageFilterFn(uint32_t format, uint32_t processing) const;

  // --------------------------------------------------------------------------
  // [Reference Counting]
  // --------------------------------------------------------------------------

  //! @brief Reference the image filter.
  FOG_INLINE ImageFilterEngine* ref() const
  {
    refCount.inc();
    return const_cast<ImageFilterEngine*>(this);
  }

  //! @brief Dereference the image filter. If reference count is decreased to
  //! zero the object is destroyed.
  FOG_INLINE void deref()
  {
    if (refCount.deref()) delete this;
  }

  //! @brief Clone filter with all arguments.
  virtual ImageFilterEngine* clone() const = 0;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;
  //! @brief filter type, see @c IMAGE_FILTER_TYPE.
  uint32_t type;
  //! @brief Filter characteristics, see @c IMAGE_FILTER_CHAR.
  uint32_t characteristics;

private:
  FOG_DISABLE_COPY(ImageFilterEngine)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_IMAGEFILTERENGINE_H
