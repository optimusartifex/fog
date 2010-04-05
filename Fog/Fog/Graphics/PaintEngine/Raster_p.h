// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTERENGINE_RASTER_P_H
#define _FOG_GRAPHICS_PAINTERENGINE_RASTER_P_H

// [Dependencies]
#include <Fog/Core/Lock.h>
#include <Fog/Core/Task.h>
#include <Fog/Core/Thread.h>
#include <Fog/Core/ThreadCondition.h>
#include <Fog/Graphics/ImageFilter.h>
#include <Fog/Graphics/PaintEngine.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Graphics/Rasterizer_p.h>
#include <Fog/Graphics/Scanline_p.h>
#include <Fog/Graphics/Stroker.h>

//! @addtogroup Fog_Graphics_Private
//! @{

namespace Fog {

// ============================================================================
// [Configuration]
// ============================================================================

// Debugging.
// #define FOG_DEBUG_RASTER_SYNCHRONIZATION
// #define FOG_DEBUG_RASTER_COMMANDS

// Minimum size to set multithreading on.
enum { RASTER_MIN_SIZE_THRESHOLD = 256*256 };

// Maximum number of threads to use for rendering.
enum { RASTER_MAX_WORKERS = 16 };
// Maximum commands and calculations to accumulate in buffer.
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
enum { RASTER_MAX_COMMANDS = 32 };     // For debugging it's better to have only minimal
enum { RASTER_MAX_CALCULATIONS = 32 }; // buffer size for the commands and calculations.
#else
enum { RASTER_MAX_COMMANDS = 1024 };
enum { RASTER_MAX_CALCULATIONS = 1024 };
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct RasterPaintAction;
struct RasterPaintCalc;
struct RasterPaintCmd;
struct RasterPaintContext;
struct RasterPaintEngine;
struct RasterPaintLayer;
struct RasterPaintState;
struct RasterPaintTask;
struct RasterWorkerManager;

// ============================================================================
// [Constants]
// ============================================================================

//! @brief Type of current painter layer
enum RASTER_LAYER_TYPE
{
  //! @brief Non-initialized.
  RASTER_LAYER_TYPE_NONE = 0,

  //! @brief Direct 32-bit painting and compositing (PRGB32, XRGB32).
  //!
  //! This layer is te most efficient layer in Fog-Framework.
  //!
  //! Pixel format summary:
  //! - @c PIXEL_FORMAT_PRGB32
  //! - @c PIXEL_FORMAT_XRGB32
  RASTER_LAYER_TYPE_DIRECT32 = 1,

  //! @brief Indirect 32-bit painting and compositing (ARGB32).
  //!
  //! This layer is less efficient than @c RASTER_LAYER_TYPE_DIRECT32, because each
  //! pixel needs to be premultiplied before compositing and demultiplied back
  //! after it has been processed.
  //!
  //! Pixel format summary:
  //! - @c PIXEL_FORMAT_ARGB32
  RASTER_LAYER_TYPE_INDIRECT32 = 2
};

//! @brief Type of current transform in raster paint engine.
enum RASTER_TRANSFORM_TYPE
{
  //! @brief Transformation matrix is identity or translation only.
  //!
  //! If translation part of matrix is used the values are aligned to pixel
  //! grid (this means that integral numbers can be used for this part). This
  //! is most efficient transform type in simple cases (fill rect, blit image,
  //! etc...).
  RASTER_TRANSFORM_EXACT = 0,

  //! @brief Transformation matrix is identity except translation part which
  //! can be represented only by real numbers. Painter has complex fast paths
  //! for this type, but it's used mainly by image blitting and texture
  //! fetching.
  RASTER_TRANSFORM_SUBPX = 1,

  //! @brief Transformation matrix is affine (not identity or simple translation).
  RASTER_TRANSFORM_AFFINE = 2
};

//! @brief Changed flags (used in multithreaded mode to send change commands).
enum RASTER_CHANGED_FLAGS
{
  RASTER_CHANGED_REGION = 0x00000001
};

//! @brief Clip type (masks).
enum RASTER_CLIP_TYPE
{
  RASTER_CLIP_SIMPLE = 0x01,
  RASTER_CLIP_COMPLEX = 0x02,
  RASTER_CLIP_MASK = 0x04
};

//! @brief Status of the raster paint engine command.
//!
//! Status of command is only used if it has associated calculation.
enum RASTER_COMMAND_STATUS
{
  //! @brief Wait until calculation is done.
  RASTER_COMMAND_WAIT = 0,
  //! @brief Ready to process the command.
  RASTER_COMMAND_READY = 1,
  //! @brief Skip this command - don't call @c RasterPaintAction::run().
  RASTER_COMMAND_SKIP = 2
};

// ============================================================================
// [Fog::BlockAllocator]
// ============================================================================

//! @brief Custom memory allocator used by raster paint engine.
//!
//! This allocator allocates larger blocks (see @c BLOCK_SIZE) dividing them
//! into small pieces demanded throught @c BlockAllocator::alloc() method.
//! Each allocation contains information about memory block used by allocator
//! and when the memory is not needed (and @c BlockAllocator::free() is called)
//! it's atomically removed from memory block.
//!
//! In short: Each memory block has information about used memory, increased by
//! @c BlockAllocator::alloc() and decreased by @c BlockAllocator::free().
//! When the number is decreased to zero then the block is free and will be 
//! reused. The goal of this algorithm is to provide fast memory alloc/free,
//! but do not eat too much memory (reuse it).
//!
//! @note The @c BlockAllocator::alloc() is reentrant and can be called only
//! by main thread, but @c BlockAllocator::free() is thread-safe and it can
//! be called (and it is) from worker threads.
struct FOG_HIDDEN BlockAllocator
{
  // --------------------------------------------------------------------------
  // [Constants]
  // --------------------------------------------------------------------------

