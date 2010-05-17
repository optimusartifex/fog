// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_GRIDLAYOUT_H
#define _FOG_GUI_LAYOUT_GRIDLAYOUT_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Gui/Layout/Layout.h>

namespace Fog {

//! @addtogroup Fog_Gui_Layout
//! @{

// ============================================================================
// [Fog::GridLayout]
// ============================================================================

//! @brief Grid layout.
struct FOG_API GridLayout : public Layout
{
  FOG_DECLARE_OBJECT(GridLayout, Layout)

  struct Row;
  struct Column;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GridLayout(Widget* parent=0, int row=-1, int colums=-1);    

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  virtual void onRemove(LayoutItem* item);
  void addItem(LayoutItem* item, int row, int column, int rowSpan=1, int columnSpan = 1, uint32_t alignment = 0);
  LayoutItem* getCellItem(int row, int column) const;
  LayoutItem* takeCellItem(int row, int column);

  // TODO: We should move this to .cpp file. I think it makes no sense to have 
  // this inlined.
  FOG_INLINE sysuint_t getColumnCount() const { return _cols.getLength(); } 
  FOG_INLINE float getColumnFlex(int column) const { return (uint)column < _cols.getLength() ? _cols.at(column)->_flex : 0; }
  FOG_INLINE int getColumnMinimumWidth(int column) const { return (uint)column < _cols.getLength() ? _cols.at(column)->_minWidth : -1; }
  FOG_INLINE int getColumnMaximumWidth(int column) const { return (uint)column < _cols.getLength() ? _cols.at(column)->_maxWidth : -1; }
  FOG_INLINE int getColumnHintWidth(int column) const { return (uint)column < _cols.getLength() ? _cols.at(column)->_hintWidth : -1; }

  FOG_INLINE sysuint_t getRowCount() const { return _rows.getLength(); }
  FOG_INLINE float getRowFlex(int row) const { return (uint)row < _rows.getLength() ? _rows.at(row)->_flex : 0; }    
  FOG_INLINE int getRowMinimumHeight(int row) const { return (uint)row < _rows.getLength() ? _rows.at(row)->_minHeight : -1; } 
  FOG_INLINE int getRowMaximumHeight(int row) const { return (uint)row < _rows.getLength() ? _rows.at(row)->_maxHeight : -1; } 
  FOG_INLINE int getRowHintHeight(int row) const { return (uint)row < _rows.getLength() ? _rows.at(row)->_hintHeight : -1; } 

  //TODO: invalidateLayout()!
  FOG_INLINE void setColumnFlex(int column, float flex) const { if ((uint)column < _cols.getLength()) _cols.at(column)->_flex = flex < 1 ? 0 : flex; }
  FOG_INLINE void setColumnMinimumWidth(int column, int minsize) { if ((uint)column < _cols.getLength()) _cols.at(column)->_minWidth=minsize; }
  FOG_INLINE void setColumnMaximumWidth(int column, int maxsize) { if ((uint)column < _cols.getLength()) _cols.at(column)->_maxWidth=maxsize; }
  FOG_INLINE void setColumnHintWidth(int column, int hintsize) { if ((uint)column < _cols.getLength()) _cols.at(column)->_hintWidth=hintsize; }
  
  FOG_INLINE void setRowFlex(int row, float flex) const { if ((uint)row < _rows.getLength()) _rows.at(row)->_flex = flex < 1 ? 0 : flex; }
  FOG_INLINE void setRowMinimumHeight(int row, int minsize) { if ((uint)row < _rows.getLength()) _rows.at(row)->_minHeight = minsize; }        
  FOG_INLINE void setRowMaximumHeight(int row, int maxsize) { if ((uint)row < _rows.getLength()) _rows.at(row)->_maxHeight = maxsize; }         
  FOG_INLINE void setRowHintHeight(int row, int hintsize) { if ((uint)row < _rows.getLength()) _rows.at(row)->_hintHeight = hintsize; } 

  FOG_INLINE void setHorizontalSpacing(int spacing) {_hspacing = spacing; }
  FOG_INLINE void setVerticalSpacing(int spacing) {_vspacing = spacing; }

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual void calculateLayoutHint(LayoutHint& hint);
  virtual void setLayoutGeometry(const IntRect&);
  virtual void invalidateLayout();

  // TODO: What about visibility, FOG_API?
  struct Column : public LayoutItem::FlexLayoutData
  {
    Column(int colid, GridLayout* layout) : _minWidth(0), _maxWidth(INT_MAX), _hintWidth(-1), _flex(0.0), _colid(colid),_layout(layout)
    {
      FOG_ASSERT(_layout);
      _layoutdata = this;
    }

    void calculateWidth();
 
