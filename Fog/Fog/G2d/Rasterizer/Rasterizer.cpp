// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Face/Face_C.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/Internal_Core_p.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Memory/MemoryBuffer.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Rasterizer/Rasterizer_p.h>
#include <Fog/G2d/Rasterizer/Scanline_p.h>
#include <Fog/G2d/Rasterizer/Span_p.h>
#include <Fog/G2d/Render/RenderApi_p.h>

namespace Fog {

// ============================================================================
// [Fog::Rasterizer8 - Constants]
// ============================================================================

enum
{
  RASTERIZER_QSORT_THRESHOLD = 9
};

enum A8_POLY_SUBPIXEL_ENUM
{
  A8_POLY_SUBPIXEL_SHIFT = 8,                           // 8
  A8_POLY_SUBPIXEL_SCALE = 1 << A8_POLY_SUBPIXEL_SHIFT, // 256
  A8_POLY_SUBPIXEL_MASK  = A8_POLY_SUBPIXEL_SCALE - 1   // 255
};

enum A8_ENUM
{
  A8_SHIFT   = 8,             // 8
  A8_SCALE   = 1 << A8_SHIFT, // 256
  A8_SCALE_2 = A8_SCALE * 2,  // 512
  A8_MASK    = A8_SCALE - 1,  // 255
  A8_MASK_2  = A8_SCALE_2 - 1 // 511
};

static FOG_INLINE int upscale(float  v) { return (int)(v * (float )A8_POLY_SUBPIXEL_SCALE + 0.5f); }
static FOG_INLINE int upscale(double v) { return (int)(v * (double)A8_POLY_SUBPIXEL_SCALE + 0.5 ); }

// Forward Declarations.
template<typename _CHUNK_TYPE, typename _CELL_TYPE>
static bool _Rasterizer8_renderLine(Rasterizer8* self, Fixed24x8 x0, Fixed24x8 y0, Fixed24x8 x1, Fixed24x8 y1);

FOG_NO_EXPORT void _Rasterizer8_Path_initSweepFunctions(Rasterizer8* rasterizer);
FOG_NO_EXPORT void _Rasterizer8_Rect_initSweepFunctions(Rasterizer8* rasterizer);

static FOG_INLINE bool _Rasterizer8_useCellD(const Rasterizer8* rasterizer)
{
  return rasterizer->_size.w <= Rasterizer8::CellD::MAX_X;
}

// ============================================================================
// [Fog::Rasterizer8 - Construction / Destruction]
// ============================================================================

Rasterizer8::Rasterizer8()
{
  // Default is no multihreading.
  _offset = 0;
  _delta = 1;

  // Clear rows.
  _rowsCapacity = 0;
  _rows = NULL;

  _storage = NULL;
  _current = NULL;

  reset();
}

Rasterizer8::~Rasterizer8()
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
// [Fog::Rasterizer8 - Reset]
// ============================================================================

void Rasterizer8::reset()
{
  // Clip-box / Bounding-box.
  _sceneBox.reset();
  _boundingBox.setBox(-1, -1, -1, -1);

  // Clear error state.
  _error = ERR_OK;
  // Default alpha is fully-opaque.
  _alpha = 0xFF;

  // Default fill rule / shape.
  _fillRule = FILL_RULE_DEFAULT;
  _shape = RASTERIZER_SHAPE_NONE;
  // Not valid either finalized.
  _isValid = false;
  _isFinalized = false;

  // Sweep functions are initialized during initialize() or finalize().
  _sweepSimpleFn = NULL;
  _sweepRegionFn = NULL;
  _sweepSpansFn  = NULL;
}

// ============================================================================
// [Fog::Rasterizer8 - Initialize]
// ============================================================================

err_t Rasterizer8::initialize()
{
  _boundingBox.setBox(-1, -1, -1, -1);

  _error = ERR_OK;
  _shape = RASTERIZER_SHAPE_NONE;
  _isValid = false;
  _isFinalized = false;

  _size.set(_sceneBox.getWidth(), _sceneBox.getHeight());
  _size24x8.set(_size.w << A8_POLY_SUBPIXEL_SHIFT, _size.h << A8_POLY_SUBPIXEL_SHIFT);

  _offsetF.set(-_sceneBox.x0, -_sceneBox.y0);
  _offsetD.set(-_sceneBox.x0, -_sceneBox.y0);
  _offset24x8.set((-_sceneBox.x0) << A8_POLY_SUBPIXEL_SHIFT, (-_sceneBox.y0) << A8_POLY_SUBPIXEL_SHIFT);

  _sweepSimpleFn = NULL;
  _sweepRegionFn = NULL;
  _sweepSpansFn  = NULL;

  uint i = (uint)_sceneBox.getHeight() + 1;
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
      goto _End;
    }

    _rowsCapacity = i;
  }

_End:
  return _error;
}

// ============================================================================
// [Fog::Rasterizer8 - Path - Helpers]
// ============================================================================

