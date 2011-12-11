// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/OS/MacUtil.h>
#include <Fog/Core/Tools/String.h>

// [Dependencies - Mac]
#import <Cocoa/Cocoa.h>

namespace Fog {
  
// ============================================================================
// [Fog::MacUtil]
// ============================================================================

static err_t FOG_CDECL StringW_fromNSString(StringW* self, const NSString* src)
{
  NSRange range;
  range.location = 0;
  range.length = [src length];

  FOG_RETURN_ON_ERROR(self->resize(range.length));

  unichar* data = reinterpret_cast<unichar*>(self->getDataX());
  [src getCharacters: data range: range];

  return ERR_OK;
}

static err_t FOG_CDECL StringW_toNSString(const StringW* self, NSString** dst)
{
  StringDataW* d = self->_d;

  const unichar* sData = reinterpret_cast<const unichar*>(d->data);
  NSUInteger sLength = (NSUInteger)d->length;

  *dst = [NSString stringWithCharacters: sData length: sLength];

  if (*dst)
    return ERR_OK;
  else
    return ERR_RT_OUT_OF_MEMORY;
}
  
// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MacUtil_init(void)
{
  fog_api.stringw_fromNSString = StringW_fromNSString;
  fog_api.stringw_toNSString = StringW_toNSString;
}

} // Fog namespace
