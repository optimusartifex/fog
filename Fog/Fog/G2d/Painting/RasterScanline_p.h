// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERSCANLINE_P_H
#define _FOG_G2D_PAINTING_RASTERSCANLINE_P_H

// [Dependencies]
#include <Fog/Core/Face/Face_C.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/ZoneAllocator_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Constants]
// ============================================================================

enum { RASTER_SPAN_INVALID_X = 0x1FFFFFF0 };

// ============================================================================
// [Fog::Scanline]
// ============================================================================

//! @internal
//!
//! @brief The scanline container base class.
//!
//! Usage notes:
//!
//! 1. Before you can use scanline container you need first to call @c begin().
//!    It ensures that all data are prepared for making spans (it calculates
//!    the maximum mask width and prepares some span instances).
//!
//! 2. Build scanline using @c newXXSpan(), @lnkXXSpan(), @c endXXSpan(), etc...
//!
//! 3. Finalize scanline using @c endScanline().
struct FOG_NO_EXPORT RasterScanline
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterScanline(uint32_t spanSize, uint32_t maskUnit);
  ~RasterScanline();

  // --------------------------------------------------------------------------
  // [Scanline - Prepare]
  // --------------------------------------------------------------------------

  //! @brief Prepare scanline.
  //!
  //! This method is useful when rendering path where you already know bounds
  //! (this means probably all operations in raster paint engine). After the
  //! the @c prepare() method was caleed then the @c begin() method can
  //! be used instead of the slower @c begin(int x0, int x1).
  FOG_INLINE err_t prepare(int w)
  {
    if (FOG_LIKELY((uint)w <= _maskCapacity)) return ERR_OK;
    return _prepare(w);
  }

  // --------------------------------------------------------------------------
  // [Scanline - Begin]
  // --------------------------------------------------------------------------

  FOG_INLINE void begin()
  {
    _maskCurrent = _maskData;
    _spanCurrent = &_spanFirst;
    if (_spanLast) _spanLast->setNext(_spanSaved);
    _spanLast = NULL;
  }

  //! @brief Initialize the basic members, after this call you can use @c valXX() methods.
  //! @param x0 - Minimum X coordinate used by this scanline, inclusive.
  //! @param x1 - Maximum X coordinate used by this scanline, exclusive.
  FOG_INLINE err_t begin(int x0, int x1)
  {
    FOG_ASSERT(x0 < x1);
    if (FOG_UNLIKELY((uint)x1 - x0 > _maskCapacity)) return _begin(x0, x1);

    // Duplicated, the same code is also in _begin().
    _maskCurrent = _maskData;
    _spanCurrent = &_spanFirst;
    if (_spanLast) _spanLast->setNext(_spanSaved);
    _spanLast = NULL;

    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Scanline - Close]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t close()
  {
    if (FOG_LIKELY(_spanCurrent->getNext() != _spanCurrent))
    {
      _spanSaved = _spanCurrent->getNext();
      _spanLast = _spanCurrent;
      _spanLast->setNext(NULL);
      return ERR_OK;
    }
    else
    {
      // Clear the infinite chain set by _growSpans().
      _spanCurrent->setNext(NULL);
      _spanLast = NULL;
      _spanSaved = NULL;
      return ERR_RT_OUT_OF_MEMORY;
    }
  }

  // --------------------------------------------------------------------------
  // [Scanline - Private]
  // --------------------------------------------------------------------------

  //! @brief Private method that can be called by @c prepare().
  err_t _prepare(int w);

  //! @brief Private method that can be called by @c begin().
  err_t _begin(int x0, int x1);

  //! @brief Private method that can be called by @c newSpan().
  RasterSpan* _growSpans();

  // --------------------------------------------------------------------------
  // [Span]
  // --------------------------------------------------------------------------

  FOG_INLINE void newSpan(int x0, uint type)
  {
    RasterSpan* span = _spanCurrent->getNext();
    if (FOG_UNLIKELY(span == NULL)) span = _growSpans();

    _spanCurrent = span;
    _spanCurrent->_x0 = x0;
    _spanCurrent->_type = type;
  }

  // --------------------------------------------------------------------------
  // [Members - Mask]
  // --------------------------------------------------------------------------

  //! @brief The mask data.
  uint8_t* _maskData;
  //! @brief The mask position.
  uint8_t* _maskCurrent;

  //! @brief The capacity of the _maskData.
  uint32_t _maskCapacity;
  //! @brief The maximu size of one pixel in the _maskData array.
  uint32_t _maskUnit;

  // --------------------------------------------------------------------------
  // [Members - Span]
  // --------------------------------------------------------------------------

  //! @brief Zone memory allocator used to alloc span instances.
  ZoneAllocator _spanAllocator;

  //! @brief First span that is never part of returned spans (it's used
  //! internally). Returned span by @c getSpans() is always get using
  //! <code>_spanFirst->getNext()</code>.
  RasterSpan _spanFirst;

  //! @brief Last span in the scanline.
  //!
  //! Last span is set-up by @c endScanline() method.
  RasterSpan* _spanLast;

  //! @brief Saved span that will be normally after the @c _spanLast. Purpose
  //! of this variable is to keep saved chain that was break by @c endScanline()
  //! method. Calling @c _begin() will join this broken chain so span
  //! builder has all spans together for building the new chain.
  RasterSpan* _spanSaved;

  //! @brief Current span, used by span-builder.
  RasterSpan* _spanCurrent;

  //! @brief The size of one span, sizeof(RasterSpan8) for example.
  uint32_t _spanSize;

