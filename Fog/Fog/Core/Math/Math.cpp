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
#include <Fog/Core/Math/Math.h>

namespace Fog {

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Math_init_integrate(void);
FOG_NO_EXPORT void Math_init_solve(void);
FOG_NO_EXPORT void Math_init_vec(void);

FOG_NO_EXPORT void Math_init(void)
{
  Math_init_vec();
  Math_init_integrate();
  Math_init_solve();
}

} // Fog namespace
