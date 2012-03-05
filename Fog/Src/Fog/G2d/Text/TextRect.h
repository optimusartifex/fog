// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_TEXTRECT_H
#define _FOG_G2D_TEXT_TEXTRECT_H

// [Dependencies]
#include <Fog/G2d/Geometry/Rect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::TextRectI]
// ============================================================================

struct FOG_NO_EXPORT TextRectI : public RectI
{
  FOG_INLINE TextRectI() :
    RectI(0, 0, 0, 0),
    _alignment(NO_FLAGS)
  {
  }

  FOG_INLINE TextRectI(int rx, int ry, int rw, int rh, uint32_t alignment) :
    RectI(rx, ry, rw, rh),
    _alignment(alignment)
  {
  }

  FOG_INLINE TextRectI(const RectI& rect, uint32_t alignment) :
    RectI(rect),
    _alignment(alignment)
  {
  }

  explicit FOG_INLINE TextRectI(_Uninitialized) : RectI(UNINITIALIZED) {}

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

  FOG_INLINE TextRectI& operator=(const TextRectI& other)
  {
    MemOps::copy_t<TextRectI>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _alignment;
};

// ============================================================================
// [Fog::TextRectF]
// ============================================================================

struct FOG_NO_EXPORT TextRectF : public RectF
{
  FOG_INLINE TextRectF() :
    RectF(0.0f, 0.0f, 0.0f, 0.0f),
    _alignment(NO_FLAGS)
  {
  }

  FOG_INLINE TextRectF(float rx, float ry, float rw, float rh, uint32_t alignment) :
    RectF(rx, ry, rw, rh),
    _alignment(alignment)
  {
  }

  FOG_INLINE TextRectF(const RectF& rect, uint32_t alignment) :
    RectF(rect),
    _alignment(alignment)
  {
  }

  explicit FOG_INLINE TextRectF(_Uninitialized) : RectF(UNINITIALIZED) {}

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

  FOG_INLINE TextRectF& operator=(const TextRectF& other)
  {
    MemOps::copy_t<TextRectF>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _alignment;
};

// ============================================================================
// [Fog::TextRectD]
// ============================================================================

struct FOG_NO_EXPORT TextRectD : public RectD
{
  FOG_INLINE TextRectD() :
    RectD(0.0, 0.0, 0.0, 0.0),
    _alignment(NO_FLAGS)
  {
  }

  FOG_INLINE TextRectD(double rx, double ry, double rw, double rh, uint32_t alignment) :
    RectD(rx, ry, rw, rh),
    _alignment(alignment)
  {
  }

  FOG_INLINE TextRectD(const RectD& rect, uint32_t alignment) :
    RectD(rect),
    _alignment(alignment)
  {
  }

  explicit FOG_INLINE TextRectD(_Uninitialized) : RectD(UNINITIALIZED) {}

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

  FOG_INLINE TextRectD& operator=(const TextRectD& other)
  {
    MemOps::copy_t<TextRectD>(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _alignment;
};

// ============================================================================
// [Fog::TextRectT<>]
// ============================================================================

_FOG_NUM_T(TextRect)
_FOG_NUM_I(TextRect)
_FOG_NUM_F(TextRect)
_FOG_NUM_D(TextRect)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_TEXTRECT_H
