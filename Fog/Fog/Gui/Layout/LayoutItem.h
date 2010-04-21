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

  // --------------------------------------------------------------------------
  // [ContentMargins getter]
  // --------------------------------------------------------------------------

  FOG_INLINE IntMargins getContentMargins() const { return _contentmargin; }
  FOG_INLINE int getContentLeftMargin() const { return _contentmargin.left; }
  FOG_INLINE int getContentRightMargin() const { return _contentmargin.right; }
  FOG_INLINE int getContentTopMargin() const { return _contentmargin.top; }
  FOG_INLINE int getContentBottomMargin() const { return _contentmargin.bottom; }

  // --------------------------------------------------------------------------
  // [ContentMargins setter]
  // --------------------------------------------------------------------------

  enum MarginPosition {MARGIN_LEFT, MARGIN_RIGHT, MARGIN_TOP, MARGIN_BOTTOM};

  //!@brief this method allows to write a margin filter
  FOG_INLINE virtual int calcMargin(int margin, MarginPosition pos) const { return margin; }

  //TODO: only invalidate if margin really changes after calcMargin
  FOG_INLINE void setContentMargins(const IntMargins& m)  { setContentMargins(m.left,m.right,m.top,m.bottom); }
  FOG_INLINE void setContentMargins(int left, int right, int top, int bottom)  { _contentmargin.set(calcMargin(left,MARGIN_LEFT),calcMargin(right,MARGIN_RIGHT),calcMargin(top,MARGIN_TOP),calcMargin(bottom,MARGIN_BOTTOM)); invalidateLayout();}  
  FOG_INLINE void setContentLeftMargin(int m)  { _contentmargin.left = calcMargin(m,MARGIN_LEFT); invalidateLayout();}
  FOG_INLINE void setContentRightMargin(int m)  { _contentmargin.right = calcMargin(m,MARGIN_RIGHT); invalidateLayout();}
  FOG_INLINE void setContentTopMargin(int m)  { _contentmargin.top = calcMargin(m,MARGIN_TOP); invalidateLayout();}
  FOG_INLINE void setContentBottomMargin(int m)  { _contentmargin.bottom = calcMargin(m,MARGIN_BOTTOM); invalidateLayout();}

  // --------------------------------------------------------------------------
  // [Height For Width]
  // --------------------------------------------------------------------------

  virtual bool hasLayoutHeightForWidth() const { return false; }
  virtual int getLayoutHeightForWidth(int width) const { return -1; }
  virtual int getLayoutMinimumHeightForWidth(int width) const { return getLayoutHeightForWidth(width); }

  // --------------------------------------------------------------------------
  // [LayoutHint]
  // --------------------------------------------------------------------------

  virtual IntSize getLayoutSizeHint() const = 0;
  virtual IntSize getLayoutMinimumSize() const = 0;
  virtual IntSize getLayoutMaximumSize() const = 0;

  // --------------------------------------------------------------------------
  // [Calculate SizeHint]
  // --------------------------------------------------------------------------

  IntSize calculateMinimumSize() const;
  IntSize calculateMaximumSize() const;

  // --------------------------------------------------------------------------
  // [Expanding Directions]
  // --------------------------------------------------------------------------

  virtual uint32_t getLayoutExpandingDirections() const = 0;

  // --------------------------------------------------------------------------
  // [Visibility]
  // --------------------------------------------------------------------------

  virtual bool isEmpty() const = 0;  

  // --------------------------------------------------------------------------
  // [Cache Handling]
  // --------------------------------------------------------------------------

  virtual void invalidateLayout() { }

  // --------------------------------------------------------------------------
  // [Geometry]
  // --------------------------------------------------------------------------

  virtual void setLayoutGeometry(const IntRect&) = 0;
  virtual IntRect getLayoutGeometry() const { return IntRect(); }   //who needs this?

  // --------------------------------------------------------------------------
  // [Alignment]
  // --------------------------------------------------------------------------

  uint32_t getLayoutAlignment() const { return _alignment; }
  void setLayoutAlignment(uint32_t a) { _alignment = a; }

  // --------------------------------------------------------------------------
  // [Flex variable support]
  // --------------------------------------------------------------------------

  void setFlex(int flex);
  void removeFlexibles();
  FOG_INLINE int getFlex() const { return _flex; }
  FOG_INLINE bool hasFlex() const { return _flex != -1; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //Struct for Calculation of Flex-Values in LayoutManager
  struct Flexibles {
    Flexibles() : _min(0), _max(0), _hint(0), _flex(-1), _potential(0), _offset(0) {}
    int _min;
    int _max;
    int _hint;

    //Request/Response
    float _flex;
    int _potential;

    int _offset;
  }* _flexibles;

  IntMargins _contentmargin; 
  Layout* _withinLayout;  //for fast identification of Layout, where this Item is inserted!
                          //maybe also used for layout pointer in widget

  int _flex : 24;
  uint32_t _alignment : 2;
  uint32_t _unused : 6;
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GUI_LAYOUT_LAYOUTITEM_H
