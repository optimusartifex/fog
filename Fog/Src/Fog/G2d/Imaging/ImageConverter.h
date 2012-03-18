// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGECONVERTER_H
#define _FOG_G2D_IMAGING_IMAGECONVERTER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>
#include <Fog/G2d/Imaging/ImagePalette.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Function Prototypes]
// ============================================================================

//! @brief Converter blitter function.
//!
//! @note This structure is the same as the internal @c RasterVBlitLineFunc used
//! by the private Fog::Raster module.
typedef void (FOG_FASTCALL *ImageConverterBlitLineFunc)(
  uint8_t* dst,
  const uint8_t* src,
  int w,
  const ImageConverterClosure* closure);

// ============================================================================
// [Fog::ImageConverterClosure]
// ============================================================================

//! @brief The image converter closure used by the blitter.
//!
//! @note This structure is the same as the internal @c RasterClosure used by
//! the private Fog::Raster module.
struct FOG_NO_EXPORT ImageConverterClosure
{
  //! @brief The dither origin, if dithering is in use, otherwise [0, 0].
  PointI ditherOrigin;
  //! @brief Extra data, may be used by the image converter or compositor.
  void* data;

  //! @brief The source palette (if indexed).
  const ImagePaletteData* palette;
  //! @brief Color key (if indexed)
  //!
  //! @note If color-key is greater than 255 then it means that it's won't be
  //! used.
  uint32_t colorKey;
};

// ============================================================================
// [Fog::ImageDither8Params]
// ============================================================================

//! @brief Image 8-bit dither parameters.
//!
//! @note Total count of R.G.B components can't exceed 256 when multiplied.
struct FOG_NO_EXPORT ImageDither8Params
{
  //! @brief Count of red components (minimum value is two).
  uint32_t rCount;
  //! @brief Count of green components (minimum value is two).
  uint32_t gCount;
  //! @brief Count of blue components (minimum value is two).
  uint32_t bCount;

  //! @brief Length of @c transposeTable (maximum value is 256).
  uint32_t transposeTableLength;
  //! @brief Table used to transpose pixel into the target value (can be NULL).
  const uint8_t* transposeTable;
};

// ============================================================================
// [Fog::ImageConverterData]
// ============================================================================

//! @brief Image converter data.
struct FOG_NO_EXPORT ImageConverterData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageConverterData* addRef() const
  {
    reference.inc();
    return const_cast<ImageConverterData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.imageconverter_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;
  //! @brief Converter function.
  ImageConverterBlitLineFunc blitFn;

  //! @brief The destination image format description.
  ImageFormatDescription dstFormatDescription;
  //! @brief The source image format description.
  ImageFormatDescription srcFormatDescription;

  //! @brief The destination palette data (if indexed).
  Static<ImagePalette> dstPalette;
  //! @brief The source palette data (if indexed).
  Static<ImagePalette> srcPalette;

  //! @brief Whether the dithering is used.
  uint32_t isDithered : 8;
  //! @brief Whether the plain-copy is used to convert image data (The same
  //! destination and source format).
  uint32_t isCopy : 8;
  //! @brief Reserved for future use.
  uint32_t isBSwap : 8;

  //! @brief The additional read-only data which may be used by an external
  //! converter or JIT-Compiled code.
  uint8_t buffer[256];
};

// ============================================================================
// [Fog::ImageConverter]
// ============================================================================

//! @brief Image converter.
struct FOG_NO_EXPORT ImageConverter
{
  // --------------------------------------------------------------------------
  // [Construction & Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageConverter()
  {
    fog_api.imageconverter_ctor(this);
  }

