// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/MemoryBuffer.h>
#include <Fog/Core/Static.h>
#include <Fog/Face/FaceByte.h>
#include <Fog/Graphics/AnalyticRasterizer_p.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/LiangBarsky_p.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Graphics/Span_p.h>

namespace Fog {

// ============================================================================
// [Fog::AnalyticRasterizer8 - Debugging]
// ============================================================================

// #define FOG_DEBUG_RASTERIZER

// ============================================================================
// [Fog::AnalyticRasterizer8 - Constants]
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

static FOG_INLINE int muldiv(double a, double b, double c)
{
  return Math::iround(a * b / c);
}

static FOG_INLINE int upscale(double v)
{
  return Math::iround(v * POLY_SUBPIXEL_SCALE);
}

static FOG_INLINE bool useCellD(const AnalyticRasterizer8* rasterizer)
{
  return rasterizer->_size.w <= AnalyticRasterizer8::CellD::MAX_X;
}

// Forward Declarations.
static void _initPathSweepFunctions(AnalyticRasterizer8* rasterizer);
static void _initRectSweepFunctions(AnalyticRasterizer8* rasterizer);

// ============================================================================
// [Fog::AnalyticRasterizer8 - Construction / Destruction]
// ============================================================================

AnalyticRasterizer8::AnalyticRasterizer8()
{
  // Clear rows.
  _rowsCapacity = 0;
  _rows = NULL;

  _storage = NULL;
  _current = NULL;

  reset();
}

AnalyticRasterizer8::~AnalyticRasterizer8()
{
  if (_rows != NULL) Memory::free(_rows);

  CellStorage* storage = _storage;
  while (storage)
  {
    CellStorage* next = storage->getNext();
    Memory::free(storage);
    storage = next;
  }
}

// ============================================================================
// [Fog::AnalyticRasterizer8 - Reset / Initialize / Finalize]
// ============================================================================

void AnalyticRasterizer8::reset()
{
  // Clip-box / bounding-box.
  _clipBox.clear();
  _boundingBox.set(-1, -1, -1, -1);

  // Clear error state.
  _error = ERR_OK;
  // Default alpha is fully-opaque.
  _alpha = 0xFF;

  // Default fill rule / shape.
  _fillRule = FILL_DEFAULT;
  _shape = SHAPE_NONE;
  // Not finalized, not valid.
  _isFinalized = false;
  _isValid = false;

  // Sweep-scanline is initialized during initialize() or finalize().
  _sweepScanlineSimpleFn = NULL;
  _sweepScanlineRegionFn = NULL;
  _sweepScanlineSpansFn = NULL;
}

err_t AnalyticRasterizer8::initialize()
{
  _boundingBox.set(-1, -1, -1, -1);

  _error = ERR_OK;

  _shape = SHAPE_NONE;
  _isFinalized = false;
  _isValid = false;

  _offset.set(-_clipBox.x1, -_clipBox.y1);
  _size.set(_clipBox.getWidth(), _clipBox.getHeight());
  _size24x8.set(_size.w << POLY_SUBPIXEL_SHIFT, _size.h << POLY_SUBPIXEL_SHIFT);

  _sweepScanlineSimpleFn = NULL;
  _sweepScanlineRegionFn = NULL;
  _sweepScanlineSpansFn = NULL;

  uint i = (uint)_clipBox.getHeight();
  if (_rowsCapacity < i)
  {
    // Align...
    i = (i + 255U) & ~255U;
    if (_rows != NULL) Memory::free(_rows);

    _rows = reinterpret_cast<void**>(Memory::alloc(i * sizeof(void*)));
    if (_rows == NULL)
    {
      _rowsCapacity = 0;

      setError(ERR_RT_OUT_OF_MEMORY);
      goto end;
    }

    _rowsCapacity = i;
  }

end:
  return _error;
}

err_t AnalyticRasterizer8::finalize()
{
  // If already finalized this is the NOP.
  if (_error || _isFinalized) return _error;

  switch (_shape)
  {
    case SHAPE_NONE:
    {
      _isFinalized = true;
      _isValid = false;
      break;
    }

    case SHAPE_PATH:
    {
      if (_boundingBox.y1 == -1)
      {
        _isFinalized = true;
        _isValid = false;
        return ERR_OK;
      }

      // Translate bounding box to match _clipBox.
      _boundingBox.translate(_clipBox.x1, _clipBox.y1);
      // Normalize bounding box to our standard, x2/y2 coordinates are outside.
      _boundingBox.x2++;
      _boundingBox.y2++;

      _isFinalized = true;
      _isValid = true;

      _initPathSweepFunctions(this);
      break;
    }

    case SHAPE_SUBPX_RECTANGLE:
    {
      SubPxRectangleShape* shapeData =
        reinterpret_cast<SubPxRectangleShape*>(_rows);

      _boundingBox = shapeData->bounds;
      _boundingBox.x2++;
      _boundingBox.y2++;

      _isFinalized = true;
      _isValid = true;

      _initRectSweepFunctions(this);
      break;
    }
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::AnalyticRasterizer8 - Commands]
// ============================================================================

static bool _initCells(AnalyticRasterizer8* rasterizer)
{
  uint chunkSize = useCellD(rasterizer)
    ? (uint)AnalyticRasterizer8::ChunkD::CHUNK_SIZE
    : (uint)AnalyticRasterizer8::ChunkQ::CHUNK_SIZE;

  // Initialize cells storage.
  if (rasterizer->_storage)
  {
    rasterizer->_current = rasterizer->_storage;
    rasterizer->_current->setup(rasterizer->_current->getStorageSize(), chunkSize);
  }
  else
  {
    if (!rasterizer->getNextChunkStorage(chunkSize))
    {
      rasterizer->setError(ERR_RT_OUT_OF_MEMORY);
      return false;
    }
  }
  return true;
}

template<typename _CHUNK_TYPE, typename _CELL_TYPE>
static void _addPath(
  AnalyticRasterizer8* rasterizer,
  const DoublePoint* vertices, const uint8_t* commands, sysuint_t i)
{
  const uint8_t* end = commands + i;

  int24x8_t startX1; // Start moveTo x position.
  int24x8_t startY1; // Start moveTo y position.
  uint startF1;      // Start [x, y] clipping flags.

  int24x8_t x1;      // Current x position.
  int24x8_t y1;      // Current y position.
  uint f1;           // Current [x, y] clipping flags.

  // TODO: Clipping always enabled. We should add some parameter that will tell
  // us the path is in bounds and no vertex need to be clipped.
  for (;;)
  {
    // First find MOVE_TO command and then run LINE_TO commands.
    for (;;)
    {
      uint8_t cmd = commands[0];
      commands++;

      if (commands == end) return;
      if (PathCmd::isMoveTo(cmd))
      {
        x1 = startX1 = upscale(vertices->x) + rasterizer->_offset.x;
        y1 = startY1 = upscale(vertices->y) + rasterizer->_offset.y;
        f1 = startF1 = LiangBarsky::getClippingFlags(x1, y1, 0, 0, rasterizer->_size24x8.w, rasterizer->_size24x8.h);

        vertices++;
        break;
      }
      else
      {
        vertices++;
      }
    }

    // Run LINE_TO / CURVE_TO commands.
    for (;;)
    {
      uint8_t cmd = commands[0];
      commands++;

      if (PathCmd::isLineTo(cmd))
      {
        int24x8_t x2 = upscale(vertices->x) + rasterizer->_offset.x;
        int24x8_t y2 = upscale(vertices->y) + rasterizer->_offset.y;
        uint f2 = LiangBarsky::getClippingFlags(x2, y2, 0, 0, rasterizer->_size24x8.w, rasterizer->_size24x8.h);

        if ((x1 != x2) | (y1 != y2) && !rasterizer->clipLine<_CHUNK_TYPE, _CELL_TYPE>(x1, y1, x2, y2, f1, f2))
          goto _bail;

        x1 = x2;
        y1 = y2;
        f1 = f2;

        vertices++;
        if (commands == end) goto closePath;
        continue;
      }

      if (PathCmd::isMoveTo(cmd) || PathCmd::isClose(cmd) || PathCmd::isStop(cmd) || commands == end)
      {
        // Close current polygon.
closePath:
        if (startX1 != x1 || startY1 != y1)
        {
          if ((x1 != startX1) | (y1 != startY1) && !rasterizer->clipLine<_CHUNK_TYPE, _CELL_TYPE>(x1, y1, startX1, startY1, f1, startF1))
            goto _bail;
        }

        if (PathCmd::isMoveTo(cmd))
        {
          x1 = startX1 = upscale(vertices->x) + rasterizer->_offset.x;
          y1 = startY1 = upscale(vertices->y) + rasterizer->_offset.y;
          f1 = startF1 = LiangBarsky::getClippingFlags(x1, y1, 0, 0, rasterizer->_size24x8.w, rasterizer->_size24x8.h);
        }
        else
        {
          vertices++;
          if (commands == end) return;
          break;
        }
      }

      vertices++;
      if (commands == end) return;
    }
  }

_bail:
  return;
}

void AnalyticRasterizer8::addPath(const DoublePath& path)
{
  if (_error) return;
  FOG_ASSERT(_isFinalized == false);

  sysuint_t length = path.getLength();
  if (length == 0) return;

  if (_shape == SHAPE_NONE)
  {
    // Initialize cells and set shape to SHAPE_PATH.
    if (!_initCells(this)) return;
    _shape = SHAPE_PATH;
  }
  else if (_shape != SHAPE_PATH)
  {
    convertToPath();
  }

  if (useCellD(this))
    _addPath<ChunkD, CellD>(this, path.getVertices(), path.getCommands(), length);
  else
    _addPath<ChunkQ, CellQ>(this, path.getVertices(), path.getCommands(), length);
}

void AnalyticRasterizer8::addRect(const DoubleRect& rect)
{
  // Initial addRect(), we try to add rectangle to the data section and to 
  // initialize shape to SHAPE_SUBPX_RECTANGLE.
  if (_shape == SHAPE_NONE)
  {
    // Convert to fixed point.
    int rx1 = Math::doubleToFixed24x8(rect.x) + _offset.x;
    int ry1 = Math::doubleToFixed24x8(rect.y) + _offset.y;
    int rx2 = Math::doubleToFixed24x8(rect.x + rect.w) + _offset.x;
    int ry2 = Math::doubleToFixed24x8(rect.y + rect.h) + _offset.y;

    // Clip.
    if (rx1 < 0) rx1 = 0;
    if (ry1 < 0) ry1 = 0;
    if (rx2 > _size24x8.w) rx2 = _size24x8.w;
    if (ry2 > _size24x8.w) ry2 = _size24x8.h;
    if (rx1 >= rx2 || ry1 >= ry2) return;

    // Okay, the rectangle is in the clipBox.
    SubPxRectangleShape* shapeData =
      reinterpret_cast<SubPxRectangleShape*>(_rows);
    shapeData->rect = rect;
    shapeData->bounds.set(
      (rx1) >> 8, (ry1) >> 8,
      (rx2) >> 8, (ry2) >> 8);

    uint fx1 = rx1 & 0xFF;
    uint fy1 = ry1 & 0xFF;
    uint fx2 = rx2 & 0xFF;
    uint fy2 = ry2 & 0xFF;

    uint horzLeft   = 256 - fx1;
    uint horzRight  = fx2;

    uint vertTop    = 256 - fy1;
    uint vertBottom = fy2;

    if (shapeData->bounds.x1 == shapeData->bounds.x2)
    {
      horzRight -= horzLeft;
      horzLeft = horzRight;
    }

    if (shapeData->bounds.y1 == shapeData->bounds.y2)
    {
      vertBottom -= vertTop;
      vertTop = vertBottom;
    }

    shapeData->coverageT[0] = Math::boundToByte((horzLeft * vertTop) >> 8);
    shapeData->coverageT[1] = Math::boundToByte(vertTop);
    shapeData->coverageT[2] = Math::boundToByte((horzRight * vertTop) >> 8);

    shapeData->coverageI[0] = Math::boundToByte(horzLeft);
    shapeData->coverageI[1] = 0xFF;
    shapeData->coverageI[2] = Math::boundToByte(horzRight);

    shapeData->coverageB[0] = Math::boundToByte((horzLeft * vertBottom) >> 8);
    shapeData->coverageB[1] = Math::boundToByte(vertBottom);
    shapeData->coverageB[2] = Math::boundToByte((horzRight * vertBottom) >> 8);

    _shape = SHAPE_SUBPX_RECTANGLE;
    return;
  }

  if (_shape != SHAPE_PATH) convertToPath();

  DoublePoint vertices[5];
  uint8_t commands[5];

  double x1 = rect.x;
  double y1 = rect.y;
  double x2 = rect.x + rect.w;
  double y2 = rect.y + rect.h;

  vertices[0].set(x1, y1);
  vertices[1].set(x2, y1);
  vertices[2].set(x2, y2);
  vertices[3].set(x1, y1);
  vertices[4].set(NAN, NAN);

  commands[0] = PATH_CMD_MOVE_TO;
  commands[1] = PATH_CMD_LINE_TO;
  commands[2] = PATH_CMD_LINE_TO;
  commands[3] = PATH_CMD_LINE_TO;
  commands[4] = PATH_CMD_END;

  if (useCellD(this))
    _addPath<ChunkD, CellD>(this, vertices, commands, 5);
  else
    _addPath<ChunkQ, CellQ>(this, vertices, commands, 5);
}

void AnalyticRasterizer8::convertToPath()
{
  switch (_shape)
  {
    case SHAPE_NONE:
    {
      _shape = SHAPE_PATH;
      break;
    }

    case SHAPE_PATH:
    {
      break;
    }

    case SHAPE_SUBPX_RECTANGLE:
    {
      SubPxRectangleShape* shapeData =
        reinterpret_cast<SubPxRectangleShape*>(_rows);
      DoubleRect rect = shapeData->rect;

      if (!_initCells(this)) { _shape = SHAPE_NONE; return; }

      _shape = SHAPE_PATH;
      addRect(rect);
      break;
    }

    case SHAPE_AFFINE_RECTANGLE:
    {
      // TODO: Rasterizer affine rectangle.
      break;
    }
  }
}

// ============================================================================
// [Fog::AnalyticRasterizer8 - Cache]
// ============================================================================

bool AnalyticRasterizer8::getNextChunkStorage(sysuint_t chunkSize)
{
  CellStorage* storage = _current ? _current->getNext() : NULL;
  if (storage != NULL)
  {
    storage->setup(storage->getStorageSize(), chunkSize);

    _current = storage;
    return true;
  }

  storage = reinterpret_cast<CellStorage*>(Memory::alloc(CellStorage::STORAGE_SIZE));
  if (storage == NULL)
  {
    _error = ERR_RT_OUT_OF_MEMORY;
    return false;
  }

  storage->_prev = _current;
  storage->_next = NULL;
  storage->setup(CellStorage::STORAGE_SIZE, chunkSize);

  if (_current)
    _current->_next = storage;
  else
    _storage = storage;

  _current = storage;
  return true;
}

// ============================================================================
// [Fog::AnalyticRasterizer8 - Clipper]
// ============================================================================

template<typename _CHUNK_TYPE, typename _CELL_TYPE>
bool AnalyticRasterizer8::clipLine(int24x8_t x1, int24x8_t y1, int24x8_t x2, int24x8_t y2, uint f1, uint f2)
{
  // Invisible by Y.
  if ((f1 & 10) == (f2 & 10) && (f1 & 10) != 0) return true;

  int24x8_t y3, y4;
  uint f3, f4;

  switch (((f1 & 5) << 1) | (f2 & 5))
  {
    // Visible by X
    case 0:
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(x1, y1, x2, y2, f1, f2)) goto _bail;
      break;

    // x2 > clip.x2
    case 1:
      y3 = y1 + muldiv(_size24x8.w - x1, y2 - y1, x2 - x1);
      f3 = LiangBarsky::getClippingFlagsY(y3, 0, _size24x8.h);
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(x1, y1, _size24x8.w, y3, f1, f3)) goto _bail;
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(_size24x8.w, y3, _size24x8.w, y2, f3, f2)) goto _bail;
      break;

