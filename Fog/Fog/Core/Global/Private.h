// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_PRIVATE_H
#define _FOG_CORE_GLOBAL_PRIVATE_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

// ============================================================================
// [Template helpers used only by Fog-Framework]
// ============================================================================

#define NumT_(_Type_) typename _Type_##T<NumT>::T
#define CharT_(_Type_) typename _Type_##T<CharT>::T
#define DstT_(_Type_) typename _Type_##T<DstT>::T
#define SrcT_(_Type_) typename _Type_##T<SrcT>::T

#define NumT_T1(_Type_, _A1_) typename _Type_##T<NumT, _A1_>::T
#define CharT_T1(_Type_, _A1_) typename _Type_##T<CharT, _A1_>::T
#define DstT_T1(_Type_, _A1_) typename _Type_##T<DstT, _A1_>::T
#define SrcT_T1(_Type_, _A1_) typename _Type_##T<SrcT, _A1_>::T

#define NumI_(_Type_) _Type_##T<NumT>::T
#define CharI_(_Type_) _Type_##T<CharT>::T
#define DstI_(_Type_) _Type_##T<DstT>::T
#define SrcI_(_Type_) _Type_##T<SrcT>::T

#define NumI_T1(_Type_, _A1_) _Type_##T<NumT, _A1_>::T
#define CharI_T1(_Type_, _A1_) _Type_##T<CharT, _A1_>::T
#define DstI_T1(_Type_, _A1_) _Type_##T<DstT, _A1_>::T
#define SrcI_T1(_Type_, _A1_) _Type_##T<SrcT, _A1_>::T

#define CharT_Char typename _CharT<CharT>::T
#define CharT_Type typename _CharT<CharT>::T::Type
#define CharT_Value typename _CharT<CharT>::T::Value
#define CharT_Func _CharT<CharT>::T

#define SrcT_Char typename _CharT<SrcT>::T

// [Guard]
#endif // _FOG_CORE_GLOBAL_PRIVATE_H
