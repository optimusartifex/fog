// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_TYPEINFO_H
#define _FOG_CORE_GLOBAL_TYPEINFO_H

// [Dependencies]
#include <Fog/Core/Global/Api.h>
#include <Fog/Core/Global/EnumCore.h>
#include <Fog/Core/Global/TypeDefs.h>

// ===========================================================================
// [_Fog_TypeFlags]
// ===========================================================================

#if defined(FOG_CC_HAS_PARTIAL_TEMPLATE_SPECIALIZATION)
template<typename Type>
struct _Fog_TypeFlags
{
  enum
  {
    TYPE = Fog::TYPE_CATEGORY_COMPLEX
  };
};

template<typename Type>
struct _Fog_TypeFlags<Type*>
{
  enum
  {
    TYPE = Fog::TYPE_CATEGORY_SIMPLE |
           Fog::TYPE_FLAG_POD        |
           Fog::TYPE_FLAG_POINTER    |
           Fog::TYPE_FLAG_INTEGER    |
           Fog::TYPE_FLAG_BIN_EQ
  };
};
#else
// Hackery.
//
// I first saw the following hackery in Qt and Boost. It's very smart method
// which can be used to get the info whether the type is a pointer when
// compiling with C++ compiler without partial template specialization.
template<typename Type> char TypeNoPtiHelper(Type*(*)());
void* TypeNoPtiHelper(...);

template<typename Type>
struct _Fog_TypeFlags
{
  enum
  {
    TYPE = ( sizeof(char) == sizeof(TypeNoPtiHelper((Type(*)())0)) )
      ? Fog::TYPE_CATEGORY_SIMPLE |
        Fog::TYPE_FLAG_POD        |
        Fog::TYPE_FLAG_POINTER    |
        Fog::TYPE_FLAG_INTEGER    |
        Fog::TYPE_FLAG_BIN_EQ
      : Fog::TYPE_CATEGORY_COMPLEX
  };
};
#endif // FOG_CC_HAS_PARTIAL_TEMPLATE_SPECIALIZATION

// ===========================================================================
// [Fog::]
// ===========================================================================

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ===========================================================================
// [Fog::TypeInfo<>]
// ===========================================================================

//! @brief Inherited by all types declared using @c _FOG_TYPE_DECLARE().
template<typename Type>
struct TypeInfo
{
  enum
  {
    // ------------------------------------------------------------------------
    // [Type]
    // ------------------------------------------------------------------------

    TYPE         = _Fog_TypeFlags<Type>::TYPE & TYPE_CATEGORY_MASK,

    IS_SIMPLE    = (TYPE == TYPE_CATEGORY_SIMPLE),
    IS_MOVABLE   = (TYPE <= TYPE_CATEGORY_MOVABLE),
    IS_COMPLEX   = (TYPE >= TYPE_CATEGORY_COMPLEX),

    // ------------------------------------------------------------------------
    // [Flags]
    // ------------------------------------------------------------------------

    FLAGS        = _Fog_TypeFlags<Type>::TYPE & TYPE_FLAG_MASK,

    IS_POD       = (FLAGS & TYPE_FLAG_POD     ) != 0,
    IS_POINTER   = (FLAGS & TYPE_FLAG_POINTER ) != 0,
    IS_IMPLICIT  = (FLAGS & TYPE_FLAG_IMPLICIT) != 0,

    IS_INTEGER   = (FLAGS & TYPE_FLAG_INTEGER ) != 0,
    IS_FLOAT     = (FLAGS & TYPE_FLAG_FLOAT   ) != 0,
    IS_DOUBLE    = (FLAGS & TYPE_FLAG_DOUBLE  ) != 0,

    NO_EQ        = (FLAGS & TYPE_FLAG_NO_EQ   ) != 0,
    OWN_EQ       = (FLAGS & TYPE_FLAG_OWN_EQ  ) != 0,
    BIN_EQ       = (FLAGS & TYPE_FLAG_BIN_EQ  ) != 0,

