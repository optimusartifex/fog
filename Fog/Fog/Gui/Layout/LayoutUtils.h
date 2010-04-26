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
#include <Fog/Gui/Layout/GridLayout.h>
//! @addtogroup Fog_Gui
//! @{

namespace Fog {

  // ============================================================================
  // [Forward Declarations]
  // ============================================================================

  struct LayoutItem;
//   template<class T>
//   void calculateFlexOffsets(T* head, int avail, int& used);

  void calculateFlexOffsets(LayoutItem* head, int avail, int& used);
  void calculateFlexOffsets(GridLayout::Column* head, int avail, int& used);
  void calculateFlexOffsets(GridLayout::Row* head, int avail, int& used);

  int collapseMargins(int margin1, int margin2);
  int collapseMargins(int margin1, int margin2, int margin3);
}
#endif