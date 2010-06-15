// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_RASTERIZER_ANALYTIC_P_H
#define _FOG_GRAPHICS_RASTERIZER_ANALYTIC_P_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Scanline_p.h>
#include <Fog/Graphics/Rasterizer_p.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::AnalyticRasterizer]
// ============================================================================

//! @internal
//!
//! @brief Analytic rasterizer implementation in C.
//!
//! This was created first as a contribution to antigrain, but currently it's
//! only in Fog.
//!
//! This is custom rasterizer that can be used in multithreaded environment
//! from multiple threads. Method sweepScanline() from antigrain
//! agg::rasterizer_scanline_aa<> template was replaced to method that accepts
//! y coordinate. After you serialize your content use sweepScanline() method 
//! with you own Y coordinate.
//!
//! To use this rasterizer you must first set gamma table that will be used. In
//! multithreaded environment recomputing gamma table in each thread command
//! is not good, so the gamma table is here just const pointer to your real
//! table that is shared across many rasterizer instances. Use setGamma()
//! function to set gamma table.
//!
//! Contribution by Petr Kobalicek <kobalicek.petr@gmail.com>,
//! This contribution follows antigrain license (Public Domain).
struct FOG_HIDDEN AnalyticRasterizer : public Rasterizer
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  AnalyticRasterizer();
  virtual ~AnalyticRasterizer();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void pooled();
  virtual void reset();

  virtual void setClipBox(const IntBox& clipBox);
  virtual void resetClipBox();

  virtual void setError(err_t error);
  virtual void resetError();

  virtual void setFillRule(uint32_t fillRule);
  virtual void setAlpha(uint32_t alpha);

  void updateFunctions();

  virtual void addPath(const DoublePath& path);
  void closePolygon();

  void clipLine(int24x8_t x1, int24x8_t y1, int24x8_t x2, int24x8_t y2, uint f1, uint f2);
  FOG_INLINE void clipLineY(int24x8_t x1, int24x8_t y1, int24x8_t x2, int24x8_t y2, uint f1, uint f2);

  void renderLine(int24x8_t x1, int24x8_t y1, int24x8_t x2, int24x8_t y2);
  FOG_INLINE void renderHLine(int ey, int24x8_t x1, int24x8_t y1, int24x8_t x2, int24x8_t y2);

  FOG_INLINE void addCurCell();
  FOG_INLINE void addCurCell_Always();
  FOG_INLINE void setCurCell(int x, int y);

  // --------------------------------------------------------------------------
  // [Cells / Rows]
  // --------------------------------------------------------------------------

  //! @brief Get sorted cells.
  //!
  //! @note This method is only valid after finalize() call.
  FOG_INLINE const CellX* getCellsSorted() const { return _cellsSorted; }

  //! @brief Get count of cells in _cellsSorted array.
  //!
  //! @note This method is only valid after finalize() call.
  FOG_INLINE sysuint_t getCellsCount() const { return _cellsCount; }

  //! @brief Get whether there are cells in rasterizer.
  //!
  //! @note This method is only valid after finalize() call.
  FOG_INLINE bool hasCells() const { return _cellsCount != 0; }

  //! @brief Rows info (index and count of cells in row).
  //!
  //! @note This method is only valid after finalize() call.
  FOG_INLINE const RowInfo* getRowsInfo() const { return _rowsInfo; }

  //! @brief Get count of rows in _rowsInfo array.
  //!
  //! @note This method is only valid after finalize() call.
  FOG_INLINE sysuint_t getRowsCount() const { return _boundingBox.y2 - _boundingBox.y1; }

  bool nextCellBuffer();
  bool finalizeCellBuffer();

  void freeXYCellBuffers(bool all);

  virtual void finalize();

  template<int _RULE, int _USE_ALPHA>
  FOG_INLINE uint _calculateAlpha(int area) const;

  template<int _RULE, int _USE_ALPHA>
  static Span8* _sweepScanlineSimpleImpl(
    Rasterizer* _rasterizer, Scanline8& scanline, int y);

  template<int _RULE, int _USE_ALPHA>
  static Span8* _sweepScanlineRegionImpl(
    Rasterizer* _rasterizer, Scanline8& scanline, int y,
    const IntBox* clipBoxes, sysuint_t count);

  template<int _RULE, int _USE_ALPHA>
  static Span8* _sweepScanlineSpansImpl(
    Rasterizer* _rasterizer, Scanline8& scanline, int y,
    const Span8* clipSpans);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Sorted cells.
  //!
  //! @note This value is only valid after finalize() call.
  CellX* _cellsSorted;

  //! @brief Sorted cells array capacity.
  //!
  //! @note This value is only valid after finalize() call.
  sysuint_t _cellsCapacity;

  //! @brief Total count of cells in all buffers.
  //!
  //! @note This value is updated only by reset(), nextCellBuffer() and 
  //! finalizeCellBuffer() methods, it not contains exact cells count until
  //! one of these methods isn't called.
  //!
  //! @note This value is only valid after finalize() call.
  sysuint_t _cellsCount;

  //! @brief Rows info (index and count of cells in row).
  //!
  //! @note This value is only valid after finalize() call.
  RowInfo* _rowsInfo;

  //! @brief Rows array capacity.
  //!
  //! @note This value is only valid after finalize() call.
  sysuint_t _rowsCapacity;

  //! @brief Whether rasterizer clipping is enabled.
  int _clipping;
  //! @brief Rasterizer clip box in 24x8 format.
  IntBox _clip24x8;

  //! @brief Current x position.
  int24x8_t _x1;
  //! @brief Current y position.
  int24x8_t _y1;
  //! @brief Current [x, y] clipping flags.
  uint _f1;

  //! @brief Last moveTo x position (for closePolygon).
  int24x8_t _startX1;
  //! @brief Last moveTo y position (for closePolygon).
  int24x8_t _startY1;
  //! @brief Last moveTo clipping flags (for closePolygon).
  uint _startF1;

  //! @brief Pointer to first cell buffer.
  CellXYBuffer* _bufferFirst;
  //! @brief Pointer to last cell buffer (currently used one).
  CellXYBuffer* _bufferLast;
  //! @brief Pointer to currently used cell buffer (this is usually the last 
  //! one, but this is not condition if rasterizer was reused).
  CellXYBuffer* _bufferCurrent;

  //! @brief Current cell in the buffer (_cells).
  CellXY* _curCell;
  //! @brief End cell in the buffer (this cell is first invalid cell in that buffer).
  CellXY* _endCell;

  //! @brief Invalid cell. It is set to _curCell and _endCell if memory
  //! allocation failed. It prevents to dereference the @c NULL pointer.
  CellXY _invalidCell;

private:
  FOG_DISABLE_COPY(AnalyticRasterizer)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_RASTERIZER_ANALYTIC_P_H
