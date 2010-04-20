// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_BOXLAYOUT_H
#define _FOG_GUI_LAYOUT_BOXLAYOUT_H


// [Dependencies]
#include <Fog/Gui/Layout/Layout.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

// ============================================================================
// [Fog::BoxLayout]
// ============================================================================

  //! @brief Base class for all layouts.
  struct FOG_API BoxLayout : public Layout
  {
    FOG_DECLARE_OBJECT(BoxLayout, Layout)
    BoxLayout(Widget *parent, int margin=-1, int spacing=-1);
    BoxLayout(int margin=-1, int spacing=-1);
    ~BoxLayout();

    virtual void add(LayoutItem *item);    
    virtual uint32_t getLayoutExpandingDirections() const;
    virtual IntSize getLayoutSizeHint() const { return _hint; }
    virtual IntSize getLayoutMinimumSize() const  { return _min; }

    virtual int getLength() const; 
    virtual LayoutItem *getAt(int index) const;
    virtual LayoutItem *takeAt(int index);

    virtual void setLayoutGeometry(const IntRect &rect);

    virtual void invalidateLayout() {
      buildCache();
      Layout::invalidateLayout();
    }

  private:
    IntSize _min;
    IntSize _hint;
    int doLayout(const IntRect &rect);
    void buildCache();
    List<LayoutItem*> _itemList;
  };

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_LAYOUT_BOXLAYOUT_H
