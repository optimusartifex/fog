// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_TYPEINFO_H
#define _FOG_CORE_GLOBAL_TYPEINFO_H

// [Dependencies]
#include <Fog/Core/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ===========================================================================
// [Fog::TypeInfo<T>
// ===========================================================================

//! @class template<typename T> TypeInfo
//! @brief Template for partial @c Fog::TypeInfo specialization.

#if defined(FOG_CC_HAVE_PARTIAL_TEMPLATE_SPECIALIZATION)

template<typename T>
struct TypeInfo
{
  enum
  {
    // ------------------------------------------------------------------------
    // [Type / Flags]
    // ------------------------------------------------------------------------

    TYPE         = TYPEINFO_COMPLEX,
    FLAGS        = 0,

    // ------------------------------------------------------------------------
    // [Basic]
    // ------------------------------------------------------------------------

    IS_PRIMITIVE = (TYPE == TYPEINFO_PRIMITIVE),
    IS_MOVABLE   = (TYPE <= TYPEINFO_MOVABLE),
    IS_COMPLEX   = (TYPE == TYPEINFO_COMPLEX),

    // ------------------------------------------------------------------------
    // [Extended]
    // ------------------------------------------------------------------------

    IS_POINTER   = 0,
    IS_POD       = 0,
    IS_FLOAT     = 0,
    IS_DOUBLE    = 0,

    HAS_COMPARE  = 0,
    HAS_EQ       = 0
  };

  typedef bool (*EqFn)(const T* a, const T* b);
  typedef int (*CompareFn)(const T* a, const T* b);
};

// TypeInfo pointer specialization. Pointer is always primitive type.
template<typename T>
struct TypeInfo<T*>
{
  enum
  {
    // ------------------------------------------------------------------------
    // [Type / Flags]
    // ------------------------------------------------------------------------

    TYPE         = TYPEINFO_PRIMITIVE,
    FLAGS        = TYPEINFO_IS_POD_TYPE,

    // ------------------------------------------------------------------------
    // [Basic]
    // ------------------------------------------------------------------------

    IS_PRIMITIVE = (TYPE == TYPEINFO_PRIMITIVE),
    IS_MOVABLE   = (TYPE <= TYPEINFO_MOVABLE),
    IS_COMPLEX   = (TYPE == TYPEINFO_COMPLEX),

    // ------------------------------------------------------------------------
    // [Extended]
    // ------------------------------------------------------------------------

    IS_POINTER   = 1,
    IS_POD       = 1, // Pointer is comparable.
    IS_FLOAT     = 0,
    IS_DOUBLE    = 0,
    HAS_COMPARE  = 0,
    HAS_EQ       = 0
  };

  typedef bool (*EqFn)(const T** a, const T** b);
  typedef int (*CompareFn)(const T** a, const T** b);
};

#else // No template specialization.

// I first saw the following hackery in Qt / Boost. It's very smart method
// which can be used to check whether the type is a pointer. To make this
// working the template TypeInfo_NoPtiHelper<> was created. The trick is
// to make another specialized variant and to compare the type size of a
// return value.
template<typename T>
char TypeInfo_NoPtiHelper(T*(*)());
// And specialization.
void* TypeInfo_NoPtiHelper(...);

template<typename T>
struct TypeInfo
{
  enum
  {
    IS_POINTER   = (sizeof(char) == sizeof( TypeInfo_NoPtiHelper((T(*)())0) ) ),

    // ------------------------------------------------------------------------
    // [Type / Flags]
    // ------------------------------------------------------------------------

    TYPE         = IS_POINTER ? TYPEINFO_PRIMITIVE : TYPEINFO_COMPLEX,
    FLAGS        = 0,

    // ------------------------------------------------------------------------
    // [Basic]
    // ------------------------------------------------------------------------

    IS_PRIMITIVE = (!IS_POINTER),
    IS_MOVABLE   = (!IS_POINTER),
    IS_COMPLEX   = (!IS_POINTER),

