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

  enum Direction { LEFTTORIGHT=0, RIGHTTOLEFT=1, TOPTOBOTTOM=0, BOTTOMTOTOP=1, DOWN = TOPTOBOTTOM, UP = BOTTOMTOTOP };

  //! @brief BoxLayout base class
  struct FOG_API BoxLayout : public Layout
  {    
    FOG_DECLARE_OBJECT(BoxLayout, Layout)
    BoxLayout(Widget *parent, int margin=-1, int spacing=-1);
    BoxLayout(int margin=-1, int spacing=-1);
    virtual ~BoxLayout();

    FOG_INLINE void addItem(LayoutItem *item) { 
      if(Layout::addChild(item) == -1) {
        return;
      }
      item->_layoutdata = new(std::nothrow) LayoutItem::FlexLayoutProperties();
    }

    virtual uint32_t getLayoutExpandingDirections() const;

    FOG_INLINE Direction getDirection() const { return static_cast<Direction>(_direction); }   
    FOG_INLINE void setDirection(Direction d) { _direction = d; }

  protected:
    FOG_INLINE bool isForward() const { return _direction == 0; }

    virtual void setLayoutGeometry(const IntRect &rect);    
    virtual int doLayout(const IntRect &rect) = 0;

    int calculateHorizontalGaps(bool collapse=true);
    int calculateVerticalGaps(bool collapse=true);
    
    uint32_t _allocated : 31;
    LayoutItem* _flexibles;

  private:
    uint32_t _direction : 1;
  };

  struct FOG_API HBoxLayout : public BoxLayout
  {
    HBoxLayout(Widget *parent, int margin=-1, int spacing=-1) : BoxLayout(parent,margin,spacing) {}
    HBoxLayout(int margin=-1, int spacing=-1) : BoxLayout(margin,spacing) {}

    virtual int doLayout(const IntRect &rect);

    virtual void calculateLayoutHint(LayoutHint& hint);
  };

  struct FOG_API VBoxLayout : public BoxLayout
  {
    VBoxLayout(Widget *parent, int margin=-1, int spacing=-1) : BoxLayout(parent,margin,spacing) {}
    VBoxLayout(int margin=-1, int spacing=-1) : BoxLayout(margin,spacing) {}

    virtual int doLayout(const IntRect &rect);
    virtual void calculateLayoutHint(LayoutHint& hint);
  };

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_LAYOUT_BOXLAYOUT_H
