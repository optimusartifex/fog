// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Error.h>

#if defined(FOG_OS_WINDOWS)
# include <windows.h>
#endif // FOG_OS_WINDOWS

namespace Fog {

// ============================================================================
// [Fog::Error]
// ============================================================================

String Error::getMessage(err_t code)
{
  String message;

  // TODO:

  return message;
}

err_t Error::registerRange(err_t start, err_t end, TranslatorFn fn)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t Error::unregisterRange(err_t start, err_t end)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_error_init(void)
{
  using namespace Fog;

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_error_shutdown(void)
{
}
