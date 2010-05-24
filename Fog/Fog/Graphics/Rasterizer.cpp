// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.4
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
//
// The author gratefully acknowleges the support of David Turner, 
// Robert Wilhelm, and Werner Lemberg - the authors of the FreeType 
// libray - in producing this work. See http://www.freetype.org for details.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//
// Adaptation for 32-bit screen coordinates has been sponsored by 
// Liberty Technology Systems, Inc., visit http://lib-sys.com
//
// Liberty Technology Systems, Inc. is the provider of
// PostScript and PDF technology for software developers.
//
//----------------------------------------------------------------------------

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/AutoLock.h>
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Graphics/ByteUtil_p.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Graphics/Rasterizer_p.h>
#include <Fog/Graphics/Span_p.h>

namespace Fog {

// ============================================================================
// [Fog::Rasterizer - Debugging]
// ============================================================================

// #define FOG_DEBUG_RASTERIZER)

// ============================================================================
// [Fog::Rasterizer - Constants]
// ============================================================================

enum POLY_SUBPIXEL_ENUM
{
  POLY_SUBPIXEL_SHIFT = 8,                        // 8
  POLY_SUBPIXEL_SCALE = 1 << POLY_SUBPIXEL_SHIFT, // 256
  POLY_SUBPIXEL_MASK  = POLY_SUBPIXEL_SCALE-1,    // 255
};

enum AA_SCALE_ENUM
{
  AA_SHIFT   = 8,              // 8
  AA_SCALE   = 1 << AA_SHIFT,  // 256
  AA_MASK    = AA_SCALE - 1,   // 255
  AA_SCALE_2 = AA_SCALE * 2,   // 512
  AA_MASK_2  = AA_SCALE_2 - 1  // 511
};

enum
{
  RASTERIZER_QSORT_THRESHOLD = 9
};

struct FOG_HIDDEN RasterizerUtil
{
  static FOG_INLINE int muldiv(double a, double b, double c) { return Math::iround(a * b / c); }
  static FOG_INLINE int upscale(double v) { return Math::iround(v * POLY_SUBPIXEL_SCALE); }
};

// Liang-Barsky clipping.
struct FOG_HIDDEN LiangBarsky
{
  // Clipping flags.
  enum FLAGS
  {
    CLIPPED_X1 = 0x4,
    CLIPPED_X2 = 0x1,
    CLIPPED_Y1 = 0x8,
    CLIPPED_Y2 = 0x2,
    CLIPPED_X  = CLIPPED_X1 | CLIPPED_X2,
    CLIPPED_Y  = CLIPPED_Y1 | CLIPPED_Y2
  };

  //----------------------------------------------------------clipping_flags
  // Determine the clipping code of the vertex according to the 
  // Cyrus-Beck line clipping algorithm
  //
  //        |        |
  //  0110  |  0010  | 0011
  //        |        |
  // -------+--------+-------- clipBox.y2
  //        |        |
  //  0100  |  0000  | 0001
  //        |        |
  // -------+--------+-------- clipBox.y1
  //        |        |
  //  1100  |  1000  | 1001
  //        |        |
  //  clipBox.x1  clipBox.x2
  //
  // 
  template<typename T, typename BoxT>
  static FOG_INLINE uint getClippingFlags(T x, T y, const BoxT& clipBox)
  {
    return (x > clipBox.x2) | ((y > clipBox.y2) << 1) | ((x < clipBox.x1) << 2) | ((y < clipBox.y1) << 3);
  }

  template<typename T, typename BoxT>
  static FOG_INLINE uint getClippingFlagsX(T x, const BoxT& clipBox)
  {
    return (x > clipBox.x2) | ((x < clipBox.x1) << 2);
  }

  template<typename T, typename BoxT>
  static FOG_INLINE uint getClippingFlagsY(T y, const BoxT& clipBox)
  {
    return ((y > clipBox.y2) << 1) | ((y < clipBox.y1) << 3);
  }

  // Clip LiangBarsky.
  template<typename T, typename BoxT>
  static FOG_INLINE uint clipLiangBarsky(T x1, T y1, T x2, T y2, const BoxT& clipBox,
    T* FOG_RESTRICT x, T* FOG_RESTRICT y)
  {
    const double nearzero = 1e-30;

    double deltax = x2 - x1, xin, xout, tinx, toutx;
    double deltay = y2 - y1, yin, yout, tiny, touty;
    double tin1;
    double tin2;
    double tout1;
    uint np = 0;

    // Bump off of the vertical.
    if (deltax == 0.0) deltax = (x1 > clipBox.x1) ? -nearzero : nearzero;
    // Bump off of the horizontal.
    if (deltay == 0.0) deltay = (y1 > clipBox.y1) ? -nearzero : nearzero;

    if (deltax > 0.0) 
    {                
      // Points to right.
      xin  = clipBox.x1;
      xout = clipBox.x2;
    }
    else 
    {
      xin  = clipBox.x2;
      xout = clipBox.x1;
    }

    if (deltay > 0.0) 
    {
      // Points up.
      yin  = clipBox.y1;
      yout = clipBox.y2;
    }
    else 
    {
      yin  = clipBox.y2;
      yout = clipBox.y1;
    }
    
    tinx = (xin - x1) / deltax;
    tiny = (yin - y1) / deltay;
    
    if (tinx < tiny) 
    {
      // Hits X first.
      tin1 = tinx;
      tin2 = tiny;
    }
    else
    {
      // Hits Y first.
      tin1 = tiny;
      tin2 = tinx;
    }
    
    if (tin1 <= 1.0) 
    {
      if (tin1 > 0.0) 
      {
        *x++ = (T)xin;
        *y++ = (T)yin;
        ++np;
      }

      if (tin2 <= 1.0)
      {
        toutx = (xout - x1) / deltax;
        touty = (yout - y1) / deltay;
        
        tout1 = (toutx < touty) ? toutx : touty;
        
        if (tin2 > 0.0 || tout1 > 0.0) 
        {
          if (tin2 <= tout1) 
          {
            if (tin2 > 0.0) 
            {
              if (tinx > tiny) 
              {
                *x++ = (T)xin;
                *y++ = (T)(y1 + tinx * deltay);
              }
              else 
              {
                *x++ = (T)(x1 + tiny * deltax);
                *y++ = (T)yin;
              }
              ++np;
            }

            if (tout1 < 1.0) 
            {
              if (toutx < touty) 
              {
                *x++ = (T)xout;
                *y++ = (T)(y1 + toutx * deltay);
              }
              else 
              {
                *x++ = (T)(x1 + touty * deltax);
                *y++ = (T)yout;
              }
            }
            else 
            {
              *x++ = x2;
              *y++ = y2;
            }
            ++np;
          }
          else 
          {
            if (tinx > tiny) 
            {
              *x++ = (T)xin;
              *y++ = (T)yout;
            }
            else 
            {
              *x++ = (T)xout;
              *y++ = (T)yin;
            }
            ++np;
          }
        }
      }
    }
    return np;
  }

  // Move point.
  template<typename T, typename BoxT>
  static FOG_INLINE bool clipMovePoint(T x1, T y1, T x2, T y2, const BoxT& clipBox,
    T* FOG_RESTRICT x,
    T* FOG_RESTRICT y, uint flags)
  {
    if (flags & CLIPPED_X)
    {
      if (x1 == x2) return false;

      T bound = (flags & CLIPPED_X1) ? clipBox.x1 : clipBox.x2;
      *y = (T)(double(bound - x1) * (y2 - y1) / (x2 - x1) + y1);
      *x = bound;
    }

    flags = getClippingFlagsY(*y, clipBox);

    if (flags & CLIPPED_Y)
    {
      if (y1 == y2) return false;

      T bound = (flags & CLIPPED_Y1) ? clipBox.y1 : clipBox.y2;
      *x = (T)(double(bound - y1) * (x2 - x1) / (y2 - y1) + x1);
      *y = bound;
    }

    return true;
  }