  //! @brief Size of one block (decreased by some value to always fit in 
  //! one/two pages). 96 bytes reserved for @c ::malloc() data and our
  //! @c Block data.
  enum { BLOCK_SIZE = 8000 };

  // --------------------------------------------------------------------------
  // [Block]
  // --------------------------------------------------------------------------

  //! @brief One larger memory block.
  struct Block
  {
    //! @brief Link to next block.
    Block* next;

    //! @brief Size of the block.
    sysuint_t size;
    //! @brief Allocator position, incremented by each @c BlockAllocator::alloc().
    sysuint_t pos;
    //! @brief Count of bytes used by the block (atomic).
    Atomic<sysuint_t> used;

    //! @brief The memory.
    uint8_t memory[BLOCK_SIZE];
  };

  // --------------------------------------------------------------------------
  // [Header]
  // --------------------------------------------------------------------------

  //! @brief Block header.
  struct Header
  {
    //! @brief Link to block structure.
    Block* block;
    //! @brief Size of block (at this time same as BLOCK_SIZE, for future).
    sysuint_t size;
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Memory allocator constructor.
  BlockAllocator();
  //! @brief Memory allocator destructor, will check if all blocks are freed.
  ~BlockAllocator();

  // --------------------------------------------------------------------------
  // [Alloc / Free]
  // --------------------------------------------------------------------------

  //! @brief Alloc @a size bytes of memory, like @c ::malloc().
  //!
  //! This method is reentrant, never call it from other than engine thread.
  void* alloc(sysuint_t size);

  //! @brief Free @a ptr allocated by @c alloc();
  FOG_INLINE void free(void* ptr);

  //! @brief Reset, will check if all blocks are freed.
  void reset();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Link to first memory block (should be the most free one).
  Block* blocks;

private:
  FOG_DISABLE_COPY(BlockAllocator)
};

FOG_INLINE void BlockAllocator::free(void* ptr)
{
  Header* header = reinterpret_cast<Header*>(reinterpret_cast<uint8_t*>(ptr) - sizeof(Header));
  header->block->used.sub(header->size);
}

// ============================================================================
// [Fog::ZoneAllocator]
// ============================================================================

//! @brief Memory allocator designed to fast alloc memory that will be freed
//! in one step (used by raster paint engine for commands and calculations).
//!
//! @note This is hackery for performance. Concept is that objects created
//! by @c ZoneAllocator are freed all at once. This means that lifetime of
//! these objects are same as zone object itselt (or managed by zone allocator
//! owner).
//!
//! This class were stripped from AsmJit.
//!   http://code.google.com/p/asmjit/
struct FOG_HIDDEN ZoneAllocator
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create new instance of zone allocator.
  //! @param chunkSize Default size for one zone chunk.
  ZoneAllocator(sysuint_t chunkSize);

  //! @brief Destroy zone allocatorinstance.
  ~ZoneAllocator();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Allocate @c size bytes of memory and return pointer to it.
  //!
  //! Pointer allocated by this way will be valid until @c ZoneAllocator object
  //! is destroyed. To create class by this way use placement @c new and
  //! @c delete operators:
  //!
  //! @code
  //! // Example of allocating simple class
  //!
  //! // Your class
  //! class Object
  //! {
  //!   // members...
  //! };
  //!
  //! // Your function
  //! void f()
  //! {
  //!   // We are using AsmJit namespace
  //!   using namespace AsmJit
  //!
  //!   // Create zone object with chunk size of 65536 bytes.
  //!   ZoneAllocator zone(8096);
  //!
  //!   // Create your objects using zone object allocating, for example:
  //!   Object* obj = new(zone.alloc(sizeof(YourClass))) Object();
  //!
  //!   // ... lifetime of your objects ...
  //!
  //!   // Destroy your objects:
  //!   obj->~Object();
  //!
  //!   // ZoneAllocator destructor will free all memory allocated through it,
  //!   // alternative is to call @c zone.free().
  //! }
  //! @endcode
  FOG_INLINE void* alloc(sysuint_t size)
  {
    // Chunks must be valid pointer if we are here.
    FOG_ASSERT(_chunks != NULL);

    if (FOG_LIKELY(_chunks->getRemainingBytes() >= size))
    {
      uint8_t* p = _chunks->data + _chunks->pos;
      _chunks->pos += size;
      return (void*)p;
    }
    else
    {
      return _alloc(size);
    }
  }

  //! @brief Internal alloc function.
  void* _alloc(sysuint_t size);

  //! @brief Free all allocated memory except first block that remains for reuse.
  //!
  //! Note that this method will invalidate all instances using this memory
  //! allocated by this zone instance.
  void reset();

  // --------------------------------------------------------------------------
  // [Chunk]
  // --------------------------------------------------------------------------

  //! @brief One allocated chunk of memory.
  struct Chunk
  {
    //! @brief Link to previous chunk.
    Chunk* prev;
    //! @brief Position in this chunk.
    sysuint_t pos;
    //! @brief Size of this chunk (in bytes).
    sysuint_t size;

    //! @brief Data.
    uint8_t data[sizeof(void*)];

    //! @brief Get count of remaining (unused) bytes in chunk.
    FOG_INLINE sysuint_t getRemainingBytes() const { return size - pos; }
  };

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  //! @brief Last allocated chunk of memory.
  Chunk* _chunks;
  //! @brief One chunk size.
  sysuint_t _chunkSize;

