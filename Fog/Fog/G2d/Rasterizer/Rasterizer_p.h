// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RASTERIZER_RASTERIZER_P_H
#define _FOG_G2D_RASTERIZER_RASTERIZER_P_H

// [Dependencies]
#include <Fog/Core/Math/Fixed.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Rasterizer/Scanline_p.h>
#include <Fog/G2d/Rasterizer/Span_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Rasterizer
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct MemoryBuffer;

// --------------------------------------------------------------------------
// [Fog::RASTERIZER_SHAPE]
// --------------------------------------------------------------------------

//! @brief Rasterizer shape.
//!
//! Analytic rasterizer contains fast-paths which may be used to rasterizer
//! specific shapes. The common, the most universal shape, which can be used
//! by any path-data is @c SHAPE_TYPE_PATH. All other shapes are specific to
//! the input data.
//!
//! @note All shapes are filled.
enum RASTERIZER_SHAPE
{
  //! @brief Initial state.
  RASTERIZER_SHAPE_NONE = 0,

  //! @brief Path (universal).
  RASTERIZER_SHAPE_PATH = 1,

  //! @brief Rectangle, only subpixel translation.
  //!
  //! The data is stored in @c RectShape structure, allocated by @c CellStorage.
  RASTERIZER_SHAPE_RECT = 2,

  // TODO: Not implemented, convert to SHAPE_LINE

  //! @brief Rectangle after affine transform, may be used also for
  //! BUTT or SQUARE lines.
  //!
  //! The data is stored in @c LineShape structure, allocated by @c CellStorage.
  RASTERIZER_SHAPE_LINE = 3
};

// ============================================================================
// [Fog::Rasterizer8]
// ============================================================================

//! @internal
//!
//! Scanline polygon rasterizer.
//!
//! Polygon rasterizer that is used to render filled polygons with 
//! high-quality Anti-Aliasing. Internally, by default, the class uses 
//! integer coordinates in format 24.8, i.e. 24 bits for integer part 
//! and 8 bits for fractional. This class can be used in the following way:
//!
//! 1. Reset/setup rasterizer:
//!
//!    reset() - If rasterizer is reused, reset ensures that all data related
//!    to previous rasterization are lost. It is the requirement.
//!
//!    @c getFillRule() / @c setFillRule(int fillRule) - Fill rule management.
//!    @c getAlpha() / @c setAlpha() - Alpha management.
//!    @c getClipBox() / @c setSceneBox() - Set clip box management.
//!
//!    You can omit initialization if you are reusing rasterizer and these
//!    members are already set to demanded values. These values are never
//!    changed when using reset(), @c initialize() or @c finalize().
//!
//! 2. Initialize rasterizer:
//!
//!    @c initialize() - Init is method that will initialize the rasterizer.
//!    After calling this method you cannot call setup methods (in ideal case
//!    setup will do nothing, in worst case your application can crash or call
//!    assertion failure in debug-more).
//!
//! 3. Call commands:
//!
//!    @c addPath(path) - Make the polygon. One can create more than one contour,
//!    but each contour must consist of at least 3 vertices, is the absolute
//!    minimum of vertices that define a triangle. The algorithm does not check
//!    either the number of vertices nor coincidence of their coordinates, but
//!    in the worst case it just won't draw anything.
//!
//!    The orger of the vertices (clockwise or counterclockwise) 
//!    is important when using the non-zero filling rule (@c FILL_RULE_NON_ZERO).
//!    In this case the vertex order of all the contours must be the same
//!    if you want your intersecting polygons to be without "holes".
//!    You actually can use different vertices order. If the contours do not 
//!    intersect each other the order is not important anyway. If they do, 
//!    contours with the same vertex order will be rendered without "holes" 
//!    while the intersecting contours with different orders will have "holes".
//!
//! 4. Finalize rasterizer:
//!
//!    @c finalize() - Finalize the shape. Finalize step is rasterizer dependent
//!    and may or may not rasterize or pre-rasterize some areas.
//!
//! 5. Sweep scanlines.
//!
//!    @c sweepScanline() - Sweep scanline to scanline container. This scanline
//!    can be passed to renderer or clipper.
//!
//! Analytic rasterizer can be used in multi-threaded environment. The 
//! sweepScanline() method is thread-safe and is normally called by more
//! threads if multi-threaded rendering is active.
//!
//! Analytic rasterizer with 8-bit precision cell members:
//!
//!   X     - a horizontal pixel position in pixel units. Should be relative
//!           to horizontal clipping bounding box.
//!   Cover - a value at interval -256 to 256. This means that 10-bits are
//!           enough to store the value.
//!   Area  - a value at interval -(511<<8) to (511<<8). This value is Cover
//!           multiplied by Weight in range (0-511). This member can be 
//!           effectively compressed into 9-bits by using only the weight
//!           and doing multiplication in sweepScanline(). This is how Fog
//!           rasterizer works!
//!
//! This means that cell structure can be compressed into:
//!
//!   DWORD (32-bit):
//!   - X     - 13 bits.
//!   - Cover - 10 bits.
//!   - Area  - 9  bits.
//!   Applicable for resolution up to 8192 pixels in horizontal.
//!
//!   QWORD (64-bit):
//!   - X     - 32 bits.
//!   - Cover - 16 bits.
//!   - Area  - 16 bits.
//!   Applicable for any resolution (X is 32-bit integer).
//!
//! NOTE: Cell area value is always stored in compact format. This means that
//! if you need the real number you need to shift it to left by 8-bits.
//!
//!
//! The analytic rasterizer idea and first implementation was based on 
//! AntiGrain, which is based on freetype2, which is based on libart?
//! Here is the original license:
//!
//! @verbatim
//! Anti-Grain Geometry - Version 2.4
//! Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//!
//! Permission to copy, use, modify, sell and distribute this software 
//! is granted provided this copyright notice appears in all copies. 
//! This software is provided "as is" without express or implied
//! warranty, and with no claim as to its suitability for any purpose.
//! @endverbatim
struct FOG_NO_EXPORT Rasterizer8
{
  // --------------------------------------------------------------------------
  // [Cell Storage]
  // --------------------------------------------------------------------------

