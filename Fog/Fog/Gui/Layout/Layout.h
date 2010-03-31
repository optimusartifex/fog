// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_LAYOUT_H
#define _FOG_GUI_LAYOUT_LAYOUT_H

// [Dependencies]
#include <Fog/Core/Event.h>
#include <Fog/Core/Object.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Gui/Constants.h>
#include <Fog/Gui/Layout/LayoutHint.h>
#include <Fog/Gui/Layout/LayoutItem.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

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
  virtual ~Layout();

  // --------------------------------------------------------------------------
  // [Layout Params]
  // --------------------------------------------------------------------------

  struct FOG_HIDDEN Params
  {
    // [Construction / Destruction]

    FOG_INLINE Params(uint32_t type) : _type(type) {}

    // [Members]

    uint32_t _type;
  };

  // --------------------------------------------------------------------------
  // [Layout Hints]
  // --------------------------------------------------------------------------

  virtual const LayoutHint& getLayoutHint() const;
  virtual void setLayoutHint(const LayoutHint& layoutHint);

  virtual const LayoutHint& getComputedLayoutHint() const;
  virtual void computeLayoutHint();

  virtual uint32_t getLayoutPolicy() const;
  virtual void setLayoutPolicy(uint32_t policy);

  // --------------------------------------------------------------------------
  // [Layout Height For Width]
  // --------------------------------------------------------------------------

  virtual bool hasHeightForWidth() const;
  virtual int getHeightForWidth(int width) const;

  // --------------------------------------------------------------------------
  // [Layout State]
  // --------------------------------------------------------------------------

  virtual bool isLayoutDirty() const;
  virtual void invalidateLayout() const;

  // --------------------------------------------------------------------------
  // [Parents]
  // --------------------------------------------------------------------------

  LayoutItem* getParentItem() const;
  Widget* getParentWidget() const;

  // --------------------------------------------------------------------------
  // [Reparent]
  // --------------------------------------------------------------------------

  virtual void reparentChildren() = 0;

  // --------------------------------------------------------------------------
  // [Event Map]
  // --------------------------------------------------------------------------

  FOG_EVENT_BEGIN()
    FOG_EVENT_DEF(EVENT_LAYOUT_ITEM_ADD    , onLayout          , LayoutEvent    , OVERRIDE)
    FOG_EVENT_DEF(EVENT_LAYOUT_ITEM_REMOVE , onLayout          , LayoutEvent    , OVERRIDE)
  FOG_EVENT_END()

protected:
  LayoutItem* _parentItem;
  LayoutHint _layoutHint;

  bool _isLayoutDirty;

private:
  friend struct Widget;

  FOG_DISABLE_COPY(Layout)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GUI_LAYOUT_LAYOUT_H