  // Clip line segment.
  //
  // Returns:
  //   (ret    ) >= 4  - Fully clipped
  //   (ret & 1) != 0  - First point has been moved
  //   (ret & 2) != 0  - Second point has been moved
  template<typename T, typename BoxT>
  static FOG_INLINE unsigned clipLineSegment(
    T* FOG_RESTRICT x1, T* FOG_RESTRICT y1,
    T* FOG_RESTRICT x2, T* FOG_RESTRICT y2, const BoxT& clipBox)
  {
    uint f1 = getClippingFlags(*x1, *y1, clipBox);
    uint f2 = getClippingFlags(*x2, *y2, clipBox);
    uint ret = 0;

    // Fully visible
    if ((f2 | f1) == 0) return 0;

    // Fully clipped
    if ((f1 & CLIPPED_X) != 0 && (f1 & CLIPPED_X) == (f2 & CLIPPED_X)) return 4;
    if ((f1 & CLIPPED_Y) != 0 && (f1 & CLIPPED_Y) == (f2 & CLIPPED_Y)) return 4;

    T tx1 = *x1;
    T ty1 = *y1;
    T tx2 = *x2;
    T ty2 = *y2;

    if (f1)
    {
      if (!clipMovePoint(tx1, ty1, tx2, ty2, clipBox, x1, y1, f1)) return 4;
      if (*x1 == *x2 && *y1 == *y2) return 4;
      ret |= 1;
    }

    if (f2)
    {
      if (!clipMovePoint(tx1, ty1, tx2, ty2, clipBox, x2, y2, f2)) return 4;
      if (*x1 == *x2 && *y1 == *y2) return 4;
      ret |= 2;
    }

    return ret;
  }
};

// ============================================================================
// [Fog::Rasterizer - Local]
// ============================================================================

struct FOG_HIDDEN RasterizerLocal
{
  RasterizerLocal() : 
    rasterizers(NULL),
    cellBuffers(NULL),
    cellsBufferCapacity(2048)
  {
  }

  ~RasterizerLocal()
  {
  }

  Lock lock;

  Rasterizer* rasterizers;
  Rasterizer::CellXYBuffer* cellBuffers;