    // x1 > clip.x2
    case 2:
      y3 = y1 + muldiv(_size24x8.w - x1, y2 - y1, x2 - x1);
      f3 = LiangBarsky::getClippingFlagsY(y3, 0, _size24x8.h);
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(_size24x8.w, y1,_size24x8.w, y3, f1, f3)) goto _bail;
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(_size24x8.w, y3, x2, y2, f3, f2)) goto _bail;
      break;

    // x1 > clip.x2 && x2 > clip.x2
    case 3:
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(_size24x8.w, y1, _size24x8.w, y2, f1, f2)) goto _bail;
      break;

    // x2 < clipX1
    case 4:
      y3 = y1 + muldiv(0 - x1, y2 - y1, x2 - x1);
      f3 = LiangBarsky::getClippingFlagsY(y3, 0, _size24x8.h);
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(x1, y1, 0, y3, f1, f3)) goto _bail;
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(0, y3, 0, y2, f3, f2)) goto _bail;
      break;

    // x1 > clip.x2 && x2 < clip.x1
    case 6:
      y3 = y1 + muldiv(_size24x8.w - x1, y2 - y1, x2 - x1);
      y4 = y1 + muldiv(0 - x1, y2 - y1, x2 - x1);
      f3 = LiangBarsky::getClippingFlagsY(y3, 0, _size24x8.h);
      f4 = LiangBarsky::getClippingFlagsY(y4, 0, _size24x8.h);
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(_size24x8.w, y1, _size24x8.w, y3, f1, f3)) goto _bail;
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(_size24x8.w, y3, 0, y4, f3, f4)) goto _bail;
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(0, y4, 0, y2, f4, f2)) goto _bail;
      break;

    // x1 < clip.x1
    case 8:
      y3 = y1 + muldiv(0 - x1, y2 - y1, x2 - x1);
      f3 = LiangBarsky::getClippingFlagsY(y3, 0, _size24x8.h);
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(0, y1, 0, y3, f1, f3)) goto _bail;
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(0, y3, x2, y2, f3, f2)) goto _bail;
      break;

    // x1 < clip.x1 && x2 > clip.x2
    case 9:
      y3 = y1 + muldiv(0 - x1, y2 - y1, x2 - x1);
      y4 = y1 + muldiv(_size24x8.w - x1, y2 - y1, x2 - x1);
      f3 = LiangBarsky::getClippingFlagsY(y3, 0, _size24x8.h);
      f4 = LiangBarsky::getClippingFlagsY(y4, 0, _size24x8.h);
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(0, y1, 0, y3, f1, f3)) goto _bail;
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(0, y3, _size24x8.w, y4, f3, f4)) goto _bail;
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(_size24x8.w, y4, _size24x8.w, y2, f4, f2)) goto _bail;
      break;

    // x1 < clip.x1 && x2 < clip.x1
    case 12:
      if (!clipLineY<_CHUNK_TYPE, _CELL_TYPE>(0, y1, 0, y2, f1, f2)) goto _bail;
      break;
  }
  return true;

