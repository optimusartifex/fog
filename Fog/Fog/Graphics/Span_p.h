// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_SPAN_P_H
#define _FOG_GRAPHICS_SPAN_P_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Graphics/Constants_p.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Constants]
// ============================================================================

enum SPAN_CMASK_LENGTH_ENUM
{
  //! @brief Helper constant to determine how many constant pixels are
  //! profitable for pure CMask span instance. Minimum is 1, but recommended
  //! are 4 or more (it depends to available SIMD instructions per platform).
  SPAN_CMASK_LENGTH_THRESHOLD = 4
};

// ============================================================================
// [Fog::Span]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @internal
//!
//! @brief Span is small element that contains position, length, CMask (const
//! mask) or VMask (variant mask).
//!
//! Spans are always related with scanline, so information about Y position is
//! not part of the span. The const mask is encoded in mask pointer so you should
//! always check if span is CMAsk (@c isCMask() method) or vmask (@c isVMask()
//! method). There are asserts so you should be warned that you are using span
//! incorrectly.
//!
//! Note that you can base another class on @c Span to extend its functionality.
//! The core idea is that @c Span is used across the API so you don't need to
//! define more classes to work with spans.
//!
//! @sa @c Span8.
struct FOG_HIDDEN Span
{
  // --------------------------------------------------------------------------
  // [Valid]
  // --------------------------------------------------------------------------

  //! @brief Whether the span is valid (used by asserts).
  FOG_INLINE bool isValid() const { return (_x1 < _x2) && (_mask != NULL); }

  // --------------------------------------------------------------------------
  // [Position / Type]
  // --------------------------------------------------------------------------

  //! @brief Get span start position, inclusive (x1).
  FOG_INLINE int getX1() const { return (int)_x1; }
  //! @brief Get span end position, exclusive (x2).
  FOG_INLINE int getX2() const { return (int)_x2; }

  //! @brief Get span length, computed as <code>x2 - x1</code>.
  FOG_INLINE int getLength() const { return (int)(_x2 - _x1); }

  //! @brief Set span start position.
  FOG_INLINE void setX1(int x1)
  {
    FOG_ASSERT(x1 >= 0);
    _x1 = (uint)x1;
  }

  //! @brief Set span end position.
  FOG_INLINE void setX2(int x2)
  {
    FOG_ASSERT(x2 >= 0);
    _x2 = (uint)x2;
  }

  //! @brief Set span start/end position.
  FOG_INLINE void setPosition(int x1, int x2)
  {
    // Disallow invalid position.
    FOG_ASSERT(x1 >= 0);
    FOG_ASSERT(x2 >= 0);
    FOG_ASSERT(x1 < x2);

    _x1 = x1;
    _x2 = x2;
  }

  //! @brief Get span type.
  FOG_INLINE uint getType() const { return _type; }

  //! @brief Set span type.
  FOG_INLINE void setType(uint type)
  {
    FOG_ASSERT(type < SPAN_TYPE_COUNT);
    _type = type;
  }

  //! @brief Set span start/end position and type.
  FOG_INLINE void setPositionAndType(int x1, int x2, uint type)
  {
    // Disallow invalid position.
    FOG_ASSERT(x1 >= 0);
    FOG_ASSERT(x2 >= 0);
    FOG_ASSERT(x1 < x2);

    // I hope that compiler can optimize such case (setting x2 with type).
    _x1 = (uint)x1;
    _x2 = (uint)x2;
    _type = type;
  }

  // --------------------------------------------------------------------------
  // [Common Interface]
  // --------------------------------------------------------------------------

  //! @brief Get whether the span is const mask.
  FOG_INLINE bool isCMask() const
  {
    return _type < SPAN_TYPE_VMASK_START;
  }

  //! @brief Get whether span is variant mask.
  FOG_INLINE bool isVMask() const
  {
    return _type >= SPAN_TYPE_VMASK_START;
  }

  //! @brief Get mask as pointer, ignoring all CMask / VMask checks.
  //!
  //! Use this method to get mask data when you want to check later whether the
  //! type is cmask or vmask, use static methods to do the check.
  FOG_INLINE uint8_t* getMaskPtr() const
  {
    return _mask;
  }

  //! @brief Set mask pointer to @a mask. Mask can be CMask or VMask, there are
  //! not checks.
  FOG_INLINE void setMaskPtr(uint8_t* mask)
  {
    _mask = mask;
  }

  // --------------------------------------------------------------------------
  // [Next]
  // --------------------------------------------------------------------------

  //! @brief Get next span.
  FOG_INLINE Span* getNext() const { return _next; }
  //! @brief Set next span.
  FOG_INLINE void setNext(Span* next) { _next = next; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Start of span (first valid pixel, inclusive).
  uint _x1 : 29;
  //! @brief Type of span. See SPAN_TYPE.
  uint _type : 3;
  //! @brief End of span (last valid pixel, exclusive).
  uint _x2;

  //! @brief CMask value or pointer to VMask.
  //!
  //! Use CMask/VMask setters and getters to access or modify this value.
  uint8_t* _mask;

  //! @brief Pointer to the next span (or @c NULL if this is the last one).
  Span* _next;
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::Span8]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @internal
//!
//! @brief Span that is used by rendering to 8-bit image formats.
//!
//! Image formats:
//! - @c IMAGE_FORMAT_PRGB32.
//! - @c IMAGE_FORMAT_ARGB32.
//! - @c IMAGE_FORMAT_XRGB32.
//! - @c IMAGE_FORMAT_A8.
struct FOG_HIDDEN Span8 : public Span
{
  // --------------------------------------------------------------------------
  // [Next]
  // --------------------------------------------------------------------------

  //! @brief Get next span.
  FOG_INLINE Span8* getNext() const
  {
    return reinterpret_cast<Span8*>(_next);
  }

  // --------------------------------------------------------------------------
  // [CMask Interface]
  // --------------------------------------------------------------------------

  //! @brief Get const mask value.
  //!
  //! It's allowed to call this method only if @c isCMask() returns @c true.
  FOG_INLINE uint32_t getCMask() const
  {
    // Never call getCMask() on VMask span.
    FOG_ASSERT(isCMask());

    return (uint32_t)(sysuint_t)_mask;
  }

  //! @brief Set const mask value.
  FOG_INLINE void setCMask(uint32_t cmask)
  {
    // Never call setCMask() on VMask span.
    FOG_ASSERT(isCMask());

    _mask = (uint8_t*)cmask;
  }

  // --------------------------------------------------------------------------
  // [VMask Interface]
  // --------------------------------------------------------------------------

  //! @brief Get variant mask.
  //!
  //! It's allowed to call this method only if @c isVMask() returns @c true.
  FOG_INLINE uint8_t* getVMask() const
  {
    // Never call getVMask() on CMask span.
    FOG_ASSERT(isVMask());

    return _mask;
  }

  //! @brief Set variant mask pointer to @a mask.
  FOG_INLINE void setVMask(uint8_t* mask) 
  {
    // Never call setVMask() on CMask span.
    FOG_ASSERT(isVMask());

    _mask = mask;
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool isPtrCMask(const uint8_t* mask)
  {
    return (sysuint_t)mask <= 0xFF;
  }

  static FOG_INLINE bool isPtrVMask(const uint8_t* mask)
  {
    return (sysuint_t)mask > 0xFF;
  }

  static FOG_INLINE uint32_t ptrToCMask(const uint8_t* mask)
  {
    return (uint32_t)(sysuint_t)mask;
  }

  static FOG_INLINE uint8_t* cmaskToPtr(uint32_t mask)
  {
    return (uint8_t*)(void*)mask;
  }
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::SpanExt8]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
//! @internal
//!
//! @brief Span extended by a @c data pointer, based on @c Span8.
struct FOG_HIDDEN SpanExt8 : public Span8
{
  // --------------------------------------------------------------------------
  // [Next]
  // --------------------------------------------------------------------------

  //! @brief Get next span.
  FOG_INLINE SpanExt8* getNext() const
  {
    return reinterpret_cast<SpanExt8*>(_next);
  }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t* getData() const
  {
    return _data;
  }

  FOG_INLINE void setData(uint8_t* data)
  {
    _data = data;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Data pointer, used by patterns to store pointer to fetched pattern
  //! with span.
  uint8_t* _data;
};
#include <Fog/Core/Pack/PackRestore.h>

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_SPAN_P_H