  FOG_DISABLE_COPY(ZoneAllocator)
};

// ============================================================================
// [Fog::RasterPaintLayer]
// ============================================================================

//! @brief Raster paint engine layer.
struct FOG_HIDDEN RasterPaintLayer
{
  //! @brief Pointer to first raster scanline.
  uint8_t* pixels;

  //! @brief Layer width.
  uint32_t width;
  //! @brief Layer height.
  uint32_t height;
  //! @brief Layer primary format - format of @c pixels.
  uint32_t format;
  //! @brief Layer bytes per pixel.
  uint32_t bytesPerPixel;
  //! @brief Layer stride (bytes per line including padding).
  sysint_t stride;

  //! @brief Type of layer.
  uint32_t type;
  //! @brief Layer secondary format - format of @c pixels converted by
  //! @c toSecondary() method.
  uint32_t secondaryFormat;

  //! @brief Convert pixels from primary format to pixels in secondary format.
  RasterEngine::VSpanFn toSecondary;
  //! @brief Convert pixels from secondary format to pixels in primary format.
  RasterEngine::VSpanFn fromSecondary;
};

// ============================================================================
// [Fog::RasterClipBuffer]
// ============================================================================

struct FOG_HIDDEN RasterClipBuffer
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterClipBuffer();
  ~RasterClipBuffer();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  err_t create(int w, int h, uint32_t format);
  err_t adopt(Image& other);

  err_t reallocRows(int h);

  void free();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  struct Row 
  {
    int x1, x2;
  };

  Atomic<sysuint_t> refCount;
  Image image;
  Row* rows;

  IntBox box;
  IntBox clip;

private:
  FOG_DISABLE_COPY(RasterClipBuffer)
};

// ============================================================================
// [Fog::RasterPaintOps]
// ============================================================================

//! @brief Context ops (used in main state and per context instance).
union RasterPaintOps
{
  struct
  {
    //! @brief Compositing operator, see @c OPERATOR_TYPE.
    uint32_t op : 8;

    //! @brief Type of source, see @c PAINTER_SOURCE_TYPE.
    uint32_t sourceType : 8;

    //! @brief Type of clip area, see @c RASTER_CLIP_TYPE.
    uint32_t clipType : 8;

    //! @brief Reserved for future use.
    uint32_t reserved : 8;
  };

  //! @brief All data packed in one integer.
  uint32_t data;
};

// ============================================================================
// [Fog::RasterPaintHints]
// ============================================================================

//! @brief Context hints (used in main state).
union RasterPaintHints
{
  struct
  {
    //! @brief Type of transform, see @c RASTER_TRANSFORM_TYPE.
    uint32_t transformType : 8;

    //! @brief Fill mode, see @c FILL_MODE.
    uint32_t fillMode : 4;

    //! @brief Anti-aliasing type / quality, see @c ANTI_ALIASING_TYPE.
    uint32_t aaQuality : 4;

    //! @brief Image interpolation type / quality, see @c INTERPOLATION_TYPE.
    uint32_t imageInterpolation : 4;

    //! @brief Gradient interpolation type / quality, see @c INTERPOLATION_TYPE.
    uint32_t gradientInterpolation : 4;

    //! @brief Whether to force vector text.
    uint32_t forceOutlineText : 1;

    //! @brief Whether line is simple (one pixel width and default caps).
    uint32_t lineIsSimple : 1;

    uint32_t metaMatrixUsed : 1;
    uint32_t metaRegionUsed : 1;
    uint32_t userRegionUsed : 1;
    uint32_t workRegionUsed : 1;
  };

  //! @brief All data packed in one integer.
  uint32_t data;
};

// ============================================================================
// [Fog::RasterPaintContext]
// ============================================================================

struct FOG_HIDDEN RasterPaintContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterPaintContext();
  ~RasterPaintContext();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE void copyFromMaster(const RasterPaintContext& master);

  // --------------------------------------------------------------------------
  // [Buffer Manager]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t* getBuffer(sysuint_t size);

  uint8_t* reallocBuffer(sysuint_t size);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Owner of this context.
  RasterPaintEngine* engine;

  //! @brief Context id (multithreading).
  //!
  //! @note If multithreading is disabled, id is -1.
  int id;

  //! @brief Context offset (multithreading).
  //!
  //! @note If multithreading is disabled, offset is 0.
  int offset;

  //! @brief Context delta (multithreading).
  //!
  //! @note If multithreading is disabled, delta is 1.
  int delta;

  //! @brief Current context ops.
  RasterPaintOps ops;

  //! @brief Current context layer.
  RasterPaintLayer layer;

  //! @brief Solid source color (applicable if source type is PAINTER_SOURCE_ARGB.
  RasterEngine::Solid solid;

  //! @brief Pattern source context.
  RasterEngine::PatternContext* pctx;

  //! @brief Pointer to compositing functions, see @c ops.op.
  RasterEngine::FunctionMap::CompositeFuncs* funcs;

  //! @brief Raster engine closure (used together with blitter from @c funcs).
  RasterEngine::Closure closure;

  //! @brief The meta origin.
  IntPoint metaOrigin;
  //! @brief The meta region.
  Region metaRegion;

  //! @brief Meta origin translated by used origin.
  IntPoint workOrigin;
  //! @brief Meta region intersected with user region translated by meta origin.
  Region workRegion;

  //! @brief Clip box (work region extents).
  IntBox clipBox;

  //! @brief Scanline instance owned by context, for current work.
  Scanline32 scanline;

  //! @brief Reusable buffer.
  uint8_t* buffer;
  //! @brief Reusable buffer size.
  sysuint_t bufferSize;

