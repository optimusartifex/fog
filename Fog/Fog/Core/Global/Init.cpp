// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Debug.h>

#include <Fog/Core/Global/Init_Core_p.h>
#include <Fog/G2d/Global/Init_G2d_p.h>
#include <Fog/Gui/Global/Init_UI_p.h>
#include <Fog/Svg/Global/Init_Svg_p.h>

// ============================================================================
// [Fog::All - Library Initializers]
// ============================================================================

static size_t _fog_init_counter = 0;

FOG_CAPI_DECLARE void _fog_init(void)
{
  if (++_fog_init_counter == 1)
  {
    Fog::_core_init();
    Fog::_g2d_init();
    Fog::_svg_init();
    Fog::_gui_init();
  }
}

FOG_CAPI_DECLARE void _fog_fini(void)
{
  if (--_fog_init_counter == 0)
  {
    Fog::_gui_fini();
    Fog::_svg_fini();
    Fog::_g2d_fini();
    Fog::_core_fini();
  }
}

// ============================================================================
// [_FogInitHelper]
// ============================================================================

// Static instance of class that will call _fog_init() and _fog_fini().
struct FOG_NO_EXPORT _FogInitHelper
{
  _FogInitHelper()
  {
    _fog_init();
  }

  ~_FogInitHelper()
  {
    // We shutdown only if there is no error and fog_failed is zero. This is
    // because for example a Fog::ThreadPool might need to release all threads,
    // but an assertion can be raised by the thread owned by it.
    //
    // So if failed, it's better to exit quickly than fail again.
    if (fog_failed == 0) _fog_fini();
  }
};

static _FogInitHelper _fog_init_helper;

#if defined(FOG_BUILD_STATIC)
FOG_CAPI_DECLARE void _fog_init_static(void)
{
  // Nothing to do here.
}
#endif // FOG_BUILD_STATIC
