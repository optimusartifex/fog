// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_FLOWLAYOUT_H
#define _FOG_GUI_LAYOUT_FLOWLAYOUT_H

// [Dependencies]
#include <Fog/Gui/Layout/Layout.h>
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
    FlowLayout(Widget *parent, int margin=-1, int hSpacing=-1, int vSpacing=-1);
    FlowLayout(int margin=-1, int hSpacing=-1, int vSpacing=-1);
    ~FlowLayout();
    
    int horizontalSpacing() const;
    int verticalSpacing() const;    

    virtual void add(LayoutItem *item);

    virtual IntSize getLayoutSizeHint() const;
    virtual uint32_t getLayoutExpandingDirections() const;
    virtual bool hasLayoutHeightForWidth() const;
    virtual int getLayoutHeightForWidth(int) const;
    virtual IntSize getLayoutMinimumSize() const;

    virtual int getLength() const; 
    virtual LayoutItem *getAt(int index) const;
    virtual LayoutItem *takeAt(int index);

    virtual void setLayoutGeometry(const IntRect &rect);

  private:
    int doLayout(const IntRect &rect, bool testOnly) const;
    List<LayoutItem*> _itemList;
    int _hSpace;
    int _vSpace;
  };

}

#endif