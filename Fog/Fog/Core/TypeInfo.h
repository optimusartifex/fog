// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TYPEINFO_H
#define _FOG_CORE_TYPEINFO_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Core/Constants.h>

namespace Fog {

// ===========================================================================
// [Fog::TypeInfo - Types]
//
// Class types for Fog::TypeInfo<T> template.
// ===========================================================================

//! @brief Types for @c Fog::TypeInfo, use together with
//! @c FOG_DECLARE_TYPEINFO macro.
enum TYPEINFO_TYPE
{
  //! @brief Simple data type like @c int, @c long, ...
  //!
  //! Simple data can be copyed from one memory location into another.
  TYPEINFO_PRIMITIVE = 0,

  //! @brief Moveable data type line @c Fog::String, ...
  //!
  //! Moveable data type can't be copyed to another location, but
  //! can be moved in memory.
  TYPEINFO_MOVABLE = 1,

  //! @brief Class data type. 
  //!
  //! Means that class cannot be moved to another location.
  TYPEINFO_CLASS = 2
};

//! @brief Additional flags for FOG_DECLARE_TYPEINFO. All flags are cleared in
//! in @c Fog::TypeInfo<T> template.
enum TYPEINFO_FLAGS
{
  TYPEINFO_IS_POD_TYPE   = 0x00000100,
  TYPEINFO_IS_FLOAT_TYPE = 0x00000200,
  TYPEINFO_HAS_COMPARE   = 0x00000400,
  TYPEINFO_HAS_EQ        = 0x00000800,
  TYPEINFO_MASK          = 0xFFFFFF00
};

// [Fog::AbstractCompareFn and Fog::AbstractEqFn]

typedef int (*AbstractCompareFn)(const void* a, const void* b);
typedef bool (*AbstractEqFn)(const void* a, const void* b);

// ===========================================================================
// [Fog::TypeInfo<T>
// ===========================================================================

//! @class template<T> TypeInfo
//! @brief Template for partial @c Fog::TypeInfo specialization.

#if defined(FOG_CC_HAVE_PARTIAL_TEMPLATE_SPECIALIZATION)

template<typename T>
struct TypeInfo
{
  // TypeInfo constants
  enum
  {
    // [Type - Generic type is always ClassType]
    TYPE = TYPEINFO_CLASS,
    FLAGS = 0,

    // [Basic Information]
    IS_POINTER = 0,
    IS_PRIMITIVE = (TYPE == TYPEINFO_PRIMITIVE),
    IS_MOVABLE = (TYPE <= TYPEINFO_MOVABLE),
    IS_CLASS = (TYPE == TYPEINFO_CLASS),

    // [Extended Information]
    IS_POD = 0,
    IS_FLOAT = 0,
    HAS_COMPARE = 0,
    HAS_EQ = 0
  };

  typedef bool (*EqFn)(const T* a, const T* b);
  typedef int (*CompareFn)(const T* a, const T* b);
};

// Specialization for pointers. Pointer is always TYPEINFO_PRIMITIVE.
template<typename T>
struct TypeInfo<T*>
{
  // TypeInfo constants
  enum
  {
    // [Type - Pointer is always simple type]
    TYPE = TYPEINFO_PRIMITIVE,
    FLAGS = TYPEINFO_IS_POD_TYPE,

    // [Type - Generic type is always ClassType]
    IS_POINTER = 1,
    IS_PRIMITIVE = (TYPE == TYPEINFO_PRIMITIVE),
    IS_MOVABLE = (TYPE <= TYPEINFO_MOVABLE),
    IS_CLASS = (TYPE == TYPEINFO_CLASS),

    // [Extended Information]
    IS_POD = 1, // POD is comparable by default
    IS_FLOAT = 0,
    HAS_COMPARE = 0,
    HAS_EQ = 0
  };