    NO_CMP       = (FLAGS & TYPE_FLAG_NO_CMP  ) != 0,
    ONW_CMP      = (FLAGS & TYPE_FLAG_OWN_CMP ) != 0,
  };
};

// ===========================================================================
// [Fog::TypeFunc<>]
// ===========================================================================

// HasCompare can be:
//
//   - -1 - Comparison is forbidden and there are no methods to use.
//
//   -  0 - [Default] Type has no built-in compare method, but when needed the
//          compiler should generate it based on the '<' and '==' operators.
//
//   -  1 - Type has built-int compare method which should be used for
//          comparison. In this case it's highly probable that type has also
//          overloaded compare operators, but using native getCompareFunc()
//          forbids C++ compiler to generate a wrapper for this function.
//
//   -  2 - Type is integral, thus optimization to do only one comparison is
//          used if possible.
template<typename Type, int HasCompare>
struct TypeFunc_Cmp {};

template<typename Type>
struct TypeFunc_Cmp<Type, 0>
{
  //! @brief Compare function wrapper.
  //!
  //! The default compare method which should be compatible with any code which
  //! overloads the compare operator < and equality operator ==. Because this
  //! leads into two comparisons for any type, including POD type, there are
  //! some overloads which should perform better than this generic function.
  //!
  //! See _FOG_TYPE_COMPARE_INT() macro how POD types are handled, see
  //! also TYPE_FLAG_CMP_API flag which can be used together which
  //! FOG_TYPE_DECLARE to inform compiler that the specified type contains
  //! compare() and getCompareFunc() methods.
  static FOG_INLINE int FOG_CDECL compare(const Type* a, const Type* b)
  {
    if (*a < *b)
      return -1;
    else if (*a == *b)
      return 0;
    else
      return 1;
  }

  //! @brief Get compare function.
  static FOG_INLINE CompareFunc getCompareFunc() { return (CompareFunc)compare; }
};

template<typename Type>
struct TypeFunc_Cmp<Type, 1>
{
  //! @brief Compare function wrapper.
  static FOG_INLINE int FOG_CDECL compare(const Type* a, const Type* b) { return Type::compare(a, b); }

  //! @brief Get compare function.
  static FOG_INLINE CompareFunc getCompareFunc() { return (CompareFunc)Type::getCompareFunc(); }
};

template<typename Type>
struct TypeFunc_Cmp<Type, 2>
{
  //! @brief Compare function wrapper.
  static FOG_INLINE int FOG_CDECL compare(const Type* a, const Type* b)
  {
    if (sizeof(Type) < sizeof(int))
    {
      return (int)*a - (int)*b;
    }
    else
    {
      if (*a < *b)
        return -1;
      else if (*a == *b)
        return 0;
      else
        return 1;
    }
  }

  //! @brief Get compare function.
  static FOG_INLINE CompareFunc getCompareFunc() { return (CompareFunc)compare; }
};

template<typename Type, int HasEq>
struct TypeFunc_Eq {};

template<typename Type>
struct TypeFunc_Eq<Type, 0>
{
  // @brief Equality function wrapper.
  static FOG_INLINE int FOG_CDECL eq(const Type* a, const Type* b) { return *a == *b; }

  //! @brief Get equality function
  static FOG_INLINE EqFunc getEqFunc() { return (EqFunc)eq; }
};

template<typename Type>
struct TypeFunc_Eq<Type, 1>
{
  // @brief Equality function wrapper.
  static FOG_INLINE int FOG_CDECL eq(const Type* a, const Type* b) { return Type::eq(a, b); }

  //! @brief Get equality function
  static FOG_INLINE EqFunc getEqFunc() { return (EqFunc)Type::getEqFunc(); }
};

