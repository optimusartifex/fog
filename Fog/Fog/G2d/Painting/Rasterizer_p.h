// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERIZER_P_H
#define _FOG_G2D_PAINTING_RASTERIZER_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemZoneAllocator.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Painting/RasterApi_p.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Painting/RasterStructs_p.h>

// ============================================================================
// [Debugging]
// ============================================================================

#if defined(FOG_DEBUG_RASTERIZER)
#include <Fog/Core/Tools/String.h>
#endif // FOG_DEBUG_RASTERIZER

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::RasterizerApi]
// ============================================================================

struct FOG_NO_EXPORT RasterizerApi
{
  typedef void (FOG_CDECL *Render8Func)(Rasterizer8* self, RasterFiller* filler, RasterScanline8* scanline);

  // --------------------------------------------------------------------------
  // [Box]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *BoxRasterizer8_Init32x0)(BoxRasterizer8* self, const BoxI* box);
  typedef void (FOG_CDECL *BoxRasterizer8_Init24x8)(BoxRasterizer8* self, const BoxI* box24x8);

  struct _Api_BoxRasterizer8
  {
    BoxRasterizer8_Init32x0 init32x0;
    BoxRasterizer8_Init24x8 init24x8;

    Render8Func render_32x0[RASTER_CLIP_COUNT];
    Render8Func render_24x8[RASTER_CLIP_COUNT];
  } box8;

  // --------------------------------------------------------------------------
  // [Path]
  // --------------------------------------------------------------------------

  struct _Api_PathRasterizer8
  {
    Render8Func render_nonzero[2][RASTER_CLIP_COUNT];
    Render8Func render_evenodd[2][RASTER_CLIP_COUNT];
  } path8;
};

extern FOG_NO_EXPORT RasterizerApi Rasterizer_api;

// ============================================================================
// [Fog::Rasterizer8]
// ============================================================================

//! @internal
//!
//! @brief Rasterizer interface (8-bit).
struct FOG_NO_EXPORT Rasterizer8
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Rasterizer8()
  {
    _scope.reset();
    _render = NULL;

    _opacity = 0x100;
    _clipType = RASTER_CLIP_BOX;
    _initialized = false;
  }

  FOG_INLINE ~Rasterizer8()
  {
  }

  // --------------------------------------------------------------------------
  // [Scene-Box]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxI& getSceneBox() const
  {
    return _sceneBox;
  }

  FOG_INLINE void setSceneBox(const BoxI& sceneBox)
  {
    _sceneBox = sceneBox;
    _sceneBox24x8.x0 = sceneBox.x0 << 8;
    _sceneBox24x8.y0 = sceneBox.y0 << 8;
    _sceneBox24x8.x1 = sceneBox.x1 << 8;
    _sceneBox24x8.y1 = sceneBox.y1 << 8;
  }

  // --------------------------------------------------------------------------
  // [Opacity]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getOpacity() const
  {
    return _opacity;
  }

  FOG_INLINE void setOpacity(uint32_t opacity)
  {
    FOG_ASSERT(opacity <= 0x100);
    _opacity = opacity;
  }

  // --------------------------------------------------------------------------
  // [Clipping]
  // --------------------------------------------------------------------------

  FOG_INLINE void setClipRegion(const BoxI* data, size_t length)
  {
    _clipType = RASTER_CLIP_REGION;
    _clip.region.y0 = data[0].y0;
    _clip.region.y1 = data[length - 1].y1;
    _clip.region.data = data;
    _clip.region.length = length;
  }

  FOG_INLINE void setClipMask(int y0, int y1, const RasterSpan8** spans)
  {
    _clipType = RASTER_CLIP_MASK;
    _clip.mask.y0 = y0;
    _clip.mask.y1 = y1;
    _clip.mask.spans = spans;
  }

  FOG_INLINE void resetClip()
  {
    _clipType = RASTER_CLIP_BOX;
  }

  // --------------------------------------------------------------------------
  // [Render]
  // --------------------------------------------------------------------------

  FOG_INLINE void render(RasterFiller* filler, RasterScanline8* scanline)
  {
    _render(this, filler, scanline);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Scene-box (always must be valid, initialy set to zero == no paint).
  BoxI _sceneBox;
  //! @brief Scene-box in 24x8 format.
  BoxI _sceneBox24x8;

  //! @brief The rasterizer scope (per thread, used by multi-threaded paint engine).
  RasterScope _scope;

  //! @brief Render function (set by rasterizer).
  RasterizerApi::Render8Func _render;

  //! @brief Whether the rasterizer is initialized (contains something which
  //! can be rendered).
  uint8_t _initialized;
  //! @brief Clip type.
  uint8_t _clipType;
  //! @brief Opacity (0 to 0x100, inclusive).
  uint16_t _opacity;

  struct FOG_NO_EXPORT _ClipBase
  {
    int y0;
    int y1;
  };

  struct FOG_NO_EXPORT _ClipRegion : public _ClipBase
  {
    const BoxI* data;
    size_t length;
  };

  struct FOG_NO_EXPORT _ClipMask : public _ClipBase
  {
    const RasterSpan8** spans;
  };

  union
  {
    _ClipBase base;
    _ClipRegion region;
    _ClipMask mask;
  } _clip;
};

