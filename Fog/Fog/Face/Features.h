// [Fog-Face Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_FACE_FEATURES_H
#define _FOG_FACE_FEATURES_H

// [Dependencies]
#include <Fog/Core/Build.h>

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_Features
//! @{

// ============================================================================
// [TESTING]
// ============================================================================

// #define FOG_FACE_HAS_64BIT
// #define FOG_FACE_HAS_CONDITIONAL_INSTRUCTIONS
// #define FOG_FACE_HAS_FAST_MULTIPLY

// ============================================================================
// [FOG_FACE_HAS_64BIT]
// ============================================================================

//! @def FOG_FACE_HAS_64BIT
//!
//! @brief Whether to use 64-bit instructions in 32-bit BYTE manipulation 
//! functions.
//!
//! This is always enabled whe using 64-bit target platform. The use-case of
//! this macro is that you can test 64-bit BYTE manipulation code (which is
//! using uint64_t) in 32-bit BYTE manipulation functions without having the
//! 64-bit processor.

#if FOG_ARCH_BITS >= 64
#define FOG_FACE_HAS_64BIT
#endif

// ============================================================================
// [FOG_FACE_HAS_CONDITIONAL_INSTRUCTIONS]
// ============================================================================

//! @def FOG_FACE_HAS_CONDITIONAL_INSTRUCTIONS
//!
//! @brief Whether to use conditional instruction (if) instead of bit 
//! manipulation.
//!
//! Enabling this may improve or decrease the performance of code depending on 
//! compiler and target platform. If you are sure that your compiler can 
//! generate good code (using cmov on x86 platform) then it's good to define 
//! it (target code will be smaller and faster).

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
#define FOG_FACE_HAS_CONDITIONAL_INSTRUCTIONS
#endif

// ============================================================================
// [FOG_FACE_HAS_FAST_MULTIPLY]
// ============================================================================

//! @def FOG_FACE_HAS_FAST_MULTIPLY
//!
//! @brief Whether to prefer multiply over bit manipulation.
//!
//! If target platform has fast multiply implementation then multiply will be
//! used instead of two or more bit manipulation instructions.
//!
//! Example of extending BYTE to DWORD without multiply (4 to 6 instructions):
//!
//! @code
//! inline uint32_t extend(uint32_t x)
//! {
//!   x |= (x <<  8);
//!   x |= (x << 16);
//!   return x;
//! }
//! @endcode
//!
//! Example of extending BYTE to DWORD using multiply (1 instruction):
//!
//! @code
//! inline uint32_t extend(uint32_t x)
//! {
//!   return x * 0x01010101U;
//! }
//! @endcode

#if defined(FOG_ARCH_X86) || \
    defined(FOG_ARCH_X86_64) || \
    defined(FOG_ARCH_PPC)
#define FOG_FACE_HAS_FAST_MULTIPLY
#endif

//! @}

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_FACE_FEATURES_H