  //! @internal
  //!
  //! @brief Cell storage, contains chunks for thousands of cells.
  struct FOG_NO_EXPORT CellStorage
  {
    enum
    {
      //! @brief Size of @c CellStorage buffer, including members.
      STORAGE_SIZE = 32768 - 80
    };

    //! @brief Setup this cell storage, providing the data and chunk size.
    //! @param storageSize Size of storage within CellStorage data.
    //! @param chunkSize Size of one chunk - 64-bytes or 128-bytes.
    FOG_INLINE void setup(sysuint_t storageSize, sysuint_t chunkSize)
    {
      _storageSize = storageSize;

      _dataBuffer = (uint8_t*)(((sysuint_t)this + sizeof(CellStorage) + 63) & ~(sysuint_t)63);
      _dataSize = storageSize - ( (sysuint_t)this - (sysuint_t)_dataBuffer );

      sysuint_t numBytes = (sysuint_t)this + storageSize - (sysuint_t)_dataBuffer;

      _chunkPtr = _dataBuffer;
      _chunkEnd = _dataBuffer + numBytes / chunkSize * chunkSize;//(uint8_t*)(((sysuint_t)this + storageSize) & ~(_chunkSize-1));
      _chunkSize = chunkSize;
    }

    //! @brief Get the previous cell storage.
    FOG_INLINE CellStorage* getPrev() const { return _prev; }
    //! @brief Get the next cell storage.
    FOG_INLINE CellStorage* getNext() const { return _next; }

    //! @brief Get the storage size.
    FOG_INLINE sysuint_t getStorageSize() const { return _storageSize; }

    //! @brief Get the data pointer.
    FOG_INLINE uint8_t* getDataBuffer() const { return _dataBuffer; }
    //! @brief Get the data size.
    FOG_INLINE sysuint_t getDataSize() const { return _dataSize; }

    //! @brief Get the current chunk pointer.
    FOG_INLINE uint8_t* getChunkPtr() const { return _chunkPtr; }
    //! @brief Get the end chunk pointer (the first invalid chunk).
    FOG_INLINE uint8_t* getChunkEnd() const { return _chunkEnd; }
    //! @brief Get the chunk size.
    FOG_INLINE sysuint_t getChunkSize() const { return _chunkSize; }

    //! @brief Previous storage (fully used ones, can be NULL).
    CellStorage* _prev;
    //! @brief Next storage (non-NULL only if rasterizer is reused).
    CellStorage* _next;

    //! @brief Size of the whole cell storage including structure members and
    //! cell chunks.
    sysuint_t _storageSize;

    //! @brief Cell data aligned to 64-bytes. The data are always allocated
    //! together with @c Storage structure.
    uint8_t* _dataBuffer;
    //! @brief Cell data size.
    sysuint_t _dataSize;

