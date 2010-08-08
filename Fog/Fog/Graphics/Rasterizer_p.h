// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_RASTERIZER_P_H
#define _FOG_GRAPHICS_RASTERIZER_P_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/Span_p.h>
#include <Fog/Graphics/Scanline_p.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::Rasterizer]
// ============================================================================

//! @internal
//!
//! Scanline polygon rasterizer.
//!
//! Polygon rasterizer that is used to render filled polygons with 
//! high-quality Anti-Aliasing. Internally, by default, the class uses 
//! integer coordinates in format 24.8, i.e. 24 bits for integer part 
//! and 8 bits for fractional - see poly_subpixel_shift. This class can be 
//! used in the following  way:
//!
//! 1. Reset/setup rasterizer:
//!
//!    reset() - If rasterizer is reused, reset ensures that all data related
//!    to previous rasterization are lost.
//!
//!    getFillRule() / setFillRule(int fillRule) - Fill rule management.
//!    getAlpha() / setAlpha() - Alpha management.
//!    getClipBox() / setClipBox() / resetClipBox() - Set clip box management.
//!
//!    You can omit initialization if you are reusing rasterizer and these
//!    members are already set to demanded values.
//!
//! 2. Initialize rasterizer:
//!
//!    initialize() - Init is method that will initialize the rasterizer and
//!    after calling it you cannot call setup methods (in ideal case setup will
//!    do nothing, in worst case your application can crash or call assertion
//!    failure in debug-more).
//!
//! 3. Call commands:
//!
//!    addPath(path) - Make the polygon. One can create more than one contour,
//!    but each contour must consist of at least 3 vertices, is the absolute
//!    minimum of vertices that define a triangle. The algorithm does not check
//!    either the number of vertices nor coincidence of their coordinates, but
//!    in the worst case it just won't draw anything.
//!
//!    The orger of the vertices (clockwise or counterclockwise) 
//!    is important when using the non-zero filling rule (@c FILL_NON_ZERO).
//!    In this case the vertex order of all the contours must be the same
//!    if you want your intersecting polygons to be without "holes".
//!    You actually can use different vertices order. If the contours do not 
//!    intersect each other the order is not important anyway. If they do, 
//!    contours with the same vertex order will be rendered without "holes" 
//!    while the intersecting contours with different orders will have "holes".
//!
//! 4. Finalize rasterizer:
//!
//!    finalize() - Finalize the shape. Finalize step is rasterizer dependent
//!    and may or may not rasterize or pre-rasterize some areas.
//!
//! 5. Sweep scanlines.
//!
//!    sweepScanline() - Sweep scanline to scanline container. This scanline
//!    can be passed to renderer or clipper.
//!
//! Rasterizers are pooled to maximize performance and decrease memory 
//! fragmentation. If you want to create or free rasterizer use methods
//! @c Rasterizer::getRasterizer() and @c Rasterizer::releaseRasterizer(),
//! respectively.
struct FOG_HIDDEN Rasterizer
{
  // --------------------------------------------------------------------------
  // [Cell]
  // --------------------------------------------------------------------------

  // Analytic rasterizer with 8-bit precision cell members:
  //
  //   X     - a horizontal pixel position in pixel units. Should be relative
  //           to horizontal clipping bounding box.
  //   Cover - a value at interval -256 to 256. This means that 10-bits are
  //           enough to store the value.
  //   Area  - a value at interval -(511<<8) to (511<<8). It can be effectively
  //           compressed into 10-bits by shifting it right and preserving a
  //           sign.
  //
  // This means that cell structure can be compressed into:
  //
  // DWORD (32-bit):
  //   X     - 12 bits.
  //   Cover - 10 bits.
  //   Area  - 10 bits.
  // Applicable for resolution up to 4096 pixels in horizontal.
  //
  // QWORD (64-bit):
  //   X     - 32 bits.
  //   Cover - 16 bits.
  //   Area  - 16 bits.
  // Applicable for any resolution.

#include <Fog/Core/Pack/PackDWord.h>
  //! @internal
  struct FOG_HIDDEN CellXY
  {
    int x;
    int y;
    int cover;
    int area;