private:
  _FOG_NO_COPY(RasterScanline)
};

// ============================================================================
// [Fog::RasterScanline8]
// ============================================================================

//! @internal
//!
//! @brief The scanline container for 8-bit per component spans.
//!
//! The @c RasterScanline8 is class that is able to produce list of the
//! @c RasterSpan8 instances applicable as a mask when using low-level render
//! functions.
struct FOG_NO_EXPORT RasterScanline8 : public RasterScanline
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterScanline8(uint32_t spanSize = sizeof(RasterSpan8)) :
    RasterScanline(spanSize, 4)
  {
  }

  FOG_INLINE ~RasterScanline8()
  {
  }

  // --------------------------------------------------------------------------
  // [Span - Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterSpan8* getSpans() const { return reinterpret_cast<RasterSpan8*>(_spanFirst.getNext()); }

  FOG_INLINE RasterSpan8* getCurrent() { return reinterpret_cast<RasterSpan8*>(_spanCurrent); }
  FOG_INLINE const RasterSpan8* getCurrent() const { return reinterpret_cast<const RasterSpan8*>(_spanCurrent); }

  // --------------------------------------------------------------------------
  // [Span - C-Mask]
  // --------------------------------------------------------------------------

  FOG_INLINE void newConstSpan(int x0, int x1, uint32_t m)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);
    // Detect an invalid mask.
    FOG_ASSERT(m <= 0x100);

    newSpan(x0, RASTER_SPAN_C);
    getCurrent()->setX1(x1);
    getCurrent()->setConstMask(m);
  }

  FOG_INLINE void lnkConstSpan(int x0, int x1, uint32_t m)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);
    // Detect an invalid mask.
    FOG_ASSERT(m <= 0x100);

    // Try to join with the current span.
    if (getCurrent()->getGenericMask() == (uint8_t*)m && getCurrent()->getX1() == x0)
      goto _Link;

    newSpan(x0, RASTER_SPAN_C);
    getCurrent()->setConstMask(m);