    //! @brief Current chunk pointer.
    uint8_t* _chunkPtr;
    //! @brief End chunk pointer.
    uint8_t* _chunkEnd;
    //! @brief Chunk size.
    sysuint_t _chunkSize;
  };

  // --------------------------------------------------------------------------
  // [CellD]
  // --------------------------------------------------------------------------

#include <Fog/Core/Pack/PackByte.h>
  //! @internal
  //!
  //! @brief Compact version of cell that fits into DWORD (32-bit integer).
  struct FOG_NO_EXPORT CellD
  {
    enum
    {
      //! @brief Maximum x position in @c Rasterizer8::CellD (12-bit, 8191).
      MAX_X = 0x00001FFF
    };

    //! @brief Set all cell values at once.
    FOG_INLINE void setData(int x, int cover, int weight)
    {
      FOG_ASSERT(x >= 0 && x <= MAX_X);
      FOG_ASSERT(cover >= -256 && cover <= 256);
      FOG_ASSERT(weight >= 0 && weight <= 511);

      _combined = ( (((uint)x     )         ) << 19 )
                + ( (((uint)cover ) & 0x3FFU) <<  9 )
                + ( (((uint)weight)         )       );
    }

    //! @brief Set all cell values to @a data.
    FOG_INLINE void setData(const CellD& data)
    {
      _combined = data._combined;
    }

    //! @brief Get the x coordinate.
    FOG_INLINE int getX() const { return (int)(_combined >> 19); }
    //! @brief Get the cell cover.
    FOG_INLINE int getCover() const { return ((int)(_combined << 13)) >> 22; }
    //! @brief Get the cell weight.
    FOG_INLINE int getWeight() const { return (int)(_combined & 0x1FF); }
    //! @brief Get the cell area (cover * weight).
    FOG_INLINE int getArea() const { return getCover() * getWeight(); }

    //! @brief Get comparable value (for sorting).
    FOG_INLINE uint32_t getComparable() const { return _combined; }

    //! @brief X, cover and area packed in a DWORD.
    uint32_t _combined;
  };
#include <Fog/Core/Pack/PackRestore.h>

  //! @brief Small chunk of @c CellD instances.
  struct FOG_NO_EXPORT ChunkD
  {
    enum
    {
      //! @brief Size of this chunk.
      CHUNK_SIZE = 128,
      //! @brief Count of cells in this chunk.
      CELLS_COUNT = (CHUNK_SIZE - sizeof(void*)) / sizeof(CellD)
    };

    //! @brief Get previous cell chunks.
    FOG_INLINE ChunkD* getPrev() const { return (ChunkD*)( (sysint_t)_prev & (sysint_t)-64 ); }

    //! @brief Get count of cells in this chunk.
    FOG_INLINE sysuint_t getCount() const { return (sysuint_t)_prev & 63; }
    //! @brief Get count of available cells in this chunk.
    FOG_INLINE sysuint_t getAvail() const { return CELLS_COUNT - getCount(); }

    //! @brief Get whether this chunk is full.
    FOG_INLINE bool isFull() const { return getCount() == CELLS_COUNT; }

    //! @brief Get cells array.
    FOG_INLINE CellD* getCells() { return _cells; }
    //! @overload
    FOG_INLINE const CellD* getCells() const { return _cells; }

    //! @brief Increment cells counter.
    FOG_INLINE void incCount(sysuint_t count)
    {
      FOG_ASSERT(getCount() + count <= CELLS_COUNT);
      _prev += count;
    }

    //! @brief Set cells counter to @a count.
    FOG_INLINE void setCount(sysuint_t count)
    {
      FOG_ASSERT(count <= CELLS_COUNT);
      _prev = (uint8_t*)((sysuint_t)_prev & ~(sysuint_t)63) + count;
    }

    //! @brief Link to prevous cells.
    //!
    //! @note _prev contains pointer and cells counter. The pointer is always
    //! allocated using 64-bytes alignment so there are 6 bits for counter.
    uint8_t* _prev;

    //! @brief Cells.
    CellD _cells[CELLS_COUNT];
  };

  // --------------------------------------------------------------------------
  // [CellQ]
  // --------------------------------------------------------------------------

#include <Fog/Core/Pack/PackByte.h>
  //! @internal
  //!
  //! @brief Full version of cell that fits into QWORD (64-bit integer).
  struct FOG_NO_EXPORT CellQ
  {
    enum
    {
      //! @brief Maximum x position in @c Rasterizer8::CellQ (32-bit).
      MAX_X = 0xFFFFFFFF
    };

