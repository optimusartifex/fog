// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Guard]
#ifndef _FOG_CORE_CPUINFO_H
#define _FOG_CORE_CPUINFO_H

// [Dependencies]
#include <Fog/Core/Build.h>

namespace Fog {

//! @addtogroup Fog_Core_Environment
//! @{

// ============================================================================
// [Fog::CpuId]
// ============================================================================

union FOG_HIDDEN CpuId
{
  uint32_t i[4];
  struct
  {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
  };
};

// ============================================================================
// [Fog::CpuInfo]
// ============================================================================

//! @brief Information about CPU features.
//!
//! Easy way to read CPU features. CPU is detected in startup time, so this class
//! has method only to read the detected state. Use @c Cpu::features to 
//! retrieve information about your CPU and @c use Cpu::Features enumeration 
//! to get interesting features for you.
//!
//! Small example how to check for SSE2 support.
//!
//! @verbatim
//! if (Fog::cpuInfo->hasFeature(Fog::CpuInfo::FEATURE_SSE2))
//! {
//!   // cpu has SSE2 support
//! }
//! else
//! {
//!   // cpu hasn't SSE2 support
//! }
//! @endverbatim
struct FOG_HIDDEN CpuInfo
{
  //! @brief Cpu short vendor string.
  char vendor[16];
  //! @brief Cpu long vendor string (brand).
  char brand[64];
  //! @brief Cpu family ID.
  uint32_t family;
  //! @brief Cpu model ID.
  uint32_t model;
  //! @brief Cpu stepping.
  uint32_t stepping;
  //! @brief Number of processors or cores.
  uint32_t numberOfProcessors;
  //! @brief Cpu features bitfield, see @c AsmJit::CpuInfo::FEATURE enum).
  uint32_t features;
  //! @brief Cpu bugs bitfield, see @c AsmJit::CpuInfo::BUG enum).
  uint32_t bugs;

  //! @brief CPU features.
  enum FEATURE
  {
    // [X86, X64]

    //! @brief Cpu has RDTSC instruction.
    FEATURE_RDTSC = 1U << 0,
    //! @brief Cpu has RDTSCP instruction.
    FEATURE_RDTSCP = 1U << 1,
    //! @brief Cpu has CMOV instruction (conditional move)
    FEATURE_CMOV = 1U << 2,
    //! @brief Cpu has CMPXCHG8B instruction
    FEATURE_CMPXCHG8B = 1U << 3,
    //! @brief Cpu has CMPXCHG16B instruction (64 bit processors)
    FEATURE_CMPXCHG16B = 1U << 4,
    //! @brief Cpu has CLFUSH instruction
    FEATURE_CLFLUSH = 1U << 5,
    //! @brief Cpu has PREFETCH instruction
    FEATURE_PREFETCH = 1U << 6,
    //! @brief Cpu supports LAHF and SAHF instrictions.
    FEATURE_LAHF_SAHF = 1U << 7,
    //! @brief Cpu supports FXSAVE and FXRSTOR instructions.
    FEATURE_FXSR = 1U << 8,
    //! @brief Cpu supports FXSAVE and FXRSTOR instruction optimizations (FFXSR).
    FEATURE_FFXSR = 1U << 9,

    //! @brief Cpu has MMX.
    FEATURE_MMX = 1U << 10,
    //! @brief Cpu has extended MMX.
    FEATURE_MMXExt = 1U << 11,
    //! @brief Cpu has 3dNow!
    FEATURE_3dNow = 1U << 12,
    //! @brief Cpu has enchanced 3dNow!
    FEATURE_3dNowExt = 1U << 13,
    //! @brief Cpu has SSE.
    FEATURE_SSE = 1U << 14,
    //! @brief Cpu has Misaligned SSE (MSSE).
    FEATURE_MSSE = 1U << 15,
    //! @brief Cpu has SSE2.
    FEATURE_SSE2 = 1U << 16,
    //! @brief Cpu has SSE3.
    FEATURE_SSE3 = 1U << 17,
    //! @brief Cpu has Supplemental SSE3 (SSSE3).
    FEATURE_SSSE3 = 1U << 18,
    //! @brief Cpu has SSE4.A.
    FEATURE_SSE4_A = 1U << 19,
    //! @brief Cpu has SSE4.1.
    FEATURE_SSE4_1 = 1U << 20,
    //! @brief Cpu has SSE4.2.
    FEATURE_SSE4_2 = 1U << 21,
    //! @brief Cpu has SSE5.
    FEATURE_SSE5 = 1U << 22,
    //! @brief Cpu supports MONITOR and MWAIT instructions.
    FEATURE_MotitorMWait = 1U << 23,
    //! @brief Cpu supports POPCNT instruction.
    FEATURE_POPCNT = 1U << 24,
    //! @brief Cpu supports LZCNT instruction.
    FEATURE_LZCNT  = 1U << 25,
    //! @brief Cpu supports multithreading.
    FEATURE_MultiThreading = 1U << 29,
    //! @brief Cpu supports execute disable bit (execute protection).
    FEATURE_ExecuteDisableBit = 1U << 30,
    //! @brief Cpu supports 64 bits.
    FEATURE_64Bit = 1U << 31
  };

  enum BUG
  {
    BUG_AMD_LOCK_MB = 1U << 0
  };

  FOG_INLINE bool hasFeature(uint32_t feature)
  { return (features & feature) != 0; }

  FOG_INLINE bool hasFeatures(uint32_t features)
  { return (features & features) == features; }

  FOG_INLINE bool hasBug(uint32_t bug)
  { return (bugs & bug) != 0; }
};

FOG_CVAR_EXTERN CpuInfo* cpuInfo;

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
//! @brief Retrieve CPUID values.
FOG_API void cpuid(uint32_t in, CpuId* out);
#endif // FOG_ARCH_X86) || FOG_ARCH_X86_64

//! @brief Detect CPU info to @a i.
FOG_API void detectCpuInfo(CpuInfo* i);

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_CPUINFO_H