static bool _Rasterizer8_initCells(Rasterizer8* rasterizer)
{
  uint chunkSize = _Rasterizer8_useCellD(rasterizer)
    ? (uint)Rasterizer8::ChunkD::CHUNK_SIZE
    : (uint)Rasterizer8::ChunkQ::CHUNK_SIZE;

  // Initialize cell-storage.
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

// ============================================================================
// [Fog::Rasterizer8 - Path - Add]
// ============================================================================

template<typename SrcT, typename _CHUNK_TYPE, typename _CELL_TYPE>
static void _Rasterizer_addPathData(Rasterizer8* self,
  const SrcT_(Point)* vertices, const uint8_t* commands, size_t count)
{
  if (count == 0) return;

  const SrcT flatness = Math2dConst<SrcT>::getDefaultFlatness();
  const uint8_t* end = commands + count;

  // Polyline points and length.
  const SrcT_(Point)* polyPoints;
  size_t polyLength;

  // Temporary path.
  SrcT_(Path)* tmpPath = (sizeof(SrcT) == sizeof(float))
    ? reinterpret_cast<SrcT_(Path)*>(&self->_tmpPathF)
    : reinterpret_cast<SrcT_(Path)*>(&self->_tmpPathD);

  // Current/Start moveTo x position.
  Fixed24x8 x0, startX0;

  // Current/Start moveTo y position.
  Fixed24x8 y0, startY0;

  // Current command.
  uint8_t cmd;

_NewPath:
  // Find: 'move-to' command and then go to the second loop.
  for (;;)
  {
    cmd = commands[0];
    commands++;
    vertices++;

    // This can happen only when 'move-to' is the last command. In this case
    // there is nothing to rasterize.
    if (commands == end) return;

    // ------------------------------------------------------------------------
    // [MoveTo]
    // ------------------------------------------------------------------------

    if (PathCmd::isMoveTo(cmd))
    {
_MoveTo:
      x0 = upscale(vertices[-1].x + self->_offsetF.x);
      y0 = upscale(vertices[-1].y + self->_offsetF.y);

      // Correct small errors which might be caused by floating point clipper.
      if (x0 < 0) x0 = 0;
      if (y0 < 0) y0 = 0;
      if (x0 > self->_size24x8.w) x0 = self->_size24x8.w;
      if (y0 > self->_size24x8.h) y0 = self->_size24x8.h;

      startX0 = x0;
      startY0 = y0;
      break;
    }
  }

  // Process: 'line-to', 'quad-to' and 'cubic-to'.
  // Stop at: 'move-to', 'close'.
  for (;;)
  {
    cmd = commands[0];

    // ------------------------------------------------------------------------
    // [LineTo]
    // ------------------------------------------------------------------------

    if (PathCmd::isLineTo(cmd))
    {
      const uint8_t* mark = commands++;

      while (commands != end)
      {
        cmd = commands[0];
        if (!PathCmd::isLineTo(cmd)) break;
        commands++;
      }

      polyPoints = vertices;
      polyLength = (size_t)(commands - mark);

      vertices += polyLength;
      goto _PolyLine;
    }

    // ------------------------------------------------------------------------
    // [QuadTo / CubicTo]
    // ------------------------------------------------------------------------

    else if (PathCmd::isQuadOrCubicTo(cmd))
    {
      tmpPath->clear();
      if (PathCmd::isQuadTo(cmd))
      {
        FOG_ASSERT(commands + 2 <= end);

        err_t err = reinterpret_cast<const SrcT_(QBezier)*>(vertices - 1)->flatten(*tmpPath, PATH_CMD_LINE_TO, flatness);
        if (FOG_IS_ERROR(err)) return;

        commands += 2;
        vertices += 2;
      }
      else
      {
        FOG_ASSERT(commands + 3 <= end);
        err_t err = reinterpret_cast<const SrcT_(CBezier)*>(vertices - 1)->flatten(*tmpPath, PATH_CMD_LINE_TO, flatness);
        if (FOG_IS_ERROR(err)) return;

        commands += 3;
        vertices += 3;
      }

      polyPoints = tmpPath->getVertices();
      polyLength = tmpPath->getLength();

_PolyLine:
      FOG_ASSERT(polyLength > 0);
      do {
        Fixed24x8 x1 = upscale(polyPoints->x + self->_offsetF.x);
        Fixed24x8 y1 = upscale(polyPoints->y + self->_offsetF.y);

        // Correct small errors which might be caused by floating point clipper.
        if (x1 < 0) x1 = 0;
        if (y1 < 0) y1 = 0;
        if (x1 > self->_size24x8.w) x1 = self->_size24x8.w;
        if (y1 > self->_size24x8.h) y1 = self->_size24x8.h;

        if ((x0 != x1) | (y0 != y1) && !_Rasterizer8_renderLine<_CHUNK_TYPE, _CELL_TYPE>(self, x0, y0, x1, y1))
          return;

        x0 = x1;
        y0 = y1;

        polyPoints++;
      } while (--polyLength);

      if (commands == end) goto _ClosePath;
    }

    // ------------------------------------------------------------------------
    // [MoveTo / Close]
    // ------------------------------------------------------------------------

    else
    {
      FOG_ASSERT(PathCmd::isMoveTo(cmd) || PathCmd::isClose(cmd));

_ClosePath:
      // Close the current polygon.
      if ((x0 != startX0) | (y0 != startY0) && !_Rasterizer8_renderLine<_CHUNK_TYPE, _CELL_TYPE>(self, x0, y0, startX0, startY0))
        return;

      if (commands == end) return;
      commands++;
      vertices++;
      if (commands == end) return;

      if (PathCmd::isMoveTo(cmd))
        goto _MoveTo;
      else
        goto _NewPath;
    }
  }
}

template<typename SrcT>
static void _Rasterizer8_addPath(Rasterizer8* self, const SrcT_(Path)& path)
{
  if (self->_error != ERR_OK) return;
  FOG_ASSERT(self->_isFinalized == false);

  size_t length = path.getLength();
  if (length == 0) return;

  // Initialize the generic path/polygon rasterizer.
  if (self->_shape != RASTERIZER_SHAPE_PATH)
    self->switchToPath();

  if (_Rasterizer8_useCellD(self))
    _Rasterizer_addPathData<SrcT, Rasterizer8::ChunkD, Rasterizer8::CellD>(self, path.getVertices(), path.getCommands(), length);
  else
    _Rasterizer_addPathData<SrcT, Rasterizer8::ChunkQ, Rasterizer8::CellQ>(self, path.getVertices(), path.getCommands(), length);
}

void Rasterizer8::addPath(const PathF& path) { _Rasterizer8_addPath<float >(this, path); }
void Rasterizer8::addPath(const PathD& path) { _Rasterizer8_addPath<double>(this, path); }

// ============================================================================
// [Fog::Rasterizer8 - Rect/Box - Add]
// ============================================================================

static void _Rasterizer8_addBox24x8(Rasterizer8* self, const BoxI& box24x8)
{
  int x0 = box24x8.x0;
  int y0 = box24x8.y0;
  int x1 = box24x8.x1;
  int y1 = box24x8.y1;

  // Clip.
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 > self->_size24x8.w) x1 = self->_size24x8.w;
  if (y1 > self->_size24x8.h) y1 = self->_size24x8.h;
  if (x0 >= x1 || y0 >= y1) return;

  // Okay, the rectangle is in the clipBox.
  Rasterizer8::ShapeRect* shape = reinterpret_cast<Rasterizer8::ShapeRect*>(self->_rows);

  shape->bounds.setBox(x0 >> 8, y0 >> 8, x1 >> 8, y1 >> 8);
  shape->box24x8.setBox(x0, y0, x1, y1);
  shape->xLeft = self->_sceneBox.x0 + shape->bounds.x0;
  shape->xRight = self->_sceneBox.x0 + shape->bounds.x1;

  uint32_t fx0 = x0 & 0xFF;
  uint32_t fy0 = y0 & 0xFF;
  uint32_t fx1 = x1 & 0xFF;
  uint32_t fy1 = y1 & 0xFF;

  uint32_t horzLeft   = 256 - fx0;
  uint32_t horzRight  = fx1;
  uint32_t vertTop    = 256 - fy0;
  uint32_t vertBottom = fy1;

  uint32_t alpha = self->_alpha;

  if ((x0 & ~0xFF) == (x1 & ~0xFF))
  {
    horzRight -= horzLeft;
    horzLeft = horzRight;
  }

  if ((y0 & ~0xFF) == (y1 & ~0xFF))
  {
    vertBottom -= vertTop;
    vertTop = vertBottom;
  }

  vertTop *= alpha;
  vertBottom *= alpha;

  shape->coverageT[0] = (horzLeft * vertTop) >> 16;
  shape->coverageT[1] = (vertTop) >> 8;
  shape->coverageT[2] = (horzRight * vertTop) >> 16;

  shape->coverageI[0] = (horzLeft * alpha) >> 8;
  shape->coverageI[1] = alpha;
  shape->coverageI[2] = (horzRight * alpha) >> 8;

  shape->coverageB[0] = (horzLeft * vertBottom) >> 16;
  shape->coverageB[1] = (vertBottom) >> 8;
  shape->coverageB[2] = (horzRight * vertBottom) >> 16;

  self->_shape = RASTERIZER_SHAPE_RECT;
}

static const uint8_t _RasterizerT_boxCommands[5] =
{
  PATH_CMD_MOVE_TO,
  PATH_CMD_LINE_TO,
  PATH_CMD_LINE_TO,
  PATH_CMD_LINE_TO,
  PATH_CMD_CLOSE
};

template<typename SrcT>
static void _Rasterizer8_addBox(Rasterizer8* self, const SrcT_(Box)& box)
{
  if (self->_error != ERR_OK) return;
  FOG_ASSERT(self->_isFinalized == false);

  if (self->_shape == RASTERIZER_SHAPE_NONE)
  {
    // Called first time, initialize the specialized rectangle-only rasterizer.
    BoxI box24x8(UNINITIALIZED);

    // Convert to a fixed point.
    box24x8.x0 = Math::fixed24x8FromFloat(box.x0 + self->_offsetF.x);
    box24x8.y0 = Math::fixed24x8FromFloat(box.y0 + self->_offsetF.y);
    box24x8.x1 = Math::fixed24x8FromFloat(box.x1 + self->_offsetF.x);
    box24x8.y1 = Math::fixed24x8FromFloat(box.y1 + self->_offsetF.y);

    _Rasterizer8_addBox24x8(self, box24x8);
  }
  else
  {
    // Called more times, the generic path/polygon rasterizer will be used.
    if (self->_shape != RASTERIZER_SHAPE_PATH)
      self->switchToPath();

    SrcT_(Point) vertices[5];

    vertices[0].set(box.x0, box.y0);
    vertices[1].set(box.x1, box.y0);
    vertices[2].set(box.x1, box.y1);
    vertices[3].set(box.x0, box.y1);
    vertices[4].set(SrcT(0.0), SrcT(0.0));

    if (_Rasterizer8_useCellD(self))
      _Rasterizer_addPathData<SrcT, Rasterizer8::ChunkD, Rasterizer8::CellD>(self, vertices, _RasterizerT_boxCommands, 5);
    else
      _Rasterizer_addPathData<SrcT, Rasterizer8::ChunkQ, Rasterizer8::CellQ>(self, vertices, _RasterizerT_boxCommands, 5);
  }
}

