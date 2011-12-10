// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_RECT_H
#define _FOG_G2D_GEOMETRY_RECT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Size.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::RectI]
// ============================================================================

//! @brief Rectangle (32-bit integer version).
struct FOG_NO_EXPORT RectI
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI() {}
  FOG_INLINE RectI(const RectI& other) { setRect(other); }
  FOG_INLINE RectI(const PointI& pt0, const SizeI& sz) { setRect(pt0, sz); }
  FOG_INLINE RectI(const PointI& pt0, const PointI& pt1) { setBox(pt0, pt1); }
  FOG_INLINE RectI(int px, int py, int pw, int ph) { setRect(px, py, pw, ph); }

  explicit FOG_INLINE RectI(_Uninitialized) {}
  explicit FOG_INLINE RectI(const BoxI& box) { setBox(box); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE int getX() const { return x; }
  FOG_INLINE int getY() const { return y; }
  FOG_INLINE int getWidth() const { return w; }
  FOG_INLINE int getHeight() const { return h; }

  FOG_INLINE int getX0() const { return x; }
  FOG_INLINE int getY0() const { return y; }
  FOG_INLINE int getX1() const { return x + w; }
  FOG_INLINE int getY1() const { return y + h; }

  FOG_INLINE int getLeft() const { return x; }
  FOG_INLINE int getTop() const { return y; }
  FOG_INLINE int getRight() const { return x + w; }
  FOG_INLINE int getBottom() const { return y + h; }

  FOG_INLINE const PointI& getPosition() const { return *(const PointI *)(const void*)(&x); }
  FOG_INLINE const SizeI& getSize() const { return *(const SizeI *)(const void*)(&w); }

  FOG_INLINE RectI& setRect(const RectI& other)
  {
    MemOps::copy_t<RectI>(this, &other);
    return *this;
  }

  FOG_INLINE RectI& setRect(const PointI& pt0, const SizeI& sz)
  {
    x = pt0.x;
    y = pt0.y;
    w = sz.w;
    h = sz.h;
    return *this;
  }

  FOG_INLINE RectI& setRect(int px, int py, int pw, int ph)
  {
    x = px;
    y = py;
    w = pw;
    h = ph;
    return *this;
  }

  FOG_INLINE RectI& setBox(const BoxI& box);

  FOG_INLINE RectI& setBox(const PointI& pt0, const PointI& pt1)
  {
    x = pt0.x;
    y = pt0.y;
    w = pt1.x - pt0.x;
    h = pt1.y - pt0.y;
    return *this;
  }

  FOG_INLINE RectI& setBox(int px0, int py0, int px1, int py1)
  {
    x = px0;
    y = py0;
    w = px1 - px0;
    h = py1 - py0;
    return *this;
  }

  FOG_INLINE RectI& setX(int rx) { x = rx; return *this; }
  FOG_INLINE RectI& setY(int ry) { y = ry; return *this; }
  FOG_INLINE RectI& setWidth(int rw) { w = rw; return *this; }
  FOG_INLINE RectI& setHeight(int rh) { h = rh; return *this; }

  FOG_INLINE RectI& setX0(int x0) { x = x0; return *this; }
  FOG_INLINE RectI& setY0(int y0) { y = y0; return *this; }
  FOG_INLINE RectI& setX1(int x1) { w = x1 - x; return *this; }
  FOG_INLINE RectI& setY1(int y1) { h = y1 - y; return *this; }

  FOG_INLINE RectI& setLeft(int x0) { x = x0; return *this; }
  FOG_INLINE RectI& setTop(int y0) { y = y0; return *this; }
  FOG_INLINE RectI& setRight(int x1) { w = x1 - x; return *this; }
  FOG_INLINE RectI& setBottom(int y1) { h = y1 - y; return *this; }

  // --------------------------------------------------------------------------
  // [REset]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI& reset()
  {
    MemOps::zero_t<RectI>(this);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI& translate(int rx, int ry) { x += rx; y += ry; return *this; }
  FOG_INLINE RectI& translate(const PointI& p) { x += p.x; y += p.y; return *this; }

  FOG_INLINE RectI translated(int rx, int ry) const { return RectI(x + rx, y + ry, w, h); }
  FOG_INLINE RectI translated(const PointI& p) const { return RectI(x + p.x, y + p.y, w, h); }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap. Remember, x1 and y1 coords aren't in the rectangle.
  FOG_INLINE bool overlaps(const RectI& r) const
  {
    return (( ((getY0() - r.getY1()) ^ (getY1() - r.getY0())) &
              ((getX0() - r.getX1()) ^ (getX1() - r.getX0())) ) < 0);
  }

  //! @brief Get whether rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const RectI& r) const
  {
    return ((r.getX0() >= getX0()) & (r.getX1() <= getX1()) &
            (r.getY0() >= getY0()) & (r.getY1() <= getY1()) );
  }

  static FOG_INLINE bool intersect(RectI& dest, const RectI& src1, const RectI& src2)
  {
    int xx = Math::max(src1.getX0(), src2.getX0());
    int yy = Math::max(src1.getY0(), src2.getY0());

    dest.setRect(
      xx,
      yy,
      Math::min(src1.getX1(), src2.getX1()) - xx,
      Math::min(src1.getY1(), src2.getY1()) - yy);
    return dest.isValid();
  }

  static FOG_INLINE void unite(RectI& dst, const RectI& src1, const RectI& src2)
  {
    int x0 = Math::min(src1.x, src2.x);
    int y0 = Math::min(src1.y, src2.y);
    int x1 = Math::max(src1.x + src1.w, src2.x + src2.w);
    int y1 = Math::max(src1.y + src1.h, src2.y + src2.h);

    dst.setRect(x0, y0, x1 - x0, y1 - y0);
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if given point is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool hitTest(int px, int py) const
  {
    return ((px >= getX0()) & (py >= getY0()) &
            (px <  getX1()) & (py <  getY1()) );
  }

  //! @brief Returns @c true if given point @a pt is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool hitTest(const PointI& pt) const
  {
    return ((pt.x >= getX0()) & (pt.y >= getY0()) &
            (pt.x <  getX1()) & (pt.y <  getY1()) );
  }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if rectangle is valid.
  //!
  //! Rectangle is only valid if @c x1 is greater than @c x0 and @c y1 is
  //! greater than @c y0.
  FOG_INLINE bool isValid() const
  {
    return (w > 0) & (h > 0);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(int px, int py, int pw, int ph) const
  {
    return (x == px) & (y == py) & (w == pw) & (h == ph);
  }

  FOG_INLINE bool eq(const RectI& other) const
  {
    return MemOps::eq_t<RectI>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Shrink / Expand]
  // --------------------------------------------------------------------------

  //! @brief Shrink rectangle by @c n.
  FOG_INLINE RectI& shrink(int n)
  {
    x += n;
    y += n;
    n <<= 1;
    w -= n;
    h -= n;

    return *this;
  }

  //! @brief Expand rectangle by @c n.
  FOG_INLINE RectI& expand(int n)
  {
    x -= n;
    y -= n;
    n <<= 1;
    w += n;
    h += n;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI& adjust(int px, int py)
  {
    x += px;
    y += py;
    w -= px;
    h -= py;

    w -= px;
    h -= py;

    return *this;
  }

  FOG_INLINE RectI& adjust(int px0, int py0, int px1, int py1)
  {
    x += px0;
    y += py0;
    w -= px0;
    h -= py0;

    w += px1;
    h += py1;

    return *this;
  }

  FOG_INLINE RectI adjusted(int px, int py) const
  {
    return RectI(x + px, y + py, w - px - px, h - py - py);
  }

  FOG_INLINE RectI adjusted(int px0, int py0, int px1, int py1) const
  {
    return RectI(x + px0, y + py0, w - px0 + px1, h - py0 + py1);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI& operator=(const RectI& other) { return setRect(other); }
  FOG_INLINE RectI& operator=(const BoxI& other) { return setBox(other); }

  FOG_INLINE RectI operator+(const PointI& p) const { return RectI(x + p.x, y + p.y, w, h); }
  FOG_INLINE RectI operator-(const PointI& p) const { return RectI(x - p.x, y - p.y, w, h); }

  FOG_INLINE RectI& operator+=(const PointI& p) { return translate(p); }
  FOG_INLINE RectI& operator-=(const PointI& p) { x -= p.x; y -= p.y; return *this; }

  FOG_INLINE bool operator==(const RectI& other) const { return  MemOps::eq_t<RectI>(this, &other); }
  FOG_INLINE bool operator!=(const RectI& other) const { return !MemOps::eq_t<RectI>(this, &other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int x;
  int y;
  int w;
  int h;
};

// ============================================================================
// [Fog::RectF]
// ============================================================================

//! @brief Rectangle (float).
struct FOG_NO_EXPORT RectF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF() {}

  FOG_INLINE RectF(const RectF& other) { setRect(other); }
  FOG_INLINE RectF(float px, float py, float pw, float ph) { setRect(px, py, pw, ph); }

  FOG_INLINE RectF(const PointF& pt0, const SizeF& sz) { setRect(pt0, sz); }
  FOG_INLINE RectF(const PointI& pt0, const SizeI& sz) { setRect(pt0, sz); }

  FOG_INLINE RectF(const PointF& pt0, const PointF& pt1) { setBox(pt0, pt1); }
  FOG_INLINE RectF(const PointI& pt0, const PointI& pt1) { setBox(pt0, pt1); }

  explicit FOG_INLINE RectF(_Uninitialized) {}

  explicit FOG_INLINE RectF(const RectI& other) { setRect(other); }
  explicit FOG_INLINE RectF(const RectD& other) { setRect(other); }

  explicit FOG_INLINE RectF(const BoxF& box) { setBox(box); }
  explicit FOG_INLINE RectF(const BoxI& box) { setBox(box); }
  explicit FOG_INLINE RectF(const BoxD& box) { setBox(box); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getX() const { return x; }
  FOG_INLINE float getY() const { return y; }
  FOG_INLINE float getWidth() const { return w; }
  FOG_INLINE float getHeight() const { return h; }

  FOG_INLINE float getX0() const { return x; }
  FOG_INLINE float getY0() const { return y; }
  FOG_INLINE float getX1() const { return x + w; }
  FOG_INLINE float getY1() const { return y + h; }

  FOG_INLINE float getLeft() const { return x; }
  FOG_INLINE float getTop() const { return y; }
  FOG_INLINE float getRight() const { return x + w; }
  FOG_INLINE float getBottom() const { return y + h; }

  FOG_INLINE PointD getPosition() const { return PointD(x, y); }
  FOG_INLINE SizeD getSize() const { return SizeD(w, h); }

  FOG_INLINE RectF& setRect(const RectF& other)
  {
    x = other.x;
    y = other.y;
    w = other.w;
    h = other.h;
    return *this;
  }

  FOG_INLINE RectF& setRect(const RectI& other)
  {
    x = float(other.x);
    y = float(other.y);
    w = float(other.w);
    h = float(other.h);
    return *this;
  }

  FOG_INLINE RectF& setRect(const RectD& other);

  FOG_INLINE RectF& setRect(const PointF& pt0, const SizeF& sz)
  {
    x = pt0.x;
    y = pt0.y;
    w = sz.w;
    h = sz.h;
    return *this;
  }

  FOG_INLINE RectF& setRect(const PointI& pt0, const SizeI& sz)
  {
    x = float(pt0.x);
    y = float(pt0.y);
    w = float(sz.w);
    h = float(sz.h);
    return *this;
  }

  FOG_INLINE RectF& setRect(float px, float py, float pw, float ph)
  {
    x = px;
    y = py;
    w = pw;
    h = ph;
    return *this;
  }

  FOG_INLINE RectF& setBox(const BoxF& other);
  FOG_INLINE RectF& setBox(const BoxI& other);
  FOG_INLINE RectF& setBox(const BoxD& other);

  FOG_INLINE RectF& setBox(const PointF& pt0, const PointF& pt1)
  {
    x = pt0.x;
    y = pt0.y;
    w = pt1.x - pt0.x;
    h = pt1.y - pt0.y;
    return *this;
  }

  FOG_INLINE RectF& setBox(const PointI& pt0, const PointI& pt1)
  {
    x = float(pt0.x);
    y = float(pt0.y);
    w = float(pt1.x - pt0.x);
    h = float(pt1.y - pt0.y);
    return *this;
  }

  FOG_INLINE RectF& setBox(const PointD& pt0, const PointD& pt1)
  {
    x = float(pt0.x);
    y = float(pt0.y);
    w = float(pt1.x - pt0.x);
    h = float(pt1.y - pt0.y);
    return *this;
  }

  FOG_INLINE RectF& setBox(float px0, float py0, float px1, float py1)
  {
    x = px0;
    y = py0;
    w = px1 - px0;
    h = py1 - py0;
    return *this;
  }

  FOG_INLINE RectF& setX(float x) { this->x = x; return *this; }
  FOG_INLINE RectF& setY(float y) { this->y = y; return *this; }
  FOG_INLINE RectF& setWidth(float w) { this->w = w; return *this; }
  FOG_INLINE RectF& setHeight(float h) { this->h = h; return *this; }

  FOG_INLINE RectF& setX0(float x0) { x = x0; return *this; }
  FOG_INLINE RectF& setY0(float y0) { y = y0; return *this; }
  FOG_INLINE RectF& setX1(float x1) { w = x1 - x; return *this; }
  FOG_INLINE RectF& setY1(float y1) { h = y1 - y; return *this; }

  FOG_INLINE RectF& setLeft(float x0) { x = x0; return *this; }
  FOG_INLINE RectF& setTop(float y0) { y = y0; return *this; }
  FOG_INLINE RectF& setRight(float x1) { w = x1 - x; return *this; }
  FOG_INLINE RectF& setBottom(float y1) { h = y1 - y; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF& reset()
  {
    x = 0.0f;
    y = 0.0f;
    w = 0.0f;
    h = 0.0f;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF& translate(const PointI& pt)
  {
    x += (float)pt.x;
    y += (float)pt.y;
    return *this;
  }

  FOG_INLINE RectF& translate(int px, int py)
  {
    x += (float)px;
    y += (float)py;
    return *this;
  }

  FOG_INLINE RectF& translate(const PointF& pt)
  {
    x += pt.x;
    y += pt.y;
    return *this;
  }

  FOG_INLINE RectF& translate(float px, float py)
  {
    x += px;
    y += py;
    return *this;
  }

  FOG_INLINE RectF translated(const PointI& pt) const
  {
    return RectF(x + (float)pt.x, y + (float)pt.y, w, h);
  }

  FOG_INLINE RectF translated(int px, int py) const
  {
    return RectF(x + (float)px, y + (float)py, w, h);
  }

  FOG_INLINE RectF translated(const PointF& pt) const
  {
    return RectF(x + (float)pt.x, y + (float)pt.y, w, h);
  }

  FOG_INLINE RectF translated(float px, float py) const
  {
    return RectF(x + (float)px, y + (float)py, w, h);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF& adjust(const PointI& pt)
  {
    return adjust((float)pt.x, (float)pt.y);
  }

  FOG_INLINE RectF& adjust(int px, int py)
  {
    return adjust((float)px, (float)py);
  }

  FOG_INLINE RectF& adjust(const PointF& pt)
  {
    return adjust(pt.x, pt.y);
  }

  FOG_INLINE RectF& adjust(float px, float py)
  {
    x += px; y += py;
    w -= px; h -= py;
    w -= px; h -= py;

    return *this;
  }

  FOG_INLINE RectF adjusted(const PointI& pt) const
  {
    return adjusted((float)pt.x, (float)pt.y);
  }

  FOG_INLINE RectF adjusted(int px, int py) const
  {
    return adjusted((float)px, (float)py);
  }

  FOG_INLINE RectF adjusted(const PointF& pt) const
  {
    return adjusted((float)pt.x, (float)pt.y);
  }

  FOG_INLINE RectF adjusted(float px, float py) const
  {
    return RectF(x + px, y + py, w - px - px, h - py - py);
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap.
  FOG_INLINE bool overlaps(const RectF& r) const
  {
    return (Math::max(getX0(), r.getX0()) < Math::min(getX1(), r.getX1()) &&
            Math::max(getY0(), r.getY0()) < Math::min(getY1(), r.getY1()) );
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const RectF& r) const
  {
    return ((r.getX0() >= getX0()) & (r.getX1() <= getX1()) &
            (r.getY0() >= getY0()) & (r.getY1() <= getY1()) );
  }

  static FOG_INLINE bool intersect(RectF& dest, const RectF& src1, const RectF& src2)
  {
    dest.x = Math::max(src1.getX0(), src2.getX0());
    dest.y = Math::max(src1.getY0(), src2.getY0());
    dest.w = Math::min(src1.getX1(), src2.getX1()) - dest.x;
    dest.h = Math::min(src1.getY1(), src2.getY1()) - dest.y;

    return dest.isValid();
  }

  static FOG_INLINE void unite(RectF& dest, const RectF& src1, const RectF& src2)
  {
    dest.x = Math::min(src1.getX0(), src2.getX0());
    dest.y = Math::min(src1.getY0(), src2.getY0());
    dest.w = Math::max(src1.getX1(), src2.getX1()) - dest.x;
    dest.h = Math::max(src1.getY1(), src2.getY1()) - dest.y;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Get whether the point is in the rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool hitTest(float px, float py) const
  {
    float ix = px - x;
    float iy = py - y;

    return ((ix >= 0.0f) & (ix < w) & (iy > 0.0f) & (iy < h));
  }

  //! @brief Get whether the point is in the rectangle.
  //! @brief pt Point coordinates.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool hitTest(const PointF& other) const
  {
    float ix = other.x - x;
    float iy = other.y - y;

    return ((ix >= 0.0f) & (ix < w) & (iy >= 0.0f) & (iy < h));
  }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  //! @brief Get whether the rectangle is valid.
  //!
  //! Rectangle is only valid if it's width and height is equal or larger
  //! than zero (it means that width and height is not negative).
  FOG_INLINE bool isValid() const
  {
    return ((w > 0.0f) & (h > 0.0f));
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(float px, float py, float pw, float ph) const
  {
    return (x == px) & (y == py) & (w == pw) & (h == ph);
  }

  FOG_INLINE bool eq(const RectF& other) const
  {
    return MemOps::eq_t<RectF>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF& operator=(const RectF& other) { return setRect(other); }
  FOG_INLINE RectF& operator=(const RectD& other) { return setRect(other); }
  FOG_INLINE RectF& operator=(const RectI& other) { return setRect(other); }

  FOG_INLINE RectF& operator=(const BoxF& other) { return setBox(other); }
  FOG_INLINE RectF& operator=(const BoxD& other) { return setBox(other); }
  FOG_INLINE RectF& operator=(const BoxI& other) { return setBox(other); }

  FOG_INLINE RectF operator+(const PointI& p) const { return RectF(x + (float)p.x, y + (float)p.y, w, h); }
  FOG_INLINE RectF operator+(const PointF& p) const { return RectF(x + (float)p.x, y + (float)p.y, w, h); }

  FOG_INLINE RectF operator-(const PointI& p) const { return RectF(x - (float)p.x, y - (float)p.y, w, h); }
  FOG_INLINE RectF operator-(const PointF& p) const { return RectF(x - (float)p.x, y - (float)p.y, w, h); }

  FOG_INLINE RectF& operator+=(const PointI& other) { return translate(other); }
  FOG_INLINE RectF& operator+=(const PointF& other) { return translate(other); }

  FOG_INLINE RectF& operator-=(const PointI& other) { x -= (float)other.x; y -= (float)other.y; return *this; }
  FOG_INLINE RectF& operator-=(const PointF& other) { x -= (float)other.x; y -= (float)other.y; return *this; }

  FOG_INLINE bool operator==(const RectF& other) const { return  MemOps::eq_t<RectF>(this, &other); }
  FOG_INLINE bool operator!=(const RectF& other) const { return !MemOps::eq_t<RectF>(this, &other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float x;
  float y;
  float w;
  float h;
};

// ============================================================================
// [Fog::RectD]
// ============================================================================

//! @brief Rectangle (64-bit integer version).
struct FOG_NO_EXPORT RectD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD() {}

  FOG_INLINE RectD(const RectD& other) { setRect(other); }
  FOG_INLINE RectD(double px, double py, double pw, double ph) { setRect(px, py, pw, ph); }

  FOG_INLINE RectD(const PointI& pt0, const SizeI& sz) { setRect(pt0, sz); }
  FOG_INLINE RectD(const PointF& pt0, const SizeF& sz) { setRect(pt0, sz); }
  FOG_INLINE RectD(const PointD& pt0, const SizeD& sz) { setRect(pt0, sz); }

  explicit FOG_INLINE RectD(_Uninitialized) {}

  explicit FOG_INLINE RectD(const RectF& other) { setRect(other); }
  explicit FOG_INLINE RectD(const RectI& other) { setRect(other); }

  explicit FOG_INLINE RectD(const BoxD& box) { setBox(box); }
  explicit FOG_INLINE RectD(const BoxF& box) { setBox(box); }
  explicit FOG_INLINE RectD(const BoxI& box) { setBox(box); }

  FOG_INLINE RectD(const PointD& pt0, const PointD& pt1) { setBox(pt0, pt1); }
  FOG_INLINE RectD(const PointF& pt0, const PointF& pt1) { setBox(pt0, pt1); }
  FOG_INLINE RectD(const PointI& pt0, const PointI& pt1) { setBox(pt0, pt1); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getX() const { return x; }
  FOG_INLINE double getY() const { return y; }
  FOG_INLINE double getWidth() const { return w; }
  FOG_INLINE double getHeight() const { return h; }

  FOG_INLINE double getX0() const { return x; }
  FOG_INLINE double getY0() const { return y; }
  FOG_INLINE double getX1() const { return x + w; }
  FOG_INLINE double getY1() const { return y + h; }

  FOG_INLINE double getLeft() const { return x; }
  FOG_INLINE double getTop() const { return y; }
  FOG_INLINE double getRight() const { return x + w; }
  FOG_INLINE double getBottom() const { return y + h; }

  FOG_INLINE PointD getPosition() const { return PointD(x, y); }
  FOG_INLINE SizeD getSize() const { return SizeD(w, h); }

  FOG_INLINE RectD& setRect(const RectD& other)
  {
    x = other.x;
    y = other.y;
    w = other.w;
    h = other.h;
    return *this;
  }

  FOG_INLINE RectD& setRect(const RectF& other)
  {
    x = double(other.x);
    y = double(other.y);
    w = double(other.w);
    h = double(other.h);
    return *this;
  }

  FOG_INLINE RectD& setRect(const RectI& other)
  {
    x = float(other.x);
    y = float(other.y);
    w = float(other.w);
    h = float(other.h);
    return *this;
  }

  FOG_INLINE RectD& setRect(const PointD& pt0, const SizeD& sz)
  {
    x = pt0.x;
    y = pt0.y;
    w = sz.w;
    h = sz.h;
    return *this;
  }

  FOG_INLINE RectD& setRect(const PointF& pt0, const SizeF& sz)
  {
    x = double(pt0.x);
    y = double(pt0.y);
    w = double(sz.w);
    h = double(sz.h);
    return *this;
  }

  FOG_INLINE RectD& setRect(const PointI& pt0, const SizeI& sz)
  {
    x = double(pt0.x);
    y = double(pt0.y);
    w = double(sz.w);
    h = double(sz.h);
    return *this;
  }

  FOG_INLINE RectD& setRect(double px, double py, double pw, double ph)
  {
    x = px;
    y = py;
    w = pw;
    h = ph;
    return *this;
  }

  FOG_INLINE RectD& setBox(const BoxD& other);
  FOG_INLINE RectD& setBox(const BoxF& other);
  FOG_INLINE RectD& setBox(const BoxI& other);

  FOG_INLINE RectD& setBox(const PointD& pt0, const PointD& pt1)
  {
    x = pt0.x;
    y = pt0.y;
    w = pt1.x - pt0.x;
    h = pt1.y - pt0.y;
    return *this;
  }

  FOG_INLINE RectD& setBox(const PointF& pt0, const PointF& pt1)
  {
    x = double(pt0.x);
    y = double(pt0.y);
    w = double(pt1.x - pt0.x);
    h = double(pt1.y - pt0.y);
    return *this;
  }

  FOG_INLINE RectD& setBox(const PointI& pt0, const PointI& pt1)
  {
    x = double(pt0.x);
    y = double(pt0.y);
    w = double(pt1.x - pt0.x);
    h = double(pt1.y - pt0.y);
    return *this;
  }

  FOG_INLINE RectD& setBox(double px0, double py0, double px1, double py1)
  {
    x = px0;
    y = py0;
    w = px1 - px0;
    h = py1 - py0;
    return *this;
  }

  FOG_INLINE RectD& setX(double x) { this->x = x; return *this; }
  FOG_INLINE RectD& setY(double y) { this->y = y; return *this; }
  FOG_INLINE RectD& setWidth(double w) { this->w = w; return *this; }
  FOG_INLINE RectD& setHeight(double h) { this->h = h; return *this; }

  FOG_INLINE RectD& setX0(double x0) { x = x0; return *this; }
  FOG_INLINE RectD& setY0(double y0) { y = y0; return *this; }
  FOG_INLINE RectD& setX1(double x1) { w = x1 - x; return *this; }
  FOG_INLINE RectD& setY1(double y1) { h = y1 - y; return *this; }

  FOG_INLINE RectD& setLeft(double x0) { x = x0; return *this; }
  FOG_INLINE RectD& setTop(double y0) { y = y0; return *this; }
  FOG_INLINE RectD& setRight(double x1) { w = x1 - x; return *this; }
  FOG_INLINE RectD& setBottom(double y1) { h = y1 - y; return *this; }

  // --------------------------------------------------------------------------
  // [reset]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD& reset()
  {
    x = 0.0;
    y = 0.0;
    w = 0.0;
    h = 0.0;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD& translate(const PointI& pt)
  {
    x += (double)pt.x;
    y += (double)pt.y;
    return *this;
  }

  FOG_INLINE RectD& translate(int px, int py)
  {
    x += (double)px;
    y += (double)py;
    return *this;
  }

  FOG_INLINE RectD& translate(const PointF& pt)
  {
    x += (double)pt.x;
    y += (double)pt.y;
    return *this;
  }

  FOG_INLINE RectD& translate(float px, float py)
  {
    x += (double)px;
    y += (double)py;
    return *this;
  }

  FOG_INLINE RectD& translate(const PointD& pt)
  {
    x += pt.x;
    y += pt.y;
    return *this;
  }

  FOG_INLINE RectD& translate(double px, double py)
  {
    x += px;
    y += py;
    return *this;
  }

  FOG_INLINE RectD translated(const PointI& pt) const
  {
    return RectD(x + (double)pt.x, y + (double)pt.y, w, h);
  }

  FOG_INLINE RectD translated(int px, int py) const
  {
    return RectD(x + (double)px, y + (double)py, w, h);
  }

  FOG_INLINE RectD translated(const PointF& pt) const
  {
    return RectD(x + (double)pt.x, y + (double)pt.y, w, h);
  }

  FOG_INLINE RectD translated(float px, float py) const
  {
    return RectD(x + (double)px, y + (double)py, w, h);
  }

  FOG_INLINE RectD translated(const PointD& pt) const
  {
    return RectD(x + pt.x, y + pt.y, w, h);
  }

  FOG_INLINE RectD translated(double px, double py) const
  {
    return RectD(x + px, y + py, w, h);
  }

  // --------------------------------------------------------------------------
  // [Adjust]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD& adjust(const PointI& pt)
  {
    return adjust((double)pt.x, (double)pt.y);
  }

  FOG_INLINE RectD& adjust(int px, int py)
  {
    return adjust((double)px, (double)py);
  }

  FOG_INLINE RectD& adjust(const PointF& pt)
  {
    return adjust((double)pt.x, (double)pt.y);
  }

  FOG_INLINE RectD& adjust(float px, float py)
  {
    return adjust((double)px, (double)py);
  }

  FOG_INLINE RectD& adjust(const PointD& pt)
  {
    return adjust(pt.x, pt.y);
  }

  FOG_INLINE RectD& adjust(double px, double py)
  {
    x += px; y += py;
    w -= px; h -= py;
    w -= px; h -= py;

    return *this;
  }

  FOG_INLINE RectD adjusted(const PointI& pt) const
  {
    return adjusted((double)pt.x, (double)pt.y);
  }

  FOG_INLINE RectD adjusted(int px, int py) const
  {
    return adjusted((double)px, (double)py);
  }

  FOG_INLINE RectD adjusted(const PointF& pt) const
  {
    return adjusted((double)pt.x, (double)pt.y);
  }

  FOG_INLINE RectD adjusted(float px, float py) const
  {
    return adjusted((double)px, (double)py);
  }

  FOG_INLINE RectD adjusted(const PointD& pt) const
  {
    return adjusted(pt.x, pt.y);
  }

  FOG_INLINE RectD adjusted(double px, double py) const
  {
    return RectD(x + px, y + py, w - px - px, h - py - py);
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap.
  FOG_INLINE bool overlaps(const RectD& r) const
  {
    return (Math::max(getX0(), r.getX0()) < Math::min(getX1(), r.getX1()) &&
            Math::max(getY0(), r.getY0()) < Math::min(getY1(), r.getY1()) );
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const RectD& r) const
  {
    return ((r.getX0() >= getX0()) & (r.getX1() <= getX1()) &
            (r.getY0() >= getY0()) & (r.getY1() <= getY1()) );
  }

  static FOG_INLINE bool intersect(RectD& dest, const RectD& src1, const RectD& src2)
  {
    dest.x = Math::max(src1.getX0(), src2.getX0());
    dest.y = Math::max(src1.getY0(), src2.getY0());
    dest.w = Math::min(src1.getX1(), src2.getX1()) - dest.x;
    dest.h = Math::min(src1.getY1(), src2.getY1()) - dest.y;

    return dest.isValid();
  }

  static FOG_INLINE void unite(RectD& dest, const RectD& src1, const RectD& src2)
  {
    dest.x = Math::min(src1.getX0(), src2.getX0());
    dest.y = Math::min(src1.getY0(), src2.getY0());
    dest.w = Math::max(src1.getX1(), src2.getX1()) - dest.x;
    dest.h = Math::max(src1.getY1(), src2.getY1()) - dest.y;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Get whether the point is in the rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool hitTest(double px, double py) const
  {
    double ix = px - x;
    double iy = py - y;

    return ((ix >= 0) & (ix < w) & (iy > 0) & (iy < h));
  }

  //! @brief Get whether the point is in the rectangle.
  //! @brief pt Point coordinates.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool hitTest(const PointD& other) const
  {
    double ix = other.x - x;
    double iy = other.y - y;

    return ((ix >= 0) & (ix < w) & (iy >= 0) & (iy < h));
  }

  // --------------------------------------------------------------------------
  // [Consistency]
  // --------------------------------------------------------------------------

  //! @brief Get whether the rectangle is valid.
  //!
  //! Rectangle is only valid if it's width and height is equal or larger
  //! than zero (it means that width and height is not negative).
  FOG_INLINE bool isValid() const
  {
    return ((w > 0.0) & (h > 0.0));
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(double px, double py, double pw, double ph) const
  {
    return (x == px) & (y == py) & (w == pw) & (h == ph);
  }

  FOG_INLINE bool eq(const RectD& other) const
  {
    return MemOps::eq_t<RectD>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD& operator=(const RectD& other) { return setRect(other); }
  FOG_INLINE RectD& operator=(const RectF& other) { return setRect(other); }
  FOG_INLINE RectD& operator=(const RectI& other) { return setRect(other); }

  FOG_INLINE RectD& operator=(const BoxD& other) { return setBox(other); }
  FOG_INLINE RectD& operator=(const BoxF& other) { return setBox(other); }
  FOG_INLINE RectD& operator=(const BoxI& other) { return setBox(other); }

  FOG_INLINE RectD operator+(const PointD& p) const { return RectD(x + (double)p.x, y + (double)p.y, w, h); }
  FOG_INLINE RectD operator+(const PointF& p) const { return RectD(x + (double)p.x, y + (double)p.y, w, h); }
  FOG_INLINE RectD operator+(const PointI& p) const { return RectD(x + (double)p.x, y + (double)p.y, w, h); }

  FOG_INLINE RectD operator-(const PointD& p) const { return RectD(x - (double)p.x, y - (double)p.y, w, h); }
  FOG_INLINE RectD operator-(const PointF& p) const { return RectD(x - (double)p.x, y - (double)p.y, w, h); }
  FOG_INLINE RectD operator-(const PointI& p) const { return RectD(x - (double)p.x, y - (double)p.y, w, h); }

  FOG_INLINE RectD& operator+=(const PointD& other) { return translate(other); }
  FOG_INLINE RectD& operator+=(const PointF& other) { return translate(other); }
  FOG_INLINE RectD& operator+=(const PointI& other) { return translate(other); }

  FOG_INLINE RectD& operator-=(const PointD& other) { x -= (double)other.x; y -= (double)other.y; return *this; }
  FOG_INLINE RectD& operator-=(const PointF& other) { x -= (double)other.x; y -= (double)other.y; return *this; }
  FOG_INLINE RectD& operator-=(const PointI& other) { x -= (double)other.x; y -= (double)other.y; return *this; }

  FOG_INLINE bool operator==(const RectD& other) const { return  MemOps::eq_t<RectD>(this, &other); }
  FOG_INLINE bool operator!=(const RectD& other) const { return !MemOps::eq_t<RectD>(this, &other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double x;
  double y;
  double w;
  double h;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE RectF& RectF::setRect(const RectD& other)
{
  x = float(other.x);
  y = float(other.y);
  w = float(other.w);
  h = float(other.h);
  return *this;
}

// ============================================================================
// [Fog::RectT<>]
// ============================================================================

_FOG_NUM_T(Rect)
_FOG_NUM_I(Rect)
_FOG_NUM_F(Rect)
_FOG_NUM_D(Rect)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F_VEC(Fog::RectF, 4)
FOG_FUZZY_DECLARE_D_VEC(Fog::RectD, 4)

// Prevent the warning about the used functions, which weren't defined (GCC).
#if !defined(_FOG_G2D_GEOMETRY_BOX_H)
#include <Fog/G2d/Geometry/Box.h>
#endif // !_FOG_G2D_GEOMETRY_BOX_H

// [Guard]
#endif // _FOG_G2D_GEOMETRY_RECT_H
