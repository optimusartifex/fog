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


  //Margin-Support
  FOG_INLINE IntMargins getContentMargins() const { return _contentmargin; }
  FOG_INLINE int getContentLeftMargin() const { return _contentmargin.left; }
  FOG_INLINE int getContentRightMargin() const { return _contentmargin.right; }
  FOG_INLINE int getContentTopMargin() const { return _contentmargin.top; }
  FOG_INLINE int getContentBottomMargin() const { return _contentmargin.bottom; }
  FOG_INLINE void setContentMargins(const IntMargins& m)  { _contentmargin = m; }
  FOG_INLINE void setContentLeftMargin(int m)  { _contentmargin.left = m; }
  FOG_INLINE void setContentRightMargin(int m)  { _contentmargin.right = m; }
  FOG_INLINE void setContentTopMargin(int m)  { _contentmargin.top = m; }
  FOG_INLINE void setContentBottomMargin(int m)  { _contentmargin.bottom = m; }
  FOG_INLINE void setContentMargins(int left, int right, int top, int bottom)  { _contentmargin.set(left,right,top,bottom); }


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

  static IntSize calculateMinimumSize(const Widget* w);
  static IntSize calculateMaximumSize(const Widget* w);
  
  uint32_t _alignment;
  Layout* _withinLayout;  //for fast identification of Layout, where this Item is inserted!

  int _flex;

  void setFlex(int flex);

  FOG_INLINE int getFlex() const { return _flex; }
  FOG_INLINE bool hasFlex() const { return _flex != -1; }

  FOG_INLINE void removeFlexibles() {
    if(_flexibles) {
      delete _flexibles;
      _flexibles = 0;  
    }
  }

  //Struct for Calculation of Flex-Values in LayoutManager
  struct Flexibles {
    Flexibles() : _min(0), _max(0), _hint(0), _flex(-1), _potential(0), _offset(0) {}
    int _min;
    int _max;
    int _hint;

    //Request/Response
    float _flex;

    //Response
    int _potential;
    int _offset;
  }* _flexibles;

  IntMargins _contentmargin; 
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GUI_LAYOUT_LAYOUTITEM_H
