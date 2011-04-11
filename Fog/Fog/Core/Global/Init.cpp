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

#include <Fog/Core/Global/Init_p.h>
#include <Fog/G2d/Global/Init_p.h>
#include <Fog/Gui/Global/Init_p.h>
#include <Fog/Svg/Global/Init_p.h>
#include <Fog/Xml/Global/Init_p.h>

// ============================================================================
// [Fog::All - Library Initializers]
// ============================================================================

FOG_CAPI_DECLARE void _fog_init(void)
{
  Fog::_core_init();
  Fog::_xml_init();
  Fog::_g2d_init();
  Fog::_svg_init();
  Fog::_gui_init();
}

FOG_CAPI_DECLARE void _fog_fini(void)
{
  Fog::_gui_fini();
  Fog::_svg_fini();
  Fog::_g2d_fini();
  Fog::_xml_fini();
  Fog::_core_fini();
}

// ============================================================================
// [Fog::All - AutoInit]
// ============================================================================

// Static instance of class that will call _fog_init() and _fog_fini().
struct FOG_NO_EXPORT FogAutoInit
{
  FogAutoInit()
  {
    _fog_init();
  }

  ~FogAutoInit()
  {
    // We shutdown only if there is no error and fog_failed is zero. This is
    // because for example a Fog::ThreadPool might need to release all threads,
    // but an assertion can be raised by the thread owned by it.
    //
    // So if failed, it's better to exit quickly than fail again.
    if (fog_failed == 0) _fog_fini();
  }
};

static FogAutoInit fog_auto_init;

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

namespace Fog {

FOG_NO_EXPORT void _core_init(void)
{
  // fog_std_init();

  _core_cpu_init();
  _core_memory_init();
  _core_lock_init(),
  _core_error_init();
  _core_math_init();             // Depends to Cpu.

  _core_list_init();
  _core_hash_init();
  _core_bytearray_init();
  _core_string_init();
  _core_textcodec_init();
  _core_managedstring_init();    // Depends to String and Lock.
  _core_strings_init();          // Depends to StringCache.
  _core_locale_init();           // Depends to TextCodec.
  _core_value_init();
  _core_library_init();
  _core_os_init();               // Depends to String and TextCodec.
  _core_stream_init();
  _core_thread_init();
  _core_threadpool_init();
  _core_object_init();           // Depends to String.
  _core_application_init();      // Depends to Many.
}

FOG_NO_EXPORT void _core_fini(void)
{
  _core_application_fini();
  _core_object_fini();
  _core_threadpool_fini();
  _core_thread_fini();
  _core_stream_fini();
  _core_os_fini();
  _core_library_fini();
  _core_value_fini();
  _core_locale_fini();
  _core_strings_fini();
  _core_managedstring_fini();
  _core_textcodec_fini();
  _core_string_fini();
  _core_bytearray_fini();
  _core_hash_fini();
  _core_list_fini();

  _core_error_fini();
  _core_memory_fini();

  // fog_std_fini();
}

} // Fog namespace