    //! @brief Set all cell values at once.
    FOG_INLINE void setData(int x, int cover, int weight)
    {
      FOG_ASSERT(x >= 0);
      FOG_ASSERT(cover >= -256 && cover <= 256);
      FOG_ASSERT(weight >= 0 && weight <= 511);

      _x = (uint32_t)x;
      _cover = (int16_t)(cover);
      _weight = (uint16_t)(uint)(weight);
    }

    //! @brief Set all cell values to @a data.
    FOG_INLINE void setData(const CellQ& data)
    {
      _combined = data._combined;
    }

    //! @brief Get x coordinate.
    FOG_INLINE int getX() const { return (int)_x; }
    //! @brief Get cell cover.
    FOG_INLINE int getCover() const { return (int)_cover; }
    //! @brief Get cell weight.
    FOG_INLINE int getWeight() const { return (int)_weight; }
    //! @brief Get cell area.
    FOG_INLINE int getArea() const { return getCover() * getWeight(); }

    //! @brief Get comparable value (for sorting).
    FOG_INLINE uint32_t getComparable() const { return _x; }

    union
    {
      struct
      {
        //! @brief X coordinate in full-precision.
        uint32_t _x;
        //! @brief Cover value.
        int16_t _cover;
        //! @brief Area value.
        uint16_t _weight;
      };

      //! @brief X, cover and area packed in a QWORD.
      uint64_t _combined;
    };
  };
#include <Fog/Core/Pack/PackRestore.h>

  //! @brief Small chunk of @c CellQ instances.
  struct FOG_NO_EXPORT ChunkQ
  {
    enum
    {
      //! @brief Size of this chunk.
      CHUNK_SIZE = 128,
      //! @brief Count of cells in this chunk.
      CELLS_COUNT = (CHUNK_SIZE - sizeof(void*)) / sizeof(CellQ)
    };

    //! @brief Get previous cell chunks.
    FOG_INLINE ChunkQ* getPrev() const { return (ChunkQ*)( (sysint_t)_prev & (sysint_t)-64 ); }

    //! @brief Get count of cells in this chunk.
    FOG_INLINE sysuint_t getCount() const { return (sysuint_t)_prev & 63; }
    //! @brief Get count of available cells in this chunk.
    FOG_INLINE sysuint_t getAvail() const { return CELLS_COUNT - getCount(); }

    //! @brief Get whether this chunk is full.
    FOG_INLINE bool isFull() const { return getCount() == CELLS_COUNT; }

    //! @brief Get cells array.
    FOG_INLINE CellQ* getCells() { return _cells; }
    //! @overload
    FOG_INLINE const CellQ* getCells() const { return _cells; }

    //! @brief Increment cells counter.
    FOG_INLINE void incCount(sysuint_t count)
    {
      FOG_ASSERT(getCount() + count <= CELLS_COUNT);
      _prev += count;
    }

    //! @brief Set cells counter to @a count.
    FOG_INLINE void setCount(sysuint_t count)
    {
      FOG_ASSERT(count <= CELLS_COUNT);
      _prev = (uint8_t*)((sysuint_t)_prev & ~(sysuint_t)63) + count;
    }

    //! @brief Link to prevous cells.
    //!
    //! @note _prev contains pointer and cells counter. The pointer is always
    //! allocated using 64-bytes alignment so there are 6 bits for counter.
    uint8_t* _prev;

#if FOG_ARCH_BITS == 32
    uint32_t _padding;
#endif

    //! @brief Cells.
    CellQ _cells[CELLS_COUNT];
  };

  //! @brief Structure that holds data related to @c SHAPE_TYPE_SUBPX_RECTANGLE.
  struct FOG_NO_EXPORT RectShape
  {
    //! @brief Bounding box.
    BoxI bounds;
    //! @brief Fixed point version of rectange passed to the addBox()/addRect() method.
    BoxI box24x8;

    //! @brief For fetching, x-left/right, including clipBox offset.
    int xLeft, xRight;

    //! @brief Top-coverage values.
    uint32_t coverageT[4];
    //! @brief Inner-coverage values.
    uint32_t coverageI[4];
    //! @brief Bottom-coverage values.
    uint32_t coverageB[4];
  };