template<typename Type>
struct TypeFunc :
  public TypeFunc_Cmp< Type, ((TypeInfo<Type>::FLAGS & TYPE_FLAG_INTEGER) != 0) ? 2 :
                             ((TypeInfo<Type>::FLAGS & TYPE_FLAG_OWN_CMP) != 0) ? 1 :
                             ((TypeInfo<Type>::FLAGS & TYPE_FLAG_NO_CMP ) == 0) ? 0 : -1 >,
  public TypeFunc_Eq < Type, ((TypeInfo<Type>::FLAGS & TYPE_FLAG_OWN_EQ ) != 0) ? 1 :
                             ((TypeInfo<Type>::FLAGS & TYPE_FLAG_NO_EQ  ) == 0) ? 0 : -1 > {};

//! @}

} // Fog namespace

// ===========================================================================
// [_FOG_TYPE_DECLARE]
// ===========================================================================

//! @brief Use this macro to declare @c Fog::TypeInfo<_Type_>.
//!
//! @c Fog::TypeInfo is template to resolve type at compile time. It's
//! used in template specializations.
#define _FOG_TYPE_DECLARE(_Type_, _Flags_) \
  template <> \
  struct _Fog_TypeFlags < _Type_ > { enum { TYPE = _Flags_ }; };

#define _FOG_TYPE_DECLARE_T1(_Type_, T1, A1, _Flags_) \
  template <T1 A1> \
  struct _Fog_TypeFlags < _Type_<A1> > { enum { TYPE = _Flags_ }; };

#define _FOG_TYPE_DECLARE_T2(_Type_, T1, A1, T2, A2, _Flags_) \
  template <T1 A1, T2 A2> \
  struct _Fog_TypeFlags < _Type_<A1, A2> > { enum { TYPE = _Flags_ }; };

#define _FOG_TYPE_DECLARE_T3(_Type_, T1, A1, T2, A2, T3, A3, _Flags_) \
  template <T1 A1, T2 A2, T3 A3> \
  struct _Fog_TypeFlags < _Type_<A1, A2, A3> > { enum { TYPE = _Flags_ }; };

#define _FOG_TYPE_DECLARE_T4(_Type_, T1, A1, T2, A2, T3, A3, T4, A4, _Flags_) \
  template <T1 A1, T2 A2, T3 A3, T4 A4> \
  struct _Fog_TypeFlags < _Type_<A1, A2, A3, A4> > { enum { TYPE = _Flags_ }; };

#define _FOG_TYPE_DECLARE_T5(_Type_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, _Flags_) \
  template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5> \
  struct _Fog_TypeFlags < _Type_<A1, A2, A3, A4, A5> > { enum { TYPE = _Flags_ }; };

#define _FOG_TYPE_DECLARE_T6(_Type_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, _Flags_) \
  template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6> \
  struct _Fog_TypeFlags < _Type_<A1, A2, A3, A4, A5, A6> > { enum { TYPE = _Flags_ }; };

#define _FOG_TYPE_DECLARE_T7(_Type_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, _Flags_) \
  template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7> \
  struct _Fog_TypeFlags < _Type_<A1, A2, A3, A4, A5, A6, A7> > { enum { TYPE = _Flags_ }; };

#define _FOG_TYPE_DECLARE_T8(_Type_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, _Flags_) \
  template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8> \
  struct _Fog_TypeFlags < _Type_<A1, A2, A3, A4, A5, A6, A7, A8> > { enum { TYPE = _Flags_ }; };

#define _FOG_TYPE_DECLARE_T9(_Type_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, _Flags_) \
  template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9> \
  struct _Fog_TypeFlags < _Type_<A1, A2, A3, A4, A5, A6, A7, A8, A9> > { enum { TYPE = _Flags_ }; };

#define _FOG_TYPE_DECLARE_T10(_Type_, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, T10, A10, _Flags_) \
  template <T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10> \
  struct _Fog_TypeFlags < _Type_<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> > { enum { TYPE = _Flags_ }; };

// ===========================================================================
// [Fog::TypeInfo - Begin]
// ===========================================================================

#define C(_Category_) Fog::TYPE_CATEGORY_##_Category_
#define F(_Flag_) Fog::TYPE_FLAG_##_Flag_