_Link:
    getCurrent()->setX1(x1);
  }

  FOG_INLINE void lnkConstSpanOrMerge(int x0, int x1, uint32_t m)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);

    uint len = (uint)(x1 - x0);
    if (len < RASTER_SPAN_C_THRESHOLD)
    {
      // If length is smaller than RASTER_SPAN_C_THRESHOLD then we generate A8-Extra.
      lnkA8Extra(x0);
      for (uint i = 0; i < len; i++) ((uint16_t*)_maskCurrent)[i] = (uint16_t)m;
      _maskCurrent += len * 2;
    }
    else
    {
      // Larger const fill.
      newSpan(x0, RASTER_SPAN_C);
      getCurrent()->setConstMask(m);
    }

    // Close span.
    getCurrent()->setX1(x1);
    FOG_ASSERT(_spanCurrent->getX0() < _spanCurrent->getX1());
  }

  // --------------------------------------------------------------------------
  // [Span - A8-Glyph]
  // --------------------------------------------------------------------------

  FOG_INLINE void newA8Glyph(int x0)
  {
    newSpan(x0, RASTER_SPAN_A8_GLYPH);
    getCurrent()->setGenericMask(_maskCurrent);
  }

  FOG_INLINE uint8_t* newA8Glyph_buf(int x0, int x1)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);

    uint8_t* mask = _maskCurrent;

    newA8Glyph(x0);
    getCurrent()->setX1(x1);
    _maskCurrent += (x1 - x0);

    return mask;
  }

  FOG_INLINE void newA8Glyph_adopt(int x0, int x1, const uint8_t* buf)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);

    newSpan(x0, RASTER_SPAN_A8_GLYPH);
    getCurrent()->setX1(x1);
    getCurrent()->setGenericMask(const_cast<uint8_t*>(buf));
  }

  FOG_INLINE void lnkA8Glyph(int x0)
  {
    // Try to join with the current span.
    if (getCurrent()->getX1() == x0 && getCurrent()->getType() == RASTER_SPAN_A8_GLYPH)
      return;

    newA8Glyph(x0);
  }

  FOG_INLINE uint8_t* lnkA8Glyph_buf(int x0, int x1)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);

    uint8_t* mask = _maskCurrent;

    // Try to join with the current span.
    if (getCurrent()->getX1() == x0 && getCurrent()->getType() == RASTER_SPAN_A8_GLYPH)
      goto _Link;

    newA8Glyph(x0);
_Link:
    getCurrent()->setX1(x1);
    _maskCurrent += (x1 - x0);

    return mask;
  }

  //! @brief Add a single value into the current A8-Glyph span sequence.
  //!
  //! You need to call @c newA8Glyph() or @c lnkA8Glyph() before the @c valA8Glyph() is used.
  FOG_INLINE void valA8Glyph(uint32_t m)
  {
    // Detect an invalid span.
    FOG_ASSERT(getCurrent()->getType() == RASTER_SPAN_A8_GLYPH ||
               getCurrent()->getType() == RASTER_SPAN_AX_GLYPH);
    // Detect buffer overflow.
    FOG_ASSERT(_maskCurrent < _maskData + _maskCapacity);
    // Detect an invalid mask.
    FOG_ASSERT(m < 0x100);

    *_maskCurrent++ = (uint8_t)m;
  }

  FOG_INLINE void endA8Glyph()
  {
    // Detect an invalid span.
    FOG_ASSERT(getCurrent()->getType() == RASTER_SPAN_A8_GLYPH ||
               getCurrent()->getType() == RASTER_SPAN_AX_GLYPH);

    getCurrent()->setX1(getCurrent()->getX0() + (int)(ssize_t)(_maskCurrent - (uint8_t*)getCurrent()->getGenericMask()));
    FOG_ASSERT(getCurrent()->getX0() < getCurrent()->getX1());
  }

  FOG_INLINE void endA8Glyph(int x1)
  {
    // Detect an invalid span.
    FOG_ASSERT(getCurrent()->getType() == RASTER_SPAN_A8_GLYPH ||
               getCurrent()->getType() == RASTER_SPAN_AX_GLYPH);
    // Detect an invalid range.
    FOG_ASSERT(getCurrent()->getX0() < x1);
    // Detect an invalid buffer position.
    FOG_ASSERT(getCurrent()->getX0() + (int)(ssize_t)(_maskCurrent - (uint8_t*)getCurrent()->getGenericMask()) == x1);

    getCurrent()->setX1(x1);
  }

  // --------------------------------------------------------------------------
  // [Span - A8-Extra]
  // --------------------------------------------------------------------------

  FOG_INLINE void newA8Extra(int x0)
  {
    newSpan(x0, RASTER_SPAN_AX_EXTRA);
    getCurrent()->setGenericMask(_maskCurrent);
  }

  FOG_INLINE uint16_t* newA8Extra_buf(int x0, int x1)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);

    uint16_t* mask = reinterpret_cast<uint16_t*>(_maskCurrent);

    newA8Extra(x0);
    getCurrent()->setX1(x1);
    _maskCurrent += (x1 - x0) * 2;

    return mask;
  }

  FOG_INLINE void newA8Extra_adopt(int x0, int x1, uint32_t type, const uint16_t* buf)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);

    newSpan(x0, RASTER_SPAN_AX_EXTRA);
    getCurrent()->setX1(x1);
    getCurrent()->setGenericMask(const_cast<uint16_t*>(buf));
  }

  FOG_INLINE void lnkA8Extra(int x0)
  {
    // Try to join with the current span.
    if (getCurrent()->getX1() == x0 && getCurrent()->getType() == RASTER_SPAN_AX_EXTRA)
      return;

    newA8Extra(x0);
  }

  FOG_INLINE uint16_t* lnkA8Extra_buf(int x0, int x1)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);

    uint16_t* mask = reinterpret_cast<uint16_t*>(_maskCurrent);

    // Try to join with the current span.
    if (getCurrent()->getX1() == x0 && getCurrent()->getType() == RASTER_SPAN_AX_EXTRA)
      goto _Link;

    newA8Extra(x0);