void Rasterizer8::addRect(const RectF& rect) { _Rasterizer8_addBox<float >(this, BoxF(rect)); }
void Rasterizer8::addRect(const RectD& rect) { _Rasterizer8_addBox<double>(this, BoxD(rect)); }

void Rasterizer8::addBox(const BoxF& box) { _Rasterizer8_addBox<float >(this, box); }
void Rasterizer8::addBox(const BoxD& box) { _Rasterizer8_addBox<double>(this, box); }

// ============================================================================
// [Fog::Rasterizer8 - Path - Switch To Path]
// ============================================================================

bool Rasterizer8::switchToPath()
{
  switch (_shape)
  {
    case RASTERIZER_SHAPE_NONE:
    {
      // If the rasterizer is not initialized, initialize it.
      if (!_Rasterizer8_initCells(this)) return false;

      _shape = RASTERIZER_SHAPE_PATH;
      return true;
    }

    case RASTERIZER_SHAPE_PATH:
    {
      return true;
    }

    case RASTERIZER_SHAPE_RECT:
    {
      ShapeRect* shape = reinterpret_cast<ShapeRect*>(_rows);
      BoxI box = shape->box24x8;

      if (!_Rasterizer8_initCells(this))
      {
        _shape = RASTERIZER_SHAPE_NONE;
        return false;
      }

      _shape = RASTERIZER_SHAPE_PATH;

      if (_Rasterizer8_useCellD(this))
      {
        _Rasterizer8_renderLine<ChunkD, CellD>(this, box.x0, box.y0, box.x1, box.y0);
        _Rasterizer8_renderLine<ChunkD, CellD>(this, box.x1, box.y0, box.x1, box.y1);
        _Rasterizer8_renderLine<ChunkD, CellD>(this, box.x1, box.y1, box.x0, box.y1);
        _Rasterizer8_renderLine<ChunkD, CellD>(this, box.x0, box.y1, box.x0, box.y0);
      }
      else
      {
        _Rasterizer8_renderLine<ChunkQ, CellQ>(this, box.x0, box.y0, box.x1, box.y0);
        _Rasterizer8_renderLine<ChunkQ, CellQ>(this, box.x1, box.y0, box.x1, box.y1);
        _Rasterizer8_renderLine<ChunkQ, CellQ>(this, box.x1, box.y1, box.x0, box.y1);
        _Rasterizer8_renderLine<ChunkQ, CellQ>(this, box.x0, box.y1, box.x0, box.y0);
      }

      return true;
    }

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return false;
}

// ============================================================================
// [Fog::Rasterizer8 - Path - Chunk Storage]
// ============================================================================

bool Rasterizer8::getNextChunkStorage(size_t chunkSize)
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
// [Fog::Rasterizer8 - Finalize]
// ============================================================================

err_t Rasterizer8::finalize()
{
  // If already finalized this is the NOP.
  if (_error != ERR_OK || _isFinalized) return _error;

  switch (_shape)
  {
    case RASTERIZER_SHAPE_NONE:
    {
      goto _NotValid;
    }

    case RASTERIZER_SHAPE_PATH:
    {
      if (_boundingBox.y0 == -1) goto _NotValid;

      _Rasterizer8_Path_initSweepFunctions(this);
      break;
    }

    case RASTERIZER_SHAPE_RECT:
    {
      ShapeRect* shape = reinterpret_cast<ShapeRect*>(_rows);
      _boundingBox.setBox(shape->bounds);

      _Rasterizer8_Rect_initSweepFunctions(this);
      break;
    }
  }

  // Translate bounding box to match _sceneBox.
  _boundingBox.translate(_sceneBox.x0, _sceneBox.y0);
  // Normalize bounding box to our standard, x1/y1 coordinates are outside.
  _boundingBox.x1++;
  _boundingBox.y1++;

  // Small tolerance here due to the floating point arithmetic. Sweep scanline
  // never return anything beyond the bounding box.
  FOG_ASSERT(_boundingBox.x1 <= _sceneBox.x1 + 1);
  FOG_ASSERT(_boundingBox.y1 <= _sceneBox.y1 + 1);

  if (_boundingBox.x1 > _sceneBox.x1) _boundingBox.x1 = _sceneBox.x1;
  if (_boundingBox.y1 > _sceneBox.y1) _boundingBox.y1 = _sceneBox.y1;

  // We added small tolerance to the rasterizer because of floating point based
  // clipping, but if there is no bounding box (due to correction above) then
  // there is no shape.
  _isValid = (_boundingBox.x0 != _boundingBox.x1 &&
              _boundingBox.y0 != _boundingBox.y1);
  _isFinalized = true;
  return ERR_OK;

_NotValid:
  _isValid = false;
  _isFinalized = true;
  return ERR_OK;
}

// ============================================================================
// [Fog::Rasterizer8 - Path - Render]
// ============================================================================

#define NEW_CHUNK(CHUNK_TYPE, _Bail, _Dst_) \
  do { \
    if (FOG_UNLIKELY(self->_current->getChunkPtr() == self->_current->getChunkEnd()) && \
                     !self->getNextChunkStorage(CHUNK_TYPE::CHUNK_SIZE)) \
    { \
      goto _Bail; \
    } \
    \
    _Dst_ = (CHUNK_TYPE*)self->_current->_chunkPtr; \
    self->_current->_chunkPtr += CHUNK_TYPE::CHUNK_SIZE; \
  } while (0)

#define ADD_CHUNK(CHUNK_TYPE, _Bail, _Y_, _Dst_) \
  do { \
    NEW_CHUNK(CHUNK_TYPE, _Bail, _Dst_); \
    _Dst_->_prev = reinterpret_cast<uint8_t*>(self->_rows[_Y_]); \
    self->_rows[_Y_] = reinterpret_cast<uint8_t*>(_Dst_); \
  } while (0)

#define GET_CHUNK(CHUNK_TYPE, _Bail, _Y_, _Dst_) \
  do { \
    _Dst_ = reinterpret_cast<CHUNK_TYPE*>(self->_rows[_Y_]); \
    if (FOG_UNLIKELY(_Dst_->isFull())) \
    { \
      NEW_CHUNK(CHUNK_TYPE, _Bail, _Dst_); \
      _Dst_->_prev = reinterpret_cast<uint8_t*>(self->_rows[_Y_]); \
      self->_rows[_Y_] = reinterpret_cast<uint8_t*>(_Dst_); \
    } \
  } while (0)

#define ADD_CELL_SINGLE(CHUNK_TYPE, _Bail, _Y_, _X_, _Cover_, _Weight_) \
  do { \
    CHUNK_TYPE* _chunk; \
    GET_CHUNK(CHUNK_TYPE, _Bail, _Y_, _chunk); \
    \
    _chunk->getCells()[_chunk->getCount()].setData(_X_, _Cover_, _Weight_); \
    _chunk->incCount(1); \
  } while (0)

template<typename _CHUNK_TYPE, typename _CELL_TYPE>
FOG_INLINE bool _Rasterizer8_renderHLine(Rasterizer8* self, int ey, Fixed24x8 x0, Fixed24x8 y0, Fixed24x8 x1, Fixed24x8 y1)
{
  int ex0;
  int ex1;
  int fx0;
  int fx1;

  int delta, p, dx;
  int lift, mod, rem;

  // Trivial case. Happens often.
  if (y0 == y1) return true;

  ex0 = x0 >> A8_POLY_SUBPIXEL_SHIFT;
  ex1 = x1 >> A8_POLY_SUBPIXEL_SHIFT;

  fx0 = x0 & A8_POLY_SUBPIXEL_MASK;
  fx1 = x1 & A8_POLY_SUBPIXEL_MASK;

  _CHUNK_TYPE* chunk;
  size_t index;

  GET_CHUNK(_CHUNK_TYPE, _Bail, ey, chunk);
  index = chunk->getCount();

  // Everything is located in a single cell.
  if (ex0 == ex1)
  {
    delta = y1 - y0;
    chunk->getCells()[index].setData(ex0, delta, fx0 + fx1);
    chunk->incCount(1);
    return true;
  }

  // Ok, we'll have to render a run of adjacent cells on the same hline...
  dx = x1 - x0;
  if (dx < 0)
  {
    dx    = -dx;

    p     = (y1 - y0) * fx0;
    delta = p / dx;
    mod   = p % dx;

    if (mod < 0) { mod += dx; delta--; }

    chunk->getCells()[index++].setData(ex0, delta, fx0);

    ex0--;
    y0 += delta;

    if (ex0 != ex1)
    {
      p     = (y1 - y0 + delta) * A8_POLY_SUBPIXEL_SCALE;
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
            ADD_CHUNK(_CHUNK_TYPE, _Bail, ey, chunk);
            index = 0;
          }
          chunk->getCells()[index++].setData(ex0, delta, A8_POLY_SUBPIXEL_SCALE);
        }

        y0 += delta;
      } while (--ex0 != ex1);
    }

    delta = y1 - y0;
    if (FOG_UNLIKELY(index == _CHUNK_TYPE::CELLS_COUNT))
    {
      chunk->setCount(_CHUNK_TYPE::CELLS_COUNT);
      ADD_CHUNK(_CHUNK_TYPE, _Bail, ey, chunk);
      index = 0;
    }

    chunk->getCells()[index++].setData(ex0, delta, fx1 + A8_POLY_SUBPIXEL_SCALE);
    chunk->setCount(index);
    return true;
  }
  else
  {
    p     = (y1 - y0) * (A8_POLY_SUBPIXEL_SCALE - fx0);
    delta = p / dx;
    mod   = p % dx;

    if (mod < 0) { mod += dx; delta--; }

    chunk->getCells()[index++].setData(ex0, delta, fx0 + A8_POLY_SUBPIXEL_SCALE);

    ex0++;
    y0 += delta;

    if (ex0 != ex1)
    {
      p     = (y1 - y0 + delta) * A8_POLY_SUBPIXEL_SCALE;
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
            ADD_CHUNK(_CHUNK_TYPE, _Bail, ey, chunk);
            index = 0;
          }
          chunk->getCells()[index++].setData(ex0, delta, A8_POLY_SUBPIXEL_SCALE);
        }

        y0 += delta;
      } while (++ex0 != ex1);
    }

    delta = y1 - y0;
    if (FOG_UNLIKELY(index == _CHUNK_TYPE::CELLS_COUNT))
    {
      chunk->setCount(_CHUNK_TYPE::CELLS_COUNT);
      ADD_CHUNK(_CHUNK_TYPE, _Bail, ey, chunk);
      index = 0;
    }

    chunk->getCells()[index++].setData(ex0, delta, fx1);
    chunk->setCount(index);
    return true;
  }

