// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_LAYOUTUTIL_H
#define _FOG_GUI_LAYOUT_LAYOUTUTIL_H

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/Object.h>
#include <Fog/Core/List.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Gui/Constants.h>
#include <Fog/Gui/Layout/GridLayout.h>

// TODO LAYOUT: Rename to LayoutUtil.cpp/.h

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

void calculateFlexOffsets(LayoutItem* head, int avail, int& used);
void calculateFlexOffsets(GridLayout::Column* head, int avail, int& used);
void calculateFlexOffsets(GridLayout::Row* head, int avail, int& used);

int collapseMargins(int margin1, int margin2);
int collapseMargins(int margin1, int margin2, int margin3);

//! @}

} // LayoutUtil namespace
} // Fog namespace

#endif // _FOG_GUI_LAYOUT_LAYOUTUTIL_H