_Link:
    getCurrent()->setX1(x1);
    _maskCurrent += (x1 - x0) * 2;

    return mask;
  }

  //! @brief Add a single value into the current A8-Extra sequence.
  //!
  //! You need to call @c newA8Extra() or @c lnkA8Extra() before the @c valA8Extra() is used.
  FOG_INLINE void valA8Extra(uint32_t m)
  {
    // Detect an invalid span.
    FOG_ASSERT(getCurrent()->getType() == RASTER_SPAN_AX_EXTRA);
    // Detect buffer overflow.
    FOG_ASSERT(_maskCurrent < _maskData + _maskCapacity);
    // Detect an invalid mask.
    FOG_ASSERT(m <= 0x100);

    ((uint16_t*)_maskCurrent)[0] = (uint16_t)m;
    _maskCurrent += 2;
  }

  FOG_INLINE void endA8Extra()
  {
    // Detect an invalid span.
    FOG_ASSERT(getCurrent()->getType() == RASTER_SPAN_AX_EXTRA);

    getCurrent()->setX1(getCurrent()->getX0() + (int)(ssize_t)(_maskCurrent - (uint8_t*)getCurrent()->getGenericMask()) / 2);
    FOG_ASSERT(getCurrent()->getX0() < getCurrent()->getX1());
  }

  FOG_INLINE void endA8Extra(int x1)
  {
    // Detect an invalid span.
    FOG_ASSERT(getCurrent()->getType() == RASTER_SPAN_AX_EXTRA);
    // Detect an invalid range.
    FOG_ASSERT(getCurrent()->getX0() < x1);
    // Detect an invalid position.
    FOG_ASSERT(getCurrent()->getX0() + (int)(ssize_t)(_maskCurrent - (uint8_t*)getCurrent()->getGenericMask()) / 2 == x1);

    getCurrent()->setX1(x1);
  }

  // --------------------------------------------------------------------------
  // [Span - ARGB32-Glyph]
  // --------------------------------------------------------------------------

  FOG_INLINE void newARGB32Glyph(int x0)
  {
    newSpan(x0, RASTER_SPAN_ARGB32_GLYPH);
    getCurrent()->setGenericMask(_maskCurrent);
  }

  FOG_INLINE uint8_t* newARGB32Glyph_buf(int x0, int x1)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);

    uint8_t* mask = _maskCurrent;

    newARGB32Glyph(x0);
    getCurrent()->setX1(x1);
    _maskCurrent += (x1 - x0) * 4;

    return mask;
  }

  FOG_INLINE void newARGB32Glyph_adopt(int x0, int x1, const uint8_t* buf)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);

    newSpan(x0, RASTER_SPAN_ARGB32_GLYPH);
    getCurrent()->setX1(x1);
    getCurrent()->setGenericMask(const_cast<uint8_t*>(buf));
  }

  FOG_INLINE void lnkARGB32Glyph(int x0)
  {
    // Try to join with the current span.
    if (getCurrent()->getX1() == x0 && getCurrent()->getType() == RASTER_SPAN_ARGB32_GLYPH)
      return;

    newARGB32Glyph(x0);
  }

  FOG_INLINE uint8_t* lnkARGB32Glyph_buf(int x0, int x1)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);

    uint8_t* mask = _maskCurrent;

    // Try to join with the current span.
    if (getCurrent()->getX1() == x0 && getCurrent()->getType() == RASTER_SPAN_ARGB32_GLYPH)
      goto _Link;

    newARGB32Glyph(x0);
