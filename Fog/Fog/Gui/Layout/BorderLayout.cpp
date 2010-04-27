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

    for(int i=0;i<list.getLength();++i) {
      LayoutItem* item = list.at(i);
      const LayoutHint& hint = item->getLayoutHint();
      LayoutProperty* prop = static_cast<LayoutProperty*>(item->_layoutdata);

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

        //Allocated height /Allocated width
        _allocatedHeight += hint.getSizeHint().getHeight() + marginY + spacingY;
        _allocatedWidth += hint.getSizeHint().getWidth() + marginX + spacingX;
      }
    }

    int minWidth = Math::max(minWidthX, minWidthY) + spacingSumX;
    int width = Math::max(widthX, widthY) + spacingSumX;
    int minHeight = Math::max(minHeightX, minHeightY) + spacingSumY;
    int height = Math::max(heightX, heightY) + spacingSumY;

    hint._minimumSize.set(minWidth, minHeight);
    hint._sizeHint.set(width, height);
  }

  void BorderLayout::setLayoutGeometry(const IntRect& rect) {
    int availWidth = rect.getWidth();
    int availHeight = rect.getHeight();

    const List<LayoutItem*>& list = getList();

    int centerheightoffset = 0;
    int centerwidthoffset = 0;

    //calculate horizontal flex
    if (hasFlex()) {
      if(_allocatedWidth != availWidth) {
        
      }

      if (_allocatedHeight != availHeight) {

      }
    } else {
      //all flex come to center!
      centerheightoffset = availHeight - _allocatedHeight;
      centerwidthoffset = availWidth - _allocatedWidth;
    }

    int used;
    int nextTop=0, nextLeft=0;
    int spacingX= getSpacing(), spacingY=getSpacing();

    int len = list.getLength()+_center? 1 : 0;

    for(int i=0;i<len;++i) {
      LayoutItem* item = _center;
      if(i < list.getLength()) {
        item = list.at(i);
        if(item == _center) {
          continue;
        }
      }
      
      const LayoutHint& hint = item->getLayoutHint();
      LayoutProperty* prop = static_cast<LayoutProperty*>(item->_layoutdata);

      int height = hint.getSizeHint().getHeight();
      int width = hint.getSizeHint().getWidth();

      int minwidth = hint.getMinimumSize().getWidth();
      int maxwidth = hint.getMaximumSize().getWidth();
      int minheight = hint.getMinimumSize().getHeight();
      int maxheight = hint.getMaximumSize().getHeight();
      int marginX = item->getContentLeftMargin() + item->getContentRightMargin();
      int marginY = item->getContentTopMargin() + item->getContentBottomMargin();

      int zheight = availHeight - marginY;
      int zwidth = availWidth - marginX;

      int left = nextLeft + item->getContentLeftMargin();
      int top = nextTop + item->getContentTopMargin();

      // Limit width to min/max
      if (zwidth < minwidth) {
        zwidth = minwidth;
      } else if (zwidth > maxwidth) {
        zwidth = maxwidth;
      }

      // Limit height to min/max
      if (zheight < minheight) {
        zheight = minheight;
      } else if (zheight > maxheight) {
        zheight = maxheight;
      }

      if(prop->_edge & Y_MASK) {
        width = zwidth;

        // Update available height
        used = height + marginY + spacingY;

        // Update coordinates, for next child
        if (prop->_edge == NORTH) {
          nextTop += used;
        } else {
          top = nextTop + (availHeight - height - item->getContentBottomMargin());
        }

        availHeight -= used;
      } else if(prop->_edge & X_MASK) {
        height = zheight;

        // Update available height
        used = width + marginX + spacingX;        

        // Update coordinates, for next child
        if (prop->_edge == WEST) {
          nextLeft += used;
        } else {
          left = nextLeft + (availWidth - width - item->getContentRightMargin());
        }
        availWidth -= used;
      } else {
        // Calculated width/height
        FOG_ASSERT(item == _center);
        width += centerwidthoffset;
        height += centerheightoffset;
      }

      item->setLayoutGeometry(IntRect(left,top,width,height));
    }
  }
}