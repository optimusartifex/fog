// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_VARID_H
#define _FOG_CORE_TOOLS_VARID_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

// ===========================================================================
// [_Fog_TypeId]
// ===========================================================================

template<typename Type>
struct _Fog_VarId {};

#define _FOG_VAR_DECLARE_ID(_Type_, _VarId_) \
template<> \
struct _Fog_VarId< _Type_ > { enum { ID = _VarId_ }; };

// ===========================================================================
// [Fog::]
// ===========================================================================

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ===========================================================================
// [Fog::VarId<>]
// ===========================================================================

template<typename Type>
struct VarId : public _Fog_VarId<Type> {};

//! @}

} // Fog namespace

// ===========================================================================
// [Fog::VarId<> - C++]
// ===========================================================================

#if defined(FOG_CC_HAVE_NATIVE_CHAR_TYPE)
_FOG_VAR_DECLARE_ID(char                       , Fog::VAR_TYPE_CHAR)
#endif // FOG_CC_HAVE_NATIVE_CHAR_TYPE
_FOG_VAR_DECLARE_ID(signed char                , Fog::VAR_TYPE_CHAR)
_FOG_VAR_DECLARE_ID(unsigned char              , Fog::VAR_TYPE_CHAR)

_FOG_VAR_DECLARE_ID(int                        , Fog::VAR_TYPE_INT32)
_FOG_VAR_DECLARE_ID(unsigned int               , Fog::VAR_TYPE_UINT32)
_FOG_VAR_DECLARE_ID(int64_t                    , Fog::VAR_TYPE_INT64)
_FOG_VAR_DECLARE_ID(uint64_t                   , Fog::VAR_TYPE_UINT64)

_FOG_VAR_DECLARE_ID(float                      , Fog::VAR_TYPE_FLOAT)
_FOG_VAR_DECLARE_ID(double                     , Fog::VAR_TYPE_DOUBLE)

_FOG_VAR_DECLARE_ID(bool                       , Fog::VAR_TYPE_BOOL)

// ===========================================================================
// [Fog::VarId<> - Fog]
// ===========================================================================

_FOG_VAR_DECLARE_ID(Fog::CharW                 , Fog::VAR_TYPE_CHAR)

_FOG_VAR_DECLARE_ID(Fog::StringA               , Fog::VAR_TYPE_STRINGA)
_FOG_VAR_DECLARE_ID(Fog::StringW               , Fog::VAR_TYPE_STRINGW)

_FOG_VAR_DECLARE_ID(Fog::List<Fog::StringA>    , Fog::VAR_TYPE_LIST_STRINGA)
_FOG_VAR_DECLARE_ID(Fog::List<Fog::StringW>    , Fog::VAR_TYPE_LIST_STRINGW)
_FOG_VAR_DECLARE_ID(Fog::List<Fog::Var>        , Fog::VAR_TYPE_LIST_VAR)

template<> struct _Fog_VarId< Fog::Hash<Fog::StringA, Fog::StringA> > { enum { ID = Fog::VAR_TYPE_HASH_STRINGA_STRINGA }; };
template<> struct _Fog_VarId< Fog::Hash<Fog::StringA, Fog::Var>     > { enum { ID = Fog::VAR_TYPE_HASH_STRINGA_VAR     }; };
template<> struct _Fog_VarId< Fog::Hash<Fog::StringW, Fog::StringW> > { enum { ID = Fog::VAR_TYPE_HASH_STRINGW_STRINGW }; };
template<> struct _Fog_VarId< Fog::Hash<Fog::StringW, Fog::Var>     > { enum { ID = Fog::VAR_TYPE_HASH_STRINGW_VAR     }; };

_FOG_VAR_DECLARE_ID(Fog::RegExpA               , Fog::VAR_TYPE_REGEXPA)
_FOG_VAR_DECLARE_ID(Fog::RegExpW               , Fog::VAR_TYPE_REGEXPW)

_FOG_VAR_DECLARE_ID(Fog::Locale                , Fog::VAR_TYPE_LOCALE)
_FOG_VAR_DECLARE_ID(Fog::Date                  , Fog::VAR_TYPE_DATE)
_FOG_VAR_DECLARE_ID(Fog::Time                  , Fog::VAR_TYPE_TIME)

