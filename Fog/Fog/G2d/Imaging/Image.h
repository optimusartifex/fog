// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGE_H
#define _FOG_G2D_IMAGING_IMAGE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/Delegate.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>
#include <Fog/G2d/Imaging/ImagePalette.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ImageVTable]
// ============================================================================

//! @brief Image virtual table.
struct FOG_NO_EXPORT ImageVTable
{
  err_t      (FOG_CDECL* create       )(ImageData** pd, const SizeI* size, uint32_t format);
  void       (FOG_CDECL* destroy      )(ImageData* d);
  void*      (FOG_CDECL* getHandle    )(const ImageData* d);
  err_t      (FOG_CDECL* updatePalette)(ImageData* d, const Range* range);
};

// ============================================================================
// [Fog::ImageData]
// ============================================================================

//! @brief Image data.
struct FOG_NO_EXPORT ImageData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageData* addRef() const
  {
    return fog_api.image_dAddRef(const_cast<ImageData*>(this));
  }

  FOG_INLINE void release()
  {
    fog_api.image_dRelease(this);
  }

  // --------------------------------------------------------------------------
  // [VTable Funcs]
  // --------------------------------------------------------------------------

  // NOTE: Create is only used to create the image, it's not used after created.

  FOG_INLINE void destroy()
  {
    vtable->destroy(this);
  }

  FOG_INLINE void* getHandle()
  {
    return vtable->getHandle(this);
  }

  FOG_INLINE err_t updatePalette(const Range& range)
  {
    return vtable->updatePalette(this, &range);
  }

  // --------------------------------------------------------------------------
  // [Helpers]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isDetached() const
  {
    return (reference.get() + ((vType & VAR_FLAG_READ_ONLY) != 0)) == 1;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

  //! @brief Whether there is exclusive access to the image.
  //!
  //! If this member is non-zero then one or more paint-engines are using the
  //! image data and image can't be referenced (assigned to an other image).
  //! It will be refused using @c ERR_RT_BUSY error code.
  uint32_t locked;

  //! @brief Image vtable.
  const ImageVTable* vtable;

  //! @brief Image size.
  SizeI size;

  union
  {
    struct
    {
      //! @brief Image format, see @c IMAGE_FORMAT.
      uint32_t format : 8;
      //! @brief Image type, see @c IMAGE_TYPE.
      uint32_t type : 5;
      //! @brief Whether the image contains adopted data.
      uint32_t adopted : 1;
      //! @brief Color key.
      uint32_t colorKey : 10;
      //! @brief Bytes per pixel.
      uint32_t bytesPerPixel : 8;
    };

    uint32_t packedProperties;
  };

#if FOG_ARCH_BITS >= 64
  uint32_t padding;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Original data pointer (in the most cases compatible with 
  uint8_t* data;
  //! @brief Base pointer to the first scanline, in most cases it's equal to
  //! @c handle[0].
  //!
  //! @note This pointer can be different to @c data member only if the image
  //! pixels are mirrored (buttom-to-top). In this case @b first points to
  //! last scanline and @c stride is negative.
  uint8_t* first;

  //! @brief Image stride.
  ssize_t stride;

  //! @brief Image palette (used only by 8-bit indexed images).
  Static<ImagePalette> palette;
};

// ============================================================================
// [Fog::Image]
// ============================================================================

//! @brief Raster image container.
struct FOG_NO_EXPORT Image
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Image()
  {
    fog_api.image_ctor(this);
  }

  FOG_INLINE Image(const Image& other)
  {
    fog_api.image_ctorCopy(this, &other);
  }

  FOG_INLINE Image(const SizeI& size, uint32_t format, uint32_t type = IMAGE_TYPE_BUFFER)
  {
    fog_api.image_ctorCreate(this, &size, format, type);
  }

  explicit FOG_INLINE Image(ImageData* d) :
    _d(d)
  {
  }
  
  FOG_INLINE ~Image()
  {
    fog_api.image_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return (getReference() + isReadOnly()) == 1; }

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : fog_api.image_detach(this);
  }

  //! @copydoc Doxygen::Implicit::_detach().
  FOG_INLINE err_t _detach()
  {
    return fog_api.image_detach(this);
  }

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::isStatic().
  FOG_INLINE bool isStatic() const { return _d->vType & VAR_FLAG_STATIC; }
  //! @brief Get whether the image is read-only.
  FOG_INLINE bool isReadOnly() const { return _d->vType & VAR_FLAG_READ_ONLY; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get image size (in pixels).
  FOG_INLINE const SizeI& getSize() const { return _d->size; }

  //! @brief Get image width (in pixels).
  FOG_INLINE int getWidth() const { return _d->size.w; }
  //! @brief Get image height (in pixels).
  FOG_INLINE int getHeight() const { return _d->size.h; }

  //! @brief Get whether the image is empty (width and height is equal to zero).
  FOG_INLINE bool isEmpty() const { return _d->stride == 0; }

  //! @brief Get image stride (bytes per line).
  //!
  //! @note Stride can be 'width * bytesPerPixel', but can be also larger.
  FOG_INLINE ssize_t getStride() const { return _d->stride; }

  //! @brief Get image format, see @c IMAGE_FORMAT enumeration.
  FOG_INLINE uint32_t getFormat() const { return _d->format; }
  //! @brief Get image type, see @c IMAGE_TYPE enumeration.
  FOG_INLINE uint32_t getType() const { return _d->type; }

  //! @brief Get image depth (8 or 32).
  FOG_INLINE uint32_t getDepth() const { return _d->bytesPerPixel == 0 ? 1 : _d->bytesPerPixel << 3; }
  //! @brief Get image bytes per pixel (1 or 4).
  FOG_INLINE uint32_t getBytesPerPixel() const { return _d->bytesPerPixel; }

  //! @brief Get image format description.
  FOG_INLINE const ImageFormatDescription& getFormatDescription() const
  {
    return ImageFormatDescription::getByFormat(_d->format);
  }

  //! @brief Get the image handle.
  //!
  //! This method can be used to retrieve handle of the original image that
  //! manages the image buffer. This image can be for example Windows DIB
  //! Section (see @c IMAGE_TYPE enumeration).
  FOG_INLINE void* getHandle() { return _d->getHandle(); }

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
    FOG_ASSERT_X(isDetached(),
      "Fog::Image::getDataX() - Not detached.");

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
    FOG_ASSERT_X(isDetached(),
      "Fog::Image::getFirstX() - Not detached.");

    return _d->first;
  }

  //! @brief Get a pointer to a scanline at index @a i (const).
  FOG_INLINE const uint8_t* getScanline(int y) const
  {
    FOG_ASSERT_X((uint)y < (uint)_d->size.h,
      "Fog::Image::getScanline() - Index out of range.");

    return _d->first + (ssize_t)y * _d->stride;
  }

  //! @brief Get a pointer to a scanline at index @a i (mutable).
  //!
  //! @note The @c Image must be detached to call this function.
  FOG_INLINE uint8_t* getScanlineX(int y)
  {
    FOG_ASSERT_X((uint)y < (uint)_d->size.h,
      "Fog::Image::getScanlineX() - Index out of range.");
    FOG_ASSERT_X(isDetached(),
      "Fog::Image::getScanlineX() - Not detached.");

    return _d->first + (ssize_t)y * _d->stride;
  }

  //! @brief Get the alpha distribution.
  FOG_INLINE uint32_t getAlphaDistribution() const
  {
    return fog_api.image_getAlphaDistribution(this);
  }

  //! @brief Called after image data was modified.
  FOG_INLINE void _modified()
  {
    return fog_api.image_modified(this);
  }

  // --------------------------------------------------------------------------
  // [Create / Adopt]
  // --------------------------------------------------------------------------

  //! @brief Create new image at @a w x @a h size in a given @a format.
  //!
  //! Please always check error value, because allocation memory for image data
  //! can fail. Also if there are invalid arguments (dimensions or format) the
  //! @c ERR_RT_INVALID_ARGUMENT will be returned.
  FOG_INLINE err_t create(const SizeI& size, uint32_t format, uint32_t type = IMAGE_TYPE_BUFFER)
  {
    return fog_api.image_create(this, &size, format, type);
  }

  //! @brief Adopt memory buffer to the image.
  FOG_INLINE err_t adopt(const ImageBits& imageBits, uint32_t adoptFlags = IMAGE_ADOPT_DEFAULT)
  {
    return fog_api.image_adopt(this, &imageBits, adoptFlags);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::reset().
  FOG_INLINE void reset()
  {
    fog_api.image_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  //! @brief Set other image to this image creating reference to it if possible.
  FOG_INLINE err_t setImage(const Image& other)
  {
    return fog_api.image_copy(this, &other);
  }

  //! @brief Copy part of other image to the image.
  FOG_INLINE err_t setImage(const Image& other, const RectI& area)
  {
    return fog_api.image_setImage(this, &other, &area);
  }

  //! @brief Set other image to this image making deep copy of it.
  FOG_INLINE err_t setDeep(const Image& other)
  {
    return fog_api.image_copyDeep(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Conversion]
  // --------------------------------------------------------------------------

  //! @brief Convert image to @a format.
  FOG_INLINE err_t convert(uint32_t format)
  {
    return fog_api.image_convert(this, format);
  }

  //! @brief Sets image format without doing any conversion. This can be only
  //! used for formats with equal bit depths.
  //!
  //! @note You can use this method for example to bypass premultiplication
  //! or demultiplication on 32 bit images.
  FOG_INLINE err_t forceFormat(uint32_t format)
  {
    return fog_api.image_forceFormat(this, format);
  }

  // --------------------------------------------------------------------------
  // [Quantization]
  // --------------------------------------------------------------------------

  //! @brief Convert image to 8 bit using optimized palette or using dithering.
  FOG_INLINE err_t convertTo8BitDepth()
  {
    return fog_api.image_convertTo8BitDepth(this);
  }

  //! @brief Convert image to 8 bit using custom palette @a pal.
  FOG_INLINE err_t convertTo8BitDepth(const ImagePalette& palette)
  {
    return fog_api.image_convertTo8BitDepthPalette(this, &palette);
  }

  // --------------------------------------------------------------------------
  // [Palette]
  // --------------------------------------------------------------------------

  //! @brief Get image palette.
  FOG_INLINE const ImagePalette& getPalette() const
  {
    return _d->palette;
  }
  
  //! @brief Set image palette.
  FOG_INLINE err_t setPalette(const ImagePalette& palette)
  {
    return fog_api.image_setPalette(this, &palette);
  }

  //! @brief Set image palette data.
  FOG_INLINE err_t setPalette(const Range& range, const Argb32* data)
  {
    return fog_api.image_setPaletteData(this, &range, data);
  }

  // --------------------------------------------------------------------------
  // [Clear / Fill]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t clear(const Argb32& argb32)
  {
    return fog_api.image_clearArgb32(this, &argb32);
  }

  FOG_INLINE err_t clear(const Color& color)
  {
    return fog_api.image_clearColor(this, &color);
  }

  FOG_INLINE err_t fillRect(const RectI& r, const Argb32& argb32,
    uint32_t compositingOperator = COMPOSITE_SRC_OVER, float opacity = 1.0f)
  {
    return fog_api.image_fillRectArgb32(this, &r, &argb32, compositingOperator, opacity);
  }

  FOG_INLINE err_t fillRect(const RectI& r, const Color& color,
    uint32_t compositingOperator = COMPOSITE_SRC_OVER, float opacity = 1.0f)
  {
    return fog_api.image_fillRectColor(this, &r, &color, compositingOperator, opacity);
  }

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t blitImage(const PointI& pt, const Image& i,
    uint32_t compositingOperator = COMPOSITE_SRC_OVER, float opacity = 1.0f)
  {
    return fog_api.image_blitImageAt(this, &pt, &i, NULL, compositingOperator, opacity);
  }

  FOG_INLINE err_t blitImage(const PointI& pt, const Image& i, const RectI& iFragment,
    uint32_t compositingOperator = COMPOSITE_SRC_OVER, float opacity = 1.0f)
  {
    return fog_api.image_blitImageAt(this, &pt, &i, &iFragment, compositingOperator, opacity);
  }

  // --------------------------------------------------------------------------
  // [Scroll]
  // --------------------------------------------------------------------------

  //! @brief Scroll data in image.
  //!
  //! @note Data that was scrolled out are unchanged.
  FOG_INLINE err_t scroll(const PointI& pt)
  {
    return fog_api.image_scroll(this, &pt, NULL);
  }

  //! @overload.
  FOG_INLINE err_t scroll(int x, int y)
  {
    PointI pt(x, y);
    return fog_api.image_scroll(this, &pt, NULL);
  }

  //! @brief Scroll data in image only in @a rect.
  //!
  //! @note Data that was scrolled out are unchanged.
  FOG_INLINE err_t scroll(const PointI& pt, const RectI& rect)
  {
    return fog_api.image_scroll(this, &pt, &rect);
  }

  //! @overload.
  FOG_INLINE err_t scroll(int x, int y, const RectI& rect)
  {
    PointI pt(x, y);
    return fog_api.image_scroll(this, &pt, &rect);
  }

  // --------------------------------------------------------------------------
  // [Effects]
  // --------------------------------------------------------------------------

  //! @brief Invert color channels.
  FOG_INLINE err_t invert(uint32_t channels)
  {
    return fog_api.image_invert(this, this, NULL, channels);
  }

  //! @brief Mirror.
  FOG_INLINE err_t mirror(uint32_t mirrorMode)
  {
    return fog_api.image_mirror(this, this, NULL, mirrorMode);
  }
  
  //! @brief Rotate image by 0, 90, 180 or 270 degrees, see @c IMAGE_ROTATE_MODE.
  FOG_INLINE err_t rotate(uint32_t rotateMode)
  {
    return fog_api.image_rotate(this, this, NULL, rotateMode);
  }

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
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Check if point at a given coordinates @a pt is in image.
  FOG_INLINE bool hitTest(const PointI& pt) const
  {
    return (uint)pt.x < (uint)getWidth() && (uint)pt.y < (uint)getHeight();
  }

  //! @brief Check if point at a given coordinates @a x and @a y is in image.
  FOG_INLINE bool hitTest(int x, int y) const
  {
    return (uint)x < (uint)getWidth() && (uint)y < (uint)getHeight();
  }

  // --------------------------------------------------------------------------
  // [Read]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t readFromFile(const StringW& fileName)
  {
    return fog_api.image_readFromFile(this, &fileName);
  }

  FOG_INLINE err_t readFromStream(Stream& stream)
  {
    return fog_api.image_readFromStream(this, &stream, NULL);
  }

  FOG_INLINE err_t readFromStream(Stream& stream, const StringW& extension)
  {
    return fog_api.image_readFromStream(this, &stream, &extension);
  }

  FOG_INLINE err_t readFromBuffer(const StringA& buffer)
  {
    return fog_api.image_readFromBufferStringA(this, &buffer, NULL);
  }

  FOG_INLINE err_t readFromBuffer(const StringA& buffer, const StringW& extension)
  {
    return fog_api.image_readFromBufferStringA(this, &buffer, &extension);
  }

  FOG_INLINE err_t readFromBuffer(const void* buffer, size_t size)
  {
    return fog_api.image_readFromBufferRaw(this, buffer, size, NULL);
  }

  FOG_INLINE err_t readFromBuffer(const void* buffer, size_t size, const StringW& extension)
  {
    return fog_api.image_readFromBufferRaw(this, buffer, size, &extension);
  }

  // --------------------------------------------------------------------------
  // [Write]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t writeToFile(const StringW& fileName) const
  {
    return fog_api.image_writeToFile(this, &fileName, NULL);
  }

  FOG_INLINE err_t writeToFile(const StringW& fileName, const Hash<StringW, Var>& options) const
  {
    return fog_api.image_writeToFile(this, &fileName, &options);
  }

  FOG_INLINE err_t writeToStream(Stream& stream, const StringW& extension) const
  {
    return fog_api.image_writeToStream(this, &stream, &extension, NULL);
  }

  FOG_INLINE err_t writeToStream(Stream& stream, const StringW& extension, const Hash<StringW, Var>& options) const
  {
    return fog_api.image_writeToStream(this, &stream, &extension, &options);
  }

  FOG_INLINE err_t writeToBuffer(StringA& buffer, uint32_t cntOp, const StringW& extension) const
  {
    return fog_api.image_writeToBuffer(this, &buffer, cntOp, &extension, NULL);
  }

  FOG_INLINE err_t writeToBuffer(StringA& buffer, uint32_t cntOp, const StringW& extension, const Hash<StringW, Var>& options) const
  {
    return fog_api.image_writeToBuffer(this, &buffer, cntOp, &extension, &options);
  }

  // --------------------------------------------------------------------------
  // [Windows Support]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  //! @brief Convert image to Windows @c HBITMAP (DIBSECTION).
  //!
  //! @note This function is for Windows-only.
  FOG_INLINE err_t toWinBitmap(HBITMAP& hBitmap)
  {
    return fog_api.image_toWinBitmap(this, &hBitmap);
  }

  //! @brief Convert image from Windows @c HBITMAP.
  //!
  //! @note This function is for Windows-only.
  FOG_INLINE err_t fromWinBitmap(HBITMAP hBitmap)
  {
    return fog_api.image_fromWinBitmap(this, hBitmap);
  }

  FOG_INLINE err_t getDC(HDC* hDC)
  {
    return fog_api.image_getDC(this, hDC);
  }

  FOG_INLINE err_t releaseDC(HDC hDC)
  {
    return fog_api.image_releaseDC(this, hDC);
  }
#endif // FOG_OS_WINDOWS

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Image& other) const
  {
    return fog_api.image_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Image& operator=(const Image& other)
  {
    fog_api.image_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const Image& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Image& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const Image& empty()
  {
    return *fog_api.image_oEmpty;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const Image* a, const Image* b)
  {
    return fog_api.image_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.image_eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Helpers]
  // --------------------------------------------------------------------------

  //! @brief Calculate stride for a given image @a width and @a depth.
  //!
  //! Stride is calculated using @a width and @a depth using 32-bit alignment
  //! that is compatible with Windows DIBs and probably to other OS specific
  //! image formats.
  static FOG_INLINE ssize_t getStrideFromWidth(int width, uint32_t depth)
  {
    return fog_api.image_getStrideFromWidth(width, depth);
  }

  // --------------------------------------------------------------------------
  // [Statics - GlyphFromPath]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t glyphFromPath(Image& dst, PointI& dstOffset, const PathF& path, uint32_t fillRule, uint32_t precision)
  {
    return fog_api.image_glyphFromPathF(&dst, &dstOffset, &path, fillRule, precision);
  }

  static FOG_INLINE err_t glyphFromPath(Image& dst, PointI& dstOffset, const PathD& path, uint32_t fillRule, uint32_t precision)
  {
    return fog_api.image_glyphFromPathD(&dst, &dstOffset, &path, fillRule, precision);
  }

  // --------------------------------------------------------------------------
  // [Statics - Effects]
  // --------------------------------------------------------------------------

  //! @brief Invert color channels.
  static FOG_INLINE err_t invert(Image& dst, const Image& src, uint32_t channels)
  {
    return fog_api.image_invert(&dst, &src, NULL, channels);
  }

  //! @overload.
  static FOG_INLINE err_t invert(Image& dst, const Image& src, const RectI& area, uint32_t channels)
  {
    return fog_api.image_invert(&dst, &src, &area, channels);
  }

  //! @brief Mirror.
  static FOG_INLINE err_t mirror(Image& dst, const Image& src, uint32_t mirrorMode)
  {
    return fog_api.image_mirror(&dst, &src, NULL, mirrorMode);
  }

  //! @overload.
  static FOG_INLINE err_t mirror(Image& dst, const Image& src, const RectI& area, uint32_t mirrorMode)
  {
    return fog_api.image_mirror(&dst, &src, &area, mirrorMode);
  }

  //! @brief Rotate image by 0, 90, 180 or 270 degrees, see @c IMAGE_ROTATE_MODE.
  static FOG_INLINE err_t rotate(Image& dst, const Image& src, uint32_t rotateMode)
  {
    return fog_api.image_rotate(&dst, &src, NULL, rotateMode);
  }

  static FOG_INLINE err_t rotate(Image& dst, const Image& src, const RectI& area, uint32_t rotateMode)
  {
    return fog_api.image_rotate(&dst, &src, &area, rotateMode);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ImageData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGE_H
