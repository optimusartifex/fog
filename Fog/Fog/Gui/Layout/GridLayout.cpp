// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Layout/GridLayout.h>
#include <Fog/Gui/Layout/LayoutUtils.h>
#include <Fog/Gui/Widget.h>

FOG_IMPLEMENT_OBJECT(Fog::GridLayout)

namespace Fog {
  GridLayout::GridLayout(Widget* parent, int rows, int colums) : Layout(parent), _colwidth(0), _rowheight(0), _vspacing(0), _hspacing(0), _colspan(0), _rowspan(0) {
    if(rows >= 0 && colums >=0) {
      addItem(0,0,0,rows,colums);
    }
  }

  void GridLayout::addItem(LayoutItem* item, int row, int column, int rowSpan, int columnSpan, uint32_t alignment) {
    if(item)
      Layout::addChild(item);

    if(row == -1)
      row = _rows.getLength();

    if(column == -1)
      column = _cols.getLength();

    //check that it's not already there... (needs some improvement!)
    if(item)
    {
      for(uint32_t i=0;i<rowSpan;++i) {
        for(uint32_t j=0;j<columnSpan;++j) {
          if(getCellItem(row+i,column+j))
          {
            //Warning already exists!
            remove(item);
            return;
          }
        }
      }
    }

    //Add Columns and Rows needed for insertion
    //TODO: use reserve to prevent unneeded memory allocations!
    int ccolumn = column+columnSpan;
    int crow = row+rowSpan;

    Row* itemrow = 0;
    Column* itemcolumn = 0;

    if(_cols.getCapacity() < ccolumn) {
      _cols.reserve(ccolumn);
    }

    if(_rows.getCapacity() < crow) {
      _cols.reserve(crow);
    }

    //check if enough columns are there
    if(ccolumn > _cols.getLength()) {
      while(ccolumn > _cols.getLength()) {
        Column* c = new(std::nothrow) Column(_cols.getLength(), this);
        _cols.append(c);
      }
    }

    //create enough rows
    if(crow > _rows.getLength()) {
      while(crow > _rows.getLength()) {
        Row* r = new(std::nothrow) Row(_rows.getLength());
        _rows.append(r);
      }
    }

    //now we need to create enough columns per row!
    for(uint32_t i=0;i<rowSpan;++i) {
      Row* r = _rows.at(row+i);

      //append enough columns into row
      while(ccolumn > r->_cols.getLength()) {
        r->_cols.append(0);
      }

      //set item into column within row
      for(uint32_t j=0;j<columnSpan;++j) {
        r->_cols.set(column+j, item);
      }
    }

    if(item) {
      LayoutProperties* prop = static_cast<LayoutProperties*>(item->_layoutdata);
      prop->_row = _rows.at(row);
      prop->_rowspan = rowSpan;
      prop->_column = _cols.at(column);
      prop->_colspan = columnSpan;

      item->setLayoutAlignment(alignment);

      //create simple single linked lists for fast span iterations
      if(columnSpan > 1) {
        prop->_colspannext = _colspan;
        _colspan = item;
      }

      if(rowSpan > 1) {
        prop->_rowspannext = _rowspan;
        _rowspan = item;
      }
    }

    invalidateLayout();
  }

  LayoutItem* GridLayout::getCellItem(int row, int column) const {
    FOG_ASSERT(row >= 0 && column >=0);
    if(column >= _cols.getLength() || row >= _rows.getLength())
      return 0;

    Row* r = _rows.at(row);

    if(column >= r->_cols.getLength()) 
      return 0;

    LayoutItem* col = r->_cols.at(column);
    return col;
  }

  void GridLayout::Column::calculateWidth() {
    //TODO: Column Cache handling!
    int width = 0, minWidth = 0;

    for (int row=0; row<_layout->_rows.getLength(); ++row) {
      LayoutItem* item = getItem(row);
      if (!item) {
        continue;
      }

      LayoutProperties* prop = static_cast<LayoutProperties*>(item->_layoutdata);
      if (prop->_colspan > 1)
        continue;

      LayoutHint hint = item->getLayoutHint();

      int margins = item->getContentLeftMargin() + item->getContentRightMargin();
      int zminWidth =  hint._minimumSize.getWidth() + margins;
      int zhintWidth =  hint._sizeHint.getWidth() + margins;

      minWidth = _flex > 0 ? Math::max(minWidth, zminWidth) : Math::max(minWidth, zhintWidth);
      width = Math::max(width, zhintWidth);
    }

    minWidth = Math::max(minWidth, _minWidth);

    width = _hintWidth == -1 ? Math::max(minWidth, Math::min(width, _maxWidth)) : _hintWidth;

    _hintWidth = width;
    _minWidth = minWidth;
  }

