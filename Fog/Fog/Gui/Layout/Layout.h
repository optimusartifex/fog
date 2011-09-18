// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_LAYOUT_H
#define _FOG_GUI_LAYOUT_LAYOUT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/Gui/Layout/LayoutItem.h>
#include <Fog/Gui/Widget/Event.h>

namespace Fog {

//! @addtogroup Fog_Gui_Layout
//! @{

// ============================================================================
// [Fog::Layout]
// ============================================================================

//! @brief Base class for all layouts.
struct FOG_API Layout : public LayoutItem
{
  FOG_DECLARE_OBJECT(Layout, LayoutItem)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Layout();
  Layout(Widget* parent, Layout* parentLayout = NULL);
  virtual ~Layout();

  // LAYOUT TODO: Move somewhere...
  virtual void onRemove(LayoutItem* item);

  // --------------------------------------------------------------------------
  // [Layout Hierarchy]
  // --------------------------------------------------------------------------

  Widget* getParentWidget() const;

  // --------------------------------------------------------------------------
  // [Total Layout Size Hints]
  // --------------------------------------------------------------------------
  SizeI getTotalSizeHint() const;
  SizeI getTotalMaximumSize() const;
  SizeI getTotalMinimumSize() const;
  int getTotalHeightForWidth(int width) const;

  // --------------------------------------------------------------------------
  // [Size Constraints]
  // --------------------------------------------------------------------------

  FOG_INLINE void setSizeConstraint(uint32_t constraint) { _constraint = constraint; }
  FOG_INLINE uint32_t getSizeConstraint() const { return _constraint; }

  // --------------------------------------------------------------------------
  // [Enable]
  // --------------------------------------------------------------------------
  FOG_INLINE void setEnabled(bool enable) { _enabled = enable; }
  FOG_INLINE bool isEnabled() const { return _enabled; }

  // --------------------------------------------------------------------------
  // [Margin]
  // --------------------------------------------------------------------------

  virtual int calcMargin(int margin, uint32_t location) const;
  RectI getContentsRect() const;

  //helper method
  void calcContentMargins(int& side, int& top) const;

  // --------------------------------------------------------------------------
  // [Spacing]
  // --------------------------------------------------------------------------

  virtual int getSpacing() const;
  virtual void setSpacing(int spacing);

  // --------------------------------------------------------------------------
  // [Expanding Directions]
  // --------------------------------------------------------------------------

  virtual uint32_t getLayoutExpandingDirections() const;

  // --------------------------------------------------------------------------
  // [Height For Width]
  // --------------------------------------------------------------------------

  virtual bool hasLayoutHeightForWidth() const;

  // --------------------------------------------------------------------------
  // [Geometry]
  // --------------------------------------------------------------------------

  virtual void setLayoutGeometry(const RectI& r);
  virtual RectI getLayoutGeometry() const;
  void callSetGeometry(const SizeI& size);

  // --------------------------------------------------------------------------
  // [Invalidation]
  // --------------------------------------------------------------------------

  virtual void updateLayout();
  virtual void invalidateLayout();

  void markAsDirty();

  // --------------------------------------------------------------------------
  // [Visibility]
  // --------------------------------------------------------------------------

  virtual bool isEmpty() const;

  // --------------------------------------------------------------------------
  // [Alignment]
  // --------------------------------------------------------------------------

  bool setLayoutAlignment(LayoutItem* item, uint32_t alignment);

  // --------------------------------------------------------------------------
  // [Update And Activation Of Layout]
  // --------------------------------------------------------------------------

  void update();
  bool activate();
  void invalidActivateAll(bool activate = true);

  // --------------------------------------------------------------------------
  // [Flex support]
  // --------------------------------------------------------------------------

  FOG_INLINE void addFlexItem() { ++_flexcount; }
  FOG_INLINE void removeFlexItem() { --_flexcount; }
  FOG_INLINE bool hasFlexItems() const { return _flexcount > 0; }

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

  //! @brief Parent item (Widget or Layout) of this Layout.
  LayoutItem* _parentItem;
  //! @brief Current rect of this Layout.
  RectI _rect;

  //! @brief Size constraint
  //! LAYOUT TODO: Not implemented).
  uint32_t _constraint;

  //! @brief To easy know whether we have flex item within layout.
  int _flexcount;

  //! @brief Max widget size.
  int _spacing : 24;
  uint _toplevel : 1;
  uint _activated : 1;
  uint _enabled : 1;
  //uint _invalidated : 1;
  uint _unused : 5;

  Layout* _nextactivate;

protected:

  // --------------------------------------------------------------------------
  // [LayoutItem Handling - only for internal use]
  // --------------------------------------------------------------------------

  FOG_INLINE LayoutItem* getAt(int index) const
  {
    if ((size_t)index < _children.getLength())
      return _children.getAt((uint)index);
    else
      return NULL;
  }

  FOG_INLINE LayoutItem* takeAt(int index)
  {
    if ((size_t)index < _children.getLength())
    {
      LayoutItem* t = _children.getAt(index);
      _children.removeAt(index);
      return t;
    }
    else
      return NULL;
  }

  FOG_INLINE int getLength() const
  { return (int)(uint)_children.getLength(); }

  int addChild(LayoutItem* item);

  int indexOf(LayoutItem*) const;
  void remove(LayoutItem* item);

  List<LayoutItem*> _children;

  void reparentChildWidgets(Widget* mw);
  static bool removeAllWidgets(LayoutItem* li, Widget* w);

private:
  int addChildLayout(Layout* l);

  _FOG_NO_COPY(Layout)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_LAYOUT_LAYOUT_H
