// [Fog-Face]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FEATURES_H
#define _FOG_CORE_FACE_FEATURES_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_Features
//! @{

// ============================================================================
// [TESTING]
// ============================================================================

// #define FOG_FACE_P32_IS_TYPE
// #define FOG_FACE_P64_IS_TYPE

// #define FOG_FACE_HAS_64BIT
// #define FOG_FACE_HAS_CMOV
// #define FOG_FACE_HAS_FAST_ADD
// #define FOG_FACE_HAS_FAST_MUL

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

#if FOG_ARCH_BITS >= 64 && !defined(FOG_FACE_HAS_64BIT)
# define FOG_FACE_HAS_64BIT
#endif // FOG_ARCH_BITS >= 64 && !defined(FOG_FACE_HAS_64BIT)

// ============================================================================
// [FOG_FACE_P64_IS_TYPE]
// ============================================================================

#if defined(FOG_FACE_HAS_64BIT)
# define FOG_FACE_P64_IS_TYPE
#endif // FOG_FACE_HAS_64BIT

// ============================================================================
// [FOG_FACE_HAS_CMOV]
// ============================================================================

//! @def FOG_FACE_HAS_CMOV
//!
//! @brief Whether to use conditional instruction (if) instead of bit
//! manipulation.
//!
//! Enabling this may improve or decrease the performance of code depending on
//! compiler and target platform. If you are sure that your compiler can
//! generate good code (using cmov on x86 platform) then it's good to define
//! it (target code will be smaller and faster).

#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
# if !defined(FOG_FACE_HAS_CMOV)
#  define FOG_FACE_HAS_CMOV
# endif
#endif

// ============================================================================
// [FOG_FACE_HAS_FAST_ADD]
// ============================================================================

//! @def FOG_FACE_HAS_FAST_MUL
//!
//! @brief Whether to prefer ADD over OR operator.
#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
# if !defined(FOG_FACE_HAS_FAST_ADD)
#  define FOG_FACE_HAS_FAST_ADD
# endif
#endif

#if defined(FOG_FACE_HAS_FAST_ADD)
# define _FOG_FACE_COMBINE_2(_X0_, _X1_) ((_X0_) + (_X1_))
# define _FOG_FACE_COMBINE_3(_X0_, _X1_, _X2_) ((_X0_) + (_X1_) + (_X2_))
# define _FOG_FACE_COMBINE_4(_X0_, _X1_, _X2_, _X3_) ((_X0_) + (_X1_) + (_X2_) + (_X3_))
# define _FOG_FACE_COMBINE_5(_X0_, _X1_, _X2_, _X3_, _X4_) ((_X0_) + (_X1_) + (_X2_) + (_X3_) + (_X4_))
#else
# define _FOG_FACE_COMBINE_2(_X0_, _X1_) ((_X0_) | (_X1_))
# define _FOG_FACE_COMBINE_3(_X0_, _X1_, _X2_) ((_X0_) | (_X1_) | (_X2_))
# define _FOG_FACE_COMBINE_4(_X0_, _X1_, _X2_, _X3_) ((_X0_) | (_X1_) | (_X2_) | (_X3_))
# define _FOG_FACE_COMBINE_5(_X0_, _X1_, _X2_, _X3_, _X4_) ((_X0_) | (_X1_) | (_X2_) | (_X3_) | (_X4_))
#endif // FOG_FACE_HAS_FAST_ADD

// ============================================================================
// [FOG_FACE_HAS_FAST_MUL]
// ============================================================================

//! @def FOG_FACE_HAS_FAST_MUL
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
# if !defined(FOG_FACE_HAS_FAST_MUL)
#  define FOG_FACE_HAS_FAST_MUL
# endif
#endif

//! @}

} // Face namespace
} // Fog namespace

// #include <Fog/Core/Face/Features_Asm.h>

// [Guard]
#endif // _FOG_CORE_FACE_FEATURES_H