  void GridLayout::Row::calculateHeight() {
    int minHeight = 0, height = 0;

    for (int col=0; col<_cols.getLength(); ++col) {
      LayoutItem* item = getColumn(col);
      if (!item)
        continue;

      LayoutProperties* prop = static_cast<LayoutProperties*>(item->_layoutdata);

      if (prop->_rowspan > 1)
        continue;

      LayoutHint hint = item->getLayoutHint();
      int margins = item->getContentTopMargin() + item->getContentBottomMargin();
      int zhintHeight =  hint._sizeHint.getHeight() + margins;

      minHeight = _flex > 0 ? Math::max(minHeight, hint._minimumSize.getHeight() + margins) : Math::max(minHeight, zhintHeight);
      height = Math::max(height, zhintHeight);
    }


    minHeight = Math::max(minHeight, _minHeight);
    _hintHeight = _hintHeight == -1 ? Math::max(minHeight, Math::min(height, _maxHeight)) : _hintHeight;
    _minHeight = minHeight;
  }


  void GridLayout::calculateColumnWidths(int& minWidth, int& hintWidth) {
    if(_colwidth == 1) {
      minWidth = _cacheMinWidth;
      hintWidth = _cacheHintWidth;
      return;
    }

    _cacheMinWidth = 0;
    _cacheHintWidth = 0;

    for (int col=0; col<_cols.getLength(); col++)
    {
      Column* column = _cols.at(col);
      column->calculateWidth();

      _cacheMinWidth += column->_flex > 0 ? column->_minWidth : column->_hintWidth;
      _cacheHintWidth += column->_hintWidth;
    }

    minWidth = _cacheMinWidth;
    hintWidth = _cacheHintWidth;
    _colwidth = 1;
  }

  void GridLayout::calculateRowHeights(int& minHeight, int& hintHeight) {
    if(_rowheight == 1) {
      minHeight = _cacheMinHeight;
      hintHeight = _cacheHintHeight;
      return;
    }

    _cacheMinHeight = 0;
    _cacheHintHeight = 0;
    
    for (int crow=0; crow<_rows.getLength(); ++crow) {
       Row* row = _rows.at(crow);
       row->calculateHeight();

       _cacheMinHeight += row->_flex > 0 ? row->_minHeight : row->_hintHeight;
       _cacheHintHeight += row->_hintHeight;
    }

    minHeight = _cacheMinHeight;
    hintHeight = _cacheHintHeight;
    _rowheight = 1;
  }

  void GridLayout::calculateColumnFlexOffsets(int availWidth) {    
    int hintwidth = getLayoutSizeHint().getWidth();
    int diff = availWidth - hintwidth;

    if (diff == 0)
      return;

    // collect all flexible children
    Column* flexibles = 0;
    for (int i=0; i<_cols.getLength(); ++i)
    {
      Column* col = _cols.at(i);      

      //optimize usage of flex
      if ((col->_flex <= 0) || (col->_hintWidth == col->_maxWidth && diff > 0) || (col->_hintWidth == col->_minWidth && diff < 0)) {
        continue;
      }

      col->initFlex(flexibles);
      flexibles = col;
    }
    
    if(flexibles)
      calculateFlexOffsets(flexibles, availWidth, hintwidth);
  }

  void GridLayout::calculateRowFlexOffsets(int availHeight) {
    int hintHeight = getLayoutSizeHint().getHeight();
    int diff = availHeight - hintHeight;

    if (diff == 0)
      return;

    // collect all flexible children
    Row* flexibles = 0;
    for (int i=0; i<_rows.getLength(); ++i)
    {
      Row* row = _rows.at(i);    

      //optimize usage of flex
      if ((row->_flex <= 0) || (row->_hintHeight == row->_maxHeight && diff > 0) || (row->_hintHeight == row->_minHeight && diff < 0))
        continue;

      row->initFlex(flexibles);      
      flexibles = row;
    }

    if(flexibles)
      calculateFlexOffsets(flexibles, availHeight, hintHeight);
  }


