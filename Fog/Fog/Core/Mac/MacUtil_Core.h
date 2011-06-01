// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MAC_MACUTIL_CORE_H
#define _FOG_CORE_MAC_MACUTIL_CORE_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Mac/MacDefs.h>
#include <Fog/Core/Tools/String.h>

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
#endif // _FOG_CORE_MAC_MACUTIL_CORE_H
