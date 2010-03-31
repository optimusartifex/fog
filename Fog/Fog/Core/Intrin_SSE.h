// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_INTRIN_SSE_H
#define _FOG_CORE_INTRIN_SSE_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <xmmintrin.h>

// MinGW workaround
#if defined(__MINGW32__)
// I can't understand why MinGW will not align data that uses FOG_ALIGN(16) attribute!
#define FOG_ALIGNED_VAR_(type, name, align) \
  int8_t name##_storage[sizeof(type) + align]; \
  type& name = *( (type *)(void *)( ((sysuint_t)(name##_storage) + align - 1) & ~(align - 1) ) )
#else
#define FOG_ALIGNED_VAR_(type, name, align) \
  FOG_ALIGNED_VAR(type, name, align)
#endif

// [Guard]
#endif // _FOG_CORE_INTRIN_SSE_H
