// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXTRECT_H
#define _FOG_G2D_TEXTRECT_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct TextRectI;
struct TextRectF;
struct TextRectD;

// ============================================================================
// [Fog::TextRectI]
// ============================================================================

struct FOG_NO_EXPORT TextRectI
{
  FOG_INLINE TextRectI() :
    _rect(0, 0, 0, 0),
    _alignment(NO_FLAGS)
  {
  }

  FOG_INLINE TextRectI(_Uninitialized) :
    _rect(UNINITIALIZED)
  {
  }

  FOG_INLINE TextRectI(int rx, int ry, int rw, int rh, uint32_t alignment) :
    _rect(rx, ry, rw, rh),
    _alignment(alignment)
  {
  }

  FOG_INLINE TextRectI(const RectI& rect, uint32_t alignment) :
    _rect(rect),
    _alignment(alignment)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const RectI& getRect() const { return _rect; }
  FOG_INLINE uint32_t getAlignment() const { return _alignment; }

  FOG_INLINE void setRect(const RectI& rect) { _rect = rect; }
  FOG_INLINE void setAlignment(uint32_t alignment) { _alignment = alignment; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _rect.reset();
    _alignment = NO_FLAGS;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RectI _rect;
  uint32_t _alignment;
};

// ============================================================================
// [Fog::TextRectF]
// ============================================================================

struct FOG_NO_EXPORT TextRectF
{
  FOG_INLINE TextRectF() :
    _rect(0.0f, 0.0f, 0.0f, 0.0f),
    _alignment(NO_FLAGS)
  {
  }

  FOG_INLINE TextRectF(_Uninitialized) :
    _rect(UNINITIALIZED)
  {
  }

  FOG_INLINE TextRectF(float rx, float ry, float rw, float rh, uint32_t alignment) :
    _rect(rx, ry, rw, rh),
    _alignment(alignment)
  {
  }

  FOG_INLINE TextRectF(const RectF& rect, uint32_t alignment) :
    _rect(rect),
    _alignment(alignment)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const RectF& getRect() const { return _rect; }
  FOG_INLINE uint32_t getAlignment() const { return _alignment; }

  FOG_INLINE void setRect(const RectF& rect) { _rect = rect; }
  FOG_INLINE void setAlignment(uint32_t alignment) { _alignment = alignment; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _rect.reset();
    _alignment = NO_FLAGS;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RectF _rect;
  uint32_t _alignment;
};

// ============================================================================
// [Fog::TextRectD]
// ============================================================================

struct FOG_NO_EXPORT TextRectD
{
  FOG_INLINE TextRectD() :
    _rect(0.0f, 0.0f, 0.0f, 0.0f),
    _alignment(NO_FLAGS)
  {
  }

  FOG_INLINE TextRectD(_Uninitialized) :
    _rect(UNINITIALIZED)
  {
  }

  FOG_INLINE TextRectD(double rx, double ry, double rw, double rh, uint32_t alignment) :
    _rect(rx, ry, rw, rh),
    _alignment(alignment)
  {
  }

  FOG_INLINE TextRectD(const RectD& rect, uint32_t alignment) :
    _rect(rect),
    _alignment(alignment)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const RectD& getRect() const { return _rect; }
  FOG_INLINE uint32_t getAlignment() const { return _alignment; }

  FOG_INLINE void setRect(const RectD& rect) { _rect = rect; }
  FOG_INLINE void setAlignment(uint32_t alignment) { _alignment = alignment; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _rect.reset();
    _alignment = NO_FLAGS;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RectD _rect;
  uint32_t _alignment;
};

// ============================================================================
// [Fog::TextRectT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D(TextRect)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::TextRectI, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::TextRectF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::TextRectD, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_G2D_TEXTRECT_H
