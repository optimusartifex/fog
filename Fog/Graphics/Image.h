// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_IMAGE_H
#define _FOG_GRAPHICS_IMAGE_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Delegate.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/Stream.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Palette.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Imaging
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ColorFilter;
struct ColorLut;
struct ColorMatrix;
struct DoublePath;
struct ImageData;
struct ImageFilter;
struct ImagePixels;

// ============================================================================
// [Fog::ImageBuffer]
// ============================================================================

//! @brief Structure that holds information about raster image buffer.
//!
//! Use it together with @c Image::adopt() or @c Painter::begin() methods.
struct ImageBuffer
{
  //! @brief Get whether the image buffer structure contains valid data.
  //!
  //! Valid data are data that can be used to represent image at least 1x1
  //! dimensions using Fog supported pixel format.
  FOG_INLINE bool isValid() const
  {
    return width > 0 && height > 0 && format < IMAGE_FORMAT_COUNT && data != NULL;
  }

  // Defined Later.

  //! @brief Import @c ImageData instance into @c ImageBuffer.
  FOG_INLINE void import(ImageData* d);
  //! @brief Import @c ImageData instance into @c ImageBuffer, using only
  //! specified rectangle @a rect.
  FOG_INLINE void import(ImageData* d, const IntRect& rect);