// ===========================================================================
// [Fog::TypeInfo - C++]
// ===========================================================================

#if defined(FOG_CC_HAS_NATIVE_CHAR_TYPE)
_FOG_TYPE_DECLARE(char                         , C(SIMPLE) | F(POD) | F(INTEGER) | F(BIN_EQ))
#endif // FOG_CC_HAS_NATIVE_CHAR_TYPE

#if defined(FOG_CC_HAS_NATIVE_WCHAR_TYPE)
_FOG_TYPE_DECLARE(wchar_t                      , C(SIMPLE) | F(POD) | F(INTEGER) | F(BIN_EQ))
#endif // FOG_CC_HAS_NATIVE_WCHAR_TYPE

_FOG_TYPE_DECLARE(signed char                  , C(SIMPLE) | F(POD) | F(INTEGER) | F(BIN_EQ))
_FOG_TYPE_DECLARE(unsigned char                , C(SIMPLE) | F(POD) | F(INTEGER) | F(BIN_EQ))
_FOG_TYPE_DECLARE(short                        , C(SIMPLE) | F(POD) | F(INTEGER) | F(BIN_EQ))
_FOG_TYPE_DECLARE(unsigned short               , C(SIMPLE) | F(POD) | F(INTEGER) | F(BIN_EQ))
_FOG_TYPE_DECLARE(int                          , C(SIMPLE) | F(POD) | F(INTEGER) | F(BIN_EQ))
_FOG_TYPE_DECLARE(unsigned int                 , C(SIMPLE) | F(POD) | F(INTEGER) | F(BIN_EQ))
_FOG_TYPE_DECLARE(int64_t                      , C(SIMPLE) | F(POD) | F(INTEGER) | F(BIN_EQ))
_FOG_TYPE_DECLARE(uint64_t                     , C(SIMPLE) | F(POD) | F(INTEGER) | F(BIN_EQ))

// TODO: better long and ulong check.

#if !(defined(FOG_CC_GNU) && FOG_ARCH_BITS == 64) && !defined(FOG_CC_CLANG)
_FOG_TYPE_DECLARE(long                         , C(SIMPLE) | F(POD) | F(INTEGER) | F(BIN_EQ))
_FOG_TYPE_DECLARE(ulong                        , C(SIMPLE) | F(POD) | F(INTEGER) | F(BIN_EQ))
#endif // long / ulong

_FOG_TYPE_DECLARE(float                        , C(SIMPLE) | F(POD) | F(FLOAT  ))
_FOG_TYPE_DECLARE(double                       , C(SIMPLE) | F(POD) | F(DOUBLE ))

// ===========================================================================
// [Fog::TypeInfo - Fog]
// ===========================================================================

// [Fog/Core/Kernel]
_FOG_TYPE_DECLARE(Fog::EventLoopObserverListBase,C(MOVABLE)               | F(NO_CMP ) | F(NO_EQ ))
_FOG_TYPE_DECLARE(Fog::EventLoopPendingTask    , C(SIMPLE )               | F(NO_CMP ) | F(NO_EQ ))

// [Fog/Core/Math]
_FOG_TYPE_DECLARE(Fog::FloatBits               , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::DoubleBits              , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::IntervalF               , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::IntervalD               , C(SIMPLE )               | F(NO_CMP )            )

// [Fog/Core/Memory]
_FOG_TYPE_DECLARE(Fog::MemBuffer               , C(MOVABLE)               | F(NO_CMP ) | F(NO_EQ ))
_FOG_TYPE_DECLARE(Fog::MemPool                 , C(MOVABLE)               | F(NO_CMP ) | F(NO_EQ ))

// [Fog/Core/OS]
_FOG_TYPE_DECLARE(Fog::FileInfo                , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::Library                 , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::Stream                  , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )

// [Fog/Core/Threading]
_FOG_TYPE_DECLARE_T1(
                  Fog::Atomic,
                  typename, T                  , C(SIMPLE )                                       )

