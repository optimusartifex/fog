// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_LAYOUTITEM_H
#define _FOG_GUI_LAYOUT_LAYOUTITEM_H

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/Object.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Gui/Constants.h>
#include <Fog/Gui/Event.h>
#include <Fog/Gui/Layout/LayoutHint.h>

namespace Fog {

//! @addtogroup Fog_Gui_Layout
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Layout;
struct Widget;

// ============================================================================
// [Fog::LayoutItem]
// ============================================================================

//! @brief Base for @c Widget and @c Layout classes.
struct FOG_API LayoutItem : public Object
{
  FOG_DECLARE_OBJECT(LayoutItem, Object)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  LayoutItem();
  virtual ~LayoutItem();

  // --------------------------------------------------------------------------
  // [Layout Hints / ComputedHints]
  // --------------------------------------------------------------------------

  virtual const LayoutHint& getLayoutHint() const = 0;
  virtual void setLayoutHint(const LayoutHint& layoutHint) = 0;

  virtual const LayoutHint& getComputedLayoutHint() const = 0;
  virtual void computeLayoutHint() = 0;

  IntSize getSizeHint() const;
  IntSize getMinimumSize() const;
  IntSize getMaximumSize() const;

  void setSizeHint(const IntSize& sizeHint);
  void setMinimumSize(const IntSize& minimumSize);
  void setMaximumSize(const IntSize& maximumSize);

  // --------------------------------------------------------------------------
  // [Layout Policy]
  // --------------------------------------------------------------------------

  virtual uint32_t getLayoutPolicy() const = 0;
  virtual void setLayoutPolicy(uint32_t policy) = 0;

  // --------------------------------------------------------------------------
  // [Layout Height For Width]
  // --------------------------------------------------------------------------

  virtual bool hasHeightForWidth() const = 0;
  virtual int getHeightForWidth(int width) const = 0;

  // --------------------------------------------------------------------------
  // [Layout State]
  // --------------------------------------------------------------------------

  virtual bool isLayoutDirty() const = 0;
  virtual void invalidateLayout() const = 0;

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  virtual void onLayout(LayoutEvent* e);

  // --------------------------------------------------------------------------
  // [Event Map]
  // --------------------------------------------------------------------------

  FOG_EVENT_BEGIN()
    FOG_EVENT_DEF(EVENT_LAYOUT_SET        , onLayout          , LayoutEvent    , OVERRIDE)
    FOG_EVENT_DEF(EVENT_LAYOUT_REMOVE     , onLayout          , LayoutEvent    , OVERRIDE)
  FOG_EVENT_END()
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_LAYOUT_LAYOUTITEM_H
