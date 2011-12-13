// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_MAC_MACUTIL_H
#define _FOG_G2D_MAC_MACUTIL_H

// [Dependencies]
#include <Fog/Core/OS/MacDefs.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>

namespace Fog {

// ============================================================================
// [Fog::MacUtil]
// ============================================================================

// [Point Conversions]
template<typename PointT>
static FOG_INLINE CGPoint toCGPoint(const PointT& pt)
{
  return CGPointMake(CGFloat(pt.x), CGFloat(pt.y));
}

// [Rect Conversions]
template<typename RectT>
static FOG_INLINE CGRect toCGRect(const RectT& rect)
{
  return CGRectMake(CGFloat(rect.x), CGFloat(rect.y), CGFloat(rect.w), CGFloat(rect.h));
}

}

// [Guard]
#endif // _FOG_G2D_MAC_MACUTIL_H