private:
  FOG_DISABLE_COPY(RasterPaintContext)
};

// ============================================================================
// [Fog::RasterPaintMainContext]
// ============================================================================

//! @brief Main state structure where is stored the current state.
struct FOG_HIDDEN RasterPaintMainContext : public RasterPaintContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterPaintMainContext();
  ~RasterPaintMainContext();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Context hints.
  RasterPaintHints hints;

  //! @brief Changed flags, see @c RASTER_CHANGED_FLAGS.
  uint32_t changed;

  //! @brief Pattern source data (applicable if source type is @c PAINTER_SOURCE_PATTERN).
  Static<Pattern> pattern;

  //! @brief Stroke parameters.
  StrokeParams strokeParams;

  //! @brief User origin.
  IntPoint userOrigin;
  //! @brief User region.
  Region userRegion;

  //! @brief Meta matrix.
  DoubleMatrix metaMatrix;

  //! @brief User transformation matrix.
  DoubleMatrix userMatrix;
  //! @brief Work transformation matrix (the matrix used to transform 
  //! coordinates from user space to raster).
  DoubleMatrix workMatrix;

  //! @brief Transformation translate point in pixels (can be used if 
  //! transform type is @c RASTER_TRANSFORM_EXACT).
  IntPoint workTranslate;

  //! @brief Transformation approximation scale used by path flattening 
  //! and stroking.
  double approximationScale;

private:
  FOG_DISABLE_COPY(RasterPaintMainContext)
};



struct FOG_HIDDEN RasterPaintState
{
  RasterEngine::PatternContext* pctx;
};




















// ============================================================================
// [Fog::RasterRenderImageAffineBound]
// ============================================================================

//! @brief Class that is used as a wrapper to do image affine transformation
//! using compositing operator that is bound (that means that multiplying weight
//! and image alpha is permited).
struct RasterRenderImageAffineBound
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterRenderImageAffineBound()
  {
    // Mark as non-initialized.
    ictx.initialized = false;
  }

  FOG_INLINE ~RasterRenderImageAffineBound()
  {
    // Destroy if initialized.
    if (ictx.initialized) ictx.destroy(&ictx);
  }

  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  bool init(
    const Image& image, const IntRect& irect,
    const DoubleMatrix& matrix,
    const IntBox& clipBox, uint32_t interpolationType);

  FOG_INLINE bool isInitialized() const { return (bool)ictx.initialized; }

  // --------------------------------------------------------------------------
  // [Render]
  // --------------------------------------------------------------------------

  void render(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RasterEngine::PatternContext ictx;
  DoublePoint pts[4];
  int pty[4];

  int leftStart;
  int leftDirection;

  int rightStart;
  int rightDirection;

  int ytop;

  int xmin;
  int ymin;
  int xmax;
  int ymax;
};

// ============================================================================
// [Fog::RasterPaintAction]
// ============================================================================

//! @brief Raster paint action is base class for @c RasterPaintCmd and
//! @c RasterPaintCalc classes. It shares common stuff and dispatching.
struct FOG_HIDDEN RasterPaintAction
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Action constructor.
  FOG_INLINE RasterPaintAction() {}

  //! @brief Action destructor (non-virtual).
  //!
  //! It's not virtual! Use @c release() to destroy resources associated
  //! with the action.
  FOG_INLINE ~RasterPaintAction() {}

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Run action (command or calculation).
  virtual void run(RasterPaintContext* ctx) = 0;
  //! @brief Release action (it means free / dereference all associated
  //! resources with this action).
  virtual void release(RasterPaintContext* ctx) = 0;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Owner of this action.
  // RasterPaintEngine* engine;

private:
  FOG_DISABLE_COPY(RasterPaintAction)
};

// ============================================================================
// [Fog::RasterPaintCmd]
// ============================================================================

//! @brief Command - base class for all painter commands.
//!
//! Command is action that is executed by each painter's thread (worker). To 
//! make sure that threads are working with own data the @c run() method must
//! use worker delta and offset variables.
//!
//! Commands are executed in FIFO (first-in, first-out) order for each thread,
//! but threads are not synchronized. It means that threads may process different
//! commands in parallel, but one-by-one.
//!
//! Command can depend to @c calculation. If calculation is not @c NULL and
//! command status is @c RASTER_COMMAND_WAIT then thread can't process
//! current command until command status is set to @c RASTER_COMMAND_READY
//! or @c RASTER_COMMAND_SKIP (that means skip this command). Instead of waiting
//! the worker can work on different calculation, see @c RasterPaintTask::run()
//! implementation. Notice that command status is set by @c RasterPaintCalc if
//! used.
//!
//! After command is processed the @c RasterPaintAction::release() is called.
//! To create own command the method must be implemented and you must ensure
//! to call @c RasterPaintCmd::_releasePattern() for painting commands. When 
//! writing command concentrate only to @c RasterPaintAction::release() method,
//! because all other stuff should be done by worker task.
struct FOG_HIDDEN RasterPaintCmd : public RasterPaintAction
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmd() {};
  FOG_INLINE ~RasterPaintCmd() {};

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  //!
  //! Initial value is count of workers and each worker decreases it when
  //! finished. If value is decreased to zero worker must release the command
  //! using @c RasterPaintAction::release() method. This is done by the worker 
  //! task.
  Atomic<int> refCount;

  //! @brief Status of this command.
  Atomic<int> status;

  //! @brief Calculation.
  RasterPaintCalc* calculation;
};

