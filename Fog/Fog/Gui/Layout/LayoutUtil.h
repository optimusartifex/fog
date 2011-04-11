// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_LAYOUTUTIL_H
#define _FOG_GUI_LAYOUT_LAYOUTUTIL_H

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/System/Object.h>
#include <Fog/Gui/Global/Constants.h>
#include <Fog/Gui/Layout/GridLayout.h>

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

#endif // _FOG_GUI_LAYOUT_LAYOUTUTIL_H
