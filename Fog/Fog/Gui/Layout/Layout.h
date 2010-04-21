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

    enum SizeConstraint { //not implemented yet
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
  virtual ~Layout();

  // --------------------------------------------------------------------------
  // [Layout Hierarchy]
  // --------------------------------------------------------------------------

  FOG_INLINE Widget* getParentWidget() const; 

  // --------------------------------------------------------------------------
  // [LayoutItem Handling]
  // --------------------------------------------------------------------------
  
  virtual LayoutItem* getAt(int index) const = 0;
  virtual LayoutItem* takeAt(int index) = 0;
  virtual int getLength() const = 0;
  virtual void add(LayoutItem* item);

  int indexOf(LayoutItem*) const;
  void remove(LayoutItem* item);
  
  void reparentChildWidgets(Widget *mw);
  static bool removeAllWidgets(LayoutItem *li, Widget *w);

  // --------------------------------------------------------------------------
  // [Total Layout Size Hints]
  // --------------------------------------------------------------------------
  IntSize getTotalSizeHint() const;
  IntSize getTotalMaximumSize() const;
  IntSize getTotalMinimumSize() const;
  int getTotalHeightForWidth(int width) const;

  // --------------------------------------------------------------------------
  // [Size Constraints]
  // --------------------------------------------------------------------------

  FOG_INLINE void setSizeConstraint(SizeConstraint constraint) { _constraint = constraint; }
  FOG_INLINE SizeConstraint getSizeConstraint() const { return _constraint; }

  // --------------------------------------------------------------------------
  // [Enable]
  // --------------------------------------------------------------------------
  FOG_INLINE void setEnabled(bool enable) { _enabled = enable; }
  FOG_INLINE bool isEnabled() const { return _enabled; }

  // --------------------------------------------------------------------------
  // [Margin]
  // --------------------------------------------------------------------------

  FOG_INLINE virtual int calcMargin(int margin, MarginPosition pos) const;  
  FOG_INLINE IntRect getContentsRect() const { return _rect.adjusted(+_contentmargin.left, +_contentmargin.top, -_contentmargin.right, -_contentmargin.bottom); }

  //helper method
  void calcContentMargins(int&side, int&top) const;
  
  // --------------------------------------------------------------------------
  // [Spacing]
  // --------------------------------------------------------------------------
  virtual int getSpacing() const;
  virtual void setSpacing(int spacing) { _spacing = spacing; }
  
  // --------------------------------------------------------------------------
  // [Expanding Directions]
  // --------------------------------------------------------------------------
  virtual uint32_t getLayoutExpandingDirections() const { return ORIENTATION_HORIZONTAL | ORIENTATION_VERTICAL; }

  // --------------------------------------------------------------------------
  // [Height For Width]
  // --------------------------------------------------------------------------
  virtual bool hasLayoutHeightForWidth() const { return false; } 
  virtual IntSize getLayoutMinimumSize() const { return IntSize(WIDGET_MIN_SIZE, WIDGET_MIN_SIZE); };
  virtual IntSize getLayoutMaximumSize() const { return IntSize(WIDGET_MAX_SIZE, WIDGET_MAX_SIZE); };

  //need to be implemented by SubClass!
  virtual IntSize getLayoutSizeHint() const = 0;

  // --------------------------------------------------------------------------
  // [Geometry]
  // --------------------------------------------------------------------------

  virtual void setLayoutGeometry(const IntRect& r) { _rect = r; }
  virtual IntRect getLayoutGeometry() const { return _rect; }
  void callSetGeometry(const IntSize& size);

  // --------------------------------------------------------------------------
  // [Invalidation]
  // --------------------------------------------------------------------------

  virtual void invalidateLayout() { _rect = IntRect(); update(); }  

  // --------------------------------------------------------------------------
  // [Visibility]
  // --------------------------------------------------------------------------

  virtual bool isEmpty() const;

  // --------------------------------------------------------------------------
  // [Alignment]
  // --------------------------------------------------------------------------

  bool setLayoutAlignment(LayoutItem *item, uint32_t alignment);  
  void addChildLayout(Layout *l);

  // --------------------------------------------------------------------------
  // [Update And Activation Of Layout]
  // --------------------------------------------------------------------------  

  bool activate();  
  void update();    
  void invalidActivateAll(LayoutItem *item, bool activate=true);

  // --------------------------------------------------------------------------
  // [Flex support]
  // -------------------------------------------------------------------------- 

  void addFlexItem(){ ++_flexcount; }
  void removeFlexItem() { --_flexcount; }
  bool hasFlexItems() const { return _flexcount > 0; } 

  // --------------------------------------------------------------------------
  // [Event Handler]
  // --------------------------------------------------------------------------  
  
  virtual void onLayout(LayoutEvent* e);

  FOG_EVENT_BEGIN()
    FOG_EVENT_DEF(EVENT_LAYOUT_REQUEST     , onLayout          , LayoutEvent    , OVERRIDE)    
  FOG_EVENT_END()

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------  

  LayoutItem* _parentItem;      //parentItem (Widget or Layout) of this Layout
  SizeConstraint _constraint;   
  IntRect _rect;                //current rect of this Layout

  int _flexcount;       //to easy know, if we have flex item within layout

  int _spacing : 24;    //max Widget-Size
  uint _toplevel : 1;  
  uint _activated : 1;
  uint _enabled : 1;
  uint _unused : 5; 

private:  
  FOG_DISABLE_COPY(Layout)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GUI_LAYOUT_LAYOUT_H
