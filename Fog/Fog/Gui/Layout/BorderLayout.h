// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_BORDERLAYOUT_H
#define _FOG_GUI_LAYOUT_BORDERLAYOUT_H

// [Dependencies]
#include <Fog/Gui/Layout/Layout.h>
#include <Fog/Core/List.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

  // ============================================================================
  // [Fog::BorderLayout]
  // ============================================================================

  //! @brief Base class for all layouts.
  struct FOG_API BorderLayout : public Layout
  {
    FOG_DECLARE_OBJECT(BorderLayout, Layout)
  };
}
#endif