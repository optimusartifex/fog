// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_LAYOUT_LAYOUTUTIL_H
#define _FOG_UI_LAYOUT_LAYOUTUTIL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/UI/Layout/GridLayout.h>

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct LayoutItem;

// ============================================================================
// [Fog::LayoutUtil]
// ============================================================================

namespace LayoutUtil {

//! @addtogroup Fog_Gui_Layout
//! @{

FOG_API void calculateFlexOffsets(LayoutItem* head, int avail, int& used);
FOG_API void calculateFlexOffsets(GridLayout::Column* head, int avail, int& used);
FOG_API void calculateFlexOffsets(GridLayout::Row* head, int avail, int& used);

FOG_API int collapseMargins(int margin1, int margin2);
FOG_API int collapseMargins(int margin1, int margin2, int margin3);

//! @}

} // LayoutUtil namespace
} // Fog namespace

#endif // _FOG_UI_LAYOUT_LAYOUTUTIL_H
