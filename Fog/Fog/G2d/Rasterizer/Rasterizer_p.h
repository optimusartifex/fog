// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RASTERIZER_RASTERIZER_P_H
#define _FOG_G2D_RASTERIZER_RASTERIZER_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Rasterizer/Scanline_p.h>
#include <Fog/G2d/Rasterizer/Span_p.h>

// ============================================================================
// [Debugging]
// ============================================================================

// #define FOG_DEBUG_RASTERIZER

#if defined(FOG_DEBUG_RASTERIZER)
#include <Fog/Core/Tools/ByteArray.h>
#endif // FOG_DEBUG_RASTERIZER

namespace Fog {

//! @addtogroup Fog_G2d_Rasterizer
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct MemoryBuffer;
struct Rasterizer8;

// ============================================================================
// [Typedefs]
// ============================================================================

typedef Span8* (*RasterizerSweepSimpleFn)(Rasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y);
typedef Span8* (*RasterizerSweepRegionFn)(Rasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y, const BoxI* clipBoxes, size_t count);
typedef Span8* (*RasterizerSweepSpansFn)(Rasterizer8* rasterizer, Scanline8& scanline, MemoryBuffer& temp, int y, const Span8* clipSpans);

// ============================================================================
// [Fog::RASTERIZER_SHAPE]
// ============================================================================

//! @brief Rasterizer fill shape.
//!
//! The rasterizer contains special fast-paths which may be used to rasterize
//! simple shapes. The generic fill-type is @c RASTERIZER_SHAPE_PATH, which
//! can be used to fill any shape (polygon based or complex path). All other
//! fill-types are specific to simple shapes like rect, line, and others...
enum RASTERIZER_SHAPE
{
  //! @brief None (no command used yet).
  RASTERIZER_SHAPE_NONE = 0,

  //! @brief Fill a path (this is the most generic shape, always usable).
  RASTERIZER_SHAPE_PATH = 1,

  //! @brief Fill a rect using subpixel accuracy.
  //!
  //! This kind of rasterizer is used to fill a simple rectangle shape.
  //!
  //! @note The data is stored in @c ShapeRect structure, allocated by
  //! @c CellStorage.
  RASTERIZER_SHAPE_RECT = 2,

