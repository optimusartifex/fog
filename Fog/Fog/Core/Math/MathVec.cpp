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
#include <Fog/Core/Global/Init_Core_p.h>
#include <Fog/Core/Math/MathVec.h>

namespace Fog {

// ============================================================================
// [Fog::Math]
// ============================================================================

static void FOG_CDECL _Core_MathVec_vFloatFromDouble(float* dst, const double* src, size_t length)
{
  for (size_t i = 0; i < length; i++) dst[i] = float(src[i]);
}

static void FOG_CDECL _Core_MathVec_vDoubleFromFloat(double* dst, const float* src, size_t length)
{
  for (size_t i = 0; i < length; i++) dst[i] = double(src[i]);
}

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_math_init_vec(void)
{
  _core.mathf.vFloatFromDouble = _Core_MathVec_vFloatFromDouble;
  _core.mathd.vDoubleFromFloat = _Core_MathVec_vDoubleFromFloat;
}

} // Fog namespace