_Bail:
  return false;
}

template<typename _CHUNK_TYPE, typename _CELL_TYPE>
static bool _Rasterizer8_renderLine(Rasterizer8* self, Fixed24x8 x0, Fixed24x8 y0, Fixed24x8 x1, Fixed24x8 y1)
{
  enum DXLimitEnum { DXLimit = 16384 << A8_POLY_SUBPIXEL_SHIFT };

  int dx = x1 - x0;

  if (FOG_UNLIKELY((dx >= DXLimit) | (dx <= -DXLimit)))
  {
    int cx = (x0 + x1) >> 1;
    int cy = (y0 + y1) >> 1;

    if (!_Rasterizer8_renderLine<_CHUNK_TYPE, _CELL_TYPE>(self, x0, y0, cx, cy)) return false;
    if (!_Rasterizer8_renderLine<_CHUNK_TYPE, _CELL_TYPE>(self, cx, cy, x1, y1)) return false;

    return true;
  }

  int dy = y1 - y0;

  int ex0 = x0 >> A8_POLY_SUBPIXEL_SHIFT;
  int ex1 = x1 >> A8_POLY_SUBPIXEL_SHIFT;
  int ey0 = y0 >> A8_POLY_SUBPIXEL_SHIFT;
  int ey1 = y1 >> A8_POLY_SUBPIXEL_SHIFT;
  int fy0 = y0 & A8_POLY_SUBPIXEL_MASK;
  int fy1 = y1 & A8_POLY_SUBPIXEL_MASK;

  int x_from, x_to;
  int p, rem, mod, lift, delta, first, incr;

  // Initialize bounding box if this is the first call to renderLine().
  if (FOG_UNLIKELY(self->_boundingBox.y0 == -1))
  {
    self->_boundingBox.x0 = ex0;
    self->_boundingBox.y0 = ey0;
    self->_boundingBox.x1 = ex0;
    self->_boundingBox.y1 = ey0;

    _CHUNK_TYPE* _chunk;
    NEW_CHUNK(_CHUNK_TYPE, _Bail, _chunk);

    _chunk->_prev = NULL;

    FOG_ASSERT(ey0 >= 0 && (uint)ey0 < self->_rowsCapacity);
    self->_rows[ey0] = _chunk;
  }

  if (ex0 < ex1)
  {
    if (ex0 < self->_boundingBox.x0) self->_boundingBox.x0 = ex0;
    if (ex1 > self->_boundingBox.x1) self->_boundingBox.x1 = ex1;
  }
  else
  {
    if (ex1 < self->_boundingBox.x0) self->_boundingBox.x0 = ex1;
    if (ex0 > self->_boundingBox.x1) self->_boundingBox.x1 = ex0;
  }

  FOG_ASSERT(ex0 >= self->_sceneBox.x0 && ey0 >= self->_sceneBox.y0 && ex0 <= self->_sceneBox.x1 && ey0 <= self->_sceneBox.y1);
  FOG_ASSERT(ex1 >= self->_sceneBox.x0 && ey1 >= self->_sceneBox.y0 && ex1 <= self->_sceneBox.x1 && ey1 <= self->_sceneBox.y1);

#define PURGE_ROWS(CHUNK_TYPE, start, count) \
  do { \
    CHUNK_TYPE** _p = (CHUNK_TYPE**)(&self->_rows[start]); \
    uint _i = start, _c = start + count; \
    \
    do { \
      CHUNK_TYPE* _chunk; \
      NEW_CHUNK(CHUNK_TYPE, _Bail, _chunk); \
      \
      _chunk->_prev = NULL; \
      \
      FOG_ASSERT(_i >= 0 && _i < self->_rowsCapacity); \
      self->_rows[_i] = _chunk; \
    } while (++_i < _c); \
  } while(0)

  if (ey0 < ey1)
  {
    if (ey0 < self->_boundingBox.y0)
    {
      PURGE_ROWS(_CHUNK_TYPE, ey0, self->_boundingBox.y0 - ey0);
      self->_boundingBox.y0 = ey0;
    }
    if (ey1 > self->_boundingBox.y1)
    {
      PURGE_ROWS(_CHUNK_TYPE, self->_boundingBox.y1 + 1, ey1 - self->_boundingBox.y1);
      self->_boundingBox.y1 = ey1;
    }
  }
  else
  {
    if (ey1 < self->_boundingBox.y0)
    {
      PURGE_ROWS(_CHUNK_TYPE, ey1, self->_boundingBox.y0 - ey1);
      self->_boundingBox.y0 = ey1;
    }
    if (ey0 > self->_boundingBox.y1)
    {
      PURGE_ROWS(_CHUNK_TYPE, self->_boundingBox.y1 + 1, ey0 - self->_boundingBox.y1);
      self->_boundingBox.y1 = ey0;
    }
  }
#undef PURGE_ROWS

  // Everything is on a single hline.
  if (ey0 == ey1)
  {
    return _Rasterizer8_renderHLine<_CHUNK_TYPE, _CELL_TYPE>(self, ey0, x0, fy0, x1, fy1);
  }

  // Vertical line - we have to calculate start and end cells, and then - the
  // common values of the area and coverage for all cells of the line. We know
  // exactly there's only one cell, so, we don't have to call renderHLine().
  incr = 1;
  if (dx == 0)
  {
    int two_fx = (x0 - (ex0 << A8_POLY_SUBPIXEL_SHIFT)) << 1;

    first = A8_POLY_SUBPIXEL_SCALE;
    if (dy < 0) { first = 0; incr = -1; }

    delta = first - fy0;
    ADD_CELL_SINGLE(_CHUNK_TYPE, _Bail, ey0, ex0, delta, two_fx);

    ey0 += incr;
    delta = first + first - A8_POLY_SUBPIXEL_SCALE;

    while (ey0 != ey1)
    {
      ADD_CELL_SINGLE(_CHUNK_TYPE, _Bail, ey0, ex0, delta, two_fx);
      ey0 += incr;
    }

    delta = first + fy1 - A8_POLY_SUBPIXEL_SCALE;
    ADD_CELL_SINGLE(_CHUNK_TYPE, _Bail, ey0, ex0, delta, two_fx);
    return true;
  }

  // Ok, we have to render several hlines.
  p = (A8_POLY_SUBPIXEL_SCALE - fy0) * dx;
  first = A8_POLY_SUBPIXEL_SCALE;

  if (dy < 0)
  {
    p     = fy0 * dx;
    first = 0;
    incr  = -1;
    dy    = -dy;
  }

  delta = p / dy;
  mod   = p % dy;

  if (mod < 0) { delta--; mod += dy; }

  x_from = x0 + delta;
  if (!_Rasterizer8_renderHLine<_CHUNK_TYPE, _CELL_TYPE>(self, ey0, x0, fy0, x_from, first))
    goto _Bail;
  ey0 += incr;

  if (ey0 != ey1)
  {
    p     = dx * A8_POLY_SUBPIXEL_SCALE;
    lift  = p / dy;
    rem   = p % dy;

    if (rem < 0) { lift--; rem += dy; }
    mod -= dy;

    while (ey0 != ey1)
    {
      delta = lift;
      mod  += rem;
      if (mod >= 0) { mod -= dy; delta++; }

      x_to = x_from + delta;
      if (!_Rasterizer8_renderHLine<_CHUNK_TYPE, _CELL_TYPE>(self, ey0, x_from, A8_POLY_SUBPIXEL_SCALE - first, x_to, first))
        goto _Bail;
      x_from = x_to;

      ey0 += incr;
    }
  }

  return _Rasterizer8_renderHLine<_CHUNK_TYPE, _CELL_TYPE>(self, ey0, x_from, A8_POLY_SUBPIXEL_SCALE - first, x1, fy1);

_Bail:
  return false;
}