// ============================================================================
// [Fog::BoxRasterizer8]
// ============================================================================

//! @internal
//!
//! @brief Scanline box/rect rasterizer with subpixel precision.
struct FOG_NO_EXPORT BoxRasterizer8 : public Rasterizer8
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE BoxRasterizer8()
  {
  }

  FOG_INLINE ~BoxRasterizer8()
  {
  }

  // --------------------------------------------------------------------------
  // [Setup]
  // --------------------------------------------------------------------------

  FOG_INLINE void init32x0(const BoxI& box)
  {
    Rasterizer_api.box8.init32x0(this, &box);
  }

  FOG_INLINE void init24x8(const BoxI& box24x8)
  {
    Rasterizer_api.box8.init24x8(this, &box24x8);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _initialized = false;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Box to fill.
  BoxI _boxBounds;
  //! @brief Box to fill in 24.8 fixed point.
  BoxI _box24x8;

  //! @brief Coverate-top.
  uint16_t _ct[4];
  //! @brief Coverage-inner.
  uint16_t _ci[4];
  //! @brief Coverage-bottom.
  uint16_t _cb[4];
};

// ============================================================================
// [Fog::PathRasterizer8]
// ============================================================================

//! @internal
//!
//! @brief Scanline path/polygon rasterizer, which produces 256 shades of gray.
//!
//! Scanline path/polygon rasterizer that is used to render filled polygons
//! using high-quality anti-aliasing. Internally, by default, the class uses
//! integer coordinates in 24.8 fixed point format, i.e. 24 bits for integer
//! part and 8 bits for fractional part.
//!
//! The rasterizer is re-usable class, which should be used by the following
//! way:
//!
//!   1. Create/Reset
//!
//!      If the rasterizer is reused, reset ensures that all data related to
//!      the previous rasterization are reset. It is the requirement. To reset
//!      the rasterizer use @c Rasterizer8::reset() method.
//!
//!      In case that the rasterizer is only used one time, the Reset step should
//!      be ignored.
//!
//!   2. Setup Parameters
//!
//!      After the rasterizer is created or reset, it's needed to specify the
//!      scene-box, fill-rule, opacity, and clipping. The scene-box is required!
//!
//!      2a) Scene-Box:
//!
//!      The scene-box is similar to clip-box, the exception is that rasterizer
//!      expects that the path/polygon is already clipped by the underlying
//!      paint-engine, so instead of complicated clipping the coordinates are
//!      simply saturated to the scene-box. This means that if the clipping is
//!      not exact (and believe me, the floating point calculation isn't!) then
//!      these small errors are corrected using saturation, instead of complex
//!      clipping algorithm, the correction is here only to prevent possible
//!      buffer underrun/overrun errors.
//!
//!      To get/set scene-box use the @c Rasterizer8::getSceneBox() and
//!      @c Rasterizer8::setSceneBox() methods.
//!
//!      2b) Fill-Rule:
//!
//!      The fill-rule describes how the shape is filled. It must be set before
//!      the path/polygon/shape is added to the rasterizer and it can't be modified.
//!      The methods @c Rasterizer8::getFillRule() and @c Rasterizer8::setFillRule()
//!      can be used to get/set the fill-rule property.
//!
//!      2c) Opacity:
//!
//!      The opacity parameter is used to rasterize a semi-transparent shape.
//!      The default 'opacity' value is 256. To control the global opacity
//!      use the @c Rasterizer8::getOpacity() and @c Rasterizer8::setOpacity()
//!      methods.
//!
//!      2d) Clipping:
//!
//!      Currently the raster paint-engine supports three kinds of clipping.
//!      The box clipping is always used (PathClipper by raster-paint engine
//!      and scene-box by Rasterizer8). To use region/mask clipping the
//!      additional steps are needed.
//!
//!      To setup region/mask clipping the setClipRegion() or setClipMask()
//!      methods must be used. Notice that the clip region/mask must be known
//!      before the shape is rasterized and can't change until the rasterizer
//!      is used.
//!
//!   3. Initialize
//!
//!      After the rasterizer parameters are set-up, the rasterizer must be
//!      initialized. The initialization is simply a validation of all
//!      parameters and preparation for adding path(s). To initialize the
//!      rasterizer use @c Rasterizer8::init() method.
//!
//!      The initialization step is mandatory!
//!
//!   4. Add polygon(s) or path(s)
//!
//!      After the rasterizer was initialized, the shapes can be added. There
//!      is no limitation to the type or count of shapes which might be added
//!      to the rasterizer, but generally all shapes are converted to the
//!      segments of lines first. To add polygon or path use addPath() method.
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
//!      using the @c RasterScanline8 container. The sweep method generates a mask,
//!      which is used by the Raster pipeline to fill the rasterized shape which
//!      is a list of raster-spans.
//!
//!      Before the sweep method is used, it's needed to attach containers where
//!      the temporary data is stored, use @c setupOutput() and @c resetOutput()
//!      to do that.
//!
//!
//! The rasterizer uses the analytic method for cover calculation of the output
//! pixels. It generates CELLs which represent the pixel covereage and area.
//! Pixel position is stored in the chunk, which is simply a list of cells.
//! It's common that chunk contains only one cell. In case that rasterized
//! shape overlaps (or the vertices are too close so they share cell(s)) then
//! the new coverage and area is merged to the existing cell (that is, there
//! are no overlapping cells in the final result).
//!
//! The analytic rasterizer idea and first implementation was based on the AGG,
//! which was based on the freetype2 library. Although there is motivation the
//! rasterizer was rewritten to use different algorithm to render-line/hline
//! and to sweep-scanline (called RenderFunc in Fog)
//!
//! Implementation disadvantages:
//!
//!   - The crossing lines are not handled correctly. The maximum error in
//!     this case is 50%. The original rasterizer was created to render text
//!     which doesn't contain self-intersecting polygons/paths.
//!
//!   - The guys which created AmanithVG library say that the AGG rasterizer
//!     also fails in case that input vertices are too close (not degenerated).
//!     I didn't test the rasterizer for such case and didn't notice issue
//!     mentioned.
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
struct FOG_NO_EXPORT PathRasterizer8 : public Rasterizer8
{
  // --------------------------------------------------------------------------
  // [Cell]
  // --------------------------------------------------------------------------

#include <Fog/Core/C++/PackByte.h>
  struct FOG_NO_EXPORT Cell
  {
    // ------------------------------------------------------------------------
    // [Accessors]
    // ------------------------------------------------------------------------

