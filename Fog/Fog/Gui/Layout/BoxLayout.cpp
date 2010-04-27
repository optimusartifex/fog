// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Layout/BoxLayout.h>
#include <Fog/Gui/Widget.h>
#include <Fog/Gui/Layout/LayoutUtils.h>

FOG_IMPLEMENT_OBJECT(Fog::BoxLayout)

namespace Fog {

// ============================================================================
// [Fog::BoxLayout]
// ============================================================================

  BoxLayout::BoxLayout(Widget *parent, int margin, int spacing)
    : Layout(parent), _direction(LEFTTORIGHT)
  {
    setSpacing(spacing);
    setContentMargins(margin, margin, margin, margin);
  }

  BoxLayout::BoxLayout(int margin, int spacing) : _direction(LEFTTORIGHT)
  {
    setSpacing(spacing);
    setContentMargins(margin, margin, margin, margin);
  }

  BoxLayout::~BoxLayout()
  {
    LayoutItem *item;
    while ((item = takeAt(0)))
      delete item;
  }

  uint32_t BoxLayout::getLayoutExpandingDirections() const
  {
    return ORIENTATION_HORIZONTAL;
  }

  void BoxLayout::setLayoutGeometry(const IntRect &rect)
  {
    //TODO: check oldrect and newrect if we really need to reposition
    Layout::setLayoutGeometry(rect);
    doLayout(rect);
  }

  int BoxLayout::calculateHorizontalGaps(bool collapse) {
    if(_children.getLength() == 0)
      return 0;

    int spacing = getSpacing();
    if(spacing < 0) spacing = 0;

    int gaps = 0;

    if (collapse)
    {
      for (int i=1; i<_children.getLength(); ++i) {
        gaps += collapseMargins(spacing, _children.at(i-1)->getContentRightMargin(), _children.at(i)->getContentLeftMargin());
      }

      // Add last child
      gaps += _children.at(0)->getContentLeftMargin() + _children.at(_children.getLength()-1)->getContentRightMargin();
    }
    else
    {
      // Simple adding of all margins
      for (int i=1; i<_children.getLength(); ++i) {
        gaps += _children.at(i)->getContentLeftMargin() + _children.at(i)->getContentRightMargin();
      }

      // Add spacing
      gaps += (spacing * (_children.getLength()-1));
    }

    return gaps;
  }


  int BoxLayout::calculateVerticalGaps(bool collapse) {
    if(_children.getLength() == 0)
      return 0;

    int spacing = getSpacing();
    if(spacing < 0) spacing = 0;
    int gaps = 0;

    if (collapse)
    {
      for (int i=1; i<_children.getLength(); ++i) 
      {
        gaps += collapseMargins(spacing, _children.at(i-1)->getContentBottomMargin(), _children.at(i)->getContentTopMargin());
      }
      gaps += _children.at(0)->getContentLeftMargin() + _children.at(_children.getLength()-1)->getContentBottomMargin();
    }
    else
    {
      for (int i=1; i<_children.getLength(); ++i) 
      {
        LayoutItem* item = _children.at(i);
        gaps += item->getContentTopMargin() + item->getContentBottomMargin();
      }
      gaps += (spacing * (_children.getLength()-1));
    }

    return gaps;
  }



  // ============================================================================
  // [Fog::HBoxLayout]
  // ============================================================================

