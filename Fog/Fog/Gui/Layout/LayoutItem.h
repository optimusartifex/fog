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
#include <Fog/Gui/Layout/LayoutPolicy.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Layout;
struct Widget;
struct SpacerItem;

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

  LayoutItem(uint32_t alignment = 0);
  virtual ~LayoutItem();

  virtual bool isEmpty() const = 0;


  virtual IntSize getLayoutSizeHint() const = 0;
  virtual IntSize getLayoutMinimumSize() const = 0;
  virtual IntSize getLayoutMaximumSize() const = 0;
  virtual uint32_t getLayoutExpandingDirections() const = 0;  
  
  //reimpl if needed
  virtual bool hasLayoutHeightForWidth() const { return false; }
  virtual int getLayoutHeightForWidth(int width) const { return -1; }
  virtual int getLayoutMinimumHeightForWidth(int width) const { return getLayoutHeightForWidth(width); }

  virtual void invalidateLayout() { }

  //methods for doing real geometry changes
  virtual void setLayoutGeometry(const IntRect&) = 0;
  virtual IntRect getLayoutGeometry() const = 0;

  uint32_t getLayoutAlignment() const { return _alignment; }
  void setLayoutAlignment(uint32_t a) { _alignment = a; }

  static IntSize calculateMinimumSize(const IntSize& sizeHint, const IntSize& minSizeHint, const IntSize& minSize, const IntSize& maxSize, const LayoutPolicy& sizePolicy);
  static IntSize calculateMinimumSize(const Widget* w);

  static IntSize calculateMaximumSize(const IntSize& sizeHint, const IntSize& minSize, const IntSize& maxSize, const LayoutPolicy& sizePolicy, uint32_t align);
  static IntSize calculateMaximumSize(const Widget* w);
  
  uint32_t _alignment;
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GUI_LAYOUT_LAYOUTITEM_H