  typedef bool (*EqFn)(const T** a, const T** b);
  typedef int (*CompareFn)(const T** a, const T** b);
};

#else // No template specialization.

// I first seen following hackery in Qt / Boost. This is very
// smart method how to get whether type is pointer or not. To
// make this working following dummy function and it's 
// specialization is needed.
//
// It's easy. It's needed to use sizeof() to determine the size
// of return value of this function. If size will be sizeof(char)
// (this is our type) then type is pointer, otherwise it's not.
template<typename T>
char TypeInfo_NoPtiHelper(T*(*)());
// And specialization.
void* TypeInfo_NoPtiHelper(...);

template<typename T>
struct TypeInfo
{
  // TypeInfo constants
  enum
  {
    // This is the hackery result.
    IS_POINTER = (sizeof(char) == sizeof( TypeInfo_NoPtiHelper((T(*)())0) ) ),

    // [Type - Generic type is always ClassType]
    TYPE = IS_POINTER ? TYPEINFO_PRIMITIVE : TYPEINFO_CLASS,
    FLAGS = 0,

    // [Basic Information]
    IS_PRIMITIVE = (!IS_POINTER),
    IS_MOVABLE = (!IS_POINTER),
    IS_CLASS = (!IS_POINTER),

    // [Extended Information]
    IS_POD = IS_POINTER,
    IS_FLOAT = 0,
    HAS_COMPARE = 0,
    HAS_EQ = 0
  };

  typedef bool (*EqFn)(const T* a, const T* b);
  typedef int (*CompareFn)(const T* a, const T* b);
};

#endif // FOG_CC_HAVE_PARTIAL_TEMPLATE_SPECIALIZATION

//! @brief Inherited by all types declared using @c FOG_DECLARE_TYPEINFO().
template<typename T, uint __TypeInfo__>
struct TypeInfo_Wrapper
{
  // TypeInfo constants
  enum {
    // [Type - Based on __TypeInfo__]
    TYPE = (__TypeInfo__ & ~TYPEINFO_MASK),
    FLAGS = (__TypeInfo__ & TYPEINFO_MASK),

    // [Basic Information]
    IS_PRIMITIVE = (TYPE == TYPEINFO_PRIMITIVE),
    IS_MOVABLE = (TYPE <= TYPEINFO_MOVABLE),
    IS_CLASS = (TYPE == TYPEINFO_CLASS),
    IS_POINTER = 0,

    // [Extended Information]
    IS_POD = (__TypeInfo__ & TYPEINFO_IS_POD_TYPE) != 0,
    IS_FLOAT = (__TypeInfo__ & TYPEINFO_IS_FLOAT_TYPE) != 0,
    HAS_COMPARE = (__TypeInfo__ & TYPEINFO_HAS_COMPARE) != 0,
    HAS_EQ = (__TypeInfo__ & TYPEINFO_HAS_EQ) != 0
  };

  typedef bool (*EqFn)(const T* a, const T* b);
  typedef int (*CompareFn)(const T* a, const T* b);
};

// ===========================================================================
// [Fog::TypeCmp<T>]
// ===========================================================================

template<typename T>
struct TypeCmp
{
  static FOG_INLINE int compare(const T* a, const T* b)
  {
    // This is default compare method. For POD integral types it's quite
    // inefficient so there are some overloads.
    //
    // Also this implementation needs overloaded operator < and == in classes
    // that are ready for comparisions. I think that this method is compatible
    // with STL and other libraries as well.
    return (*a < *b) ? -1 : ((*a == *b) ? 0 : 1);
  }
};

// Some overloads to optimize performance.
#define __FOG_DECLARE_TYPECMP_INT(TYPE) \
template<> \
struct TypeCmp<TYPE> \
{ \
  static FOG_INLINE int compare(const TYPE* a, const TYPE* b) { return *a - *b; } \
};

__FOG_DECLARE_TYPECMP_INT(int8_t)
__FOG_DECLARE_TYPECMP_INT(int16_t)
__FOG_DECLARE_TYPECMP_INT(int32_t)

#undef __FOG_DECLARE_TYPECMP_INT

//! @brief Inherited by all types declared using @c FOG_DECLARE_TYPECMP().
template<typename T>
struct TypeCmp_Wrapper : public T {};

// ===========================================================================
// [FOG_DECLARE_TYPEINFO()]
// ===========================================================================

/*
template<typename Type>
struct TypeToType { typedef Type Self; };

template<typename Base, typename A1>
struct TypeToType1 { typedef Base< TypeToType<A1>::Self > Self; };
*/

//! @brief Use this macro to declare @c Fog::TypeInfo. 
//!
//! @c Fog::TypeInfo is template to resolve type at compile time. It's
//! used in template specializations
#define FOG_DECLARE_TYPEINFO(__symbol__, __typeinfo__) \
namespace Fog { \
template <> \
struct TypeInfo <__symbol__> : public TypeInfo_Wrapper< __symbol__, __typeinfo__ > {}; \
}

#define FOG_DECLARE_TYPEINFO_TEMPLATE1(__symbol__, T1, A1, __typeinfo__) \
namespace Fog { \
template <T1 A1> \
struct TypeInfo < __symbol__<A1> > : public TypeInfo_Wrapper< __symbol__<A1>, __typeinfo__ > {}; \
}

#define FOG_DECLARE_TYPEINFO_TEMPLATE2(__symbol__, T1, A1, T2, A2, __typeinfo__) \
namespace Fog { \
template <T1 A1, T2 A2> \
struct TypeInfo < __symbol__<A1, A2> > : public TypeInfo_Wrapper< __symbol__<A1, A2>, __typeinfo__ > {}; \
}

#define FOG_DECLARE_TYPEINFO_TEMPLATE3(__symbol__, T1, A1, T2, A2, T3, A3, __typeinfo__) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3> \
struct TypeInfo < __symbol__<A1, A2, A3> > : public TypeInfo_Wrapper< __symbol__<A1, A2, A3>, __typeinfo__ > {}; \
}