  void GridLayout::calculateLayoutHint(LayoutHint& hint) {
    //calculate column widths
    int minWidth, width;
    calculateColumnWidths(minWidth, width);

    //calculate row heights
    int minHeight, height;
    calculateRowHeights(minHeight, height);

    //calculate spacing
    int spacingX = _hspacing * (_cols.getLength() - 1);
    int spacingY = _vspacing * (_rows.getLength() - 1);

    //calculate margin
    spacingX += getContentLeftMargin() + getContentRightMargin();
    spacingY += getContentTopMargin() + getContentBottomMargin();

    hint._minimumSize.set(minWidth + spacingX,minHeight + spacingY);
    hint._sizeHint.set(width + spacingX,height + spacingY);
  }

  int GridLayout::calculateSpanWidth(int col, LayoutItem* item) const {
    LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);

    // compute sizes width including cell spanning
    int width = _hspacing * (widgetProps->_colspan - 1);
    for (int i=0; i<widgetProps->_colspan; ++i) {
      Column* column = _cols.at(col+i);
      //update if flex was calculated for this column
      updateColumnFlexWidth(column);
      width += column->_hintWidth;
    }

    return width;
  }

  int GridLayout::calculateSpanHeight(int crow, LayoutItem* item) const {
    LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);

    int height = _vspacing * (widgetProps->_rowspan - 1);
    for (int i=0; i<widgetProps->_rowspan; i++) {
      Row* row = _rows.at(crow+i);
      //update if flex was calculated for this row
      updateRowFlexHeight(row);
      height += row->_hintHeight;
    }

    return height;
  }

  void GridLayout::setLayoutGeometry(const IntRect& rect) {
    int maxRowIndex = _rows.getLength();
    int maxColIndex = _cols.getLength();
    if(maxRowIndex == 0 || maxColIndex == 0)
      return;

    int availWidth = rect.getWidth();
    int availHeight = rect.getHeight();

    calculateColumnFlexOffsets(availWidth);
    calculateRowFlexOffsets(availHeight);

    // do the layout
    int curleft = rect.x + getContentLeftMargin();
    int top = rect.y + getContentTopMargin();

    for (int col=0; col<maxColIndex; ++col) {
      Column* column = _cols.at(col);
      int curtop = top;

      for (int crow=0; crow<maxRowIndex; ++crow) {
        Row* row = _rows.at(crow);
        LayoutItem* item = getCellItem(crow,col);        
        
        if (!item)
        {
          // ignore empty cells
          updateColumnFlexWidth(column);
          updateRowFlexHeight(row);
          curtop += row->_hintHeight + _vspacing;
          continue;
        }

        LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);
        if(!isItemOrigin(item, row, column))
        {
          // ignore cells, which have cell spanning but are not the origin of the item
          curtop += row->_hintHeight + _vspacing;
          continue;
        }

        int spanWidth = calculateSpanWidth(col, item);
        int spanHeight = calculateSpanHeight(crow, item);

        LayoutHint cellHint = item->getLayoutHint();

        int hmargins = item->getContentTopMargin() + item->getContentBottomMargin();
        int vmargins = item->getContentLeftMargin() + item->getContentRightMargin();

        int cellWidth = Math::max(cellHint.getMinimumSize().getWidth(), Math::min(spanWidth-vmargins, cellHint.getMaximumSize().getWidth()));
        int cellHeight = Math::max(cellHint.getMinimumSize().getHeight(), Math::min(spanHeight-hmargins, cellHint.getMaximumSize().getHeight()));

        //TODO: Use CellAlignment if item has smaller size than size of cell!
        int cellLeft = curleft + item->getContentLeftMargin();
        int cellTop = curtop + item->getContentTopMargin();

        item->setLayoutGeometry(IntRect(cellLeft,cellTop,cellWidth,cellHeight));

        curtop += row->_hintHeight + _vspacing;
      }

      curleft += column->_hintWidth + _hspacing;
    }
  }

}