    // ------------------------------------------------------------------------
    // [Extended]
    // ------------------------------------------------------------------------

    IS_POD       = IS_POINTER,
    IS_FLOAT     = 0,
    IS_DOUBLE    = 0,
    HAS_COMPARE  = 0,
    HAS_EQ       = 0
  };

  typedef bool (*EqFn)(const T* a, const T* b);
  typedef int (*CompareFn)(const T* a, const T* b);
};

#endif // FOG_CC_HAVE_PARTIAL_TEMPLATE_SPECIALIZATION

//! @brief Inherited by all types declared using @c _FOG_TYPEINFO_DECLARE().
template<typename T, uint __TypeInfo__>
struct TypeInfo_Wrapper
{
  enum
  {
    // ------------------------------------------------------------------------
    // [Type]
    // ------------------------------------------------------------------------

    TYPE         = (__TypeInfo__ & ~TYPEINFO_MASK),
    FLAGS        = (__TypeInfo__ & TYPEINFO_MASK),

    IS_PRIMITIVE = (TYPE == TYPEINFO_PRIMITIVE),
    IS_MOVABLE   = (TYPE <= TYPEINFO_MOVABLE),
    IS_COMPLEX   = (TYPE == TYPEINFO_COMPLEX),

    // ------------------------------------------------------------------------
    // [Extended]
    // ------------------------------------------------------------------------

    IS_POINTER   = 0,
    IS_POD       = (__TypeInfo__ & TYPEINFO_IS_POD_TYPE   ) != 0,
    IS_FLOAT     = (__TypeInfo__ & TYPEINFO_IS_FLOAT_TYPE ) != 0,
    IS_DOUBLE    = (__TypeInfo__ & TYPEINFO_IS_DOUBLE_TYPE) != 0,
    HAS_COMPARE  = (__TypeInfo__ & TYPEINFO_HAS_COMPARE   ) != 0,
    HAS_EQ       = (__TypeInfo__ & TYPEINFO_HAS_EQ        ) != 0
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
#define ___FOG_TYPECMP_DECLAREINT(TYPE) \
template<> \
struct TypeCmp<TYPE> \
{ \
  static FOG_INLINE int compare(const TYPE* a, const TYPE* b) { return *a - *b; } \
};

___FOG_TYPECMP_DECLAREINT(int8_t)
___FOG_TYPECMP_DECLAREINT(int16_t)
___FOG_TYPECMP_DECLAREINT(int32_t)

#undef ___FOG_TYPECMP_DECLAREINT

//! @brief Inherited by all types declared using @c _FOG_TYPECMP_DECLARE().
template<typename T>
struct TypeCmp_Wrapper : public T {};

// ===========================================================================
// [_FOG_TYPEINFO_DECLARE()]
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
#define _FOG_TYPEINFO_DECLARE(_Symbol_, _TypeInfo_) \
namespace Fog { \
template <> \
struct TypeInfo < ::_Symbol_ > : public TypeInfo_Wrapper< ::_Symbol_, _TypeInfo_ > {}; \
}

#define _FOG_TYPEINFO_DECLARE_T1(_Symbol_, T1, A1, _TypeInfo_) \
namespace Fog { \
template <T1 A1> \
struct TypeInfo < ::_Symbol_<A1> > : public TypeInfo_Wrapper< ::_Symbol_<A1>, _TypeInfo_ > {}; \
}

#define _FOG_TYPEINFO_DECLARE_T2(_Symbol_, T1, A1, T2, A2, _TypeInfo_) \
namespace Fog { \
template <T1 A1, T2 A2> \
struct TypeInfo < ::_Symbol_<A1, A2> > : public TypeInfo_Wrapper< ::_Symbol_<A1, A2>, _TypeInfo_ > {}; \
}

#define _FOG_TYPEINFO_DECLARE_T3(_Symbol_, T1, A1, T2, A2, T3, A3, _TypeInfo_) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3> \
struct TypeInfo < ::_Symbol_<A1, A2, A3> > : public TypeInfo_Wrapper< ::_Symbol_<A1, A2, A3>, _TypeInfo_ > {}; \
}

#define _FOG_TYPEINFO_DECLARE_T4(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, _TypeInfo_) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4> \
struct TypeInfo < ::_Symbol_<A1, A2, A3, A4> > : public TypeInfo_Wrapper< ::_Symbol_<A1, A2, A3, A4>, _TypeInfo_ > {}; \
}

