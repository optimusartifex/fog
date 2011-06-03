// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGECONVERTER_H
#define _FOG_G2D_IMAGING_IMAGECONVERTER_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>
#include <Fog/G2d/Imaging/ImagePalette.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ImageConverterClosure;

// ============================================================================
// [Function Prototypes]
// ============================================================================

//! @brief Converter blitter function.
//!
//! @note This structure is the same as the internal @c RenderVBlitLineFn used
//! by the private Fog::Render module.
typedef void (FOG_FASTCALL *ImageConverterBlitLineFn)(
  uint8_t* dst,
  const uint8_t* src,
  int w,
  const ImageConverterClosure* closure);

// ============================================================================
// [Fog::ImageConverterClosure]
// ============================================================================

//! @brief The image converter closure used by the blitter.
//!
//! @note This structure is the same as the internal @c RenderClosure used by
//! the private Fog::Render module.
struct FOG_NO_EXPORT ImageConverterClosure
{
  //! @brief The dither origin, if dithering is in use, otherwise [0, 0].
  PointI ditherOrigin;
  //! @brief The source palette (if indexed).
  const ImagePaletteData* palette;

  //! @brief Extra data, may be used by the image converter or compositor.
  void* data;
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
  //! @brief Reference count.
  mutable Atomic<size_t> refCount;
  //! @brief Converter function.
  ImageConverterBlitLineFn blitFn;

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
struct FOG_API ImageConverter
{
  // --------------------------------------------------------------------------
  // [Construction & Destruction]
  // --------------------------------------------------------------------------

  ImageConverter();
  ImageConverter(const ImageConverter& other);

  ImageConverter(
    const ImageFormatDescription& dstFormatDescription,
    const ImageFormatDescription& srcFormatDescription,
    uint32_t dither = false,
    const ImagePalette* dstPalette = NULL,
    const ImagePalette* srcPalette = NULL);

  ~ImageConverter();

  // --------------------------------------------------------------------------
  // [Validity]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const { return _d != _dnull.instancep(); }

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  err_t create(
    const ImageFormatDescription& dstFormatDescription,
    const ImageFormatDescription& srcFormatDescription,
    uint32_t dither = false,
    const ImagePalette* dstPalette = NULL,
    const ImagePalette* srcPalette = NULL);

  //! @brief Create a converter which is able to target 8-bit indexed image
  //! as a destination.
  err_t createDithered8(
    const ImageDither8Params& dstParams,
    const ImageFormatDescription& srcFormatDescription,
    const ImagePalette* dstPalette = NULL,
    const ImagePalette* srcPalette = NULL);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Blit - Setup]
  // --------------------------------------------------------------------------

  //! @brief Configure the image-converter closure.
  FOG_INLINE void setupClosure(ImageConverterClosure* closure) const
  {
    closure->ditherOrigin.reset();
    closure->palette = _d->srcPalette.instance()._d;
    closure->data = _d;
  }

  //! @brief Configure the image-converter (with enabled dithering).
  FOG_INLINE void setupClosure(ImageConverterClosure* closure, const PointI& ditherOrigin) const
  {
    closure->ditherOrigin = ditherOrigin;
    closure->palette = _d->srcPalette.instance()._d;
    closure->data = _d;
  }

  // --------------------------------------------------------------------------
  // [Blit - Span]
  // --------------------------------------------------------------------------

  void blitSpan(void* dst, const void* src, int w);
  void blitSpan(void* dst, const void* src, int w, const PointI& ditherOrigin);

  // --------------------------------------------------------------------------
  // [Blit - Rect]
  // --------------------------------------------------------------------------

  void blitRect(void* dst, size_t dstStride, const void* src, size_t srcStride, int w, int h);
  void blitRect(void* dst, size_t dstStride, const void* src, size_t srcStride, int w, int h, const PointI& ditherOrigin);

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get the blitter function.
  FOG_INLINE ImageConverterBlitLineFn getBlitFn() const
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
  // [Static]
  // --------------------------------------------------------------------------

  static Static<ImageConverterData> _dnull;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ImageConverterData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::ImageConverter, Fog::TYPEINFO_MOVABLE)
_FOG_TYPEINFO_DECLARE(Fog::ImageConverterClosure, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::ImageDither8Params, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Swap]
// ============================================================================

_FOG_SWAP_D(Fog::ImageConverter)

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGECONVERTER_H
