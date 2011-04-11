// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_THREADING_ATOMICPADDING_H
#define _FOG_CORE_THREADING_ATOMICPADDING_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Cpu/Cpu.h>

namespace Fog {

//! @addtogroup Fog_Core_Atomic
//! @{

// ============================================================================
// [Fog::AtomicPaddingMemory<>]
// ============================================================================

template<sysuint_t Size>
struct AtomicPaddingMemory
{
  uint8_t padding[Size];
};

template<>
struct AtomicPaddingMemory<0>
{
};

// ============================================================================
// [Fog::AtomicPadding<>]
// ============================================================================

//! @brief The padding buffer.
//!
//! The padding buffer is required to avoid false shading between CPU cores'
//! private cache.
template<typename T0>
struct AtomicPadding1 : public AtomicPaddingMemory
  <CPU_CACHE_LINE - (sizeof(T0) % CPU_CACHE_LINE)>
{
};

template<typename T0, typename T1>
struct AtomicPadding2 : public AtomicPaddingMemory
  <CPU_CACHE_LINE - ((sizeof(T0) + sizeof(T1)) % CPU_CACHE_LINE)>
{
};

template<typename T0, typename T1, typename T2>
struct AtomicPadding3 : public AtomicPaddingMemory
  <CPU_CACHE_LINE - ((sizeof(T0) + sizeof(T1) + sizeof(T2)) % CPU_CACHE_LINE)>
{
};

template<typename T0, typename T1, typename T2, typename T3>
struct AtomicPadding4 : public AtomicPaddingMemory
  <CPU_CACHE_LINE - ((sizeof(T0) + sizeof(T1) + sizeof(T2) + sizeof(T3)) % CPU_CACHE_LINE)>
{
};

// ============================================================================
// [Fog::AtomicPaddingSeparator]
// ============================================================================

struct AtomicPaddingSeparator : public AtomicPaddingMemory
  <CPU_CACHE_LINE>
{
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADING_ATOMICPADDING_H