// ============================================================================
// [Fog::RasterPaintCmdLayerChange]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdLayerChange : public RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdLayerChange() {};
  FOG_INLINE ~RasterPaintCmdLayerChange() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The layer data.
  RasterPaintLayer layer;

  IntPoint metaOrigin;
  Region metaRegion;

  IntPoint workOrigin;
  Region workRegion;
};

// ============================================================================
// [Fog::RasterPaintCmdRegionChange]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdRegionChange : public RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdRegionChange() {};
  FOG_INLINE ~RasterPaintCmdRegionChange() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  IntPoint metaOrigin;
  Region metaRegion;

  IntPoint workOrigin;
  Region workRegion;
};

// ============================================================================
// [Fog::RasterPaintCmdDraw]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdDraw : public RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdDraw() {};
  FOG_INLINE ~RasterPaintCmdDraw() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  //! @brief Initialize paint command to classic painting (used by all paint
  //! commands except image
  //! blitting).
  FOG_INLINE void _initPaint(RasterPaintContext* ctx);

  //! @brief Initialize paint command for image blitting (used by all image
  //! blit commands, but not image affine blit commands).
  FOG_INLINE void _initBlit(RasterPaintContext* ctx);

  FOG_INLINE void _beforeBlit(RasterPaintContext* ctx);

  //! @brief Called before paint in worker thread, by @c run() method.
  FOG_INLINE void _beforePaint(RasterPaintContext* ctx);

  //! @brief Called after paint in worker thread, by @c run() method.
  FOG_INLINE void _afterPaint(RasterPaintContext* ctx);

  //! @brief This method destroys only general @c RasterPaintCmdDraw data.
  FOG_INLINE void _releasePattern(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RasterPaintOps ops;

  union
  {
    //! @brief Solid color (if source type is @c PAINTER_SOURCE_ARGB).
    RasterEngine::Solid solid;
    //! @brief Pattern context (if source type is @c PAINTER_SOURCE_PATTERN).
    RasterEngine::PatternContext* pctx;
  };
};

// ============================================================================
// [Fog::RasterPaintCmdBoxes]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdBoxes : public RasterPaintCmdDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdBoxes() {};
  FOG_INLINE ~RasterPaintCmdBoxes() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  sysuint_t count;
  IntBox boxes[1];
};

// ============================================================================
// [Fog::RasterPaintCmdImage]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdImage : public RasterPaintCmdDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdImage() {};
  FOG_INLINE ~RasterPaintCmdImage() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<Image> image;

  IntRect dst;
  IntRect src;
};

// ============================================================================
// [Fog::RasterPaintCmdImageAffineBound]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdImageAffineBound : public RasterPaintCmdDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdImageAffineBound() { renderer.init(); };
  FOG_INLINE ~RasterPaintCmdImageAffineBound() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<RasterRenderImageAffineBound> renderer;
};

// ============================================================================
// [Fog::RasterPaintCmdGlyphSet]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdGlyphSet : public RasterPaintCmdDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdGlyphSet() {};
  FOG_INLINE ~RasterPaintCmdGlyphSet() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<GlyphSet> glyphSet;

  IntPoint pt;
  IntBox boundingBox;
};

// ============================================================================
// [Fog::RasterPaintCmdPath]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdPath : public RasterPaintCmdDraw
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmdPath() {};
  FOG_INLINE ~RasterPaintCmdPath() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Rasterizer (where path is rasterized by @c RasterPaintCalc).
  //!
  //! Rasterizer is set-up by calculation.
  Rasterizer* ras;
};

// ============================================================================
// [Fog::RasterPaintCalc]
// ============================================================================

//! @brief Calculation
//!
//! Calculation is complex action (usually sequental) that can be run only by
//! one thread. To improve performance ofsuch calculations raster paint engine
//! can distribute them to different threads.
struct FOG_HIDDEN RasterPaintCalc : public RasterPaintAction
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCalc() {};
  FOG_INLINE ~RasterPaintCalc() {};

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Related command to this calculation.
  RasterPaintCmd* relatedTo;
};

// ============================================================================
// [Fog::RasterPaintCalcPath]
// ============================================================================

struct FOG_HIDDEN RasterPaintCalcFillPath : public RasterPaintCalc
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCalcFillPath() {};
  FOG_INLINE ~RasterPaintCalcFillPath() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Owner raster paint engine.
  RasterPaintEngine* engine;

  //! @brief Path to process.
  Static<DoublePath> path;

  //! @brief Transformation matrix.
  Static<DoubleMatrix> matrix;

  //! @brief Clip box.
  IntBox clipBox;

  //! @brief Transformation type (see @c RASTER_TRANSFORM_TYPE).
  uint32_t transformType;

  //! @brief Fill mode.
  uint32_t fillMode;

  //! @brief Approximation scale.
  double approximationScale;
};

// ============================================================================
// [Fog::RasterPaintCalcStrokePath]
// ============================================================================

struct FOG_HIDDEN RasterPaintCalcStrokePath : public RasterPaintCalc
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCalcStrokePath() {};
  FOG_INLINE ~RasterPaintCalcStrokePath() {};

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run(RasterPaintContext* ctx);
  virtual void release(RasterPaintContext* ctx);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Owner raster paint engine.
  RasterPaintEngine* engine;

  //! @brief Path to process.
  Static<DoublePath> path;

  //! @brief Transformation matrix.
  Static<DoubleMatrix> matrix;

  //! @brief Clip box.
  IntBox clipBox;

  //! @brief Transformation type (see @c RASTER_TRANSFORM_TYPE).
  uint32_t transformType;

  //! @brief Stroker.
  Static<Stroker> stroker;
};

// ============================================================================
// [Fog::RasterPaintTask]
// ============================================================================