// ============================================================================
// [Fog::Rasterizer8 - Path - QSort]
// ============================================================================

template <typename CELL>
static FOG_INLINE void _Rasterizer8_swapCells(CELL* a, CELL* b)
{
  CELL temp = *a;
  *a = *b;
  *b = temp;
}

template<typename CELL>
static FOG_INLINE void _Rasterizer8_qsortCells(CELL* start, size_t num)
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
    size_t len = size_t(limit - base);

    CELL* i;
    CELL* j;
    CELL* pivot;

    if (len > RASTERIZER_QSORT_THRESHOLD)
    {
      // We use base + len/2 as the pivot.
      pivot = base + len / 2;
      _Rasterizer8_swapCells(base, pivot);

      i = base + 1;
      j = limit - 1;

      // Now ensure that *i <= *base <= *j .
      if (j->getComparable() < i->getComparable()) _Rasterizer8_swapCells(i, j);
      if (base->getComparable() < i->getComparable()) _Rasterizer8_swapCells(base, i);
      if (j->getComparable() < base->getComparable()) _Rasterizer8_swapCells(base, j);

      for (;;)
      {
        uint32_t c = base->getComparable();
        do { i++; } while (i->getComparable() < c);
        do { j--; } while (c < j->getComparable());

        if (i > j) break;
        _Rasterizer8_swapCells(i, j);
      }

      _Rasterizer8_swapCells(base, j);

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
        for (; j[0].getComparable() > j[1].getComparable(); j--)
        {
          _Rasterizer8_swapCells(j + 1, j);
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
// [Fog::Rasterizer8 - Path - Sweep - Helpers]
// ============================================================================

// Formula:
//
// Cover = ?
// Weight = ?
// Area = Cover * Weight;
//
// FIRST:
//   Alpha = abs( ((Cover << (POLY_SUBPIXEL_SHIFT + 1)) - Area) >> (POLY_SUBPIXEL_SHIFT * 2 + 1 - A8_SHIFT) );
//   Alpha = abs( ((Cover << 9) - Area) >> 9);
//
// SPAN:
//   Alpha = abs( ((Cover << (POLY_SUBPIXEL_SHIFT + 1))       ) >> (POLY_SUBPIXEL_SHIFT * 2 + 1 - A8_SHIFT) );
//   Alpha = abs( Cover );
//
// NON_ZERO:
//   if (Alpha > A8_MASK) Alpha = A8_MASK;
//
// EVEN_ODD:
//   Alpha &= A8_MASK_2;
//   if (Alpha > A8_SCALE) Alpha = A8_SCALE_2 - cover;
//   if (Alpha > A8_MASK) Alpha = A8_MASK;
template<int _RULE, int _USE_ALPHA>
static FOG_INLINE uint32_t _Rasterizer8_calculateAlpha(const Rasterizer8* rasterizer, int area)
{
  int cover = area >> (A8_POLY_SUBPIXEL_SHIFT*2 + 1 - A8_SHIFT);
  if (cover < 0) cover = -cover;

  if (_RULE == FILL_RULE_NON_ZERO)
  {
    if (cover > A8_SCALE) cover = A8_SCALE;
  }
  else
  {
    cover &= A8_MASK_2;
    if (cover > A8_SCALE) cover = A8_SCALE_2 - cover;
  }

  if (_USE_ALPHA) cover = (cover * rasterizer->_alpha) >> 8;
  return cover;
}

template<typename _CHUNK_TYPE, typename _CELL_TYPE>
static size_t _Rasterizer8_mergeCells(Rasterizer8* rasterizer, void* _chunks, MemoryBuffer& temp, _CELL_TYPE** cellsOut)
{
  _CHUNK_TYPE* chunk = reinterpret_cast<_CHUNK_TYPE*>(_chunks);
  _CELL_TYPE* cellCur = chunk->getCells();

  size_t numCells = chunk->getCount();
  if (numCells == 0) return 0;

  {
    // First calculate count of cells needed to alloc.
    _CHUNK_TYPE* chunkCur = chunk->getPrev();
    while (chunkCur)
    {
      numCells += chunkCur->getCount();
      chunkCur = chunkCur->getPrev();
    };

    _CELL_TYPE* buf = reinterpret_cast<_CELL_TYPE*>(temp.alloc((size_t)numCells * sizeof(_CELL_TYPE)));
    if (FOG_IS_NULL(buf)) return false;

    chunkCur = chunk;
    do {
      cellCur = chunkCur->getCells();
      size_t i = chunkCur->getCount();

      // Copy cells reversed if they are in reversed order.
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
  _Rasterizer8_qsortCells<_CELL_TYPE>(cellCur, numCells);

  *cellsOut = cellCur;
  return numCells;
}

// ============================================================================
// [Fog::Rasterizer8 - Path - Sweep - Simple]
// ============================================================================

#define CELL_DECLARE() \
  _CELL_TYPE* cellCur; \
  size_t numCells = _Rasterizer8_mergeCells<_CHUNK_TYPE, _CELL_TYPE>(rasterizer, rasterizer->_rows[y], temp, &cellCur); \
  \
  if (numCells == 0) \
    return NULL; \
  \
  if (scanline.begin(rasterizer->_boundingBox.x0, rasterizer->_boundingBox.x1) != ERR_OK) \
    return NULL; \
  \
  int xOffset = rasterizer->_sceneBox.x0; \
  int x; \
  int nextX = cellCur->getX() + xOffset; \
  int area; \
  int cover = 0; \
  int coversh

#define CELL_FETCH() \
  FOG_MACRO_BEGIN \
    x      = nextX; \
    area   = cellCur->getArea(); \
    cover += cellCur->getCover(); \
    \
    while (--numCells) \
    { \
      cellCur++; \
      \
      nextX = cellCur->getX() + xOffset; \
      FOG_ASSERT(nextX >= x); \
      if (nextX != x) break; \
      \
      area  += cellCur->getArea(); \
      cover += cellCur->getCover(); \
    } \
    \
    coversh = cover << (A8_POLY_SUBPIXEL_SHIFT + 1); \
  FOG_MACRO_END

template<typename _CHUNK_TYPE, typename _CELL_TYPE, int _RULE, int _USE_ALPHA>
static Span8* _Rasterizer8_sweepSimpleImpl(
  Rasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y)
{
  y -= rasterizer->_sceneBox.y0;

  FOG_ASSERT(rasterizer->_isFinalized);
  if ((uint)y >= (uint)rasterizer->_size.h) return NULL;

  // --------------------------------------------------------------------------
  // [Cells]
  // --------------------------------------------------------------------------

  CELL_DECLARE();

  // --------------------------------------------------------------------------
  // [Main-Loop]
  // --------------------------------------------------------------------------

  for (;;)
  {
    CELL_FETCH();

    // ------------------------------------------------------------------------
    // [H-Line]
    // ------------------------------------------------------------------------

    if (x + 1 == nextX)
    {
      // Skip this cell if resulting mask is zero.
      uint32_t alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area);
      if (alpha == 0) continue;

      // Okay, it seems that we will now generate some masks, embedded to one
      // span instance.
      scanline.lnkA8Extra(x);
      scanline.valA8Extra(alpha);

      for (;;)
      {
        CELL_FETCH();
        alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area);

        if (++x == nextX)
        {
          scanline.valA8Extra(alpha);
          continue;
        }
        else
        {
          break;
        }
      }

      if (area != 0)
      {
        if (alpha != 0x00) scanline.valA8Extra(alpha);
      }
      else
      {
        x--;
      }

      scanline.endA8Extra();
      if (numCells == 0) break;
    }

    // ------------------------------------------------------------------------
    // [V-Line]
    // ------------------------------------------------------------------------

    else
    {
      if (area != 0)
      {
        uint32_t alpha = alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area);
        if (alpha != 0)
        {
          scanline.lnkA8Extra(x);
          scanline.valA8Extra(alpha);
          scanline.endA8Extra(x + 1);
        }
        x++;
      }
      if (numCells == 0) break;
    }

    // ------------------------------------------------------------------------
    // [Area]
    // ------------------------------------------------------------------------

    {
      uint32_t alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh);
      if (alpha) scanline.lnkConstSpanOrMerge(x, nextX, alpha);
    }
  }

  // --------------------------------------------------------------------------
  // [End]
  // --------------------------------------------------------------------------

  if (scanline.close() != ERR_OK) return NULL;
#if defined(FOG_DEBUG_RASTERIZER)
  _Rasterizer_dumpSpans(y, scanline.getSpans());
#endif // FOG_DEBUG_RASTERIZER
  return scanline.getSpans();
}

