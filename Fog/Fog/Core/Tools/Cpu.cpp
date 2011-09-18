// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Cpu.h>

// [Dependencies - Windows]
#if defined(_MSC_VER) && _MSC_VER >= 1400
# include <intrin.h>
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

// ============================================================================
// [Fog::Cpu - Global]
// ============================================================================

static Static<Cpu> Cpu_instance;

// ============================================================================
// [Fog::Cpu - Number Of Processors]
// ============================================================================

static uint32_t Cpu_detectNumberOfProcessors(void)
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
    return 1;

  return static_cast<uint32_t>(res);
#else
  return 1;
#endif
}

// ============================================================================
// [Fog::Cpu - CPUID]
// ============================================================================

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)

union FOG_NO_EXPORT CpuId
{
  struct
  {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
  };

  uint32_t i[4];
};

#if defined(FOG_CC_MSC)
static void FOG_CDECL Cpu_cpuid(uint32_t in, CpuId* out)
{
#if _MSC_VER >= 1400
  // Done by intrinsics.
  __cpuid(reinterpret_cast<int*>(out->i), in);
#else // _MSC_VER < 1400
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
#endif // _MSC_VER < 1400
}
#endif // FOG_CC_MSC

#if defined(FOG_CC_GNU)
static void FOG_CDECL Cpu_cpuid(uint32_t in, CpuId* out)
{
// When using GCC inline assembly it's needed to preserve EBX or RBX register.
#if defined(FOG_ARCH_X86)
#define _Cpuid(a, b, c, d, inp) \
  asm("mov %%ebx, %%edi\n"    \
      "cpuid\n"               \
      "xchg %%edi, %%ebx\n"   \
      : "=a" (a), "=D" (b), "=c" (c), "=d" (d) : "a" (inp))
#else
#define _Cpuid(a, b, c, d, inp) \
  asm("mov %%rbx, %%rdi\n"    \
      "cpuid\n"               \
      "xchg %%rdi, %%rbx\n"   \
      : "=a" (a), "=D" (b), "=c" (c), "=d" (d) : "a" (inp))
#endif
  _Cpuid(out->eax, out->ebx, out->ecx, out->edx, in);
}
#endif // FOG_CC_GNU

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

