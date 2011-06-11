// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_FUNCTION_H
#define _FOG_CORE_MATH_FUNCTION_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Global/Class.h>

namespace Fog {

//! @addtogroup Fog_Core_Math
//! @{

// ============================================================================
// [Fog::FunctionF]
// ============================================================================

struct FOG_API FunctionF
{
  virtual ~FunctionF() = 0;
  virtual err_t evaluate(float* dst, float t) const = 0;
};

// ============================================================================
// [Fog::FunctionD]
// ============================================================================

struct FOG_API FunctionD
{
  virtual ~FunctionD() = 0;
  virtual err_t evaluate(double* dst, double t) const = 0;
};

// ============================================================================
// [Fog::FunctionT<>]
// ============================================================================

FOG_CLASS_PRECISION_F_D(Function)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_FUNCTION_H