_FOG_VAR_DECLARE_ID(Fog::PointI                , Fog::VAR_TYPE_POINTI)
_FOG_VAR_DECLARE_ID(Fog::PointF                , Fog::VAR_TYPE_POINTF)
_FOG_VAR_DECLARE_ID(Fog::PointD                , Fog::VAR_TYPE_POINTD)
_FOG_VAR_DECLARE_ID(Fog::SizeI                 , Fog::VAR_TYPE_SIZEI)
_FOG_VAR_DECLARE_ID(Fog::SizeF                 , Fog::VAR_TYPE_SIZEF)
_FOG_VAR_DECLARE_ID(Fog::SizeD                 , Fog::VAR_TYPE_SIZED)
_FOG_VAR_DECLARE_ID(Fog::BoxI                  , Fog::VAR_TYPE_BOXI)
_FOG_VAR_DECLARE_ID(Fog::BoxF                  , Fog::VAR_TYPE_BOXF)
_FOG_VAR_DECLARE_ID(Fog::BoxD                  , Fog::VAR_TYPE_BOXD)
_FOG_VAR_DECLARE_ID(Fog::RectI                 , Fog::VAR_TYPE_RECTI)
_FOG_VAR_DECLARE_ID(Fog::RectF                 , Fog::VAR_TYPE_RECTF)
_FOG_VAR_DECLARE_ID(Fog::RectD                 , Fog::VAR_TYPE_RECTD)
_FOG_VAR_DECLARE_ID(Fog::LineF                 , Fog::VAR_TYPE_LINEF)
_FOG_VAR_DECLARE_ID(Fog::LineD                 , Fog::VAR_TYPE_LINED)
_FOG_VAR_DECLARE_ID(Fog::QBezierF              , Fog::VAR_TYPE_QBEZIERF)
_FOG_VAR_DECLARE_ID(Fog::QBezierD              , Fog::VAR_TYPE_QBEZIERD)
_FOG_VAR_DECLARE_ID(Fog::CBezierF              , Fog::VAR_TYPE_CBEZIERF)
_FOG_VAR_DECLARE_ID(Fog::CBezierD              , Fog::VAR_TYPE_CBEZIERD)
_FOG_VAR_DECLARE_ID(Fog::TriangleF             , Fog::VAR_TYPE_TRIANGLEF)
_FOG_VAR_DECLARE_ID(Fog::TriangleD             , Fog::VAR_TYPE_TRIANGLED)
_FOG_VAR_DECLARE_ID(Fog::RoundF                , Fog::VAR_TYPE_ROUNDF)
_FOG_VAR_DECLARE_ID(Fog::RoundD                , Fog::VAR_TYPE_ROUNDD)
_FOG_VAR_DECLARE_ID(Fog::CircleF               , Fog::VAR_TYPE_CIRCLEF)
_FOG_VAR_DECLARE_ID(Fog::CircleD               , Fog::VAR_TYPE_CIRCLED)
_FOG_VAR_DECLARE_ID(Fog::EllipseF              , Fog::VAR_TYPE_ELLIPSEF)
_FOG_VAR_DECLARE_ID(Fog::EllipseD              , Fog::VAR_TYPE_ELLIPSED)
_FOG_VAR_DECLARE_ID(Fog::ArcF                  , Fog::VAR_TYPE_ARCF)
_FOG_VAR_DECLARE_ID(Fog::ArcD                  , Fog::VAR_TYPE_ARCD)
_FOG_VAR_DECLARE_ID(Fog::ChordF                , Fog::VAR_TYPE_CHORDF)
_FOG_VAR_DECLARE_ID(Fog::ChordD                , Fog::VAR_TYPE_CHORDD)
_FOG_VAR_DECLARE_ID(Fog::PieF                  , Fog::VAR_TYPE_PIEF)
_FOG_VAR_DECLARE_ID(Fog::PieD                  , Fog::VAR_TYPE_PIED)
_FOG_VAR_DECLARE_ID(Fog::PolygonF              , Fog::VAR_TYPE_POLYGONF)
_FOG_VAR_DECLARE_ID(Fog::PolygonD              , Fog::VAR_TYPE_POLYGOND)
_FOG_VAR_DECLARE_ID(Fog::PathF                 , Fog::VAR_TYPE_PATHF)
_FOG_VAR_DECLARE_ID(Fog::PathD                 , Fog::VAR_TYPE_PATHD)
_FOG_VAR_DECLARE_ID(Fog::Region                , Fog::VAR_TYPE_REGION)

_FOG_VAR_DECLARE_ID(Fog::TransformF            , Fog::VAR_TYPE_TRANSFORMF)
_FOG_VAR_DECLARE_ID(Fog::TransformD            , Fog::VAR_TYPE_TRANSFORMD)

_FOG_VAR_DECLARE_ID(Fog::MatrixF               , Fog::VAR_TYPE_MATRIXF)
_FOG_VAR_DECLARE_ID(Fog::MatrixD               , Fog::VAR_TYPE_MATRIXD)

_FOG_VAR_DECLARE_ID(Fog::Color                 , Fog::VAR_TYPE_COLOR)
_FOG_VAR_DECLARE_ID(Fog::Texture               , Fog::VAR_TYPE_TEXTUREF)
_FOG_VAR_DECLARE_ID(Fog::LinearGradientF       , Fog::VAR_TYPE_LINEAR_GRADIENTF)
_FOG_VAR_DECLARE_ID(Fog::LinearGradientD       , Fog::VAR_TYPE_LINEAR_GRADIENTD)
_FOG_VAR_DECLARE_ID(Fog::RadialGradientF       , Fog::VAR_TYPE_RADIAL_GRADIENTF)
_FOG_VAR_DECLARE_ID(Fog::RadialGradientD       , Fog::VAR_TYPE_RADIAL_GRADIENTD)
_FOG_VAR_DECLARE_ID(Fog::ConicalGradientF      , Fog::VAR_TYPE_CONICAL_GRADIENTF)
_FOG_VAR_DECLARE_ID(Fog::ConicalGradientD      , Fog::VAR_TYPE_CONICAL_GRADIENTD)
_FOG_VAR_DECLARE_ID(Fog::RectangularGradientF  , Fog::VAR_TYPE_RECTANGULAR_GRADIENTF)
_FOG_VAR_DECLARE_ID(Fog::RectangularGradientD  , Fog::VAR_TYPE_RECTANGULAR_GRADIENTD)

_FOG_VAR_DECLARE_ID(Fog::ColorStop             , Fog::VAR_TYPE_COLOR_STOP)
_FOG_VAR_DECLARE_ID(Fog::ColorStopList         , Fog::VAR_TYPE_COLOR_STOP_LIST)

_FOG_VAR_DECLARE_ID(Fog::Image                 , Fog::VAR_TYPE_IMAGE)
_FOG_VAR_DECLARE_ID(Fog::ImagePalette          , Fog::VAR_TYPE_IMAGE_PALETTE)
_FOG_VAR_DECLARE_ID(Fog::ImageFilter           , Fog::VAR_TYPE_IMAGE_FILTER)

_FOG_VAR_DECLARE_ID(Fog::Font                  , Fog::VAR_TYPE_FONT)

// [Guard]
#endif // _FOG_CORE_TOOLS_VARID_H
