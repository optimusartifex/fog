// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_MACUTIL_H
#define _FOG_G2D_MACUTIL_H

#include <Fog/Core/Config/Config.h>
#if defined(FOG_OS_MAC)

#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Foundation/Foundation.h>

namespace Fog {

// [Point Conversions]
template<typename PointT>
NSPoint FOG_INLINE toNSPoint(const PointT& pt) { return NSMakePoint(pt.getX(), pt.getY()); }

// [Rect Conversions]
template<typename RectT>
NSRect FOG_INLINE toNSRect(const RectT& rect) { return NSMakeRect(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight()); }

}

#else
#warning "Fog::MacUtil - Mac support not enabled, but header file included!"
#endif // FOG_OS_MAC

// [Guard]
#endif // _FOG_G2D_MACUTIL_H
