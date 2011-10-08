// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/OS/MacUtil.h>

// [Cocoa]
#import <Cocoa/Cocoa.h>

namespace Fog {
namespace MacUtil {
  
// ============================================================================
// [Fog::MacUtil]
// ============================================================================

err_t StringFromNS(String& dst, NSString* src)
{
  NSRange range;
  range.location = 0;
  range.length = [src length];

  FOG_RETURN_ON_ERROR(dst.resize(range.length));

  unichar* data = reinterpret_cast<unichar*>(dst.getDataX());
  [src getCharacters: data range: range];

  return ERR_OK;
}

// TODO: Not Fog style.
NSString* NSFromString(const String& src)
{
  const unichar* data = reinterpret_cast<const unichar*>(src.getData());
  NSUInteger length = (NSUInteger)src.getLength();

  return [NSString stringWithCharacters: data length: length];
}
  
} // MacUtil namespace
} // Fog namespace