#define _FOG_TYPEINFO_DECLARE_T5(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, _TypeInfo_) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5> \
struct TypeInfo < ::_Symbol_<A1, A2, A3, A4, A5> > : public TypeInfo_Wrapper< ::_Symbol_<A1, A2, A3, A4, A5>, _TypeInfo_ > {}; \
}

#define _FOG_TYPEINFO_DECLARE_T6(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, _TypeInfo_) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6> \
struct TypeInfo < ::_Symbol_<A1, A2, A3, A4, A5, A6> > : public TypeInfo_Wrapper< ::_Symbol_<A1, A2, A3, A4, A5, A6>, _TypeInfo_ > {}; \
}

#define _FOG_TYPEINFO_DECLARE_T7(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, _TypeInfo_) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7> \
struct TypeInfo < ::_Symbol_<A1, A2, A3, A4, A5, A6, A7> > : public TypeInfo_Wrapper< ::_Symbol_<A1, A2, A3, A4, A5, A6, A7>, _TypeInfo_ > {}; \
}

#define _FOG_TYPEINFO_DECLARE_T8(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, _TypeInfo_) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8> \
struct TypeInfo < ::_Symbol_<A1, A2, A3, A4, A5, A6, A7, A8> > : public TypeInfo_Wrapper< ::_Symbol_<A1, A2, A3, A4, A5, A6, A7, A8>, _TypeInfo_ > {}; \
}

#define _FOG_TYPEINFO_DECLARE_T9(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, _TypeInfo_) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9> \
struct TypeInfo < ::_Symbol_<A1, A2, A3, A4, A5, A6, A7, A8, A9> > : public TypeInfo_Wrapper< ::_Symbol_<A1, A2, A3, A4, A5, A6, A7, A8, A9>, _TypeInfo_ > {}; \
}

#define _FOG_TYPEINFO_DECLARE_T10(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, T10, A10, _TypeInfo_) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10> \
struct TypeInfo < ::_Symbol_<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> > : public TypeInfo_Wrapper< ::_Symbol_<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>, _TypeInfo_ > {}; \
}

// ===========================================================================
// [_FOG_TYPECMP_DECLARE()]
// ===========================================================================

#define _FOG_TYPECMP_DECLARE(_Symbol_) \
namespace Fog { \
template <> \
struct TypeCmp <_Symbol_> : public TypeCmp_Wrapper< _Symbol_ > {}; \
}

#define _FOG_TYPECMP_DECLARE_TEMPLATE1(_Symbol_, T1, A1) \
namespace Fog { \
template <T1 A1> \
struct TypeCmp < _Symbol_<A1> > : public TypeCmp_Wrapper< _Symbol_<A1> > {}; \
}

#define _FOG_TYPECMP_DECLARE_TEMPLATE2(_Symbol_, T1, A1, T2, A2) \
namespace Fog { \
template <T1 A1, T2 A2> \
struct TypeCmp < _Symbol_<A1, A2> > : public TypeCmp_Wrapper< _Symbol_<A1, A2> > {}; \
}

#define _FOG_TYPECMP_DECLARE_TEMPLATE3(_Symbol_, T1, A1, T2, A2, T3, A3) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3> \
struct TypeCmp < _Symbol_<A1, A2, A3> > : public TypeCmp_Wrapper< _Symbol_<A1, A2, A3> > {}; \
}

#define _FOG_TYPECMP_DECLARE_TEMPLATE4(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4> \
struct TypeCmp < _Symbol_<A1, A2, A3, A4> > : public TypeCmp_Wrapper< _Symbol_<A1, A2, A3, A4> > {}; \
}

