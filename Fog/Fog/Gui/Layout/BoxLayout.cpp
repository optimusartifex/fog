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
    : Layout(parent)
  {
    setSpacing(spacing);
    setContentMargins(margin, margin, margin, margin);
  }

  BoxLayout::BoxLayout(int margin, int spacing)
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

  void BoxLayout::add(LayoutItem *item)
  {
    Layout::add(item);
    item->_flexibles = new LayoutItem::Flexibles();    
    _itemList.append(item);
  }

  int BoxLayout::getLength() const
  {
    return _itemList.getLength();
  }

  LayoutItem *BoxLayout::getAt(int index) const
  {
    if(index < (int)_itemList.getLength())
      return _itemList.at(index);

    return 0;
  }

  LayoutItem *BoxLayout::takeAt(int index)
  {
    if (index >= 0 && index < (int)_itemList.getLength()) {
      LayoutItem * item  = _itemList.take(index);
      if(item) item->removeFlexibles();
      return item;
    }
    
    return 0;
  }

  uint32_t BoxLayout::getLayoutExpandingDirections() const
  {
    return ORIENTATION_HORIZONTAL;
  }

  void BoxLayout::setLayoutGeometry(const IntRect &rect)
  {
    Layout::setLayoutGeometry(rect);
    doLayout(rect);
  }

  void BoxLayout::buildCache() {
    // Initialize
    int minWidth=0, width=0;
    int minHeight=0, height=0;

    // Iterate over children
    for (sysuint_t i=0; i<_itemList.getLength(); ++i)
    {
      LayoutItem* child = _itemList.at(i);
      IntSize hint = child->getLayoutSizeHint();
      IntSize min = child->getLayoutMinimumSize();

      // Sum up widths
      width += hint.getWidth();

      // Detect if child is shrinkable or has percent width and update minWidth
      if (child->hasFlex()) {
        minWidth += min.getWidth();
      } else {
        minWidth += hint.getWidth();
      }

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
    int gaps = (getContentLeftMargin() + calculateHorizontalGaps(_itemList, getSpacing(), true) + getContentRightMargin());

    _min.set(minWidth + gaps,minHeight+getContentTopMargin()+getContentBottomMargin());
    _hint.set(width + gaps,height+getContentTopMargin()+getContentBottomMargin());
  }

  int BoxLayout::doLayout(const IntRect &rect)
  {
    buildCache();

    int availWidth = rect.getWidth();
    int availHeight = rect.getHeight();

    int gaps = getContentLeftMargin() + calculateHorizontalGaps(_itemList, getSpacing(), true) + getContentRightMargin();    
    int allocatedWidth = gaps;

    if(hasFlexItems()) {            
      //Prepare Values!
      for (sysuint_t i=0; i<_itemList.getLength(); ++i)
      {
        LayoutItem* item = _itemList.at(i);
        int hint = item->getLayoutSizeHint().getWidth();
        if(item->hasFlex()) {
          item->_flexibles->_min = item->getLayoutMinimumSize().getWidth();
          item->_flexibles->_max = item->getLayoutMaximumSize().getWidth();
          item->_flexibles->_hint = hint;
          item->_flexibles->_flex = (float)item->getFlex();
          item->_flexibles->_offset = 0;
        }

        allocatedWidth += hint;
      }

      if(allocatedWidth != availWidth) {
        calculateFlexOffsets(_itemList, availWidth, allocatedWidth);
      }
    }

    int top, height, marginRight, marginTop, marginBottom;
    int left = collapseMargins(getContentLeftMargin(), _itemList.at(0)->getContentLeftMargin());
    int spacing = getSpacing();

    // Render children and separators
    for (sysuint_t i=0; i<_itemList.getLength(); ++i)
    {
      LayoutItem* child = _itemList.at(i);
      IntSize hint = child->getLayoutSizeHint();

      int width = hint.getWidth();
      width += child->_flexibles->_offset;

      if(child->_flexibles->_offset < 0) {
        width = width;
      }

      marginTop = getContentTopMargin() + child->getContentTopMargin();
      marginBottom = getContentBottomMargin() + child->getContentBottomMargin();

      // Find usable height
      height = Math::max<int>(child->getLayoutMinimumSize().getHeight(), Math::min(availHeight-marginTop-marginBottom, child->getLayoutMaximumSize().getHeight()));

      // Respect vertical alignment
      top = marginTop;

      // Add collapsed margin
      if (i > 0) {
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
} // Fog namespace

