// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPU_INITIALIZER_H
#define _FOG_CORE_CPU_INITIALIZER_H

// [Dependencies]
#include <Fog/Core/Cpu/Cpu.h>
#include <Fog/Core/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Core_Cpu
//! @{

// ============================================================================
// [FOG_OPTIMIZE_MMX]
// ============================================================================

#if defined(FOG_OPTIMIZE_MMX)
#define FOG_CPU_DECLARE_INITIALIZER_MMX(_Initializer_) \
  FOG_NO_EXPORT void _Initializer_(void);

#if defined(FOG_HARDCODE_MMX)
#define FOG_CPU_USE_INITIALIZER_MMX(_Initializer_) \
  _Initializer_();
#else
#define FOG_CPU_USE_INITIALIZER_MMX(_Initializer_) \
  if (::Fog::Cpu::get()->hasFeature(::Fog::CPU_FEATURE_MMX)) _Initializer_();
#endif // FOG_HARDCODE_MMX

#else
#define FOG_CPU_DECLARE_INITIALIZER_MMX(_Initializer_)
#define FOG_CPU_USE_INITIALIZER_MMX(_Initializer_)
#endif // FOG_OPTIMIZE_MMX

// ============================================================================
// [FOG_OPTIMIZE_3DNOW]
// ============================================================================

#if defined(FOG_OPTIMIZE_3DNOW)
#define FOG_CPU_DECLARE_INITIALIZER_3DNOW(_Initializer_) \
  FOG_NO_EXPORT void _Initializer_(void);

#if defined(FOG_HARDCODE_3DNOW)
#define FOG_CPU_USE_INITIALIZER_3DNOW(_Initializer_) \
  _Initializer_();
#else
#define FOG_CPU_USE_INITIALIZER_3DNOW(_Initializer_) \
  if (::Fog::Cpu::get()->hasFeature(::Fog::CPU_FEATURE_3DNOW)) _Initializer_();
#endif // FOG_HARDCODE_3DNOW

#else
#define FOG_CPU_DECLARE_INITIALIZER_3DNOW(_Initializer_)
#define FOG_CPU_USE_INITIALIZER_3DNOW(_Initializer_)
#endif // FOG_OPTIMIZE_3DNOW

// ============================================================================
// [FOG_OPTIMIZE_SSE]
// ============================================================================

#if defined(FOG_OPTIMIZE_SSE)
#define FOG_CPU_DECLARE_INITIALIZER_SSE(_Initializer_) \
  FOG_NO_EXPORT void _Initializer_(void);

#if defined(FOG_HARDCODE_SSE)
#define FOG_CPU_USE_INITIALIZER_SSE(_Initializer_) \
  _Initializer_();
#else
#define FOG_CPU_USE_INITIALIZER_SSE(_Initializer_) \
  if (::Fog::Cpu::get()->hasFeature(::Fog::CPU_FEATURE_SSE)) _Initializer_();
#endif // FOG_HARDCODE_SSE

#else
#define FOG_CPU_DECLARE_INITIALIZER_SSE(_Initializer_)
#define FOG_CPU_USE_INITIALIZER_SSE(_Initializer_)
#endif // FOG_OPTIMIZE_SSE

// ============================================================================
// [FOG_OPTIMIZE_SSE2]
// ============================================================================

#if defined(FOG_OPTIMIZE_SSE2)
#define FOG_CPU_DECLARE_INITIALIZER_SSE2(_Initializer_) \
  FOG_NO_EXPORT void _Initializer_(void);

#if defined(FOG_HARDCODE_SSE2)
#define FOG_CPU_USE_INITIALIZER_SSE2(_Initializer_) \
  _Initializer_();
#else
#define FOG_CPU_USE_INITIALIZER_SSE2(_Initializer_) \
  if (::Fog::Cpu::get()->hasFeature(::Fog::CPU_FEATURE_SSE2)) _Initializer_();
#endif // FOG_HARDCODE_SSE2

#else
#define FOG_CPU_DECLARE_INITIALIZER_SSE2(_Initializer_)
#define FOG_CPU_USE_INITIALIZER_SSE2(_Initializer_)
#endif // FOG_OPTIMIZE_SSE2

// ============================================================================
// [FOG_OPTIMIZE_SSE3]
// ============================================================================

#if defined(FOG_OPTIMIZE_SSE3)
#define FOG_CPU_DECLARE_INITIALIZER_SSE3(_Initializer_) \
  FOG_NO_EXPORT void _Initializer_(void);

#if defined(FOG_HARDCODE_SSE3)
#define FOG_CPU_USE_INITIALIZER_SSE3(_Initializer_) \
  _Initializer_();
#else
#define FOG_CPU_USE_INITIALIZER_SSE3(_Initializer_) \
  if (::Fog::Cpu::get()->hasFeature(::Fog::CPU_FEATURE_SSE3)) _Initializer_();
#endif // FOG_HARDCODE_SSE3

#else
#define FOG_CPU_DECLARE_INITIALIZER_SSE3(_Initializer_)
#define FOG_CPU_USE_INITIALIZER_SSE3(_Initializer_)
#endif // FOG_OPTIMIZE_SSE3

// ============================================================================
// [FOG_OPTIMIZE_SSSE3]
// ============================================================================

#if defined(FOG_OPTIMIZE_SSSE3)
#define FOG_CPU_DECLARE_INITIALIZER_SSSE3(_Initializer_) \
  FOG_NO_EXPORT void _Initializer_(void);

#if defined(FOG_HARDCODE_SSSE3)
#define FOG_CPU_USE_INITIALIZER_SSSE3(_Initializer_) \
  _Initializer_();
#else
#define FOG_CPU_USE_INITIALIZER_SSSE3(_Initializer_) \
  if (::Fog::Cpu::get()->hasFeature(::Fog::CPU_FEATURE_SSSE3)) _Initializer_();
#endif // FOG_HARDCODE_SSSE3

#else
#define FOG_CPU_DECLARE_INITIALIZER_SSSE3(_Initializer_)
#define FOG_CPU_USE_INITIALIZER_SSSE3(_Initializer_)
#endif // FOG_OPTIMIZE_SSSE3

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_CPU_INITIALIZER_H