  void HBoxLayout::calculateLayoutHint(LayoutHint& hint) {
    // Initialize
    int minWidth=0, width=0;
    int minHeight=0, height=0;

    // Iterate over children
    for (sysuint_t i=0; i<getLength(); ++i)
    {
      LayoutItem* child = getAt(i);
      IntSize hint = child->getLayoutSizeHint();
      IntSize min = child->getLayoutMinimumSize();

      // Sum up widths
      width += hint.getWidth();

      // Detect if child is shrinkable or has percent width and update minWidth
      minWidth += child->hasFlex() ? min.getWidth() : hint.getWidth();

      // Build vertical margin sum
      int margin = child->getContentTopMargin() + child->getContentBottomMargin();

      // Find biggest height
      if ((hint.getHeight()+margin) > height) {
        height = hint.getHeight() + margin;
      }

      // Find biggest minHeight
      if ((min.getHeight()+margin) > minHeight) {
        minHeight = min.getHeight() + margin;
      }
    }

    // Respect gaps
    int gaps = (getContentLeftMargin() + calculateHorizontalGaps(true) + getContentRightMargin());

    hint._minimumSize.set(minWidth + gaps,minHeight+getContentTopMargin()+getContentBottomMargin());
    hint._sizeHint.set(width + gaps,height+getContentTopMargin()+getContentBottomMargin());
    hint._maximumSize.set(INT_MAX,INT_MAX);
  }

  int HBoxLayout::doLayout(const IntRect &rect)
  {    
    if(!rect.isValid())
      return 0;
    int availWidth = rect.getWidth();
    int availHeight = rect.getHeight();

    //support for Margin of Layout
    int gaps = getContentLeftMargin() + calculateHorizontalGaps(true) + getContentRightMargin();    
    int allocatedWidth = gaps;

    if(hasFlexItems()) {            
      //Prepare Values!
      LayoutItem* flexibles = 0;
      for (sysuint_t i=0; i<getLength(); ++i)
      {
        LayoutItem* item = getAt(i);
        int hint = item->getLayoutSizeHint().getWidth();
        if(item->hasFlex()) {
          item->_layoutdata->_min = item->getLayoutMinimumSize().getWidth();
          item->_layoutdata->_max = item->getLayoutMaximumSize().getWidth();
          item->_layoutdata->_hint = hint;
          item->_layoutdata->_flex = (float)item->getFlex();
          item->_layoutdata->_offset = 0;

          item->_layoutdata->_next = flexibles;
          flexibles = item;
        }

        allocatedWidth += hint;
      }

      if(allocatedWidth != availWidth && flexibles) {
        calculateFlexOffsets(flexibles, availWidth, allocatedWidth);
      }
    }

    int top, height, marginTop, marginBottom;
    int marginRight = -INT_MAX;    
    int spacing = getSpacing();

    // Render children and separators
    bool forward = isForward();

    register int i=-1;    
    int len = getLength()-1;
    int start = 0;

    if(!forward) {
      i = len+1;
      len = 0;
      start = len;
    }

    int left = rect.x + collapseMargins(getContentLeftMargin(), getAt(start)->getContentLeftMargin());

    while(i != len)  {
      forward ? ++i : --i;
      LayoutItem* child = getAt(i);
      IntSize hint = child->getLayoutSizeHint();

      int width = hint.getWidth();
      width += child->_layoutdata->_offset;

      marginTop = getContentTopMargin() + child->getContentTopMargin();
      marginBottom = getContentBottomMargin() + child->getContentBottomMargin();

      // Find usable height
      height = Math::max<int>(child->getLayoutMinimumSize().getHeight(), Math::min(availHeight-marginTop-marginBottom, child->getLayoutMaximumSize().getHeight()));

      // Respect vertical alignment
      top = rect.y + marginTop;

      // Add collapsed margin
      if (marginRight != -INT_MAX) {
        // Support margin collapsing
        left += collapseMargins(spacing, marginRight, child->getContentLeftMargin());
      }

      // Layout child
      child->setLayoutGeometry(IntRect(left, top, width, height));

      // Add width
      left += width;

      // Remember right margin (for collapsing)
      marginRight = child->getContentRightMargin();
    }

    return 0;
  }



  // ============================================================================
  // [Fog::VBoxLayout]
  // ============================================================================