    FOG_INLINE void set(int _cover, int _area)
    {
      FOG_ASSERT(_cover >= -256     && _cover <= 256    );
      FOG_ASSERT(_area  >= -256*511 && _area  <= 256*511);

      cover = _cover;
      area = _area;
    }

    FOG_INLINE void add(int _cover, int _area)
    {
      FOG_ASSERT(_cover >= -256     && _cover <= 256    );
      FOG_ASSERT(_area  >= -256*511 && _area  <= 256*511);

      cover += _cover;
      area += _area;
    }

    FOG_INLINE void sub(int _cover, int _area)
    {
      FOG_ASSERT(_cover >= -256     && _cover <= 256    );
      FOG_ASSERT(_area  >= -256*511 && _area  <= 256*511);

      cover -= _cover;
      area -= _area;
    }

    // ------------------------------------------------------------------------
    // [Members]
    // ------------------------------------------------------------------------

    int cover;
    int area;
  };
#include <Fog/Core/C++/PackRestore.h>

  // --------------------------------------------------------------------------
  // [Chunk]
  // --------------------------------------------------------------------------

#include <Fog/Core/C++/PackByte.h>
  struct FOG_NO_EXPORT Chunk
  {
    // ------------------------------------------------------------------------
    // [Accessors]
    // ------------------------------------------------------------------------

