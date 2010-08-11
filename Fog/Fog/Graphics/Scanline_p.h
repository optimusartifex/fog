// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_SCANLINE_P_H
#define _FOG_GRAPHICS_SCANLINE_P_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/ByteSIMD_p.h>
#include <Fog/Core/MemoryAllocator_p.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Span_p.h>
#include <Fog/Graphics/RasterUtil_p.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::Scanline8]
// ============================================================================

//! @internal
//!
//! @brief Scanline container.
//!
//! @c Scanline8 is class that is able to produce list of @c Span8 instances
//! applicable as a mask when using low-level blit functions.
//!
//! Usage notes:
//!
//! 1. Before you can use scanline container you need to call @c newScanline().
//!    It ensures that all data are prepared for making spans (it calculates
//!    maximum mask width and prepares some span instances).
//!
//! 2. Build scanline using @c newSpan(), @endVSpan(), @c endCSpan(), etc...
//!
//! 3. Finalize scanline using @c endScanline().
struct FOG_HIDDEN Scanline8
{
  enum { INVALID_SPAN = 0x1FFFFFF0 };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Scanline8(uint32_t spanSize = sizeof(Span8));
  ~Scanline8();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE Span8* getSpans() const
  {
    return _spanFirst.getNext();
  }

  // --------------------------------------------------------------------------
  // [ScanLine]
  // --------------------------------------------------------------------------

  //! @brief Prepare scanline.
  //!
  //! This method is useful when rendering path where you already know bounds
  //! (this means probably all operations in raster paint engine). After you
  //! called the prepareScanline() method you can call newScanlineNoRealloc()
  //! instead of newScanline(int x1, int x2).
  FOG_INLINE err_t prepareScanline(int w)
  {
    if ((uint)w > _maskCapacity) return _prepareScanline(w);
    return ERR_OK;
  }

  //! @brief Initialize basic members, after this call you can call
  //! @c addValue() method.
  //! @param x1 - Minimum X coordinate used by this scanline, inclusive.
  //! @param x2 - Maximum X coordinate used by this scanline, exclusive.
  FOG_INLINE err_t newScanline(int x1, int x2)
  {
    FOG_ASSERT(x1 < x2);
    if ((uint)x2 - x1 > _maskCapacity) return _newScanline(x1, x2);

    // Duplicated, the same code is also in _newScanline().
    _maskCurrent = _maskData;
    _spanCurrent = &_spanFirst;
    if (_spanLast) _spanLast->setNext(_spanSaved);
    _spanLast = NULL;

    return ERR_OK;
  }

  FOG_INLINE void newScanlineNoRealloc()
  {
    _maskCurrent = _maskData;
    _spanCurrent = &_spanFirst;
    if (_spanLast) _spanLast->setNext(_spanSaved);
    _spanLast = NULL;
  }

  FOG_INLINE err_t endScanline()
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
  // [Span]
  // --------------------------------------------------------------------------

  FOG_INLINE void newSpan(int x1, uint32_t type)
  {
    Span8* span = _spanCurrent->getNext();
    if (FOG_UNLIKELY(span == NULL)) span = _growSpans();

    _spanCurrent = span;
    _spanCurrent->_x1 = x1;
    _spanCurrent->_type = type;
  }

  FOG_INLINE void newVSpanAlpha(int x1)
  {
    // Try to join more spans if possible.
    if (_spanCurrent->getX2() == x1 && _spanCurrent->isVMask()) return;

    newSpan(x1, SPAN_TYPE_VMASK_A_DENSE);
    _spanCurrent->setMaskPtr(_maskCurrent);
  }

  //! @brief Add single span into the current VMask span sequence.
  //! 
  //! You need to call @c newVSpan() before you can use @c addValue().
  FOG_INLINE void addValueAlpha(uint32_t m)
  {
    // Detect incorrect span type.
    FOG_ASSERT(_spanCurrent->getType() == SPAN_TYPE_VMASK_A_DENSE ||
               _spanCurrent->getType() == SPAN_TYPE_VMASK_A_SPARSE);
    // Detect buffer overrun.
    FOG_ASSERT(_maskCurrent < _maskData + _maskCapacity);

    *_maskCurrent++ = (uint8_t)m;
  }

  FOG_INLINE void endVSpanAlpha(int x2)
  {
    FOG_ASSERT(_spanCurrent->getX1() + (int)(sysint_t)(_maskCurrent - _spanCurrent->getMaskPtr()) == x2);
    FOG_ASSERT(_spanCurrent->getX1() < x2);
    _spanCurrent->setX2(x2);
  }

  FOG_INLINE uint8_t* addVSpanAlpha(int x1, int x2)
  {
    FOG_ASSERT(x1 < x2);
    newVSpanAlpha(x1);

    uint8_t* mask = _maskCurrent;
    _spanCurrent->setX2(x2);
    _maskCurrent += (x2 - x1);
    return mask;
  }