// This is task created per painter thread that contains all variables needed
// to process painter commands in parallel. The goal is that condition variable
// is shared across all painter threads so one signal will wake them all.
struct FOG_HIDDEN RasterPaintTask : public Task
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterPaintTask(Lock* condLock);
  virtual ~RasterPaintTask();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual void run();
  virtual void destroy();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // State
  enum STATE
  {
    RUNNING,
    WAITING,
    DONE
  };

  Atomic<int> state;

  // True if worker should quit from main loop.
  Atomic<int> shouldQuit;

  // Commands and calculations.
  volatile sysint_t calcCurrent;
  volatile sysint_t cmdCurrent;

  // Worker context.
  RasterPaintContext ctx;

  // Worker condition.
  ThreadCondition cond;
};

// ============================================================================
// [Fog::RasterWorkerManager]
// ============================================================================

// Structure shared across all workers (threads).
struct FOG_HIDDEN RasterWorkerManager
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterWorkerManager();
  ~RasterWorkerManager();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  // To call the lock must be locked!
  RasterPaintTask* wakeUpScheduled(RasterPaintTask* calledFrom);
  RasterPaintTask* wakeUpSleeping(RasterPaintTask* calledFrom);
  bool isCompleted();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  sysuint_t numWorkers;               // Count of workers used in engine.

  Atomic<sysuint_t> finishedWorkers;  // Count of workers finished (used to quit).
  Atomic<sysuint_t> waitingWorkers;   // Count of workers waiting (for calculation).

  Lock lock;                          // Lock for synchronization primitives.
  ThreadCondition allFinishedCondition;
  ThreadEvent* releaseEvent;

  Thread* threads[RASTER_MAX_WORKERS];
  Static<RasterPaintTask> tasks[RASTER_MAX_WORKERS];

  // Commands and calculations allocator.
  ZoneAllocator zoneAllocator;

  // Commands manager.
  volatile sysint_t cmdPosition;
  RasterPaintCmd* volatile cmdData[RASTER_MAX_COMMANDS];

  // Calculations manager.
  volatile sysint_t calcPosition;
  RasterPaintCalc* volatile calcData[RASTER_MAX_CALCULATIONS];
};

// ============================================================================
// [Fog::RasterPaintEngine]
// ============================================================================

