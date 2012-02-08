// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RASTER_RASTERINIT_P_H
#define _FOG_G2D_RASTER_RASTERINIT_P_H

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>

namespace Fog {

// ============================================================================
// [Fog::FOG_RASTER_INIT / FOG_RASTER_SKIP]
// ============================================================================

//! @internal
//!
//! @brief Initialize the function pointer @a _Dst_ by @a _Func_.
#define FOG_RASTER_INIT(_Dst_, _Func_) funcs._Dst_ = _Func_

//! @internal
//!
//! @brief Initialize the function poitner @a _Dst_ later by post-init (MARKER).
#define FOG_RASTER_SKIP(_Dst_) (void)funcs._Dst_

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RASTER_RASTERINIT_P_H
