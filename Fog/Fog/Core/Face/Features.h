// [Fog-Face]
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

// #define FOG_FACE_HAS_MMX
// #define FOG_FACE_HAS_SSE
// #define FOG_FACE_HAS_SSE2

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
# if !defined(FOG_FACE_HAS_64BIT)
#  define FOG_FACE_HAS_64BIT
# endif
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

#if defined(FOG_ARCH_X86) || \
    defined(FOG_ARCH_X86_64)
# if !defined(FOG_FACE_HAS_CONDITIONAL_INSTRUCTIONS)
#  define FOG_FACE_HAS_CONDITIONAL_INSTRUCTIONS
# endif
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
# if !defined(FOG_FACE_HAS_FAST_MULTIPLY)
#  define FOG_FACE_HAS_FAST_MULTIPLY
# endif
#endif

// ============================================================================
// [FOG_FACE_HAS_MMX]
// ============================================================================

#if defined(FOG_HARDCODE_MMX)
# define FOG_FACE_HAS_MMX
#endif // FOG_HARDCODE_MMX

// ============================================================================
// [FOG_FACE_HAS_SSE]
// ============================================================================

#if defined(FOG_HARDCODE_SSE)
# define FOG_FACE_HAS_SSE
#endif // FOG_HARDCODE_SSE

// ============================================================================
// [FOG_FACE_HAS_SSE2]
// ============================================================================

#if defined(FOG_HARDCODE_SSE2)
# define FOG_FACE_HAS_SSE2
#endif // FOG_HARDCODE_SSE2

//! @}

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_FACE_FEATURES_H