#define FOG_DECLARE_TYPEINFO_TEMPLATE4(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, __typeinfo__) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4> \
struct TypeInfo < __symbol__<A1, A2, A3, A4> > : public TypeInfo_Wrapper< __symbol__<A1, A2, A3, A4>, __typeinfo__ > {}; \
}

#define FOG_DECLARE_TYPEINFO_TEMPLATE5(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, __typeinfo__) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5> \
struct TypeInfo < __symbol__<A1, A2, A3, A4, A5> > : public TypeInfo_Wrapper< __symbol__<A1, A2, A3, A4, A5>, __typeinfo__ > {}; \
}

#define FOG_DECLARE_TYPEINFO_TEMPLATE6(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, __typeinfo__) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6> \
struct TypeInfo < __symbol__<A1, A2, A3, A4, A5, A6> > : public TypeInfo_Wrapper< __symbol__<A1, A2, A3, A4, A5, A6>, __typeinfo__ > {}; \
}

#define FOG_DECLARE_TYPEINFO_TEMPLATE7(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, __typeinfo__) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7> \
struct TypeInfo < __symbol__<A1, A2, A3, A4, A5, A6, A7> > : public TypeInfo_Wrapper< __symbol__<A1, A2, A3, A4, A5, A6, A7>, __typeinfo__ > {}; \
}

#define FOG_DECLARE_TYPEINFO_TEMPLATE8(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, __typeinfo__) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8> \
struct TypeInfo < __symbol__<A1, A2, A3, A4, A5, A6, A7, A8> > : public TypeInfo_Wrapper< __symbol__<A1, A2, A3, A4, A5, A6, A7, A8>, __typeinfo__ > {}; \
}

#define FOG_DECLARE_TYPEINFO_TEMPLATE9(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, __typeinfo__) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9> \
struct TypeInfo < __symbol__<A1, A2, A3, A4, A5, A6, A7, A8, A9> > : public TypeInfo_Wrapper< __symbol__<A1, A2, A3, A4, A5, A6, A7, A8, A9>, __typeinfo__ > {}; \
}

#define FOG_DECLARE_TYPEINFO_TEMPLATE10(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, T10, A10, __typeinfo__) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10> \
struct TypeInfo < __symbol__<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> > : public TypeInfo_Wrapper< __symbol__<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>, __typeinfo__ > {}; \
}

// ===========================================================================
// [FOG_DECLARE_TYPECMP()]
// ===========================================================================

#define FOG_DECLARE_TYPECMP(__symbol__) \
namespace Fog { \
template <> \
struct TypeCmp <__symbol__> : public TypeCmp_Wrapper< __symbol__ > {}; \
}

#define FOG_DECLARE_TYPECMP_TEMPLATE1(__symbol__, T1, A1) \
namespace Fog { \
template <T1 A1> \
struct TypeCmp < __symbol__<A1> > : public TypeCmp_Wrapper< __symbol__<A1> > {}; \
}

