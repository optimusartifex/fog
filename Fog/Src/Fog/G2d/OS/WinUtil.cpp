// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/OS/WinUtil.h>
#include <Fog/Core/Threading/ThreadLocal.h>
#include <Fog/G2d/OS/WinUtil.h>

namespace Fog {

// ============================================================================
// [Fog::WinUtil - Helpers]
// ============================================================================

static Static<ThreadLocal> WinUtil_threadLocal;

static void FOG_CDECL WinUtil_threadLocal_dtor(void* value)
{
  if (value != NULL)
    ::DeleteDC((HDC)value);
}

// ============================================================================
// [Fog::WinUtil - ThreadLocalDC]
// ============================================================================

static HDC FOG_CDECL WinUtil_getThreadLocalDC(void)
{
  HDC hdc = (HDC)WinUtil_threadLocal->get();
  if (hdc != NULL)
    return hdc;

  hdc = ::CreateCompatibleDC(NULL);
  if (hdc == NULL)
    return NULL;
  WinUtil_threadLocal->set((void*)hdc);

  ::SetGraphicsMode(hdc, GM_ADVANCED);
  return hdc;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void WinUtil_G2d_init(void)
{
  WinUtil_threadLocal.init();
  WinUtil_threadLocal->create(WinUtil_threadLocal_dtor);

  fog_api.winutil_getThreadLocalDC = WinUtil_getThreadLocalDC;
}

FOG_NO_EXPORT void WinUtil_G2d_fini(void)
{
  WinUtil_threadLocal.destroy();
}

} // Fog namespace