  //! @brief Image buffer width.
  int width;
  //! @brief Image buffer height.
  int height;
  //! @brief Image buffer format.
  uint32_t format;
  //! @brief Image buffer stride (bytes per line).
  sysint_t stride;
  //! @brief Pointer to the first image scanline.
  uint8_t* data;
};

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
  // [Members]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageData* refAlways() const
  { refCount.inc(); return const_cast<ImageData*>(this); }

  virtual ImageData* ref() const;
  virtual void deref();

  virtual ImageData* clone() const;

  virtual err_t create(int w, int h, uint32_t format);
  virtual err_t destroy();

  virtual void* getHandle();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;

  //! @brief Image width.
  int width;
  //! @brief Image height.
  int height;

  //! @brief Paint and image filtering engine can increase / decrease this 
  //! counter. If image is being destroyed and @c lockCounter is larger
  //! than zero then application is terminated (assert + error reporting).
  uint32_t locked;

  //! @brief Image type, see @c IMAGE_TYPE.
  uint32_t type : 8;
  //! @brief Image flags, see @c IMAGE_FLAGS.
  uint32_t flags : 8;
  //! @brief Image format, see @c IMAGE_FORMAT.
  uint32_t format : 8;
  //! @brief Bytes per pixel.
  uint32_t bytesPerPixel : 8;

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
  Palette palette;

  //! @brief Buffer for static data allocation (allocated together with this
  //! structure).
  uint8_t buffer[8];

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE sysuint_t getSizeFor(sysuint_t size)
  { return sizeof(ImageData) - (sizeof(uint8_t) * 8) + size; }

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
  Image(int w, int h, uint32_t format, uint32_t type = IMAGE_TYPE_MEMORY);
  FOG_INLINE explicit Image(ImageData* d) : _d(d) {}
  ~Image();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::refCount().
  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return refCount() == 1 && !isReadOnly(); }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return !isDetached() ? _detach() : (err_t)ERR_OK; }
  //! @copydoc Doxygen::Implicit::_detach().
  err_t _detach();
  //! @copydoc Doxygen::Implicit::free().
  void free();

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getFlags().
  FOG_INLINE uint32_t getFlags() const { return _d->flags; }

  //! @copydoc Doxygen::Implicit::isNull().
  FOG_INLINE bool isNull() const { return _d == _dnull.instancep(); }
  //! @copydoc Doxygen::Implicit::isDynamic().
  FOG_INLINE bool isDynamic() const { return _d->flags & IMAGE_DATA_FLAG_DYNAMIC; }
  //! @copydoc Doxygen::Implicit::isSharable().
  FOG_INLINE bool isSharable() const { return _d->flags & IMAGE_DATA_FLAG_SHARABLE; }
  //! @copydoc Doxygen::Implicit::isKeepAlive().
  FOG_INLINE bool isKeepAlive() const { return _d->flags & IMAGE_DATA_FLAG_KEEP_ALIVE; }

  //! @brief Returns true if image is read only.
  FOG_INLINE bool isReadOnly() const { return _d->flags & IMAGE_DATA_FLAG_READ_ONLY; }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Get image handle.
  //!
  //! This method can be used to retrieve handle of the original image that 
  //! manages the image buffer. This image can be for example Windows DIB
  //! Section (see @c IMAGE_TYPE enumeration).
  FOG_INLINE void* getHandle() { return _d->getHandle(); }

  //! @brief Get constant pointer to image data (first byte in image buffer).
  //!
  //! Note that image data should be usually equal to image first scanline. But
  //! it's possible that this pointer will be last image scanline in cases that
  //! raster data are filled from bottom to top (Windows DIBs). So if you want
  //! to write portable code, use always @c getFirst() or @c getScanline() methods.
  FOG_INLINE const uint8_t* getData() const
  {
    return _d->data;
  }

  //! @brief Get mutable pointer to image data (first byte in image buffer).
  //!
  //! Note that image data should be usually equal to image first scanline. But
  //! it's possible that this pointer will be last image scanline in cases that
  //! raster data are filled from bottom to top (Windows DIBs). So if you want
  //! to write portable code, use always @c mFirst() or @c mScanline() methods.
  FOG_INLINE uint8_t* getMData()
  {
    return (detach() == ERR_OK) ? _d->data : NULL;
  }

  //! @brief Get mutable pointer to image data (first byte in image buffer).
  //!
  //! Note that image data should be usually equal to image first scanline. But
  //! it's possible that this pointer will be last image scanline in cases that
  //! raster data are filled from bottom to top (Windows DIBs). So if you want
  //! to write portable code, use always @c xFirst() or @c xScanline() methods.
  //!
  //! @note Image must be detached to call this function.
  FOG_INLINE uint8_t* getXData()
  {
    FOG_ASSERT_X(isDetached(), "Fog::Image::getXData() - Not detached data.");
    return _d->data;
  }

  //! @brief Get constant pointer to image first scanline.
  FOG_INLINE const uint8_t* getFirst() const
  {
    return _d->first;
  }

  //! @brief Get mutable pointer to image first scanline.
  FOG_INLINE uint8_t* getMFirst()
  {
    return (detach() == ERR_OK) ? _d->first : NULL;
  }

  //! @brief Get constant pointer to image first scanline.
  //!
  //! @note Image must be detached to call this function.
  FOG_INLINE uint8_t* getXFirst()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::Image::getXFirst() - Not detached data.");
    return _d->first;
  }

  //! @brief Get constant pointer to @c i scanline.
  FOG_INLINE const uint8_t* getScanline(uint32_t i) const
  {
    FOG_ASSERT_X((sysuint_t)i < (sysuint_t)_d->height,
      "Fog::Image::getScanline() - Index out of range");
    return _d->first + (sysint_t)i * _d->stride;
  }

  //! @brief Get mutable pointer to @c i scanline.
  FOG_INLINE uint8_t* getMScanline(uint32_t i)
  {
    FOG_ASSERT_X((sysuint_t)i < (sysuint_t)_d->height,
      "Fog::Image::getMScanline() - Index out of range");
    return (detach() == ERR_OK) ? _d->first + (sysint_t)i * _d->stride : NULL;
  }

  //! @brief Get mutable pointer to @c i scanline.
  //!
  //! @note Image must be detached to call this function.
  FOG_INLINE uint8_t* getXScanline(uint32_t i)
  {
    FOG_ASSERT_X((sysuint_t)i < (sysuint_t)_d->height,
      "Fog::Image::getXScanline() - Index out of range");
    FOG_ASSERT_X(isDetached(),
      "Fog::Image::getXScanline() - Not detached data.");
    return _d->first + (sysint_t)i * _d->stride;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get image width (in pixels).
  FOG_INLINE int getWidth() const { return _d->width; }
  //! @brief Get image height (in pixels).
  FOG_INLINE int getHeight() const { return _d->height; }

  //! @brief Get whether image is empty (width and height equals to zero).
  FOG_INLINE bool isEmpty() const { return _d->stride == 0; }

  //! @brief Get image stride (bytes per line).
  //!
  //! @note Stride can be 'width * bytesPerPixel', but can be also larger.
  FOG_INLINE sysint_t getStride() const { return _d->stride; }

  // --------------------------------------------------------------------------
  // [Type / Format]
  // --------------------------------------------------------------------------

  //! @brief Get image type, see @c IMAGE_TYPE enumeration.
  FOG_INLINE uint32_t getType() const { return _d->type; }

  //! @brief Get image format, see @c IMAGE_FORMAT enumeration.
  FOG_INLINE uint32_t getFormat() const { return _d->format; }
  //! @brief Get image bytes per pixel (1 or 4).
  FOG_INLINE uint32_t getBytesPerPixel() const { return _d->bytesPerPixel; }

  //! @brief Get image depth (8 or 32).
  FOG_INLINE uint32_t getDepth() const { return _d->bytesPerPixel == 0 ? 1 : _d->bytesPerPixel << 3; }

  //! @brief Get whether image is indexed (8-bit image with palette).
  FOG_INLINE bool isIndexed() const { return _d->format == IMAGE_FORMAT_I8; }

  //! @brief Get whether image uses premultiplied pixels (@c IMAGE_FORMAT_PRGB32).
  FOG_INLINE bool isPremultiplied() const { return _d->format == IMAGE_FORMAT_PRGB32; }

  // --------------------------------------------------------------------------
  // [Create / Adopt]
  // --------------------------------------------------------------------------

  //! @brief Create new image at @a w x @a h size in a given @a format.
  //!
  //! Please always check error value, because allocation memory for image data
  //! can fail. Also if there are invalid arguments (dimensions or format) the
  //! InvalidArgument will be returned.
  err_t create(int w, int h, uint32_t format, uint32_t type = IMAGE_TYPE_MEMORY);

  //! @brief Adopt memory buffer to the image.
  err_t adopt(const ImageBuffer& buffer, uint32_t adoptFlags = IMAGE_ADOPT_DEFAULT);

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  //! @brief Set other image to this image creating reference to it if possible.
  err_t set(const Image& other);

  //! @brief Copy part of other image to the image.
  err_t set(const Image& other, const IntRect& area);

  //! @brief Set other image to this image making deep copy of it.
  err_t setDeep(const Image& other);

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  //! @brief Convert image to @a format and @a type.
  err_t convert(uint32_t format);

  //! @brief Convert image to 8 bit using optimized palette or dithering.
  err_t to8Bit();

  //! @brief Convert image to 8 bit using custom palette @a pal.
  err_t to8Bit(const Palette& pal);

  //! @brief Sets image format without doing any conversion. This can be only
  //! used for formats with equal bit depths.
  //!
  //! @note You can use this method for example to bypass premultiplication
  //! or demultiplication on 32 bit images.
  err_t forceFormat(uint32_t format);

  // --------------------------------------------------------------------------
  // [Palette]
  // --------------------------------------------------------------------------

  //! @brief Get image palette.
  FOG_INLINE const Palette& getPalette() const { return _d->palette; }
  //! @brief Set image palette.
  err_t setPalette(const Palette& palette);
  //! @brief Set image palette entries.
  err_t setPalette(sysuint_t index, const Argb* rgba, sysuint_t count);

  // --------------------------------------------------------------------------
  // [Lock / Unlock Pixels]
  // --------------------------------------------------------------------------

  err_t lockPixels(ImagePixels& pixels, uint32_t lockMode = IMAGE_LOCK_READWRITE);
  err_t lockPixels(ImagePixels& pixels, const IntRect& rect, uint32_t lockMode = IMAGE_LOCK_READWRITE);
  err_t unlockPixels(ImagePixels& pixels);

  // --------------------------------------------------------------------------
  // [GetDib / SetDib]
  // --------------------------------------------------------------------------

  err_t getDib(int x, int y, uint w, uint32_t dibFormat, void* dst) const;
  err_t setDib(int x, int y, uint w, uint32_t dibFormat, const void* src);

  // --------------------------------------------------------------------------
  // [Swap RGB and ARGB]
  // --------------------------------------------------------------------------

  err_t swapRgb();
  err_t swapArgb();

  // --------------------------------------------------------------------------
  // [Premultiply / Demultiply]
  // --------------------------------------------------------------------------

  err_t premultiply();
  err_t demultiply();

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
  // [Channel related]
  // --------------------------------------------------------------------------

  Image extractChannel(uint32_t channel) const;

  // --------------------------------------------------------------------------
  // [Color Filter]
  // --------------------------------------------------------------------------

  err_t filter(const ColorFilter& f, const IntRect* area = NULL);
  err_t filter(const ColorLut& lut, const IntRect* area = NULL);
  err_t filter(const ColorMatrix& cm, const IntRect* area = NULL);

  // --------------------------------------------------------------------------
  // [Image Filter]
  // --------------------------------------------------------------------------

  err_t filter(const ImageFilter& f, const IntRect* area = NULL);

  // --------------------------------------------------------------------------
  // [Scaling]
  // --------------------------------------------------------------------------

  Image scale(const IntSize& to, uint32_t interpolationType = IMAGE_INTERPOLATION_SMOOTH);

  // --------------------------------------------------------------------------
  // [Painting]
  // --------------------------------------------------------------------------

  err_t clear(Argb c0);

  err_t drawPixel(const IntPoint& pt, Argb c0);
  err_t drawLine(const IntPoint& pt0, const IntPoint& pt1, Argb c0, bool lastPoint = true);

  err_t fillRect(const IntRect& r, Argb c0, int op = OPERATOR_SRC_OVER);

  err_t fillQGradient(const IntRect& r, Argb c0, Argb c1, Argb c2, Argb c3, int op = OPERATOR_SRC_OVER);
  err_t fillHGradient(const IntRect& r, Argb c0, Argb c1, int op = OPERATOR_SRC_OVER);
  err_t fillVGradient(const IntRect& r, Argb c0, Argb c1, int op = OPERATOR_SRC_OVER);

  err_t drawImage(const IntPoint& pt, const Image& src, uint32_t op = OPERATOR_SRC_OVER, uint32_t opacity = 255);
  err_t drawImage(const IntPoint& pt, const Image& src, const IntRect& srcRect, uint32_t op = OPERATOR_SRC_OVER, uint32_t opacity = 255);

  //! @brief Scroll data in image.
  //!
  //! @note Data that was scrolled out are unchanged.
  err_t scroll(int x, int y);

  //! @brief Scroll data in image only in rectangle @a r.
  //!
  //! @note Data that was scrolled out are unchanged.
  err_t scroll(int x, int y, const IntRect& r);

  // --------------------------------------------------------------------------
  // [Misc]
  // --------------------------------------------------------------------------

  //! @brief Check if point at a given coordinates @a x and @a y is in image.
  FOG_INLINE bool hasPoint(int x, int y) 
  { 
    return (uint)x < (uint)getWidth() && (uint)y < (uint)getHeight(); 
  }
  
  //! @brief Check if point at a given coordinates @a at is in image.
  FOG_INLINE bool hasPoint(const IntPoint& pt)
  { 
    return (uint)pt.x < (uint)getWidth() && (uint)pt.y < (uint)getHeight(); 
  }

  static err_t glyphFromPath(Image& glyph, IntPoint& offset, const DoublePath& path);

  // --------------------------------------------------------------------------
  // [Windows Support]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  //! @brief Convert image to Windows @c HBITMAP (DIBSECTION).
  //!
  //! @note This function is for Windows-only.
  HBITMAP toHBITMAP();

  //! @brief Convert image from Windows @c HBITMAP (Ideally DIBSECTION).
  //!
  //! @note This function is for Windows-only.
  err_t fromHBITMAP(HBITMAP hBitmap);

  //! @brief Get HDC mapped to the image.
  //!
  //! After the HDC is not needed anymore use @c releaseDC() method to release
  //! the device context.
  //!
  //! @note This function is for Windows-only and it returns HDC only when
  //! the image was created using @c IMAGE_TYPE_WIN_DIB type. Otherwise @c NULL
  //! is returned.
  //!
  //! @sa @c releaseDC().
  HDC getDC();

  //! @brief Release HDC mapped to the image, retrieved by @c getDC() method.
  //!
  //! @note This function is for Windows-only.
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

  //! @brief Converts a given format @a format into depth.
  static uint32_t getDepthFromFormat(uint32_t format);

  //! @brief Converts a given format @a format into bytes per pixel.
  static uint32_t getBytesPerPixelFromFormat(uint32_t format);

  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  static ImageData* _allocData(sysuint_t size);
  static ImageData* _allocData(int w, int h, uint32_t format);
  static void _freeData(ImageData* d);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(ImageData)
};

// ============================================================================
// [Defined Later]
// ============================================================================

FOG_INLINE void ImageBuffer::import(ImageData* d)
{
  width = d->width;
  height = d->height;
  format = d->format;
  stride = d->stride;
  data = d->first;
}

FOG_INLINE void ImageBuffer::import(ImageData* d, const IntRect& rect)
{
  // Rect must be normalized.
  FOG_ASSERT(rect.x >= 0 && rect.y >= 0 &&rect.x + rect.w <= d->width && rect.y + rect.h <= d->height);
  FOG_ASSERT(rect.w > 0 && rect.h > 0);

  width = rect.w;
  height = rect.h;
  format = d->format;
  stride = d->stride;
  data = d->first + (uint)rect.y * d->stride + (uint)rect.x * (uint)d->bytesPerPixel;
}

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<T>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Image , Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_GRAPHICS_IMAGE_H