  //! @brief Fill a rect XORed by another rect (stroke) using subpixel accuracy.
  //!
  //! This kind of rasterizer is used to fill a stroked rectangle with angular
  //! (miter join) corners.
  //!
  //! @note The data is stored in @c ShapeRectXorRect structure, allocated by
  //! @c CellStorage.
  RASTERIZER_SHAPE_RECT_XOR_RECT = 3
};

// ============================================================================
// [Fog::Rasterizer8]
// ============================================================================

//! @internal
//!
//! @brief Scanline path/polygon rasterizer, which produces 256 levels of gray.
//!
//! Scanline path/polygon rasterizer that is used to render filled polygons
//! using high-quality anti-aliasing. Internally, by default, the class uses
//! integer coordinates in 24.8 fixed point format, i.e. 24 bits for integer
//! part and 8 bits for fractional part.
//!
//! Because the path/polygon rasterization is too generic, there are some
//! specialized fast-paths which can be used to rasterize simple shapes.
//! The common shapes are:
//!
//!   - @c RASTERIZER_SHAPE_NONE - Not initialized.
//!   - @c RASTERIZER_SHAPE_PATH - Path/Polygon rasterization (default)
//!   - @c RASTERIZER_SHAPE_RECT - Used to fill one rectangle.
//!   - @c RASTERIZER_SHAPE_RECT_XOR_RECT - Used to fill a stroked rectangle
//!     (two rectangles, but the second subtracts the first one).
//!
//! The rasterizer is re-usable class, which should be used by the following
//! way:
//!
//!   1. Reset
//!
//!      If the rasterizer is reused, reset ensures that all data  related to
//!      previous rasterization are lost. It is the requirement. To reset the
//!      rasterizer use @c Rasterizer8::reset() method.
//!
//!   2. Setup
//!
//!      After the rasterizer is created or reset, it's needed to specify the
//!      scene-box, fill-rule, and global alpha. Scene-box is required!
//!
//!      The scene-box is similar to clip-box, the exception is that rasterizer
//!      excepts that the path/polygon is already clipped by the underlying
//!      paint-engine, so instead of complicated clipping the coordinates are
//!      simply saturated to the scene-box. This means that if the clipping is
//!      not exact (and believe me, the floating point calculation is not!)
//!      then these small errors are simply corrected, preventing buffer-overrun
//!      problems.
//!
//!      To get/set scene-box use the @c Rasterizer8::getSceneBox() and
//!      @c Rasterizer8::setSceneBox() methods.
//!
//!      The fill-rule describes how the shape is filled. It must be set before
//!      the path/polygon/shape is added to the rasterizer and can't be modified.
//!      The methods @c Rasterizer8::getFillRule() and @c Rasterizer8::setFillRule()
//!      can be used to get/set the fill-rule property.
//!
//!      The last parameter 'alpha' is used to rasterize a semi-transparent
//!      shape. The default 'alpha' value is 256. To control the global alpha
//!      use the @c Rasterizer8::getAlpha() and @c Rasterizer8::setAlpha() methods.
//!
//!   3. Initialize
//!
//!      After the rasterizer parameters are set-up, the rasterizer must be
//!      initialized. The initialization is simply a validation of all
//!      parameters and preparation for adding shapes. To initialize the
//!      rasterizer use @c Rasterizer8::initialize() method.
//!
//!      The initialization step is mandatory!
//!
//!   4. Add shapes (polygons/paths/specials)
//!
//!      After the rasterizer was initialized, the shapes can be added.
//!      Currently the rasterizer supports multi-shape mode, that means that
//!      there is no limitation to shapes combination and count. Simply call
//!      methods which start by 'add', for example @c addPath(), @c addBox(),
//!      etc...
//!
//!      The scanline path/polygon rasterizer requires that the one contour
//!      must consist of at least 3 vertices, is the absolute minimum of
//!      vertices that define a triangle. The algorithm does not check either
//!      the number of vertices nor coincidence of their coordinates, but in
//!      the worst case it just render nothing.
//!
//!      The order of the vertices (clockwise or counterclockwise) is important
//!      when using the non-zero filling rule (@c FILL_RULE_NON_ZERO). In this
//!      case the vertex order of all the contours must be the same if you want
//!      your intersecting polygons to be without "holes". You actually can use
//!      different vertices order. If the contours do not intersect each other
//!      the order is not important anyway. If they do, contours with the same
//!      vertex order will be rendered without "holes" while the intersecting
//!      contours with different orders will have "holes".
//!
//!   5. Finalize
//!
//!      After the shapes were added to the rasterizer using the 'add' methods,
//!      the rasterizer must be finalized. Internally the finalization is step
//!      which depends on the rasterizer shape (special shapes have no
//!      finalization), but rasterizer takes care of that. To finalize the
//!      rasterizer use the @c Rasterizer8::finalize() method.
//!
//!      The finalization step is mandatory!
//!
//!   6. Sweep
//!
//!      The sweep is the last step that is used to fetch the rasterized shape
//!      using the @c Scanline8 container. The sweep method generates a mask,
//!      which is used by the Render pipeline to fill the rasterizer shape.
//!
//!
//!
//! The rasterizer uses the analytic approach to exactly calculate the coverage
//! of the output pixels. It generates CELLs which represent the pixel position
//! (cell position), cover, and area. Pixel position is used to locate the cell,
//! cover and area are used to calculate the final coverage. Notice that when
//! rasterizing shapes which are crossing themselves or near, there are more
//! cells with the same position.
//!
//! One cell contains these members:
//!
//!   X     - a horizontal pixel position in pixel units. Must be relative to
//!           the rasterizer scene-box.
//!   Cover - a value at interval -256 to 256. This means that 10-bits are
//!           enough to store the value.
//!   Area  - a value at interval -(511<<8) to (511<<8). This value is the
//!           Cover multiplied by the Weight in range (0-511). Notice that
//!           instead of storing the result (Area), the rasterizer stores only
//!           the weight, to same memory.
//!
//! The cell structures (compressed) used by the rasterizer:
//!
//!   - DWORD (32-bit):
//!     - X     - 13 bits.
//!     - Cover - 10 bits.
//!     - Area  - 9  bits.
//!
//!     Applicable for resolution up to 8192 pixels in horizontal direction.
//!     In this case the cell structure is only 4-byte long and it's stored
//!     simply as one 32-bit integer. This is the common and the most used
//!     case.
//!
//!   - QWORD (64-bit):
//!     - X     - 32 bits.
//!     - Cover - 16 bits.
//!     - Area  - 16 bits.
//!
//!     Applicable for any resolution (X is a 32-bit integer). This cell was
//!     introduced to support painting in very high-resolution. It's rarely
//!     used.
//!
//! NOTE: The cell 'Area' value is always stored in compact format. This
//! means that to get the final value shifting by 8-bits left is needed.
//!
//! The analytic rasterizer idea and first implementation was based on the
//! AntiGrain geometry, which is based on the freetype2 library. There are
//! some disadvantages which should be fixed in the future.
//!
//! The rasterizer disadvantages:
//!
//!   - The crossing lines are not handled correctly. The maximum error in
//!     this case is 50%. Because the original rasterizer was created to
//!     render the text (which doesn't contain self-intersecting polygons).
//!
//!   - The guys which created AmanithVG library says that the rasterizer
//!     also fails in cases that the input vertices are too close (not
//!     strictly degenerated). I didn't tested the rasterizer for such case.
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
  // [CellStorage]
  // --------------------------------------------------------------------------

