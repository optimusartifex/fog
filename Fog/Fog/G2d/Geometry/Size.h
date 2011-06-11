// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_SIZE_H
#define _FOG_G2D_GEOMETRY_SIZE_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct SizeI;
struct SizeF;
struct SizeD;

// ============================================================================
// [Fog::SizeI]
// ============================================================================

//! @brief Size (32-bit integer version).
struct SizeI
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeI() {}
  FOG_INLINE SizeI(_Uninitialized) {}

  FOG_INLINE SizeI(const SizeI& other) : w(other.w), h(other.h) {}
  FOG_INLINE SizeI(int sw, int sh) : w(sw), h(sh) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE int getWidth() const { return w; }
  FOG_INLINE int getHeight() const { return h; }

  FOG_INLINE SizeI& set(const SizeI& other)
  {
    w = other.w;
    h = other.h;
    return *this;
  }

  FOG_INLINE SizeI& set(int sw, int sh)
  {
    w = sw;
    h = sh;
    return *this;
  }

  FOG_INLINE SizeI& setWidth(int sw) { w = sw; return *this; }
  FOG_INLINE SizeI& setHeight(int sh) { h = sh; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeI& reset()
  {
    w = 0;
    h = 0;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    return (w > 0) & (h > 0);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const SizeI& other) const
  {
    return (w == other.w) & (h == other.h);
  }

  FOG_INLINE bool eq(int sw, int sh) const
  {
    return (w == sw) && (h == sh);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeI& adjust(int sw, int sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE SizeI& adjust(const SizeI& sz)
  {
    w += sz.w;
    h += sz.h;
    return *this;
  }

  FOG_INLINE SizeI adjusted(int sw, int sh) const
  {
    return SizeI(w + sw, h + sh);
  }

  FOG_INLINE SizeI adjusted(const SizeI& sz) const
  {
    return SizeI(w + sz.w, h + sz.h);
  }

  // --------------------------------------------------------------------------
  // [Expand]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeI expandedTo(const SizeI& other) const
  {
    return SizeI(Math::max(w, other.w), Math::max(h, other.h));
  }

  // --------------------------------------------------------------------------
  // [Bound]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeI boundedTo(const SizeI& other) const
  {
    return SizeI(Math::min(w, other.w), Math::min(h, other.h));
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeF toSizeF() const;
  FOG_INLINE SizeD toSizeD() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeI& operator=(const SizeI& other) { return set(other); }

  FOG_INLINE SizeI operator+(const SizeI& other) const { return SizeI(w + other.w, h + other.h); }
  FOG_INLINE SizeI operator-(const SizeI& other) const { return SizeI(w - other.w, h - other.h); }

  FOG_INLINE SizeI& operator+=(const SizeI& other) { w += other.w; h += other.h; return *this; }
  FOG_INLINE SizeI& operator-=(const SizeI& other) { w -= other.w; h -= other.h; return *this; }

  FOG_INLINE bool operator==(const SizeI& other) const { return (w == other.w) & (h == other.h); }
  FOG_INLINE bool operator!=(const SizeI& other) const { return (w != other.w) | (h != other.h); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int w;
  int h;
};

// ============================================================================
// [Fog::SizeF]
// ============================================================================

struct SizeF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeF() {}
  FOG_INLINE SizeF(_Uninitialized) {}

  FOG_INLINE SizeF(const SizeI& other) : w((float)other.w), h((float)other.h) {}
  FOG_INLINE SizeF(const SizeF& other) : w(other.w), h(other.h) {}
  FOG_INLINE SizeF(int sw, int sh) : w((float)sw), h((float)sh) {}
  FOG_INLINE SizeF(float sw, float sh) : w(sw), h(sh) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getWidth() const { return w; }
  FOG_INLINE float getHeight() const { return h; }

  FOG_INLINE SizeF& set(const SizeI& other)
  {
    w = (float)other.w;
    h = (float)other.h;
    return *this;
  }

  FOG_INLINE SizeF& set(int sw, int sh)
  {
    w = (float)sw;
    h = (float)sh;
    return *this;
  }

  FOG_INLINE SizeF& set(const SizeF& other)
  {
    w = other.w;
    h = other.h;
    return *this;
  }

  FOG_INLINE SizeF& set(float sw, float sh)
  {
    w = sw;
    h = sh;
    return *this;
  }

  FOG_INLINE SizeF& setWidth(int sw) { w = (float)sw; return *this; }
  FOG_INLINE SizeF& setHeight(int sh) { h = (float)sh; return *this; }

  FOG_INLINE SizeF& setWidth(float sw) { w = sw; return *this; }
  FOG_INLINE SizeF& setHeight(float sh) { h = sh; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeF& reset()
  {
    w = 0.0f;
    h = 0.0f;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    return (w > 0.0) & (h > 0.0);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const SizeF& other) const
  {
    return (w == other.w) & (h == other.h);
  }

  FOG_INLINE bool eq(float sw, float sh) const
  {
    return (w == sw) & (h == sh);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeF& adjust(int sw, int sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE SizeF& adjust(const SizeI& sz)
  {
    w += sz.w;
    h += sz.h;
    return *this;
  }

  FOG_INLINE SizeF& adjust(float sw, float sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE SizeF& adjust(const SizeF& sz)
  {
    w += sz.w;
    h += sz.h;
    return *this;
  }

  FOG_INLINE SizeF adjusted(int sw, int sh) const
  {
    return SizeF(w + sw, h + sh);
  }

  FOG_INLINE SizeF adjusted(const SizeI& sz) const
  {
    return SizeF(w + sz.w, h + sz.h);
  }

  FOG_INLINE SizeF adjusted(float sw, float sh) const
  {
    return SizeF(w + sw, h + sh);
  }

  FOG_INLINE SizeF adjusted(const SizeF& sz) const
  {
    return SizeF(w + sz.w, h + sz.h);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeD toSizeD() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeF& operator=(const SizeI& other) { return set(other); }
  FOG_INLINE SizeF& operator=(const SizeF& other) { return set(other); }

  FOG_INLINE SizeF operator+(const SizeI& other) const { return SizeF(w + other.w, h + other.h); }
  FOG_INLINE SizeF operator+(const SizeF& other) const { return SizeF(w + other.w, h + other.h); }

  FOG_INLINE SizeF operator-(const SizeI& other) const { return SizeF(w - other.w, h - other.h); }
  FOG_INLINE SizeF operator-(const SizeF& other) const { return SizeF(w - other.w, h - other.h); }

  FOG_INLINE SizeF& operator+=(const SizeI& other) { w += other.w; h += other.h; return *this; }
  FOG_INLINE SizeF& operator+=(const SizeF& other) { w += other.w; h += other.h; return *this; }

  FOG_INLINE SizeF& operator-=(const SizeI& other) { w -= other.w; h -= other.h; return *this; }
  FOG_INLINE SizeF& operator-=(const SizeF& other) { w -= other.w; h -= other.h; return *this; }

  FOG_INLINE bool operator==(const SizeF& other) const { return (w == other.w) & (h == other.h); }
  FOG_INLINE bool operator!=(const SizeF& other) const { return (w != other.w) | (h != other.h); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float w;
  float h;
};

// ============================================================================
// [Fog::SizeD]
// ============================================================================

struct SizeD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeD() {}
  FOG_INLINE SizeD(_Uninitialized) {}

  FOG_INLINE SizeD(const SizeI& other) : w((double)other.w), h((double)other.h) {}
  FOG_INLINE SizeD(const SizeF& other) : w((double)other.w), h((double)other.h) {}
  FOG_INLINE SizeD(const SizeD& other) : w(other.w), h(other.h) {}

  FOG_INLINE SizeD(int sw, int sh) : w((double)sw), h((double)sh) {}
  FOG_INLINE SizeD(float sw, float sh) : w((double)sw), h((double)sh) {}
  FOG_INLINE SizeD(double sw, double sh) : w(sw), h(sh) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getWidth() const { return w; }
  FOG_INLINE double getHeight() const { return h; }

  FOG_INLINE SizeD& set(const SizeI& other)
  {
    w = (double)other.w;
    h = (double)other.h;
    return *this;
  }

  FOG_INLINE SizeD& set(int sw, int sh)
  {
    w = (double)sw;
    h = (double)sh;
    return *this;
  }

  FOG_INLINE SizeD& set(const SizeF& other)
  {
    w = (double)other.w;
    h = (double)other.h;
    return *this;
  }

  FOG_INLINE SizeD& set(float sw, float sh)
  {
    w = (double)sw;
    h = (double)sh;
    return *this;
  }

  FOG_INLINE SizeD& set(const SizeD& other)
  {
    w = other.w;
    h = other.h;
    return *this;
  }

  FOG_INLINE SizeD& set(double sw, double sh)
  {
    w = sw;
    h = sh;
    return *this;
  }

  FOG_INLINE SizeD& setWidth(int sw) { w = sw; return *this; }
  FOG_INLINE SizeD& setHeight(int sh) { h = sh; return *this; }

  FOG_INLINE SizeD& setWidth(float sw) { w = sw; return *this; }
  FOG_INLINE SizeD& setHeight(float sh) { h = sh; return *this; }

  FOG_INLINE SizeD& setWidth(double sw) { w = sw; return *this; }
  FOG_INLINE SizeD& setHeight(double sh) { h = sh; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeD& reset()
  {
    w = 0.0;
    h = 0.0;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    return (w > 0.0) & (h > 0.0);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const SizeD& other) const
  {
    return (w == other.w) & (h == other.h);
  }

  FOG_INLINE bool eq(double sw, double sh) const
  {
    return (w == sw) & (h == sh);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeD& adjust(int sw, int sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE SizeD& adjust(const SizeI& sz)
  {
    w += sz.w;
    h += sz.h;
    return *this;
  }

  FOG_INLINE SizeD& adjust(float sw, float sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE SizeD& adjust(const SizeF& sz)
  {
    w += sz.w;
    h += sz.h;
    return *this;
  }

  FOG_INLINE SizeD& adjust(double sw, double sh)
  {
    w += sw;
    h += sh;
    return *this;
  }

  FOG_INLINE SizeD& adjust(const SizeD& sz)
  {
    w += sz.w;
    h += sz.h;
    return *this;
  }

  FOG_INLINE SizeD adjusted(int sw, int sh) const
  {
    return SizeD(w + sw, h + sh);
  }

  FOG_INLINE SizeD adjusted(const SizeI& sz) const
  {
    return SizeD(w + sz.w, h + sz.h);
  }

  FOG_INLINE SizeD adjusted(float sw, float sh) const
  {
    return SizeD(w + sw, h + sh);
  }

  FOG_INLINE SizeD adjusted(const SizeF& sz) const
  {
    return SizeD(w + sz.w, h + sz.h);
  }

  FOG_INLINE SizeD adjusted(double sw, double sh) const
  {
    return SizeD(w + sw, h + sh);
  }

  FOG_INLINE SizeD adjusted(const SizeD& sz) const
  {
    return SizeD(w + sz.w, h + sz.h);
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeF toSizeF() const { return SizeF((float)w, (float)h); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeD& operator=(const SizeI& other) { return set(other); }
  FOG_INLINE SizeD& operator=(const SizeF& other) { return set(other); }
  FOG_INLINE SizeD& operator=(const SizeD& other) { return set(other); }

  FOG_INLINE SizeD operator+(const SizeI& other) const { return SizeD(w + other.w, h + other.h); }
  FOG_INLINE SizeD operator+(const SizeF& other) const { return SizeD(w + other.w, h + other.h); }
  FOG_INLINE SizeD operator+(const SizeD& other) const { return SizeD(w + other.w, h + other.h); }

  FOG_INLINE SizeD operator-(const SizeI& other) const { return SizeD(w - other.w, h - other.h); }
  FOG_INLINE SizeD operator-(const SizeF& other) const { return SizeD(w - other.w, h - other.h); }
  FOG_INLINE SizeD operator-(const SizeD& other) const { return SizeD(w - other.w, h - other.h); }

  FOG_INLINE SizeD& operator+=(const SizeI& other) { w += other.w; h += other.h; return *this; }
  FOG_INLINE SizeD& operator+=(const SizeF& other) { w += other.w; h += other.h; return *this; }
  FOG_INLINE SizeD& operator+=(const SizeD& other) { w += other.w; h += other.h; return *this; }

  FOG_INLINE SizeD& operator-=(const SizeI& other) { w -= other.w; h -= other.h; return *this; }
  FOG_INLINE SizeD& operator-=(const SizeF& other) { w -= other.w; h -= other.h; return *this; }
  FOG_INLINE SizeD& operator-=(const SizeD& other) { w -= other.w; h -= other.h; return *this; }

  FOG_INLINE bool operator==(const SizeD& other) const { return (w == other.w) & (h == other.h); }
  FOG_INLINE bool operator!=(const SizeD& other) const { return (w != other.w) | (h != other.h); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double w;
  double h;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE SizeF SizeI::toSizeF() const { return SizeF((float)w, (float)h); }
FOG_INLINE SizeD SizeI::toSizeD() const { return SizeD((double)w, (double)h); }
FOG_INLINE SizeD SizeF::toSizeD() const { return SizeD((double)w, (double)h); }

// ============================================================================
// [Fog::SizeT<>]
// ============================================================================

FOG_CLASS_PRECISION_F_D_I(Size)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::SizeI, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::SizeF, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::SizeD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE(Fog::SizeF, Math::isFuzzyEq(a.w, b.w) && Math::isFuzzyEq(a.h, b.h))
FOG_FUZZY_DECLARE(Fog::SizeD, Math::isFuzzyEq(a.w, b.w) && Math::isFuzzyEq(a.h, b.h))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_SIZE_H
