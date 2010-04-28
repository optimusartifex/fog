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
    : Layout(parent), _direction(LEFTTORIGHT), _flexibles(0), _allocated(0)
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

    _allocated = 0;
    _flexibles = 0;

    // Iterate over children
    for (sysuint_t i=0; i<getLength(); ++i)
    {
      LayoutItem* item = getAt(i);
      IntSize hint = item->getLayoutSizeHint();
      IntSize min = item->getLayoutMinimumSize();

      // Sum up widths
      width += hint.getWidth();

      // Detect if child is shrinkable or has percent width and update minWidth
      minWidth += item->hasFlex() ? min.getWidth() : hint.getWidth();

      // Build vertical margin sum
      int margin = item->getContentTopMargin() + item->getContentBottomMargin();

      // Find biggest height
      if ((hint.getHeight()+margin) > height) {
        height = hint.getHeight() + margin;
      }

      // Find biggest minHeight
      if ((min.getHeight()+margin) > minHeight) {
        minHeight = min.getHeight() + margin;
      }

      int whint = hint.getWidth();
      if(item->hasFlex()) {
        LayoutItem::FlexLayoutProperties * prop = static_cast<LayoutItem::FlexLayoutProperties*> (item->_layoutdata);
        prop->_min = item->getLayoutMinimumSize().getWidth();
        prop->_max = item->getLayoutMaximumSize().getWidth();
        prop->_hint = whint;
        prop->_flex = (float)item->getFlex();
        prop->_offset = 0;

        prop->_next = _flexibles;
        _flexibles = item;
      }

      _allocated += whint;
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
    int allocatedWidth = gaps + _allocated;

    if(_flexibles && allocatedWidth != availWidth) {
      calculateFlexOffsets(_flexibles, availWidth, allocatedWidth);
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
      width += ((LayoutItem::FlexLayoutProperties*)child->_layoutdata)->_offset;

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

    _allocated = 0;
    _flexibles = 0;

    // Iterate over children
    for (sysuint_t i=0; i<getLength(); ++i)
    {
      LayoutItem* item = getAt(i);
      IntSize hint = item->getLayoutSizeHint();
      IntSize min = item->getLayoutMinimumSize();

      // Sum up widths
      height += hint.getHeight();

      // Detect if child is shrinkable or has percent width and update minWidth
      if (item->hasFlex()) {
        minHeight += min.getHeight();
      } else {
        minHeight += hint.getHeight();
      }

      // Build vertical margin sum
      int margin = item->getContentTopMargin() + item->getContentBottomMargin();

      // Find biggest height
      if ((hint.getWidth()+margin) > width) {
        width = hint.getWidth() + width;
      }

      // Find biggest minHeight
      if ((min.getWidth()+margin) > minWidth) {
        minWidth = min.getWidth() + margin;
      }
      
      int hinth = hint.getHeight();
      if(item->hasFlex()) {
        LayoutItem::FlexLayoutProperties * prop = static_cast<LayoutItem::FlexLayoutProperties*> (item->_layoutdata);
        prop->_min = item->getLayoutMinimumSize().getHeight();
        prop->_max = item->getLayoutMaximumSize().getHeight();
        prop->_hint = hinth;
        prop->_flex = (float)item->getFlex();
        prop->_offset = 0;

        prop->_next = _flexibles;
        _flexibles = item;
      }

      _allocated += hinth;
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
    int allocatedHeight = gaps + _allocated;

    if(_flexibles && allocatedHeight != availHeight) {
      calculateFlexOffsets(_flexibles, availHeight, allocatedHeight);
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
      height += ((LayoutItem::FlexLayoutProperties*)child->_layoutdata)->_offset;

      marginLeft = getContentLeftMargin() + child->getContentLeftMargin();
      marginRight = getContentRightMargin() + child->getContentRightMargin();

      // Find usable height
      int width = Math::max<int>(child->getLayoutMinimumSize().getWidth(), Math::min(availWidth-marginLeft-marginRight, child->getLayoutMaximumSize().getWidth()));

      // Respect vertical alignment
      left = rect.x + marginLeft;

      // Add collapsed margin
      if (marginBottom != -INT_MAX) {
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

