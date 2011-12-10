// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/OS/MacUtil.h>

// [Dependencies - Mac]
#import <Cocoa/Cocoa.h>

namespace Fog {
namespace MacUtil {
  
// ============================================================================
// [Fog::MacUtil]
// ============================================================================

err_t StringW_fromNSString(StringW& s, NSString* src)
{
  NSRange range;
  range.location = 0;
  range.length = [src length];

  FOG_RETURN_ON_ERROR(s.resize(range.length));

  unichar* data = reinterpret_cast<unichar*>(s.getDataX());
  [src getCharacters: data range: range];

  return ERR_OK;
}

err_t StringW_toNSString(const StringW& s, NSString** dst)
{
  const unichar* sData = reinterpret_cast<const unichar*>(s.getData());
  NSUInteger sLength = (NSUInteger)s.getLength();

  *dst = [NSString stringWithCharacters: sData length: sLength];

  if (*dst)
    return ERR_OK;
  else
    return ERR_RT_OUT_OF_MEMORY;
}
  
} // MacUtil namespace
} // Fog namespace