_bail:
  return false;
}

template<typename _CHUNK_TYPE, typename _CELL_TYPE>
FOG_INLINE bool AnalyticRasterizer8::clipLineY(int24x8_t x1, int24x8_t y1, int24x8_t x2, int24x8_t y2, uint f1, uint f2)
{
  f1 &= 10;
  f2 &= 10;

  if ((f1 | f2) == 0)
  {
    // Fully visible.
    return renderLine<_CHUNK_TYPE, _CELL_TYPE>(x1, y1, x2, y2); 
  }
  else
  {
    // Invisible by Y.
    if (f1 == f2) return true;

    int24x8_t tx1 = x1;
    int24x8_t ty1 = y1;
    int24x8_t tx2 = x2;
    int24x8_t ty2 = y2;

    if (f1 & 8) // y1 < clip.y1
    {
      tx1 = x1 + muldiv(0 - y1, x2 - x1, y2 - y1);
      ty1 = 0;
    }

    if (f1 & 2) // y1 > clip.y2
    {
      tx1 = x1 + muldiv(_size24x8.h - y1, x2 - x1, y2 - y1);
      ty1 = _size24x8.h;
    }

    if (f2 & 8) // y2 < clip.y1
    {
      tx2 = x1 + muldiv(0 - y1, x2 - x1, y2 - y1);
      ty2 = 0;
    }

    if (f2 & 2) // y2 > cliphy2
    {
      tx2 = x1 + muldiv(_size24x8.h - y1, x2 - x1, y2 - y1);
      ty2 = _size24x8.h;
    }

    return renderLine<_CHUNK_TYPE, _CELL_TYPE>(tx1, ty1, tx2, ty2); 
  }
}