  //! @internal
  //!
  //! @brief Cell storage, contains chunks for thousands of cells.
  struct FOG_NO_EXPORT CellStorage
  {
    // ------------------------------------------------------------------------
    // [Constants]
    // ------------------------------------------------------------------------

    enum
    {
      //! @brief Size of @c CellStorage buffer, including members.
      STORAGE_SIZE = 32768 - 80
    };

    // ------------------------------------------------------------------------
    // [Setup]
    // ------------------------------------------------------------------------

    //! @brief Setup this cell storage, providing the data and chunk size.
    //! @param storageSize Size of storage within CellStorage data.
    //! @param chunkSize Size of one chunk - 64-bytes or 128-bytes.
    FOG_INLINE void setup(size_t storageSize, size_t chunkSize)
    {
      _storageSize = storageSize;

      _dataBuffer = (uint8_t*)(((size_t)this + sizeof(CellStorage) + 63) & ~(size_t)63);
      _dataSize = storageSize - ( (size_t)this - (size_t)_dataBuffer );

      size_t numBytes = (size_t)this + storageSize - (size_t)_dataBuffer;

      _chunkPtr = _dataBuffer;
      _chunkEnd = _dataBuffer + numBytes / chunkSize * chunkSize;//(uint8_t*)(((size_t)this + storageSize) & ~(_chunkSize-1));
      _chunkSize = chunkSize;
    }

    // ------------------------------------------------------------------------
    // [Accessors]
    // ------------------------------------------------------------------------

    //! @brief Get the previous cell storage.
    FOG_INLINE CellStorage* getPrev() const { return _prev; }
    //! @brief Get the next cell storage.
    FOG_INLINE CellStorage* getNext() const { return _next; }

    //! @brief Get the storage size.
    FOG_INLINE size_t getStorageSize() const { return _storageSize; }

    //! @brief Get the data pointer.
    FOG_INLINE uint8_t* getDataBuffer() const { return _dataBuffer; }
    //! @brief Get the data size.
    FOG_INLINE size_t getDataSize() const { return _dataSize; }

