// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_HASHUTIL_H
#define _FOG_CORE_HASHUTIL_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Char.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/List.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/SequenceInfo.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/Std.h>
#include <Fog/Core/TypeInfo.h>

namespace Fog {
namespace HashUtil {

//! @addtogroup Fog_Core_Essentials
//! @{

// ============================================================================
// [Fog::HashUtil::hashData/hashString]
// ============================================================================

//! @brief Hash data.
//!
//! @param data Pointer to data sequence.
//! @param size Size of data. You cant use DETECT_LENGTH here.
//!
//! @note @c hashData() and @c hashString() methods will result in different
//! hashes, also the speed of hashString() is maximized.
FOG_API uint32_t hashData(const void* data, sysuint_t size);

// STANDARD hash functions for 8 bit and 16 bit NULL terminated strings.
// 8 bit and 16 bit hashes are compatible between LATIN1 and UTF16 strings.
//
// BIG NOTE: hashString() and hashData() functions are INCOMPATIBLE!

//! @brief Hash 8-bit string.
//!
//! @param key 8-bit string to hash.
//! @param length Length of string or DETECT_LENGTH to detect it.
//!
//! @note Hashing LATIN1 and UTF16 strings produces equal hash results.
//!
//! @note @c hashData() and @c hashString() methods will result in different
//! hashes, also the speed of hashString() is maximized.
FOG_API uint32_t hashString(const char* key, sysuint_t length);

//! @brief Hash 16-bit string.
//!
//! @param key 16-bit string to hash.
//! @param length Length of string or DETECT_LENGTH to detect it.
//!
//! @note Hashing LATIN1 and UTF16 strings produces equal hash results.
//!
//! @note @c hashData() and @c hashString() methods will result in different
//! hashes, also the speed of hashString() is maximized.
FOG_API uint32_t hashString(const Char* key, sysuint_t length);

//! @brief Combine two hash values into one.
static FOG_INLINE uint32_t combineHash(uint32_t hash0, uint32_t hash1)
{
 return hash0 + hash1;
}

//! @brief Combine three hash values into one.
static FOG_INLINE uint32_t combineHash(uint32_t h0, uint32_t h1, uint32_t h2)
{
 return h0 + h1 + h2;
}

//! @brief Combine four hash values into one.
static FOG_INLINE uint32_t combineHash(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3)
{
 return h0 + h1 + h2 + h3;
}

//! @brief Combine five hash values into one.
static FOG_INLINE uint32_t combineHash(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4)
{
 return h0 + h1 + h2 + h3 + h4;
}

//! @brief Combine six hash values into one.
static FOG_INLINE uint32_t combineHash(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4, uint32_t h5)
{
 return h0 + h1 + h2 + h3 + h4 + h5;
}

//! @brief Combine seven hash values into one.
static FOG_INLINE uint32_t combineHash(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4, uint32_t h5, uint32_t h6)
{
 return h0 + h1 + h2 + h3 + h4 + h5 + h6;
}

//! @brief Combine eight hash values into one.
static FOG_INLINE uint32_t combineHash(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4, uint32_t h5, uint32_t h6, uint32_t h7)
{
 return h0 + h1 + h2 + h3 + h4 + h5 + h6 + h7;
}

// ============================================================================
// [Fog::getHashCode]
// ============================================================================

// Default action is to call getHashCode() method.
template<typename T>
FOG_INLINE uint32_t getHashCode(const T& key) { return key.getHashCode(); }

#define FOG_DECLARE_HASHABLE(__type__, __execute__) \
template<> \
FOG_INLINE uint32_t getHashCode<__type__>(const __type__& key) __execute__

#define FOG_DECLARE_HASHABLE_PTR(__type__, __execute__) \
template<> \
FOG_INLINE uint32_t getHashCode<__type__ &>(__type__& key) __execute__ \
\
template<> \
FOG_INLINE uint32_t getHashCode<const __type__ &>(const __type__& key) __execute__

FOG_DECLARE_HASHABLE(char, { return (uint8_t)key; })
FOG_DECLARE_HASHABLE(uint8_t, { return (uint8_t)key; })
FOG_DECLARE_HASHABLE(int16_t, { return (uint16_t)key; })
FOG_DECLARE_HASHABLE(uint16_t, { return (uint16_t)key; })
FOG_DECLARE_HASHABLE(int32_t, { return (uint32_t)key; })
FOG_DECLARE_HASHABLE(uint32_t, { return (uint32_t)key; })
FOG_DECLARE_HASHABLE(int64_t, { return ((uint32_t)((uint64_t)(key) >> 31)) ^ (uint32_t)(key); })
FOG_DECLARE_HASHABLE(uint64_t, { return ((uint32_t)((uint64_t)(key) >> 31)) ^ (uint32_t)(key); })

FOG_DECLARE_HASHABLE(float, { Math::FloatAndInt32 data; data.f = key; return getHashCode(data.i32); })
FOG_DECLARE_HASHABLE(double, { Math::DoubleAndInt64 data; data.d = key; return getHashCode(data.i64); })

#if FOG_ARCH_BITS == 32
template<typename T>
FOG_INLINE uint32_t getHashCode(T* key) { return (uint32_t)key; }
#else
template<typename T>
FOG_INLINE uint32_t getHashCode(T* key) { return ((uint32_t)((uint64_t)(key) >> 31)) ^ (uint32_t)(uint64_t)(key); }
#endif // FOG_ARCH_BITS

//! @}

} // HashUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_HASHUTIL_H
