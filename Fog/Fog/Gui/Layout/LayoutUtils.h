// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_LAYOUTUTILS_H
#define _FOG_GUI_LAYOUT_LAYOUTUTILS_H

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/Object.h>
#include <Fog/Core/List.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Gui/Constants.h>
//! @addtogroup Fog_Gui
//! @{

namespace Fog {

  // ============================================================================
  // [Forward Declarations]
  // ============================================================================

  struct LayoutItem;

  void calculateFlexOffsets(const List<LayoutItem*>& arr, int avail, int& used);
  int calculateHorizontalGaps(const List<LayoutItem*>& children, int spacing=0, bool collapse=true);
  int calculateVerticalGaps(const List<LayoutItem*>& children, int spacing, bool collapse);

  int collapseMargins(int margin1, int margin2);
  int collapseMargins(int margin1, int margin2, int margin3);

}
#endif