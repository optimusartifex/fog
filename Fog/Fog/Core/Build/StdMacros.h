// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_BUILD_STDMACROS_H
#define _FOG_CORE_BUILD_STDMACROS_H

#if !defined(FOG_IDE) && !defined(_FOG_BUILD_BUILD_H)
#error "Fog::Core::Build - StdMacros.h can be only included by Fog/Core/Build.h"
#endif // _FOG_BUILD_BUILD_H

// ============================================================================
// [Fog::Core::Build - Standard Macros]
// ============================================================================

//! @addtogroup Fog_Core_Macros
//! @{

//! @brief Creates @c __selftype__ typedef in class thats for callbacks and references
//!
//! @sa @c FOG_DECLARE_D
#define FOG_DECLARE_SELFTYPE(selftype) \
  typedef selftype __selftype__;

//! @brief Creates @c __dtype__ typedef and @c d member in class
//!
//! @sa @c FOG_DECLARE_SELFTYPE, @c FOG_DECLARE_D_METHODS
#define FOG_DECLARE_D(dtype) \
  typedef dtype __dtype__; \
  \
  __dtype__* _d;

//! @brief Use this macro to generate empty copy constructor and empty copy operator.
//! Use in 'private' section.
#define FOG_DISABLE_COPY(selftype) \
  FOG_INLINE FOG_HIDDEN selftype(const selftype& other); \
  FOG_INLINE FOG_HIDDEN selftype& operator=(const selftype& other);

// Workaround for Borland compiler that don't know about variadic macros
#define FOG_MACRO1(a0) a0
#define FOG_MACRO2(a0, a1) a0, a1
#define FOG_MACRO3(a0, a1, a2) a0, a1, a2
#define FOG_MACRO4(a0, a1, a2, a3) a0, a1, a2, a3
#define FOG_MACRO5(a0, a1, a2, a3, a4) a0, a1, a2, a3, a4
#define FOG_MACRO6(a0, a1, a2, a3, a4, a5) a0, a1, a2, a3, a4, a5
#define FOG_MACRO7(a0, a1, a2, a3, a4, a5, a6) a0, a1, a2, a3, a4, a5, a6
#define FOG_MACRO8(a0, a1, a2, a3, a4, a5, a6, a7) a0, a1, a2, a3, a4, a5, a6, a7
#define FOG_MACRO9(a0, a1, a2, a3, a4, a5, a6, a7, a8) a0, a1, a2, a3, a4, a5, a6, a7, a8

//! @brief Some systems hasn't defined @c offsetof keyword, but FOG_OFFSET_OF
//! is always defined and it's possible to use it to get offset from a class
//! member (only single inheritance is allowed).
#define FOG_OFFSET_OF(struct, field) ((sysuint_t) ((const uchar*) &((const struct*)0x1)->field) - 1)

//! @brief Size of static table. Do not use this macro to get size of dynamic
//! table, you can't get it by this way!!!
//!
//! Use it for example in following way:
//! @code
//! uint table[] = { 1, 2, 3, 4, 5 };
//! uint i;
//!
//! for (i = 0; i != FOG_ARRAY_SIZE(table); i++) {
//!   printf("table[%u]=%u\n", i, table[i]);
//! }
//! @endcode
#define FOG_ARRAY_SIZE(element) ( sizeof(element) / sizeof(element[0]) )

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
#define FOG_MAKE_UINT16_SEQ(byte0, byte1) \
  (uint16_t) ( ((uint16_t)(byte0) <<  0) | \
               ((uint16_t)(byte1) <<  8) )
#define FOG_MAKE_UINT32_SEQ(byte0, byte1, byte2, byte3) \
  (uint32_t) ( ((uint32_t)(byte0) <<  0) | \
               ((uint32_t)(byte1) <<  8) | \
               ((uint32_t)(byte2) << 16) | \
               ((uint32_t)(byte3) << 24) )
#define FOG_MAKE_UINT64_SEQ(byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7) \
  (uint64_t) ( ((uint64_t)(byte0) <<  0) | \
               ((uint64_t)(byte1) <<  8) | \
               ((uint64_t)(byte2) << 16) | \
               ((uint64_t)(byte3) << 24) | \
               ((uint64_t)(byte4) << 32) | \
               ((uint64_t)(byte5) << 40) | \
               ((uint64_t)(byte6) << 48) | \
               ((uint64_t)(byte7) << 56) )
#else
#define FOG_MAKE_UINT16_SEQ(byte0, byte1) \
  (uint16_t) ( ((uint16_t)(byte1) <<  0) | \
               ((uint16_t)(byte0) <<  8) )
#define FOG_MAKE_UINT32_SEQ(byte0, byte1, byte2, byte3) \
  (uint32_t) ( ((uint32_t)(byte3) <<  0) | \
               ((uint32_t)(byte2) <<  8) | \
               ((uint32_t)(byte1) << 16) | \
               ((uint32_t)(byte0) << 24) )
#define FOG_MAKE_UINT64_SEQ(byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7) \
  (uint64_t) ( ((uint64_t)(byte7) <<  0) | \
               ((uint64_t)(byte6) <<  8) | \
               ((uint64_t)(byte5) << 16) | \
               ((uint64_t)(byte4) << 24) | \
               ((uint64_t)(byte3) << 32) | \
               ((uint64_t)(byte2) << 40) | \
               ((uint64_t)(byte1) << 48) | \
               ((uint64_t)(byte0) << 56) )
#endif

#define FOG_RETURN_ON_ERROR(expression) \
  FOG_BEGIN_MACRO \
    err_t __err_inside = (expression); \
    if (FOG_UNLIKELY(__err_inside != ::Fog::ERR_OK)) return __err_inside; \
  FOG_END_MACRO

// ============================================================================
// [Noop]
// ============================================================================

#define FOG_NOP ((void)0)

//! @}

#endif // _FOG_CORE_BUILD_STDMACROS_H