// ============================================================================
// [Fog::AnalyticRasterizer8 - Renderer]
// ============================================================================

#define NEW_CHUNK(CHUNK_TYPE, _bail, _dst) \
  do { \
    if (FOG_UNLIKELY(_current->getChunkPtr() == _current->getChunkEnd()) && !getNextChunkStorage(CHUNK_TYPE::CHUNK_SIZE)) \
      goto _bail; \
    _dst = (CHUNK_TYPE*)_current->_chunkPtr; \
    _current->_chunkPtr += CHUNK_TYPE::CHUNK_SIZE; \
  } while (0)

#define ADD_CHUNK(CHUNK_TYPE, _bail, _y, _dst) \
  do { \
    NEW_CHUNK(CHUNK_TYPE, _bail, _dst); \
    _dst->_prev = reinterpret_cast<uint8_t*>(_rows[_y]); \
    _rows[_y] = reinterpret_cast<uint8_t*>(_dst); \
  } while (0)

#define GET_CHUNK(CHUNK_TYPE, _bail, _y, _dst) \
  do { \
    _dst = reinterpret_cast<CHUNK_TYPE*>(_rows[_y]); \
    if (FOG_UNLIKELY(_dst->isFull())) \
    { \
      NEW_CHUNK(CHUNK_TYPE, _bail, _dst); \
      _dst->_prev = reinterpret_cast<uint8_t*>(_rows[_y]); \
      _rows[_y] = reinterpret_cast<uint8_t*>(_dst); \
    } \
  } while (0)

#define ADD_CELL_SINGLE(CHUNK_TYPE, _bail, _y, _x, _cover, _weight) \
  do { \
    CHUNK_TYPE* _chunk; \
    GET_CHUNK(CHUNK_TYPE, _bail, _y, _chunk); \
    \
    _chunk->getCells()[_chunk->getCount()].setData(_x, _cover, _weight); \
    _chunk->incCount(1); \
  } while (0)

