// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_LAYOUT_H
#define _FOG_GUI_LAYOUT_LAYOUT_H

// [Dependencies]
#include <Fog/Gui/Event.h>
#include <Fog/Core/Object.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Gui/Constants.h>
#include <Fog/Gui/Layout/LayoutItem.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

// ============================================================================
// [Fog::Layout]
// ============================================================================

const int DEFAULT_LAYOUT_SPACING = 6;
const int DEFAULT_WIDGET_MARGIN = 9;
const int DEFAULT_WINDOW_MARGIN = 11;

//! @brief Base class for all layouts.
struct FOG_API Layout : public LayoutItem
{
  FOG_DECLARE_OBJECT(Layout, LayoutItem)

    enum SizeConstraint {
      CONSTRAINT_DEFAULT,
      CONSTRAINT_NO,
      CONSTRAINT_MINIMUM_SIZE,
      CONSTRAINT_FIXED_SIZE,
      CONSTRAINT_MAXIMUM_SIZE,
      CONSTRAINT_MINIMUM_MAXIMUM_SIZE
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Layout();
  Layout(Widget *parent, Layout *parentLayout=0);

  Widget* getParentWidget() const
  {
    if (_toplevel) {
      if (_parentItem) {
        FOG_ASSERT(_parentItem->isLayout());
        return static_cast<Layout*>(_parentItem)->getParentWidget();
      } else {
        return 0;
      }
    }

    FOG_ASSERT(_parentItem && _parentItem->isWidget());
    return (Widget *)(_parentItem);
  }


  virtual ~Layout();

  int getTotalHeightForWidth(int width) const;

  IntSize getTotalSizeHint() const;
  IntSize getTotalMaximumSize() const;
  IntSize getTotalMinimumSize() const;

  FOG_INLINE void setSizeConstraint(SizeConstraint constraint) { _constraint = constraint; }
  FOG_INLINE SizeConstraint getSizeConstraint() const { return _constraint; }

  FOG_INLINE void setEnabled(bool enable) { _enabled = enable; }
  FOG_INLINE bool isEnabled() const { return _enabled; }

  FOG_INLINE int calcMargin(int margin) const;

  FOG_INLINE void setContentsMargins(int left, int right, int top, int bottom) { 
    IntMargins m = _contentmargins;
    _contentmargins.left = calcMargin(left);
    _contentmargins.right = calcMargin(right);
    _contentmargins.top = calcMargin(top);
    _contentmargins.bottom = calcMargin(bottom);
    if(_contentmargins.eq(m))
      return;

    _margininvalid = false;
    invalidateLayout();
  }

  FOG_INLINE void setContentsMargins(const IntMargins& m) { 
    setContentsMargins(m.left,m.right,m.top,m.bottom);
  }
  
  FOG_INLINE IntMargins getContentsMargins() { 
    if(_margininvalid) 
      setContentsMargins(_contentmargins); 
    return _contentmargins; 
  }

  FOG_INLINE IntRect getContentsRect() {
    if(_margininvalid) 
      setContentsMargins(_contentmargins);
    return _rect.adjusted(+_contentmargins.left, +_contentmargins.top, -_contentmargins.right, -_contentmargins.bottom);
  }

  virtual int getSpacing() const { 
    if (_spacing >=0) {
      return _spacing;
    } else {
      // arbitrarily prefer horizontal spacing to vertical spacing
      //return qSmartSpacing(this, QStyle::PM_LayoutHorizontalSpacing);
      if (!_parentItem) {
        return -1;
      } else if (_parentItem->isWidget()) {
        return DEFAULT_LAYOUT_SPACING;
      } else {
        FOG_ASSERT(_parentItem->isLayout());
        return static_cast<Layout *>(_parentItem)->getSpacing();
      }

      return 0;
    }  
  }
  virtual void setSpacing(int spacing) { _spacing = spacing; }
  

  //From LayoutItem  
  virtual uint32_t getLayoutExpandingDirections() const { return ORIENTATION_HORIZONTAL | ORIENTATION_VERTICAL; }
  virtual bool hasLayoutHeightForWidth() const { return false; } 
  virtual IntSize getLayoutMinimumSize() const { return IntSize(WIDGET_MIN_SIZE, WIDGET_MIN_SIZE); };
  virtual IntSize getLayoutMaximumSize() const { return IntSize(WIDGET_MAX_SIZE, WIDGET_MAX_SIZE); };
  virtual void setLayoutGeometry(const IntRect& r) {
    _rect = r;
  }
  virtual IntRect getLayoutGeometry() const {
    return _rect;
  }

  virtual void invalidateLayout() {
    _rect = IntRect();
    update();
  }  

  //need to be implemented by SubClass!
  virtual IntSize getLayoutSizeHint() const = 0;

  virtual void add(LayoutItem* item) {
    item->_withinLayout = this;
  }
  void add(Widget* widget);

  virtual LayoutItem* getAt(int index) const = 0;
  virtual LayoutItem* takeAt(int index) = 0;
  virtual int getLength() const = 0;

  virtual bool isEmpty() const;
  virtual void remove(LayoutItem* index);
  virtual int indexOf(LayoutItem*) const;

  void setAlignment(uint32_t alignment) {
    _alignment = alignment;
  }

  bool setAlignment(LayoutItem *item, uint32_t alignment)
  {
    if(item->_withinLayout == this) {
      item->_alignment = alignment;
      invalidateLayout();
      return true;
    }
    
    return false;
  }

  LayoutItem* _parentItem;

  void addChildLayout(Layout *l);

  static bool removeWidgetRecursively(LayoutItem *li, Widget *w);
  void reparentChildWidgets(Widget *mw);

  virtual void onLayout(LayoutEvent* e);

  FOG_EVENT_BEGIN()
    FOG_EVENT_DEF(EVENT_LAYOUT_REQUEST     , onLayout          , LayoutEvent    , OVERRIDE)    
  FOG_EVENT_END()

protected:
  bool activate();
  void update();
  void callSetGeometry(const IntSize& size);
  void activateRecursiveHelper(LayoutItem *item);

  IntMargins _contentmargins;

  int _spacing : 24;    //max Widget-Size
  uint _toplevel : 1;
  uint _enabled : 1;
  uint _activated : 1;
  uint _margininvalid : 1;
  uint _unused : 4;
  
  SizeConstraint _constraint;
  IntRect _rect;

private:
  FOG_DISABLE_COPY(Layout)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GUI_LAYOUT_LAYOUT_H
