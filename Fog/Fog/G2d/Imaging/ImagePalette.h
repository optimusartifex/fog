// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEPALETTE_H
#define _FOG_G2D_IMAGING_IMAGEPALETTE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ImagePaletteData]
// ============================================================================

struct FOG_NO_EXPORT ImagePaletteData
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  typedef uint8_t (FOG_FASTCALL *FindRgbFunc)(const ImagePaletteData* d, uint32_t r, uint32_t g, uint32_t b);

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE ImagePaletteData* addRef() const
  {
    reference.inc();
    return const_cast<ImagePaletteData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      MemMgr::free(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // ${VAR:BEGIN}
  //
  // This data-object is binary compatible with the VarData header in the first
  // form called - "implicitly shared class". The members must be binary
  // compatible with the header below:
  //
  // +==============+============+============================================+
  // | Size         | Name       | Description / Purpose                      |
  // +==============+============+============================================+
  // | size_t       | reference  | Atomic reference count, can be managed by  |
  // |              |            | VarData without calling container specific |
  // |              |            | methods.                                   |
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | vType      | Variable type and flags.                   |
  // +==============+============+============================================+
  //
  // ${VAR:END}

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

  //! @brief Count of palette entries used.
  uint32_t length;

  //! @brief Specialized function to find palette entry which is closest to
  //! a given RGB.
  FindRgbFunc findRgbFunc;

  //! @brief Palette data.
  Argb32 data[256];
};

// ============================================================================
// [Fog::ImagePalette]
// ============================================================================

//! @brief The ImagePalette is array of 256 RGB32 color elements which
//! represents colors in 8-bit indexed image (the pixel value is index to the
//! palette array).
struct FOG_NO_EXPORT ImagePalette
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ImagePalette()
  {
    fog_api.imagepalette_ctor(this);
  }

  FOG_INLINE ImagePalette(const ImagePalette& other)
  {
    fog_api.imagepalette_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE ImagePalette(ImagePalette&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  explicit FOG_INLINE ImagePalette(ImagePaletteData* d) : _d(d)
  {
  }

  FOG_INLINE ~ImagePalette()
  {
    fog_api.imagepalette_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  //! @copydoc Doxygen::Implicit::_detach().
  FOG_INLINE err_t _detach() { return fog_api.imagepalette_detach(this); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get palette data.
  FOG_INLINE const Argb32* getData() const
  {
    return _d->data;
  }

  //! @brief Get mutable data without calling detach().
  FOG_INLINE Argb32* getDataX()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::ImagePalette::getDataX() - Not detached.");

    return reinterpret_cast<Argb32*>(_d->data);
  }

  //! @brief Set palette data to @a other, making weak-copy.
  FOG_INLINE err_t setData(const ImagePalette& other)
  {
    return fog_api.imagepalette_copy(this, &other);
  }

  //! @brief Set palette data to @a other, making deep-copy.
  FOG_INLINE err_t setDeep(const ImagePalette& other)
  {
    return fog_api.imagepalette_setDeep(this, &other);
  }

  //! @brief Replace palette data at @a range by @a data.
  FOG_INLINE err_t setData(const Range& range, const Argb32* data)
  {
    return fog_api.imagepalette_setData(this, &range, data);
  }

  //! @brief Get palette length.
  FOG_INLINE size_t getLength() const
  {
    return _d->length;
  }

  //! @brief Set palette length.
  FOG_INLINE err_t setLength(size_t length)
  {
    return fog_api.imagepalette_setLength(this, length);
  }

  // --------------------------------------------------------------------------
  // [At]
  // --------------------------------------------------------------------------

  FOG_INLINE Argb32 getAt(size_t index) const
  {
    FOG_ASSERT_X(index < 256,
      "Fog::ImagePalette::at() - Index out of range.");

    return _d->data[index];
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.imagepalette_clear(this);
  }

  FOG_INLINE void reset()
  {
    fog_api.imagepalette_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Find]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t findRgb(uint32_t r, uint32_t g, uint32_t b) const
  {
    return _d->findRgbFunc(_d, r, g, b);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const ImagePalette& other) const
  {
    return fog_api.imagepalette_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const ImagePalette& operator=(const ImagePalette& other)
  {
    fog_api.imagepalette_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const ImagePalette& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ImagePalette& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const ImagePalette& empty()
  {
    return *fog_api.imagepalette_oEmpty;
  }

  // --------------------------------------------------------------------------
  // [Statics - Construction]
  // --------------------------------------------------------------------------

  static FOG_INLINE ImagePalette fromGreyscale(uint32_t length)
  {
    return ImagePalette(fog_api.imagepalette_dCreateGreyscale(length));
  }

  static FOG_INLINE ImagePalette fromColorCube(uint32_t r, uint32_t g, uint32_t b)
  {
    return ImagePalette(fog_api.imagepalette_dCreateColorCube(r, g, b));
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const ImagePalette* a, const ImagePalette* b)
  {
    return fog_api.imagepalette_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.imagepalette_eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Helpers]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool isGreyscale(const Argb32* data, size_t length)
  {
    return fog_api.imagepalette_isGreyscale(data, length);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ImagePaletteData);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEPALETTE_H