  void VBoxLayout::calculateLayoutHint(LayoutHint& hint) {
    // Initialize
    int minWidth=0, width=0;
    int minHeight=0, height=0;

    // Iterate over children
    for (sysuint_t i=0; i<getLength(); ++i)
    {
      LayoutItem* child = getAt(i);
      IntSize hint = child->getLayoutSizeHint();
      IntSize min = child->getLayoutMinimumSize();

      // Sum up widths
      height += hint.getHeight();

      // Detect if child is shrinkable or has percent width and update minWidth
      if (child->hasFlex()) {
        minHeight += min.getHeight();
      } else {
        minHeight += hint.getHeight();
      }

      // Build vertical margin sum
      int margin = child->getContentTopMargin() + child->getContentBottomMargin();

      // Find biggest height
      if ((hint.getWidth()+margin) > width) {
        width = hint.getWidth() + width;
      }

      // Find biggest minHeight
      if ((min.getWidth()+margin) > minWidth) {
        minWidth = min.getWidth() + margin;
      }
    }

    // Respect gaps
    int gaps = (getContentLeftMargin() + calculateVerticalGaps(true) + getContentRightMargin());

    hint._minimumSize.set(minWidth+getContentLeftMargin()+getContentRightMargin(),minHeight+gaps);
    hint._sizeHint.set(width+getContentLeftMargin()+getContentRightMargin(),height+gaps);
    hint._maximumSize.set(INT_MAX,INT_MAX);
  }


  int VBoxLayout::doLayout(const IntRect &rect)
  {
    int availWidth = rect.getWidth();
    int availHeight = rect.getHeight();

    //support for Margin of Layout
    int gaps = getContentTopMargin() + calculateVerticalGaps(true) + getContentBottomMargin();
    int allocatedHeight = gaps;

    if(hasFlexItems()) {
      //Prepare Values!
      LayoutItem* flexibles = 0;

      for (sysuint_t i=0; i<getLength(); ++i)
      {
        LayoutItem* item = getAt(i);
        int hint = item->getLayoutSizeHint().getHeight();
        if(item->hasFlex()) {
          item->_layoutdata->_min = item->getLayoutMinimumSize().getHeight();
          item->_layoutdata->_max = item->getLayoutMaximumSize().getHeight();
          item->_layoutdata->_hint = hint;
          item->_layoutdata->_flex = (float)item->getFlex();
          item->_layoutdata->_offset = 0;
          
          item->_layoutdata->_next = flexibles;
          flexibles = item;
        }

        allocatedHeight += hint;
      }

      if(allocatedHeight != availHeight && flexibles) {
        calculateFlexOffsets(flexibles, availHeight, allocatedHeight);
      }
    }

    int left, height, marginLeft, marginRight;
    int marginBottom = -INT_MAX;    
    int spacing = getSpacing();

    // Render children and separators
    bool forward = isForward();

    register int i=-1;    
    int len = getLength()-1;
    int start = 0;

    if(!forward) {
      i = len+1;
      len = 0;
      start = len;
    }

    int top = rect.y + collapseMargins(getContentTopMargin(), getAt(start)->getContentTopMargin());

    while(i != len)  {
      forward ? ++i : --i;
      LayoutItem* child = getAt(i);
      IntSize hint = child->getLayoutSizeHint();

      height = hint.getHeight();
      height += child->_layoutdata->_offset;

      marginLeft = getContentLeftMargin() + child->getContentLeftMargin();
      marginRight = getContentRightMargin() + child->getContentRightMargin();

      // Find usable height
      int width = Math::max<int>(child->getLayoutMinimumSize().getWidth(), Math::min(availWidth-marginLeft-marginRight, child->getLayoutMaximumSize().getWidth()));

      // Respect vertical alignment
      left = rect.x + marginLeft;

      // Add collapsed margin
      if (marginRight != -INT_MAX) {
        // Support margin collapsing
        top += collapseMargins(spacing, marginBottom, child->getContentTopMargin());
      }

      // Layout child
      child->setLayoutGeometry(IntRect(left, top, width, height));

      // Add width
      top += height;

      // Remember right margin (for collapsing)
      marginRight = child->getContentRightMargin();
    }

    return 0;
  }

} // Fog namespaces

