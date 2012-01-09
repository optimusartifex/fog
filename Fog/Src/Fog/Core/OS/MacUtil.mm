// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/OS/MacUtil.h>
#include <Fog/Core/Tools/String.h>

// [Dependencies - Mac]
#import <Foundation/Foundation.h>

namespace Fog {
  
// ============================================================================
// [Fog::MacUtil]
// ============================================================================

static err_t FOG_CDECL StringW_opCFString(StringW* self, uint32_t cntOp, CFStringRef src)
{
  CFIndex length = CFStringGetLength(src);

  if (length == 0)
  {
    if (cntOp == CONTAINER_OP_REPLACE)
      self->clear();
    return ERR_OK;
  }

  CharW* p = self->_prepare(cntOp, length);
  if (FOG_IS_NULL(p))
    return ERR_RT_OUT_OF_MEMORY;

  CFStringGetCharacters(src, CFRangeMake(0, length), reinterpret_cast<UniChar*>(p));
  return ERR_OK;
}

static err_t FOG_CDECL StringW_toCFString(const StringW* self, CFStringRef* dst)
{
  StringDataW* d = self->_d;
  size_t length = d->length;

  if (sizeof(size_t) > sizeof(CFIndex) && length >= (size_t)LONG_MAX)
    return ERR_RT_OVERFLOW;

  *dst = CFStringCreateWithCharacters(NULL, (const UniChar*)d->data, length);
  if (*dst != NULL)
    return ERR_OK;
  else
    return ERR_RT_OUT_OF_MEMORY;
}
  
// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MacUtil_init(void)
{
  fog_api.stringw_opCFString = StringW_opCFString;
  fog_api.stringw_toCFString = StringW_toCFString;
}

} // Fog namespace
