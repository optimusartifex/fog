// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Layout/BorderLayout.h>
#include <Fog/Gui/Layout/LayoutUtils.h>
#include <Fog/Gui/Widget.h>

FOG_IMPLEMENT_OBJECT(Fog::BorderLayout)

namespace Fog {
  void BorderLayout::calculateLayoutHint(LayoutHint& hint) {    
    int widthX=0, minWidthX=0;
    int heightX=0, minHeightX=0;
    int widthY=0, minWidthY=0;
    int heightY=0, minHeightY=0;

    int spacingX= getSpacing(), spacingY=getSpacing();
    int spacingSumX=-spacingX, spacingSumY=-spacingY;

    const List<LayoutItem*>& list = getList();

    _allocatedHeight = _allocatedWidth = 0;

    _horizontalflex = 0;
    _verticalflex = 0;

    for(int i=0;i<list.getLength();++i) {
      LayoutItem* item = list.at(i);
      const LayoutHint& hint = item->getLayoutHint();
      LayoutData* prop = item->getLayoutData<LayoutData>();

      int marginX = item->getContentLeftMargin() + item->getContentRightMargin();
      int marginY = item->getContentTopMargin() + item->getContentBottomMargin();

      if(prop->_edge & X_MASK) {
        widthY = Math::max(widthY, hint.getSizeHint().getWidth() + widthX + marginX);
        minWidthY = Math::max(minWidthY,hint.getMinimumSize().getWidth() + minWidthX + marginX);

        // Add the needed heights of this widget
        heightY += hint.getSizeHint().getHeight() + marginY;
        minHeightY += hint.getMinimumSize().getHeight() + marginY;

        // Add spacing
        spacingSumY += spacingY;

        //Allocated height
        _allocatedHeight += hint.getSizeHint().getHeight() + marginY + spacingY;

        if(prop->hasFlex()) {
          LayoutData* prop = item->getLayoutData<LayoutData>();
          prop->_flex = prop->getFlex();
          prop->_hint = hint.getSizeHint().getWidth();
          prop->_min = hint.getMinimumSize().getWidth();
          prop->_max = hint.getMaximumSize().getWidth();

          prop->_next = _horizontalflex;
          _horizontalflex = item;
        }
      } else if(prop->_edge & Y_MASK) {
        heightX = Math::max(heightX, hint.getSizeHint().getHeight() + heightY + marginY);
        minHeightX = Math::max(minHeightX, hint.getMinimumSize().getHeight() + minHeightY + marginY);

        // Add the needed widths of this widget
        widthX += hint.getSizeHint().getWidth() + marginX;
        minWidthX += hint.getMinimumSize().getWidth() + marginX;

        // Add spacing
        spacingSumX += spacingX;

        //Allocated width
        _allocatedWidth += hint.getSizeHint().getWidth() + marginX + spacingX;

        if(prop->hasFlex()) {
          LayoutData* prop = item->getLayoutData<LayoutData>();
          prop->_flex = prop->getFlex();
          prop->_hint = hint.getSizeHint().getHeight();
          prop->_min = hint.getMinimumSize().getHeight();
          prop->_max = hint.getMaximumSize().getHeight();          

          prop->_next = _verticalflex;
          _verticalflex = item;
        }
      } else {
        FOG_ASSERT(item == _center);
        // A centered widget must be added to both sums as
        // it stretches into the remaining available space.
        widthX += hint.getSizeHint().getWidth() + marginX;
        minWidthX += hint.getMinimumSize().getWidth() + marginX;

        heightY += hint.getSizeHint().getHeight() + marginY;
        minHeightY += hint.getMinimumSize().getHeight() + marginY;

        // Add spacing
        spacingSumX += spacingX;
        spacingSumY += spacingY;

        //Allocated height / Allocated width
        _allocatedHeight += hint.getSizeHint().getHeight() + marginY + spacingY;
        _allocatedWidth += hint.getSizeHint().getWidth() + marginX + spacingX;

        LayoutData* prop = item->getLayoutData<LayoutData>();
        prop->setFlex(prop->getFlex() < 1 ? 1:prop->getFlex()); //center has min flex value of 1!
      }
    }

    //set margins to LayoutMargins
    int marginX = getContentLeftMargin() + getContentRightMargin();
    int marginY = getContentTopMargin() + getContentBottomMargin();

    int minWidth = Math::max(minWidthX, minWidthY) + spacingSumX + marginX;
    int width = Math::max(widthX, widthY) + spacingSumX + marginX;
    int minHeight = Math::max(minHeightX, minHeightY) + spacingSumY + marginY;
    int height = Math::max(heightX, heightY) + spacingSumY + marginY;

    hint._minimumSize.set(minWidth, minHeight);
    hint._sizeHint.set(width, height);    
  }