// ============================================================================
// [Fog::Rasterizer8 - Path - Sweep - Region]
// ============================================================================

template<typename _CHUNK_TYPE, typename _CELL_TYPE, int _RULE, int _USE_ALPHA>
static Span8* _Rasterizer8_sweepRegionImpl(
  Rasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y,
  const BoxI* clipBoxes, size_t count)
{
  if (!count) return NULL;
  y -= rasterizer->_sceneBox.y0;

  FOG_ASSERT(rasterizer->_isFinalized);
  if ((uint)y >= (uint)rasterizer->_size.h) return NULL;

  // --------------------------------------------------------------------------
  // [Cells]
  // --------------------------------------------------------------------------

  CELL_DECLARE();

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  const BoxI* clipCur = clipBoxes;
  const BoxI* clipEnd = clipBoxes + count;

  int clipX0;                         // Clip span start.
  int clipX1 = clipCur->x1;           // Clip span end (not part of clip span).

  // Advance clip (discard clip-boxes that can't intersect).
  while (clipX1 <= nextX)
  {
_AdvanceClip:
    if (++clipCur == clipEnd) goto _End;
    clipX1 = clipCur->x1;
  }

  clipX0 = clipCur->x0;
  FOG_ASSERT(nextX < clipX1);

  // --------------------------------------------------------------------------
  // [Main-Loop]
  // --------------------------------------------------------------------------

  for (;;)
  {
    CELL_FETCH();

    // ------------------------------------------------------------------------
    // [H-Line]
    // ------------------------------------------------------------------------

    if (x + 1 == nextX)
    {
      if (x < clipX0) continue;

      // Skip this cell if resulting mask is zero.
      uint32_t alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area);
      if (alpha == 0) continue;

      // Okay, it seems that we will now generate some masks, embedded to one
      // span instance.
      scanline.lnkA8Extra(x);
      scanline.valA8Extra(alpha);

      if (clipX1 <= nextX)
      {
        scanline.endA8Extra(nextX);
        goto _AdvanceClip;
      }

      for (;;)
      {
        CELL_FETCH();
        alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area);

        if (++x == nextX)
        {
          scanline.valA8Extra(alpha);
          if (clipX1 <= nextX)
          {
            scanline.endA8Extra(x);
            goto _AdvanceClip;
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
        if (alpha != 0x00) scanline.valA8Extra(alpha);
      }
      else
      {
        x--;
      }

      if (numCells == 0) break;
    }

    // ------------------------------------------------------------------------
    // [V-Line]
    // ------------------------------------------------------------------------

    else
    {
      uint32_t alpha;
      if (area != 0)
      {
        if (x >= clipX0 && (alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area)) != 0)
        {
          scanline.lnkA8Extra(x);
          scanline.valA8Extra(alpha);
          scanline.endA8Extra(x + 1);
        }
        x++;
      }
      if (numCells == 0) break;
    }

    // ------------------------------------------------------------------------
    // [Area]
    // ------------------------------------------------------------------------

    {
      if (x >= clipX1)
      {
        if (++clipCur == clipEnd) goto _End;
        clipX0 = clipCur->x0;
        clipX1 = clipCur->x1;
        FOG_ASSERT(x < clipX1);
      }

      uint32_t alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh);
      if (alpha && nextX > clipX0)
      {
        if (x < clipX0) x = clipX0;

        if (x < clipX1)
        {
          FOG_ASSERT(x < clipX1);
          int toX = Math::min<int>(nextX, clipX1);
          scanline.lnkConstSpanOrMerge(x, toX, alpha);
          x = toX;
        }

        if (nextX > clipX1)
        {
          for (;;)
          {
            if (++clipCur == clipEnd) goto _End;
            clipX0 = clipCur->x0;
            clipX1 = clipCur->x1;
            if (nextX > clipX0)
            {
              scanline.lnkConstSpan(clipX0, Math::min<int>(nextX, clipX1), alpha);
              if (nextX >= clipX1) continue;
            }
            break;
          }
        }
      }

      if (nextX >= clipX1) goto _AdvanceClip;
    }
  }

  // --------------------------------------------------------------------------
  // [End]
  // --------------------------------------------------------------------------

_End:
  if (scanline.close() != ERR_OK) return NULL;