template<typename _CHUNK_TYPE, typename _CELL_TYPE>
bool AnalyticRasterizer8::renderLine(int24x8_t x1, int24x8_t y1, int24x8_t x2, int24x8_t y2)
{
  enum DXLimitEnum { DXLimit = 16384 << POLY_SUBPIXEL_SHIFT };

  int dx = x2 - x1;

  if (FOG_UNLIKELY((dx >= DXLimit) | (dx <= -DXLimit)))
  {
    int cx = (x1 + x2) >> 1;
    int cy = (y1 + y2) >> 1;

    if (!AnalyticRasterizer8::renderLine<_CHUNK_TYPE, _CELL_TYPE>(x1, y1, cx, cy)) return false;
    if (!AnalyticRasterizer8::renderLine<_CHUNK_TYPE, _CELL_TYPE>(cx, cy, x2, y2)) return false;

    return true;
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

  // Initialize bounding box if this is the first call to renderLine().
  if (FOG_UNLIKELY(_boundingBox.y1 == -1))
  {
    _boundingBox.x1 = ex1;
    _boundingBox.y1 = ey1;
    _boundingBox.x2 = ex1;
    _boundingBox.y2 = ey1;

    _CHUNK_TYPE* _chunk;
    NEW_CHUNK(_CHUNK_TYPE, _bail, _chunk);

    _chunk->_prev = NULL; \
    _rows[ey1] = _chunk;
  }

  if (ex1 < ex2)
  {
    if (ex1 < _boundingBox.x1) _boundingBox.x1 = ex1;
    if (ex2 > _boundingBox.x2) _boundingBox.x2 = ex2;
  }
  else
  {
    if (ex2 < _boundingBox.x1) _boundingBox.x1 = ex2;
    if (ex1 > _boundingBox.x2) _boundingBox.x2 = ex1;
  }

#define PURGE_ROWS(CHUNK_TYPE, start, count) \
  do { \
    CHUNK_TYPE** _p = (CHUNK_TYPE**)(&_rows[start]); \
    uint _c = count; \
    do { \
      CHUNK_TYPE* _chunk; \
      NEW_CHUNK(CHUNK_TYPE, _bail, _chunk); \
      \
      _chunk->_prev = NULL; \
      *_p++ = _chunk; \
    } while (--_c); \
  } while(0)

  if (ey1 < ey2)
  {
    if (ey1 < _boundingBox.y1) { PURGE_ROWS(_CHUNK_TYPE, ey1                , _boundingBox.y1 - ey1); _boundingBox.y1 = ey1; }
    if (ey2 > _boundingBox.y2) { PURGE_ROWS(_CHUNK_TYPE, _boundingBox.y2 + 1, ey2 - _boundingBox.y2); _boundingBox.y2 = ey2; }
  }
  else
  {
    if (ey2 < _boundingBox.y1) { PURGE_ROWS(_CHUNK_TYPE, ey2                , _boundingBox.y1 - ey2); _boundingBox.y1 = ey2; }
    if (ey1 > _boundingBox.y2) { PURGE_ROWS(_CHUNK_TYPE, _boundingBox.y2 + 1, ey1 - _boundingBox.y2); _boundingBox.y2 = ey1; }
  }
#undef PURGE_ROWS

  // Everything is on a single hline.
  if (ey1 == ey2)
  {
    return renderHLine<_CHUNK_TYPE, _CELL_TYPE>(ey1, x1, fy1, x2, fy2);
  }

  // Vertical line - we have to calculate start and end cells, and then - the
  // common values of the area and coverage for all cells of the line. We know
  // exactly there's only one cell, so, we don't have to call renderHLine().
  incr = 1;
  if (dx == 0)
  {
    int two_fx = (x1 - (ex1 << POLY_SUBPIXEL_SHIFT)) << 1;

    first = POLY_SUBPIXEL_SCALE;
    if (dy < 0) { first = 0; incr = -1; }

    delta = first - fy1;
    ADD_CELL_SINGLE(_CHUNK_TYPE, _bail, ey1, ex1, delta, two_fx);

    ey1 += incr;
    delta = first + first - POLY_SUBPIXEL_SCALE;

    while (ey1 != ey2)
    {
      ADD_CELL_SINGLE(_CHUNK_TYPE, _bail, ey1, ex1, delta, two_fx);
      ey1 += incr;
    }

    delta = first + fy2 - POLY_SUBPIXEL_SCALE;
    ADD_CELL_SINGLE(_CHUNK_TYPE, _bail, ey1, ex1, delta, two_fx);
    return true;
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
  renderHLine<_CHUNK_TYPE, _CELL_TYPE>(ey1, x1, fy1, x_from, first);
  ey1 += incr;

  if (ey1 != ey2)
  {
    p     = dx * POLY_SUBPIXEL_SCALE;
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
      if (!renderHLine<_CHUNK_TYPE, _CELL_TYPE>(ey1, x_from, POLY_SUBPIXEL_SCALE - first, x_to, first)) goto _bail;
      x_from = x_to;

      ey1 += incr;
    }
  }

  return renderHLine<_CHUNK_TYPE, _CELL_TYPE>(ey1, x_from, POLY_SUBPIXEL_SCALE - first, x2, fy2);

_bail:
  return false;
}

template<typename _CHUNK_TYPE, typename _CELL_TYPE>
FOG_INLINE bool AnalyticRasterizer8::renderHLine(int ey, int24x8_t x1, int24x8_t y1, int24x8_t x2, int24x8_t y2)
{
  int ex1;
  int ex2;
  int fx1;
  int fx2;

  int delta, p, dx;
  int lift, mod, rem;

  // Trivial case. Happens often.
  if (y1 == y2) return true;

  ex1 = x1 >> POLY_SUBPIXEL_SHIFT;
  ex2 = x2 >> POLY_SUBPIXEL_SHIFT;

  fx1 = x1 & POLY_SUBPIXEL_MASK;
  fx2 = x2 & POLY_SUBPIXEL_MASK;

  _CHUNK_TYPE* chunk;
  uint index;

  GET_CHUNK(_CHUNK_TYPE, _bail, ey, chunk);
  index = chunk->getCount();

  // Everything is located in a single cell.
  if (ex1 == ex2)
  {
    delta = y2 - y1;
    chunk->getCells()[index].setData(ex1, delta, fx1 + fx2);
    chunk->incCount(1);
    return true;
  }

  // Ok, we'll have to render a run of adjacent cells on the same hline...
  dx = x2 - x1;
  if (dx < 0)
  {
    dx    = -dx;

    p     = (y2 - y1) * fx1;
    delta = p / dx;
    mod   = p % dx;

    if (mod < 0) { mod += dx; delta--; }

    chunk->getCells()[index++].setData(ex1, delta, fx1);

    ex1--;
    y1 += delta;

    if (ex1 != ex2)
    {
      p     = (y2 - y1 + delta) * POLY_SUBPIXEL_SCALE;
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
          if (FOG_UNLIKELY(index == _CHUNK_TYPE::CELLS_COUNT))
          {
            chunk->setCount(_CHUNK_TYPE::CELLS_COUNT);
            ADD_CHUNK(_CHUNK_TYPE, _bail, ey, chunk);
            index = 0;
          }
          chunk->getCells()[index++].setData(ex1, delta, POLY_SUBPIXEL_SCALE);
        }

        y1 += delta;
      } while (--ex1 != ex2);
    }

    delta = y2 - y1;
    if (FOG_UNLIKELY(index == _CHUNK_TYPE::CELLS_COUNT))
    {
      chunk->setCount(_CHUNK_TYPE::CELLS_COUNT);
      ADD_CHUNK(_CHUNK_TYPE, _bail, ey, chunk);
      index = 0;
    }

    chunk->getCells()[index++].setData(ex1, delta, fx2 + POLY_SUBPIXEL_SCALE);
    chunk->setCount(index);
    return true;
  }
  else
  {
    p     = (y2 - y1) * (POLY_SUBPIXEL_SCALE - fx1);
    delta = p / dx;
    mod   = p % dx;

    if (mod < 0) { mod += dx; delta--; }

    chunk->getCells()[index++].setData(ex1, delta, fx1 + POLY_SUBPIXEL_SCALE);

    ex1++;
    y1 += delta;

    if (ex1 != ex2)
    {
      p     = (y2 - y1 + delta) * POLY_SUBPIXEL_SCALE;
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
          if (FOG_UNLIKELY(index == _CHUNK_TYPE::CELLS_COUNT))
          {
            chunk->setCount(_CHUNK_TYPE::CELLS_COUNT);
            ADD_CHUNK(_CHUNK_TYPE, _bail, ey, chunk);
            index = 0;
          }
          chunk->getCells()[index++].setData(ex1, delta, POLY_SUBPIXEL_SCALE);
        }

        y1 += delta;
      } while (++ex1 != ex2);
    }

    delta = y2 - y1;
    if (FOG_UNLIKELY(index == _CHUNK_TYPE::CELLS_COUNT))
    {
      chunk->setCount(_CHUNK_TYPE::CELLS_COUNT);
      ADD_CHUNK(_CHUNK_TYPE, _bail, ey, chunk);
      index = 0;
    }

    chunk->getCells()[index++].setData(ex1, delta, fx2);
    chunk->setCount(index);
    return true;
  }

