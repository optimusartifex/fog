// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_GRIDLAYOUT_H
#define _FOG_GUI_LAYOUT_GRIDLAYOUT_H

// [Dependencies]
#include <Fog/Gui/Layout/Layout.h>
#include <Fog/Core/List.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

  // ============================================================================
  // [Fog::GridLayout]
  // ============================================================================

  //! @brief Base class for all layouts.
  struct FOG_API GridLayout : public Layout
  {
    FOG_DECLARE_OBJECT(GridLayout, Layout)
    GridLayout(Widget* parent=0);

    FOG_INLINE virtual void prepareItem(LayoutItem* item, sysuint_t index) { item->_layoutdata = new(std::nothrow) LayoutProperties(); }

    void addItem(LayoutItem* item, int row, int column, int rowSpan=1, int columnSpan = 1, uint32_t alignment = 0);
    LayoutItem* getCellItem(int row, int column) const;

    FOG_INLINE sysuint_t getColumnCount() const { return _cols.getLength(); } 
    FOG_INLINE float getColumnFlex(int column) const { return column < _cols.getLength() ? _rows.at(column)->_flex : -1; }
    FOG_INLINE int getColumnMinimumWidth(int column) const { return column < _cols.getLength() ? _cols.at(column)->_minWidth : -1; }
    FOG_INLINE int getColumnMaximumWidth(int column) const { return column < _cols.getLength() ? _cols.at(column)->_maxWidth : -1; }
    FOG_INLINE int getColumnHintWidth(int column) const { return column < _cols.getLength() ? _cols.at(column)->_hintWidth : -1; }

    FOG_INLINE sysuint_t getRowCount() const { return _rows.getLength(); }
    FOG_INLINE float getRowFlex(int row) const { return row < _rows.getLength() ? _rows.at(row)->_flex : -1; }    
    FOG_INLINE int getRowMinimumHeight(int row) const { return row < _rows.getLength() ? _rows.at(row)->_minHeight : -1; } 
    FOG_INLINE int getRowMaximumHeight(int row) const { return row < _rows.getLength() ? _rows.at(row)->_maxHeight : -1; } 
    FOG_INLINE int getRowHintHeight(int row) const { return row < _rows.getLength() ? _rows.at(row)->_hintHeight : -1; } 

    //TODO: invalidateLayout()!
    FOG_INLINE void setColumnMinimumWidth(int column, int minsize) { if(column < _cols.getLength()) _cols.at(column)->_minWidth=minsize; }
    FOG_INLINE void setRowMinimumHeight(int row, int minsize) { if(row < _rows.getLength()) _rows.at(row)->_minHeight = minsize; }        
    FOG_INLINE void setColumnMaximumWidth(int column, int maxsize) { if(column < _cols.getLength()) _cols.at(column)->_maxWidth=maxsize; }
    FOG_INLINE void setRowMaximumHeight(int row, int maxsize) { if(row < _rows.getLength()) _rows.at(row)->_maxHeight = maxsize; }     
    FOG_INLINE void setColumnHintWidth(int column, int hintsize) { if(column < _cols.getLength()) _cols.at(column)->_hintWidth=hintsize; }
    FOG_INLINE void setRowHintHeight(int row, int hintsize) { if(row < _rows.getLength()) _rows.at(row)->_hintHeight = hintsize; } 

    FOG_INLINE void setHorizontalSpacing(int spacing) {_hspacing = spacing; }
    FOG_INLINE void setVerticalSpacing(int spacing) {_vspacing = spacing; }


    // ============================================================================
    // [Grid Layout Implementation]
    // ============================================================================
    virtual void calculateLayoutHint(LayoutHint& hint);
    virtual void setLayoutGeometry(const IntRect&);
    FOG_INLINE virtual void invalidateLayout() { _rowheight = 0; _colwidth = 0; Layout::invalidateLayout(); }

  private:

    void calculateColumnWidths();
    void calculateRowHeights();

    struct LayoutProperties : public Layout::LayoutStruct {
      int _colspan;
      int _rowspan;

      int _row;
      int _column;
    };

    struct Column {
      Column() : _minWidth(-1), _maxWidth(-1), _hintWidth(-1), _flex(1.0), _cacheWidth(-1) {}
      int _minWidth;
      int _maxWidth;
      int _hintWidth;

      int _cacheWidth;    //if _cacheWidth < 0 -> cache dirty!

      float _flex;
    };

    struct Row {
      Row(LayoutItem* i) : _minHeight(-1), _maxHeight(-1), _hintHeight(-1), _flex(1.0), _cacheHeight(-1) {}
      int _minHeight;
      int _maxHeight;
      int _hintHeight;

      int _cacheHeight;    //if _cacheHeight < 0 -> cache dirty!

      float _flex;
      List<LayoutItem*> _cols;
    };

    //per Row a Description and the List of LayoutItems
    List<Row*> _rows;

    //with description of Columns
    List<Column*> _cols;

    int _hspacing;
    int _vspacing;

    uint32_t _colwidth : 1;    //marks that the columns width cache is dirty
    uint32_t _rowheight : 1;   //marks that the row height cache is dirty
    uint32_t _unused : 30;     //
  };
}
#endif