  //! @brief Structure that holds data related to @c SHAPE_TYPE_AFFINE_RECTANGLE.
  struct FOG_NO_EXPORT LineShape
  {
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a @ Rasterizer8 instance.
  Rasterizer8();
  //! @brief Destroy the @ Rasterizer8 instance.
  ~Rasterizer8();

  // --------------------------------------------------------------------------
  // [Clip Box]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxI& getSceneBox() const { return _sceneBox; }
  FOG_INLINE void setSceneBox(const BoxI& sceneBox) { _sceneBox = sceneBox; }

  // --------------------------------------------------------------------------
  // [Bounding Box]
  // --------------------------------------------------------------------------

  //! @brief Get rasterized object bounding box.
  //!
  //! @note This method is only valid after @c finalize() call.
  FOG_INLINE const BoxI& getBoundingBox() const { return _boundingBox; }

  // --------------------------------------------------------------------------
  // [Error]
  // --------------------------------------------------------------------------

  //! @brief Get the rasterizer error.
  FOG_INLINE err_t getError() const { return _error; }
  //! @brief Set the rasterizer error.
  FOG_INLINE void setError(err_t error) { _error = error; }

  // --------------------------------------------------------------------------
  // [Alpha]
  // --------------------------------------------------------------------------

  //! @brief Get the rasterizer alpha (0...256)
  FOG_INLINE uint32_t getAlpha() const { return _alpha; }
  //! @brief Set the rasterizer alpha (0...256)
  FOG_INLINE void setAlpha(uint32_t alpha) { _alpha = alpha; }

  // --------------------------------------------------------------------------
  // [Fill Rule]
  // --------------------------------------------------------------------------

