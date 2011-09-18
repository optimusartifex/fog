// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_COLORSTOP_H
#define _FOG_G2D_SOURCE_COLORSTOP_H

// [Dependencies]
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

// ============================================================================
// [Fog::ColorStop]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
struct FOG_NO_EXPORT ColorStop
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorStop() :
    _color(),
    _offset(0.0f)
  {
  }

  FOG_INLINE ColorStop(const ColorStop& other) :
    _color(other._color),
    _offset(other._offset)
  {
  }

  FOG_INLINE ColorStop(float offset, const Argb32& argb) :
    _color(argb),
    _offset(offset)
  {
  }

  FOG_INLINE ColorStop(float offset, const Color& color) :
    _color(color),
    _offset(offset)
  {
  }

  explicit FOG_INLINE ColorStop(_Uninitialized) : _color(UNINITIALIZED) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const Color& getColor() const { return _color; }
  FOG_INLINE Argb32 getArgb32() const { return Argb32(_color._argb32); }

  FOG_INLINE void setColor(const Color& color) { _color = color; }

  FOG_INLINE void setArgb32(const Argb32& argb32) { _color.setArgb32(argb32); }
  FOG_INLINE void setArgb64(const Argb64& argb64) { _color.setArgb64(argb64); }
  FOG_INLINE void setArgbF(const ArgbF& argbf) { _color.setArgbF(argbf); }
  FOG_INLINE void setAhsvF(const AhsvF& ahsvf) { _color.setAhsvF(ahsvf); }
  FOG_INLINE void setAhslF(const AhslF& ahslf) { _color.setAhslF(ahslf); }
  FOG_INLINE void setAcmykF(const AcmykF& acmykf) { _color.setAcmykF(acmykf); }

  FOG_INLINE float getOffset() const { return _offset; }
  FOG_INLINE void setOffset(float offset) { _offset = offset; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero_t<ColorStop>(this);
  }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    return _offset >= 0.0f && _offset <= 1.0f && _color.isValid();
  }

  FOG_INLINE void normalize()
  {
    if (_offset < 0.0f)
      _offset = 0.0f;
    else if (_offset > 1.0f)
      _offset = 1.0f;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorStop& operator=(const ColorStop& other)
  {
    MemOps::copy_t<ColorStop>(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const ColorStop& other) const { return  MemOps::eq_t<ColorStop>(this, &other); }
  FOG_INLINE bool operator!=(const ColorStop& other) const { return !MemOps::eq_t<ColorStop>(this, &other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Color _color;
  float _offset;
};
#include <Fog/Core/C++/PackRestore.h>

//! @}

} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F(Fog::ColorStop,
  Fuzzy<float>::eq(a._offset, b._offset, epsilon) &&
  Fuzzy<Color>::eq(a._color, b._color, epsilon))

// [Guard]
#endif // _FOG_G2D_SOURCE_COLORSTOP_H
