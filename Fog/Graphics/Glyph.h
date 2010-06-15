// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_GLYPH_H
#define _FOG_GRAPHICS_GLYPH_H

// [Dependencies]
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Path.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Text
//! @{

// ============================================================================
// [Fog::GlyphData]
// ============================================================================

//! @brief Glyph data.
struct FOG_API GlyphData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GlyphData();
  explicit GlyphData(const GlyphData* other);
  ~GlyphData();

  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  FOG_INLINE GlyphData* ref() const
  {
    refCount.inc();
    return const_cast<GlyphData*>(this);
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref()) delete this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;

  //! @brief Glyph bitmap data, supported formats are only A8 for now.
  Image bitmap;
  //! @brief Bitmap offset.
  IntPoint offset;

  //! @brief Begin width.
  int beginWidth;
  //! @brief End width.
  int endWidth;
  //! @brief Glyph advance.
  int advance;

private:
  FOG_DISABLE_COPY(GlyphData)
};

// ============================================================================
// [Fog::Glyph]
// ============================================================================

//! @brief Glyph.
struct FOG_API Glyph
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Glyph() : _d(_dnull->ref()) {}
  FOG_INLINE Glyph(const Glyph& other) : _d(other._d->ref()) {}

  FOG_INLINE explicit Glyph(GlyphData* d) : _d(d) {}

  FOG_INLINE ~Glyph() { _d->deref(); }

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::refCount().
  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return _d->refCount.get() == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? ERR_OK : _detach(); }

  //! @copydoc Doxygen::Implicit::_detach().
  err_t _detach();

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isNull() const { return _d == _dnull.instancep(); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const GlyphData* getData() const { return _d; }

  FOG_INLINE const Image& getBitmap() const { return _d->bitmap; }
  FOG_INLINE const IntPoint& getOffset() const { return _d->offset; }

  FOG_INLINE int getBeginWidth() const { return _d->beginWidth; }
  FOG_INLINE int getEndWidth() const { return _d->endWidth; }
  FOG_INLINE int getAdvance() const { return _d->advance; }

  err_t setBitmap(const Image& bitmap);
  err_t setOffset(const IntPoint& offset);

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  Glyph& operator=(const Glyph& other);

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Static<GlyphData> _dnull;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(GlyphData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Glyph, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_GRAPHICS_GLYPH_H
