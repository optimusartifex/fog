// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_INIT_P_H
#define _FOG_CORE_GLOBAL_INIT_P_H

// [Dependencies]
#include <Fog/Core/Global/Constants.h>

// ============================================================================
// [Fog::All - Init / Fini]
// ============================================================================

FOG_CAPI_EXTERN void _fog_init(void);
FOG_CAPI_EXTERN void _fog_fini(void);

// ============================================================================
// [Fog::Core - Init / Fini]
// ============================================================================

namespace Fog {

// [Fog/Core]
FOG_NO_EXPORT void _core_init(void);
FOG_NO_EXPORT void _core_fini(void);

// [Fog/Core/Collection]
FOG_NO_EXPORT void _core_list_init(void);
FOG_NO_EXPORT void _core_list_fini(void);

FOG_NO_EXPORT void _core_hash_init(void);
FOG_NO_EXPORT void _core_hash_fini(void);

// [Fog/Core/Cpu]
FOG_NO_EXPORT void _core_cpu_init(void);

// [Fog/Core/Data]
FOG_NO_EXPORT void _core_value_init(void);
FOG_NO_EXPORT void _core_value_fini(void);

// [Fog/Core/Global]
FOG_NO_EXPORT void _core_error_init(void);
FOG_NO_EXPORT void _core_error_fini(void);

// [Fog/Core/IO]
FOG_NO_EXPORT void _core_stream_init(void);
FOG_NO_EXPORT void _core_stream_fini(void);

// [Fog/Core/Library]
FOG_NO_EXPORT void _core_library_init(void);
FOG_NO_EXPORT void _core_library_fini(void);

// [Fog/Core/Math]
FOG_NO_EXPORT void _core_math_init(void);
FOG_NO_EXPORT void _core_math_init_solve(void);

// [Fog/Core/Memory]
FOG_NO_EXPORT void _core_memory_init(void);
FOG_NO_EXPORT void _core_memory_fini(void);

// [Fog/Core/OS]
FOG_NO_EXPORT void _core_os_init(void);
FOG_NO_EXPORT void _core_os_fini(void);

// [Fog/Core/System]
FOG_NO_EXPORT void _core_application_init(void);
FOG_NO_EXPORT void _core_application_fini(void);

FOG_NO_EXPORT void _core_object_init(void);
FOG_NO_EXPORT void _core_object_fini(void);

// [Fog/Core/Threading]
FOG_NO_EXPORT void _core_lock_init(void);
FOG_NO_EXPORT void _core_lock_fini(void);

FOG_NO_EXPORT void _core_thread_init(void);
FOG_NO_EXPORT void _core_thread_fini(void);

FOG_NO_EXPORT void _core_threadpool_init(void);
FOG_NO_EXPORT void _core_threadpool_fini(void);

// [Fog/Core/Tools]
FOG_NO_EXPORT void _core_bytearray_init(void);
FOG_NO_EXPORT void _core_bytearray_fini(void);

FOG_NO_EXPORT void _core_locale_init(void);
FOG_NO_EXPORT void _core_locale_fini(void);

FOG_NO_EXPORT void _core_managedstring_init(void);
FOG_NO_EXPORT void _core_managedstring_fini(void);

FOG_NO_EXPORT void _core_string_init(void);
FOG_NO_EXPORT void _core_string_fini(void);

FOG_NO_EXPORT void _core_strings_init(void);
FOG_NO_EXPORT void _core_strings_fini(void);

FOG_NO_EXPORT void _core_textcodec_init(void);
FOG_NO_EXPORT void _core_textcodec_fini(void);

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_INIT_P_H