_bail:
  return false;
}

// ============================================================================
// [Fog::AnalyticRasterizer8 - QSort]
// ============================================================================

template <typename CELL>
static FOG_INLINE void swapCells(CELL* FOG_RESTRICT a, CELL* FOG_RESTRICT b)
{
  CELL temp = *a;
  *a = *b;
  *b = temp;
}

template<class CELL>
static FOG_INLINE void qsortCells(CELL* start, uint32_t num)
{
  CELL*  stack[80];
  CELL** top; 
  CELL*  limit;
  CELL*  base;

  limit = start + num;
  base  = start;
  top   = stack;

  for (;;)
  {
    sysuint_t len = sysuint_t(limit - base);

    CELL* i;
    CELL* j;
    CELL* pivot;

    if (len > RASTERIZER_QSORT_THRESHOLD)
    {
      // We use base + len/2 as the pivot.
      pivot = base + len / 2;
      swapCells(base, pivot);

      i = base + 1;
      j = limit - 1;

      // Now ensure that *i <= *base <= *j .
      if (j->getComparable() < i->getComparable()) swapCells(i, j);
      if (base->getComparable() < i->getComparable()) swapCells(base, i);
      if (j->getComparable() < base->getComparable()) swapCells(base, j);

      for (;;)
      {
        uint32_t c = base->getComparable();
        do { i++; } while (i->getComparable() < c);
        do { j--; } while (c < j->getComparable());

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
        for (; j[0].getComparable() >= j[1].getComparable(); j--)
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

// ============================================================================
// [Fog::AnalyticRasterizer8 - Sweep]
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
static FOG_INLINE uint _calculateAlpha(const AnalyticRasterizer8* rasterizer, int area)
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

  if (_USE_ALPHA) cover = Face::b32_1x1MulDiv255(cover, rasterizer->_alpha);
  return cover;
}

template<typename _CHUNK_TYPE, typename _CELL_TYPE>
static bool _mergeCells(AnalyticRasterizer8* rasterizer, void* _chunks, MemoryBuffer& temp, _CELL_TYPE** cellsOut, uint* numCellsOut)
{
  _CHUNK_TYPE* chunk = reinterpret_cast<_CHUNK_TYPE*>(_chunks);
  _CELL_TYPE* cellCur = chunk->getCells();

  uint numCells = chunk->getCount();
  if (!numCells) return false;

  {
    // First calculate count of cellCur needed to alloc.
    _CHUNK_TYPE* chunkCur;

    chunkCur = chunk->getPrev();
    while (chunkCur)
    {
      numCells += chunkCur->getCount();
      chunkCur = chunkCur->getPrev();
    };

    _CELL_TYPE* buf = reinterpret_cast<_CELL_TYPE*>(temp.alloc((sysuint_t)numCells * sizeof(_CELL_TYPE)));
    if (!buf) return false;

    chunkCur = chunk;
    do {
      cellCur = chunkCur->getCells();
      uint i = chunkCur->getCount();

      // Copy cells reversed if they are in reverse order.
      if (i > 1 && cellCur[0].getComparable() > cellCur[1].getComparable())
      {
        cellCur += i - 1;
        do { buf->setData(*cellCur); buf++; cellCur--; } while (--i);
      }
      else
      {
        do { buf->setData(*cellCur); buf++; cellCur++; } while (--i);
      }

      chunkCur = chunkCur->getPrev();
    } while (chunkCur);

    cellCur = reinterpret_cast<_CELL_TYPE*>(temp.getMemoryBuffer());
  }

  // QSort.
  qsortCells<_CELL_TYPE>(cellCur, numCells);

  *cellsOut = cellCur;
  *numCellsOut = numCells;
  return true;
}

template<typename _CHUNK_TYPE, typename _CELL_TYPE, int _RULE, int _USE_ALPHA>
static Span8* _sweepScanlineSimpleImpl(
  AnalyticRasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y)
{
  FOG_ASSERT(rasterizer->_isFinalized);

  y -= rasterizer->_clipBox.y1;
  if ((uint)y >= (uint)rasterizer->_size.h) return NULL;

  _CELL_TYPE* cellCur;
  uint numCells;

  if (!_mergeCells<_CHUNK_TYPE, _CELL_TYPE>(rasterizer, rasterizer->_rows[y], temp, &cellCur, &numCells))
    return NULL;

  int xOffset = rasterizer->_clipBox.x1;
  int x;
  int nextX = cellCur->getX() + xOffset;
  int area;
  int cover = 0;

  if (scanline.newScanline(rasterizer->_boundingBox.x1, rasterizer->_boundingBox.x2) != ERR_OK)
    return NULL;

  for (;;)
  {
    x      = nextX;
    area   = cellCur->getArea();
    cover += cellCur->getCover();

    while (--numCells)
    {
      cellCur++;

      nextX = cellCur->getX() + xOffset;
      if (nextX != x) break;

      area  += cellCur->getArea();
      cover += cellCur->getCover();
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
      if ((alpha = _calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area)) == 0) continue;

      // Okay, it seems that we will now generate some masks, embedded to one
      // span instance.
      scanline.newVSpanAlpha(x);
      scanline.addValueAlpha((uint8_t)alpha);

      for (;;)
      {
        x      = nextX;
        area   = cellCur->getArea();
        cover += cellCur->getCover();

        while (--numCells)
        {
          cellCur++;

          nextX = cellCur->getX() + xOffset;
          if (nextX != x) break;

          area  += cellCur->getArea();
          cover += cellCur->getCover();
        }

        coversh = cover << (POLY_SUBPIXEL_SHIFT + 1);
        alpha = _calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area);

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
        if ((alpha = _calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area)) != 0)
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
      uint alpha = _calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh);
      if (alpha) scanline.addCSpanOrMergeVSpan(x, nextX, alpha);
    }
  }

  if (scanline.endScanline() != ERR_OK) return NULL;
#if defined(FOG_DEBUG_RASTERIZER)
  dumpSpans(y, scanline.getSpans());
#endif // FOG_DEBUG_RASTERIZER
  return scanline.getSpans();
}