    //! @brief Get the current chunk pointer.
    FOG_INLINE uint8_t* getChunkPtr() const { return _chunkPtr; }
    //! @brief Get the end chunk pointer (the first invalid chunk).
    FOG_INLINE uint8_t* getChunkEnd() const { return _chunkEnd; }
    //! @brief Get the chunk size.
    FOG_INLINE size_t getChunkSize() const { return _chunkSize; }

    // ------------------------------------------------------------------------
    // [Members]
    // ------------------------------------------------------------------------

    //! @brief Previous storage (fully used ones, can be NULL).
    CellStorage* _prev;
    //! @brief Next storage (non-NULL only if rasterizer is reused).
    CellStorage* _next;

    //! @brief Size of the whole cell storage including structure members and
    //! cell chunks.
    size_t _storageSize;

    //! @brief Cell data aligned to 64-bytes. The data are always allocated
    //! together with @c Storage structure.
    uint8_t* _dataBuffer;
    //! @brief Cell data size.
    size_t _dataSize;

    //! @brief Current chunk pointer.
    uint8_t* _chunkPtr;
    //! @brief End chunk pointer.
    uint8_t* _chunkEnd;
    //! @brief Chunk size.
    size_t _chunkSize;
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
    // ------------------------------------------------------------------------
    // [Constants]
    // ------------------------------------------------------------------------

    enum
    {
      //! @brief Maximum x position in @c Rasterizer8::CellD (12-bit, 8191).
      MAX_X = 0x00001FFF
    };

    // ------------------------------------------------------------------------
    // [Accessors]
    // ------------------------------------------------------------------------

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

    // ------------------------------------------------------------------------
    // [Members]
    // ------------------------------------------------------------------------

    //! @brief X, cover and area packed in a DWORD.
    uint32_t _combined;
  };
#include <Fog/Core/Pack/PackRestore.h>

  //! @brief Small chunk of @c CellD instances.
  struct FOG_NO_EXPORT ChunkD
  {
    // ------------------------------------------------------------------------
    // [Constants]
    // ------------------------------------------------------------------------

    enum
    {
      //! @brief Size of this chunk.
      CHUNK_SIZE = 128,
      //! @brief Count of cells in this chunk.
      CELLS_COUNT = (CHUNK_SIZE - sizeof(void*)) / sizeof(CellD)
    };

    // ------------------------------------------------------------------------
    // [Accessors]
    // ------------------------------------------------------------------------

    //! @brief Get previous cell chunks.
    FOG_INLINE ChunkD* getPrev() const { return (ChunkD*)( (sysint_t)_prev & (sysint_t)-64 ); }

    //! @brief Get count of cells in this chunk.
    FOG_INLINE size_t getCount() const { return (size_t)_prev & 63; }
    //! @brief Get count of available cells in this chunk.
    FOG_INLINE size_t getAvail() const { return CELLS_COUNT - getCount(); }

    //! @brief Get whether this chunk is full.
    FOG_INLINE bool isFull() const { return getCount() == CELLS_COUNT; }

    //! @brief Get cells array.
    FOG_INLINE CellD* getCells() { return _cells; }
    //! @overload
    FOG_INLINE const CellD* getCells() const { return _cells; }

    //! @brief Increment cells counter.
    FOG_INLINE void incCount(size_t count)
    {
      FOG_ASSERT(getCount() + count <= CELLS_COUNT);
      _prev += count;
    }

    //! @brief Set cells counter to @a count.
    FOG_INLINE void setCount(size_t count)
    {
      FOG_ASSERT(count <= CELLS_COUNT);
      _prev = (uint8_t*)((size_t)_prev & ~(size_t)63) + count;
    }

    // ------------------------------------------------------------------------
    // [Members]
    // ------------------------------------------------------------------------

    //! @brief Link to prevous cells.
    //!
    //! @note _prev contains pointer and cells counter. The pointer is always
    //! allocated using 64-byte alignment so there are 6 bits for counter.
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
    // ------------------------------------------------------------------------
    // [Constants]
    // ------------------------------------------------------------------------