  void BorderLayout::calculateVerticalFlexOffsets(int availHeight, int& allocatedHeight) {
    if (_verticalflex && allocatedHeight != availHeight) {
      if(_center) {
        LayoutData* prop = _center->getLayoutData<LayoutData>();
        prop->_next = _verticalflex;
        _verticalflex = _center;

        if(prop->getFlex() == -1) {
          prop->setFlex(1);
        }

        prop->_hint = _center->getLayoutSizeHint().getHeight();
        prop->_min = _center->getLayoutMinimumSize().getHeight();
        prop->_max = _center->getLayoutMaximumSize().getHeight();
        prop->_flex = prop->getFlex();
      }

      calculateFlexOffsets(_verticalflex, availHeight, allocatedHeight);

      //clean it for next run!
      if(_center) {
        LayoutData* prop = _center->getLayoutData<LayoutData>();
        _verticalflex = (LayoutItem*)prop->_next;
      }
    }
  }

  void BorderLayout::calculateHorizonzalFlexOffsets(int availWidth, int& allocatedWidth) {
    if(_horizontalflex && allocatedWidth != availWidth) {
      if(_center) {
        LayoutData* prop = _center->getLayoutData<LayoutData>();
        prop->_next = _horizontalflex;
        _horizontalflex = _center;

        if(prop->getFlex() == -1) {
          prop->setFlex(1);
        }

        prop->_hint = _center->getLayoutSizeHint().getWidth();
        prop->_min = _center->getLayoutMinimumSize().getWidth();
        prop->_max = _center->getLayoutMaximumSize().getWidth();
        prop->_flex = prop->getFlex();
      }

      calculateFlexOffsets(_horizontalflex, availWidth, allocatedWidth);

      //clean it for next run!
      if(_center) {
        FlexLayoutData* prop = _center->getLayoutData<LayoutData>();
        _horizontalflex = (LayoutItem*)prop->_next;
      }
    }
  }

#define CLEAN(VAR, MIN, MAX) VAR = VAR < MIN ? MIN : ( VAR > MAX ? MAX : VAR)

  void BorderLayout::setLayoutGeometry(const IntRect& rect) {
    int availWidth = rect.getWidth();
    int availHeight = rect.getHeight();
    
    //Do not allow to overlap 
    if(availHeight < getLayoutSizeHint().getHeight()) {
      availHeight = getLayoutSizeHint().getHeight();
    }
    //Do not allow to overlap 
    if(availWidth < getLayoutSizeHint().getWidth()) {
      availWidth = getLayoutSizeHint().getWidth();
    }

    int allocatedHeight = _allocatedHeight;
    int allocatedWidth = _allocatedWidth;

    const List<LayoutItem*>& list = getList();

    calculateHorizonzalFlexOffsets(availWidth, allocatedWidth);
    calculateVerticalFlexOffsets(availHeight, allocatedHeight);

    int curTop=rect.y + getContentTopMargin();
    int curLeft=rect.x + getContentLeftMargin();

    //TODO: add Support XY-Spacings!
    int spacingX= getSpacing();
    int spacingY= getSpacing();

    for(int i=0;i<list.getLength();++i) {
      LayoutItem* item = list.at(i);
      if(item == _center) {
        //Center is also within _children of Layout, but we handle
        //it differently
        continue;
      }
      
      const LayoutHint& hint = item->getLayoutHint();
      LayoutData* prop = item->getLayoutData<LayoutData>();

      int height = item->getLayoutSizeHint().getHeight();
      int width = item->getLayoutSizeHint().getWidth();
      int left = curLeft + item->getContentLeftMargin();
      int top = curTop + item->getContentTopMargin();

      int marginY = item->getContentYMargins();
      int marginX = item->getContentXMargins();

      if(prop->_edge & Y_MASK) {
        //NORTH or SOUTH
        width = availWidth - marginX;
        CLEAN(width, hint.getMinimumSize().getWidth(), hint.getMaximumSize().getWidth());        

        if(prop->_offset != 0) {
          height += prop->_offset;
          prop->_offset = 0;
        }

        // Update available height
        int used = height + marginY + spacingY;

        // Update coordinates, for next child
        if (prop->_edge == NORTH) {
          curTop += used;
        } else {
          top = curTop + (availHeight - height - item->getContentBottomMargin());
        }

        availHeight -= used;
      } else if(prop->_edge & X_MASK) {
        height = availHeight - marginY;
        CLEAN(height, hint.getMinimumSize().getHeight(), hint.getMaximumSize().getHeight()); 

        if(prop->_offset != 0) {
          width += prop->_offset;
          prop->_offset = 0;
        }

        // Update available width
        int used = width + marginX + spacingX;        

        // Update coordinates, for next child
        if (prop->_edge == WEST) {
          curLeft += used;
        } else {
          left = curLeft + (availWidth - width - item->getContentRightMargin());
        }
        availWidth -= used;
      }

      item->setLayoutGeometry(IntRect(left,top,width,height));
    }

    if(_center) {
      int width = availWidth - _center->getContentXMargins();
      int height = availHeight - _center->getContentYMargins();
      CLEAN(width, _center->getLayoutMinimumSize().getWidth(), _center->getLayoutMaximumSize().getWidth());
      CLEAN(height, _center->getLayoutMinimumSize().getHeight(), _center->getLayoutMaximumSize().getHeight());

      _center->setLayoutGeometry(IntRect(curLeft + _center->getContentLeftMargin(),curTop + _center->getContentTopMargin(),width,height));
    }
  }
}