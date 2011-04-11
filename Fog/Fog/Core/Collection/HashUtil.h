// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_COLLECTION_HASHUTIL_H
#define _FOG_CORE_COLLECTION_HASHUTIL_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Char.h>

namespace Fog {
namespace HashUtil {

//! @addtogroup Fog_Core_Collection
//! @{

// ============================================================================
// [Fog::HashUtil::makeDataHash/makeStringHash]
// ============================================================================

//! @brief Hash data.
//!
//! @param data Pointer to data sequence.
//! @param size Size of data. You cant use DETECT_LENGTH here.
//!
//! @note @c makeDataHash() and @c makeStringHash() methods will result in different
//! hashes, also the speed of makeStringHash() is maximized.
FOG_API uint32_t makeDataHash(const void* data, sysuint_t size);

// STANDARD hash functions for 8 bit and 16 bit NULL terminated strings.
// 8 bit and 16 bit hashes are compatible between LATIN1 and UTF16 strings.
//
// BIG NOTE: makeStringHash() and makeDataHash() functions are INCOMPATIBLE!

//! @brief Hash 8-bit string.
//!
//! @param key 8-bit string to hash.
//! @param length Length of string or DETECT_LENGTH to detect it.
//!
//! @note Hashing LATIN1 and UTF16 strings produces equal result. H
//!
//! @note @c makeDataHash() and @c makeStringHash() methods will result in different
//! hashes, also the speed of makeStringHash() is maximized.
FOG_API uint32_t makeStringHash(const char* key, sysuint_t length);

//! @brief Hash 16-bit string.
//!
//! @param key 16-bit string to hash.
//! @param length Length of string or DETECT_LENGTH to detect it.
//!
//! @note Hashing LATIN1 and UTF16 strings produces equal result.
//!
//! @note @c makeDataHash() and @c makeStringHash() methods will result in different
//! hashes, also the speed of makeStringHash() is maximized.
FOG_API uint32_t makeStringHash(const Char* key, sysuint_t length);

// ============================================================================
// [Fog::HashUtil::combineHash]
// ============================================================================

//! @brief Combine two hash values into one.
static FOG_INLINE uint32_t combineHash(uint32_t h0, uint32_t h1)
{
  return h0 + h1;
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

//! @brief Combine nine hash values into one.
static FOG_INLINE uint32_t combineHash(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4, uint32_t h5, uint32_t h6, uint32_t h7, uint32_t h8)
{
  return h0 + h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8;
}

//! @brief Combine ten hash values into one.
static FOG_INLINE uint32_t combineHash(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4, uint32_t h5, uint32_t h6, uint32_t h7, uint32_t h8, uint32_t h9)
{
  return h0 + h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8 + h9;
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

FOG_DECLARE_HASHABLE(float, { FloatBits data; data.f = key; return getHashCode(data.i32); })
FOG_DECLARE_HASHABLE(double, { DoubleBits data; data.d = key; return getHashCode(data.i64); })

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
#endif // _FOG_CORE_COLLECTION_HASHUTIL_H