_Link:
    getCurrent()->setX1(x1);
    _maskCurrent += (x1 - x0) * 4;

    return mask;
  }

  FOG_INLINE void valARGB32Glyph(uint32_t m)
  {
    // Detect an invalid span.
    FOG_ASSERT(getCurrent()->getType() == RASTER_SPAN_ARGB32_GLYPH ||
               getCurrent()->getType() == RASTER_SPAN_ARGBXX_GLYPH);
    // Detect buffer overflow.
    FOG_ASSERT(_maskCurrent < _maskData + _maskCapacity);

    ((uint32_t*)_maskCurrent)[0] = m;
    _maskCurrent += 4;
  }

  FOG_INLINE void endARGB32Glyph()
  {
    // Detect an invalid span.
    FOG_ASSERT(getCurrent()->getType() == RASTER_SPAN_ARGB32_GLYPH ||
               getCurrent()->getType() == RASTER_SPAN_ARGBXX_GLYPH);

    getCurrent()->setX1(getCurrent()->getX0() + (int)(ssize_t)(_maskCurrent - (uint8_t*)getCurrent()->getGenericMask()) / 4);
    FOG_ASSERT(getCurrent()->getX0() < getCurrent()->getX1());
  }

  FOG_INLINE void endARGB32Glyph(int x1)
  {
    // Detect an invalid span.
    FOG_ASSERT(getCurrent()->getType() == RASTER_SPAN_ARGB32_GLYPH ||
               getCurrent()->getType() == RASTER_SPAN_ARGBXX_GLYPH);
    // Detect an invalid range.
    FOG_ASSERT(getCurrent()->getX0() < x1);
    // Detect an invalid buffer position.
    FOG_ASSERT(getCurrent()->getX0() + (int)(ssize_t)(_maskCurrent - (uint8_t*)getCurrent()->getGenericMask()) / 4 == x1);

    getCurrent()->setX1(x1);
  }

  // --------------------------------------------------------------------------
  // [Unsorted]
  // --------------------------------------------------------------------------

  // TODO: Refactor, better function names needed.
  /*
  FOG_INLINE void addA8SpanCopyAndMul(int x0, int x1, uint32_t type, const uint8_t* mskA, uint32_t mskB)
  {
    // Detect an invalid range.
    FOG_ASSERT(x0 < x1);

    newSpan(x0, type);
    getCurrent()->setX1(x1);
    getCurrent()->setGenericMask(const_cast<uint8_t*>(_maskCurrent));

    size_t w = (uint)(x1 - x0);

    Face::p32 mskAp;
    Face::p32 mskBp;

    Face::p32Copy(mskBp, mskB);

    do {
      Face::p32Load1b(mskAp, mskA);
      Face::p32MulDiv255SBW(mskAp, mskAp, mskBp);
      Face::p32Store1b(_maskCurrent, mskAp);

      _maskCurrent++;
      mskA++;
    } while (--w);
  }
  */

private:
  _FOG_NO_COPY(RasterScanline8)
};

// ============================================================================
// [Fog::RasterScanline16]
// ============================================================================

// TODO:

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERSCANLINE_P_H