#if defined(FOG_DEBUG_RASTERIZER)
  _Rasterizer_dumpSpans(y, scanline.getSpans());
#endif // FOG_DEBUG_RASTERIZER
  return scanline.getSpans();
}

// ============================================================================
// [Fog::Rasterizer8 - Path - Sweep - Spans]
// ============================================================================

template<typename _CHUNK_TYPE, typename _CELL_TYPE, int _RULE, int _USE_ALPHA>
static Span8* _Rasterizer8_sweepSpansImpl(
  Rasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y,
  const Span8* clipSpans)
{
  if (FOG_UNLIKELY(clipSpans == NULL)) return NULL;
  y -= rasterizer->_sceneBox.y0;

  FOG_ASSERT(rasterizer->_isFinalized);
  if ((uint)y >= (uint)rasterizer->_size.h) return NULL;

  // --------------------------------------------------------------------------
  // [Cells]
  // --------------------------------------------------------------------------

  CELL_DECLARE();

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  const Span8* clipCur = clipSpans;   // Clip span instance.
  int clipX0;                         // Clip span start.
  int clipX1 = clipCur->getX1();      // Clip span end (not part of clip span).
  uint32_t clipType;                  // Clip span type.
  const uint8_t* clipMask;            // Clip span mask ptr (or const-value).

#define CLIP_SPAN_CHANGED() \
  FOG_MACRO_BEGIN \
    clipX0 = clipCur->getX0(); \
    FOG_ASSERT(clipX0 < clipX1); \
    \
    clipType = clipCur->getType(); \
    clipMask = reinterpret_cast<uint8_t*>(clipCur->getGenericMask()); \
    clipMask -= Span8::getMaskAdvance(clipType, clipX0); \
  FOG_MACRO_END

  // --------------------------------------------------------------------------
  // [Prepare]
  // --------------------------------------------------------------------------

  // Advance clip (discard clip-boxes that can't intersect).
  while (clipX1 <= nextX)
  {
_AdvanceClip:
    if ((clipCur = clipCur->getNext()) == NULL) goto _End;
    clipX1 = clipCur->getX1();
  }

  CLIP_SPAN_CHANGED();
  FOG_ASSERT(nextX < clipX1);

  // --------------------------------------------------------------------------
  // [Main-Loop]
  // --------------------------------------------------------------------------

_Continue:
  for (;;)
  {
    CELL_FETCH();
    FOG_ASSERT(x < clipX1);

    // ------------------------------------------------------------------------
    // [H-Line]
    // ------------------------------------------------------------------------

    if (x + 1 == nextX)
    {
      if (x < clipX0) continue;

      // Skip this cell if resulting mask is zero.
      uint32_t alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area);
      if (alpha == 0)
      {
        if (clipX1 <= nextX) goto _AdvanceClip;
        continue;
      }

      switch (clipType)
      {
        case SPAN_C:
        {
          Face::p32 m = Span8::getConstMaskFromPointer(clipMask);
          Face::p32MulDiv256SBW(alpha, alpha, m);

          if (alpha == 0x00)
          {
            if (clipX1 <= nextX) goto _AdvanceClip;
            goto _Continue;
          }

          scanline.lnkA8Extra(x);
          scanline.valA8Extra(alpha);
          if (clipX1 <= nextX) { scanline.endA8Extra(nextX); goto _AdvanceClip; }

          for (;;)
          {
            CELL_FETCH();
            FOG_ASSERT(x >= clipX0 && x < clipX1);

            alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area);
            Face::p32MulDiv256SBW(alpha, alpha, m);

            if (++x != nextX) break;

            scanline.valA8Extra(alpha);
            if (clipX1 <= nextX)
            {
              scanline.endA8Extra(x);
              goto _AdvanceClip;
            }
          }

          if (area != 0)
          {
            if (alpha != 0) scanline.valA8Extra(alpha);
          }
          else
          {
            x--;
          }

          scanline.endA8Extra();
          break;
        }

        case SPAN_A8_GLYPH:
        case SPAN_AX_GLYPH:
        {
          Face::p32 m;

          Face::p32Load1b(m, clipMask + x);
          Face::p32MulDiv256SBW(m, m, alpha);

          if (m == 0x00)
          {
            if (clipX1 <= nextX) goto _AdvanceClip;
            goto _Continue;
          }

          scanline.lnkA8Glyph(x);
          scanline.valA8Glyph(m);
          if (clipX1 <= nextX) { scanline.endA8Glyph(nextX); goto _AdvanceClip; }

          for (;;)
          {
            CELL_FETCH();
            FOG_ASSERT(x >= clipX0 && x < clipX1);

            alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area);
            Face::p32Load1b(m, clipMask + x);
            Face::p32MulDiv256SBW(m, m, alpha);

            if (++x == nextX)
            {
              scanline.valA8Glyph(m);
              if (clipX1 <= nextX) { scanline.endA8Glyph(x); goto _AdvanceClip; }
            }
            else
            {
              break;
            }
          }

          if (area != 0)
          {
            if (m != 0x00) scanline.valA8Glyph(m);
          }
          else
          {
            x--;
          }

          scanline.endA8Glyph();
          break;
        }

        case SPAN_AX_EXTRA:
        {
          Face::p32 m;

          Face::p32Load2aNative(m, clipMask + x * 2);
          Face::p32MulDiv256SBW(m, m, alpha);

          if (m == 0x00)
          {
            if (clipX1 <= nextX) goto _AdvanceClip;
            goto _Continue;
          }

          scanline.lnkA8Extra(x);
          scanline.valA8Extra(m);
          if (clipX1 <= nextX) { scanline.endA8Extra(nextX); goto _AdvanceClip; }

          for (;;)
          {
            CELL_FETCH();
            FOG_ASSERT(x >= clipX0 && x < clipX1);

            alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area);
            Face::p32Load2aNative(m, clipMask + x * 2);
            Face::p32MulDiv256SBW(m, m, alpha);

            if (++x == nextX)
            {
              scanline.valA8Extra(m);
              if (clipX1 <= nextX) { scanline.endA8Extra(x); goto _AdvanceClip; }
            }
            else
            {
              break;
            }
          }

          if (area != 0)
          {
            if (m != 0x00) scanline.valA8Extra(m);
          }
          else
          {
            x--;
          }

          scanline.endA8Extra();
          break;
        }

        case SPAN_ARGB32_GLYPH:
        case SPAN_ARGBXX_GLYPH:
        {
          Face::p32 m;

          Face::p32Load4aNative(m, clipMask + x * 4);
          Face::p32MulDiv256PBB_SBW(m, m, alpha);

          if (m == 0x00000000)
          {
            if (clipX1 <= nextX) goto _AdvanceClip;
            goto _Continue;
          }

          scanline.lnkARGB32Glyph(x);
          scanline.valARGB32Glyph(m);
          if (clipX1 <= nextX) { scanline.endARGB32Glyph(nextX); goto _AdvanceClip; }

          for (;;)
          {
            CELL_FETCH();
            FOG_ASSERT(x >= clipX0 && x < clipX1);

            alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area);
            Face::p32Load4aNative(m, clipMask + x * 4);
            Face::p32MulDiv256PBB_SBW(m, m, alpha);

            if (++x == nextX)
            {
              scanline.valARGB32Glyph(m);
              if (clipX1 <= nextX) { scanline.endARGB32Glyph(x); goto _AdvanceClip; }
            }
            else
            {
              break;
            }
          }

          if (area != 0)
          {
            if (m != 0x00000000) scanline.valARGB32Glyph(m);
          }
          else
          {
            x--;
          }

          scanline.endARGB32Glyph();
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }

    // ------------------------------------------------------------------------
    // [V-Line]
    // ------------------------------------------------------------------------

    else
    {
      uint32_t alpha;

      if (area != 0)
      {
        if (x >= clipX0 && (alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh - area)) != 0)
        {
          FOG_ASSERT(x >= clipX0 && x < clipX1);
          Face::p32 m;

          switch (clipType)
          {
            case SPAN_C:
              m = Span8::getConstMaskFromPointer(clipMask);
              goto _VLine_AxExtra;

            case SPAN_A8_GLYPH:
            case SPAN_AX_GLYPH:
              Face::p32Load1b(m, clipMask + x);
              Face::p32MulDiv256SBW(m, m, alpha);

              if (m != 0x00)
              {
                scanline.lnkA8Glyph(x);
                scanline.valA8Glyph(m);
                scanline.endA8Glyph(x + 1);
              }
              break;

            case SPAN_AX_EXTRA:
              Face::p32Load4aNative(m, clipMask + x * 2);
_VLine_AxExtra:
              Face::p32MulDiv256SBW(m, m, alpha);

              if (m != 0x00)
              {
                scanline.lnkA8Extra(x);
                scanline.valA8Extra(m);
                scanline.endA8Extra(x + 1);
              }
              break;

            case SPAN_ARGB32_GLYPH:
            case SPAN_ARGBXX_GLYPH:
              Face::p32Load4aNative(m, clipMask + x * 4);
              Face::p32MulDiv256PBB_SBW(m, m, alpha);

              if (m != 0x00000000)
              {
                scanline.lnkARGB32Glyph(x);
                scanline.valARGB32Glyph(m);
                scanline.endARGB32Glyph(x + 1);
              }
              break;

            default:
              FOG_ASSERT_NOT_REACHED();
          }
        }
        x++;
      }
    }

    if (numCells == 0) break;

    // ------------------------------------------------------------------------
    // [Area]
    // ------------------------------------------------------------------------

    if (nextX > clipX0)
    {
      if (clipX1 <= x)
      {
        if ((clipCur = clipCur->getNext()) == NULL) goto _End;
        clipX1 = clipCur->getX1();
        CLIP_SPAN_CHANGED();
        FOG_ASSERT(x < clipX1);
      }

      uint32_t alpha = _Rasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(rasterizer, coversh);

      if (alpha)
      {
        if (x < clipX0) x = clipX0;

        if (x < clipX1)
        {
          int toX = Math::min<int>(nextX, clipX1);

          FOG_ASSERT(x < clipX1);
          FOG_ASSERT(x < toX);

          switch (clipType)
          {
            case SPAN_C:
            {
              Face::p32 m = Span8::getConstMaskFromPointer(clipMask);
              Face::p32MulDiv256SBW(m, m, alpha);
              if (m != 0x00) scanline.lnkConstSpanOrMerge(x, toX, m);
              break;
            }

            case SPAN_A8_GLYPH:
            case SPAN_AX_GLYPH:
            {
              // TODO:
              /*
              _g2d_render.mask[MASK_OP_INTERSECT][IMAGE_FORMAT_A8].v_op_c(
                scanline.lnkA8Extra_buf(x, toX),  // Destination.
                clipMask + x,                     // Source A.
                alpha,                            // Source B.
                toX - x);                         // Length.
              */
              break;
            }

            case SPAN_AX_EXTRA:
            {
              // TODO:
              break;
            }

            case SPAN_ARGB32_GLYPH:
            case SPAN_ARGBXX_GLYPH:
            {
              // TODO:
              break;
            }

            default:
              FOG_ASSERT_NOT_REACHED();
          }

          x = toX;
        }

        if (nextX > clipX1)
        {
          for (;;)
          {
            if ((clipCur = clipCur->getNext()) == NULL) goto _End;

            clipX1 = clipCur->getX1();
            CLIP_SPAN_CHANGED();

            if (nextX > clipX0)
            {
              int toX = Math::min<int>(nextX, clipX1);
              FOG_ASSERT(clipX0 < toX);

              switch (clipType)
              {
                case SPAN_C:
                {
                  Face::p32 m = Span8::getConstMaskFromPointer(clipMask);
                  Face::p32MulDiv256SBW(m, m, alpha);
                  if (m != 0x00) scanline.lnkConstSpanOrMerge(clipX0, toX, m);
                  break;
                }

                case SPAN_A8_GLYPH:
                case SPAN_AX_GLYPH:
                {
                  // TODO:
                  /*
                  _g2d_render.mask[MASK_OP_INTERSECT][IMAGE_FORMAT_A8].v_op_c(
                    scanline.lnkA8Extra_buf(clipX0, toX),  // Destination.
                    clipMask + clipX0,                     // Source A.
                    alpha,                                 // Source B.
                    toX - clipX0);                         // Length.
                  */
                  break;
                }

                case SPAN_AX_EXTRA:
                {
                  // TODO:
                  break;
                }

                case SPAN_ARGB32_GLYPH:
                case SPAN_ARGBXX_GLYPH:
                {
                  // TODO:
                  break;
                }

                default:
                  FOG_ASSERT_NOT_REACHED();
              }
              if (clipX1 <= nextX) continue;
            }
            break;
          }
        }
      }

      if (clipX1 <= nextX) goto _AdvanceClip;
    }
  }

  // --------------------------------------------------------------------------
  // [End]
  // --------------------------------------------------------------------------

