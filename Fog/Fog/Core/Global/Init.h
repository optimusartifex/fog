// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_INIT_H
#define _FOG_CORE_GLOBAL_INIT_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Fog - Entry Point / Finalizer]
// ============================================================================

//! @brief Fog-library initializer.
FOG_CAPI_EXTERN void _fog_init(void);
//! @brief Fog-library finalizer.
FOG_CAPI_EXTERN void _fog_fini(void);

// C++ hack to initialize the Fog library statically. It's just needed to
// touch this symbol from the code that consumes this library.
#if defined(FOG_BUILD_STATIC)
FOG_CAPI_EXTERN void _fog_init_static(void);
#endif // FOG_BUILD_STATIC

//! @}

// [Guard]
#endif // _FOG_CORE_GLOBAL_INIT_H
