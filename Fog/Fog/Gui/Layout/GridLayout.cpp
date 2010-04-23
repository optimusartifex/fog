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

    if(_cols.getCapacity() < ccolumn) {
      _cols.reserve(ccolumn);
    }

    if(_rows.getCapacity() < crow) {
      _cols.reserve(crow);
    }


    //check if enough columns are there
    if(ccolumn > _cols.getLength()) {
      while(ccolumn > _cols.getLength()) {
        Column* c = new(std::nothrow) Column();
        _cols.append(c);
      }
    }

    //create enough rows
    if(crow > _rows.getLength()) {
      while(crow > _rows.getLength()) {
        Row* r = new(std::nothrow) Row();
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
      prop->_row = row;
      prop->_rowspan = rowSpan;
      prop->_column = column;
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

  void GridLayout::fixHeightRowSpan() {
    int spacing = _vspacing;

    LayoutItem* item = _rowspan;

    while(item)
    {
      LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);
      int vMargins = item->getContentTopMargin() + item->getContentBottomMargin();
      int zminHeight =  item->getLayoutHint()._minimumSize.getHeight() + vMargins;
      int zhintHeight =  item->getLayoutHint()._sizeHint.getHeight() + vMargins;

      int prefSpanHeight = spacing * (widgetProps->_rowspan - 1);
      int minSpanHeight = prefSpanHeight;

      LayoutItem * flexibles = 0;
      LayoutItem * flexiblesAll = 0;

      for (int j=0; j<widgetProps->_rowspan; ++j)
      {
        int rowpos = widgetProps->_row+j;
        FOG_ASSERT(rowpos < _rows.getLength());
        Row* row = _rows.at(rowpos);
        float rowFlex = getRowFlex(rowpos);

        // compute flex array for the preferred width
        if (rowFlex > 0)
        {
          widgetProps->_min = row->_minHeight;
          widgetProps->_max = row->_maxHeight;
          widgetProps->_hint = row->_hintHeight;
          widgetProps->_flex = rowFlex;
          widgetProps->_offset = 0;

          widgetProps->_next = flexibles;
          flexibles = item;
        }

        prefSpanHeight += row->_hintHeight;
        minSpanHeight += row->_minHeight;
      }

      // If there is not enough space for the preferred size
      // increment the preferred column sizes.
      if (prefSpanHeight < zhintHeight) {
        calculateFlexOffsets(flexibles, zhintHeight, prefSpanHeight);
        LayoutItem* item = flexibles;
        while(item) {
          LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);
          Row* row = _rows.at(widgetProps->_row);
          row->_hintHeight += widgetProps->_offset;
          item = (LayoutItem*)item->_layoutdata->_next;
        }
      }

      // If there is not enought space for the min size
      // increment the min column sizes.
      if (minSpanHeight < zminHeight) {
        calculateFlexOffsets(flexibles, zhintHeight, minSpanHeight);

        LayoutItem* item = flexibles;
        while(item) {
          LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);
          Row* row = _rows.at(widgetProps->_row);
          row->_minHeight += widgetProps->_offset;
          item = (LayoutItem*)item->_layoutdata->_next;
        }
      }

      item = widgetProps->_rowspannext;
    }
  }

  void GridLayout::fixWidthColumnSpan() {
    int spacing = _hspacing;

    LayoutItem* item = _colspan;

    while(item)
    {
      LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);
      int hMargins = item->getContentLeftMargin() + item->getContentRightMargin();        
      int zminWidth =  item->getLayoutHint()._minimumSize.getWidth() + hMargins;
      int zhintWidth =  item->getLayoutHint()._sizeHint.getWidth() + hMargins;

      int prefSpanWidth = spacing * (widgetProps->_colspan - 1);
      int minSpanWidth = prefSpanWidth;

      LayoutItem * flexibles = 0;

      for (int j=0; j<widgetProps->_colspan; ++j)
      {
        int col = widgetProps->_column+j;
        FOG_ASSERT(col < _cols.getLength());
        Column* column = _cols.at(col);
        float colFlex = getColumnFlex(col);

        // compute flex array for the preferred width
        if (colFlex > 0)
        {
          widgetProps->_min = column->_minWidth;
          widgetProps->_max = column->_maxWidth;
          widgetProps->_hint = column->_hintWidth;
          widgetProps->_flex = colFlex;
          widgetProps->_offset = 0;

          widgetProps->_next = flexibles;
          flexibles = item;
        }

        prefSpanWidth += column->_hintWidth;
        minSpanWidth += column->_minWidth;
      }

      // If there is not enough space for the preferred size
      // increment the preferred column sizes.
      if (prefSpanWidth < zhintWidth) {
        calculateFlexOffsets(flexibles, zhintWidth, prefSpanWidth);
        LayoutItem* item = flexibles;
        while(item) {
          LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);
          Column* col = _cols.at(widgetProps->_column);
          col->_hintWidth += widgetProps->_offset;
          item = (LayoutItem*)item->_layoutdata->_next;
        }
      }

      // If there is not enough space for the min size
      // increment the min column sizes.
      if (minSpanWidth < zminWidth) {
        calculateFlexOffsets(flexibles, zhintWidth, minSpanWidth);

        LayoutItem* item = flexibles;
        while(item) {
          LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);
          Column* col = _cols.at(widgetProps->_column);
          col->_minWidth += widgetProps->_offset;
          item = (LayoutItem*)item->_layoutdata->_next;
        }
      }


      item = widgetProps->_colspannext;
    }
  }

  void GridLayout::calculateColumnWidths() {
    if(_colwidth == 1) {
      return;
    }

    for (int col=0; col<_cols.getLength(); col++)
    {
      int width = 0, minWidth = 0, maxWidth = INT_MAX;

      Column* column = _cols.at(col);

      for (int row=0; row<_rows.getLength(); ++row) {
        LayoutItem* item = getCellItem(row,col);
        if (!item) {
          continue;
        }

        LayoutProperties* prop = static_cast<LayoutProperties*>(item->_layoutdata);

        if (prop->_colspan > 1) {
          // ignore columns with col spans at this place
          // these columns will be taken into account later
          continue;
       }

        int hMargins = item->getContentLeftMargin() + item->getContentRightMargin();        
        int zminWidth =  item->getLayoutHint()._minimumSize.getWidth() + hMargins;
        int zhintWidth =  item->getLayoutHint()._sizeHint.getWidth() + hMargins;

        if (getColumnFlex(col) > 0) {
          minWidth = Math::max(minWidth, zminWidth);
        } else {
          minWidth = Math::max(minWidth, zhintWidth);
        }

        width = Math::max(width, zhintWidth);
      }


      minWidth = Math::max(minWidth, getColumnMinimumWidth(col));
      maxWidth = getColumnMaximumWidth(col);

      if (getColumnHintWidth(col) != -1) {
        width = getColumnHintWidth(col);
      } else {
        width = Math::max(minWidth, Math::min(width, maxWidth));
      }

      column->_hintWidth = width;
      column->_maxWidth = maxWidth;
      column->_minWidth = minWidth;
    }

    if (_colspan) {
      fixWidthColumnSpan();
    }

    _colwidth = 1;
  }


  void GridLayout::calculateRowHeights() {
    if(_rowheight == 1) {
      return;
    }
    
    for (int row=0; row<_rows.getLength(); ++row) {
      //int width = 0, minWidth = 0, maxWidth = INT_MAX;
      int minHeight = 0, height = 0, maxHeight = INT_MAX;

      Row* crow = _rows.at(row);

      for (int col=0; col<_cols.getLength(); ++col) {
        LayoutItem* item = getCellItem(row,col);
        if (!item) {
          continue;
        }

        LayoutProperties* prop = static_cast<LayoutProperties*>(item->_layoutdata);

        if (prop->_rowspan > 1) {
          // ignore columns with col spans at this place
          // these columns will be taken into account later
          continue;
        }

        int vMargins = item->getContentTopMargin() + item->getContentBottomMargin();
        int zminHeight =  item->getLayoutHint()._minimumSize.getHeight() + vMargins;
        int zhintHeight =  item->getLayoutHint()._sizeHint.getHeight() + vMargins;

        if (getRowFlex(row) > 0) {
          minHeight = Math::max(minHeight, zminHeight);
        } else {
          minHeight = Math::max(minHeight, zhintHeight);
        }

        height = Math::max(height, zhintHeight);
      }


      minHeight = Math::max(minHeight, getRowMinimumHeight(row));
      maxHeight = getRowMaximumHeight(row);

      if (getRowHintHeight(row) != -1) {
        height = getRowHintHeight(row);
      } else {
        height = Math::max(minHeight, Math::min(height, maxHeight));
      }

      crow->_hintHeight = height;
      crow->_maxHeight = maxHeight;
      crow->_minHeight = minHeight;
    }

    if (_rowspan) {
      fixHeightRowSpan();
    }

    _rowheight = 1;
  }

  void GridLayout::calculateColumnFlexOffsets(int width) {
    IntSize hint = getLayoutSizeHint();
    int diff = width - hint.getWidth();

    if (diff == 0) {
      return;
    }

    // collect all flexible children
    Column* flexibles = 0;

    for (int i=0; i<_cols.getLength(); ++i)
    {
      Column* col = _cols.at(i);;
      float colFlex = getColumnFlex(i);

      LayoutProperties* widgetProps = static_cast<LayoutProperties*>(col->_layoutdata);

      if ((colFlex <= 0) || (col->_hintWidth == col->_maxWidth && diff > 0) || (col->_hintWidth == col->_minWidth && diff < 0)) {
        //widgetProps->_offset = 0;
        continue;
      }      

      widgetProps->_min = col->_minWidth;
      widgetProps->_max = col->_maxWidth;
      widgetProps->_hint = col->_hintWidth;
      widgetProps->_flex = colFlex;
      widgetProps->_offset = 0;

      widgetProps->_next = flexibles;
      flexibles = col;
    }

    //return qx.ui.layout.Util.computeFlexOffsets(flexibles, width, hint.width);
    int hintwidth = hint.getWidth();
    calculateFlexOffsets(flexibles, width, hintwidth);
    hintwidth = hintwidth;
  }

  void GridLayout::calculateRowFlexOffsets(int height) {
    IntSize hint = getLayoutSizeHint();
    int diff = height - hint.getHeight();

    if (diff == 0) {
      return;
    }

    // collect all flexible children
    Row* flexibles = 0;

    for (int i=0; i<_rows.getLength(); ++i)
    {
      Row* row = _rows.at(i);;
      int rowFlex = getRowFlex(i);
      LayoutProperties* widgetProps = static_cast<LayoutProperties*>(row->_layoutdata);

      if ((rowFlex <= 0) || (row->_hintHeight == row->_maxHeight && diff > 0) || (row->_hintHeight == row->_minHeight && diff < 0)) {
        //widgetProps->_offset = 0;
        continue;
      }

      widgetProps->_min = row->_minHeight;
      widgetProps->_max = row->_maxHeight;
      widgetProps->_hint = row->_hintHeight;
      widgetProps->_flex = rowFlex;
      widgetProps->_offset = 0;

      widgetProps->_next = flexibles;
      flexibles = row;
    }

    //return qx.ui.layout.Util.computeFlexOffsets(flexibles, width, hint.width);
    int hintheight = hint.getHeight();
    calculateFlexOffsets(flexibles, height, hintheight);
  }


  void GridLayout::calculateLayoutHint(LayoutHint& hint) {
    // calculate col widths
    calculateColumnWidths();

    int minWidth=0, width=0;

    for (int i=0; i<_cols.getLength(); ++i)
    {
      Column* col = _cols.at(i);
      if (getColumnFlex(i) > 0) {
        minWidth += col->_minWidth;
      } else {
        minWidth += col->_hintWidth;
      }

      width += col->_hintWidth;
    }

    // calculate row heights
    calculateRowHeights();

    int minHeight=0, height=0;
    for (int i=0; i<_rows.getLength(); ++i)
    {
      Row* row = _rows[i];

      if (getRowFlex(i) > 0) {
        minHeight += row->_minHeight;
      } else {
        minHeight += row->_hintHeight;
      }

      height += row->_hintHeight;
    }

    int spacingX = _hspacing * (_cols.getLength() - 1);
    int spacingY = _vspacing * (_rows.getLength() - 1);

    hint._minimumSize.set(minWidth + spacingX,minHeight + spacingY);
    hint._sizeHint.set(width + spacingX,height + spacingY);
  }

  void GridLayout::setLayoutGeometry(const IntRect& rect) {
    int availWidth = rect.getWidth();
    int availHeight = rect.getHeight();

    int maxColIndex = _cols.getLength();
    int maxRowIndex = _rows.getLength();

    if(maxRowIndex == 0)
      return;

    calculateColumnFlexOffsets(availWidth);
    calculateRowFlexOffsets(availHeight);

    // do the layout
    int left = 0;
    Column* lastcolumn = 0;
    Row* lastrow = 0;

    //TODO: optimize col/row-span iterations!
    for (int col=0; col<maxColIndex; ++col) {
      Column* column = _cols.at(col);     
      int top = 0;
      //offset is added later to also support colspan
      if(column->_static._offset != 0) {
        column->_hintWidth += column->_static._offset;
        column->_static._offset = 0;
      }

      for (int crow=0; crow<maxRowIndex; crow++) {
        Row* row = _rows.at(crow);
        LayoutItem* widget = getCellItem(crow,col);

        if(row->_static._offset != 0) {
          row->_hintHeight += row->_static._offset;
          row->_static._offset = 0;
        }

        // ignore empty cells
        if (!widget)
        {
          top += row->_hintHeight + _vspacing;    //_cacheHeight??
          continue;
        }

        LayoutProperties* widgetProps = static_cast<LayoutProperties*>(widget->_layoutdata);
        // ignore cells, which have cell spanning but are not the origin
        // of the widget
        if(widgetProps->_row != crow || widgetProps->_column != col)
        {
          top += row->_hintHeight + _vspacing;
          continue;
        }

        // compute sizes width including cell spanning
        int spanWidth = _hspacing * (widgetProps->_colspan - 1);
        for (int i=0; i<widgetProps->_colspan; i++) {
          Column* ccolumn = _cols.at(col+i);
          //because columns can be spanned
          if(ccolumn->_static._offset != 0) {
            ccolumn->_hintWidth += ccolumn->_static._offset;
            ccolumn->_static._offset = 0;      
          }
          spanWidth += ccolumn->_hintWidth;
        }

        int spanHeight = _vspacing * (widgetProps->_rowspan - 1);
        for (int i=0; i<widgetProps->_rowspan; i++) {
          Row* rrow = _rows.at(crow+i);
          //for correct top-variable calculation we need to add to _hintHeight here!
          if(rrow->_static._offset != 0) {
            rrow->_hintHeight += rrow->_static._offset;
            rrow->_static._offset = 0;
          }
          spanHeight += rrow->_hintHeight;
        }


        LayoutHint cellHint = widget->getLayoutHint();
        int marginTop = widget->getContentTopMargin();
        int marginLeft = widget->getContentLeftMargin();
        int marginBottom = widget->getContentBottomMargin();
        int marginRight = widget->getContentRightMargin();

        int cellWidth = Math::max(cellHint.getMinimumSize().getWidth(), Math::min(spanWidth-marginLeft-marginRight, cellHint.getMaximumSize().getWidth()));
        int cellHeight = Math::max(cellHint.getMinimumSize().getHeight(), Math::min(spanHeight-marginTop-marginBottom, cellHint.getMaximumSize().getHeight()));

        //var cellAlign = this.getCellAlign(row, col);
        int cellLeft = left + marginLeft; //Util.computeHorizontalAlignOffset(cellAlign.hAlign, cellWidth, spanWidth, marginLeft, marginRight);
        int cellTop = top + marginTop; //Util.computeVerticalAlignOffset(cellAlign.vAlign, cellHeight, spanHeight, marginTop, marginBottom);

        widget->setLayoutGeometry(IntRect(cellLeft,cellTop,cellWidth,cellHeight));

        top += row->_hintHeight + _vspacing;
      }

      left += column->_hintWidth + _hspacing;
    }

  }

}