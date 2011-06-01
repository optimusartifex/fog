// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_INTERNAL_CORE_P_H
#define _FOG_CORE_GLOBAL_INTERNAL_CORE_P_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>

// ============================================================================
// [Fog::Internal]
//
// Internal macros / functions used only by Fog. Not exported or visible to
// consumers.
// ============================================================================

#define NumT_(_Type_) typename _Type_##T<NumT>::T
#define DstT_(_Type_) typename _Type_##T<DstT>::T
#define SrcT_(_Type_) typename _Type_##T<SrcT>::T

#define NumT_T1(_Type_, _A1_) typename _Type_##T<NumT, _A1_>::T
#define DstT_T1(_Type_, _A1_) typename _Type_##T<DstT, _A1_>::T
#define SrcT_T1(_Type_, _A1_) typename _Type_##T<SrcT, _A1_>::T

#define NumI_(_Type_) _Type_##T<NumT>::T
#define DstI_(_Type_) _Type_##T<DstT>::T
#define SrcI_(_Type_) _Type_##T<SrcT>::T

#define NumI_(_Type_) _Type_##T<NumT>::T
#define DstI_(_Type_) _Type_##T<DstT>::T
#define SrcI_(_Type_) _Type_##T<SrcT>::T

#define NumI_T1(_Type_, _A1_) _Type_##T<NumT, _A1_>::T
#define DstI_T1(_Type_, _A1_) _Type_##T<DstT, _A1_>::T
#define SrcI_T1(_Type_, _A1_) _Type_##T<SrcT, _A1_>::T

// [Guard]
#endif // _FOG_CORE_GLOBAL_INTERNAL_CORE_P_H
