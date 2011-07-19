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
#include <Fog/Core/Math/MathVec.h>

namespace Fog {

// ============================================================================
// [Fog::Math]
// ============================================================================

static void FOG_CDECL MathF_vecFloatFromDouble(float* dst, const double* src, size_t length)
{
  for (size_t i = 0; i < length; i++) dst[i] = float(src[i]);
}

static void FOG_CDECL MathD_vecDoubleFromFloat(double* dst, const float* src, size_t length)
{
  for (size_t i = 0; i < length; i++) dst[i] = double(src[i]);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Math_init_vec(void)
{
  _api.mathf.vecFloatFromDouble = MathF_vecFloatFromDouble;
  _api.mathd.vecDoubleFromFloat = MathD_vecDoubleFromFloat;
}

} // Fog namespace
