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
#include <Fog/Gui/Widget.h>

FOG_IMPLEMENT_OBJECT(Fog::GridLayout)

namespace Fog {
  GridLayout::GridLayout(Widget* parent) : Layout(parent), _colwidth(0), _rowheight(0), _vspacing(0), _hspacing(0) {
    
  }

  void GridLayout::addItem(LayoutItem* item, int row, int column, int rowSpan, int columnSpan, uint32_t alignment) {
    Layout::addChild(item);

    if(row == -1)
      row = _rows.getLength();

    if(column == -1)
      column = _cols.getLength();

    //check that it's not already there... (needs some improvement!)
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

    //Add Columns and Rows needed for insertion
    //TODO: use reserve to prevent unneeded memory allocations!
    while(column+columnSpan > _cols.getLength()) {
      Column* c = new(std::nothrow) Column();
      _cols.append(c);
    }

    while(row+rowSpan > _rows.getLength()) {
      if(_rows.getLength()>=row) {
        Row* r = new(std::nothrow) Row(item);
        _rows.append(r);
      } else {
        Row* r = new(std::nothrow) Row(0);
        _rows.append(r);
      }
    }

    //check if that it's not already there... (needs some improvement!)
    for(uint32_t i=0;i<rowSpan;++i) {
      Row* r = _rows.at(row+i);
      while(r->_cols.getLength() < column) {
        r->_cols.append(0);
      }

      for(uint32_t j=0;j<columnSpan;++j) {        
        r->_cols.append(item);
      }
    }

    LayoutProperties* prop = static_cast<LayoutProperties*>(item->_layoutdata);
    prop->_row = row;
    prop->_rowspan = rowSpan;
    prop->_column = column;
    prop->_colspan = columnSpan;
  }

  LayoutItem* GridLayout::getCellItem(int row, int column) const {
    FOG_ASSERT(row >= 0 && column >=0);
    if(column >= _cols.getLength() || row >= _rows.getLength())
      return 0;

    Row* r = _rows.at(row);
    FOG_ASSERT(r);

    if(column >= r->_cols.getLength()) 
      return 0;

    LayoutItem* col = r->_cols.at(column);
    FOG_ASSERT(col);
    return col;
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
        int width = getColumnHintWidth(col);
      } else {
        int width = Math::max(minWidth, Math::min(width, maxWidth));
      }

      column->_hintWidth = width;
      column->_maxWidth = maxWidth;
      column->_minWidth = minWidth;
    }
// 
//     if (this.__colSpans.length > 0) {
//       this._fixWidthsColSpan(colWidths);
//     }

    _colwidth = 1;
  }


  void GridLayout::calculateRowHeights() {
    if(_rowheight == 1) {
      return;
    }

    
    for (int row=0; row<_rows.getLength(); ++row) {
      //int width = 0, minWidth = 0, maxWidth = INT_MAX;
      int minHeight = 0, height = 0, maxHeight = 0;

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
        int width = getRowHintHeight(row);
      } else {
        int width = Math::max(minHeight, Math::min(width, maxHeight));
      }

      crow->_hintHeight = height;
      crow->_maxHeight = maxHeight;
      crow->_minHeight = minHeight;
    }
    // 
    //     if (this.__colSpans.length > 0) {
    //       this._fixWidthsColSpan(colWidths);
    //     }

    _rowheight = 1;
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

  void GridLayout::setLayoutGeometry(const IntRect&) {

  }
}