#define _FOG_TYPECMP_DECLARE_TEMPLATE5(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5> \
struct TypeCmp < _Symbol_<A1, A2, A3, A4, A5> > : public TypeCmp_Wrapper< _Symbol_<A1, A2, A3, A4, A5> > {}; \
}

#define _FOG_TYPECMP_DECLARE_TEMPLATE6(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6> \
struct TypeCmp < _Symbol_<A1, A2, A3, A4, A5, A6> > : public TypeCmp_Wrapper< _Symbol_<A1, A2, A3, A4, A5, A6> > {}; \
}

#define _FOG_TYPECMP_DECLARE_TEMPLATE7(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7> \
struct TypeCmp < _Symbol_<A1, A2, A3, A4, A5, A6, A7> > : public TypeCmp_Wrapper< _Symbol_<A1, A2, A3, A4, A5, A6, A7> > {}; \
}

#define _FOG_TYPECMP_DECLARE_TEMPLATE8(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8> \
struct TypeCmp < _Symbol_<A1, A2, A3, A4, A5, A6, A7, A8> > : public TypeCmp_Wrapper< _Symbol_<A1, A2, A3, A4, A5, A6, A7, A8> > {}; \
}

#define _FOG_TYPECMP_DECLARE_TEMPLATE9(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9> \
struct TypeCmp < _Symbol_<A1, A2, A3, A4, A5, A6, A7, A8, A9> > : public TypeCmp_Wrapper< _Symbol_<A1, A2, A3, A4, A5, A6, A7, A8, A9> > {}; \
}

#define _FOG_TYPECMP_DECLARE_TEMPLATE10(_Symbol_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, T10, A10) \
namespace Fog { \
template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10> \
struct TypeCmp < _Symbol_<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> > : public TypeCmp_Wrapper< _Symbol_<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> > {}; \
}

//! @}

} // Fog namespace

// ===========================================================================
// [Fog::TypeInfo<> - C++ Types]
// ===========================================================================

#define _FOG_TYPEINFO_DECLARE_POD(_Symbol_, _TypeInfo_) \
namespace Fog { \
template <> \
struct TypeInfo < _Symbol_ > : public TypeInfo_Wrapper< _Symbol_, _TypeInfo_ > {}; \
}

_FOG_TYPEINFO_DECLARE_POD(int8_t  , Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
_FOG_TYPEINFO_DECLARE_POD(uint8_t , Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
_FOG_TYPEINFO_DECLARE_POD(int16_t , Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
_FOG_TYPEINFO_DECLARE_POD(uint16_t, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
_FOG_TYPEINFO_DECLARE_POD(int32_t , Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
_FOG_TYPEINFO_DECLARE_POD(uint32_t, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
_FOG_TYPEINFO_DECLARE_POD(int64_t , Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
_FOG_TYPEINFO_DECLARE_POD(uint64_t, Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)

_FOG_TYPEINFO_DECLARE_POD(float   , Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE | Fog::TYPEINFO_IS_FLOAT_TYPE)
_FOG_TYPEINFO_DECLARE_POD(double  , Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE | Fog::TYPEINFO_IS_FLOAT_TYPE | Fog::TYPEINFO_IS_DOUBLE_TYPE)

#if !defined(FOG_CC_MSC) && !defined(FOG_CC_BORLAND)
// char is same as int8_t or uint8_t for borland compiler
_FOG_TYPEINFO_DECLARE_POD(char    , Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
#endif // FOG_CC_BORLAND

// TODO: long and ulong checking
#if !(defined(FOG_CC_GNU) && FOG_ARCH_BITS == 64) && !defined(FOG_CC_CLANG)
_FOG_TYPEINFO_DECLARE_POD(long    , Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
_FOG_TYPEINFO_DECLARE_POD(ulong   , Fog::TYPEINFO_PRIMITIVE | Fog::TYPEINFO_IS_POD_TYPE)
#endif // long / ulong

// TODO: wchar_t

// [Guard]
#endif // _FOG_CORE_GLOBAL_TYPEINFO_H
