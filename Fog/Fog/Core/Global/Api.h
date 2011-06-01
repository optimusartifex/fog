// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_API_H
#define _FOG_CORE_GLOBAL_API_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct FunctionF;
struct FunctionD;
struct IntervalF;
struct IntervalD;

// ============================================================================
// [Fog::_CoreApi]
// ============================================================================

//! @internal
//!
//! @brief Fog/Core functions.
struct FOG_NO_EXPORT _CoreApi
{
  // --------------------------------------------------------------------------
  // [Math]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL* MathF_VFloatFromDouble)(float* dst, const double* src, sysuint_t length);
  typedef void (FOG_CDECL* MathD_VDoubleFromFloat)(double* dst, const float* src, sysuint_t length);

  typedef err_t (FOG_CDECL* MathF_Integrate)(float* dst, const FunctionF& func, const IntervalF& interval, uint32_t steps);
  typedef err_t (FOG_CDECL* MathD_Integrate)(double* dst, const FunctionD& func, const IntervalD& interval, uint32_t steps);

  typedef int (FOG_CDECL* MathF_SolveQuadraticFunction)(float* dst, const float* src);
  typedef int (FOG_CDECL* MathD_SolveQuadraticFunction)(double* dst, const double* src);

  typedef int (FOG_CDECL* MathF_SolveQuadraticFunctionAt)(float* dst, const float* src, const IntervalF& interval);
  typedef int (FOG_CDECL* MathD_SolveQuadraticFunctionAt)(double* dst, const double* src, const IntervalD& interval);

  typedef int (FOG_CDECL* MathF_SolveCubicFunction)(float* dst, const float* src);
  typedef int (FOG_CDECL* MathD_SolveCubicFunction)(double* dst, const double* src);

  typedef int (FOG_CDECL* MathF_SolveCubicFunctionAt)(float* dst, const float* src, const IntervalF& interval);
  typedef int (FOG_CDECL* MathD_SolveCubicFunctionAt)(double* dst, const double* src, const IntervalD& interval);

  struct FOG_NO_EXPORT _FuncsMathF
  {
    MathF_VFloatFromDouble vFloatFromDouble;
    MathF_Integrate integrate[MATH_INTEGRATION_METHOD_COUNT];
    MathF_SolveQuadraticFunction solveQuadraticFunction;
    MathF_SolveQuadraticFunctionAt solveQuadraticFunctionAt;
    MathF_SolveCubicFunction solveCubicFunction;
    MathF_SolveCubicFunctionAt solveCubicFunctionAt;
  } mathf;

  struct FOG_NO_EXPORT _FuncsMathD
  {
    MathD_VDoubleFromFloat vDoubleFromFloat;
    MathD_Integrate integrate[MATH_INTEGRATION_METHOD_COUNT];
    MathD_SolveQuadraticFunction solveQuadraticFunction;
    MathD_SolveQuadraticFunctionAt solveQuadraticFunctionAt;
    MathD_SolveCubicFunction solveCubicFunction;
    MathD_SolveCubicFunctionAt solveCubicFunctionAt;
  } mathd;

  // --------------------------------------------------------------------------
  // [Memory]
  // --------------------------------------------------------------------------

  typedef void* (FOG_CDECL *Memory_Alloc)(sysuint_t size);
  typedef void* (FOG_CDECL *Memory_Calloc)(sysuint_t size);
  typedef void* (FOG_CDECL *Memory_Realloc)(void* ptr, sysuint_t size);
  typedef void (FOG_CDECL *Memory_Free)(void* ptr);

  typedef void (FOG_CDECL *Memory_Cleanup)();
  typedef err_t (FOG_CDECL *Memory_RegisterCleanupHandler)(void* handler, void* closure);
  typedef err_t (FOG_CDECL *Memory_UnregisterCleanupHandler)(void* handler, void* closure);

  typedef void* (FOG_CDECL *Memory_Copy)(void* dst, const void* src, sysuint_t size);
  typedef void* (FOG_CDECL *Memory_Move)(void* dst, const void* src, sysuint_t size);

  typedef void* (FOG_CDECL *Memory_Zero)(void* dst, sysuint_t size);
  typedef void* (FOG_CDECL *Memory_Set)(void* dst, uint val, sysuint_t size);

  typedef void (FOG_CDECL *Memory_Xchg)(void* mem0, void* mem1, sysuint_t size);

  struct FOG_NO_EXPORT _FuncsMemory
  {
    // These functions are prefixed by _m_, because some memory leak detectors
    // use macros to replace malloc(), realloc(), free(), etc... This prevents
    // to be messed with that.
    Memory_Alloc _m_alloc;
    Memory_Calloc _m_calloc;
    Memory_Realloc _m_realloc;
    Memory_Free _m_free;

    Memory_Cleanup cleanup;
    Memory_RegisterCleanupHandler registerCleanupHandler;
    Memory_UnregisterCleanupHandler unregisterCleanupHandler;

    Memory_Copy copy;
    Memory_Move move;
    Memory_Zero zero;
    Memory_Set set;

    Memory_Copy copy_nt;
    Memory_Zero zero_nt;
    Memory_Set set_nt;

    Memory_Xchg xchg;
  } memory;
};

extern FOG_API _CoreApi _core;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_API_H
