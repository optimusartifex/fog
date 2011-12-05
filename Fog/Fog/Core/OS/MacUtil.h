// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_MACUTIL_H
#define _FOG_CORE_OS_MACUTIL_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Tools/String.h>

// [Forward Declarations]
#ifdef __OBJC__
@class NSAutoreleasePool;
@class NSFont;
@class NSString;
#else
class NSAutoreleasePool;
class NSFont;
class NSString;
#endif

namespace Fog {
namespace MacUtil {

// ============================================================================
// [Fog::MacUtil]
// ============================================================================

err_t StringFromNS(String& dst, NSString* src);
// TODO: Not Fog style.
NSString* NSFromString(const String& src);

} // MacUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_OS_MACUTIL_H
