// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/Arc.h>
#include <Fog/G2d/Global/Init_G2d_p.h>

namespace Fog {

// ============================================================================
// [Fog::Arc - GetBoundingRect]
// ============================================================================

template<typename Number>
static void FOG_CDECL _G2d_ArcT_getBoundingRect(const typename ArcT<Number>::T* self, typename RectT<Number>::T* dst, bool includeCenterPoint)
{
  // The bounding box is calculated using unit circle (-1, -1 to 1, 1) and then
  // scaled using radius (rx, ry) and translated using center point (cx, cy).
  Number start = self->start;
  Number sweep = self->sweep;

  // Orientation is not important, let's normalize arc to start -> start + sweep
  if (sweep < Number(0.0))
  {
    start += sweep;
    sweep = -sweep;
  }

  Number rx = self->radius.x;
  Number ry = self->radius.y;

  Number minx, miny;
  Number maxx, maxy;

  if (sweep < (float)(2.0 * MATH_PI))
  {
    start = Math::mod(start, Number(2.0 * MATH_PI));
    if (start < Number(0.0)) start += Number(2.0 * MATH_PI);

    Number end = start + sweep;
    Number px, py;

    Math::sincos(start, &miny, &minx);
    maxx = minx;
    maxy = miny;

    Math::sincos(end, &py, &px);
    if (minx > px) minx = px;
    if (miny > py) miny = py; 
    if (maxx < px) maxx = px;
    if (maxy < py) maxy = py;

    // Build quadrant mask.
    Number pos = start;
    Number next = Number(0.0);

    uint32_t quadrant = 1 << 0;
    uint32_t mask = 0;

    if (start == Number(0.0)) mask = 0x01;

    if (pos < Number(MATH_PI * 2.0)) { next = Number(MATH_PI * 2.0); quadrant = 1 << 3; }
    if (pos < Number(MATH_PI * 1.5)) { next = Number(MATH_PI * 1.5); quadrant = 1 << 2; }
    if (pos < Number(MATH_PI * 1.0)) { next = Number(MATH_PI      ); quadrant = 1 << 1; }
    if (pos < Number(MATH_PI * 0.5)) { next = Number(MATH_PI * 0.5); quadrant = 1 << 0; }

    while (end >= next)
    {
      mask |= quadrant;
      quadrant <<= 1;

      next += Number(MATH_PI_DIV_2);
    }

    // Apply quadrant mask to finalize min/max.
    if (mask & 0x11) miny =-Number(1.0);
    if (mask & 0x22) maxx = Number(1.0);
    if (mask & 0x44) maxy = Number(1.0);
    if (mask & 0x88) minx =-Number(1.0);

    // Include center point if asked to do.
    if (includeCenterPoint)
    {
      if (minx > Number(0.0)) minx = Number(0.0);
      if (maxx < Number(0.0)) maxx = Number(0.0);
      if (miny > Number(0.0)) miny = Number(0.0);
      if (maxy < Number(0.0)) maxy = Number(0.0);
    }

    // Scale.
    minx *= rx;
    miny *= ry;
    maxx *= rx;
    maxy *= ry;
  }
  else
  {
    minx = -rx;
    miny = -ry;
    maxx =  rx;
    maxy =  ry;
  }

  // Translate.
  Number cx = self->center.x;
  Number cy = self->center.y;
  dst->set(cx + minx, cy + miny, cx + maxx - minx, cy + maxy - miny);
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_arc_init(void)
{
  _g2d.arcf.getBoundingRect = _G2d_ArcT_getBoundingRect<float>;
  _g2d.arcd.getBoundingRect = _G2d_ArcT_getBoundingRect<double>;
}

} // Fog namespace
