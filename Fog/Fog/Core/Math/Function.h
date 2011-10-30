// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_FUNCTION_H
#define _FOG_CORE_MATH_FUNCTION_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::MathFunctionF]
// ============================================================================

struct FOG_API MathFunctionF
{
  virtual ~MathFunctionF();
  virtual err_t evaluate(float* dst, float t) const = 0;
};

// ============================================================================
// [Fog::MathFunctionD]
// ============================================================================

struct FOG_API MathFunctionD
{
  virtual ~MathFunctionD();
  virtual err_t evaluate(double* dst, double t) const = 0;
};

// ============================================================================
// [Fog::FunctionT<>]
// ============================================================================

_FOG_NUM_T(MathFunction)
_FOG_NUM_F(MathFunction)
_FOG_NUM_D(MathFunction)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_FUNCTION_H