  uint32_t cellsBufferCapacity;
};

static Static<RasterizerLocal> rasterizer_local;

// ============================================================================
// [Fog::AnalyticRasterizer]
// ============================================================================

//! @brief Rasterizer implementation in C.
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
//! Note, gamma table is BYTE type not int as in original rasterizer.
//!
//! Contribution by Petr Kobalicek <kobalicek.petr@gmail.com>,
//! This contribution follows antigrain license (Public Domain).
struct FOG_HIDDEN AnalyticRasterizer : public Rasterizer
{
  AnalyticRasterizer();
  virtual ~AnalyticRasterizer();

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

// ============================================================================
// [Fog::Rasterizer]
// ============================================================================

Rasterizer::Rasterizer()
{
  // Must be set by the real rasterizer.
  _sweepScanlineSimpleFn = NULL;
  _sweepScanlineRegionFn = NULL;
  _sweepScanlineSpansFn = NULL;

  _clipBox.clear();
  _cellsBounds.clear();

  _error = ERR_OK;

  // Set fill rule to invalid, real rasterizer will set it to default in 
  // construction time (and also set the _sweep... methods).
  _fillRule = FILL_RULE_COUNT;
  // Default alpha is full-opaque.
  _alpha = 0xFF;

  _finalized = false;
  _autoClose = true;

  _cellsSorted = NULL;
  _cellsCapacity = 0;
  _cellsCount = 0;

  _rowsInfo = NULL;
  _rowsCapacity = 0;

  _poolNext = NULL;
}

Rasterizer::~Rasterizer()
{
  if (_cellsSorted) Memory::free(_cellsSorted);
  if (_rowsInfo) Memory::free(_rowsInfo);
}

// ============================================================================
// [Fog::Rasterizer - Pooling]
// ============================================================================

Rasterizer* Rasterizer::getRasterizer()
{
  AutoLock locked(rasterizer_local->lock);
  Rasterizer* rasterizer;

  if (rasterizer_local->rasterizers)
  {
    rasterizer = rasterizer_local->rasterizers;
    rasterizer_local->rasterizers = rasterizer->_poolNext;
    rasterizer->_poolNext = NULL;
  }
  else
  {
    rasterizer = new(std::nothrow) AnalyticRasterizer();
  }

  return rasterizer;
}

void Rasterizer::releaseRasterizer(Rasterizer* rasterizer)
{
  AutoLock locked(rasterizer_local->lock);

  rasterizer->_poolNext = rasterizer_local->rasterizers;
  rasterizer_local->rasterizers = rasterizer;
}

Rasterizer::CellXYBuffer* Rasterizer::getCellXYBuffer()
{
  CellXYBuffer* cellBuffer = NULL;

  {
    AutoLock locked(rasterizer_local->lock);
    if (rasterizer_local->cellBuffers)
    {
      cellBuffer = rasterizer_local->cellBuffers;
      rasterizer_local->cellBuffers = cellBuffer->next;
      cellBuffer->next = NULL;
      cellBuffer->prev = NULL;
      cellBuffer->count = 0;
    }
  }

  if (cellBuffer == NULL)
  {
    cellBuffer = (CellXYBuffer*)Memory::alloc(
      sizeof(CellXYBuffer) - sizeof(CellXY) + sizeof(CellXY) * rasterizer_local->cellsBufferCapacity);
    if (cellBuffer == NULL) return NULL;

    cellBuffer->next = NULL;
    cellBuffer->prev = NULL;
    cellBuffer->count = 0;
    cellBuffer->capacity = rasterizer_local->cellsBufferCapacity;
  }

  return cellBuffer;
}

void Rasterizer::releaseCellXYBuffer(CellXYBuffer* cellBuffer)
{
  AutoLock locked(rasterizer_local->lock);

  // Get last.
  CellXYBuffer* last = cellBuffer;
  while (last->next) last = last->next;

  last->next = rasterizer_local->cellBuffers;
  rasterizer_local->cellBuffers = cellBuffer;
}

void Rasterizer::cleanup()
{
  // Free all rasterizers.
  Rasterizer* rasterizerCurr;
  Rasterizer* rasterizerNext;

  {
    AutoLock locked(rasterizer_local->lock);

    rasterizerCurr = rasterizer_local->rasterizers;
    rasterizer_local->rasterizers = NULL;
  }

  while (rasterizerCurr)
  {
    rasterizerNext = rasterizerCurr->_poolNext;
    delete rasterizerCurr;
    rasterizerCurr = rasterizerNext;
  }

  // Free all cell buffers.
  CellXYBuffer* bufferCurr;
  CellXYBuffer* bufferNext;

  {
    AutoLock locked(rasterizer_local->lock);

    bufferCurr = rasterizer_local->cellBuffers;
    rasterizer_local->cellBuffers = NULL;
  }

  while (bufferCurr)
  {
    bufferNext = bufferCurr->next;
    Memory::free(bufferCurr);
    bufferCurr = bufferNext;
  }
}

// ============================================================================
// [Fog::AnalyticRasterizer - Construction / Destruction]
// ============================================================================

AnalyticRasterizer::AnalyticRasterizer()
{
  _bufferFirst = Rasterizer::getCellXYBuffer();
  _bufferLast = _bufferFirst;

  setFillRule(FILL_DEFAULT);

  reset();
}

AnalyticRasterizer::~AnalyticRasterizer()
{
  if (_bufferFirst) Rasterizer::releaseCellXYBuffer(_bufferFirst);
}

void AnalyticRasterizer::pooled()
{
  reset();

  // Defaults.
  _fillRule = FILL_EVEN_ODD;
  _autoClose = true;

  freeXYCellBuffers(false);

  // If this rasterizer was used for something really big, we will free the
  // memory.
  if (_cellsCapacity > 1024)
  {
    Memory::free(_cellsSorted);
    _cellsSorted = NULL;
    _cellsCapacity = 0;
  }
  if (_rowsCapacity > 1024)
  {
    Memory::free(_rowsInfo);
    _rowsInfo = NULL;
    _rowsCapacity = 0;
  }
}

void AnalyticRasterizer::reset()
{
  _error = ERR_OK;

  _clipping = false;
  _clipBox.clear();
  _clip24x8.clear();

  _finalized = false;

  _x1 = 0;
  _y1 = 0;
  _f1 = 0;

  _startX1 = 0;
  _startY1 = 0;
  _startF1 = 0;

  _bufferCurrent = _bufferFirst;

  _curCell = &_invalidCell;
  _endCell = _curCell + 1;

  _cellsCount = 0; 
  _cellsBounds.set(0x7FFFFFFF, 0x7FFFFFFF, -0x7FFFFFFF, -0x7FFFFFFF);
}

// ============================================================================
// [Fog::AnalyticRasterizer - Clipping]
// ============================================================================

void AnalyticRasterizer::setClipBox(const IntBox& clipBox)
{
  _clipping = true;
  _clipBox = clipBox;

  _clip24x8.set(clipBox.x1 << POLY_SUBPIXEL_SHIFT, clipBox.y1 << POLY_SUBPIXEL_SHIFT,
                clipBox.x2 << POLY_SUBPIXEL_SHIFT, clipBox.y2 << POLY_SUBPIXEL_SHIFT);
}

void AnalyticRasterizer::resetClipBox()
{
  _clipping = false;
  _clipBox.clear();
  _clip24x8.clear();
}

// ============================================================================
// [Fog::AnalyticRasterizer - Error]
// ============================================================================

void AnalyticRasterizer::setError(err_t error)
{
  _error = error;
}

void AnalyticRasterizer::resetError()
{
  _error = ERR_OK;
}

// ============================================================================
// [Fog::AnalyticRasterizer - Fill Rule]
// ============================================================================

void AnalyticRasterizer::setFillRule(uint32_t fillRule)
{
  if (_fillRule == fillRule) return;
  FOG_ASSERT(fillRule < FILL_RULE_COUNT);

  _fillRule = fillRule;
  updateFunctions();
}

// ============================================================================
// [Fog::AnalyticRasterizer - Alpha]
// ============================================================================

void AnalyticRasterizer::setAlpha(uint32_t alpha)
{
  if (_alpha == alpha) return;
  FOG_ASSERT(alpha <= 0xFF);

  _alpha = alpha;
  updateFunctions();
}

// ============================================================================
// [Fog::AnalyticRasterizer - Update Functions]
// ============================================================================

void AnalyticRasterizer::updateFunctions()
{
  switch (_fillRule)
  {
    case FILL_NON_ZERO:
      if (_alpha == 0xFF)
      {
        _sweepScanlineSimpleFn = _sweepScanlineSimpleImpl<FILL_NON_ZERO, 0>;
        _sweepScanlineRegionFn = _sweepScanlineRegionImpl<FILL_NON_ZERO, 0>;
        _sweepScanlineSpansFn  = _sweepScanlineSpansImpl<FILL_NON_ZERO, 0>;
      }
      else
      {
        _sweepScanlineSimpleFn = _sweepScanlineSimpleImpl<FILL_NON_ZERO, 1>;
        _sweepScanlineRegionFn = _sweepScanlineRegionImpl<FILL_NON_ZERO, 1>;
        _sweepScanlineSpansFn  = _sweepScanlineSpansImpl<FILL_NON_ZERO, 1>;
      }
      break;

    case FILL_EVEN_ODD:
      if (_alpha == 0xFF)
      {
        _sweepScanlineSimpleFn = _sweepScanlineSimpleImpl<FILL_EVEN_ODD, 0>;
        _sweepScanlineRegionFn = _sweepScanlineRegionImpl<FILL_EVEN_ODD, 0>;
        _sweepScanlineSpansFn  = _sweepScanlineSpansImpl<FILL_EVEN_ODD, 0>;
      }
      else
      {
        _sweepScanlineSimpleFn = _sweepScanlineSimpleImpl<FILL_EVEN_ODD, 1>;
        _sweepScanlineRegionFn = _sweepScanlineRegionImpl<FILL_EVEN_ODD, 1>;
        _sweepScanlineSpansFn  = _sweepScanlineSpansImpl<FILL_EVEN_ODD, 1>;
      }
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::AnalyticRasterizer - Commands]
// ============================================================================

void AnalyticRasterizer::addPath(const DoublePath& path)
{
  FOG_ASSERT(_finalized == false);

  sysuint_t i = path.getLength();
  if (!i) return;

  const uint8_t* commands = path.getCommands();
  const DoublePoint* vertices = path.getVertices();

  if (_clipping)
  {
    do {
      uint8_t cmd = commands[0];

      // CmdLineTo is first, because it's most used command.
      if (PathCmd::isLineTo(cmd))
      {
        int24x8_t x2 = RasterizerUtil::upscale(vertices->x);
        int24x8_t y2 = RasterizerUtil::upscale(vertices->y);
        uint f2 = LiangBarsky::getClippingFlags(x2, y2, _clip24x8);

        clipLine(_x1, _y1, x2, y2, _f1, f2);
        _x1 = x2;
        _y1 = y2;
        _f1 = f2;
      }
      else if (PathCmd::isMoveTo(cmd))
      {
        if (_autoClose && (_x1 != _startX1 || _y1 != _startY1))
        {
          clipLine(_x1, _y1, _startX1, _startY1, _f1, _startF1);
        }

        _x1 = _startX1 = RasterizerUtil::upscale(vertices->x);
        _y1 = _startY1 = RasterizerUtil::upscale(vertices->y);
        _f1 = _startF1 = LiangBarsky::getClippingFlags(_x1, _y1, _clip24x8);
      }
      else if (PathCmd::isClose(cmd))
      {
        if (_x1 != _startX1 || _y1 != _startY1)
        {
          clipLine(_x1, _y1, _startX1, _startY1, _f1, _startF1);
        }

        _x1 = _startX1;
        _y1 = _startY1;
        _f1 = _startF1;
      }
      else if (PathCmd::isStop(cmd))
      {
        _startX1 = 0;
        _startY1 = 0;
        _startF1 = 0;

        _x1 = 0;
        _y1 = 0;
        _f1 = 0;
      }

      commands++;
      vertices++;
    } while (--i);
  }
  else
  {
    do {
      uint8_t cmd = commands[0];

      // CmdLineTo is first, because it's most used command.
      if (PathCmd::isLineTo(cmd))
      {
        int24x8_t x2 = RasterizerUtil::upscale(vertices->x);
        int24x8_t y2 = RasterizerUtil::upscale(vertices->y);

        renderLine(_x1, _y1, x2, y2);
        _x1 = x2;
        _y1 = y2;
      }
      else if (PathCmd::isMoveTo(cmd))
      {
        if (_autoClose && (_x1 != _startX1 || _y1 != _startY1))
        {
          renderLine(_x1, _y1, _startX1, _startY1);
        }

        _x1 = _startX1 = RasterizerUtil::upscale(vertices->x);
        _y1 = _startY1 = RasterizerUtil::upscale(vertices->y);
      }
      else if (PathCmd::isClose(cmd))
      {
        if (_x1 != _startX1 || _y1 != _startY1)
        {
          renderLine(_x1, _y1, _startX1, _startY1);
        }

        _x1 = _startX1;
        _y1 = _startY1;
        _f1 = _startF1;
      }
      else if (PathCmd::isStop(cmd))
      {
        _startX1 = 0;
        _startY1 = 0;
        _startF1 = 0;

        _x1 = 0;
        _y1 = 0;
        _f1 = 0;
      }

      commands++;
      vertices++;
    } while (--i);
  }
}

void AnalyticRasterizer::closePolygon()
{
  if (_x1 != _startX1 || _y1 != _startY1)
  {
    if (_clipping)
      clipLine(_x1, _y1, _startX1, _startY1, _f1, _startF1);
    else
      renderLine(_x1, _y1, _startX1, _startY1);
  }
}

// ============================================================================
// [Fog::AnalyticRasterizer - Clipper]
// ============================================================================

void AnalyticRasterizer::clipLine(int24x8_t x1, int24x8_t y1, int24x8_t x2, int24x8_t y2, uint f1, uint f2)
{
  // Invisible by Y.
  if ((f1 & 10) == (f2 & 10) && (f1 & 10) != 0) return;

  int24x8_t y3, y4;
  uint f3, f4;

  switch (((f1 & 5) << 1) | (f2 & 5))
  {
    // Visible by X
    case 0:
      clipLineY(x1, y1, x2, y2, f1, f2);
      break;

    // x2 > clip.x2
    case 1:
      y3 = y1 + RasterizerUtil::muldiv(_clip24x8.x2 - x1, y2 - y1, x2 - x1);
      f3 = LiangBarsky::getClippingFlagsY(y3, _clip24x8);
      clipLineY(x1, y1, _clip24x8.x2, y3, f1, f3);
      clipLineY(_clip24x8.x2, y3, _clip24x8.x2, y2, f3, f2);
      break;

    // x1 > clip.x2
    case 2:
      y3 = y1 + RasterizerUtil::muldiv(_clip24x8.x2 - x1, y2 - y1, x2 - x1);
      f3 = LiangBarsky::getClippingFlagsY(y3, _clip24x8);
      clipLineY(_clip24x8.x2, y1, _clip24x8.x2, y3, f1, f3);
      clipLineY(_clip24x8.x2, y3, x2, y2, f3, f2);
      break;

    // x1 > clip.x2 && x2 > clip.x2
    case 3:
      clipLineY(_clip24x8.x2, y1, _clip24x8.x2, y2, f1, f2);
      break;

    // x2 < clipX1
    case 4:
      y3 = y1 + RasterizerUtil::muldiv(_clip24x8.x1 - x1, y2 - y1, x2 - x1);
      f3 = LiangBarsky::getClippingFlagsY(y3, _clip24x8);
      clipLineY(x1, y1, _clip24x8.x1, y3, f1, f3);
      clipLineY(_clip24x8.x1, y3, _clip24x8.x1, y2, f3, f2);
      break;

    // x1 > clip.x2 && x2 < clip.x1
    case 6:
      y3 = y1 + RasterizerUtil::muldiv(_clip24x8.x2 - x1, y2 - y1, x2 - x1);
      y4 = y1 + RasterizerUtil::muldiv(_clip24x8.x1 - x1, y2 - y1, x2 - x1);
      f3 = LiangBarsky::getClippingFlagsY(y3, _clip24x8);
      f4 = LiangBarsky::getClippingFlagsY(y4, _clip24x8);
      clipLineY(_clip24x8.x2, y1, _clip24x8.x2, y3, f1, f3);
      clipLineY(_clip24x8.x2, y3, _clip24x8.x1, y4, f3, f4);
      clipLineY(_clip24x8.x1, y4, _clip24x8.x1, y2, f4, f2);
      break;

    // x1 < clip.x1
    case 8:
      y3 = y1 + RasterizerUtil::muldiv(_clip24x8.x1 - x1, y2 - y1, x2 - x1);
      f3 = LiangBarsky::getClippingFlagsY(y3, _clip24x8);
      clipLineY(_clip24x8.x1, y1, _clip24x8.x1, y3, f1, f3);
      clipLineY(_clip24x8.x1, y3, x2, y2, f3, f2);
      break;

    // x1 < clip.x1 && x2 > clip.x2
    case 9:
      y3 = y1 + RasterizerUtil::muldiv(_clip24x8.x1 - x1, y2 - y1, x2 - x1);
      y4 = y1 + RasterizerUtil::muldiv(_clip24x8.x2 - x1, y2 - y1, x2 - x1);
      f3 = LiangBarsky::getClippingFlagsY(y3, _clip24x8);
      f4 = LiangBarsky::getClippingFlagsY(y4, _clip24x8);
      clipLineY(_clip24x8.x1, y1, _clip24x8.x1, y3, f1, f3);
      clipLineY(_clip24x8.x1, y3, _clip24x8.x2, y4, f3, f4);
      clipLineY(_clip24x8.x2, y4, _clip24x8.x2, y2, f4, f2);
      break;

    // x1 < clip.x1 && x2 < clip.x1
    case 12:
      clipLineY(_clip24x8.x1, y1, _clip24x8.x1, y2, f1, f2);
      break;
  }
}

FOG_INLINE void AnalyticRasterizer::clipLineY(int24x8_t x1, int24x8_t y1, int24x8_t x2, int24x8_t y2, uint f1, uint f2)
{
  f1 &= 10;
  f2 &= 10;

  if ((f1 | f2) == 0)
  {
    // Fully visible.
    renderLine(x1, y1, x2, y2); 
  }
  else
  {
    // Invisible by Y.
    if (f1 == f2) return;

    int24x8_t tx1 = x1;
    int24x8_t ty1 = y1;
    int24x8_t tx2 = x2;
    int24x8_t ty2 = y2;

    if (f1 & 8) // y1 < clip.y1
    {
      tx1 = x1 + RasterizerUtil::muldiv(_clip24x8.y1 - y1, x2 - x1, y2 - y1);
      ty1 = _clip24x8.y1;
    }

    if (f1 & 2) // y1 > clip.y2
    {
      tx1 = x1 + RasterizerUtil::muldiv(_clip24x8.y2 - y1, x2 - x1, y2 - y1);
      ty1 = _clip24x8.y2;
    }

    if (f2 & 8) // y2 < clip.y1
    {
      tx2 = x1 + RasterizerUtil::muldiv(_clip24x8.y1 - y1, x2 - x1, y2 - y1);
      ty2 = _clip24x8.y1;
    }

    if (f2 & 2) // y2 > clip.y2
    {
      tx2 = x1 + RasterizerUtil::muldiv(_clip24x8.y2 - y1, x2 - x1, y2 - y1);
      ty2 = _clip24x8.y2;
    }

    renderLine(tx1, ty1, tx2, ty2); 
  }
}

// ============================================================================
// [Fog::AnalyticRasterizer - Render]
// ============================================================================

void AnalyticRasterizer::renderLine(int24x8_t x1, int24x8_t y1, int24x8_t x2, int24x8_t y2)
{
  enum DXLimitEnum { DXLimit = 16384 << POLY_SUBPIXEL_SHIFT };

  int dx = x2 - x1;

  if (FOG_UNLIKELY((dx >= DXLimit) | (dx <= -DXLimit)))
  {
    int cx = (x1 + x2) >> 1;
    int cy = (y1 + y2) >> 1;

    AnalyticRasterizer::renderLine(x1, y1, cx, cy);
    AnalyticRasterizer::renderLine(cx, cy, x2, y2);
    return;
  }

  int dy = y2 - y1;

  int ex1 = x1 >> POLY_SUBPIXEL_SHIFT;
  int ex2 = x2 >> POLY_SUBPIXEL_SHIFT;
  int ey1 = y1 >> POLY_SUBPIXEL_SHIFT;
  int ey2 = y2 >> POLY_SUBPIXEL_SHIFT;
  int fy1 = y1 & POLY_SUBPIXEL_MASK;
  int fy2 = y2 & POLY_SUBPIXEL_MASK;

  int x_from, x_to;
  int p, rem, mod, lift, delta, first, incr;

  if (ex1 < _cellsBounds.x1) _cellsBounds.x1 = ex1;
  if (ex1 > _cellsBounds.x2) _cellsBounds.x2 = ex1;
  if (ey1 < _cellsBounds.y1) _cellsBounds.y1 = ey1;
  if (ey1 > _cellsBounds.y2) _cellsBounds.y2 = ey1;
  if (ex2 < _cellsBounds.x1) _cellsBounds.x1 = ex2;
  if (ex2 > _cellsBounds.x2) _cellsBounds.x2 = ex2;
  if (ey2 < _cellsBounds.y1) _cellsBounds.y1 = ey2;
  if (ey2 > _cellsBounds.y2) _cellsBounds.y2 = ey2;

  // Prepare the first cell (_invalidCell can be set also on out of memory).
  if (FOG_UNLIKELY(_curCell == &_invalidCell))
  {
    if (_error != ERR_OK) return;

    nextCellBuffer();
    _curCell->setCell(ex1, ey1, 0, 0);
  }
  // If current cell position is different than [ex1, ey1] we have to create
  // new cell.
  else
  {
    setCurCell(ex1, ey1);
  }

  FOG_ASSERT(_curCell->x == ex1 && _curCell->y == ey1);

  // Everything is on a single hline.
  if (ey1 == ey2)
  {
    renderHLine(ey1, x1, fy1, x2, fy2);
    return;
  }

  // Vertical line - we have to calculate start and end cells, and then - the
  // common values of the area and coverage for all cells of the line. We know
  // exactly there's only one cell, so, we don't have to call renderHLine().
  incr = 1;
  if (dx == 0)
  {
    int two_fx = (x1 - (ex1 << POLY_SUBPIXEL_SHIFT)) << 1;
    int area;

    first = POLY_SUBPIXEL_SCALE;
    if (dy < 0) { first = 0; incr  = -1; }

    delta = first - fy1;

    _curCell->addCovers(delta, two_fx * delta);
    addCurCell();
    ey1 += incr;

    delta = first + first - POLY_SUBPIXEL_SCALE;
    area = two_fx * delta;

    while (ey1 != ey2)
    {
      _curCell->setCell(ex1, ey1, delta, area);
      addCurCell_Always();
      ey1 += incr;
    }

    delta = first + fy2 - POLY_SUBPIXEL_SCALE;
    _curCell->setCell(ex1, ey1, delta, two_fx * delta);
    return;
  }

  // Ok, we have to render several hlines.
  p = (POLY_SUBPIXEL_SCALE - fy1) * dx;
  first = POLY_SUBPIXEL_SCALE;

  if (dy < 0)
  {
    p     = fy1 * dx;
    first = 0;
    incr  = -1;
    dy    = -dy;
  }

  delta = p / dy;
  mod   = p % dy;

  if (mod < 0) { delta--; mod += dy; }

  x_from = x1 + delta;
  renderHLine(ey1, x1, fy1, x_from, first);

  ey1 += incr;
  addCurCell();
  _curCell->setCell(x_from >> POLY_SUBPIXEL_SHIFT, ey1, 0, 0);

  if (ey1 != ey2)
  {
    p     = POLY_SUBPIXEL_SCALE * dx;
    lift  = p / dy;
    rem   = p % dy;

    if (rem < 0) { lift--; rem += dy; }
    mod -= dy;

    while (ey1 != ey2)
    {
      delta = lift;
      mod  += rem;
      if (mod >= 0) { mod -= dy; delta++; }

      x_to = x_from + delta;
      renderHLine(ey1, x_from, POLY_SUBPIXEL_SCALE - first, x_to, first);
      x_from = x_to;

      ey1 += incr;

      addCurCell();
      _curCell->setCell(x_from >> POLY_SUBPIXEL_SHIFT, ey1, 0, 0);
    }
  }

  renderHLine(ey1, x_from, POLY_SUBPIXEL_SCALE - first, x2, fy2);
}

FOG_INLINE void AnalyticRasterizer::renderHLine(int ey, int24x8_t x1, int24x8_t y1, int24x8_t x2, int24x8_t y2)
{
  int ex1;
  int ex2;
  int fx1;
  int fx2;

  int delta, p, first, dx;
  int incr, lift, mod, rem;

  ex1 = x1 >> POLY_SUBPIXEL_SHIFT;
  ex2 = x2 >> POLY_SUBPIXEL_SHIFT;

  FOG_ASSERT(_curCell->x == ex1 && _curCell->y == ey);

  // Trivial case. Happens often.
  if (y1 == y2)
  {
    setCurCell(ex2, ey);
    return;
  }

  fx1 = x1 & POLY_SUBPIXEL_MASK;
  fx2 = x2 & POLY_SUBPIXEL_MASK;

  // Everything is located in a single cell. That is easy!
  if (ex1 == ex2)
  {
    delta = y2 - y1;
    _curCell->addCovers(delta, (fx1 + fx2) * delta);
    return;
  }

  // Ok, we'll have to render a run of adjacent cells on the same hline...
  p     = (POLY_SUBPIXEL_SCALE - fx1) * (y2 - y1);
  first = POLY_SUBPIXEL_SCALE;
  incr  = 1;

  dx = x2 - x1;

  if (dx < 0)
  {
    p     = fx1 * (y2 - y1);
    first = 0;
    incr  = -1;
    dx    = -dx;
  }

  delta = p / dx;
  mod   = p % dx;

  if (mod < 0) { mod += dx; delta--; }

  _curCell->addCovers(delta, (fx1 + first) * delta);
  addCurCell();

  ex1 += incr;
  y1 += delta;

  if (ex1 != ex2)
  {
    p     = POLY_SUBPIXEL_SCALE * (y2 - y1 + delta);
    lift  = p / dx;
    rem   = p % dx;

    if (rem < 0) { lift--; rem += dx; }
    mod -= dx;

    do {
      delta = lift;
      mod  += rem;
      if (mod >= 0) { mod -= dx; delta++; }

      if (delta)
      {
        _curCell->setCell(ex1, ey, delta, POLY_SUBPIXEL_SCALE * delta);
        addCurCell_Always();
      }

      y1  += delta;
      ex1 += incr;
    } while (ex1 != ex2);
  }

  delta = y2 - y1;
  _curCell->setCell(ex1, ey, delta, (fx2 + POLY_SUBPIXEL_SCALE - first) * delta);
}

FOG_INLINE void AnalyticRasterizer::addCurCell()
{
  if (!_curCell->hasCovers()) return;
  if (FOG_UNLIKELY(++_curCell == _endCell)) nextCellBuffer();
}

FOG_INLINE void AnalyticRasterizer::addCurCell_Always()
{
  FOG_ASSERT(_curCell->hasCovers());
  if (FOG_UNLIKELY(++_curCell == _endCell)) nextCellBuffer();
}

FOG_INLINE void AnalyticRasterizer::setCurCell(int x, int y)
{
  if (_curCell->hasPosition(x, y)) return;

  addCurCell();
  _curCell->setCell(x, y, 0, 0);
}

bool AnalyticRasterizer::nextCellBuffer()
{
  // If there is no buffer we quietly do nothing.
  if (FOG_UNLIKELY(!_bufferCurrent)) goto error;

  // If we are starting filling we just return true.
  if (_curCell == &_invalidCell) goto init;

  // Finalize current buffer.
  _bufferCurrent->count = _bufferCurrent->capacity;
  _cellsCount += _bufferCurrent->count;

  // Try to get next buffer. First try link in current buffer, otherwise use
  // rasterizer's pool.
  if (_bufferCurrent->next)
  {
    _bufferCurrent = _bufferCurrent->next;
    goto init;
  }

  // Next buffer not found, try to get cell buffer from pool (getCellXYBuffer
  // can also allocate new buffer if the pool is empty).
  _bufferCurrent = Rasterizer::getCellXYBuffer();
  if (_bufferCurrent)
  {
    // Link
    _bufferLast->next = _bufferCurrent;
    _bufferCurrent->prev = _bufferLast;
    _bufferLast = _bufferCurrent;
    goto init;
  }

error:
  // Initialize cell pointers to _invalidCell.
  _curCell = &_invalidCell;
  _endCell = _curCell + 1;

  if (_error != ERR_RT_OUT_OF_MEMORY) setError(ERR_RT_OUT_OF_MEMORY);
  return false;

init:
  _curCell = _bufferCurrent->cells;
  _endCell = _curCell + _bufferCurrent->capacity;
  return true;
}

bool AnalyticRasterizer::finalizeCellBuffer()
{
  // If there is no buffer we quietly do nothing.
  if (!_bufferCurrent) return false;

  _bufferCurrent->count = (uint32_t)(_curCell - _bufferCurrent->cells);
  _cellsCount += _bufferCurrent->count;

  // Clear cell pointers (after finalize the access to cells is forbidden).
  _curCell = &_invalidCell;
  _endCell = _curCell + 1;

  return true;
}

void AnalyticRasterizer::freeXYCellBuffers(bool all)
{
  if (_bufferFirst != NULL)
  {
    // Release all cell buffers except the first one.
    CellXYBuffer* candidate = (all) ? _bufferFirst : _bufferFirst->next;
    if (!candidate) return;

    Rasterizer::releaseCellXYBuffer(candidate);
    if (all)
    {
      _bufferFirst = NULL;
      _bufferLast = NULL;
    }
    else
    {
      // First cell is now last cell.
      _bufferLast = _bufferFirst;

      // Clear links.
      _bufferFirst->next = NULL;
      _bufferFirst->prev = NULL;
    }
  }
}

// ============================================================================
// [Fog::AnalyticRasterizer - Finalize]
// ============================================================================

template <typename T>
static FOG_INLINE void swapCells(T* FOG_RESTRICT a, T* FOG_RESTRICT b)
{
  T temp;

  temp.set(*a);
  a->set(*b);
  b->set(temp);
}

template<class Cell>
static FOG_INLINE void qsortCells(Cell* start, sysuint_t num)
{
  Cell*  stack[80];
  Cell** top; 
  Cell*  limit;
  Cell*  base;

  limit = start + num;
  base  = start;
  top   = stack;

  for (;;)
  {
    sysuint_t len = sysuint_t(limit - base);

    Cell* i;
    Cell* j;
    Cell* pivot;

    if (len > RASTERIZER_QSORT_THRESHOLD)
    {
      // We use base + len/2 as the pivot.
      pivot = base + len / 2;
      swapCells(base, pivot);

      i = base + 1;
      j = limit - 1;

      // Now ensure that *i <= *base <= *j .
      if (j->x < i->x) swapCells(i, j);
      if (base->x < i->x) swapCells(base, i);
      if (j->x < base->x) swapCells(base, j);

      for (;;)
      {
        int x = base->x;
        do { i++; } while (i->x < x);
        do { j--; } while (x < j->x);

        if (i > j) break;
        swapCells(i, j);
      }

      swapCells(base, j);

      // Now, push the largest sub-array.
      if (j - base > limit - i)
      {
        top[0] = base;
        top[1] = j;
        base   = i;
      }
      else
      {
        top[0] = i;
        top[1] = limit;
        limit  = j;
      }
      top += 2;
    }
    else
    {
      // The sub-array is small, perform insertion sort.
      j = base;
      i = j + 1;

      for (; i < limit; j = i, i++)
      {
        // Optimization experiment for X64, not successful:
        //
        // int j1X = j[1].x;
        // uint64_t j1T = ((uint64_t*)&j[1].cover)[0];
        //
        // for (;;)
        // {
        //   int j0X = j[0].x;
        //   if (j0X < j1X) break;
        //
        //   j[1].x = j0X;
        //   ((uint64_t*)&j[1].cover)[0] = ((uint64_t*)&j[0].cover)[0];
        //
        //   j[0].x = j1X;
        //   ((uint64_t*)&j[0].cover)[0] = j1T;
        //
        //   if (j-- == base) break;
        // }
        for (; j[0].x >= j[1].x; j--)
        {
          swapCells(j + 1, j);
          if (j == base) break;
        }
      }

      if (top > stack)
      {
        top  -= 2;
        base  = top[0];
        limit = top[1];
      }
      else
      {
        break;
      }
    }
  }
}

void AnalyticRasterizer::finalize()
{
  // Perform sort only the first time.
  if (_finalized) return;

  if (_autoClose) closePolygon();
  if (_curCell == &_invalidCell) return;

  if (_curCell->hasCovers()) _curCell++;
  if (!finalizeCellBuffer() || !_cellsCount) return;

  // DBG: Check to see if min/max works well.
  // for (uint nc = 0; nc < m_numCells; nc++)
  // {
  //   cell_type* cell = m_cells[nc >> cell_block_shift] + (nc & cell_block_mask);
  //   if (cell->x < _cellsBounds.x1 || 
  //       cell->y < _cellsBounds.y1 || 
  //       cell->x > _cellsBounds.x2 || 
  //       cell->y > _cellsBounds.y2)
  //   {
  //     cell = cell; // Breakpoint here
  //   }
  // }

  // Normalize bounding box to our standard, x2/y2 coordinates are outside.
  _cellsBounds.x2++;
  _cellsBounds.y2++;

  sysuint_t rows = (sysuint_t)(_cellsBounds.y2 - _cellsBounds.y1);

  if (_cellsCapacity < _cellsCount)
  {
    // Reserve a bit more if initial value is too small.
    sysuint_t cap = Math::max(_cellsCount, (sysuint_t)256);

    if (_cellsSorted) Memory::free(_cellsSorted);
    _cellsSorted = (CellX*)Memory::alloc(sizeof(CellX) * cap);
    if (_cellsSorted) _cellsCapacity = cap;
  }

  if (_rowsCapacity < rows)
  {
    // Reserve a bit more if initial value is too small.
    sysuint_t cap = Math::max(rows, (sysuint_t)256);

    if (_rowsInfo) Memory::free(_rowsInfo);
    _rowsInfo = (RowInfo*)Memory::alloc(sizeof(RowInfo) * cap);
    if (_rowsInfo) _rowsCapacity = cap;
  }

  // Report error if something failed.
  if (!_cellsSorted || !_rowsInfo)
  {
    setError(ERR_RT_OUT_OF_MEMORY);
    return;
  }

  // Work variables.
  CellXYBuffer* buf;
  sysuint_t i;
  CellXY* cell;

  // Create the Y-histogram (count the numbers of cells for each Y).

  // If we are here _bufferFirst and _bufferCurrent must exist.
  FOG_ASSERT(_bufferFirst);
  FOG_ASSERT(_bufferCurrent);

  // Zero all values in lookup table (and histogram) - this is initial state.
  Memory::zero(_rowsInfo, sizeof(RowInfo) * rows);

  buf = _bufferFirst;
  do {
    cell = buf->cells;
    for (i = buf->count; i; i--, cell++) _rowsInfo[cell->y - _cellsBounds.y1].index++;

    // Stop if this is last used cells buffer.
    if (buf == _bufferCurrent) break;
  } while ((buf = buf->next));

  // Convert the Y-histogram into the array of starting indexes.
  {
    uint32_t start = 0;
    for (i = 0; i < rows; i++)
    {
      uint32_t v = _rowsInfo[i].index;
      _rowsInfo[i].index = start;
      start += v;
    }
  }

  // Fill the cell pointer array sorted by Y.
  buf = _bufferFirst;
  do {
    cell = buf->cells;
    for (i = buf->count; i; i--, cell++)
    {
      RowInfo& ri =_rowsInfo[cell->y - _cellsBounds.y1];
      _cellsSorted[ri.index + ri.count++].set(*cell);
    }

    // Stop if this is last used cells buffer.
    if (buf == _bufferCurrent) break;
  } while ((buf = buf->next));

  // Finally arrange the X-arrays.
  for (i = 0; i < rows; i++)
  {
    const RowInfo& ri = _rowsInfo[i];
    if (ri.count > 1) qsortCells(_cellsSorted + ri.index, ri.count);
  }

  // Free unused cell buffers.
  freeXYCellBuffers(false);

  // Mark rasterizer as sorted.
  _finalized = true;
}

// ============================================================================
// [Fog::AnalyticRasterizer - Sweep]
// ============================================================================

#if defined(FOG_DEBUG_RASTERIZER)
static void dumpSpans(int y, const Span8* span)
{
  ByteArray b;
  b.appendFormat("Y=%d - ", y);

  while (span)
  {
    if (span->isCMask())
    {
      b.appendFormat("C[%d %d]%0.2X", span->x1, span->x2, span->getCMask());
    }
    else
    {
      b.appendFormat("M[%d %d]", span->x1, span->x2);
      for (int x = 0; x < span->getLength(); x++)
        b.appendFormat("%0.2X", span->getVMask()[x]);
    }
    b.append(' ');
    span = span->getNext();
  }

  printf("%s\n", b.getData());
}
#endif // FOG_DEBUG_RASTERIZER

template<int _RULE, int _USE_ALPHA>
FOG_INLINE uint AnalyticRasterizer::_calculateAlpha(int area) const
{
  int cover = area >> (POLY_SUBPIXEL_SHIFT*2 + 1 - AA_SHIFT);
  if (cover < 0) cover = -cover;

  if (_RULE == FILL_NON_ZERO)
  {
    if (cover > AA_MASK) cover = AA_MASK;
  }
  else
  {
    cover &= AA_MASK_2;
    if (cover > AA_SCALE) cover = AA_SCALE_2 - cover;
    if (cover > AA_MASK) cover = AA_MASK;
  }

  if (_USE_ALPHA) cover = ByteUtil::scalar_muldiv255(cover, _alpha);
  //return _gamma[cover];
  return cover;
}

template<int _RULE, int _USE_ALPHA>
Span8* AnalyticRasterizer::_sweepScanlineSimpleImpl(
  Rasterizer* _rasterizer, Scanline8& scanline, int y)
{
  AnalyticRasterizer* rasterizer =
    reinterpret_cast<AnalyticRasterizer*>(_rasterizer);

  FOG_ASSERT(rasterizer->_finalized);
  if (y >= rasterizer->_cellsBounds.y2) return NULL;

  const RowInfo& ri = rasterizer->_rowsInfo[y - rasterizer->_cellsBounds.y1];

  uint numCells = ri.count;
  if (!numCells) return NULL;

  const CellX* cellCur = &rasterizer->_cellsSorted[ri.index];

  int x;
  int nextX = cellCur->x;
  int area;
  int cover = 0;

  if (scanline.newScanline(rasterizer->_cellsBounds.x1, rasterizer->_cellsBounds.x2) != ERR_OK)
    return NULL;

  for (;;)
  {
    x      = nextX;
    area   = cellCur->area;
    cover += cellCur->cover;

    while (--numCells)
    {
      cellCur++;
      if ((nextX = cellCur->x) != x) break;
      area  += cellCur->area;
      cover += cellCur->cover;
    }

    int coversh = cover << (POLY_SUBPIXEL_SHIFT + 1);

    // There are two possibilities:
    //
    // 1. Next cellCur is in position x + 1. This means that we are rendering 
    //    shape and the line direction in current scanline is horizontal.
    //    Horizontal direction means that the generated span-mask is larger
    //    than one pixel.
    if (x + 1 == nextX)
    {
      uint alpha;
      // Skip this cellCur if resulting mask is zero.
      if ((alpha = rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh - area)) == 0) continue;

      // Okay, it seems that we will now generate some masks, embedded to one
      // span instance.
      scanline.newVSpanAlpha(x);
      scanline.addValueAlpha((uint8_t)alpha);

      for (;;)
      {
        x      = nextX;
        area   = cellCur->area;
        cover += cellCur->cover;

        while (--numCells)
        {
          cellCur++;
          if ((nextX = cellCur->x) != x) break;
          area  += cellCur->area;
          cover += cellCur->cover;
        }

        coversh = cover << (POLY_SUBPIXEL_SHIFT + 1);
        alpha = rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh - area);

        if (++x == nextX)
        {
          scanline.addValueAlpha(alpha);
          continue;
        }
        else
        {
          break;
        }
      }

      if (area != 0)
      {
        if (alpha != 0)
        {
          scanline.addValueAlpha(alpha);
          scanline.endVSpanAlpha(x);
        }
        else
        {
          scanline.endVSpanAlpha(x - 1);
        }
      }
      else
      {
        scanline.endVSpanAlpha(--x);
      }
      if (numCells == 0) break;
    }

    // 2. Next cellCur is not near or it's the last one. We need to add mask if
    //    alpha is larger than zero.
    else
    {
      uint alpha;
      if (area != 0)
      {
        if ((alpha = rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh - area)) != 0)
        {
          scanline.newVSpanAlpha(x);
          scanline.addValueAlpha(alpha);
          scanline.endVSpanAlpha(x + 1);
        }
        x++;
      }
      if (numCells == 0) break;
    }

