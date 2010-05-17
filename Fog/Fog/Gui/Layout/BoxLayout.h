// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_BOXLAYOUT_H
#define _FOG_GUI_LAYOUT_BOXLAYOUT_H

// [Dependencies]
#include <Fog/Gui/Layout/Layout.h>

//! @addtogroup Fog_Gui_Layout
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

  struct LayoutProperty {
    LayoutProperty(Layout* layout) : _nextpercent(0), _layout(layout) {
      FOG_INIT_FLEX_PROPERTY();
      FOG_INIT_PERCENT_SIZE_PROPERTY();
    }

    LayoutItem* _nextpercent;
    Layout* _layout;

    FOG_DECLARE_FLEX_PROPERTY()
    FOG_DECLARE_PERCENT_SIZE_PROPERTY()
  };

  struct LayoutData : LayoutItem::FlexLayoutData {
    LayoutData(Layout* layout) : _user(layout) {}
    FOG_INLINE bool hasFlex() const { return _user.hasFlex(); }
    FOG_INLINE int getFlex() const { return _user.getFlex(); }
    FOG_INLINE void setFlex(int flex) { return _user.setFlex(flex); }
    LayoutProperty _user;
  };
  typedef LayoutProperty PropertyType;

  FOG_INLINE void addItem(LayoutItem *item, int flex=-1) { 
    if (Layout::addChild(item) == -1) {
      return;
    }
    LayoutData* data = new(std::nothrow) LayoutData(this);
    data->_user.setFlex(flex);

    item->_layoutdata = data;
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
  
  LayoutItem* _flexibles;
  uint32_t _allocated : 31;    
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