#define FOG_DECLARE_TYPECMP_TEMPLATE2(__symbol__, T1, A1, T2, A2) \
namespace Fog { \
template <T1 A1, T2 A2> \
struct TypeCmp < __symbol__<A1, A2> > : public TypeCmp_Wrapper< __symbol__<A1, A2> > {}; \
}

#define FOG_DECLARE_TYPECMP_TEMPLATE3(__symbol__, T1, A1, T2, A2, T3, A3) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3> \
struct TypeCmp < __symbol__<A1, A2, A3> > : public TypeCmp_Wrapper< __symbol__<A1, A2, A3> > {}; \
}

#define FOG_DECLARE_TYPECMP_TEMPLATE4(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4> \
struct TypeCmp < __symbol__<A1, A2, A3, A4> > : public TypeCmp_Wrapper< __symbol__<A1, A2, A3, A4> > {}; \
}

#define FOG_DECLARE_TYPECMP_TEMPLATE5(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5> \
struct TypeCmp < __symbol__<A1, A2, A3, A4, A5> > : public TypeCmp_Wrapper< __symbol__<A1, A2, A3, A4, A5> > {}; \
}

#define FOG_DECLARE_TYPECMP_TEMPLATE6(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6> \
struct TypeCmp < __symbol__<A1, A2, A3, A4, A5, A6> > : public TypeCmp_Wrapper< __symbol__<A1, A2, A3, A4, A5, A6> > {}; \
}

#define FOG_DECLARE_TYPECMP_TEMPLATE7(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7> \
struct TypeCmp < __symbol__<A1, A2, A3, A4, A5, A6, A7> > : public TypeCmp_Wrapper< __symbol__<A1, A2, A3, A4, A5, A6, A7> > {}; \
}

#define FOG_DECLARE_TYPECMP_TEMPLATE8(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8> \
struct TypeCmp < __symbol__<A1, A2, A3, A4, A5, A6, A7, A8> > : public TypeCmp_Wrapper< __symbol__<A1, A2, A3, A4, A5, A6, A7, A8> > {}; \
}

#define FOG_DECLARE_TYPECMP_TEMPLATE9(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9> \
struct TypeCmp < __symbol__<A1, A2, A3, A4, A5, A6, A7, A8, A9> > : public TypeCmp_Wrapper< __symbol__<A1, A2, A3, A4, A5, A6, A7, A8, A9> > {}; \
}

#define FOG_DECLARE_TYPECMP_TEMPLATE10(__symbol__, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, T10, A10) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10> \
struct TypeCmp < __symbol__<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> > : public TypeCmp_Wrapper< __symbol__<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> > {}; \
}

} // Fog namespace

// ===========================================================================
// [Fog::TypeInfo - Built-In]
// ===========================================================================

FOG_DECLARE_TYPEINFO(int8_t, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
FOG_DECLARE_TYPEINFO(uint8_t, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
FOG_DECLARE_TYPEINFO(int16_t, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
FOG_DECLARE_TYPEINFO(uint16_t, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
FOG_DECLARE_TYPEINFO(int32_t, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
FOG_DECLARE_TYPEINFO(uint32_t, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
FOG_DECLARE_TYPEINFO(int64_t, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
FOG_DECLARE_TYPEINFO(uint64_t, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)

FOG_DECLARE_TYPEINFO(float, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE | Fog::TYPEINFO_IS_FLOAT_TYPE)
FOG_DECLARE_TYPEINFO(double, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE | Fog::TYPEINFO_IS_FLOAT_TYPE)

#if !defined(FOG_CC_MSVC) && !defined(FOG_CC_BORLAND)
// char is same as int8_t or uint8_t for borland compiler
FOG_DECLARE_TYPEINFO(char, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
#endif // FOG_CC_BORLAND

// TODO: long and ulong checking
#if !(defined(FOG_CC_GNU) && FOG_ARCH_BITS == 64)
FOG_DECLARE_TYPEINFO(long, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
FOG_DECLARE_TYPEINFO(ulong, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
#endif // long / ulong

// TODO: wchar_t

// [Guard]
#endif // _FOG_CORE_TYPEINFO_H