template<typename _CHUNK_TYPE, typename _CELL_TYPE, int _RULE, int _USE_ALPHA>
static Span8* _sweepScanlineRegionImpl(
  AnalyticRasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y,
  const IntBox* clipBoxes, sysuint_t count)
{
  // TODO:
  return NULL;
}

template<typename _CHUNK_TYPE, typename _CELL_TYPE, int _RULE, int _USE_ALPHA>
static Span8* _sweepScanlineSpansImpl(
  AnalyticRasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y,
  const Span8* clipSpans)
{
  // TODO:
  return NULL;
}

static Span8* _sweepRectSimpleImpl(
  AnalyticRasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y)
{
  y -= rasterizer->_boundingBox.y1;
  if ((uint)y >= (uint)rasterizer->_boundingBox.y2) return NULL;

  int xOffset = rasterizer->_clipBox.x1;

  if (scanline.newScanline(rasterizer->_boundingBox.x1, rasterizer->_boundingBox.x2) != ERR_OK)
    return NULL;

  AnalyticRasterizer8::SubPxRectangleShape* shapeData =
    reinterpret_cast<AnalyticRasterizer8::SubPxRectangleShape*>(rasterizer->_rows);

  int x1 = shapeData->bounds.x1;
  int x2 = shapeData->bounds.x2;

  int w = x2 - x1;

  const uint8_t* covers = shapeData->coverageI;

  if (FOG_UNLIKELY(y == 0))
  {
    covers = shapeData->coverageT;
  }
  else if (FOG_UNLIKELY(y == (shapeData->bounds.y2 - shapeData->bounds.y1)))
  {
    covers = shapeData->coverageB;
  }

  scanline.addVSpanAlpha(x1, x1 + 1)[0] = (uint8_t)covers[0];
  if (w > 1) scanline.addCSpanOrMergeVSpan(x1 + 1, x2, covers[1]);
  if (w > 0) scanline.addVSpanAlphaOrMergeVSpan(x2, x2 + 1)[0] = covers[2];

  if (scanline.endScanline() != ERR_OK) return NULL;
#if defined(FOG_DEBUG_RASTERIZER)
  dumpSpans(y, scanline.getSpans());
#endif // FOG_DEBUG_RASTERIZER
  return scanline.getSpans();
}