    {
      uint alpha = rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh);
      if (alpha) scanline.addCSpanOrMergeVSpan(x, nextX, alpha);
    }
  }

  if (scanline.endScanline() != ERR_OK) return NULL;
#if defined(FOG_DEBUG_RASTERIZER)
  dumpSpans(y, scanline.getSpans());
#endif // FOG_DEBUG_RASTERIZER
  return scanline.getSpans();
}

template<int _RULE, int _USE_ALPHA>
Span8* AnalyticRasterizer::_sweepScanlineRegionImpl(
  Rasterizer* _rasterizer, Scanline8& scanline, int y,
  const IntBox* clipBoxes, sysuint_t count)
{
  AnalyticRasterizer* rasterizer =
    reinterpret_cast<AnalyticRasterizer*>(_rasterizer);

  FOG_ASSERT(rasterizer->_finalized);
  if (y >= rasterizer->_cellsBounds.y2) return NULL;

  const RowInfo& ri = rasterizer->_rowsInfo[y - rasterizer->_cellsBounds.y1];

  uint numCells = ri.count;
  if (!numCells) return NULL;

  const CellX* cellCur = &rasterizer->_cellsSorted[ri.index];

  // Clipping.
  const IntBox* clipCur = clipBoxes;
  const IntBox* clipEnd = clipBoxes + count;
  if (FOG_UNLIKELY(clipCur == clipEnd)) return NULL;

  if (scanline.newScanline(rasterizer->_cellsBounds.x1, rasterizer->_cellsBounds.x2) != ERR_OK)
    return NULL;

  int x;
  int nextX = cellCur->x;
  int area;
  int cover = 0;
  int coversh;

  // Current clip box start / end point (not part of clip span).
  int clipX1;
  int clipX2 = clipCur->x2;

  // Advance clip (discard clip-boxes that can't intersect).
  while (clipX2 <= nextX)
  {
advanceClip:
    if (++clipCur == clipEnd) goto end;
    clipX2 = clipCur->x2;
  }

  clipX1 = clipCur->x1;
  FOG_ASSERT(nextX < clipX2);

  for (;;)
  {
    x      = nextX;
    area   = cellCur->area;
    cover += cellCur->cover;
    FOG_ASSERT(x < clipX2);

    while (--numCells)
    {
      cellCur++;
      if ((nextX = cellCur->x) != x) break;
      area  += cellCur->area;
      cover += cellCur->cover;
    }

    coversh = cover << (POLY_SUBPIXEL_SHIFT + 1);

    // There are two possibilities:
    //
    // 1. Next cellCur is in position x + 1. This means that we are rendering 
    //    shape and the line direction in current scanline is horizontal.
    //    Horizontal direction means that the generated span-mask is larger
    //    than one pixel.
    if (x + 1 == nextX)
    {
      if (x < clipX1) continue;

      uint alpha;
      // Skip this cellCur if resulting mask is zero.
      if ((alpha = rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh - area)) == 0) continue;

      // Okay, it seems that we will now generate some masks, embedded to one
      // span instance.
      scanline.newVSpanAlpha(x);
      scanline.addValueAlpha(alpha);

      if (clipX2 <= nextX)
      {
        scanline.endVSpanAlpha(nextX);
        goto advanceClip;
      }

      for (;;)
      {
        x      = nextX;
        area   = cellCur->area;
        cover += cellCur->cover;

        while (--numCells)
        {
          cellCur++;
          if ((nextX = cellCur->x) != x) break;
          area  += cellCur->area;
          cover += cellCur->cover;
        }

        coversh = cover << (POLY_SUBPIXEL_SHIFT + 1);
        alpha = rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh - area);

        if (++x == nextX)
        {
          scanline.addValueAlpha(alpha);
          if (clipX2 <= nextX)
          {
            scanline.endVSpanAlpha(x);
            goto advanceClip;
          }
          continue;
        }
        else
        {
          break;
        }
      }

      if (area != 0)
      {
        if (alpha != 0)
        {
          scanline.addValueAlpha(alpha);
          scanline.endVSpanAlpha(x);
        }
        else
        {
          scanline.endVSpanAlpha(x - 1);
        }
      }
      else
      {
        scanline.endVSpanAlpha(--x);
      }
      if (numCells == 0) break;
    }

    // 2. Next cellCur is not near or it's the last one. We need to add mask if
    //    alpha is larger than zero.
    else
    {
      uint alpha;
      if (area != 0)
      {
        if (x >= clipX1 && (alpha = rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh - area)) != 0)
        {
          scanline.newVSpanAlpha(x);
          scanline.addValueAlpha(alpha);
          scanline.endVSpanAlpha(x + 1);
        }
        x++;
      }
      if (numCells == 0) break;
    }

    {
      if (x >= clipX2)
      {
        if (++clipCur == clipEnd) goto end;
        clipX1 = clipCur->x1;
        clipX2 = clipCur->x2;
        FOG_ASSERT(x < clipX2);
      }

      uint alpha = rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh);
      if (alpha && nextX > clipX1)
      {
        if (x < clipX1) x = clipX1;

        if (x < clipX2)
        {
          FOG_ASSERT(x < clipX2);
          int toX = Math::min<int>(nextX, clipX2);
          scanline.addCSpanOrMergeVSpan(x, toX, alpha);
          x = toX;
        }

        if (nextX > clipX2)
        {
          for (;;)
          {
            if (++clipCur == clipEnd) goto end;
            clipX1 = clipCur->x1;
            clipX2 = clipCur->x2;
            if (nextX > clipX1)
            {
              scanline.addCSpan(clipX1, Math::min<int>(nextX, clipX2), alpha);
              if (nextX >= clipX2) continue;
            }
            break;
          }
        }
      }

      if (nextX >= clipX2) goto advanceClip;
    }
  }