    enum
    {
      //! @brief Maximum x position in @c Rasterizer8::CellQ (32-bit).
      MAX_X = 0xFFFFFFFF
    };

    // ------------------------------------------------------------------------
    // [Accessors]
    // ------------------------------------------------------------------------

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

    // ------------------------------------------------------------------------
    // [Members]
    // ------------------------------------------------------------------------

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
    // ------------------------------------------------------------------------
    // [Constants]
    // ------------------------------------------------------------------------

    enum
    {
      //! @brief Size of this chunk.
      CHUNK_SIZE = 128,
      //! @brief Count of cells in this chunk.
      CELLS_COUNT = (CHUNK_SIZE - sizeof(void*)) / sizeof(CellQ)
    };

    // ------------------------------------------------------------------------
    // [Accessors]
    // ------------------------------------------------------------------------

    //! @brief Get previous cell chunks.
    FOG_INLINE ChunkQ* getPrev() const { return (ChunkQ*)( (sysint_t)_prev & (sysint_t)-64 ); }

    //! @brief Get count of cells in this chunk.
    FOG_INLINE size_t getCount() const { return (size_t)_prev & 63; }
    //! @brief Get count of available cells in this chunk.
    FOG_INLINE size_t getAvail() const { return CELLS_COUNT - getCount(); }

    //! @brief Get whether this chunk is full.
    FOG_INLINE bool isFull() const { return getCount() == CELLS_COUNT; }

    //! @brief Get cells array.
    FOG_INLINE CellQ* getCells() { return _cells; }
    //! @overload
    FOG_INLINE const CellQ* getCells() const { return _cells; }

    //! @brief Increment cells counter.
    FOG_INLINE void incCount(size_t count)
    {
      FOG_ASSERT(getCount() + count <= CELLS_COUNT);
      _prev += count;
    }

    //! @brief Set cells counter to @a count.
    FOG_INLINE void setCount(size_t count)
    {
      FOG_ASSERT(count <= CELLS_COUNT);
      _prev = (uint8_t*)((size_t)_prev & ~(size_t)63) + count;
    }

    // ------------------------------------------------------------------------
    // [Members]
    // ------------------------------------------------------------------------

    //! @brief Link to prevous cells.
    //!
    //! @note _prev contains pointer and cells counter. The pointer is always
    //! allocated using 64-byte alignment so there are 6 bits for counter.
    uint8_t* _prev;

#if FOG_ARCH_BITS == 32
    uint32_t _padding;
#endif

    //! @brief Cells.
    CellQ _cells[CELLS_COUNT];
  };

  // --------------------------------------------------------------------------
  // [ShapeRect]
  // --------------------------------------------------------------------------

  //! @brief Structure that holds data related to @c RASTERIZER_SHAPE_RECT.
  struct FOG_NO_EXPORT ShapeRect
  {
    //! @brief Bounding box.
    BoxI bounds;
    //! @brief Fixed point version of rectange passed to the addBox()/addRect() method.
    BoxI box24x8;

    //! @brief For fetching, x-left/right. Screen-box offset is included.
    int xLeft, xRight;

    //! @brief Top-coverage values.
    uint32_t coverageT[4];
    //! @brief Inner-coverage values.
    uint32_t coverageI[4];
    //! @brief Bottom-coverage values.
    uint32_t coverageB[4];
  };

  // --------------------------------------------------------------------------
  // [ShapeLine]
  // --------------------------------------------------------------------------

  //! @brief Structure that holds data related to @c SHAPE_TYPE_AFFINE_RECTANGLE.
  struct FOG_NO_EXPORT ShapeLine
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

  //! @brief Reset.
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
  // [Path - Add]
  // --------------------------------------------------------------------------

  //! @brief Add path to the rasterizer.
  void addPath(const PathF& path);
  //! @overload
  void addPath(const PathD& path);

