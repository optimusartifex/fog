// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGE_H
#define _FOG_G2D_IMAGING_IMAGE_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/System/Delegate.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>
#include <Fog/G2d/Imaging/ImagePalette.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ImageBits;
struct ImageData;
struct ImageFx;
struct ImageFxFilter;
struct PathF;
struct PathD;

// ============================================================================
// [Fog::ImageData]
// ============================================================================

//! @brief Image data.
struct FOG_API ImageData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ImageData();
  virtual ~ImageData();

  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageData* refAlways() const
  {
    refCount.inc();
    return const_cast<ImageData*>(this);
  }

  virtual ImageData* ref() const;
  virtual void deref();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual ImageData* clone() const;
  virtual void* getHandle();

  virtual void paletteModified(const Range& range);

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE sysuint_t getSizeFor(sysuint_t size)
  { return sizeof(ImageData) - (sizeof(uint8_t) * 8) + size; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;

  //! @brief Whether there is exclusive access to the image.
  //!
  //! If this member is non-zero then one or more paint-engines are using the
  //! image data and image can't be referenced (assigned to an other image).
  //! It will be refused using @c ERR_RT_BUSY error code.
  uint32_t locked;

  //! @brief Image type, see @c IMAGE_BUFFER.
  uint32_t type : 8;
  //! @brief Image flags, see @c IMAGE_FLAGS.
  uint32_t flags : 8;
  //! @brief Image format, see @c IMAGE_FORMAT.
  uint32_t format : 8;
  //! @brief Bytes per pixel.
  uint32_t bytesPerPixel : 8;

  uint32_t colorKey;

#if FOG_ARCH_BITS >= 64
  uint32_t padding;
#endif // FOG_ARCH_BITS

  //! @brief Image size.
  SizeI size;

  //! @brief Data pointer that points to raw memory received from the image
  //! device.
  uint8_t* data;
  //! @brief Base pointer to scanline, in most cases it's equal to @c data.
  //!
  //! @note This pointer can be different to @c data member only if the image
  //! pixels are mirrored (buttom-to-top). In this case @b first points to
  //! last scanline and @c stride is negative.
  uint8_t* first;
  //! @brief Image stride.
  sysint_t stride;

  //! @brief Image palette (only for 8 bit indexed images).
  ImagePalette palette;

  //! @brief Buffer for static data allocation (allocated together with this structure).
  uint8_t buffer[8];

private:
  FOG_DISABLE_COPY(ImageData)
};

// ============================================================================
// [Fog::Image]
// ============================================================================

