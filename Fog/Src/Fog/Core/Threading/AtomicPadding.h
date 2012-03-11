// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_THREADING_ATOMICPADDING_H
#define _FOG_CORE_THREADING_ATOMICPADDING_H

// [Dependencies]
#include <Fog/Core/Tools/Cpu.h>

namespace Fog {

//! @addtogroup Fog_Core_Threading
//! @{

// ============================================================================
// [Fog::AtomicPaddingMemory<>]
// ============================================================================

//! @internal
//!
//! @brief Low-level atomic padding structure.
template<size_t Size>
struct AtomicPaddingMemory
{
  uint8_t padding[Size];
};

//! @internal
//!
//! @brief Low-level zero-bytes atomic padding.
template<>
struct AtomicPaddingMemory<0>
{
};

// ============================================================================
// [Fog::AtomicPadding<>]
// ============================================================================

//! @brief Atomic padding buffer.
//!
//! The padding buffer is required to avoid false sharing between CPU cores'
//! private cache.
template<typename T0>
struct AtomicPadding1 : public AtomicPaddingMemory
  < (CPU_CACHE_LINE - ((sizeof(T0)) % CPU_CACHE_LINE)) % CPU_CACHE_LINE >
{
};

//! @brief See @ref AtomicPadding1.
template<typename T0, typename T1>
struct AtomicPadding2 : public AtomicPaddingMemory
  < (CPU_CACHE_LINE - ((sizeof(T0) + sizeof(T1)) % CPU_CACHE_LINE)) % CPU_CACHE_LINE >
{
};

//! @brief See @ref AtomicPadding1.
template<typename T0, typename T1, typename T2>
struct AtomicPadding3 : public AtomicPaddingMemory
  < (CPU_CACHE_LINE - ((sizeof(T0) + sizeof(T1) + sizeof(T2)) % CPU_CACHE_LINE)) % CPU_CACHE_LINE >
{
};

//! @brief See @ref AtomicPadding1.
template<typename T0, typename T1, typename T2, typename T3>
struct AtomicPadding4 : public AtomicPaddingMemory
  < (CPU_CACHE_LINE - ((sizeof(T0) + sizeof(T1) + sizeof(T2) + sizeof(T3)) % CPU_CACHE_LINE)) % CPU_CACHE_LINE >
{
};

// ============================================================================
// [Fog::AtomicPaddingSeparator]
// ============================================================================

//! @brief Atomic padding separator.
//!
//! Atomic padding separator is structure which adds @ref CPU_CACHE_LINE bytes
//! to the place where it's used. It's used mainly to avoid sharing data 
//! between more CPU cores.
struct AtomicPaddingSeparator : public AtomicPaddingMemory<CPU_CACHE_LINE> {};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_THREADING_ATOMICPADDING_H
