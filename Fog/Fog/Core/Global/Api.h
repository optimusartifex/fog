// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_API_H
#define _FOG_CORE_GLOBAL_API_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

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

  typedef int (FOG_CDECL* MathF_SolveQuadraticFunction)(float* dst, const float* src);
  typedef int (FOG_CDECL* MathD_SolveQuadraticFunction)(double* dst, const double* src);

  typedef int (FOG_CDECL* MathF_SolveCubicFunction)(float* dst, const float* src);
  typedef int (FOG_CDECL* MathD_SolveCubicFunction)(double* dst, const double* src);

  struct FOG_NO_EXPORT _FuncsMathF
  {
    MathF_SolveQuadraticFunction solveQuadraticFunction;
    MathF_SolveCubicFunction solveCubicFunction;
  } mathf;

  struct FOG_NO_EXPORT _FuncsMathD
  {
    MathD_SolveQuadraticFunction solveQuadraticFunction;
    MathD_SolveCubicFunction solveCubicFunction;
  } mathd;

  // --------------------------------------------------------------------------
  // [Memory]
  // --------------------------------------------------------------------------

  typedef void* (FOG_CDECL *Memory_Alloc)(sysuint_t size);
  typedef void* (FOG_CDECL *Memory_Calloc)(sysuint_t size);
  typedef void* (FOG_CDECL *Memory_Realloc)(void* ptr, sysuint_t size);
  typedef void* (FOG_CDECL *Memory_ReallocOrFree)(void* ptr, sysuint_t size);
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
    Memory_Alloc alloc;
    Memory_Calloc calloc;
    Memory_Realloc realloc;
    Memory_ReallocOrFree reallocOrFree;
    Memory_Free free;

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
