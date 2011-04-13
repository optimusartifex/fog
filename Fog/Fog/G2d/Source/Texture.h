// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_TEXTURE_H
#define _FOG_G2D_SOURCE_TEXTURE_H

// [Dependencies]
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

// ============================================================================
// [Fog::Texture]
// ============================================================================

struct FOG_NO_EXPORT Texture
{
  FOG_INLINE Texture() :
    _fragment(0, 0, 0, 0),
    _tileType(TEXTURE_TILE_DEFAULT)
  {
  }

  FOG_INLINE Texture(const Image& image) :
    _image(image),
    _fragment(0, 0, image.getWidth(), image.getHeight()),
    _tileType(TEXTURE_TILE_DEFAULT)
  {
  }

  FOG_INLINE Texture(const Image& image, uint32_t tileType) :
    _image(image),
    _fragment(0, 0, image.getWidth(), image.getHeight()),
    _tileType(tileType)
  {
  }

  FOG_INLINE Texture(const Image& image, uint32_t tileType, const Color& clampColor) :
    _image(image),
    _fragment(0, 0, image.getWidth(), image.getHeight()),
    _tileType(tileType),
    _clampColor(clampColor)
  {
  }

  FOG_INLINE Texture(const Image& image, const RectI& fragment) :
    _image(image),
    _fragment(fragment),
    _tileType(TEXTURE_TILE_DEFAULT)
  {
  }

  FOG_INLINE Texture(const Image& image, const RectI& fragment, uint32_t tileType) :
    _image(image),
    _fragment(fragment),
    _tileType(tileType)
  {
  }

  FOG_INLINE Texture(const Image& image, const RectI& fragment, uint32_t tileType, const Color& clampColor) :
    _image(image),
    _fragment(fragment),
    _tileType(tileType),
    _clampColor(clampColor)
  {
  }

  FOG_INLINE Texture(const Texture& other) :
    _image(other._image),
    _fragment(other._fragment),
    _tileType(other._tileType),
    _clampColor(other._clampColor)
  {
  }

  FOG_INLINE ~Texture()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const Image& getImage() const { return _image; }
  FOG_INLINE const RectI& getFragment() const { return _fragment; }
  FOG_INLINE uint32_t getTileType() const { return _tileType; }
  FOG_INLINE const Color& getClampColor() const { return _clampColor; }

  FOG_INLINE void setImage(const Image& image) { _image = image; }
  FOG_INLINE void setFragment(const RectI& fragment) { _fragment = fragment; }
  FOG_INLINE void setTileType(uint32_t tileType) { _tileType = tileType; }
  FOG_INLINE void setClampColor(const Color& clampColor) { _clampColor = clampColor; }

  FOG_INLINE void setTexture(const Image& image)
  {
    _image = image;
    _fragment.set(0, 0, image.getWidth(), image.getHeight());
    _tileType = TEXTURE_TILE_DEFAULT;
    _clampColor.reset();
  }

  FOG_INLINE void setTexture(const Image& image, uint32_t tileType)
  {
    _image = image;
    _fragment.set(0, 0, image.getWidth(), image.getHeight());
    _tileType = tileType;
    _clampColor.reset();
  }

  FOG_INLINE void setTexture(const Image& image, uint32_t tileType, const Color& clampColor)
  {
    _image = image;
    _fragment.set(0, 0, image.getWidth(), image.getHeight());
    _tileType = tileType;
    _clampColor = clampColor;
  }

  FOG_INLINE void setTexture(const Image& image, const RectI& fragment)
  {
    _image = image;
    _fragment = fragment;
    _tileType = TEXTURE_TILE_DEFAULT;
    _clampColor.reset();
  }

  FOG_INLINE void setTexture(const Image& image, const RectI& fragment, uint32_t tileType)
  {
    _image = image;
    _fragment = fragment;
    _tileType = tileType;
    _clampColor.reset();
  }

  FOG_INLINE void setTexture(const Image& image, const RectI& fragment, uint32_t tileType, const Color& clampColor)
  {
    _image = image;
    _fragment = fragment;
    _tileType = tileType;
    _clampColor = clampColor;
  }

  FOG_INLINE void setTexture(const Texture& texture)
  {
    _image = texture._image;
    _fragment = texture._fragment;
    _tileType = texture._tileType;
    _clampColor = texture._clampColor;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _image.reset();
    _fragment.reset();
    _tileType = TEXTURE_TILE_DEFAULT;
    _clampColor.reset();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Texture& operator=(const Texture& other)
  {
    setTexture(other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Image instance.
  Image _image;
  //! @brief Image fragment (can't be outside bounds).
  RectI _fragment;
  //! @brief Tile mode.
  uint32_t _tileType;
  //! @brief The background color for @c TEXTURE_TILE_CLAMP tile-type.
  Color _clampColor;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SOURCE_TEXTURE_H
