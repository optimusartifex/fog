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
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Palette.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ColorFilter;
struct ColorLut;
struct ColorMatrix;
struct ImageFilter;
struct DoublePath;

// ============================================================================
// [Fog::ImageBuffer]
// ============================================================================

//! @brief Structure that holds information about raster image buffer.
//!
//! Use it together with @c Image::adopt() or @c Painter::begin() methods.
struct ImageBuffer
{
  //! @brief Image buffer width.
  int width;
  //! @brief Image buffer height.
  int height;
  //! @brief Image buffer format.
  int format;
  //! @brief Image buffer stride (bytes per line).
  sysint_t stride;
  //! @brief Pointer to first image scanline.
  uint8_t* data;
};

// ============================================================================
// [Fog::Image]
// ============================================================================

//! @brief Raster image container.
struct FOG_API Image
{
  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  struct FOG_API Data
  {
    enum
    {
      IsDynamic = (1U << 0),
      IsSharable = (1U << 1),
      IsStrong = (1U << 2),
      IsReadOnly = (1U << 3)
    };

    Data();
    ~Data();

    // [Ref / Deref]

    Data* ref() const;
    void deref();

    FOG_INLINE Data* refAlways() const { refCount.inc(); return const_cast<Data*>(this); }

    static Data* alloc(sysuint_t size);
    static Data* alloc(int w, int h, int format);
    static void free(Data* d);

    static Data* copy(const Data* other);

    // [Members]

    //! @brief Reference count.
    mutable Atomic<sysuint_t> refCount;
    //! @brief Image flags.
    uint32_t flags;
    //! @brief Image width.
    int width;
    //! @brief Image height.
    int height;
    //! @brief Image pixel format.
    int format;
    //! @brief Image depth.
    int depth;
    //! @brief Image bytes per pixel
    int bytesPerPixel;
    //! @brief Image stride.
    sysint_t stride;
    //! @brief Image palette (only for 8 bit indexed images).
    Palette palette;
    //! @brief Data pointer that points to raw memory received from image device.
    uint8_t* data;
    //! @brief Base pointer to scanline, in most cases it's equal to @c _data.
    uint8_t* first;
    //! @brief Size of @c buffer.
    sysuint_t size;
    //! @brief Buffer for static allocations.
    uint8_t buffer[4];
  };

  static Static<Data> sharedNull;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Image();
  FOG_INLINE explicit Image(Data* d) : _d(d) {}
  Image(const Image& other);
  Image(int w, int h, int format);
  ~Image();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::refCount().
  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return refCount() == 1 || isReadOnly(); }
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
  FOG_INLINE bool isNull() const { return _d == sharedNull.instancep(); }
  //! @copydoc Doxygen::Implicit::isDynamic().
  FOG_INLINE bool isDynamic() const { return _d->flags & Data::IsDynamic; }
  //! @copydoc Doxygen::Implicit::isSharable().
  FOG_INLINE bool isSharable() const { return _d->flags & Data::IsSharable; }
  //! @copydoc Doxygen::Implicit::isStrong().
  FOG_INLINE bool isStrong() const { return _d->flags & Data::IsStrong; }
  //! @brief Returns true if image is read only.
  FOG_INLINE bool isReadOnly() const { return _d->flags & Data::IsReadOnly; }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Get constant pointer to image data (first byte in image buffer).
  //!
  //! Note that image data should be usually equal to image first scanline. But
  //! it's possible that this pointer will be last image scanline in cases that
  //! raster data are filled from bottom to top (Windows DIBs). So if you want
  //! to write portable code, use always @c cFirst() or @c cScanline() methods.
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
    FOG_ASSERT_X(isDetached(), "Fog::Image::getXFirst() - Not detached data.");
    return _d->first;
  }

  //! @brief Get constant pointer to @c i scanline.
  FOG_INLINE const uint8_t* getScanline(uint32_t i) const
  {
    FOG_ASSERT_X((sysuint_t)i < (sysuint_t)_d->height, "Fog::Image::getScanline() - Index out of range");
    return _d->first + (sysint_t)i * _d->stride;
  }

  //! @brief Get mutable pointer to @c i scanline.
  FOG_INLINE uint8_t* getMScanline(uint32_t i)
  {
    FOG_ASSERT_X((sysuint_t)i < (sysuint_t)_d->height, "Fog::Image::getScanline() - Index out of range");
    return (detach() == ERR_OK) ? _d->first + (sysint_t)i * _d->stride : NULL;
  }

  //! @brief Get mutable pointer to @c i scanline.
  //!
  //! @note Image must be detached to call this function.
  FOG_INLINE uint8_t* getXScanline(uint32_t i)
  {
    FOG_ASSERT_X((sysuint_t)i < (sysuint_t)_d->height, "Fog::Image::getScanline() - Index out of range");
    FOG_ASSERT_X(isDetached(), "Fog::Image::getXScanline() - Not detached data.");
    return _d->first + (sysint_t)i * _d->stride;
  }

  // --------------------------------------------------------------------------
  // [Dimensions]
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
  // [Format]
  // --------------------------------------------------------------------------

  //! @brief Get image format, see @c PIXEL_FORMAT enumeration.
  FOG_INLINE int getFormat() const { return _d->format; }
  //! @brief Get image depth (8, 24 or 32).
  FOG_INLINE int getDepth() const { return _d->depth; }
  //! @brief Get image bytes per pixel.
  FOG_INLINE int getBytesPerPixel() const { return _d->bytesPerPixel; }

  //! @brief Get whether image is indexed (8-bit image with palette).
  FOG_INLINE bool isIndexed() const { return _d->format == PIXEL_FORMAT_I8; }

  //! @brief Get whether image is premultiplied (@c PIXEL_FORMAT_PRGB32).
  FOG_INLINE bool isPremultiplied() const { return _d->format == PIXEL_FORMAT_PRGB32; }

  // --------------------------------------------------------------------------
  // [Create / Adopt]
  // --------------------------------------------------------------------------

  //! @brief Create new image at @a w x @a h size in a given @a format.
  //!
  //! Please always check error value, because allocation memory for image data
  //! can fail. Also if there are invalid arguments (dimensions or format) the
  //! InvalidArgument will be returned.
  err_t create(int w, int h, int format);

  //! @brief Adopt memory buffer to the image.
  err_t adopt(const ImageBuffer& buffers, uint32_t adoptFlags = IMAGE_ADOPT_DEFAULT);

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  //! @brief Set other image to this image creating reference to it if possible.
  err_t set(const Image& other);
  //! @brief Set other image to this image making deep copy of it.
  err_t setDeep(const Image& other);

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  //! @brief Convert image to @a format.
  err_t convert(int format);

  //! @brief Convert image to 8 bit using optimized palette or dithering.
  err_t to8Bit();

  //! @brief Convert image to 8 bit using custom palette @a pal.
  err_t to8Bit(const Palette& pal);

  //! @brief Sets image format without doing any conversion. This can be only
  //! used for formats with equal bit depths.
  //!
  //! @note You can use this method for example to bypass premultiplication
  //! or demultiplication on 32 bit images.
  err_t forceFormat(int format);

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
  // [GetDib / SetDib]
  // --------------------------------------------------------------------------

  err_t getDib(int x, int y, uint w, int dibFormat, void* dst) const;
  err_t setDib(int x, int y, uint w, int dibFormat, const void* src);

  // --------------------------------------------------------------------------
  // [Swap RGB and RGBA]
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

  Image scale(const IntSize& to, int interpolationType = INTERPOLATION_SMOOTH);

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

  err_t blitImage(const IntPoint& pt, const Image& src, uint32_t op = OPERATOR_SRC_OVER, uint32_t opacity = 255);
  err_t blitImage(const IntPoint& pt, const Image& src, const IntRect& srcRect, uint32_t op = OPERATOR_SRC_OVER, uint32_t opacity = 255);

  //! @brief Scroll data in image.
  //!
  //! @note Data that was scrolled out are unchanged.
  err_t scroll(int x, int y);
  //! @brief Scroll data in image in rectangle @a r.
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
  // [WinAPI functions]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)

  // TODO: Introduce ENUM to set alpha format of HBITMAP.

  //! @brief Convert image to Windows @c HBITMAP (DIBSECTION).
  //!
  //! @note This function is Windows-only.
  HBITMAP toHBITMAP();
  //! @brief Convert image from Windows @c HBITMAP (Ideally DIBSECTION).
  //!
  //! @note This function is Windows-only.
  err_t fromHBITMAP(HBITMAP hBitmap);
#endif // FOG_OS_WINDOWS

  // --------------------------------------------------------------------------
  // [Read]
  // --------------------------------------------------------------------------

  err_t readFile(const String& fileName);
  err_t readStream(Stream& stream);
  err_t readStream(Stream& stream, const String& extension);
  err_t readBuffer(const ByteArray& buffer);
  err_t readBuffer(const ByteArray& buffer, const String& extension);
  err_t readBuffer(const void* buffer, sysuint_t size);
  err_t readBuffer(const void* buffer, sysuint_t size, const String& extension);

  // --------------------------------------------------------------------------
  // [Write]
  // --------------------------------------------------------------------------

  err_t writeFile(const String& fileName) const;
  err_t writeStream(Stream& stream, const String& extension) const;
  err_t writeBuffer(ByteArray& buffer, const String& extension) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Image& operator=(const Image& other) { set(other); return *this; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  //! @brief Calculate stride for a given image @a width and @a depth.
  //!
  //! Stride is calculated using @a width and @a depth using 32-bit alignment
  //! that is compatible to Windows DIBs and probably to other OS specific
  //! image formats.
  static sysint_t calcStride(int width, int depth);

  //! @brief Converts a given format @a format into depth.
  static int formatToDepth(int format);

  //! @brief Converts a given format @a format into bytes per pixel.
  static int formatToBytesPerPixel(int format);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(Data)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_IMAGE_H
