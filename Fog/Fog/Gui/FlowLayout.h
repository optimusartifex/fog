// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_FLOWLAYOUT_H
#define _FOG_GUI_FLOWLAYOUT_H

// [Dependencies]
#include <Fog/Gui/Layout.h>
#include <Fog/Core/List.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

// ============================================================================
// [Fog::FlowLayout]
// ============================================================================

//! @brief Base class for all layouts.
struct FOG_API FlowLayout : public Layout
{
  FOG_DECLARE_OBJECT(FlowLayout, Layout)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FlowLayout(Widget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
  FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
  virtual ~FlowLayout();
  
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  int getHorizontalSpacing() const;
  int getVerticalSpacing() const;

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual void add(LayoutItem *item);

  virtual SizeI getLayoutSizeHint() const;
  virtual uint32_t getLayoutExpandingDirections() const;
  virtual bool hasLayoutHeightForWidth() const;
  virtual int getLayoutHeightForWidth(int) const;
  virtual SizeI getLayoutMinimumSize() const;

  virtual void setLayoutGeometry(const RectI &rect);

private:
  int doLayout(const RectI &rect, bool testOnly) const;
  int _hSpace;
  int _vSpace;
};

} // Fog namespace

#endif // _FOG_GUI_FLOWLAYOUT_H
