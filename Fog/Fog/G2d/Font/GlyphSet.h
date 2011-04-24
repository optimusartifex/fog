// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_FONT_GLYPHSET_H
#define _FOG_G2D_FONT_GLYPHSET_H

// [Dependencies]
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Font/Glyph.h>
#include <Fog/G2d/Geometry/Rect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::GlyphSet]
// ============================================================================

//! @brief Glyph set is used as a container where are serialized glyphs from
//! font engine.
//!
//! Glyph set is quite simple array that is optimized to play with font engines
//! and painters. It uses quite optimized way how glyphs are stored and allows
//! minimal manipulation.
//!
//! Typical font engine method that serializes glyphs looks like this:
//! @code
//! err_t serialize(GlyphSet &glyphSet)
//! {
//!   err_t err;
//!
//!   // Begin must be called to inform glyphSet that there will be added
//!   // glyphs.
//!   if ( (err = glyphSet.begin()) ) return err;
//!
//!   // Add your glyphs to glyphSet... If there is error reported by glyphSet
//!   // it will automatically finalize glyphSet, so you can't use add() again.
//!   if ( (err = glyphSet.add(...)) ) return err;
//!
//!   // Finalize by calling end() method.
//!   if ( (err = glyphSet.end()) ) return err;
//! }
//! @endcode
struct FOG_API GlyphSet
{
  // [Data]

  struct FOG_API Data
  {
    // [Flags]

    //! @brief String data flags.
    enum
    {
      //! @brief Data are created on the heap.
      //!
      //! Object is created by function like @c Fog::Memory::alloc() or by
      //! @c new operator. It this flag is not set, you can't delete object from
      //! the heap and object is probabbly only temporary (short life object).
      IsDynamic = (1U << 0),

      //! @brief Data are shareable.
      //!
      //! Object can be directly referenced by internal method @c ref().
      //! Sharable data are usually created on the heap and together
      //! with this flag is set also @c IsDynamic, but it isn't prerequisite.
      IsSharable = (1U << 1)
    };

    // [Ref / Deref]

    Data* ref() const;
    void deref();

    FOG_INLINE Data* refAlways() const
    {
      refCount.inc();
      return const_cast<Data*>(this);
    }

    // [Glyphs]

    //! @brief Get glyphs data casted to <code>Glyph*</code>.
    FOG_INLINE Glyph *glyphs() { return reinterpret_cast<Glyph*>(data); }
    //! @brief Get glyphs data casted to <code>const Glyph*</code>.
    FOG_INLINE const Glyph *glyphs() const { return reinterpret_cast<const Glyph*>(data); }

    // [Statics]

    static Data* adopt(void* data, sysuint_t capacity);
    static Data* alloc(sysuint_t capacity);
    static Data* realloc(Data* d, sysuint_t capacity);
    static Data* copy(const Data* d);
    static void free(Data* d);

    // [Members]

    //! @brief Reference count.
    mutable Atomic<sysuint_t> refCount;
    //! @brief Flags.
    uint32_t flags;
    //! @brief Capacity.
    sysuint_t capacity;
    //! @brief Length.
    sysuint_t length;
    //! @brief Extents.
    RectI extents;
    //! @brief Advance;
    int advance;
    //! @brief Glyphs data (inlined to this structure).
    char data[sizeof(Glyph)];
  };

  static Static<Data> _dnull;

  // [Construction / Destruction]

  GlyphSet();
  GlyphSet(const GlyphSet& other);

  explicit FOG_INLINE GlyphSet(Data* d) : _d(d) {}

  ~GlyphSet();

  // [Methods]

  FOG_INLINE sysuint_t getRefCount() const { return _d->refCount.get(); }
  FOG_INLINE bool isDetached() const { return getRefCount() == 1; }

  FOG_INLINE sysuint_t getCapacity() const { return _d->capacity; }
  FOG_INLINE sysuint_t getLength() const { return _d->length; }

  FOG_INLINE const RectI& getExtents() const { return _d->extents; }
  FOG_INLINE int getAdvance() const { return _d->advance; }

  FOG_INLINE const Glyph* glyphs() const { return _d->glyphs(); }

  void clear();
  void reset();

  err_t reserve(sysuint_t capacity);
  err_t grow(sysuint_t by);

  err_t begin(sysuint_t howMuch = 0);
  err_t end();

  //! @brief Add glyph to glyph set.
  //!
  //! This method can be only called within @begin() and @end().
  FOG_INLINE err_t add(GlyphData* gd)
  {
    err_t err;
    if ((_d->refCount.get() != 1 || _d->length == _d->capacity) && (err = grow(1)))
      return err;

    _add(gd);
    return ERR_OK;
  }

  //! @brief Add glyph to glyph set
  FOG_INLINE void _add(GlyphData* gd)
  {
    FOG_ASSERT(_d->length < _d->capacity);
    FOG_ASSERT(_d->refCount.get() == 1);

    _d->glyphs()[_d->length++]._d = gd;
  }

  // [Operator Overload]

  GlyphSet& operator=(const GlyphSet& other);

  // [Members]

  FOG_DECLARE_D(Data)
};

// ============================================================================
// [Fog::TemporaryGlyphSet]
// ============================================================================

template<sysuint_t N>
struct TemporaryGlyphSet : public GlyphSet
{
  // Keep 'Storage' name for this struct for Borland compiler
  struct Storage
  {
    Data _d;
    uint8_t _embedded[sizeof(Glyph)*N];
  } _storage;

  FOG_INLINE TemporaryGlyphSet() : GlyphSet(Data::adopt((void*)&_storage, N))
  {
  }

  FOG_INLINE ~TemporaryGlyphSet() {}

private:
  FOG_INLINE TemporaryGlyphSet(const GlyphSet& other) :
    GlyphSet(other._d->ref()) {}
  FOG_INLINE TemporaryGlyphSet(const TemporaryGlyphSet<N>& other) :
    GlyphSet(other._d->ref()) {}
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::GlyphSet, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_FONT_GLYPHSET_H