_End:
  if (scanline.close() != ERR_OK) return NULL;
#if defined(FOG_DEBUG_RASTERIZER)
  _Rasterizer_dumpSpans(y, scanline.getSpans());
#endif // FOG_DEBUG_RASTERIZER
  return scanline.getSpans();

  // --------------------------------------------------------------------------
  // [Cleanup]
  // --------------------------------------------------------------------------

#undef CLIP_SPAN_CHANGED
}

#undef CELL_DECLARE
#undef CELL_FETCH

static void _Rasterizer8_Path_initSweepFunctions(Rasterizer8* rasterizer)
{
#define SETUP_SWEEP(rasterizer, _FILL_MODE, _USE_ALPHA) \
  FOG_MACRO_BEGIN \
    if (_Rasterizer8_useCellD(rasterizer)) \
    { \
      rasterizer->_sweepSimpleFn = _Rasterizer8_sweepSimpleImpl<Rasterizer8::ChunkD, Rasterizer8::CellD, _FILL_MODE, _USE_ALPHA>; \
      rasterizer->_sweepRegionFn = _Rasterizer8_sweepRegionImpl<Rasterizer8::ChunkD, Rasterizer8::CellD, _FILL_MODE, _USE_ALPHA>; \
      rasterizer->_sweepSpansFn  = _Rasterizer8_sweepSpansImpl <Rasterizer8::ChunkD, Rasterizer8::CellD, _FILL_MODE, _USE_ALPHA>; \
    } \
    else \
    { \
      rasterizer->_sweepSimpleFn = _Rasterizer8_sweepSimpleImpl<Rasterizer8::ChunkQ, Rasterizer8::CellQ, _FILL_MODE, _USE_ALPHA>; \
      rasterizer->_sweepRegionFn = _Rasterizer8_sweepRegionImpl<Rasterizer8::ChunkQ, Rasterizer8::CellQ, _FILL_MODE, _USE_ALPHA>; \
      rasterizer->_sweepSpansFn  = _Rasterizer8_sweepSpansImpl <Rasterizer8::ChunkQ, Rasterizer8::CellQ, _FILL_MODE, _USE_ALPHA>; \
    } \
  FOG_MACRO_END

  // Setup sweep scanline methods.
  switch (rasterizer->_fillRule)
  {
    case FILL_RULE_NON_ZERO:
      if (rasterizer->_alpha == 0x100)
        SETUP_SWEEP(rasterizer, FILL_RULE_NON_ZERO, 0);
      else
        SETUP_SWEEP(rasterizer, FILL_RULE_NON_ZERO, 1);
      break;

    case FILL_RULE_EVEN_ODD:
      if (rasterizer->_alpha == 0x100)
        SETUP_SWEEP(rasterizer, FILL_RULE_EVEN_ODD, 0);
      else
        SETUP_SWEEP(rasterizer, FILL_RULE_EVEN_ODD, 1);
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

} // Fog namespace