    FOG_INLINE void setCell(int _x, int _y, int _cover, int _area) { x = _x; y = _y; cover = _cover; area = _area; }
    FOG_INLINE void setCell(const CellXY& other) { x = other.x; y = other.y; cover = other.cover; area = other.area; }

    FOG_INLINE void setPosition(int _x, int _y) { x = _x, y = _y; }
    FOG_INLINE bool hasPosition(int _x, int _y) const { return ((_x - x) | (_y - y)) == 0; }

    FOG_INLINE void setCovers(int _cover, int _area) { cover = _cover; area = _area; }
    FOG_INLINE void addCovers(int _cover, int _area) { cover += _cover; area += _area; }
    FOG_INLINE bool hasCovers() const { return (cover | area) != 0; }
  };
#include <Fog/Core/Pack/PackRestore.h>

#include <Fog/Core/Pack/PackDWord.h>
  //! @internal
  struct FOG_HIDDEN CellX
  {
    int x;
    int cover;
    int area;

    FOG_INLINE void set(int _x, int _cover, int _area)
    {
      x = _x;
      cover = _cover;
      area = _area;
    }

    FOG_INLINE void set(const CellX& other)
    {
      x = other.x;
      cover = other.cover;
      area = other.area;
    }

    FOG_INLINE void set(const CellXY& other)
    {
      x = other.x;
      cover = other.cover;
      area = other.area;
    }
  };
#include <Fog/Core/Pack/PackRestore.h>

  // --------------------------------------------------------------------------
  // [CellXYBuffer]
  // --------------------------------------------------------------------------

  //! @internal
  //!
  //! @brief Cell buffer.
  struct FOG_HIDDEN CellXYBuffer
  {
    CellXYBuffer* prev;
    CellXYBuffer* next;
    uint32_t capacity;
    uint32_t count;
    CellXY cells[1];
  };

  // --------------------------------------------------------------------------
  // [RowInfo]
  // --------------------------------------------------------------------------

  //! @internal
  //!
  //! @brief Lookup table that contains index and count of cells in sorted cells
  //! buffer. Each index to this table represents one row.
  struct FOG_HIDDEN RowInfo
  {
    uint32_t index;
    uint32_t count;
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Rasterizer();
  virtual ~Rasterizer();

  //! @brief Called before rasterizer is pooled. Default implementation is NOP.
  virtual void pooled() = 0;

  // --------------------------------------------------------------------------
  // [Clip Box]
  // --------------------------------------------------------------------------

  FOG_INLINE const IntBox& getClipBox() const { return _clipBox; }
  FOG_INLINE void setClipBox(const IntBox& clipBox) { _clipBox = clipBox; }
  FOG_INLINE void resetClipBox() { _clipBox.clear(); }

  // --------------------------------------------------------------------------
  // [Bounding Box]
  // --------------------------------------------------------------------------

  //! @brief Get rasterized object bounding box.
  //!
  //! @note This method is only valid after @c finalize() call.
  FOG_INLINE const IntBox& getBoundingBox() const { return _boundingBox; }

  // --------------------------------------------------------------------------
  // [Error]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getError() const { return _error; }
  FOG_INLINE void setError(err_t error) { _error = error; }
  FOG_INLINE void resetError() { _error = ERR_OK; }

  // --------------------------------------------------------------------------
  // [Fill Rule]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getFillRule() const { return _fillRule; }
  FOG_INLINE void setFillRule(uint32_t fillRule) { _fillRule = fillRule; }

  // --------------------------------------------------------------------------
  // [Alpha]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getAlpha() const { return _alpha; }
  FOG_INLINE void setAlpha(uint32_t alpha) { _alpha = alpha; }

  // --------------------------------------------------------------------------
  // [Finalized / Valid]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isFinalized() const { return _isFinalized; }
  FOG_INLINE bool isValid() const { return _isValid; }

  // --------------------------------------------------------------------------
  // [Reset / Initialize / Finalize]
  // --------------------------------------------------------------------------

