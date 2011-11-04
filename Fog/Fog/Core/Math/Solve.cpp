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
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/Solve.h>
#include <Fog/Core/Memory/MemBufferTmp_p.h>
#include <Fog/Core/Tools/Algorithm.h>
#include <Fog/Core/Tools/Swap.h>

namespace Fog {

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Math_init_solvePolynomialAnalytic(void);
FOG_NO_EXPORT void Math_init_solvePolynomialEigen(void);
FOG_NO_EXPORT void Math_init_solvePolynomialJenkinsTraub(void);

FOG_NO_EXPORT void Math_init_solve(void)
{
  Math_init_solvePolynomialAnalytic();
  Math_init_solvePolynomialEigen();
  Math_init_solvePolynomialJenkinsTraub();
}

} // Fog namespace