  FOG_INLINE uint8_t* addVSpanAlphaOrMergeVSpan(int x1, int x2)
  {
    FOG_ASSERT(x1 < x2);

    if (_spanCurrent->getX2() != x1 || !_spanCurrent->isVMask()) 
      newVSpanAlpha(x1);

    uint8_t* mask = _maskCurrent;
    _spanCurrent->setX2(x2);
    _maskCurrent += (x2 - x1);
    return mask;
  }

  FOG_INLINE void addVSpanAlphaAdopt(int x1, int x2, uint32_t type, const uint8_t* msk)
  {
    FOG_ASSERT(x1 < x2);

    newSpan(x1, type);
    _spanCurrent->setX2(x2);
    _spanCurrent->setMaskPtr(const_cast<uint8_t*>(msk));
  }

  FOG_INLINE void addVSpanAlphaCopyAndMul(int x1, int x2, uint32_t type, const uint8_t* mskA, uint32_t mskB)
  {
    FOG_ASSERT(x1 < x2);

    newSpan(x1, type);
    _spanCurrent->setX2(x2);
    _spanCurrent->setMaskPtr(const_cast<uint8_t*>(_maskCurrent));

    sysuint_t w = (uint)(x2 - x1);
    do {
      _maskCurrent[0] = ByteSIMD::u32MulDiv255(mskA[0], mskB);
      _maskCurrent++;
      mskA++;
    } while (--w);
  }

  FOG_INLINE void addCSpan(int x1, int x2, uint32_t m)
  {
    FOG_ASSERT(x1 < x2);
    // Try to join more spans if possible.
    if (_spanCurrent->getMaskPtr() == (uint8_t*)m && _spanCurrent->getX2() == x1)
    {
      _spanCurrent->setX2(x2);
    }
    else
    {
      newSpan(x1, SPAN_TYPE_CMASK);
      _spanCurrent->setX2(x2);
      _spanCurrent->setCMask(m);
    }
  }

  FOG_INLINE void addCSpanOrMergeVSpan(int x1, int x2, uint32_t m)
  {
    FOG_ASSERT(x1 < x2);
    uint len = (uint)(x2 - x1);

    if (len < SPAN_CMASK_LENGTH_THRESHOLD)
    {
      // If length is smaller than SPAN_CMASK_LENGTH_THRESHOLD then we generate VMask.
      newVSpanAlpha(x1);

      // Following code works only if this constant is equal or less than 4. I
      // provided also variant for 8 bytes, but I think that 8 bytes is overkill.
      m = RasterUtil::extendMask8(m);
      if (SPAN_CMASK_LENGTH_THRESHOLD <= 4)
      {
        ((uint32_t*)_maskCurrent)[0] = m;
      }
      else
      {
        ((uint32_t*)_maskCurrent)[0] = m;
        ((uint32_t*)_maskCurrent)[1] = m;
        // I hope that nobody is going to set this limit to a larger value.
        FOG_ASSERT(SPAN_CMASK_LENGTH_THRESHOLD <= 8);
      }
      _maskCurrent += len;
    }
    else
    {
      // Larger const fill.
      newSpan(x1, SPAN_TYPE_CMASK);
      _spanCurrent->setCMask(m);
    }

    // Close span.
    _spanCurrent->setX2(x2);
    FOG_ASSERT(_spanCurrent->getX1() < _spanCurrent->getX2());
  }

  // --------------------------------------------------------------------------
  // [Private methods]
  // --------------------------------------------------------------------------

private:
  //! @brief Private method that can be called by @c prepareScanline().
  err_t _prepareScanline(int w);

  //! @brief Private method that can be called by @c newScanline().
  err_t _newScanline(int x1, int x2);

  //! @brief Private method that can be called by @c newSpan().
  Span8* _growSpans();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

public:
  //! @brief Covers array, temporary array where is stored mask for current
  //! scanline.
  uint8_t* _maskData;
  //! @brief Current position in covers array.
  uint8_t* _maskCurrent;

  //! @brief Capacity of _coversData array.
  uint32_t _maskCapacity;

  //! @brief First span that is never part of returned spans (it's used 
  //! internally). Returned span by @c getSpans() is always get using
  //! <code>_spanFirst->getNext()</code>.
  Span8 _spanFirst;

  //! @brief Last span in the scanline.
  //!
  //! Last span is set-up by @c endScanline() method.
  Span8* _spanLast;

  //! @brief Saved span that will be normally after the @c _spanLast. Purpose
  //! of this variable is to keep saved chain that was break by @c endScanline()
  //! method. Calling @c _newScanline() will join this broken chain so span
  //! builder has all spans together for building the new chain.
  Span8* _spanSaved;

  //! @brief Current span, used by span-builder.
  Span8* _spanCurrent;

  //! @brief Zone memory allocator used to alloc span instances.
  ZoneMemoryAllocator _spanAllocator;
  //! @brief Size of one span, default is sizeof(Span8), may be specified
  //! at construction time.
  uint32_t _spanSize;

private:
  FOG_DISABLE_COPY(Scanline8)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_SCANLINE_P_H