end:
  if (scanline.endScanline() != ERR_OK) return NULL;
#if defined(FOG_DEBUG_RASTERIZER)
  dumpSpans(y, scanline.getSpans());
#endif // FOG_DEBUG_RASTERIZER
  return scanline.getSpans();
}

template<int _RULE, int _USE_ALPHA>
Span8* AnalyticRasterizer::_sweepScanlineSpansImpl(
  Rasterizer* _rasterizer, Scanline8& scanline, int y,
  const Span8* clipSpans)
{
  AnalyticRasterizer* rasterizer =
    reinterpret_cast<AnalyticRasterizer*>(_rasterizer);

  FOG_ASSERT(rasterizer->_finalized);
  if (y >= rasterizer->_cellsBounds.y2) return NULL;

  const RowInfo& ri = rasterizer->_rowsInfo[y - rasterizer->_cellsBounds.y1];

  uint numCells = ri.count;
  if (!numCells) return NULL;

  const CellX* cellCur = &rasterizer->_cellsSorted[ri.index];

  // Clipping.
  const Span8* clipCur = clipSpans;
  if (FOG_UNLIKELY(clipCur == NULL)) return NULL;

  if (scanline.newScanline(rasterizer->_cellsBounds.x1, rasterizer->_cellsBounds.x2) != ERR_OK)
    return NULL;

  int x;
  int nextX = cellCur->x;
  int area;
  int cover = 0;
  int coversh;

  // Current clip box start / end point (not part of clip span).
  int clipX1;
  int clipX2 = clipCur->getX2();
  const uint8_t* clipMask;

  // Advance clip (discard clip-boxes that can't intersect).
  while (clipX2 <= nextX)
  {
advanceClip:
    if ((clipCur = clipCur->getNext()) == NULL) goto end;
    clipX2 = clipCur->getX2();
  }

#define CLIP_SPAN_CHANGED() \
  { \
    clipX1 = clipCur->getX1(); \
    FOG_ASSERT(clipX1 < clipX2); \
    \
    clipMask = clipCur->getMaskPtr(); \
    if (Span8::isPtrVMask(clipMask)) clipMask -= clipX1; \
  }

  CLIP_SPAN_CHANGED()
  FOG_ASSERT(nextX < clipX2);

  for (;;)
  {
    x      = nextX;
    area   = cellCur->area;
    cover += cellCur->cover;
    FOG_ASSERT(x < clipX2);

    while (--numCells)
    {
      cellCur++;
      if ((nextX = cellCur->x) != x) break;
      area  += cellCur->area;
      cover += cellCur->cover;
    }

    coversh = cover << (POLY_SUBPIXEL_SHIFT + 1);

    // There are two possibilities:
    //
    // 1. Next cellCur is in position x + 1. This means that we are rendering 
    //    shape and the line direction in current scanline is horizontal.
    //    Horizontal direction means that the generated span-mask is larger
    //    than one pixel.
    if (x + 1 == nextX)
    {
      if (x < clipX1) continue;

      uint alpha;
      // Skip this cellCur if resulting mask is zero.
      if ((alpha = rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh - area)) == 0)
      {
        if (clipX2 <= nextX) goto advanceClip;
        continue;
      }

      // Okay, it seems that we will now generate some masks, embedded to one
      // span instance.
      if (Span8::isPtrCMask(clipMask))
      {
        uint32_t m = Span8::ptrToCMask(clipMask);
        if ((alpha = ByteUtil::scalar_muldiv255(alpha, m)) == 0)
        {
          if (clipX2 <= nextX) goto advanceClip;
          continue;
        }

        scanline.newVSpanAlpha(x);
        scanline.addValueAlpha(alpha);

        if (clipX2 <= nextX)
        {
          scanline.endVSpanAlpha(nextX);
          goto advanceClip;
        }

        for (;;)
        {
          x      = nextX;
          area   = cellCur->area;
          cover += cellCur->cover;

          while (--numCells)
          {
            cellCur++;
            if ((nextX = cellCur->x) != x) break;
            area  += cellCur->area;
            cover += cellCur->cover;
          }

          coversh = cover << (POLY_SUBPIXEL_SHIFT + 1);
          alpha = ByteUtil::scalar_muldiv255(
            rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh - area), m);

          FOG_ASSERT(x >= clipX1 && x < clipX2);
          if (++x == nextX)
          {
            scanline.addValueAlpha(alpha);
            if (clipX2 <= nextX)
            {
              scanline.endVSpanAlpha(x);
              goto advanceClip;
            }
            continue;
          }
          else
          {
            break;
          }
        }
      }
      else
      {
        if ((alpha = ByteUtil::scalar_muldiv255(alpha, clipMask[x])) == 0)
        {
          if (clipX2 <= nextX) goto advanceClip;
          continue;
        }

        scanline.newVSpanAlpha(x);
        scanline.addValueAlpha(alpha);

        if (clipX2 <= nextX)
        {
          scanline.endVSpanAlpha(nextX);
          goto advanceClip;
        }

        for (;;)
        {
          x      = nextX;
          area   = cellCur->area;
          cover += cellCur->cover;

          while (--numCells)
          {
            cellCur++;
            if ((nextX = cellCur->x) != x) break;
            area  += cellCur->area;
            cover += cellCur->cover;
          }

          FOG_ASSERT(x >= clipX1 && x < clipX2);
          
          coversh = cover << (POLY_SUBPIXEL_SHIFT + 1);
          alpha = ByteUtil::scalar_muldiv255(
            rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh - area), clipMask[x]);

          if (++x == nextX)
          {
            scanline.addValueAlpha(alpha);
            if (clipX2 <= nextX)
            {
              scanline.endVSpanAlpha(x);
              goto advanceClip;
            }
            continue;
          }
          else
          {
            break;
          }
        }
      }

      if (area != 0)
      {
        if (alpha != 0)
        {
          scanline.addValueAlpha(alpha);
          scanline.endVSpanAlpha(x);
        }
        else
        {
          scanline.endVSpanAlpha(x - 1);
        }
      }
      else
      {
        scanline.endVSpanAlpha(--x);
      }
      if (numCells == 0) break;
    }

    // 2. Next cellCur is not near or it's the last one. We need to add mask if
    //    alpha is larger than zero.
    else
    {
      uint alpha;
      if (area != 0)
      {
        if (x >= clipX1 && (alpha = rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh - area)) != 0)
        {
          FOG_ASSERT(x >= clipX1 && x < clipX2);
          uint m = (Span8::isPtrCMask(clipMask)) ? Span8::ptrToCMask(clipMask) : clipMask[x];
          alpha = ByteUtil::scalar_muldiv255(alpha, m);
          if (alpha)
          {
            scanline.newVSpanAlpha(x);
            scanline.addValueAlpha(alpha);
            scanline.endVSpanAlpha(x + 1);
          }
        }
        x++;
      }
      if (numCells == 0) break;
    }

    if (nextX > clipX1)
    {
      if (clipX2 <= x)
      {
        if ((clipCur = clipCur->getNext()) == NULL) goto end;
        clipX2 = clipCur->getX2();
        CLIP_SPAN_CHANGED()
        FOG_ASSERT(x < clipX2);
      }

      uint alpha = rasterizer->_calculateAlpha<_RULE, _USE_ALPHA>(coversh);
      if (alpha)
      {
        if (x < clipX1) x = clipX1;

        if (x < clipX2)
        {
          int toX = Math::min<int>(nextX, clipX2);
          FOG_ASSERT(x < clipX2);
          FOG_ASSERT(x < toX);

          if (Span8::isPtrCMask(clipMask))
          {
            uint alphaAdj = ByteUtil::scalar_muldiv255(alpha, Span8::ptrToCMask(clipMask));
            if (alphaAdj) scanline.addCSpanOrMergeVSpan(x, toX, alphaAdj);
          }
          else
          {
            rasterFuncs.mask[CLIP_OP_INTERSECT][IMAGE_FORMAT_A8].v_op_c(
              scanline.addVSpanAlphaOrMergeVSpan(x, toX), // Destination.
              clipMask + x,                               // Source A.
              alpha,                                      // Source B.
              toX - x);                                   // Length.
          }

          x = toX;
        }

        if (nextX > clipX2)
        {
          for (;;)
          {
            if ((clipCur = clipCur->getNext()) == NULL) goto end;
            clipX2 = clipCur->getX2();
            CLIP_SPAN_CHANGED()
            if (nextX > clipX1)
            {
              int toX = Math::min<int>(nextX, clipX2);
              FOG_ASSERT(clipX1 < toX);
              if (Span8::isPtrCMask(clipMask))
              {
                uint alphaAdj = ByteUtil::scalar_muldiv255(alpha, Span8::ptrToCMask(clipMask));
                if (alphaAdj) scanline.addCSpanOrMergeVSpan(clipX1, toX, alphaAdj);
              }
              else
              {
                rasterFuncs.mask[CLIP_OP_INTERSECT][IMAGE_FORMAT_A8].v_op_c(
                  scanline.addVSpanAlphaOrMergeVSpan(clipX1, toX), // Destination.
                  clipMask + clipX1,                               // Source A.
                  alpha,                                           // Source B.
                  toX - clipX1);                                   // Length.
              }
              if (clipX2 <= nextX) continue;
            }
            break;
          }
        }

      }

      if (clipX2 <= nextX) goto advanceClip;
    }
  }

end:
  if (scanline.endScanline() != ERR_OK) return NULL;
#if defined(FOG_DEBUG_RASTERIZER)
  dumpSpans(y, scanline.getSpans());
#endif // FOG_DEBUG_RASTERIZER
  return scanline.getSpans();

#undef CLIP_SPAN_CHANGED
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_rasterizer_init(void)
{
  using namespace Fog;

  rasterizer_local.init();
  return ERR_OK;
}

FOG_INIT_DECLARE void fog_rasterizer_shutdown(void)
{
  using namespace Fog;

  Rasterizer::cleanup();
  rasterizer_local.destroy();
}
