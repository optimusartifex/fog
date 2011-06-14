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

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

namespace Fog {

FOG_NO_EXPORT void _core_init(void)
{
  _core_cpu_init();
  _core_memory_init_ops();
  _core_memory_init_alloc();
  _core_memory_init_cleanup();
  _core_memory_init_manager();

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
}

} // Fog namespace