  // --------------------------------------------------------------------------
  // [Rect/Box - Add]
  // --------------------------------------------------------------------------

  //! @brief Add rectangle to the rasterizer.
  void addRect(const RectF& rect);
  //! @overload
  void addRect(const RectD& rect);

  //! @brief Add box to the rasterizer.
  void addBox(const BoxF& box);
  //! @overload
  void addBox(const BoxD& box);

  // --------------------------------------------------------------------------
  // [Path - Switch To Path]
  // --------------------------------------------------------------------------

  //! @brief Convert any shape that is being rasterized to the @c SHAPE_TYPE_PATH.
  //!
  //! This method is used to switch to scanline polygon rasterizer. It's called
  //! in case that simple shape rasterizer is being used, but another shape is
  //! added to the rasterizer, thus it's not possible to finish rasterization
  //! using the simple-shape rasterizer.
  bool switchToPath();

  // --------------------------------------------------------------------------
  // [Path - Chunk Storage]
  // --------------------------------------------------------------------------

  //! @internal
  bool getNextChunkStorage(size_t chunkSize);

  // --------------------------------------------------------------------------
  // [Finalize]
  // --------------------------------------------------------------------------

  //! @brief Finalize, called after one or more @c addPath() commands.
  err_t finalize();

  // --------------------------------------------------------------------------
  // [Sweep]
  // --------------------------------------------------------------------------

  //! @brief Sweep scanline @a y.
  FOG_INLINE Span8* sweep(Scanline8& scanline, MemoryBuffer& temp, int y)
  {
    return _sweepSimpleFn(this, scanline, temp, y);
  }

  //! @brief Enhanced version of @c sweep() that accepts clip boxes.
  //!
  //! This method is called by raster paint engine if clipping region is complex.
  FOG_INLINE Span8* sweep(Scanline8& scanline, MemoryBuffer& temp, int y, const BoxI* clipBoxes, size_t count)
  {
    return _sweepRegionFn(this, scanline, temp, y, clipBoxes, count);
  }

  //! @brief Enhanced version of @c sweep() that accepts clip spans.
  //!
  //! This method is called by raster paint engine if clipping region is mask.
  FOG_INLINE Span8* sweep(Scanline8& scanline, MemoryBuffer& temp, int y, const Span8* clipSpans)
  {
    return _sweepSpansFn(this, scanline, temp, y, clipSpans);
  }

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

  //! @brief Fill-Rule (see @c FILL_RULE);
  uint8_t _fillRule;
  //! @brief Shape (see @c RASTERIZER_SHAPE).
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
  RasterizerSweepSimpleFn _sweepSimpleFn;
  //! @brief Sweep scanline using region clipping.
  RasterizerSweepRegionFn _sweepRegionFn;
  //! @brief Sweep scanline using anti-aliased clipping.
  RasterizerSweepSpansFn _sweepSpansFn;

  //! @brief Temporary path (float).
  PathF _tmpPathF;
  //! @brief Temporary path (double).
  PathD _tmpPathD;

private:
  _FOG_CLASS_NO_COPY(Rasterizer8)
};

// ============================================================================
// [Fog::Rasterizer - Helpers]
// ============================================================================

#if defined(FOG_DEBUG_RASTERIZER)
static void Rasterizer_dumpSpans(int y, const Span8* span)
{
  ByteArray b;
  b.appendFormat("Y=%d - ", y);

  while (span)
  {
    if (span->isConst())
    {
      b.appendFormat("C[%d %d]%0.2X", span->x0, span->x1, span->getConstMask());
    }
    else
    {
      b.appendFormat("M[%d %d]", span->x0, span->x1);
      for (int x = 0; x < span->getLength(); x++)
        b.appendFormat("%0.2X", span->getVMask()[x]);
    }
    b.append(' ');
    span = span->getNext();
  }

  printf("%s\n", b.getData());
}
#endif // FOG_DEBUG_RASTERIZER

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RASTERIZER_RASTERIZER_P_H