// [Fog/Core/Tools]
_FOG_TYPE_DECLARE(Fog::StubA                   , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::Ascii8                  , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::Local8                  , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::Utf8                    , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::StubW                   , C(SIMPLE )               | F(NO_CMP )            )

_FOG_TYPE_DECLARE_T2(
                  Fog::Hash,
                  typename, KeyT,
                  typename, ItemT              , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP)             )

_FOG_TYPE_DECLARE_T1(
                  Fog::List,
                  typename, ItemT              , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP)             )

_FOG_TYPE_DECLARE(Fog::CharA                   , C(SIMPLE )                            | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::CharW                   , C(SIMPLE )                            | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::Date                    , C(SIMPLE )               | F(OWN_CMP) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::FormatInt               , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::FormatReal              , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::Locale                  , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::LoggerRecord            , C(MOVABLE)               | F(NO_CMP ) | F(NO_EQ ))
_FOG_TYPE_DECLARE(Fog::ManagedStringW          , C(MOVABLE) | F(IMPLICIT) | F(OWN_CMP) | F(OWN_EQ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::Range                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::Random                  , C(SIMPLE )               | F(NO_CMP)  | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::RegExpA                 , C(MOVABLE) | F(IMPLICIT)                         )
_FOG_TYPE_DECLARE(Fog::RegExpW                 , C(MOVABLE) | F(IMPLICIT)                         )
_FOG_TYPE_DECLARE(Fog::StringA                 , C(MOVABLE) | F(IMPLICIT) | F(OWN_CMP) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::StringW                 , C(MOVABLE) | F(IMPLICIT) | F(OWN_CMP) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::TextCodec               , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::TextCodecState          , C(SIMPLE )               | F(NO_CMP ) | F(NO_EQ ))
_FOG_TYPE_DECLARE(Fog::Time                    , C(SIMPLE )                            | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::TimeDelta               , C(SIMPLE )                            | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::TimeTicks               , C(SIMPLE )                            | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::Var                     , C(MOVABLE) | F(IMPLICIT) | F(OWN_CMP) | F(OWN_EQ))

// [Fog/G2d/Geometry]
_FOG_TYPE_DECLARE(Fog::ArcF                    , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ArcD                    , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::BoxI                    , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::BoxF                    , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::BoxD                    , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::CBezierF                , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::CBezierD                , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ChordF                  , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ChordD                  , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::CircleF                 , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::CircleD                 , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::CoordF                  , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::CoordD                  , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::EllipseF                , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::EllipseD                , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::LineF                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::LineD                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::PathF                   , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::PathD                   , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::PathClipperF            , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::PathClipperD            , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::PathFlattenParamsF      , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::PathFlattenParamsD      , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::PathStrokerF            , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::PathStrokerD            , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::PathStrokerHints        , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::PathStrokerParamsF      , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::PathStrokerParamsD      , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::PieF                    , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::PieD                    , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::PointI                  , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::PointF                  , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::PointD                  , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::QBezierF                , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::QBezierD                , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::RectI                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::RectF                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::RectD                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::RoundF                  , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::RoundD                  , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ScalingF                , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ScalingD                , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::SizeI                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::SizeF                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::SizeD                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ThicknessI              , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ThicknessF              , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ThicknessD              , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::TransformF              , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::TransformD              , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::TriangleF               , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::TriangleD               , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))

// [Fog/G2d/Imaging]
_FOG_TYPE_DECLARE(Fog::Image                   , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::ImageBits               , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::ImageConverter          , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::ImageDither8Params      , C(SIMPLE )               | F(NO_CMP ) | F(NO_EQ) )
_FOG_TYPE_DECLARE(Fog::ImageFilter             , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::ImageFormatDescription  , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::ImagePalette            , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )

// [Fog/G2d/Imaging/Filters]
_FOG_TYPE_DECLARE(Fog::FeBlur                  , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::FeColorMatrix           , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::FeColorLut              , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::FeColorLutArray         , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::FeComponentFunction     , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::FeComponentFunctionGamma, C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::FeComponentFunctionLinear,C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::FeComponentTransfer     , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::FeMorphology            , C(SIMPLE )               | F(NO_CMP )            )

// [Fog/G2d/Painting]
_FOG_TYPE_DECLARE(Fog::PaintDeviceInfo         , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::PaintParamsF            , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::PaintParamsD            , C(MOVABLE)               | F(NO_CMP )            )

// [Fog/G2d/Source]
_FOG_TYPE_DECLARE(Fog::AcmykF                  , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::AcmykBaseF              , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::AhslF                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::AhslBaseF               , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::AhsvF                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::AhsvBaseF               , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::Argb32                  , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ArgbBase32              , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::Argb64                  , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ArgbBase64              , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ArgbF                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ArgbBaseF               , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::Color                   , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ColorBase               , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ColorStop               , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::ColorStopList           , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP ) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::ConicalGradientF        , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::ConicalGradientD        , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::GradientF               , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::GradientD               , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::LinearGradientF         , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::LinearGradientD         , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::Pattern                 , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP ) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::RadialGradientF         , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::RadialGradientD         , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::RectangularGradientF    , C(MOVABLE)               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::RectangularGradientD    , C(MOVABLE)               | F(NO_CMP )            )

// [Fog/G2d/Text]
_FOG_TYPE_DECLARE(Fog::Font                    , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP ) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::FontKerningChars        , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::FontKerningPairI        , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::FontKerningPairF        , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::FontManager             , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::FontMetricsF            , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::FontProvider            , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::GlyphBitmap             , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::GlyphOutline            , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::TextExtentsI            , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::TextExtentsF            , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::TextExtentsD            , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::TextLayout              , C(MOVABLE)               | F(NO_CMP ) | F(NO_EQ ))
_FOG_TYPE_DECLARE(Fog::TextRectI               , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::TextRectF               , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))
_FOG_TYPE_DECLARE(Fog::TextRectD               , C(SIMPLE )               | F(NO_CMP ) | F(BIN_EQ))

// [Fog/G2d/Tools]
_FOG_TYPE_DECLARE(Fog::Dpi                     , C(SIMPLE )               | F(NO_CMP )            )
_FOG_TYPE_DECLARE(Fog::MatrixF                 , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP ) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::MatrixD                 , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP ) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::Region                  , C(MOVABLE) | F(IMPLICIT) | F(NO_CMP ) | F(OWN_EQ))

#if defined(FOG_BUILD_UI)
// [Fog/UI/Engine]
_FOG_TYPE_DECLARE(Fog::UIEngineDisplayInfo     , C(SIMPLE )               | F(NO_CMP ) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::UIEngineEvent           , C(SIMPLE )               | F(NO_CMP ) | F(NO_EQ ))
_FOG_TYPE_DECLARE(Fog::UIEngineKeyboardInfo    , C(SIMPLE )               | F(NO_CMP ) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::UIEngineKeyboardState   , C(SIMPLE )               | F(NO_CMP ) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::UIEngineMouseInfo       , C(SIMPLE )               | F(NO_CMP ) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::UIEngineMouseState      , C(SIMPLE )               | F(NO_CMP ) | F(OWN_EQ))
_FOG_TYPE_DECLARE(Fog::UIEnginePaletteInfo     , C(SIMPLE )               | F(NO_CMP ) | F(OWN_EQ))
#endif // FOG_BUILD_UI

// ===========================================================================
// [Fog::TypeInfo - End]
// ===========================================================================

#undef F
#undef C

// [Guard]
#endif // _FOG_CORE_GLOBAL_TYPEINFO_H
