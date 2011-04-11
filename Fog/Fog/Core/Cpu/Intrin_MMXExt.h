// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPU_INTRIN_MMXEXT_H
#define _FOG_CORE_CPU_INTRIN_MMXEXT_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Cpu/Intrin_MMX.h>

// ============================================================================
// [Fog::Core - MMX-Ext - Fixes]
// ============================================================================

// Include some MMX instructions when building only using pure MMX (no SSE) 
// or MMX+3dNow known as MMX extended instructions. Mainly used by specialized 
// MMX+3dNow optimizations where without SSE.
//
// These MMX extensions are always declared by xmmintrin.h, but it can be 
// only declared when SSE is enabled when using GCC.
#if defined(FOG_CC_GNU)

static FOG_INLINE int _mm_movemask_pi8(__m64 a)
{
  int result;
  __asm__("pmovmskb %1, %0\n\t" : "=r"(result) : "y"(a));
  return result;
}

#endif // FOG_CC_GNU

// [Guard]
#endif // _FOG_CORE_CPU_INTRIN_MMXEXT_H