  //! @brief Set the rasterizer fill-rule.
  FOG_INLINE uint32_t getFillRule() const { return _fillRule; }
  //! @brief Get the rasterizer fill-rule.
  FOG_INLINE void setFillRule(uint32_t fillRule) { _fillRule = (uint8_t)fillRule; }

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  //! @brief Get whether the rasterizer is valid.
  FOG_INLINE uint8_t isValid() const { return _isValid; }
  //! @brief Get whether the rasterizer is finalized.
  FOG_INLINE uint8_t isFinalized() const { return _isFinalized; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @brief Reset rasterizer.
  void reset();

  // --------------------------------------------------------------------------
  // [Initialize]
  // --------------------------------------------------------------------------

  //! @brief Initialize the rasterizer, called after setup methods and before
  //! adding path(s).
  //!
  //! During setup the methods like setAlpha(), setFillRule() can be called,
  //! after calling initialize() these methods shouldn't be called, because
  //! rasterizer can use different algorithms to perform rasterization (even-odd
  //! vs. non-zero fill rule, alpha, quality, etc...).
  err_t initialize();

  // --------------------------------------------------------------------------
  // [Finalize]
  // --------------------------------------------------------------------------

  //! @brief Finalize, called after one or more @c addPath() commands.
  err_t finalize();

  // --------------------------------------------------------------------------
  // [Commands - Rect/Box]
  // --------------------------------------------------------------------------

  //! @brief Add rectangle to the rasterizer.
  void addRect(const RectF& rect);
  //! @overload
  void addRect(const RectD& rect);

  //! @brief Add box to the rasterizer.
  void addBox(const BoxF& box);
  //! @overload
  void addBox(const BoxD& box);

  //! @internal
  void _addBox24x8(const BoxI& box);

  // --------------------------------------------------------------------------
  // [Commands - Path]
  // --------------------------------------------------------------------------

  //! @brief Add path to the rasterizer.
  void addPath(const PathF& path);
  //! @overload
  void addPath(const PathD& path);

  // --------------------------------------------------------------------------
  // [Commands - Switch To Path]
  // --------------------------------------------------------------------------

  //! @brief Convert any shape that is being rasterized to the @c SHAPE_TYPE_PATH.
  void switchToPath();

  // --------------------------------------------------------------------------
  // [Cache]
  // --------------------------------------------------------------------------

  //! @internal
  bool getNextChunkStorage(sysuint_t chunkSize);

  // --------------------------------------------------------------------------
  // [Renderer]
  // --------------------------------------------------------------------------

  template<typename _CHUNK_TYPE, typename _CELL_TYPE>
  bool renderLine(Fixed24x8 x1, Fixed24x8 y1, Fixed24x8 x2, Fixed24x8 y2);

  template<typename _CHUNK_TYPE, typename _CELL_TYPE>
  FOG_INLINE bool renderHLine(int ey, Fixed24x8 x1, Fixed24x8 y1, Fixed24x8 x2, Fixed24x8 y2);

  // --------------------------------------------------------------------------
  // [Sweep]
  // --------------------------------------------------------------------------

  typedef Span8* (*SweepScanlineSimpleFn)(Rasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y);
  typedef Span8* (*SweepScanlineRegionFn)(Rasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y, const BoxI* clipBoxes, sysuint_t count);
  typedef Span8* (*SweepScanlineSpansFn)(Rasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y, const Span8* clipSpans);

  // --------------------------------------------------------------------------
  // [Cells / Rows]
  // --------------------------------------------------------------------------

  //! @brief Sweep scanline @a y.
  FOG_INLINE Span8* sweepScanline(Scanline8& scanline, MemoryBuffer& temp, int y)
  { return _sweepScanlineSimpleFn(this, scanline, temp, y); }

  //! @brief Enhanced version of @c sweepScanline() that accepts clip region.
  //!
  //! This method is called by raster paint engine if clipping region is complex.
  FOG_INLINE Span8* sweepScanline(Scanline8& scanline, MemoryBuffer& temp, int y, const BoxI* clipBoxes, sysuint_t count)
  { return _sweepScanlineRegionFn(this, scanline, temp, y, clipBoxes, count); }

  //! @brief Enhanced version of @c sweepScanline() that accepts clip spans.
  //!
  //! This method is called by raster paint engine if clipping region is mask.
  FOG_INLINE Span8* sweepScanline(Scanline8& scanline, MemoryBuffer& temp, int y, const Span8* clipSpans)
  { return _sweepScanlineSpansFn(this, scanline, temp, y, clipSpans); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The raterizer offset (per thread, used by multithreaded painter).
  int _offset;
  //! @brief The rasterizer delta (per thread, used by multithreaded painter).
  int _delta;

  //! @brief Clip box (always must be valid, initialy set to zero -> no paint).
  BoxI _sceneBox;
  //! @brief bounding box of a rasterized object.
  BoxI _boundingBox;

  //! @brief Maximum size of rasterized shape, generated by @c initialize() and
  //! calculated using @c _clipBox.getWidth() and @c _clipBox.getHeight().
  SizeI _size;
  //! @brief Maximum size in 24x8 format.
  SizeI _size24x8;

  //! @brief The vertex translation offset (Fixed24x8).
  //!
  //! This offset simplifies mathematics used by access the @c _rows[] structure
  //! and also guarantees that no point will be negative. If @c _clipBox is
  //! [5, 5, 10, 10] then @c _offset will be set to [-5 (fixed), -5 (fixed)].
  PointI _offset24x8;
  //! @brief The vertex translation offset (float).
  PointF _offsetF;
  //! @brief The vertex translation offset (double).
  PointD _offsetD;

  //! @brief Error code (in case that any error happened it's reported here).
  err_t _error;

  //! @brief Alpha.
  uint32_t _alpha;

  //! @brief Fill rule;
  uint8_t _fillRule;
  //! @brief Shape type.
  uint8_t _shape;

  //! @brief Whether the rasterized object is empty (no-paint).
  uint8_t _isValid;
  //! @brief Whether the rasterizer was finalized.
  uint8_t _isFinalized;

  //! @brief Rows array capacity.
  //!
  //! @note This value is only valid after @c finalize() call.
  uint32_t _rowsCapacity;

  //! @brief Cell chunks per rows. Index of value 0 is index to _clipBox.y1.
  //!
  //! _rows can contain garbage in these indexes:
  //! - _clipBox.y1 -> _boundingBox.y1
  //! - _boundingBox.y2 -> _clipBox.y2
  void** _rows;

  //! @brief Cells storage (begin of the list).
  CellStorage* _storage;
  //! @brief Cells storage (current position in the list).
  CellStorage* _current;

  //! @brief Sweep scanline using box clipping.
  SweepScanlineSimpleFn _sweepScanlineSimpleFn;
  //! @brief Sweep scanline using region clipping.
  SweepScanlineRegionFn _sweepScanlineRegionFn;
  //! @brief Sweep scanline using anti-aliased clipping.
  SweepScanlineSpansFn _sweepScanlineSpansFn;

  //! @brief Temporary path (float).
  PathF _temporaryPathF;
  //! @brief Temporary path (double).
  PathD _temporaryPathD;

private:
  FOG_DISABLE_COPY(Rasterizer8)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RASTERIZER_RASTERIZER_P_H