struct FOG_HIDDEN RasterPaintEngine : public PaintEngine
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create new instance of @c RasterPaintEngine.
  //!
  //! @param buffer Buffer where to painting will be performed.
  //! @param imaged Image data (can be @c NULL), only for increasing and 
  //! decreasing ImageData::inUse member.
  //! @param initFlags Initialization flags, see @c PAINTER_INIT_FLAGS.
  RasterPaintEngine(const ImageBuffer& buffer, ImageData* imaged, uint32_t initFlags);

  //! @brief Destroy paint engine, decreasing @c ImageData::inUse flag (if set).
  virtual ~RasterPaintEngine();

  // --------------------------------------------------------------------------
  // [Width / Height / Format]
  // --------------------------------------------------------------------------

  virtual int getWidth() const;
  virtual int getHeight() const;
  virtual uint32_t getFormat() const;

  // --------------------------------------------------------------------------
  // [Engine / Flush]
  // --------------------------------------------------------------------------

  virtual uint32_t getEngine() const;
  virtual void setEngine(uint32_t engine, uint32_t cores = 0);

  virtual void flush(uint32_t flags);
  void flushWithQuit();

  FOG_INLINE bool isSingleThreaded() const { return workerManager == NULL; }

  // --------------------------------------------------------------------------
  // [Hints]
  // --------------------------------------------------------------------------

  virtual int getHint(uint32_t hint) const;
  virtual err_t setHint(uint32_t hint, int value);

  // --------------------------------------------------------------------------
  // [Meta]
  // --------------------------------------------------------------------------

  virtual void setMetaVars(const Region& region, const IntPoint& origin);
  virtual void resetMetaVars();

  virtual void setUserVars(const Region& region, const IntPoint& origin);
  virtual void setUserOrigin(const IntPoint& origin, uint32_t originOp);
  virtual void resetUserVars();

  virtual Region getMetaRegion() const;
  virtual Region getUserRegion() const;

  virtual IntPoint getMetaOrigin() const;
  virtual IntPoint getUserOrigin() const;

  // --------------------------------------------------------------------------
  // [Operator]
  // --------------------------------------------------------------------------

  virtual uint32_t getOperator() const;
  virtual err_t setOperator(uint32_t op);

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  virtual uint32_t getSourceType() const;

  virtual Argb getSourceArgb() const;
  virtual Pattern getSourcePattern() const;

  virtual err_t setSource(Argb argb);
  virtual err_t setSource(const Pattern& pattern);

  // --------------------------------------------------------------------------
  // [Fill Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getFillMode() const;
  virtual err_t setFillMode(uint32_t mode);

  // --------------------------------------------------------------------------
  // [Stroke Parameters]
  // --------------------------------------------------------------------------

  virtual StrokeParams getStrokeParams() const;
  virtual err_t setStrokeParams(const StrokeParams& strokeParams);

  virtual double getLineWidth() const;
  virtual err_t setLineWidth(double lineWidth);

  virtual uint32_t getStartCap() const;
  virtual err_t setStartCap(uint32_t startCap);

  virtual uint32_t getEndCap() const;
  virtual err_t setEndCap(uint32_t endCap);

  virtual err_t setLineCaps(uint32_t lineCaps);

  virtual uint32_t getLineJoin() const;
  virtual err_t setLineJoin(uint32_t lineJoin);

  virtual double getMiterLimit() const;
  virtual err_t setMiterLimit(double miterLimit);

  virtual List<double> getDashes() const;
  virtual err_t setDashes(const double* dashes, sysuint_t count);
  virtual err_t setDashes(const List<double>& dashes);

  virtual double getDashOffset() const;
  virtual err_t setDashOffset(double offset);

  // --------------------------------------------------------------------------
  // [Transformations]
  // --------------------------------------------------------------------------

  virtual DoubleMatrix getMatrix() const;
  virtual void setMatrix(const DoubleMatrix& m);
  virtual void resetMatrix();

  virtual void rotate(double angle, uint32_t order);
  virtual void scale(double sx, double sy, uint32_t order);
  virtual void skew(double sx, double sy, uint32_t order);
  virtual void translate(double x, double y, uint32_t order);
  virtual void transform(const DoubleMatrix& m, uint32_t order);

  virtual void worldToScreen(DoublePoint* pt) const;
  virtual void screenToWorld(DoublePoint* pt) const;

  virtual void alignPoint(DoublePoint* pt) const;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  virtual void save();
  virtual void restore();

  // --------------------------------------------------------------------------
  // [Raster Drawing]
  // --------------------------------------------------------------------------

  virtual void drawPoint(const IntPoint& p);
  virtual void drawLine(const IntPoint& start, const IntPoint& end);
  virtual void drawRect(const IntRect& r);
  virtual void drawRound(const IntRect& r, const IntPoint& radius);
  virtual void fillRect(const IntRect& r);
  virtual void fillRects(const IntRect* r, sysuint_t count);
  virtual void fillRound(const IntRect& r, const IntPoint& radius);
  virtual void fillRegion(const Region& region);

  virtual void fillAll();

  // --------------------------------------------------------------------------
  // [Vector Drawing]
  // --------------------------------------------------------------------------

  virtual void drawPoint(const DoublePoint& p);
  virtual void drawLine(const DoublePoint& start, const DoublePoint& end);
  virtual void drawLine(const DoublePoint* pts, sysuint_t count);
  virtual void drawPolygon(const DoublePoint* pts, sysuint_t count);
  virtual void drawRect(const DoubleRect& r);
  virtual void drawRects(const DoubleRect* r, sysuint_t count);
  virtual void drawRound(const DoubleRect& r, const DoublePoint& radius);
  virtual void drawEllipse(const DoublePoint& cp, const DoublePoint& r);
  virtual void drawArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep);
  virtual void drawPath(const DoublePath& path);

  virtual void fillPolygon(const DoublePoint* pts, sysuint_t count);
  virtual void fillRect(const DoubleRect& r);
  virtual void fillRects(const DoubleRect* r, sysuint_t count);
  virtual void fillRound(const DoubleRect& r, const DoublePoint& radius);
  virtual void fillEllipse(const DoublePoint& cp, const DoublePoint& r);
  virtual void fillArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep);
  virtual void fillPath(const DoublePath& path);

  // --------------------------------------------------------------------------
  // [Glyph / Text Drawing]
  // --------------------------------------------------------------------------

  virtual void drawGlyph(const IntPoint& pt, const Glyph& glyph, const IntRect* clip);
  virtual void drawGlyphSet(const IntPoint& pt, const GlyphSet& glyphSet, const IntRect* clip);

  virtual void drawText(const IntPoint& p, const String& text, const Font& font, const IntRect* clip);
  virtual void drawText(const IntRect& r, const String& text, const Font& font, uint32_t align, const IntRect* clip);

  // --------------------------------------------------------------------------
  // [Image drawing]
  // --------------------------------------------------------------------------

  virtual void blitImage(const IntPoint& p, const Image& image, const IntRect* irect);
  virtual void blitImage(const DoublePoint& p, const Image& image, const IntRect* irect);

  // --------------------------------------------------------------------------
  // [Helpers]
  // --------------------------------------------------------------------------

  err_t switchTo(const ImageBuffer& buffer, ImageData* d);

  void _updateWorkRegion();
  void _updateTransform(bool translationOnly);

  void _setClipDefaults();
  void _setCapsDefaults();

  RasterEngine::PatternContext* _getPatternContext();
  void _resetPatternContext();

  FOG_INLINE void _updateLineWidth()
  {
    ctx.hints.lineIsSimple = (
      ctx.strokeParams.getLineWidth() == 1.0 &&
      ctx.strokeParams.getDashes().getLength() == 0);
  }

  void _deleteStates();

  // --------------------------------------------------------------------------
  // [Layer]
  // --------------------------------------------------------------------------

  //! @brief Setup layer.
  //!
  //! Fills layer secondaryFormat, toSecondary and fromSecondary members.
  void _setupLayer(RasterPaintLayer* layer);
 
  // --------------------------------------------------------------------------
  // [Serializers]
  // --------------------------------------------------------------------------

  // Serializers are always called from painter thread.
  void _serializeBoxes(const IntBox* box, sysuint_t count);
  void _serializeImage(const IntRect& dst, const Image& image, const IntRect& src);
  void _serializeImageAffine(const DoublePoint& pt, const Image& image, const IntRect* irect);
  void _serializeGlyphSet(const IntPoint& pt, const GlyphSet& glyphSet, const IntRect* clip);
  void _serializePath(const DoublePath& path, bool stroke);

  template<typename T> FOG_INLINE T* _createCommand(sysuint_t size = sizeof(T));
  template<typename T> FOG_INLINE T* _createCalc(sysuint_t size = sizeof(T));

  FOG_INLINE void _beforeNewAction();
  void _postCommand(RasterPaintCmd* cmd, RasterPaintCalc* clc = NULL);

  // --------------------------------------------------------------------------
  // [Rasterization]
  // --------------------------------------------------------------------------

  //! @brief Rasterizer used only by singlethreaded mode by default rasterizer.
  bool _rasterizePath(const DoublePath& path, bool stroke);

  // --------------------------------------------------------------------------
  // [Renderering]
  // --------------------------------------------------------------------------

  static void _renderBoxes(RasterPaintContext* ctx, const IntBox* box, sysuint_t count);
  static void _renderImage(RasterPaintContext* ctx, const IntRect& dst, const Image& image, const IntRect& src);
  static void _renderImageAffineBound(RasterPaintContext* ctx, const DoublePoint& pt, const Image& image);
  static void _renderGlyphSet(RasterPaintContext* ctx, const IntPoint& pt, const GlyphSet& glyphSet, const IntBox& boundingBox);
  static void _renderPath(RasterPaintContext* ctx, Rasterizer* ras);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Custom memory allocator instance used by the raster paint engine
  //! for the most memory allocations.
  BlockAllocator blockAllocator;

  //! @brief Main raster context.
  RasterPaintMainContext ctx;

  //! @brief States stack (for @c save() and @c restore() methods).
  List<RasterPaintState*> states;

  // If we are running in single-core environment it's better to use one
  // rasterizer for everythging.
  Rasterizer* ras;

  // Multithreading
  RasterWorkerManager* workerManager;

  // Temporary path.
  DoublePath tmpPath;

  // Temporary glyph set.
  GlyphSet tmpGlyphSet;

  // Temporary region.
  Region tmpRegion;
};

