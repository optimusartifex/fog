// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_HASHUTIL_H
#define _FOG_CORE_TOOLS_HASHUTIL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Char.h>

namespace Fog {
namespace HashUtil {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::HashFunc - Binary]
// ============================================================================

//! @brief Hash data.
//!
//! @param data Pointer to data sequence.
//! @param size Size of data. You cant use DETECT_LENGTH here.
static FOG_INLINE uint32_t hashBinary(const void* data, size_t length)
{
  return _api.hashutil_hashBinary(data, length);
}

// ============================================================================
// [Fog::HashFunc - StubA / StubW]
// ============================================================================

//! @brief Hash 8-bit string (LATIN1).
static FOG_INLINE uint32_t hashStubA(const StubA& k)
{
  return _api.hashutil_hashStubA(&k);
}

//! @brief Hash 16-bit string (UTF-16).
static FOG_INLINE uint32_t hashStubW(const StubW& k)
{
  return _api.hashutil_hashStubW(&k);
}

// ============================================================================
// [Fog::HashFunc - Integer]
// ============================================================================

static FOG_INLINE uint32_t hashU32(uint32_t k) { return k; }
static FOG_INLINE uint32_t hashI32(int32_t  k) { return (uint32_t)k; }
static FOG_INLINE uint32_t hashU64(uint64_t k) { return (uint32_t)(k >> 31) ^ (uint32_t)k; }
static FOG_INLINE uint32_t hashI64(int64_t  k) { return hashU64((uint64_t)k); }

static FOG_INLINE uint32_t hashPtr(const void* k)
{
  if (sizeof(void*) == sizeof(uint32_t))
    return hashU32((uint32_t)(size_t)k);
  else
    return hashU64((uint64_t)(size_t)k);
}

// ============================================================================
// [Fog::HashFunc - Vector]
// ============================================================================

static FOG_INLINE uint32_t hashVectorD(const void* data, size_t length) { return _api.hashutil_hashVectorD(data, length); }
static FOG_INLINE uint32_t hashVectorQ(const void* data, size_t length) { return _api.hashutil_hashVectorQ(data, length); }

// ============================================================================
// [Fog::HashFunc - Combine]
// ============================================================================

//! @brief Combine two hash values into one.
static FOG_INLINE uint32_t combine(uint32_t h0, uint32_t h1)
{
  return h0 + h1;
}

//! @brief Combine three hash values into one.
static FOG_INLINE uint32_t combine(uint32_t h0, uint32_t h1, uint32_t h2)
{
  return h0 + h1 + h2;
}

//! @brief Combine four hash values into one.
static FOG_INLINE uint32_t combine(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3)
{
  return h0 + h1 + h2 + h3;
}

//! @brief Combine five hash values into one.
static FOG_INLINE uint32_t combine(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4)
{
  return h0 + h1 + h2 + h3 + h4;
}

//! @brief Combine six hash values into one.
static FOG_INLINE uint32_t combine(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4, uint32_t h5)
{
  return h0 + h1 + h2 + h3 + h4 + h5;
}

//! @brief Combine seven hash values into one.
static FOG_INLINE uint32_t combine(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4, uint32_t h5, uint32_t h6)
{
  return h0 + h1 + h2 + h3 + h4 + h5 + h6;
}

//! @brief Combine eight hash values into one.
static FOG_INLINE uint32_t combine(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4, uint32_t h5, uint32_t h6, uint32_t h7)
{
  return h0 + h1 + h2 + h3 + h4 + h5 + h6 + h7;
}

//! @brief Combine nine hash values into one.
static FOG_INLINE uint32_t combine(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4, uint32_t h5, uint32_t h6, uint32_t h7, uint32_t h8)
{
  return h0 + h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8;
}

//! @brief Combine ten hash values into one.
static FOG_INLINE uint32_t combine(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4, uint32_t h5, uint32_t h6, uint32_t h7, uint32_t h8, uint32_t h9)
{
  return h0 + h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8 + h9;
}

// ============================================================================
// [Fog::getHashCode]
// ============================================================================

template<typename Type, int IS_POINTER>
struct HashHelper
{
  // Default action is to call getHashCode() method.
  static FOG_INLINE uint32_t hash(const Type& k) { return k.getHashCode(); }
};

template<typename Type>
struct HashHelper<Type, 1>
{
  // Default action is to call getHashCode() method.
  static FOG_INLINE uint32_t hash(const Type& k) { return hashPtr(k); }
};

template<typename Type>
FOG_STATIC_INLINE_T uint32_t hash(const Type& k)
{
  return HashHelper< Type, TypeInfo<Type>::IS_POINTER >::hash(k);
}

#define FOG_DECLARE_HASHABLE(_Type_, _Code_) \
template<> \
FOG_STATIC_INLINE_T uint32_t hash<_Type_>(const _Type_& k) _Code_

#define FOG_DECLARE_HASHABLE_PTR(_Type_, _Code_) \
template<> \
FOG_STATIC_INLINE_T uint32_t hash<_Type_ &>(_Type_& k) _Code_ \
\
template<> \
FOG_STATIC_INLINE_T uint32_t hash<const _Type_ &>(const _Type_& k) _Code_

#if defined(FOG_CC_HAVE_NATIVE_CHAR_TYPE)
FOG_DECLARE_HASHABLE(char          , { return (uint8_t)k;        })
#endif // FOG_CC_HAVE_NATIVE_CHAR_TYPE

#if defined(FOG_CC_HAVE_NATIVE_WCHAR_TYPE)
FOG_DECLARE_HASHABLE(wchar_t       , { return k;                 })
#endif // FOG_CC_HAVE_NATIVE_WCHAR_TYPE

FOG_DECLARE_HASHABLE(signed char   , { return (unsigned char)k;  })
FOG_DECLARE_HASHABLE(unsigned char , { return (unsigned char)k;  })
FOG_DECLARE_HASHABLE(short         , { return (unsigned short)k; })
FOG_DECLARE_HASHABLE(unsigned short, { return k;                 })
FOG_DECLARE_HASHABLE(int           , { return (unsigned int)k;   })
FOG_DECLARE_HASHABLE(unsigned int  , { return k;                 })
FOG_DECLARE_HASHABLE(int64_t       , { return hashI64(k);        })
FOG_DECLARE_HASHABLE(uint64_t      , { return hashU64(k);        })

FOG_DECLARE_HASHABLE(float         , { FloatBits  u; u.f = k; return hash(u.u32); })
FOG_DECLARE_HASHABLE(double        , { DoubleBits u; u.d = k; return hash(u.u64); })

FOG_DECLARE_HASHABLE(StubA         , { return hashStubA(k);      })
FOG_DECLARE_HASHABLE(StubW         , { return hashStubW(k);      })

//! @}

} // Fog namespace
} // HashUtil namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_HASHUTIL_H