    void initFlex(Column* flexibles)
    {
      LayoutProperties* widgetProps = static_cast<LayoutProperties*>(_layoutdata);

      widgetProps->_min = _minWidth;
      widgetProps->_max = _maxWidth;
      widgetProps->_hint = _hintWidth;
      widgetProps->_flex = _flex;
      widgetProps->_offset = 0;

      //build simple single linked list
      widgetProps->_next = flexibles;
    }

    FOG_INLINE LayoutItem* getItem(int row) const { return (uint)row < _layout->_rows.getLength() ? _layout->_rows.at(row)->getColumn(_colid) : 0;}
    FOG_INLINE Row* getRow(int row) const { return (uint)row < _layout->_rows.getLength() ? _layout->_rows.at(row) : 0;}

    int _minWidth;
    int _maxWidth;
    int _hintWidth;

    int _colid;

    float _flex;
    GridLayout* _layout;

    //to be able to us Column directly within FlexEngine
    LayoutItem::FlexLayoutData* _layoutdata;
  };

  // TODO: What about visibility, FOG_API?
  struct Row : public LayoutItem::FlexLayoutData
  {
    Row(int rowid) : _minHeight(0), _maxHeight(INT_MAX), _hintHeight(-1), _flex(0.0), _rowid(rowid)
    {
      _layoutdata = this;
    }

    void initFlex(Row* flexibles)
    {
      LayoutProperties* widgetProps = static_cast<LayoutProperties*>(_layoutdata);

      widgetProps->_min = _minHeight;
      widgetProps->_max = _maxHeight;
      widgetProps->_hint = _hintHeight;
      widgetProps->_flex = _flex;
      widgetProps->_offset = 0;

      widgetProps->_next = flexibles;
    }

    void calculateHeight();
    FOG_INLINE LayoutItem* getColumn(int id) const { return (uint)id < _cols.getLength() ? _cols.at(id) : 0; }

    int _minHeight;
    int _maxHeight;
    int _hintHeight;

    int _rowid;

    float _flex;
    List<LayoutItem*> _cols;

    //to be able to us Row directly within FlexEngine
    LayoutItem::FlexLayoutData* _layoutdata;
  };

private:
  void calculateColumnWidths(int& minWidth, int& hintWidth);
  void calculateRowHeights(int& minWidth, int& hintWidth);

  void calculateColumnFlexOffsets(int availWidth);
  void calculateRowFlexOffsets(int availHeight);

  int calculateSpanWidth(int col, LayoutItem*) const;
  int calculateSpanHeight(int row, LayoutItem*) const;

  FOG_INLINE int updateColumnFlexWidth(Column* column) const
  {
    int ret = column->_hintWidth;
    if (column->_offset != 0)
    {
      ret += column->_offset;
      //column->_static._offset = 0;
    }

    return ret;
  }

  FOG_INLINE int updateRowFlexHeight(Row* row) const
  {
    int ret = row->_hintHeight;
    if (row->_offset != 0)
    {
      ret += row->_offset;
      //row->_static._offset = 0;
    }

    return ret;
  }

  FOG_INLINE bool isItemOrigin(LayoutItem* item, Row* row, Column* column) const
  {
    LayoutProperties* widgetProps = static_cast<LayoutProperties*>(item->_layoutdata);
    return widgetProps->_column == column && widgetProps->_row == row;
  }

  struct LayoutProperties : public LayoutItem::FlexLayoutData
  {
    LayoutProperties() : _colspannext(0), _rowspannext(0), _colspan(-1), _rowspan(-1)
    {
    }

    FOG_INLINE bool isOriginOfWidget(int row, int col) const
    {
      return _row->_rowid != row || _column->_colid != col;
    }

    int _colspan;
    int _rowspan;

    Row* _row;
    Column* _column;

    LayoutItem* _colspannext;
    LayoutItem* _rowspannext;
  };

  //per Row a Description and the List of LayoutItems
  List<Row*> _rows;

  Row* _rowflexibles;

  //with description of Columns
  List<Column*> _cols;

  Column* _colflexibles;

  LayoutItem* _colspan;
  LayoutItem* _rowspan;

  int _cacheMinWidth;
  int _cacheHintWidth;
  int _cacheMinHeight;
  int _cacheHintHeight;

  int _hspacing;
  int _vspacing;

  //! @brief Marks that the columns width cache is dirty.
  uint32_t _colwidth : 1;
  //! @brief Marks that the row height cache is dirty.
  uint32_t _rowheight : 1;
  //! @brief Reserved for future use.
  uint32_t _reserved : 30;
};

} // Fog namespace

#endif // _FOG_GUI_LAYOUT_GRIDLAYOUT_H
