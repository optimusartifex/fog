// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Layout/LayoutUtils.h>
#include <Fog/Core.h>
#include <Fog/Gui/Layout/GridLayout.h>

namespace Fog {

  template<class T> 
  void calculateFlexOffsetsT(T* head, int avail, int& used) {
    if(head == 0) return;

    bool grow = avail > used;
    int remaining = Math::abs(avail - used);        

    FOG_ASSERT(remaining != 0);   //check: is this too hard??

    bool set = false;
    float flexSum = 0;

    // Continue as long as we need to do anything
    while (remaining != 0)
    {
      bool breakfrom = true;
      // Find minimum potential for next correction
      float flexStep = FLOAT_MAX;
      T* item = head;      

      //calculate sum of all flexes and initialize variables!
      //TODO: can be done on setter so we can save this loop!
      while(item) {
        LayoutItem::FlexLayoutProperties* child = static_cast<LayoutItem::FlexLayoutProperties*>(item->_layoutdata);

        if(!set) {
          child->_offset = 0;
          child->_potential = grow ? child->_max - child->_hint : child->_hint - child->_min;
          child->_flex = grow ? child->_flex : (1 / child->_flex);
          flexSum += child->_flex;
        }

        if (child->_potential > 0) {
          flexStep = Math::min<float>(flexStep, child->_potential / child->_flex);          
        }

        item = (T*)child->_next;
      }

      if(flexSum==0)  //should not happen, but just to be sure :-)
        break;
      
      set = true;
      
      //never take more space than the parent has!
      flexStep = Math::min<float>(remaining, flexStep * flexSum) / flexSum; //calculate the divisor for this round

      //start with the calculation
      item = head;
      int roundingOffset = 0;      

      while(item) {
        LayoutItem::FlexLayoutProperties* child = static_cast<LayoutItem::FlexLayoutProperties*>(item->_layoutdata);

        if (child->_potential > 0)
        {
          breakfrom = false;
          // Compute offset for this step
          int currentOffset = Math::min<int>(remaining, child->_potential, ::ceil(flexStep * child->_flex));
          // Update child status
          child->_potential -= currentOffset;

          child->_offset += grow? currentOffset : -currentOffset;
          used+=child->_offset;

          // Update parent status
          remaining -= currentOffset;
        }

        item = (T*)child->_next;
      }

      if(breakfrom)
        break;
    }
  }

  void calculateFlexOffsets(LayoutItem* head, int avail, int& used) {
    return calculateFlexOffsetsT<LayoutItem>(head,avail,used);
  }
  void calculateFlexOffsets(GridLayout::Column* head, int avail, int& used) {
    return calculateFlexOffsetsT<GridLayout::Column>(head,avail,used);
  }
  void calculateFlexOffsets(GridLayout::Row* head, int avail, int& used) {
    return calculateFlexOffsetsT<GridLayout::Row>(head,avail,used);
  }

#define CALC(VALUE)\
  if(VALUE < 0) {\
  min = Math::min(min, VALUE);\
  } else if(VALUE > 0) {\
  max = Math::max(max, VALUE);\
  }\

  int collapseMargins(int margin1, int margin2, int margin3) {
    int max=0, min=0;
    CALC(margin1);
    CALC(margin2);
    CALC(margin3);
    return max + min;
  }

  int collapseMargins(int margin1, int margin2) {
    int max=0, min=0;
    CALC(margin1);
    CALC(margin2);
    return max + min;
  }

#undef CALC



}