static void Cpu_detect(Cpu* cpu)
{
  uint32_t a;
  uint32_t features = NO_FLAGS;

  // Reset.
  memset(cpu, 0, sizeof(Cpu));
  memcpy(cpu->_vendor, "Unknown", 8);

  cpu->_numberOfProcessors = Cpu_detectNumberOfProcessors();

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
  CpuId out;

  // Get vendor string.
  Cpu_cpuid(0, &out);

  reinterpret_cast<uint32_t*>(cpu->_vendor)[0] = out.ebx;
  reinterpret_cast<uint32_t*>(cpu->_vendor)[1] = out.edx;
  reinterpret_cast<uint32_t*>(cpu->_vendor)[2] = out.ecx;

  for (a = 0; a < FOG_ARRAY_SIZE(cpuVendorInfo); a++)
  {
    if (cpuVencorEq(cpuVendorInfo[a], cpu->_vendor))
    {
      cpu->_vendorId = cpuVendorInfo[a].id;
      break;
    }
  }

  // Get feature flags in ECX/EDX, and family/model in EAX.
  Cpu_cpuid(1, &out);

  // Family and model fields.
  cpu->_family   = (out.eax >> 8) & 0x0F;
  cpu->_model    = (out.eax >> 4) & 0x0F;
  cpu->_stepping = (out.eax     ) & 0x0F;

  // Use extended family and model fields.
  if (cpu->_family == 0x0F)
  {
    cpu->_family += ((out.eax >> 20) & 0xFF);
    cpu->_model  += ((out.eax >> 16) & 0x0F) << 4;
  }

  if (out.ecx & 0x00000001U) features |= CPU_FEATURE_SSE3;
  if (out.ecx & 0x00000002U) features |= CPU_FEATURE_PCLMULDQ;
  if (out.ecx & 0x00000008U) features |= CPU_FEATURE_MONITOR_MWAIT;
  if (out.ecx & 0x00000200U) features |= CPU_FEATURE_SSSE3;
  if (out.ecx & 0x00002000U) features |= CPU_FEATURE_CMPXCHG16B;
  if (out.ecx & 0x00080000U) features |= CPU_FEATURE_SSE4_1;
  if (out.ecx & 0x00100000U) features |= CPU_FEATURE_SSE4_2;
  if (out.ecx & 0x00400000U) features |= CPU_FEATURE_MOVBE;
  if (out.ecx & 0x00800000U) features |= CPU_FEATURE_POPCNT;
  if (out.ecx & 0x10000000U) features |= CPU_FEATURE_AVX;

  if (out.edx & 0x00000010U) features |= CPU_FEATURE_RDTSC;
  if (out.edx & 0x00000100U) features |= CPU_FEATURE_CMPXCHG8B;
  if (out.edx & 0x00008000U) features |= CPU_FEATURE_CMOV;
  if (out.edx & 0x00800000U) features |= CPU_FEATURE_MMX;
  if (out.edx & 0x01000000U) features |= CPU_FEATURE_FXSR;
  if (out.edx & 0x02000000U) features |= CPU_FEATURE_SSE | CPU_FEATURE_MMX_EXT;
  if (out.edx & 0x04000000U) features |= CPU_FEATURE_SSE | CPU_FEATURE_SSE2;
  if (out.edx & 0x10000000U) features |= CPU_FEATURE_MULTITHREADING;

  if (cpu->_vendorId == CPU_VENDOR_AMD && (out.edx & 0x10000000U))
  {
    // AMD sets Multithreading to ON if it has more CPU cores.
    if (cpu->_numberOfProcessors == 1) cpu->_numberOfProcessors = 2;
  }

  // AMD-Opteron Rev.E lock-bug detection.
  if (cpu->_vendorId == CPU_VENDOR_AMD &&
      cpu->_family == 15 &&
      cpu->_model >= 32 &&
      cpu->_model <= 63)
  {
    cpu->_bugs |= CPU_BUG_AMD_LOCK_MB;
  }

  // Calling cpuid with 0x80000000 as the in argument gets the number of valid
  // extended IDs.
  Cpu_cpuid(0x80000000, &out);
  uint32_t exIds = Math::min(out.eax, 0x80000004);
  uint32_t* brand = reinterpret_cast<uint32_t*>(cpu->_brand);

  for (a = 0x80000001; a <= exIds; a++)
  {
    Cpu_cpuid(a, &out);

    switch (a)
    {
      case 0x80000001:
        if (out.ecx & 0x00000001U) features |= CPU_FEATURE_LAHF_SAHF;
        if (out.ecx & 0x00000020U) features |= CPU_FEATURE_LZCNT;
        if (out.ecx & 0x00000040U) features |= CPU_FEATURE_SSE4_A;
        if (out.ecx & 0x00000080U) features |= CPU_FEATURE_MSSE;
        if (out.ecx & 0x00000100U) features |= CPU_FEATURE_PREFETCH;

        if (out.edx & 0x00100000U) features |= CPU_FEATURE_EXECUTE_DISABLE_BIT;
        if (out.edx & 0x00200000U) features |= CPU_FEATURE_FFXSR;
        if (out.edx & 0x00400000U) features |= CPU_FEATURE_MMX_EXT;
        if (out.edx & 0x08000000U) features |= CPU_FEATURE_RDTSCP;
        if (out.edx & 0x20000000U) features |= CPU_FEATURE_64_BIT;
        if (out.edx & 0x40000000U) features |= CPU_FEATURE_3DNOW_EXT | CPU_FEATURE_MMX_EXT;
        if (out.edx & 0x80000000U) features |= CPU_FEATURE_3DNOW;
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

  // Finished...
  cpu->_features = features;
  simplifyBrandString(cpu->_brand);
#endif // FOG_ARCH_X86 || FOG_ARCH_X64
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Cpu_init(void)
{
  _api.cpu.instance = &Cpu_instance;
  Cpu_detect(_api.cpu.instance);
}

} // Fog namespace
