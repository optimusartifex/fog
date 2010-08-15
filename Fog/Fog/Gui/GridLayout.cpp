// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/GridLayout.h>
#include <Fog/Gui/LayoutUtil.h>
#include <Fog/Gui/Widget.h>

FOG_IMPLEMENT_OBJECT(Fog::GridLayout)

namespace Fog {

// ============================================================================
// [Fog::GridLayout]
// ============================================================================

GridLayout::GridLayout(Widget* parent, int rows, int colums) :
  Layout(parent),
  _rowflexibles(0),
  _colflexibles(0),
  _vspacing(0),
  _hspacing(0),
  _colwidth(0),
  _rowheight(0),
  _colspan(0),
  _rowspan(0)
{
  if (rows >= 0 && colums >= 0)
  {
    addItem(0, 0, 0, rows, colums);
  }
}

int GridLayout::getColumnCount() const
{
  return (int)(uint)_cols.getLength();
}

float GridLayout::getColumnFlex(int column) const
{
  return (uint)column < _cols.getLength() ? _cols.at(column)->_flex : 0;
}

int GridLayout::getColumnMinimumWidth(int column) const
{
  return (uint)column < _cols.getLength() ? _cols.at(column)->_minWidth : -1;
}

int GridLayout::getColumnMaximumWidth(int column) const
{
  return (uint)column < _cols.getLength() ? _cols.at(column)->_maxWidth : -1;
}

int GridLayout::getColumnHintWidth(int column) const
{
  return (uint)column < _cols.getLength() ? _cols.at(column)->_hintWidth : -1;
}

int GridLayout::getRowCount() const
{
  return (int)(uint)_rows.getLength();
}

float GridLayout::getRowFlex(int row) const
{
  return (uint)row < _rows.getLength() ? _rows.at(row)->_flex : 0;
}

int GridLayout::getRowMinimumHeight(int row) const
{
  return (uint)row < _rows.getLength() ? _rows.at(row)->_minHeight : -1;
}

int GridLayout::getRowMaximumHeight(int row) const
{
  return (uint)row < _rows.getLength() ? _rows.at(row)->_maxHeight : -1;
}

int GridLayout::getRowHintHeight(int row) const
{
  return (uint)row < _rows.getLength() ? _rows.at(row)->_hintHeight : -1;
}

void GridLayout::setColumnFlex(int column, float flex)
{
  if ((uint)column < _cols.getLength()) _cols.at(column)->_flex = flex < 1 ? 0 : flex;
}

void GridLayout::setColumnMinimumWidth(int column, int minsize)
{
  if ((uint)column < _cols.getLength()) _cols.at(column)->_minWidth = minsize;
}

void GridLayout::setColumnMaximumWidth(int column, int maxsize)
{
  if ((uint)column < _cols.getLength()) _cols.at(column)->_maxWidth = maxsize;
}

void GridLayout::setColumnHintWidth(int column, int hintsize)
{
  if ((uint)column < _cols.getLength()) _cols.at(column)->_hintWidth = hintsize;
}

void GridLayout::setRowFlex(int row, float flex)
{
  if ((uint)row < _rows.getLength()) _rows.at(row)->_flex = flex < 1 ? 0 : flex;
}

void GridLayout::setRowMinimumHeight(int row, int minsize)
{
  if ((uint)row < _rows.getLength()) _rows.at(row)->_minHeight = minsize;
}

void GridLayout::setRowMaximumHeight(int row, int maxsize)
{
  if ((uint)row < _rows.getLength()) _rows.at(row)->_maxHeight = maxsize;
}

void GridLayout::setRowHintHeight(int row, int hintsize)
{
  if ((uint)row < _rows.getLength()) _rows.at(row)->_hintHeight = hintsize;
}

void GridLayout::onRemove(LayoutItem* item)
{
  LayoutProperties* prop = item->getLayoutData<LayoutProperties>();

  // An item was removed before itemproperties are created.
  if (!prop)
    return;

  // Clean up data within row/col structure.
  for (int i = prop->_row->_rowid; i <prop->_rowspan; i++)
  {
    for (int j = prop->_column->_colid; j <prop->_colspan; j++)
    {
      takeCellItem(prop->_row->_rowid + i, prop->_column->_colid + j);
    }
  }

  // Clean up colspan chain.
  if (prop->_colspan > 1)
  {
    LayoutItem* iter = _colspan;
    LayoutItem* prev = 0;

    if (item == iter)
    {
      _colspan = item->getLayoutData<LayoutProperties>()->_colspannext;
    }
    else
    {
      while (iter)
      {
        LayoutProperties* prop = iter->getLayoutData<LayoutProperties>();
        if (iter == item)
        {
          prev->getLayoutData<LayoutProperties>()->_colspannext = prop->_colspannext;
          prop->_colspannext = 0;
          break;
        }
        prev = iter;
        iter = prop->_colspannext;
      }
    }
  }

  // Clean up rowspan chain.
  if (prop->_rowspan > 1)
  {
    LayoutItem* iter = _rowspan;
    LayoutItem* prev = 0;

    if (item == iter)
    {
      _colspan = item->getLayoutData<LayoutProperties>()->_rowspannext;
    }
    else
    {
      while (iter)
      {
        LayoutProperties* prop = iter->getLayoutData<LayoutProperties>();

        if (iter == item)
        {
          prev->getLayoutData<LayoutProperties>()->_rowspannext = prop->_rowspannext;
          prop->_rowspannext = 0;
          break;
        }
        prev = iter;
        iter = prop->_rowspannext;
      }
    }
  }    
}

void GridLayout::addItem(LayoutItem* item, int row, int column, int rowSpan, int columnSpan, uint32_t alignment)
{
  if (item)
  {
    if (Layout::addChild(item) == -1)
      return;
  }

  if (row == -1)
    row = (int)_rows.getLength();

  if (column == -1)
    column = (int)_cols.getLength();

  // Check that it's not already there...
  // TODO: needs some improvement!
  if (item)
  {
    for (int i = 0; i < rowSpan; i++)
    {
      for (int j = 0; j < columnSpan; j++)
      {
        if (getCellItem(row + i, column + j))
        {
          //Warning already exists!
          remove(item);
          return;
        }
      }
    }

    item->_layoutdata = fog_new LayoutProperties();
  }

  // Add Columns and Rows needed for insertion
  // TODO: use reserve to prevent unneeded memory allocations!
  int ccolumn = column+columnSpan;
  int crow = row+rowSpan;

  Row* itemrow = 0;
  Column* itemcolumn = 0;

  if (_cols.getCapacity() < (uint)ccolumn)
  {
    _cols.reserve(ccolumn);
  }

  if (_rows.getCapacity() < (uint)crow)
  {
    _cols.reserve(crow);
  }

  // Check if enough columns are there.
  if ((uint)ccolumn > _cols.getLength())
  {
    while ((uint)ccolumn > _cols.getLength())
    {
      Column* c = fog_new Column((int)_cols.getLength(), this);
      _cols.append(c);
    }
  }

  // Create enough rows.
  if ((uint)crow > _rows.getLength())
  {
    while ((uint)crow > _rows.getLength())
    {
      Row* r = fog_new Row((int)_rows.getLength());
      _rows.append(r);
    }
  }

  // Now we need to create enough columns per row!
  for (int i = 0; i < rowSpan; i++)
  {
    Row* r = _rows.at(row+i);

    // Append enough columns into row.
    while ((uint)ccolumn > r->_cols.getLength())
    {
      r->_cols.append(0);
    }

    // Set item into column within row.
    for (int j = 0; j < columnSpan; j++)
    {
      r->_cols.set(column+j, item);
    }
  }

  if (item)
  {
    LayoutProperties* prop = static_cast<LayoutProperties*>(item->_layoutdata);
    prop->_row = _rows.at(row);
    prop->_rowspan = rowSpan;
    prop->_column = _cols.at(column);
    prop->_colspan = columnSpan;

    item->setLayoutAlignment(alignment);

    // Create simple single linked lists for fast span iterations.
    if (columnSpan > 1)
    {
      prop->_colspannext = _colspan;
      _colspan = item;
    }

    if (rowSpan > 1)
    {
      prop->_rowspannext = _rowspan;
      _rowspan = item;
    }
  }
}

LayoutItem* GridLayout::takeCellItem(int row, int column)
{
  FOG_ASSERT(row >= 0 && column >=0);
  if ((uint)column >= _cols.getLength() || (uint)row >= _rows.getLength())
    return 0;

  Row* r = _rows.at(row);

  if ((uint)column >= r->_cols.getLength()) 
    return 0;

  return r->_cols.take(column);
}

LayoutItem* GridLayout::getCellItem(int row, int column) const
{
  FOG_ASSERT(row >= 0 && column >=0);

  if ((uint)column >= _cols.getLength() || (uint)row >= _rows.getLength())
    return 0;

  Row* r = _rows.at(row);

  if ((uint)column >= r->_cols.getLength()) 
    return 0;

  return r->_cols.at(column);
}

void GridLayout::Column::calculateWidth()
{
  // LAYOUT TODO: Column Cache handling!
  int width = 0, minWidth = 0;

  for (int row = 0; row < (sysint_t)_layout->_rows.getLength(); ++row)
  {
    LayoutItem* item = getItem(row);
    if (!item)
    {
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

void GridLayout::Row::calculateHeight()
{
  int minHeight = 0, height = 0;

  for (int col = 0; col < (sysint_t)_cols.getLength(); ++col)
  {
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

void GridLayout::calculateColumnWidths(int& minWidth, int& hintWidth)
{
  if (_colwidth == 1)
  {
    minWidth = _cacheMinWidth;
    hintWidth = _cacheHintWidth;
    return;
  }

  _cacheMinWidth = 0;
  _cacheHintWidth = 0;

  _colflexibles = 0;

  for (int col = 0; col < (sysint_t)_cols.getLength(); col++)
  {
    Column* column = _cols.at(col);
    column->calculateWidth();

    _cacheMinWidth += column->_flex > 0 ? column->_minWidth : column->_hintWidth;
    _cacheHintWidth += column->_hintWidth;

    if ((column->_flex > 0))
    {
      column->initFlex(_colflexibles);
      _colflexibles = column;
    }
  }

  minWidth = _cacheMinWidth;
  hintWidth = _cacheHintWidth;
  _colwidth = 1;
}

void GridLayout::calculateRowHeights(int& minHeight, int& hintHeight)
{
  if (_rowheight == 1)
  {
    minHeight = _cacheMinHeight;
    hintHeight = _cacheHintHeight;
    return;
  }

  _cacheMinHeight = 0;
  _cacheHintHeight = 0;

  _rowflexibles = 0;
  
  for (int crow = 0; crow < (sysint_t)_rows.getLength(); ++crow)
  {
     Row* row = _rows.at(crow);
     row->calculateHeight();

     _cacheMinHeight += row->_flex > 0 ? row->_minHeight : row->_hintHeight;
     _cacheHintHeight += row->_hintHeight;

     if ((row->_flex > 0))
     {
       row->initFlex(_rowflexibles);
       _rowflexibles = row;
     }
  }

  minHeight = _cacheMinHeight;
  hintHeight = _cacheHintHeight;
  _rowheight = 1;
}

void GridLayout::calculateColumnFlexOffsets(int availWidth)
{    
  int hintwidth = getLayoutSizeHint().getWidth();
  int diff = availWidth - hintwidth;

  if (diff == 0)
    return;
  
  if (_colflexibles)
    LayoutUtil::calculateFlexOffsets(_colflexibles, availWidth, hintwidth);
}

void GridLayout::calculateRowFlexOffsets(int availHeight)
{
  int hintHeight = getLayoutSizeHint().getHeight();
  int diff = availHeight - hintHeight;

  if (diff == 0)
    return;

  if (_rowflexibles)
    LayoutUtil::calculateFlexOffsets(_rowflexibles, availHeight, hintHeight);
}

void GridLayout::calculateLayoutHint(LayoutHint& hint)
{
  // Calculate column widths.
  int minWidth, width;
  calculateColumnWidths(minWidth, width);

  // Calculate row heights.
  int minHeight, height;
  calculateRowHeights(minHeight, height);

  // Calculate spacing.
  int spacingX = _hspacing * (int)(_cols.getLength() - 1);
  int spacingY = _vspacing * (int)(_rows.getLength() - 1);

  // Calculate margin.
  spacingX += getContentLeftMargin() + getContentRightMargin();
  spacingY += getContentTopMargin() + getContentBottomMargin();

  hint._minimumSize.set(minWidth + spacingX,minHeight + spacingY);
  hint._sizeHint.set(width + spacingX,height + spacingY);
}

int GridLayout::calculateSpanWidth(int col, LayoutItem* item) const
{
  LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);

  // Compute sizes width including cell spanning.
  int width = _hspacing * (widgetProps->_colspan - 1);
  for (int i=0; i<widgetProps->_colspan; ++i)
  {
    Column* column = _cols.at(col+i);
    // Update if flex was calculated for this column.
    width += updateColumnFlexWidth(column);
  }

  return width;
}

int GridLayout::calculateSpanHeight(int crow, LayoutItem* item) const
{
  LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);

  int height = _vspacing * (widgetProps->_rowspan - 1);
  for (int i=0; i<widgetProps->_rowspan; i++)
  {
    Row* row = _rows.at(crow+i);
    // Update if flex was calculated for this row.
    height += updateRowFlexHeight(row);      
  }

  return height;
}

void GridLayout::setLayoutGeometry(const IntRect& rect)
{
  int maxRowIndex = (int)_rows.getLength();
  int maxColIndex = (int)_cols.getLength();

  if (maxRowIndex == 0 || maxColIndex == 0)
    return;

  int availWidth = rect.getWidth();
  int availHeight = rect.getHeight();

  calculateColumnFlexOffsets(availWidth);
  calculateRowFlexOffsets(availHeight);

  // Do the layout.
  int curleft = rect.x + getContentLeftMargin();
  int top = rect.y + getContentTopMargin();

  for (int col=0; col<maxColIndex; ++col)
  {
    Column* column = _cols.at(col);
    int curtop = top;

    //int x = updateColumnFlexWidth(column); 
    int width = updateColumnFlexWidth(column);

    for (int crow=0; crow<maxRowIndex; ++crow)
    {
      Row* row = _rows.at(crow);
      LayoutItem* item = getCellItem(crow,col);

      // LAYOUT TODO:
      // int y = updateRowFlexHeight(row);
      int height = updateRowFlexHeight(row);
      
      if (!item)
      {
        // Ignore empty cells.
        curtop += height + _vspacing;
        continue;
      }

      LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);
      if (!isItemOrigin(item, row, column))
      {
        // Ignore cells, which have cell spanning but are not the origin of 
        // the item.
        curtop += height + _vspacing;
        continue;
      }

      int spanWidth = calculateSpanWidth(col, item);
      int spanHeight = calculateSpanHeight(crow, item);

      LayoutHint cellHint = item->getLayoutHint();

      int hmargins = item->getContentTopMargin() + item->getContentBottomMargin();
      int vmargins = item->getContentLeftMargin() + item->getContentRightMargin();

      int cellWidth = Math::max(cellHint.getMinimumSize().getWidth(), Math::min(spanWidth-vmargins, cellHint.getMaximumSize().getWidth()));
      int cellHeight = Math::max(cellHint.getMinimumSize().getHeight(), Math::min(spanHeight-hmargins, cellHint.getMaximumSize().getHeight()));

      // LAYOUT TODO: Use CellAlignment if item has smaller size than size of cell!
      int cellLeft = curleft + item->getContentLeftMargin();
      int cellTop = curtop + item->getContentTopMargin();

      item->setLayoutGeometry(IntRect(cellLeft, cellTop, cellWidth, cellHeight));

      curtop += height + _vspacing;
    }

    curleft += width + _hspacing;
  }
}

void GridLayout::invalidateLayout()
{
  Layout::invalidateLayout();
  _rowheight = 0;
  _colwidth = 0;
}

} // Fog namespace
