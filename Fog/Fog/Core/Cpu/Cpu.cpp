// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Cpu/Cpu.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Math/Math.h>

#if defined(FOG_OS_WINDOWS)
# include <windows.h>
#endif // FOG_OS_WINDOWS

// 2009-02-05: Thanks to Mike Tajmajer for supporting VC7.1 compiler. This
// shouldn't affect x64 compilation, because x64 compiler starts with
// VS2005 (VC8.0).
#if defined(_MSC_VER)
# if _MSC_VER >= 1400
#  include <intrin.h>
# endif // _MSC_VER >= 1400 (>= VS2005)
#endif // _MSC_VER

#if defined(FOG_OS_POSIX)
# include <errno.h>
# include <string.h>
# include <sys/statvfs.h>
# include <sys/utsname.h>
# include <unistd.h>
#endif // FOG_OS_POSIX

namespace Fog {

// ============================================================================
// [Fog::Cpu]
// ============================================================================

Cpu _core_cpu;

static uint32_t detectNumberOfProcessors(void)
{
#if defined(FOG_OS_WINDOWS)
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return info.dwNumberOfProcessors;
#elif defined(FOG_OS_POSIX)
  // It seems that sysconf returns the number of "logical" processors on both
  // mac and linux.  So we get the number of "online logical" processors.
  long res = sysconf(_SC_NPROCESSORS_ONLN);
  if (res == -1)
  {
    FOG_ASSERT(0);
    return 1;
  }

  return static_cast<uint32_t>(res);
#else
  return 1;
#endif
}

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
void cpuid(uint32_t in, CpuId* out)
{
#if defined(_MSC_VER)
# if _MSC_VER >= 1400
  // done by intrinsics
  __cpuid(reinterpret_cast<int*>(out->i), in);
# else // _MSC_VER < 1400
  uint32_t cpuid_in = in;
  uint32_t* cpuid_out = out->i;

  __asm
  {
    mov     eax, cpuid_in
    mov     edi, cpuid_out
    cpuid
    mov     dword ptr[edi +  0], eax
    mov     dword ptr[edi +  4], ebx
    mov     dword ptr[edi +  8], ecx
    mov     dword ptr[edi + 12], edx
  }
# endif // _MSC_VER < 1400

#elif defined(__GNUC__)

// Note, need to preserve ebx/rbx register!
# if defined(FOG_ARCH_X86)
#  define __mycpuid(a, b, c, d, inp) \
  asm ("mov %%ebx, %%edi\n"    \
       "cpuid\n"               \
       "xchg %%edi, %%ebx\n"   \
       : "=a" (a), "=D" (b), "=c" (c), "=d" (d) : "a" (inp))
# else
#  define __mycpuid(a, b, c, d, inp) \
  asm ("mov %%rbx, %%rdi\n"    \
       "cpuid\n"               \
       "xchg %%rdi, %%rbx\n"   \
       : "=a" (a), "=D" (b), "=c" (c), "=d" (d) : "a" (inp))
# endif
  __mycpuid(out->eax, out->ebx, out->ecx, out->edx, in);

#endif // compiler
}
#endif // FOG_ARCH_X86) || FOG_ARCH_X86_64

static void detectCpu(Cpu* i)
{
  uint32_t a;

  // First clear our struct
  memset(i, 0, sizeof(Cpu));
  memcpy(i->vendor, "Unknown", 8);

  i->numberOfProcessors = detectNumberOfProcessors();

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
  CpuId out;

  // Get vendor string
  cpuid(0, &out);

  memcpy(i->vendor, &out.ebx, 4);
  memcpy(i->vendor + 4, &out.edx, 4);
  memcpy(i->vendor + 8, &out.ecx, 4);

  // get feature flags in ecx/edx, and family/model in eax
  cpuid(1, &out);

  // family and model fields
  i->family   = (out.eax >> 8) & 0x0F;
  i->model    = (out.eax >> 4) & 0x0F;
  i->stepping = (out.eax     ) & 0x0F;

  // use extended family and model fields
  if (i->family == 0x0F)
  {
    i->family += ((out.eax >> 20) & 0xFF);
    i->model  += ((out.eax >> 16) & 0x0F) << 4;
  }

  if (out.ecx & 0x00000001U) i->features |= CPU_FEATURE_SSE3;
  if (out.ecx & 0x00000008U) i->features |= CPU_FEATURE_MONITOR_MWAIT;
  if (out.ecx & 0x00000200U) i->features |= CPU_FEATURE_SSSE3;
  if (out.ecx & 0x00002000U) i->features |= CPU_FEATURE_CMPXCHG16B;
  if (out.ecx & 0x00080000U) i->features |= CPU_FEATURE_SSE4_1;
  if (out.ecx & 0x00100000U) i->features |= CPU_FEATURE_SSE4_2;
  if (out.ecx & 0x00800000U) i->features |= CPU_FEATURE_POPCNT;

  if (out.edx & 0x00000010U) i->features |= CPU_FEATURE_RDTSC;
  if (out.edx & 0x00000100U) i->features |= CPU_FEATURE_CMPXCHG8B;
  if (out.edx & 0x00008000U) i->features |= CPU_FEATURE_CMOV;
  if (out.edx & 0x00800000U) i->features |= CPU_FEATURE_MMX;
  if (out.edx & 0x01000000U) i->features |= CPU_FEATURE_FXSR;
  if (out.edx & 0x02000000U) i->features |= CPU_FEATURE_SSE | CPU_FEATURE_MMX_EXT;
  if (out.edx & 0x04000000U) i->features |= CPU_FEATURE_SSE | CPU_FEATURE_SSE2;
  if (out.edx & 0x10000000U) i->features |= CPU_FEATURE_MULTITHREADING;

  if (strcmp(i->vendor, "AuthenticAMD") == 0 && 
      (out.edx & 0x10000000U))
  {
    // AMD sets Multithreading to ON if it has more cores.
    if (i->numberOfProcessors == 1) i->numberOfProcessors = 2;
  }

  // This comment comes from V8 and I think that its important:
  //
  // Opteron Rev E has a bug in which on very rare occasions a locked
  // instruction doesn't act as a read-acquire barrier if followed by a
  // non-locked read-modify-write instruction.  Rev F has this bug in 
  // pre-release versions, but not in versions released to customers,
  // so we test only for Rev E, which is family 15, model 32..63 inclusive.

  if (strcmp(i->vendor, "AuthenticAMD") == 0 && 
      i->family == 15 && i->model >= 32 && i->model <= 63) 
  {
    i->bugs |= CPU_BUG_AMD_LOCK_MB;
  }

  // Calling cpuid with 0x80000000 as the in argument
  // gets the number of valid extended IDs.

  cpuid(0x80000000, &out);
  uint32_t exIds = Math::min(out.eax, 0x80000004);
  uint32_t* brand = reinterpret_cast<uint32_t*>(i->brand);

  for (a = 0x80000001; a <= exIds; a++)
  {
    cpuid(a, &out);

    switch (a)
    {
      case 0x80000001:
        if (out.ecx & 0x00000001U) i->features |= CPU_FEATURE_LAHF_SAHF;
        if (out.ecx & 0x00000020U) i->features |= CPU_FEATURE_LZCNT;
        if (out.ecx & 0x00000040U) i->features |= CPU_FEATURE_SSE4_A;
        if (out.ecx & 0x00000080U) i->features |= CPU_FEATURE_MSSE;
        if (out.ecx & 0x00000100U) i->features |= CPU_FEATURE_PREFETCH;
        if (out.ecx & 0x00000800U) i->features |= CPU_FEATURE_SSE5;

        if (out.edx & 0x00100000U) i->features |= CPU_FEATURE_EXECUTE_DISABLE_BIT;
        if (out.edx & 0x00200000U) i->features |= CPU_FEATURE_FFXSR;
        if (out.edx & 0x00400000U) i->features |= CPU_FEATURE_MMX_EXT;
        if (out.edx & 0x08000000U) i->features |= CPU_FEATURE_RDTSCP;
        if (out.edx & 0x20000000U) i->features |= CPU_FEATURE_64_BIT;
        if (out.edx & 0x40000000U) i->features |= CPU_FEATURE_3DNOW_EXT | CPU_FEATURE_MMX_EXT;
        if (out.edx & 0x80000000U) i->features |= CPU_FEATURE_3DNOW;
        break;

      case 0x80000002:
      case 0x80000003:
      case 0x80000004:
        *brand++ = out.eax;
        *brand++ = out.ebx;
        *brand++ = out.ecx;
        *brand++ = out.edx;
        break;

      // there are more information that can be implemented in the future
    }
  }

  if (exIds < 0x80000002)
  {
    // Set brand string to vendor if it can't be read.
    memcpy(i->brand, i->vendor, 16);
  }
  else
  {
    // Remove extra spaces from brand string.
    char* bDst = i->brand;
    char* bSrc = i->brand;
    char c;

    do {
      c = *bSrc++;
      *bDst++ = c;
      if (c == ' ')
      {
        while ((c = *bSrc) == ' ') bSrc++;
      }
    } while (c != '\0');
    *bDst = '\0';
  }
#endif // FOG_ARCH_X86 || FOG_ARCH_X64
}

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_cpu_init(void)
{
  detectCpu(&_core_cpu);
}

} // Fog namespace
