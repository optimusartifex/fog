// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPU_CPU_H
#define _FOG_CORE_CPU_CPU_H

// [Dependencies]
#include <Fog/Core/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Core_Cpu
//! @{

// ============================================================================
// [Fog::CPU_CACHE_LINE]
// ============================================================================

enum { CPU_CACHE_LINE = 64 };

// ============================================================================
// [Fog::CpuId]
// ============================================================================

union FOG_NO_EXPORT CpuId
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
// [Fog::Cpu]
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
//! if (Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_SSE2))
//! {
//!   // Cpu has SSE2 support.
//! }
//! else
//! {
//!   // Cpu hasn't SSE2 support.
//! }
//! @endverbatim
struct FOG_NO_EXPORT Cpu
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
  //! @brief Cpu features bitfield, see @c CPU_FEATURE enum).
  uint32_t features;
  //! @brief Cpu bugs bitfield, see @c CPU_BUG enum).
  uint32_t bugs;

  FOG_INLINE bool hasFeature(uint32_t feature) const { return (this->features & feature) != 0; }
  FOG_INLINE bool hasFeatures(uint32_t features) const { return (this->features & features) == features; }
  FOG_INLINE bool hasBug(uint32_t bug) const { return (this->bugs & bug) != 0; }

  static FOG_INLINE const Cpu* get();
};

extern FOG_API Cpu _core_cpu;
FOG_INLINE const Cpu* Cpu::get() { return &_core_cpu; }

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
//! @brief Retrieve CPUID values.
FOG_API void cpuid(uint32_t in, CpuId* out);
#endif // FOG_ARCH_X86) || FOG_ARCH_X86_64

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_CPU_CPU_H