    FOG_INLINE int getLength() const { return x1 - x0; }

    // ------------------------------------------------------------------------
    // [Statics]
    // ------------------------------------------------------------------------

    static FOG_INLINE size_t getSizeOf(size_t capacity)
    {
      return sizeof(Chunk) - sizeof(Cell) + capacity * sizeof(Cell);
    }

    // ------------------------------------------------------------------------
    // [Members]
    // ------------------------------------------------------------------------

    Chunk* prev;
    Chunk* next;
    int x0;
    int x1;
    Cell cells[1];
  };
#include <Fog/Core/C++/PackRestore.h>

  // --------------------------------------------------------------------------
  // [Row]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT Row
  {
    //! @brief First chunk in this row (sorted from left).
    //!
    //! @note The chunk list is circular, thus chunk->prev can be used to
    //! access the last chunk.
    Chunk* first;
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a @ Rasterizer8 instance.
  PathRasterizer8();
  //! @brief Destroy the @ Rasterizer8 instance.
  ~PathRasterizer8();

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
  // [Init]
  // --------------------------------------------------------------------------

  //! @brief Initialize the rasterizer, called after setup methods and before
  //! adding path(s).
  //!
  //! During setup the methods like setOpacity(), setFillRule() can be called,
  //! after calling init() these methods shouldn't be called, because rasterizer
  //! can use different algorithms to perform rasterization (even-odd  vs.
  //! non-zero fill rule, opacity, quality, etc...).
  err_t init();

  // --------------------------------------------------------------------------
  // [Add]
  // --------------------------------------------------------------------------

  //! @brief Add path to the rasterizer (float).
  void addPath(const PathF& path);
  //! @overload
  void addPath(const PathF& path, const PointF& offset);

  //! @brief Add path to the rasterizer (double).
  void addPath(const PathD& path);
  //! @overload
  void addPath(const PathD& path, const PointD& offset);

  //! @brief Add box to the rasterizer.
  void addBox(const BoxF& path);
  //! @overload
  void addBox(const BoxD& path);

  // --------------------------------------------------------------------------
  // [Chunk Storage]
  // --------------------------------------------------------------------------

  //! @internal
  bool getNextChunkStorage(size_t chunkSize);

  // --------------------------------------------------------------------------
  // [Finalize]
  // --------------------------------------------------------------------------

  //! @brief Finalize, called after one or more @c addPath() commands.
  err_t finalize();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Cell chunk allocator.
  MemZoneAllocator _allocator;

  //! @brief bounding box of the rasterized shape.
  BoxI _boundingBox;

  //! @brief Maximum size of rasterized shape, generated by @c init() and
  //! calculated using @c _clipBox.getWidth() and @c _clipBox.getHeight().
  SizeI _size;
  //! @brief Maximum size in 24x8 format.
  SizeI _size24x8;

  //! @brief The vertex translation offset (float).
  PointF _offsetF;
  //! @brief The vertex translation offset (double).
  PointD _offsetD;

  //! @brief Error code (in case that any error happened it's reported here).
  err_t _error;

  //! @brief Fill-Rule (see @c FILL_RULE);
  uint8_t _fillRule;

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
  //! _rows can contain garbage in these indices:
  //! - _clipBox.y1 -> _boundingBox.y1
  //! - _boundingBox.y2 -> _clipBox.y2
  Row* _rows;

private:
  _FOG_NO_COPY(PathRasterizer8)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERIZER_P_H
