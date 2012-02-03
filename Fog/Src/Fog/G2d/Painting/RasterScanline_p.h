// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERSCANLINE_P_H
#define _FOG_G2D_PAINTING_RASTERSCANLINE_P_H

// [Dependencies]
#include <Fog/Core/Face/FaceC.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemZoneAllocator.h>
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

  RasterScanline();
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
  FOG_INLINE err_t prepare(size_t maxSize)
  {
    if (FOG_UNLIKELY((uint)maxSize > _maskCapacity))
      return _prepare(maxSize);
    else
      return ERR_OK;
  }

  //! @brief Private method that can be called by @c prepare().
  err_t _prepare(size_t maxSize);

  // --------------------------------------------------------------------------
  // [Members - Mask]
  // --------------------------------------------------------------------------

  //! @brief The mask storage.
  uint8_t* _mask;
  //! @brief The capacity of the _mask.
  size_t _maskCapacity;

  // --------------------------------------------------------------------------
  // [Members - Span]
  // --------------------------------------------------------------------------

  //! @brief Fake span, used as first "invisible" span when building scanline.
  RasterSpan _fakeSpan;

  //! @brief Zone memory allocator used to alloc span instances.
  MemZoneAllocator _spanAllocator;

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

  FOG_INLINE RasterScanline8() :
    RasterScanline()
  {
  }

  FOG_INLINE ~RasterScanline8()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterSpan8* getFakeSpan() const
  {
    return (RasterSpan8*)&_fakeSpan;
  }

  // --------------------------------------------------------------------------
  // [Begin / End]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t* begin()
  {
    FOG_ASSERT(_mask != NULL);

    _spanAllocator.clear();
    return _mask;
  }

  FOG_INLINE RasterSpan8* end(RasterSpan8* currentSpan)
  {
    currentSpan->_next = NULL;
    return static_cast<RasterSpan8*>(_fakeSpan._next);
  }

  // --------------------------------------------------------------------------
  // [AllocSpan]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterSpan8* allocSpan()
  {
    return static_cast<RasterSpan8*>(
      _spanAllocator.alloc(sizeof(RasterSpan)));
  }

  FOG_INLINE RasterSpan8* allocSpans(size_t count)
  {
    return static_cast<RasterSpan8*>(
      _spanAllocator.alloc(count * sizeof(RasterSpan)));
  }

private:
  _FOG_NO_COPY(RasterScanline8)
};

// ============================================================================
// [Fog::RasterScanline16]
// ============================================================================

// TODO: 16-bit image processing.

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERSCANLINE_P_H
