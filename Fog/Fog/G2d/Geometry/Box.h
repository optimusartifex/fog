// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_BOX_H
#define _FOG_G2D_GEOMETRY_BOX_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Memory/MemoryOps.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct BoxI;
struct BoxF;
struct BoxD;

struct RectI;
struct RectF;
struct RectD;

// ============================================================================
// [Fog::BoxI]
// ============================================================================

//! @brief Box (32-bit integer version).
struct BoxI
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxI() {}
  FOG_INLINE BoxI(_Uninitialized) {}

  FOG_INLINE BoxI(int px0, int py0, int px1, int py1) :
    x0(px0), y0(py0), x1(px1), y1(py1)
  {
  }

  FOG_INLINE BoxI(const BoxI& other) :
    x0(other.x0), y0(other.y0), x1(other.x1), y1(other.y1)
  {
  }

  explicit FOG_INLINE BoxI(const RectI& other) : 
    x0(other.x), y0(other.y), x1(other.x + other.w), y1(other.y + other.h)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE int getX() const { return x0; }
  FOG_INLINE int getY() const { return y0; }
  FOG_INLINE int getWidth() const { return x1 - x0; }
  FOG_INLINE int getHeight() const { return y1 - y0; }

  FOG_INLINE int getX0() const { return x0; }
  FOG_INLINE int getY0() const { return y0; }
  FOG_INLINE int getX1() const { return x1; }
  FOG_INLINE int getY1() const { return y1; }

  FOG_INLINE PointI getPosition() const { return PointI(x0, y0); }
  FOG_INLINE SizeI getSize() const { return SizeI(x1 - x0, y1 - y0); }

  FOG_INLINE BoxI& set(const BoxI &other)
  { 
    Memory::copy_t<BoxI>(this, &other);
    return *this;
  }
  
  FOG_INLINE BoxI& set(int px0, int py0, int px1, int py1)
  { 
    x0 = px0; 
    y0 = py0; 
    x1 = px1; 
    y1 = py1;

    return *this;
  }

  FOG_INLINE BoxI& setX(int x) { x0 = x; return *this; }
  FOG_INLINE BoxI& setY(int y) { y0 = y; return *this; }
  FOG_INLINE BoxI& setWidth(int w) { x1 = x0 + w; return *this; }
  FOG_INLINE BoxI& setHeight(int h) { y1 = y0 + h; return *this; }

  FOG_INLINE BoxI& setX0(int px0) { x0 = px0; return *this; }
  FOG_INLINE BoxI& setY0(int py0) { y0 = py0; return *this; }
  FOG_INLINE BoxI& setX1(int px1) { x1 = px1; return *this; }
  FOG_INLINE BoxI& setY1(int py1) { y1 = py1; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxI& reset()
  {
    Memory::zero_t<BoxI>(this);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxI& translate(int px, int py)
  { 
    x0 += px;
    y0 += py;
    x1 += px;
    y1 += py;
    return *this;
  }

  FOG_INLINE BoxI& translate(const PointI& p)
  { 
    x0 += p.x;
    y0 += p.y;
    x1 += p.x;
    y1 += p.y;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap. Remember, x1 and y1 coords aren't in the rectangle.
  FOG_INLINE bool overlaps(const BoxI& r) const
  {
    return (( ((y0 - r.y1) ^ (y1 - r.y0)) &
              ((x0 - r.x1) ^ (x1 - r.x0)) ) < 0);
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const BoxI& r) const
  {
    return ((r.x0 >= x0) & (r.x1 <= x1) &
            (r.y0 >= y0) & (r.y1 <= y1) );
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if given point is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(int px, int py) const
  {
    return ((px >= x0) & (py >= y0) & (px < x1) & (py < y1));
  }

  //! @brief Returns @c true if given point @a p is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(const PointI& p) const
  {
    return ((p.x >= x0) & (p.y >= y0) & (p.x < x1) & (p.y < y1));
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
    return (x1 > x0) & (y1 > y0);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(int px0, int py0, int px1, int py1) const
  {
    return (x0 == px0) & (y0 == py0) & (x1 == px1) & (y1 == py1);
  }

  FOG_INLINE bool eq(const BoxI& other) const
  {
    return Memory::eq_t<BoxI>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Shrink / Expand]
  // --------------------------------------------------------------------------

  //! @brief Shrinks rectangle by @c n coordinates.
  FOG_INLINE BoxI& shrink(int n)
  {
    x0 += n;
    y0 += n;
    x1 -= n;
    y1 -= n;
    
    return *this;
  }

  //! @brief Expands rectangle by @c n coordinates.
  FOG_INLINE BoxI& expand(int n)
  {
    x0 -= n;
    y0 -= n;
    x1 += n;
    y1 += n;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI toRectI() const { return RectI(x0, y0, x1 - x0, y1 - y0); }
  FOG_INLINE RectF toRectF() const { return RectF((float)x0, (float)y0, (float)(x1 - x0), (float)(y1 - y0)); }
  FOG_INLINE RectD toRectD() const { return RectD(x0, y0, x1 - x0, y1 - y0); }

  FOG_INLINE BoxF toBoxF() const;
  FOG_INLINE BoxD toBoxD() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxI& operator=(const BoxI& other) { return set(other); }

  FOG_INLINE BoxI operator+(const PointI& p) const { return BoxI(x0 + p.x, y0 + p.y, x1 + p.x, y1 + p.y); }
  FOG_INLINE BoxI operator-(const PointI& p) const { return BoxI(x0 - p.x, y0 - p.y, x1 - p.x, y1 - p.y); }

  FOG_INLINE BoxI& operator+=(const PointI& p) { return translate(p); }
  FOG_INLINE BoxI& operator-=(const PointI& p) { x0 -= p.x; y0 -= p.y; x1 -= p.x; y1 -= p.y; return *this; }

  FOG_INLINE bool operator==(const BoxI& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const BoxI& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool intersect(BoxI& dest, const BoxI& src1, const BoxI& src2)
  {
#if defined(FOG_HARDCODE_SSE2)
    FOG_ALIGNED_VAR(static const uint32_t, c0[4], 16) = { 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF };

    __m128i xmm0, xmm1, xmm2;

    xmm0 = _mm_loadu_si128((__m128i *)(&src1));
    xmm1 = _mm_loadu_si128((__m128i *)(&src2));

    xmm2 = _mm_cmpgt_epi32(xmm0, xmm1);
    xmm2 = _mm_xor_si128(xmm2, _mm_load_si128((const __m128i *)c0));

    xmm0 = _mm_and_si128(xmm0, xmm2);
    xmm2 = _mm_andnot_si128(xmm2, xmm1);
    xmm0 = _mm_or_si128(xmm0, xmm2);

    _mm_storeu_si128((__m128i *)(&dest), xmm0);
#else
    dest.set(Math::max(src1.x0, src2.x0), Math::max(src1.y0, src2.y0),
             Math::min(src1.x1, src2.x1), Math::min(src1.y1, src2.y1));
#endif // FOG_HARDCODE_SSE2
    return dest.isValid();
  }

  static FOG_INLINE void bound(BoxI& dest, const BoxI& src1, const BoxI& src2)
  {
    dest.set(Math::min(src1.x0, src2.x0),
             Math::min(src1.y0, src2.y0),
             Math::max(src1.x1, src2.x1),
             Math::max(src1.y1, src2.y1));
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int x0;
  int y0;
  int x1;
  int y1;
};

// ============================================================================
// [Fog::BoxF]
// ============================================================================

//! @brief Box (float).
struct BoxF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxF() {}
  FOG_INLINE BoxF(_Uninitialized) {}

  FOG_INLINE BoxF(float px0, float py0, float px1, float py1) :
    x0(px0),
    y0(py0),
    x1(px1),
    y1(py1)
  {
  }

  FOG_INLINE BoxF(const BoxF& other) :
    x0(other.x0),
    y0(other.y0),
    x1(other.x1),
    y1(other.y1)
  {
  }

  explicit FOG_INLINE BoxF(const BoxI& other) :
    x0((float)other.x0),
    y0((float)other.y0),
    x1((float)other.x1),
    y1((float)other.y1)
  {
  }

  explicit FOG_INLINE BoxF(const RectI& other) : 
    x0((float)other.x),
    y0((float)other.y),
    x1((float)(other.x + other.w)),
    y1((float)(other.y + other.h))
  {
  }

  explicit FOG_INLINE BoxF(const RectF& other) : 
    x0(other.x),
    y0(other.y),
    x1(other.x + other.w),
    y1(other.y + other.h)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getX() const { return x0; }
  FOG_INLINE float getY() const { return y0; }
  FOG_INLINE float getWidth() const { return x1 - x0; }
  FOG_INLINE float getHeight() const { return y1 - y0; }

  FOG_INLINE float getX0() const { return x0; }
  FOG_INLINE float getY0() const { return y0; }
  FOG_INLINE float getX1() const { return x1; }
  FOG_INLINE float getY1() const { return y1; }

  FOG_INLINE PointF getPosition() const { return PointF(x0, y0); }
  FOG_INLINE SizeF getSize() const { return SizeF(x1 - x0, y1 - y0); }

  FOG_INLINE BoxF& set(const BoxI& other)
  {
    x0 = (float)other.x0; y0 = (float)other.y0;
    x1 = (float)other.x1; y1 = (float)other.y1;
    return *this;
  }

  FOG_INLINE BoxF& set(const BoxF& other)
  { 
    Memory::copy_t<BoxF>(this, &other);
    return *this;
  }
  
  FOG_INLINE BoxF& set(float px0, float py0, float px1, float py1)
  { 
    x0 = px0; 
    y0 = py0; 
    x1 = px1; 
    y1 = py1;

    return *this;
  }

  FOG_INLINE BoxF& setX(float x) { x0 = x; return *this; }
  FOG_INLINE BoxF& setY(float y) { y0 = y; return *this; }
  FOG_INLINE BoxF& setWidth(float w) { x1 = x0 + w; return *this; }
  FOG_INLINE BoxF& setHeight(float h) { y1 = y0 + h; return *this; }

  FOG_INLINE BoxF& setX0(float px0) { x0 = px0; return *this; }
  FOG_INLINE BoxF& setY0(float py0) { y0 = py0; return *this; }
  FOG_INLINE BoxF& setX1(float px1) { x1 = px1; return *this; }
  FOG_INLINE BoxF& setY1(float py1) { y1 = py1; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxF& reset()
  {
    x0 = 0.0f;
    y0 = 0.0f;
    x1 = 0.0f;
    y1 = 0.0f;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxF& translate(float px, float py)
  { 
    x0 += px;
    y0 += py;
    x1 += px;
    y1 += py;
    return *this;
  }

  FOG_INLINE BoxF& translate(const PointF& p)
  { 
    x0 += p.x;
    y0 += p.y;
    x1 += p.x;
    y1 += p.y;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap. Remember, x1 and y1 coords aren't in the rectangle.
  FOG_INLINE bool overlaps(const BoxF& r) const
  {
    return ((r.x0 >= x0) | (r.x1 <= x1) |
            (r.y0 >= y0) | (r.y1 <= y1) );
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const BoxF& r) const
  {
    return ((r.x0 >= x0) & (r.x1 <= x1) &
            (r.y0 >= y0) & (r.y1 <= y1) );
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if given point is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(float px, float py) const
  {
    return ((px >= x0) & (py >= y0) & (px < x1) & (py < y1));
  }

  //! @brief Returns @c true if given point @a p is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(const PointF& p) const
  {
    return ((p.x >= x0) & (p.y >= y0) & (p.x < x1) & (p.y < y1));
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
    return (x1 > x0) & (y1 > y0);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(float px0, float py0, float px1, float py1) const
  {
    return (x0 == px0) & (y0 == py0) & (x1 == px1) & (y1 == py1);
  }

  FOG_INLINE bool eq(const BoxF& other) const
  {
    return (x0 == other.x0) & (y0 == other.y0) & (x1 == other.x1) & (y1 == other.y1);
  }

  // --------------------------------------------------------------------------
  // [Shrink / Expand]
  // --------------------------------------------------------------------------

  //! @brief Shrinks rectangle by @c n coordinates.
  FOG_INLINE BoxF& shrink(float n)
  {
    x0 += n;
    y0 += n;
    x1 -= n;
    y1 -= n;
    
    return *this;
  }

  //! @brief Expands rectangle by @c n coordinates.
  FOG_INLINE BoxF& expand(float n)
  {
    x0 -= n;
    y0 -= n;
    x1 += n;
    y1 += n;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI toRectI() const { return RectI((int)x0, (int)y0, (int)(x1 - x0), (int)(y1 - y0)); }
  FOG_INLINE RectF toRectF() const { return RectF(x0, y0, x1 - x0, y1 - y0); }
  FOG_INLINE RectD toRectD() const { return RectD(x0, y0, x1 - x0, y1 - y0); }

  FOG_INLINE BoxI toBoxI() const;
  FOG_INLINE BoxD toBoxD() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxF& operator=(const BoxI& other) { return set(other); }
  FOG_INLINE BoxF& operator=(const BoxF& other) { return set(other); }

  FOG_INLINE BoxF operator+(const PointI& p) const { return BoxF(x0 + (float)p.x, y0 + (float)p.y, x1 + (float)p.x, y1 + (float)p.y); }
  FOG_INLINE BoxF operator+(const PointF& p) const { return BoxF(x0 + (float)p.x, y0 + (float)p.y, x1 + (float)p.x, y1 + (float)p.y); }

  FOG_INLINE BoxF operator-(const PointI& p) const { return BoxF(x0 - (float)p.x, y0 - (float)p.y, x1 - (float)p.x, y1 - (float)p.y); }
  FOG_INLINE BoxF operator-(const PointF& p) const { return BoxF(x0 - (float)p.x, y0 - (float)p.y, x1 - (float)p.x, y1 - (float)p.y); }

  FOG_INLINE BoxF& operator+=(const PointI& p) { return translate(p); }
  FOG_INLINE BoxF& operator+=(const PointF& p) { return translate(p); }

  FOG_INLINE BoxF& operator-=(const PointI& p) { x0 -= (float)p.x; y0 -= (float)p.y; x1 -= (float)p.x; y1 -= (float)p.y; return *this; }
  FOG_INLINE BoxF& operator-=(const PointF& p) { x0 -= (float)p.x; y0 -= (float)p.y; x1 -= (float)p.x; y1 -= (float)p.y; return *this; }

  FOG_INLINE bool operator==(const BoxF& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const BoxF& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool intersect(BoxF& dest, const BoxF& src1, const BoxF& src2)
  {
    dest.set(Math::max(src1.x0, src2.x0),
             Math::max(src1.y0, src2.y0),
             Math::min(src1.x1, src2.x1),
             Math::min(src1.y1, src2.y1));
    return dest.isValid();
  }

  static FOG_INLINE void bound(BoxF& dest, const BoxF& src1, const BoxF& src2)
  {
    dest.set(Math::min(src1.x0, src2.x0),
             Math::min(src1.y0, src2.y0),
             Math::max(src1.x1, src2.x1),
             Math::max(src1.y1, src2.y1));
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float x0;
  float y0;
  float x1;
  float y1;
};

// ============================================================================
// [Fog::BoxD]
// ============================================================================

//! @brief Box (double).
struct FOG_NO_EXPORT BoxD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxD() {}
  FOG_INLINE BoxD(_Uninitialized) {}

  FOG_INLINE BoxD(double px0, double py0, double px1, double py1) :
    x0(px0), y0(py0), x1(px1), y1(py1)
  {
  }

  FOG_INLINE BoxD(const BoxD& other) :
    x0(other.x0), y0(other.y0), x1(other.x1), y1(other.y1)
  {
  }

  explicit FOG_INLINE BoxD(const BoxF& other) :
    x0(other.x0), y0(other.y0), x1(other.x1), y1(other.y1)
  {
  }

  explicit FOG_INLINE BoxD(const BoxI& other) :
    x0(other.x0), y0(other.y0), x1(other.x1), y1(other.y1)
  {
  }

  explicit FOG_INLINE BoxD(const RectD& other) : 
    x0(other.x), y0(other.y), x1(other.x + other.w), y1(other.y + other.h)
  {
  }

  explicit FOG_INLINE BoxD(const RectF& other) : 
    x0(other.x), y0(other.y), x1(other.x + other.w), y1(other.y + other.h)
  {
  }

  explicit FOG_INLINE BoxD(const RectI& other) : 
    x0(other.x), y0(other.y), x1(other.x + other.w), y1(other.y + other.h)
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getX() const { return x0; }
  FOG_INLINE double getY() const { return y0; }
  FOG_INLINE double getWidth() const { return x1 - x0; }
  FOG_INLINE double getHeight() const { return y1 - y0; }

  FOG_INLINE double getX0() const { return x0; }
  FOG_INLINE double getY0() const { return y0; }
  FOG_INLINE double getX1() const { return x1; }
  FOG_INLINE double getY1() const { return y1; }

  FOG_INLINE PointD getPosition() const { return PointD(x0, y0); }
  FOG_INLINE SizeD getSize() const { return SizeD(x1 - x0, y1 - y0); }

  FOG_INLINE BoxD& set(const BoxD &other)
  { 
    Memory::copy_t<BoxD>(this, &other);
    return *this;
  }
  
  FOG_INLINE BoxD& set(const BoxF &other)
  { 
    x0 = other.x0;
    y0 = other.y0;
    x1 = other.x1;
    y1 = other.y1;
    return *this;
  }

  FOG_INLINE BoxD& set(const BoxI &other)
  { 
    x0 = other.x0;
    y0 = other.y0;
    x1 = other.x1;
    y1 = other.y1;
    return *this;
  }

  FOG_INLINE BoxD& set(double px0, double py0, double px1, double py1)
  { 
    x0 = px0; 
    y0 = py0; 
    x1 = px1; 
    y1 = py1;

    return *this;
  }

  FOG_INLINE BoxD& setX(double x) { x0 = x; return *this; }
  FOG_INLINE BoxD& setY(double y) { y0 = y; return *this; }
  FOG_INLINE BoxD& setWidth(double w) { x1 = x0 + w; return *this; }
  FOG_INLINE BoxD& setHeight(double h) { y1 = y0 + h; return *this; }

  FOG_INLINE BoxD& setX0(double px0) { x0 = px0; return *this; }
  FOG_INLINE BoxD& setY0(double py0) { y0 = py0; return *this; }
  FOG_INLINE BoxD& setX1(double px1) { x1 = px1; return *this; }
  FOG_INLINE BoxD& setY1(double py1) { y1 = py1; return *this; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxD& reset()
  {
    x0 = 0.0;
    y0 = 0.0;
    x1 = 0.0;
    y1 = 0.0;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxD& translate(double px, double py)
  { 
    x0 += px;
    y0 += py;
    x1 += px;
    y1 += py;
    return *this;
  }

  FOG_INLINE BoxD& translate(const PointD& p)
  { 
    x0 += p.x;
    y0 += p.y;
    x1 += p.x;
    y1 += p.y;
    return *this;
  }

  FOG_INLINE BoxD& translate(const PointI& p)
  { 
    x0 += p.x;
    y0 += p.y;
    x1 += p.x;
    y1 += p.y;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Algebra]
  // --------------------------------------------------------------------------

  //! @brief Checks if two rectangles overlap.
  //! @return @c true if two rectangles overlap, @c false if two rectangles
  //! do not overlap. Remember, x1 and y1 coords aren't in the rectangle.
  FOG_INLINE bool overlaps(const BoxD& r) const
  {
    return ((r.x0 >= x0) | (r.x1 <= x1) |
            (r.y0 >= y0) | (r.y1 <= y1) );
  }

  //! @brief Returns @c true if rectangle completely subsumes @a r.
  FOG_INLINE bool subsumes(const BoxD& r) const
  {
    return ((r.x0 >= x0) & (r.x1 <= x1) &
            (r.y0 >= y0) & (r.y1 <= y1) );
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Returns @c true if given point is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(double px, double py) const
  {
    return ((px >= x0) & (py >= y0) & (px < x1) & (py < y1));
  }

  //! @brief Returns @c true if given point @a p is in rectangle.
  //! @brief x Point x coordinate.
  //! @brief y Point y coordinate.
  //! @return @c true if point is in rectangle.
  FOG_INLINE bool contains(const PointD& p) const
  {
    return ((p.x >= x0) & (p.y >= y0) & (p.x < x1) & (p.y < y1));
  }

  //! @overload
  FOG_INLINE bool contains(const PointF& p) const
  {
    double px = p.x;
    double py = p.y;

    return ((px >= x0) & (py >= y0) & (px < x1) & (py < y1));
  }

  //! @overload
  FOG_INLINE bool contains(const PointI& p) const
  {
    double px = p.x;
    double py = p.y;

    return ((px >= x0) & (py >= y0) & (px < x1) & (py < y1));
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
    return (x1 > x0) & (y1 > y0);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(double px0, double py0, double px1, double py1) const
  {
    return (x0 == px0) & (y0 == py0) & (x1 == px1) & (y1 == py1);
  }

  FOG_INLINE bool eq(const BoxD& other) const
  {
    return (x0 == other.x0) & (y0 == other.y0) & (x1 == other.x1) & (y1 == other.y1);
  }

  // --------------------------------------------------------------------------
  // [Shrink / Expand]
  // --------------------------------------------------------------------------

  //! @brief Shrinks rectangle by @c n coordinates.
  FOG_INLINE BoxD& shrink(double n)
  {
    x0 += n;
    y0 += n;
    x1 -= n;
    y1 -= n;
    
    return *this;
  }

  //! @brief Expands rectangle by @c n coordinates.
  FOG_INLINE BoxD& expand(double n)
  {
    x0 -= n;
    y0 -= n;
    x1 += n;
    y1 += n;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE RectI toRectI() const { return RectI((int)x0, (int)y0, (int)(x1 - x0), (int)(y1 - y0)); }
  FOG_INLINE RectF toRectF() const { return RectF((float)x0, (float)y0, (float)(x1 - x0), (float)(y1 - y0)); }
  FOG_INLINE RectD toRectD() const { return RectD(x0, y0, x1 - x0, y1 - y0); }

  FOG_INLINE BoxI toBoxI() const;
  FOG_INLINE BoxF toBoxF() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxD& operator=(const BoxI& other) { return set(other); }
  FOG_INLINE BoxD& operator=(const BoxF& other) { return set(other); }
  FOG_INLINE BoxD& operator=(const BoxD& other) { return set(other); }

  FOG_INLINE BoxD operator+(const PointI& p) const { return BoxD(x0 + (double)p.x, y0 + (double)p.y, x1 + (double)p.x, y1 + (double)p.y); }
  FOG_INLINE BoxD operator+(const PointF& p) const { return BoxD(x0 + (double)p.x, y0 + (double)p.y, x1 + (double)p.x, y1 + (double)p.y); }
  FOG_INLINE BoxD operator+(const PointD& p) const { return BoxD(x0 + (double)p.x, y0 + (double)p.y, x1 + (double)p.x, y1 + (double)p.y); }

  FOG_INLINE BoxD operator-(const PointI& p) const { return BoxD(x0 - (double)p.x, y0 - (double)p.y, x1 - (double)p.x, y1 - (double)p.y); }
  FOG_INLINE BoxD operator-(const PointF& p) const { return BoxD(x0 - (double)p.x, y0 - (double)p.y, x1 - (double)p.x, y1 - (double)p.y); }
  FOG_INLINE BoxD operator-(const PointD& p) const { return BoxD(x0 - (double)p.x, y0 - (double)p.y, x1 - (double)p.x, y1 - (double)p.y); }

  FOG_INLINE BoxD& operator+=(const PointI& p) { return translate(p); }
  FOG_INLINE BoxD& operator+=(const PointF& p) { return translate(p); }
  FOG_INLINE BoxD& operator+=(const PointD& p) { return translate(p); }

  FOG_INLINE BoxD& operator-=(const PointI& p) { x0 -= p.x; y0 -= p.y; x1 -= p.x; y1 -= p.y; return *this; }
  FOG_INLINE BoxD& operator-=(const PointF& p) { x0 -= p.x; y0 -= p.y; x1 -= p.x; y1 -= p.y; return *this; }
  FOG_INLINE BoxD& operator-=(const PointD& p) { x0 -= p.x; y0 -= p.y; x1 -= p.x; y1 -= p.y; return *this; }

  FOG_INLINE bool operator==(const BoxD& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const BoxD& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool intersect(BoxD& dest, const BoxD& src1, const BoxD& src2)
  {
    dest.set(Math::max(src1.x0, src2.x0),
             Math::max(src1.y0, src2.y0),
             Math::min(src1.x1, src2.x1),
             Math::min(src1.y1, src2.y1));
    return dest.isValid();
  }

  static FOG_INLINE void bound(BoxD& dest, const BoxD& src1, const BoxD& src2)
  {
    dest.set(Math::min(src1.x0, src2.x0),
             Math::min(src1.y0, src2.y0),
             Math::max(src1.x1, src2.x1),
             Math::max(src1.y1, src2.y1));
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double x0;
  double y0;
  double x1;
  double y1;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE RectI::RectI(const BoxI& box) : x(box.x0), y(box.y0), w(box.x1 - box.x0), h(box.y1 - box.y0) {}
FOG_INLINE RectF::RectF(const BoxF& box) : x(box.x0), y(box.y0), w(box.x1 - box.x0), h(box.y1 - box.y0) {}
FOG_INLINE RectD::RectD(const BoxD& box) : x(box.x0), y(box.y0), w(box.x1 - box.x0), h(box.y1 - box.y0) {}

FOG_INLINE BoxF BoxI::toBoxF() const { return BoxF((float)x0, (float)y0, (float)x1, (float)y1); }
FOG_INLINE BoxD BoxI::toBoxD() const { return BoxD((double)x0, (double)y0, (double)x1, (double)y1); }

FOG_INLINE BoxI BoxF::toBoxI() const { return BoxI((int)x0, (int)y0, (int)x1, (int)y1); }
FOG_INLINE BoxD BoxF::toBoxD() const { return BoxD((double)x0, (double)y0, (double)x1, (double)y1); }

FOG_INLINE BoxI BoxD::toBoxI() const { return BoxI((int)x0, (int)y0, (int)x1, (int)y1); }
FOG_INLINE BoxF BoxD::toBoxF() const { return BoxF((float)x0, (float)y0, (float)x1, (float)y1); }

// ============================================================================
// [Fog::BoxT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D_I(Box)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::BoxI, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::BoxF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::BoxD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_DECLARE_FUZZY(Fog::BoxF, Math::feqv((const float *)&a, (const float *)&b, 4))
FOG_DECLARE_FUZZY(Fog::BoxD, Math::feqv((const double*)&a, (const double*)&b, 4))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_BOX_H