static void _initPathSweepFunctions(AnalyticRasterizer8* rasterizer)
{
#define SETUP_SWEEP(rasterizer, _FILL_MODE, _USE_ALPHA) \
  do { \
    if (useCellD(rasterizer)) \
    { \
      rasterizer->_sweepScanlineSimpleFn = _sweepScanlineSimpleImpl<AnalyticRasterizer8::ChunkD, AnalyticRasterizer8::CellD, _FILL_MODE, _USE_ALPHA>; \
      rasterizer->_sweepScanlineRegionFn = _sweepScanlineRegionImpl<AnalyticRasterizer8::ChunkD, AnalyticRasterizer8::CellD, _FILL_MODE, _USE_ALPHA>; \
      rasterizer->_sweepScanlineSpansFn  = _sweepScanlineSpansImpl <AnalyticRasterizer8::ChunkD, AnalyticRasterizer8::CellD, _FILL_MODE, _USE_ALPHA>; \
    } \
    else \
    { \
      rasterizer->_sweepScanlineSimpleFn = _sweepScanlineSimpleImpl<AnalyticRasterizer8::ChunkQ, AnalyticRasterizer8::CellQ, _FILL_MODE, _USE_ALPHA>; \
      rasterizer->_sweepScanlineRegionFn = _sweepScanlineRegionImpl<AnalyticRasterizer8::ChunkQ, AnalyticRasterizer8::CellQ, _FILL_MODE, _USE_ALPHA>; \
      rasterizer->_sweepScanlineSpansFn  = _sweepScanlineSpansImpl <AnalyticRasterizer8::ChunkQ, AnalyticRasterizer8::CellQ, _FILL_MODE, _USE_ALPHA>; \
    } \
  } while(0)

  // Setup sweep scanline methods.
  switch (rasterizer->_fillRule)
  {
    case FILL_NON_ZERO:
      if (rasterizer->_alpha == 0xFF)
        SETUP_SWEEP(rasterizer, FILL_NON_ZERO, 0);
      else
        SETUP_SWEEP(rasterizer, FILL_NON_ZERO, 1);
      break;

    case FILL_EVEN_ODD:
      if (rasterizer->_alpha == 0xFF)
        SETUP_SWEEP(rasterizer, FILL_EVEN_ODD, 0);
      else
        SETUP_SWEEP(rasterizer, FILL_EVEN_ODD, 1);
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

static void _initRectSweepFunctions(AnalyticRasterizer8* rasterizer)
{
  rasterizer->_sweepScanlineSimpleFn = _sweepRectSimpleImpl;
  // TODO:
  rasterizer->_sweepScanlineRegionFn = NULL;
  rasterizer->_sweepScanlineSpansFn = NULL;
}









#if 0
template<int _RULE, int _USE_ALPHA>
Span8* AnalyticRasterizer8::_sweepScanlineRegionImpl(
  AnalyticRasterizer8* rasterizer, Scanline8& scanline, int y,
  const IntBox* clipBoxes, sysuint_t count)
{
  FOG_ASSERT(rasterizer->_isFinalized);
  if (y >= rasterizer->_boundingBox.y2) return NULL;

  const RowInfo& ri = rasterizer->_rowsInfo[y - rasterizer->_boundingBox.y1];

  uint numCells = ri.count;
  if (!numCells) return NULL;

  const CellX* cellCur = &rasterizer->_cellsSorted[ri.index];

  // Clipping.
  const IntBox* clipCur = clipBoxes;
  const IntBox* clipEnd = clipBoxes + count;
  if (FOG_UNLIKELY(clipCur == clipEnd)) return NULL;

  if (scanline.newScanline(rasterizer->_boundingBox.x1, rasterizer->_boundingBox.x2) != ERR_OK)
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
      if ((alpha = _calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area)) == 0) continue;

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
        alpha = _calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area);

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
        if (x >= clipX1 && (alpha = _calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area)) != 0)
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

      uint alpha = _calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh);
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
Span8* AnalyticRasterizer8::_sweepScanlineSpansImpl(
  AnalyticRasterizer8* rasterizer, Scanline8& scanline, int y,
  const Span8* clipSpans)
{
  FOG_ASSERT(rasterizer->_isFinalized);
  if (y >= rasterizer->_boundingBox.y2) return NULL;

  const RowInfo& ri = rasterizer->_rowsInfo[y - rasterizer->_boundingBox.y1];

  uint numCells = ri.count;
  if (!numCells) return NULL;

  const CellX* cellCur = &rasterizer->_cellsSorted[ri.index];

  // Clipping.
  const Span8* clipCur = clipSpans;
  if (FOG_UNLIKELY(clipCur == NULL)) return NULL;

  if (scanline.newScanline(rasterizer->_boundingBox.x1, rasterizer->_boundingBox.x2) != ERR_OK)
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
      if ((alpha = _calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area)) == 0)
      {
        if (clipX2 <= nextX) goto advanceClip;
        continue;
      }

      // Okay, it seems that we will now generate some masks, embedded to one
      // span instance.
      if (Span8::isPtrCMask(clipMask))
      {
        uint32_t m = Span8::ptrToCMask(clipMask);
        if ((alpha = Face::b32_1x1MulDiv255(alpha, m)) == 0)
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
          alpha = Face::b32_1x1MulDiv255(_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area), m);

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
        if ((alpha = Face::b32_1x1MulDiv255(alpha, clipMask[x])) == 0)
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
          alpha = Face::b32_1x1MulDiv255(_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area), clipMask[x]);

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
        if (x >= clipX1 && (alpha = _calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area)) != 0)
        {
          FOG_ASSERT(x >= clipX1 && x < clipX2);
          uint m = (Span8::isPtrCMask(clipMask)) ? Span8::ptrToCMask(clipMask) : clipMask[x];
          alpha = Face::b32_1x1MulDiv255(alpha, m);
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

      uint alpha = _calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh);
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
            uint alphaAdj = Face::b32_1x1MulDiv255(alpha, Span8::ptrToCMask(clipMask));
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
                uint alphaAdj = Face::b32_1x1MulDiv255(alpha, Span8::ptrToCMask(clipMask));
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
#endif

} // Fog namespace