//! @brief Raster image container.
struct FOG_API Image
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Image();
  Image(const Image& other);
  Image(const SizeI& size, uint32_t imageFormat, uint32_t imageBuffer = IMAGE_BUFFER_MEMORY);
  explicit FOG_INLINE Image(ImageData* d) : _d(d) {}
  ~Image();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getRefCount().
  FOG_INLINE sysuint_t getRefCount() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return (getRefCount() + isReadOnly()) == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }

  //! @copydoc Doxygen::Implicit::_detach().
  err_t _detach();

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::reset().
  void reset();

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getFlags().
  FOG_INLINE uint32_t getFlags() const { return _d->flags; }

  //! @copydoc Doxygen::Implicit::isNull().
  FOG_INLINE bool isNull() const { return _d == _dnull.instancep(); }
  //! @copydoc Doxygen::Implicit::isStatic().
  FOG_INLINE bool isStatic() const { return _d->flags & IMAGE_DATA_STATIC; }
  //! @copydoc Doxygen::Implicit::isPrivate().
  FOG_INLINE bool isPrivate() const { return _d->flags & IMAGE_DATA_PRIVATE; }
  //! @copydoc Doxygen::Implicit::isKeepAlive().
  FOG_INLINE bool isKeepAlive() const { return _d->flags & IMAGE_DATA_KEEP_ALIVE; }

  //! @brief Get whether the image is read-only.
  FOG_INLINE bool isReadOnly() const { return _d->flags & IMAGE_DATA_READ_ONLY; }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const { return _d->size.w != 0; }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Get pointer to the image data (first byte in image buffer) (const).
  //!
  //! Note that image data should be usually equal to image first scanline. But
  //! it's possible that this pointer will be last image scanline in cases that
  //! raster data are filled from bottom to top (Windows DIBs). So if you want
  //! to write portable code, use always @c getFirst() or @c getScanline() methods.
  FOG_INLINE const uint8_t* getData() const
  {
    return _d->data;
  }

  //! @brief Get pointer to the image data (first byte in image buffer) (mutable).
  //!
  //! Note that image data should be usually equal to image first scanline. But
  //! it's possible that this pointer will be last image scanline in cases that
  //! raster data are filled from bottom to top (Windows DIBs). So if you want
  //! to write portable code, use always @c xFirst() or @c xScanline() methods.
  //!
  //! @note Image must be detached to call this function.
  FOG_INLINE uint8_t* getDataX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::Image::getDataX() - Called on non-detached object.");
    return _d->data;
  }

  //! @brief Get a pointer to the first image scanline (const).
  FOG_INLINE const uint8_t* getFirst() const
  {
    return _d->first;
  }

  //! @brief Get a pointer to the first image scanline (mutable).
  //!
  //! @note The @c Image must be detached to call this function.
  FOG_INLINE uint8_t* getFirstX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::Image::getFirstX() - Called on non-detached object.");
    return _d->first;
  }

  //! @brief Get a pointer to a scanline at index @a i (const).
  FOG_INLINE const uint8_t* getScanline(uint32_t i) const
  {
    FOG_ASSERT_X(i < (uint32_t)_d->size.h, "Fog::Image::getScanline() - Index out of range");
    return _d->first + (sysint_t)i * _d->stride;
  }

  //! @brief Get a pointer to a scanline at index @a i (mutable).
  //!
  //! @note The @c Image must be detached to call this function.
  FOG_INLINE uint8_t* getScanlineX(uint32_t i)
  {
    FOG_ASSERT_X(i < (uint32_t)_d->size.h, "Fog::Image::getScanlineX() - Index out of range");
    FOG_ASSERT_X(isDetached(), "Fog::Image::getScanlineX() - Called on non-detached object.");
    return _d->first + (sysint_t)i * _d->stride;
  }

  // --------------------------------------------------------------------------
  // [Handle]
  // --------------------------------------------------------------------------

  //! @brief Get image handle.
  //!
  //! This method can be used to retrieve handle of the original image that
  //! manages the image buffer. This image can be for example Windows DIB
  //! Section (see @c IMAGE_BUFFER enumeration).
  FOG_INLINE void* getHandle() { return _d->getHandle(); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get image size (in pixels).
  FOG_INLINE const SizeI& getSize() const { return _d->size; }
  //! @brief Get image width (in pixels).
  FOG_INLINE int getWidth() const { return _d->size.w; }
  //! @brief Get image height (in pixels).
  FOG_INLINE int getHeight() const { return _d->size.h; }

  //! @brief Get whether image is empty (width and height equals to zero).
  FOG_INLINE bool isEmpty() const { return _d->stride == 0; }

  //! @brief Get image stride (bytes per line).
  //!
  //! @note Stride can be 'width * bytesPerPixel', but can be also larger.
  FOG_INLINE sysint_t getStride() const { return _d->stride; }

  // --------------------------------------------------------------------------
  // [Type / Format]
  // --------------------------------------------------------------------------

  //! @brief Get image type, see @c IMAGE_BUFFER enumeration.
  FOG_INLINE uint32_t getType() const { return _d->type; }

  //! @brief Get image format, see @c IMAGE_FORMAT enumeration.
  FOG_INLINE uint32_t getFormat() const { return _d->format; }
  //! @brief Get image bytes per pixel (1 or 4).
  FOG_INLINE uint32_t getBytesPerPixel() const { return _d->bytesPerPixel; }

  //! @brief Get image depth (8 or 32).
  FOG_INLINE uint32_t getDepth() const { return _d->bytesPerPixel == 0 ? 1 : _d->bytesPerPixel << 3; }

  //! @brief Get image format description.
  FOG_INLINE const ImageFormatDescription& getFormatDescription() const
  { return ImageFormatDescription::getByFormat(_d->format); }

  // --------------------------------------------------------------------------
  // [Create / Adopt]
  // --------------------------------------------------------------------------

  //! @brief Create new image at @a w x @a h size in a given @a format.
  //!
  //! Please always check error value, because allocation memory for image data
  //! can fail. Also if there are invalid arguments (dimensions or format) the
  //! @c ERR_RT_INVALID_ARGUMENT will be returned.
  err_t create(const SizeI& size, uint32_t format, uint32_t bufferType = IMAGE_BUFFER_MEMORY);

  //! @brief Adopt memory buffer to the image.
  err_t adopt(const ImageBits& imageBits, uint32_t adoptFlags = IMAGE_ADOPT_DEFAULT);

  // --------------------------------------------------------------------------
  // [Modified]
  // --------------------------------------------------------------------------

  //! @brief Called after image data was modified.
  void _modified();

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  //! @brief Set other image to this image creating reference to it if possible.
  err_t set(const Image& other);

  //! @brief Copy part of other image to the image.
  err_t set(const Image& other, const RectI& area);

  //! @brief Set other image to this image making deep copy of it.
  err_t setDeep(const Image& other);

  // --------------------------------------------------------------------------
  // [Conversion]
  // --------------------------------------------------------------------------

  //! @brief Convert image to @a format.
  err_t convert(uint32_t format);

  //! @brief Sets image format without doing any conversion. This can be only
  //! used for formats with equal bit depths.
  //!
  //! @note You can use this method for example to bypass premultiplication
  //! or demultiplication on 32 bit images.
  err_t forceFormat(uint32_t format);

  // --------------------------------------------------------------------------
  // [Quantization]
  // --------------------------------------------------------------------------

  //! @brief Convert image to 8 bit using optimized palette or using dithering.
  err_t convertTo8BitDepth();

  //! @brief Convert image to 8 bit using custom palette @a pal.
  err_t convertTo8BitDepth(const ImagePalette& pal);

  // --------------------------------------------------------------------------
  // [ImagePalette]
  // --------------------------------------------------------------------------

  //! @brief Get image palette.
  FOG_INLINE const ImagePalette& getPalette() const { return _d->palette; }
  //! @brief Set image palette.
  err_t setPalette(const ImagePalette& palette);
  //! @brief Set image palette entries.
  err_t setPalette(const Range& range, const Argb32* pal);

  // --------------------------------------------------------------------------
  // [Alpha Distribution]
  // --------------------------------------------------------------------------

  uint32_t getAlphaDistribution() const;

  // --------------------------------------------------------------------------
  // [Invert]
  // --------------------------------------------------------------------------

  static err_t invert(Image& dest, const Image& src, uint32_t channels);
  FOG_INLINE err_t invert(uint32_t channels) { return invert(*this, *this, channels); }

  // --------------------------------------------------------------------------
  // [Mirror]
  // --------------------------------------------------------------------------

  static err_t mirror(Image& dest, const Image& src, uint32_t mirrorMode);
  FOG_INLINE err_t mirror(uint32_t mirrorMode) { return mirror(*this, *this, mirrorMode); }

  // --------------------------------------------------------------------------
  // [Rotate]
  // --------------------------------------------------------------------------

  //! @brief Rotate image by 0, 90, 180 or 270 degrees, see @c IMAGE_ROTATE_MODE.
  static err_t rotate(Image& dest, const Image& src, uint32_t rotateMode);
  //! @brief Rotate image by 0, 90, 180 or 270 degrees, see @c IMAGE_ROTATE_MODE.
  FOG_INLINE err_t rotate(uint32_t rotateMode) { return rotate(*this, *this, rotateMode); }

  // --------------------------------------------------------------------------
  // [Filter]
  // --------------------------------------------------------------------------

#if 0
  static err_t filter(
    const ImageFx& fx,
    Image& dst, PointI* dstOffset,
    const Image& src, const RectI* srcFragment);

  static err_t filter(
    const ImageFxFilter& fxFilter,
    Image& dst, PointI* dstOffset,
    const Image& src, const RectI* srcFragment);

  Image filtered(const ImageFx& fx, const RectI* srcFragment);
  Image filtered(const ImageFxFilter& fxFilter, const RectI* srcFragment);
#endif
  // --------------------------------------------------------------------------
  // [Scale]
  // --------------------------------------------------------------------------

#if 0
  Image scaled(const SizeI& to, uint32_t quality = IMAGE_QUALITY_DEFAULT) const;
#endif

  // --------------------------------------------------------------------------
  // [Fill]
  // --------------------------------------------------------------------------

  err_t clear(const Color& color);

  err_t fillRect(const RectI& r, const Color& color,
    uint32_t compositingOperator = COMPOSITE_SRC_OVER, float opacity = 1.0f);

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  err_t blitImage(const PointI& pt, const Image& im,
    uint32_t compositingOperator = COMPOSITE_SRC_OVER, float opacity = 1.0f);
  err_t blitImage(const PointI& pt, const Image& im, const RectI& srcRect,
    uint32_t compositingOperator = COMPOSITE_SRC_OVER, float opacity = 1.0f);

  // --------------------------------------------------------------------------
  // [Scroll]
  // --------------------------------------------------------------------------

  //! @brief Scroll data in image.
  //!
  //! @note Data that was scrolled out are unchanged.
  err_t scroll(int x, int y);

  //! @brief Scroll data in image only in rectangle @a r.
  //!
  //! @note Data that was scrolled out are unchanged.
  err_t scroll(int x, int y, const RectI& r);

  // --------------------------------------------------------------------------
  // [Misc]
  // --------------------------------------------------------------------------

  //! @brief Check if point at a given coordinates @a x and @a y is in image.
  FOG_INLINE bool hasPoint(int x, int y) const
  {
    return (uint)x < (uint)getWidth() && (uint)y < (uint)getHeight();
  }

  //! @brief Check if point at a given coordinates @a at is in image.
  FOG_INLINE bool hasPoint(const PointI& pt) const
  {
    return (uint)pt.x < (uint)getWidth() && (uint)pt.y < (uint)getHeight();
  }

  static err_t glyphFromPath(Image& glyph, PointI& offset, const PathD& path, uint32_t precision);

  // --------------------------------------------------------------------------
  // [Windows Support]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  //! @brief Convert image to Windows @c HBITMAP (DIBSECTION).
  //!
  //! @note This function is for Windows-only.
  HBITMAP toWinBitmap() const;

  //! @brief Convert image from Windows @c HBITMAP.
  //!
  //! @note This function is for Windows-only.
  err_t fromWinBitmap(HBITMAP hBitmap);

  //! @brief Get HDC mapped to the image.
  //!
  //! After the HDC is not needed anymore use @c releaseDC() method to release
  //! the device context.
  //!
  //! @note This function is Windows-only and it returns HDC only when the
  //! image was created using @c IMAGE_BUFFER_WIN_DIB type. Otherwise @c NULL
  //! is returned.
  //!
  //! @sa @c releaseDC().
  HDC getDC();

  //! @brief Release HDC mapped to the image, retrieved by @c getDC() method.
  //!
  //! @note This function is Windows-only and it releases HDC only when the
  //! image was created using @c IMAGE_BUFFER_WIN_DIB type.
  //!
  //! @sa @c getDC().
  void releaseDC(HDC hDC);
#endif // FOG_OS_WINDOWS

  // --------------------------------------------------------------------------
  // [Read]
  // --------------------------------------------------------------------------

  err_t readFromFile(const String& fileName);
  err_t readFromStream(Stream& stream);
  err_t readFromStream(Stream& stream, const String& extension);
  err_t readFromBuffer(const ByteArray& buffer);
  err_t readFromBuffer(const ByteArray& buffer, const String& extension);
  err_t readFromBuffer(const void* buffer, sysuint_t size);
  err_t readFromBuffer(const void* buffer, sysuint_t size, const String& extension);

  // --------------------------------------------------------------------------
  // [Write]
  // --------------------------------------------------------------------------

  err_t writeToFile(const String& fileName) const;
  err_t writeToStream(Stream& stream, const String& extension) const;
  err_t writeToBuffer(ByteArray& buffer, const String& extension) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Image& operator=(const Image& other) { set(other); return *this; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  //! @brief Shared null image.
  static Static<ImageData> _dnull;

  //! @brief Calculate stride for a given image @a width and @a depth.
  //!
  //! Stride is calculated using @a width and @a depth using 32-bit alignment
  //! that is compatible to Windows DIBs and probably to other OS specific
  //! image formats.
  static sysint_t getStrideFromWidth(int width, uint32_t depth);

  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  static ImageData* _dalloc(sysuint_t size);
  static ImageData* _dalloc(const SizeI& size, uint32_t format);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(ImageData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Image, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGE_H
