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

// [Dependencies - Windows]
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

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <errno.h>
# include <string.h>
# include <sys/statvfs.h>
# include <sys/utsname.h>
# include <unistd.h>
#endif // FOG_OS_POSIX

namespace Fog {

Cpu _core_cpu;

// ============================================================================
// [Fog::Cpu - Number Of Processors]
// ============================================================================

static uint32_t _Cpu_detectNumberOfProcessors(void)
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

// ============================================================================
// [Fog::Cpu - CPUID]
// ============================================================================

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

// ============================================================================
// [Fog::Cpu - Vendors]
// ============================================================================

struct CpuVendorInfo
{
  uint32_t id;
  char text[12];
};

static const CpuVendorInfo cpuVendorInfo[] = 
{
  { CPU_VENDOR_INTEL    , { 'G', 'e', 'n', 'u', 'i', 'n', 'e', 'I', 'n', 't', 'e', 'l' } },

  { CPU_VENDOR_AMD      , { 'A', 'u', 't', 'h', 'e', 'n', 't', 'i', 'c', 'A', 'M', 'D' } },
  { CPU_VENDOR_AMD      , { 'A', 'M', 'D', 'i', 's', 'b', 'e', 't', 't', 'e', 'r', '!' } },

  { CPU_VENDOR_NSM      , { 'G', 'e', 'o', 'd', 'e', ' ', 'b', 'y', ' ', 'N', 'S', 'C' } },
  { CPU_VENDOR_NSM      , { 'C', 'y', 'r', 'i', 'x', 'I', 'n', 's', 't', 'e', 'a', 'd' } },

  { CPU_VENDOR_TRANSMETA, { 'G', 'e', 'n', 'u', 'i', 'n', 'e', 'T', 'M', 'x', '8', '6' } },
  { CPU_VENDOR_TRANSMETA, { 'T', 'r', 'a', 'n', 's', 'm', 'e', 't', 'a', 'C', 'P', 'U' } },

  { CPU_VENDOR_VIA      , { 'V', 'I', 'A',  0 , 'V', 'I', 'A',  0 , 'V', 'I', 'A',  0  } },
  { CPU_VENDOR_VIA      , { 'C', 'e', 'n', 't', 'a', 'u', 'r', 'H', 'a', 'u', 'l', 's' } }
};

static FOG_INLINE bool cpuVencorEq(const CpuVendorInfo& info, const char* vendorString)
{
  const uint32_t* a = reinterpret_cast<const uint32_t*>(info.text);
  const uint32_t* b = reinterpret_cast<const uint32_t*>(vendorString);

  return (a[0] == b[0]) &
         (a[1] == b[1]) &
         (a[2] == b[2]) ;
}

static FOG_INLINE void simplifyBrandString(char* s)
{
  // Always clear the current character in the buffer. This ensures that there
  // is no garbage after the string NULL terminator.
  char* d = s;

  char prev = 0;
  char curr = s[0];
  s[0] = '\0';

  for (;;)
  {
    if (curr == 0) break;

    if (curr == ' ')
    {
      if (prev == '@') goto _Skip;
      if (s[1] == ' ' || s[1] == '@') goto _Skip;
    }

    d[0] = curr;
    d++;
    prev = curr;

_Skip:
    curr = *++s;
    s[0] = '\0';
  }

  d[0] = '\0';
}

// ============================================================================
// [Fog::Cpu - Detect]
// ============================================================================

static void _Cpu_detectCpu(Cpu* i)
{
  uint32_t a;

  // First clear our struct
  memset(i, 0, sizeof(Cpu));
  memcpy(i->vendor, "Unknown", 8);

  i->numberOfProcessors = _Cpu_detectNumberOfProcessors();

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
  CpuId out;

  // Get vendor string
  cpuid(0, &out);

  memcpy(i->vendor, &out.ebx, 4);
  memcpy(i->vendor + 4, &out.edx, 4);
  memcpy(i->vendor + 8, &out.ecx, 4);

  for (a = 0; a < 3; a++)
  {
    if (cpuVencorEq(cpuVendorInfo[a], i->vendor))
    {
      i->vendorId = cpuVendorInfo[a].id;
      break;
    }
  }

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
  if (out.ecx & 0x00000002U) i->features |= CPU_FEATURE_PCLMULDQ;
  if (out.ecx & 0x00000008U) i->features |= CPU_FEATURE_MONITOR_MWAIT;
  if (out.ecx & 0x00000200U) i->features |= CPU_FEATURE_SSSE3;
  if (out.ecx & 0x00002000U) i->features |= CPU_FEATURE_CMPXCHG16B;
  if (out.ecx & 0x00080000U) i->features |= CPU_FEATURE_SSE4_1;
  if (out.ecx & 0x00100000U) i->features |= CPU_FEATURE_SSE4_2;
  if (out.ecx & 0x00400000U) i->features |= CPU_FEATURE_MOVBE;
  if (out.ecx & 0x00800000U) i->features |= CPU_FEATURE_POPCNT;
  if (out.ecx & 0x10000000U) i->features |= CPU_FEATURE_AVX;

  if (out.edx & 0x00000010U) i->features |= CPU_FEATURE_RDTSC;
  if (out.edx & 0x00000100U) i->features |= CPU_FEATURE_CMPXCHG8B;
  if (out.edx & 0x00008000U) i->features |= CPU_FEATURE_CMOV;
  if (out.edx & 0x00800000U) i->features |= CPU_FEATURE_MMX;
  if (out.edx & 0x01000000U) i->features |= CPU_FEATURE_FXSR;
  if (out.edx & 0x02000000U) i->features |= CPU_FEATURE_SSE | CPU_FEATURE_MMX_EXT;
  if (out.edx & 0x04000000U) i->features |= CPU_FEATURE_SSE | CPU_FEATURE_SSE2;
  if (out.edx & 0x10000000U) i->features |= CPU_FEATURE_MULTITHREADING;

  if (i->vendorId == CPU_VENDOR_AMD && (out.edx & 0x10000000U))
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

  if (i->vendorId == CPU_VENDOR_AMD && i->family == 15 && i->model >= 32 && i->model <= 63)
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

  // Simplify the brand string (remove unnecessary spaces to make it printable).
  simplifyBrandString(i->brand);
#endif // FOG_ARCH_X86 || FOG_ARCH_X64
}

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_cpu_init(void)
{
  _Cpu_detectCpu(&_core_cpu);
}

} // Fog namespace
