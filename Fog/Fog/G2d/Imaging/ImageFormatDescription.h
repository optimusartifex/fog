// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEFORMATDESCRIPTION_H
#define _FOG_G2D_IMAGING_IMAGEFORMATDESCRIPTION_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Memory/MemoryOps.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ImageFormatDescription]
// ============================================================================

struct FOG_NO_EXPORT ImageFormatDescription
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getFormat() const { return _format; }
  FOG_INLINE uint32_t getDepth() const { return _depth; }
  FOG_INLINE uint32_t getBytesPerPixel() const { return _bytesPerPixel; }
  FOG_INLINE uint32_t getPrecision() const { return _precision; }

  FOG_INLINE uint32_t getCompatibleFormat() const { return _g2d.imageformatdescription.getCompatibleFormat(this); }

  FOG_INLINE uint32_t getComponentMask() const { return _componentMask; }

  //! @brief Get whether the image format is premultiplied.
  FOG_INLINE uint32_t isPremultiplied() const { return _isPremultiplied; }

  //! @brief Get whether the image format is indexed (@c IMAGE_FORMAT_I8).
  FOG_INLINE uint32_t isIndexed() const { return _isIndexed; }

  //! @brief Get whether the image format is byte-swapped.
  //!
  //! @note Byte-swapping is turned off in case that the mask can be changed,
  //! but there are image formats where byte-swapping cannot be turned off,
  //! for example 16-bit 555/565 formats and 16-bit per component formats.
  FOG_INLINE uint32_t isByteSwapped() const { return _isByteSwapped; }

  //! @brief Get whether the components are 8-bit long and aligned with the bytes
  //!
  //! @note This is hint for image converter special cases.
  FOG_INLINE uint32_t hasAlignedComponents() const { return _hasAlignedComponents; }

  //! @brief Get whether the unused bits should be filled by ones.
  FOG_INLINE uint32_t fillUnusedBits() const { return _fillUnusedBits; }

  //! @brief Get the unused bits mask (32-bit).
  FOG_INLINE uint32_t getUnusedBits32() const { return ~(0xFFFFFFFFU << _depth) & ((uint32_t)_aMask | (uint32_t)_rMask | (uint32_t)_gMask | (uint32_t)_bMask); }
  //! @brief Get the unused bits mask (64-bit).
  FOG_INLINE uint64_t getUnusedBits64() const { return ~(FOG_UINT64_C(0xFFFFFFFFFFFFFFFF) << _depth) & (_aMask | _rMask | _gMask | _bMask); }

  //! @brief Get the unused bits mask (32-bit).
  FOG_INLINE uint32_t getUsedBits32() const { return (uint32_t)_aMask | (uint32_t)_rMask | (uint32_t)_gMask | (uint32_t)_bMask; }
  //! @brief Get the unused bits mask (64-bit).
  FOG_INLINE uint64_t getUsedBits64() const { return _aMask | _rMask | _gMask | _bMask; }

  FOG_INLINE uint32_t getAPos() const { return _aPos; }
  FOG_INLINE uint32_t getRPos() const { return _rPos; }
  FOG_INLINE uint32_t getGPos() const { return _gPos; }
  FOG_INLINE uint32_t getBPos() const { return _bPos; }

  FOG_INLINE uint32_t getASize() const { return _aSize; }
  FOG_INLINE uint32_t getRSize() const { return _rSize; }
  FOG_INLINE uint32_t getGSize() const { return _gSize; }
  FOG_INLINE uint32_t getBSize() const { return _bSize; }

  FOG_INLINE uint32_t getAMax() const { return (1U << _aSize) - 1; }
  FOG_INLINE uint32_t getRMax() const { return (1U << _rSize) - 1; }
  FOG_INLINE uint32_t getGMax() const { return (1U << _gSize) - 1; }
  FOG_INLINE uint32_t getBMax() const { return (1U << _bSize) - 1; }

  FOG_INLINE uint32_t getAMask32() const { return (uint32_t)_aMask; }
  FOG_INLINE uint32_t getRMask32() const { return (uint32_t)_rMask; }
  FOG_INLINE uint32_t getGMask32() const { return (uint32_t)_gMask; }
  FOG_INLINE uint32_t getBMask32() const { return (uint32_t)_bMask; }

  FOG_INLINE uint64_t getAMask64() const { return _aMask; }
  FOG_INLINE uint64_t getRMask64() const { return _rMask; }
  FOG_INLINE uint64_t getGMask64() const { return _gMask; }
  FOG_INLINE uint64_t getBMask64() const { return _bMask; }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const { return _depth != 0; }

  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  //! @brief Create the pixel format which uses up to 16 bits per component.
  //!
  //! This method is designed to setup the integer based pixel format.
  FOG_INLINE err_t createArgb(
    uint32_t depth, uint32_t flags,
    uint64_t aMask, uint64_t rMask, uint64_t gMask, uint64_t bMask)
  {
    return _g2d.imageformatdescription.createArgb(this,
      depth, flags, aMask, rMask, gMask, bMask);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    memset(this, 0, sizeof(*this));
    _format = IMAGE_FORMAT_NULL;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator==(const ImageFormatDescription& other) const
  {
    // Small optimization useful when comparing a list of image format
    // descriptions in a loop. If both formats are equal and built-in then
    // we can simply return true.
    if (_format != other._format) return false;
    if (_format < IMAGE_FORMAT_COUNT) return true;

    return Memory::eq_t<ImageFormatDescription>(this, &other);
  }

  FOG_INLINE bool operator!=(const ImageFormatDescription& other)
  {
    if (_format != other._format) return true;

    return !Memory::eq_t<ImageFormatDescription>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE const ImageFormatDescription* getList();
  static FOG_INLINE const ImageFormatDescription& getByFormat(uint32_t id);

  static FOG_INLINE ImageFormatDescription fromArgb(
    uint32_t depth, uint32_t flags,
    uint64_t aMask, uint64_t rMask, uint64_t gMask, uint64_t bMask)
  {
    ImageFormatDescription desc;
    desc.createArgb(depth, flags, aMask, rMask, gMask, bMask);
    return desc;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The related image format id, see @c IMAGE_FORMAT.
  uint16_t _format;
  //! @brief The image depth.
  uint16_t _depth;

  //! @brief The bytes per pixel (depth / 8).
  uint8_t _bytesPerPixel;
  //! @brief The precision, see @c IMAGE_PRECISION.
  uint8_t _precision;
  //! @brief The components.
  uint8_t _componentMask;

  //! @brief Whether the R.G.B components are premultiplied by the alpha.
  uint8_t _isPremultiplied : 1;
  //! @brief Whether the pixel is index to an external @c Palette.
  uint8_t _isIndexed : 1;
  //! @brief Whether the pixel is byte-swapped.
  uint8_t _isByteSwapped : 1;
  //! @brief Whether the pixel components are aligned to bytes/words/dwords (hint).
  uint8_t _hasAlignedComponents : 1;
  //! @brief Whether to fill all unused bits (internal).
  uint8_t _fillUnusedBits : 1;
  //! @brief Reserved bits, currently unused.
  uint8_t _reservedBits : 3;

  //! @brief The 'Alpha' component position in bits (0...63).
  uint8_t _aPos;
  //! @brief The 'Red' component position in bits (0...63).
  uint8_t _rPos;
  //! @brief The 'Green' component position in bits (0...63).
  uint8_t _gPos;
  //! @brief The 'Blue' component position in bits (0...63).
  uint8_t _bPos;

  //! @brief The 'Alpha' component bit-size (for depth <= 64).
  uint8_t _aSize;
  //! @brief The 'Red' component bit-size (for depth <= 64).
  uint8_t _rSize;
  //! @brief The 'Green' component bit-size (for depth <= 64).
  uint8_t _gSize;
  //! @brief The 'Blue' component bit-size (for depth <= 64).
  uint8_t _bSize;

  //! @brief The 'Alpha' component mask (for depth <= 64).
  uint64_t _aMask;
  //! @brief The 'Red' component mask (for depth <= 64).
  uint64_t _rMask;
  //! @brief The 'Green' component mask (for depth <= 64).
  uint64_t _gMask;
  //! @brief The 'Blue' component mask (for depth <= 64).
  uint64_t _bMask;
};

extern FOG_API const ImageFormatDescription _G2d_ImageFormatDescription_list[IMAGE_FORMAT_COUNT + 1];

FOG_INLINE const ImageFormatDescription* ImageFormatDescription::getList()
{
  return _G2d_ImageFormatDescription_list;
}

FOG_INLINE const ImageFormatDescription& ImageFormatDescription::getByFormat(uint32_t id)
{
  FOG_ASSERT(id <= IMAGE_FORMAT_COUNT);
  return _G2d_ImageFormatDescription_list[id];
}

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::ImageFormatDescription, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEFORMATDESCRIPTION_H
