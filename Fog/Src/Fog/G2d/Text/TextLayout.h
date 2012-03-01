// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_TEXTLAYOUT_H
#define _FOG_G2D_TEXT_TEXTLAYOUT_H

// [Dependencies]
#include <Fog/G2d/Text/Font.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::TextLayoutRectI]
// ============================================================================

struct FOG_NO_EXPORT TextLayoutRectI : public RectI
{
  FOG_INLINE TextLayoutRectI() :
    RectI(0, 0, 0, 0),
    _alignment(NO_FLAGS)
  {
  }

  FOG_INLINE TextLayoutRectI(int rx, int ry, int rw, int rh, uint32_t alignment) :
    RectI(rx, ry, rw, rh),
    _alignment(alignment)
  {
  }

  FOG_INLINE TextLayoutRectI(const RectI& rect, uint32_t alignment) :
    RectI(rect),
    _alignment(alignment)
  {
  }

  explicit FOG_INLINE TextLayoutRectI(_Uninitialized) : RectI(UNINITIALIZED) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getAlignment() const { return _alignment; }
  FOG_INLINE void setAlignment(uint32_t alignment) { _alignment = alignment; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { resetRect(); resetAlignment(); }
  FOG_INLINE void resetRect() { RectI::reset(); }
  FOG_INLINE void resetAlignment() { _alignment = NO_FLAGS; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE TextLayoutRectI& operator=(const TextLayoutRectI& other)
  {
    MemOps::copy_t<TextLayoutRectI>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _alignment;
};

// ============================================================================
// [Fog::TextLayoutRectF]
// ============================================================================

struct FOG_NO_EXPORT TextLayoutRectF : public RectF
{
  FOG_INLINE TextLayoutRectF() :
    RectF(0.0f, 0.0f, 0.0f, 0.0f),
    _alignment(NO_FLAGS)
  {
  }

  FOG_INLINE TextLayoutRectF(float rx, float ry, float rw, float rh, uint32_t alignment) :
    RectF(rx, ry, rw, rh),
    _alignment(alignment)
  {
  }

  FOG_INLINE TextLayoutRectF(const RectF& rect, uint32_t alignment) :
    RectF(rect),
    _alignment(alignment)
  {
  }

  explicit FOG_INLINE TextLayoutRectF(_Uninitialized) : RectF(UNINITIALIZED) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getAlignment() const { return _alignment; }
  FOG_INLINE void setAlignment(uint32_t alignment) { _alignment = alignment; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { resetRect(); resetAlignment(); }
  FOG_INLINE void resetRect() { RectF::reset(); }
  FOG_INLINE void resetAlignment() { _alignment = NO_FLAGS; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE TextLayoutRectF& operator=(const TextLayoutRectF& other)
  {
    MemOps::copy_t<TextLayoutRectF>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _alignment;
};

// ============================================================================
// [Fog::TextLayoutRectD]
// ============================================================================

struct FOG_NO_EXPORT TextLayoutRectD : public RectD
{
  FOG_INLINE TextLayoutRectD() :
    RectD(0.0, 0.0, 0.0, 0.0),
    _alignment(NO_FLAGS)
  {
  }

  FOG_INLINE TextLayoutRectD(double rx, double ry, double rw, double rh, uint32_t alignment) :
    RectD(rx, ry, rw, rh),
    _alignment(alignment)
  {
  }

  FOG_INLINE TextLayoutRectD(const RectD& rect, uint32_t alignment) :
    RectD(rect),
    _alignment(alignment)
  {
  }

  explicit FOG_INLINE TextLayoutRectD(_Uninitialized) : RectD(UNINITIALIZED) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getAlignment() const { return _alignment; }
  FOG_INLINE void setAlignment(uint32_t alignment) { _alignment = alignment; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset() { resetRect(); resetAlignment(); }
  FOG_INLINE void resetRect() { RectD::reset(); }
  FOG_INLINE void resetAlignment() { _alignment = NO_FLAGS; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE TextLayoutRectD& operator=(const TextLayoutRectD& other)
  {
    MemOps::copy_t<TextLayoutRectD>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _alignment;
};

// ============================================================================
// [Fog::TextLayoutRectT<>]
// ============================================================================

_FOG_NUM_T(TextLayoutRect)
_FOG_NUM_I(TextLayoutRect)
_FOG_NUM_F(TextLayoutRect)
_FOG_NUM_D(TextLayoutRect)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_TEXTLAYOUT_H
