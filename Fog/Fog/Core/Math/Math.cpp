// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>

namespace Fog {

// ============================================================================
// [Fog::Math]
// ============================================================================

namespace Math {
} // Math namespace

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_math_init(void)
{
  _core_math_init_solve();
}

} // Fog namespace
