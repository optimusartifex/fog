// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_GRIDLAYOUT_H
#define _FOG_GUI_GRIDLAYOUT_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Gui/Layout.h>

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

  int getColumnCount() const;
  float getColumnFlex(int column) const;
  int getColumnMinimumWidth(int column) const;
  int getColumnMaximumWidth(int column) const;
  int getColumnHintWidth(int column) const;

  int getRowCount() const;
  float getRowFlex(int row) const;
  int getRowMinimumHeight(int row) const;
  int getRowMaximumHeight(int row) const;
  int getRowHintHeight(int row) const;

  //TODO: invalidateLayout()!
  void setColumnFlex(int column, float flex);
  void setColumnMinimumWidth(int column, int minsize);
  void setColumnMaximumWidth(int column, int maxsize);
  void setColumnHintWidth(int column, int hintsize);

  void setRowFlex(int row, float flex);
  void setRowMinimumHeight(int row, int minsize);
  void setRowMaximumHeight(int row, int maxsize);
  void setRowHintHeight(int row, int hintsize);

  virtual void onRemove(LayoutItem* item);
  void addItem(LayoutItem* item, int row, int column, int rowSpan = 1, int columnSpan = 1, uint32_t alignment = 0);
  LayoutItem* getCellItem(int row, int column) const;
  LayoutItem* takeCellItem(int row, int column);

  FOG_INLINE void setHorizontalSpacing(int spacing) {_hspacing = spacing; }
  FOG_INLINE void setVerticalSpacing(int spacing) {_vspacing = spacing; }

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual void calculateLayoutHint(LayoutHint& hint);
  virtual void setLayoutGeometry(const RectI&);
  virtual void invalidateLayout();

  // TODO: What about visibility, FOG_API?
  struct Column : public LayoutItem::FlexLayoutData
  {
    Column(int colid, GridLayout* layout) : _minWidth(0), _maxWidth(INT_MAX), _hintWidth(-1), _colid(colid), _flex(0.0), _layout(layout)
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

      // Build simple single linked list.
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

    // To be able to us Column directly within FlexEngine.
    LayoutItem::FlexLayoutData* _layoutdata;
  };

  // TODO: What about visibility, FOG_API?
  struct Row : public LayoutItem::FlexLayoutData
  {
    Row(int rowid) : _minHeight(0), _maxHeight(INT_MAX), _hintHeight(-1), _rowid(rowid), _flex(0.0)
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

    // To be able to us Row directly within FlexEngine.
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
      // LAYOUT TODO:
      // column->_static._offset = 0;
    }

    return ret;
  }

  FOG_INLINE int updateRowFlexHeight(Row* row) const
  {
    int ret = row->_hintHeight;
    if (row->_offset != 0)
    {
      ret += row->_offset;
      // LAYOUT TODO:
      // row->_static._offset = 0;
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
    LayoutProperties() : _colspan(-1), _rowspan(-1), _colspannext(0), _rowspannext(0)
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

#endif // _FOG_GUI_GRIDLAYOUT_H