  //! @brief Reset rasterizer.
  virtual void reset() = 0;

  //! @brief Initialize the rasterizer, called after setup methods and before
  //! adding path(s).
  //!
  //! During setup the methods like setAlpha(), setFillRule() can be called,
  //! after calling initialize() these methods shouldn't be called, because
  //! rasterizer can use different algorithms to perform rasterization (even-odd
  //! vs. non-zero fill rule, alpha, quality, etc...).
  virtual void initialize() = 0;
  virtual void finalize() = 0;

  // --------------------------------------------------------------------------
  // [Commands]
  // --------------------------------------------------------------------------

  virtual void addPath(const DoublePath& path) = 0;

  // --------------------------------------------------------------------------
  // [Sweep]
  // --------------------------------------------------------------------------

  typedef Span8* (*SweepScanlineSimpleFn)(Rasterizer* rasterizer, Scanline8& scanline, int y);
  typedef Span8* (*SweepScanlineRegionFn)(Rasterizer* rasterizer, Scanline8& scanline, int y, const IntBox* clipBoxes, sysuint_t count);
  typedef Span8* (*SweepScanlineSpansFn)(Rasterizer* rasterizer, Scanline8& scanline, int y, const Span8* clipSpans);

  //! @brief Sweep scanline @a y.
  FOG_INLINE Span8* sweepScanline(Scanline8& scanline, int y)
  { return _sweepScanlineSimpleFn(this, scanline, y); }

  //! @brief Enhanced version of @c sweepScanline() that accepts clip region.
  //!
  //! This method is called by raster paint engine if clipping region is complex.
  FOG_INLINE Span8* sweepScanline(Scanline8& scanline, int y, const IntBox* clipBoxes, sysuint_t count)
  { return _sweepScanlineRegionFn(this, scanline, y, clipBoxes, count); }

  //! @brief Enhanced version of @c sweepScanline() that accepts clip spans.
  //!
  //! This method is called by raster paint engine if clipping region is mask.
  FOG_INLINE Span8* sweepScanline(Scanline8& scanline, int y, const Span8* clipSpans)
  { return _sweepScanlineSpansFn(this, scanline, y, clipSpans); }

  // --------------------------------------------------------------------------
  // [Pooling]
  // --------------------------------------------------------------------------

  //! @brief Get rasterizer instance.
  static Rasterizer* getRasterizer();
  //! @brief Release rasterizer instance.
  static void releaseRasterizer(Rasterizer* rasterizer);

  //! @brief Get cell buffer instance.
  static CellXYBuffer* getCellXYBuffer();
  //! @brief Release cell buffer instance.
  static void releaseCellXYBuffer(CellXYBuffer* cellBuffer);

  //! @brief Free all pooled rasterizer and cell buffer instances.
  static void cleanup();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  //! @brief Link to next pooled @c Rasterizer instance. Always NULL when you 
  //! get rasterizer by @c Rasterizer::getRasterizer() method.
  Rasterizer* _pool;

public:
  //! @brief Clip bounding box (always must be valid, initialy set to zero).
  IntBox _clipBox;

  //! @brief bounding box of rasterized object (after clipping).
  IntBox _boundingBox;

  //! @brief Fill rule;
  uint32_t _fillRule;

  //! @brief Alpha.
  uint32_t _alpha;

  //! @brief Rasterizer error code.
  err_t _error;

  //! @brief Whether rasterizer is finalized.
  bool _isFinalized;

  //! @brief Whether the rasterized object is empty (not paint).
  bool _isValid;

  //! @brief Sweep scanline using box clipping.
  SweepScanlineSimpleFn _sweepScanlineSimpleFn;
  //! @brief Sweep scanline using region clipping.
  SweepScanlineRegionFn _sweepScanlineRegionFn;
  //! @brief Sweep scanline using anti-aliased clipping.
  SweepScanlineSpansFn _sweepScanlineSpansFn;

private:
  FOG_DISABLE_COPY(Rasterizer)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_RASTERIZER_P_H