  FOG_INLINE ImageConverter(const ImageConverter& other)
  {
    fog_api.imageconverter_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE ImageConverter(ImageConverter&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE ImageConverter(
    const ImageFormatDescription& dstFormatDescription,
    const ImageFormatDescription& srcFormatDescription,
    uint32_t dither = false,
    const ImagePalette* dstPalette = NULL,
    const ImagePalette* srcPalette = NULL)
  {
    fog_api.imageconverter_ctorCreate(this,
      &dstFormatDescription,
      &srcFormatDescription,
      dither,
      dstPalette,
      srcPalette);
  }

  explicit FOG_INLINE ImageConverter(ImageConverterData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~ImageConverter()
  {
    fog_api.imageconverter_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Validity]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    return _d != fog_api.imageconverter_oNull->_d;
  }

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t create(
    const ImageFormatDescription& dstFormatDescription,
    const ImageFormatDescription& srcFormatDescription,
    uint32_t dither = false,
    const ImagePalette* dstPalette = NULL,
    const ImagePalette* srcPalette = NULL)
  {
    return fog_api.imageconverter_create(this,
      &dstFormatDescription,
      &srcFormatDescription,
      dither,
      dstPalette,
      srcPalette);
  }

  //! @brief Create a converter which is able to target 8-bit indexed image
  //! as a destination.
  FOG_INLINE err_t createDithered8(
    const ImageDither8Params& dstParams,
    const ImageFormatDescription& srcFormatDescription,
    const ImagePalette* dstPalette = NULL,
    const ImagePalette* srcPalette = NULL)
  {
    return fog_api.imageconverter_createDithered8(this,
      &dstParams,
      &srcFormatDescription,
      dstPalette,
      srcPalette);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.imageconverter_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Blit - Setup]
  // --------------------------------------------------------------------------

  //! @brief Configure the image-converter closure.
  FOG_INLINE void setupClosure(ImageConverterClosure* closure) const
  {
    closure->ditherOrigin.reset();
    closure->data = _d;
    closure->palette = _d->srcPalette->_d;
    closure->colorKey = 0xFFFFFFFF;
  }

  //! @brief Configure the image-converter (with enabled dithering).
  FOG_INLINE void setupClosure(ImageConverterClosure* closure, const PointI& ditherOrigin) const
  {
    closure->ditherOrigin = ditherOrigin;
    closure->data = _d;
    closure->palette = _d->srcPalette->_d;
    closure->colorKey = 0xFFFFFFFF;
  }

  // --------------------------------------------------------------------------
  // [Blit - Span]
  // --------------------------------------------------------------------------

  FOG_INLINE void blitLine(void* dst, const void* src, int w) const
  {
    return fog_api.imageconverter_blitLine(this, dst, src, w, NULL);
  }

  FOG_INLINE void blitLine(void* dst, const void* src, int w, const PointI& ditherOrigin) const
  {
    return fog_api.imageconverter_blitLine(this, dst, src, w, &ditherOrigin);
  }

  // --------------------------------------------------------------------------
  // [Blit - Rect]
  // --------------------------------------------------------------------------

  FOG_INLINE void blitRect(void* dst, size_t dstStride, const void* src, size_t srcStride, int w, int h) const
  {
    return fog_api.imageconverter_blitRect(this, dst, dstStride, src, srcStride, w, h, NULL);
  }

  FOG_INLINE void blitRect(void* dst, size_t dstStride, const void* src, size_t srcStride, int w, int h, const PointI& ditherOrigin) const
  {
    return fog_api.imageconverter_blitRect(this, dst, dstStride, src, srcStride, w, h, &ditherOrigin);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get the blitter function.
  FOG_INLINE ImageConverterBlitLineFunc getBlitFn() const
  {
    return _d->blitFn;
  }

  //! @brief Get the destination image format description.
  FOG_INLINE const ImageFormatDescription& getDestinationFormatDescription() const
  {
    return _d->dstFormatDescription;
  }

  //! @brief Get the source image format description.
  FOG_INLINE const ImageFormatDescription& getSourceFormatDescription() const
  {
    return _d->srcFormatDescription;
  }

  //! @brief Get whether the dithering is enabled.
  FOG_INLINE bool isDithered() const { return (bool)_d->isDithered; }
  //! @brief Get whether the conversion is plain-copy.
  FOG_INLINE bool isCopy() const { return (bool)_d->isCopy; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ImageConverterData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGECONVERTER_H
