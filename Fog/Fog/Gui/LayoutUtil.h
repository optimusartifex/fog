// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUTUTIL_H
#define _FOG_GUI_LAYOUTUTIL_H

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/Object.h>
#include <Fog/Core/List.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Gui/Constants.h>
#include <Fog/Gui/GridLayout.h>

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

#endif // _FOG_GUI_LAYOUTUTIL_H