// ============================================================================
// [Inline]
// ============================================================================

FOG_INLINE void RasterPaintContext::copyFromMaster(const RasterPaintContext& master)
{
  ops = master.ops;
  layer = master.layer;

  solid = master.solid;
  pctx = NULL;

  funcs = master.funcs;
  closure = master.closure;

  metaOrigin = master.metaOrigin;
  metaRegion = master.metaRegion;

  workOrigin = master.workOrigin;
  workRegion = master.workRegion;

  clipBox = master.clipBox;
}

FOG_INLINE uint8_t* RasterPaintContext::getBuffer(sysuint_t size)
{
  return (size < bufferSize) ? buffer : reallocBuffer(size);
}

FOG_INLINE void RasterPaintCmdDraw::_initPaint(RasterPaintContext* ctx)
{
  ops = ctx->ops;

  if (FOG_LIKELY(ops.sourceType == PAINTER_SOURCE_ARGB))
  {
    solid = ctx->solid;
  }
  else // if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN)
  {
    // Pattern context must be initialized if we are here.
    FOG_ASSERT(ctx->pctx && ctx->pctx->initialized);
    pctx = ctx->pctx;
    pctx->refCount.inc();
  }
}

FOG_INLINE void RasterPaintCmdDraw::_beforeBlit(RasterPaintContext* ctx)
{
  FOG_ASSERT(ops.sourceType == PAINTER_SOURCE_ARGB);

  ctx->ops = ops;
  ctx->funcs = RasterEngine::getCompositeFuncs(ctx->layer.format, ops.op);
}

FOG_INLINE void RasterPaintCmdDraw::_beforePaint(RasterPaintContext* ctx)
{
  ctx->ops = ops;
  ctx->funcs = RasterEngine::getCompositeFuncs(ctx->layer.format, ops.op);

  if (FOG_LIKELY(ops.sourceType == PAINTER_SOURCE_ARGB))
    ctx->solid = solid;
  else
    ctx->pctx = pctx;
}

FOG_INLINE void RasterPaintCmdDraw::_afterPaint(RasterPaintContext* ctx)
{
  ctx->pctx = NULL;
}

FOG_INLINE void RasterPaintCmdDraw::_initBlit(RasterPaintContext* ctx)
{
  ops = ctx->ops;
  ops.sourceType = PAINTER_SOURCE_ARGB;
}

FOG_INLINE void RasterPaintCmdDraw::_releasePattern(RasterPaintContext* ctx)
{
  if (ops.sourceType == PAINTER_SOURCE_PATTERN && pctx->refCount.deref())
  {
    pctx->destroy(pctx);
    ctx->engine->blockAllocator.free(pctx);
  }
}

template <typename T>
FOG_INLINE T* RasterPaintEngine::_createCommand(sysuint_t size)
{
  T* command = reinterpret_cast<T*>(workerManager->zoneAllocator.alloc(size));
  if (!command) return NULL;

  new(command) T;

  command->refCount.init((uint)workerManager->numWorkers);
  command->status.init(RASTER_COMMAND_READY);

  command->calculation = NULL;

  return command;
}

template<typename T>
T* RasterPaintEngine::_createCalc(sysuint_t size)
{
  T* calculation = reinterpret_cast<T*>(workerManager->zoneAllocator.alloc(size));
  if (!calculation) return NULL;

  new(calculation) T;
  calculation->engine = this;
  return calculation;
}

FOG_INLINE void RasterPaintEngine::_beforeNewAction()
{
  // Flush everything if commands get maximum. We need to ensure that there are
  // at least two commands left, because _postCommand() can create one change
  // clip / region / something else command.
  if (workerManager->cmdPosition >= RASTER_MAX_COMMANDS - 1)
  {
#if defined(FOG_DEBUG_RASTER_COMMANDS)
    fog_debug("Fog::Painter::_postCommand() - command buffer is full");
#endif // FOG_DEBUG_RASTER_COMMANDS
    flush(PAINTER_FLUSH_SYNC);
  }
}

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_PAINTERENGINE_RASTER_P_H
