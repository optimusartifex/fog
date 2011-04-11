// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDERINIT_P_H
#define _FOG_G2D_RENDER_RENDERINIT_P_H

// [Dependencies]
#include <Fog/G2d/Global/Init_p.h>

namespace Fog {

// ============================================================================
// [Fog::Init - C]
// ============================================================================

FOG_NO_EXPORT void _g2d_render_init(void);
FOG_NO_EXPORT void _g2d_render_init_c(void);
FOG_NO_EXPORT void _g2d_render_init_post(void);
FOG_NO_EXPORT void _g2d_render_fini(void);

// ============================================================================
// [Fog::RENDER_INIT / RENDER_POST]
// ============================================================================

//! @internal
//!
//! @brief Initialize the function pointer @a _Dst_ by @a _Func_.
#define RENDER_INIT(_Dst_, _Func_) funcs._Dst_ = _Func_

//! @internal
//!
//! @brief Initialize the function poitner @a _Dst_ later by post-init (MARKER).
#define RENDER_POST(_Dst_) FOG_NOP

//! @internal
//!
//! @brief Never initialize the function poitner @a _Dst_ (MARKER).
#define RENDER_DONT(_Dst_) FOG_NOP

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDERINIT_P_H
