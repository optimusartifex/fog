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
#include <Fog/Gui/Layout/LayoutHint.h>

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
  FOG_INLINE virtual int getLayoutMinimumHeightForWidth(int width) const { return getLayoutHeightForWidth(width); }

  // --------------------------------------------------------------------------
  // [LayoutHint]
  // --------------------------------------------------------------------------

  virtual void calculateLayoutHint(LayoutHint& hint) = 0;

  //TODO: maybe we can insert clearDirty() in special function, so that it is only
  //checked there, to have a real const getter!

  FOG_INLINE void clearDirty() {
    FOG_ASSERT(_dirty);
    if(!isEmpty()) {
      calculateLayoutHint(_cache); 
    } else {
      _cache._maximumSize = _cache._minimumSize = _cache._sizeHint = IntSize(0,0);
    }
   
    _dirty = 0; 
  }

  FOG_INLINE const LayoutHint& getLayoutHint() const {
    if(_dirty) {
      const_cast<LayoutItem*>(this)->clearDirty();
    }
    return _cache;
  }

  FOG_INLINE const IntSize& getLayoutSizeHint() const { return getLayoutHint()._sizeHint; }
  FOG_INLINE const IntSize& getLayoutMinimumSize() const { return getLayoutHint()._minimumSize; }
  FOG_INLINE const IntSize& getLayoutMaximumSize() const { return getLayoutHint()._maximumSize; }

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

  virtual void invalidateLayout() { _dirty = 1; }

  // --------------------------------------------------------------------------
  // [Geometry]
  // --------------------------------------------------------------------------

  virtual void setLayoutGeometry(const IntRect&) = 0;
  virtual IntRect getLayoutGeometry() const { return IntRect(); }

  // --------------------------------------------------------------------------
  // [Alignment]
  // --------------------------------------------------------------------------

  uint32_t getLayoutAlignment() const { return _alignment; }
  void setLayoutAlignment(uint32_t a) { _alignment = a; }

  // --------------------------------------------------------------------------
  // [Flex variable support]
  // --------------------------------------------------------------------------

  void setFlex(int flex);
  void removeLayoutStruct();
  FOG_INLINE int getFlex() const { return _flex; }
  FOG_INLINE bool hasFlex() const { return _flex != -1; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //Struct for Calculation of Flex-Values in LayoutManager
  struct LayoutStruct {
    LayoutStruct() : _min(0), _max(0), _hint(0), _flex(-1), _potential(0), _offset(0), _next(0) {}
    int _min;     //height or width
    int _max;     //height or width
    int _hint;    //height or width

    //Request/Response
    float _flex;  //the flex value for this Item

    int _potential; //for internal use
    int _offset;  //for internal use

    void* _next;   //for faster Flexible handling (could be different type)
  }* _layoutdata;

  IntMargins _contentmargin;
  LayoutHint _cache;
  Layout* _withinLayout;  //for fast identification of Layout, where this Item is inserted!
                          //maybe also used for layout pointer in widget

  int _flex : 24;
  uint32_t _alignment : 2;
  uint32_t _dirty : 1;
  uint32_t _unused : 5;
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GUI_LAYOUT_LAYOUTITEM_H
