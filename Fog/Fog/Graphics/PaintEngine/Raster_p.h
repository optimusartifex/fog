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

struct RasterPaintCalc;
struct RasterPaintCapsState;
struct RasterPaintClipState;
struct RasterPaintCmd;
struct RasterPaintContext;
struct RasterPaintLayer;
struct RasterPaintStoredState;
struct RasterPaintWorkerManager;
struct RasterPaintTask;

struct RasterPaintEngine;

// ============================================================================
// [Constants]
// ============================================================================

//! @brief Type of current painter layer
enum LAYER_TYPE
{
  //! @brief Direct 32-bit painting and compositing (PRGB32, XRGB32).
  //!
  //! This layer is te most efficient layer in Fog-Framework.
  //!
  //! Pixel format summary:
  //! - @c PIXEL_FORMAT_PRGB32
  //! - @c PIXEL_FORMAT_XRGB32
  LAYER_TYPE_DIRECT32 = 0,

  //! @brief Indirect 32-bit painting and compositing (ARGB32).
  //!
  //! This layer is less efficient than @c LAYER_TYPE_DIRECT32, because each
  //! pixel needs to be premultiplied before compositing and demultiplied back
  //! after it has been processed.
  //!
  //! Pixel format summary:
  //! - @c PIXEL_FORMAT_ARGB32
  LAYER_TYPE_INDIRECT32 = 1
};

//! @brief Type of current transform in raster paint engine.
enum TRANSFORM_TYPE
{
  //! @brief Transformation matrix is identity or translation only.
  //!
  //! If translation part of matrix is used the values are aligned to pixel
  //! grid (this means that integral numbers can be used for this part). This
  //! is most efficient transform type in simple cases (fill rect, blit image,
  //! etc...).
  TRANSFORM_TRANSLATE_EXACT = 0,

  //! @brief Transformation matrix is identity except translation part which
  //! can be represented only by real numbers. Painter has complex fast paths
  //! for this type, but it's used mainly by image blitting and texture
  //! fetching.
  TRANSFORM_TRANSLATE_SUBPX = 1,

  //! @brief Transformation matrix is affine (not identity or simple translation).
  TRANSFORM_AFFINE = 2
};

//! @brief Status of the raster paint engine command.
//!
//! Status of command is only used if it has associated calculation.
enum COMMAND_STATUS
{
  //! @brief Wait until calculation is done.
  COMMAND_STATUS_WAIT = 0,
  //! @brief Ready to process the command.
  COMMAND_STATUS_READY = 1,
  //! @brief Skip this command - don't call @c RasterPaintAction::run().
  COMMAND_STATUS_SKIP = 2
};

// ============================================================================
// [Fog::MemoryAllocator]
// ============================================================================

//! @brief Memory allocator used by raster paint engine.
//!
//! This allocator allocates larger blocks (see @c BLOCK_SIZE) dividing them
//! into small pieces demanded throught @c MemoryAllocator::alloc() method.
//! Each allocation contains information about memory block used by allocator
//! and when the memory is not needed (and @c MemoryAllocator::free() is called)
//! it's atomically removed from memory block.
//!
//! In short: Each memory block has information about used memory, increased by
//! @c MemoryAllocator::alloc() and decreased by @c MemoryAllocator::free().
//! When the number is decreased to zero then the block is free and will be 
//! reused. The goal of this algorithm is to provide fast memory alloc/free,
//! but do not eat too much memory (reuse it).
//!
//! @note The @c MemoryAllocator::alloc() is reentrant and can be called only
//! by main thread, but @c MemoryAllocator::free() is thread-safe and it can
//! be called (and it is) from worker threads.
struct FOG_HIDDEN MemoryAllocator
{
  // --------------------------------------------------------------------------
  // [Constants]
  // --------------------------------------------------------------------------

  //! @brief Size of one block (decreased by some value to always fit in 
  //! one/two pages). 96 bytes reserved for @c ::malloc() data and our
  //! @c Block data.
  enum { BLOCK_SIZE = 8096 };

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
    //! @brief Allocator position, incremented by each @c MemoryAllocator::alloc().
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
  MemoryAllocator();
  //! @brief Memory allocator destructor, will check if all blocks are freed.
  ~MemoryAllocator();

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
  FOG_DISABLE_COPY(MemoryAllocator)
};

FOG_INLINE void MemoryAllocator::free(void* ptr)
{
  Header* header = reinterpret_cast<Header*>(reinterpret_cast<uint8_t*>(ptr) - sizeof(Header));
  header->block->used.sub(header->size);
}

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
// [Fog::RasterPaintClipState]
// ============================================================================

//! @brief Raster paint engine clipping state.
struct FOG_HIDDEN RasterPaintClipState
{
  // ------------------------------------------------------------------------
  // [Construction / Destruction]
  // ------------------------------------------------------------------------

  RasterPaintClipState();
  RasterPaintClipState(const RasterPaintClipState& other);
  ~RasterPaintClipState();

  // ------------------------------------------------------------------------
  // [Ref]
  // ------------------------------------------------------------------------

  FOG_INLINE RasterPaintClipState* ref() const;

  // ------------------------------------------------------------------------
  // [Operator Overload]
  // ------------------------------------------------------------------------

  RasterPaintClipState& operator=(const RasterPaintClipState& other);

  // ------------------------------------------------------------------------
  // [Members]
  // ------------------------------------------------------------------------

  mutable Atomic<sysuint_t> refCount;

  IntPoint metaOrigin;
  IntPoint userOrigin;
  IntPoint workOrigin;

  Region metaRegion;
  Region userRegion;
  Region workRegion;

  IntBox clipBox;

  uint8_t metaRegionUsed;
  uint8_t userRegionUsed;
  uint8_t workRegionUsed;
  uint8_t clipSimple;
};

// ============================================================================
// [Fog::RasterPaintCapsState]
// ============================================================================

//! @brief Raster paint engine capabilities.
struct FOG_HIDDEN RasterPaintCapsState
{
  // ------------------------------------------------------------------------
  // [Construction / Destruction]
  // ------------------------------------------------------------------------

  RasterPaintCapsState();
  RasterPaintCapsState(const RasterPaintCapsState& other);
  ~RasterPaintCapsState();

  // ------------------------------------------------------------------------
  // [Ref]
  // ------------------------------------------------------------------------

  FOG_INLINE RasterPaintCapsState* ref() const;

  // ------------------------------------------------------------------------
  // [Operator Overload]
  // ------------------------------------------------------------------------

  RasterPaintCapsState& operator=(const RasterPaintCapsState& other);

  // ------------------------------------------------------------------------
  // [Members]
  // ------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;

  union
  {
    struct
    {
      //! @brief Compositing operator, see @c OPERATOR_TYPE.
      uint32_t op : 8;

      //! @brief Type of source, see @c PAINTER_SOURCE_TYPE.
      uint32_t sourceType : 8;

      //! @brief Type of transform, see @c TRANSFORM_TYPE.
      uint32_t transformType : 8;

      //! @brief Fill mode, see @c FILL_MODE.
      uint32_t fillMode : 8;

      //! @brief Anti-aliasing type / quality, see @c ANTI_ALIASING_TYPE.
      uint32_t aaQuality : 4;

      //! @brief Image interpolation type / quality, see @c INTERPOLATION_TYPE.
      uint32_t imageInterpolation : 4;

      //! @brief Gradient interpolation type / quality, see @c INTERPOLATION_TYPE.
      uint32_t gradientInterpolation : 4;

      //! @brief Whether to force vector text.
      uint32_t forceOutlineText : 4;

      //! @brief Whether line is simple (one pixel width and default caps).
      uint32_t lineIsSimple : 1;
    };

    //! @brief All packed data in one array (for fast copy).
    uint8_t data[8];
  };

  union
  {
    //! @brief Solid source data (if @c sourceType is @c PAINTER_SOURCE_SOLID).
    RasterEngine::Solid solid;
    //! @brief Pattern source data (if @c sourceType is @c PAINTER_SOURCE_PATTERN).
    Static<Pattern> pattern;
    //! @brief Color filter data (if @c sourceType is @c PAINTER_SOURCE_COLOR_FILTER).
    Static<ColorFilter> colorFilter;
  };

  //! @brief Pointer to compositing functions, see @c op.
  RasterEngine::FunctionMap::CompositeFuncs* rops;

  //! @brief Stroke parameters.
  StrokeParams strokeParams;

  //! @brief Transformation matrix.
  DoubleMatrix transform;
  //! @brief Transformation approximation scale used by path flattening.
  double approximationScale;

  //! @brief Saved transform matrix translation (tx and ty values).
  DoublePoint transformTranslateSaved;

  //! @brief Transformation translate point in pixels (can be used if
  //! transform type is @c TRANSFORM_TRANSLATE_EXACT).
  IntPoint transformTranslateInt;
};

// ============================================================================
// [Fog::RasterPaintStoredState]
// ============================================================================

//! @brief State structure used by save() and restore() methods.
struct FOG_HIDDEN RasterPaintStoredState
{
  RasterPaintClipState* clipState;
  RasterPaintCapsState* capsState;
  RasterEngine::PatternContext* pctx;
};

// ============================================================================
// [Fog::RasterPaintContext]
// ============================================================================

// RasterPaintContext is accessed always from only one thread.
struct FOG_HIDDEN RasterPaintContext
{
  // ------------------------------------------------------------------------
  // [Construction / Destruction]
  // ------------------------------------------------------------------------

  RasterPaintContext();
  ~RasterPaintContext();

  // ------------------------------------------------------------------------
  // [Buffer Manager]
  // ------------------------------------------------------------------------

  uint8_t* getBuffer(sysint_t size);

  // ------------------------------------------------------------------------
  // [Members]
  // ------------------------------------------------------------------------

  // Owner of this context.
  RasterPaintEngine* engine;

  // Layer.
  RasterPaintLayer* layer;
  // Clip state.
  RasterPaintClipState* clipState;
  // Capabilities state.
  RasterPaintCapsState* capsState;

  // Raster closure.
  RasterEngine::Closure closure;

  // Pattern context.
  RasterEngine::PatternContext* pctx;

  // Scanline rasterizer container.
  Scanline32 scanline;

  // Multithreading id, offset and delta.
  int id;     // If multithreading is disabled, id is -1.
  int offset; // If multithreading is disabled, offset is 0.
  int delta;  // If multithreading is disabled, delta is 1.

  // Static embedded buffer for fast alloc/free, see getBuffer().
  uint8_t* buffer;
  sysint_t bufferSize;
  uint8_t bufferStatic[1024*8];

private:
  FOG_DISABLE_COPY(RasterPaintContext)
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
    const IntBox& clipBox, int interpolationType);

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
  //! @brief Action constructor.
  FOG_INLINE RasterPaintAction() {}

  //! @brief Action destructor (non-virtual).
  //!
  //! It's not virtual! Use @c release() to destroy resources associated
  //! with the action.
  FOG_INLINE ~RasterPaintAction() {}

  // [Abstract]

  //! @brief Run action (command or calculation).
  virtual void run(RasterPaintContext* ctx) = 0;
  //! @brief Release action (it means free / dereference all associated
  //! resources with this action).
  virtual void release() = 0;

  //! @brief Free all common resources (called only by @c release() method).
  //!
  //! This is convenience method that is often used by @c release() method
  //! implementation. It's inlined for efficiency.
  FOG_INLINE void _free();

  // [Members]

  //! @brief Owner of this action.
  RasterPaintEngine* engine;

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
//! command status is @c COMMAND_STATUS_WAIT then thread can't process
//! current command until command status is set to @c COMMAND_STATUS_READY
//! or COMMAND_STATUS_SKIP (that means skip this command). Instead of waiting
//! the worker can work on different calculation, see @c RasterPaintTask::run()
//! implementation. Notice that command status is set by @c RasterPaintCalc if
//! used.
//!
//! After command is processed the @c RasterPaintAction::release() is called.
//! To create own command the method must be implemented and you must ensure
//! to call @c RasterPaintCmd::_releaseObjects() there. When writing command
//! concentrate only to @c RasterPaintAction::release() method, because all
//! other stuff should be done by worker task.
struct FOG_HIDDEN RasterPaintCmd : public RasterPaintAction
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPaintCmd() {};
  FOG_INLINE ~RasterPaintCmd() {};

  //! @brief This method destroys only general @c RasterPaintCmd data.
  //!
  //! Within this method the @c RasterPaintAction::_free() is not called, so
  //! make sure you call _free() after _releaseObjects().
  FOG_INLINE void _releaseObjects();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  //!
  //! Initial value is count of workers and each worker decreases it when
  //! finished. If value is decreased to zero worker must release the command
  //! using @c RasterPaintAction::release() method. This is done by worker task.
  Atomic<int> refCount;

  //! @brief Status of this command.
  Atomic<int> status;

  //! @brief Layer - contains info about raster.
  RasterPaintLayer* layer;
  //! @brief Clip state of the command.
  RasterPaintClipState* clipState;
  //! @brief Capabilities of the command.
  RasterPaintCapsState* capsState;

  //! @brief Pattern context (must be @c NULL if single color is used).
  //!
  //! TODOC
  RasterEngine::PatternContext* pctx;

  //! @brief Calculation.
  RasterPaintCalc* calculation;
};

// ============================================================================
// [Fog::RasterPaintCmdBoxes]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdBoxes : public RasterPaintCmd
{
  // [Construction / Destruction]

  FOG_INLINE RasterPaintCmdBoxes() {};
  FOG_INLINE ~RasterPaintCmdBoxes() {};

  // [Implementation]

  virtual void run(RasterPaintContext* ctx);
  virtual void release();

  // [Members]

  sysuint_t count;
  IntBox boxes[1];
};

// ============================================================================
// [Fog::RasterPaintCmdImage]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdImage : public RasterPaintCmd
{
  // [Construction / Destruction]

  FOG_INLINE RasterPaintCmdImage() {};
  FOG_INLINE ~RasterPaintCmdImage() {};

  // [Implementation]

  virtual void run(RasterPaintContext* ctx);
  virtual void release();

  // [Members]

  Static<Image> image;

  IntRect dst;
  IntRect src;
};

// ============================================================================
// [Fog::RasterPaintCmdImageAffineBound]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdImageAffineBound : public RasterPaintCmd
{
  // [Construction / Destruction]

  FOG_INLINE RasterPaintCmdImageAffineBound() { renderer.init(); };
  FOG_INLINE ~RasterPaintCmdImageAffineBound() {};

  // [Implementation]

  virtual void run(RasterPaintContext* ctx);
  virtual void release();

  // [Members]

  Static<RasterRenderImageAffineBound> renderer;
};

// ============================================================================
// [Fog::RasterPaintCmdGlyphSet]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdGlyphSet : public RasterPaintCmd
{
  // [Construction / Destruction]

  FOG_INLINE RasterPaintCmdGlyphSet() {};
  FOG_INLINE ~RasterPaintCmdGlyphSet() {};

  // [Implementation]

  virtual void run(RasterPaintContext* ctx);
  virtual void release();

  // [Members]

  Static<GlyphSet> glyphSet;

  IntPoint pt;
  IntBox boundingBox;
};

// ============================================================================
// [Fog::RasterPaintCmdPath]
// ============================================================================

struct FOG_HIDDEN RasterPaintCmdPath : public RasterPaintCmd
{
  // [Construction / Destruction]

  FOG_INLINE RasterPaintCmdPath() {};
  FOG_INLINE ~RasterPaintCmdPath() {};

  // [Implementation]

  virtual void run(RasterPaintContext* ctx);
  virtual void release();

  // [Members]

  //! @brief Rasterizer (where path is rasterized by @c RasterPaintCalc)
  Rasterizer* ras;

  //! @brief Whether it's texture blit command instead of draw/fill path
  //! command.
  //!
  //! Texture blit commands are serialized through @c _serializeImageAffine(),
  //! difference is that it's always used PAINTER_SOURCE_PATTERN type to do
  //! pattern based blit.
  int textureBlit;
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
  FOG_INLINE RasterPaintCalc() {};
  FOG_INLINE ~RasterPaintCalc() {};

  //! @brief Related command to this calculation.
  RasterPaintCmd* relatedTo;
};

// ============================================================================
// [Fog::RasterPaintCalcPath]
// ============================================================================

struct FOG_HIDDEN RasterPaintCalcPath : public RasterPaintCalc
{
  FOG_INLINE RasterPaintCalcPath() {};
  FOG_INLINE ~RasterPaintCalcPath() {};

  // [Implementation]

  virtual void run(RasterPaintContext* ctx);
  virtual void release();

  // [Members]

  //! @brief Owner raster paint engine.
  RasterPaintEngine* engine;

  //! @brief Path to process.
  Static<DoublePath> path;
  bool stroke;
};

// ============================================================================
// [Fog::RasterPaintTask]
// ============================================================================

// This is task created per painter thread that contains all variables needed
// to process painter commands in parallel. The goal is that condition variable
// is shared across all painter threads so one signal will wake them all.
struct FOG_HIDDEN RasterPaintTask : public Task
{
  RasterPaintTask(Lock* condLock);
  virtual ~RasterPaintTask();

  virtual void run();
  virtual void destroy();

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
// [Fog::RasterPaintWorkerManager]
// ============================================================================

// Structure shared across all workers (threads).
struct FOG_HIDDEN RasterPaintWorkerManager
{
  // [Construction / Destruction]

  RasterPaintWorkerManager();
  ~RasterPaintWorkerManager();

  // [Methods]

  // To call the lock must be locked!
  RasterPaintTask* wakeUpScheduled(RasterPaintTask* calledFrom);
  RasterPaintTask* wakeUpSleeping(RasterPaintTask* calledFrom);
  bool isCompleted();

  // [Members]

  sysuint_t numWorkers;               // Count of workers used in engine.

  Atomic<sysuint_t> finishedWorkers;  // Count of workers finished (used to quit).
  Atomic<sysuint_t> waitingWorkers;   // Count of workers waiting (for calculation).

  Lock lock;                          // Lock for synchronization primitives.
  ThreadCondition allFinishedCondition;
  ThreadEvent* releaseEvent;

  Thread* threads[RASTER_MAX_WORKERS];
  Static<RasterPaintTask> tasks[RASTER_MAX_WORKERS];

  // Commands and calculations allocator.
  MemoryAllocator allocator;

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

  RasterPaintEngine(const ImageBuffer& buffer, uint32_t initFlags);
  virtual ~RasterPaintEngine();

  // --------------------------------------------------------------------------
  // [Width / Height / Format]
  // --------------------------------------------------------------------------

  virtual int getWidth() const;
  virtual int getHeight() const;
  virtual int getFormat() const;

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
  virtual void setHint(uint32_t hint, int value);

  // --------------------------------------------------------------------------
  // [Meta]
  // --------------------------------------------------------------------------

  virtual void setMetaVariables(
    const IntPoint& metaOrigin,
    const Region& metaRegion,
    bool useMetaRegion,
    bool reset);

  virtual void setMetaOrigin(const IntPoint& pt);
  virtual void setUserOrigin(const IntPoint& pt);

  virtual void translateMetaOrigin(const IntPoint& pt);
  virtual void translateUserOrigin(const IntPoint& pt);

  virtual void setUserRegion(const IntRect& r);
  virtual void setUserRegion(const Region& r);

  virtual void resetMetaVars();
  virtual void resetUserVars();

  virtual IntPoint getMetaOrigin() const;
  virtual IntPoint getUserOrigin() const;

  virtual Region getMetaRegion() const;
  virtual Region getUserRegion() const;

  virtual bool isMetaRegionUsed() const;
  virtual bool isUserRegionUsed() const;

  // --------------------------------------------------------------------------
  // [Operator]
  // --------------------------------------------------------------------------

  virtual uint32_t getOperator() const;
  virtual void setOperator(uint32_t op);

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  virtual uint32_t getSourceType() const;

  virtual err_t getSourceArgb(Argb& argb) const;
  virtual err_t getSourcePattern(Pattern& pattern) const;

  virtual void setSource(Argb argb);
  virtual void setSource(const Pattern& pattern);
  virtual void setSource(const ColorFilter& colorFilter);

  // --------------------------------------------------------------------------
  // [Fill Parameters]
  // --------------------------------------------------------------------------

  virtual uint32_t getFillMode() const;
  virtual void setFillMode(uint32_t mode);

  // --------------------------------------------------------------------------
  // [Stroke Parameters]
  // --------------------------------------------------------------------------

  virtual void getStrokeParams(StrokeParams& strokeParams) const;
  virtual void setStrokeParams(const StrokeParams& strokeParams);

  virtual double getLineWidth() const;
  virtual void setLineWidth(double lineWidth);

  virtual uint32_t getStartCap() const;
  virtual void setStartCap(uint32_t startCap);

  virtual uint32_t getEndCap() const;
  virtual void setEndCap(uint32_t endCap);

  virtual void setLineCaps(uint32_t lineCaps);

  virtual uint32_t getLineJoin() const;
  virtual void setLineJoin(uint32_t lineJoin);

  virtual double getMiterLimit() const;
  virtual void setMiterLimit(double miterLimit);

  virtual List<double> getDashes() const;
  virtual void setDashes(const double* dashes, sysuint_t count);
  virtual void setDashes(const List<double>& dashes);

  virtual double getDashOffset() const;
  virtual void setDashOffset(double offset);

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

  virtual void clear();
  virtual void drawPoint(const IntPoint& p);
  virtual void drawLine(const IntPoint& start, const IntPoint& end);
  virtual void drawRect(const IntRect& r);
  virtual void drawRound(const IntRect& r, const IntPoint& radius);
  virtual void fillRect(const IntRect& r);
  virtual void fillRects(const IntRect* r, sysuint_t count);
  virtual void fillRound(const IntRect& r, const IntPoint& radius);
  virtual void fillRegion(const Region& region);

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

  void _updateWorkRegion();
  void _updateTransform(bool translationOnly);

  void _setClipDefaults();
  void _setCapsDefaults();

  RasterEngine::PatternContext* _getPatternRasterPaintContext();
  void _resetPatternRasterPaintContext();

  FOG_INLINE void _updateLineWidth()
  {
    ctx.capsState->lineIsSimple = (
      ctx.capsState->strokeParams.getLineWidth() == 1.0 &&
      ctx.capsState->strokeParams.getDashes().getLength() == 0);
  }

  RasterPaintCapsState* _detachCapsState();
  RasterPaintClipState* _detachClipState();

  FOG_INLINE void _derefClipState(RasterPaintClipState* clipState);
  FOG_INLINE void _derefCapsState(RasterPaintCapsState* capsState);

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
  template<typename T> FOG_INLINE T* _createCommand(sysuint_t size, RasterEngine::PatternContext* pctx);
  template<typename T> FOG_INLINE T* _createCalc(sysuint_t size = sizeof(T));

  void _postCommand(RasterPaintCmd* cmd, RasterPaintCalc* clc = NULL);

  // --------------------------------------------------------------------------
  // [Rasterization]
  // --------------------------------------------------------------------------

  static bool _rasterizePath(RasterPaintContext* ctx, Rasterizer* ras, const DoublePath& path, bool stroke);

  // --------------------------------------------------------------------------
  // [Renderering]
  // --------------------------------------------------------------------------

  static void _renderBoxes(RasterPaintContext* ctx, const IntBox* box, sysuint_t count);
  static void _renderImage(RasterPaintContext* ctx, const IntRect& dst, const Image& image, const IntRect& src);
  static void _renderImageAffineBound(RasterPaintContext* ctx, const DoublePoint& pt, const Image& image);
  static void _renderGlyphSet(RasterPaintContext* ctx, const IntPoint& pt, const GlyphSet& glyphSet, const IntBox& boundingBox);
  static void _renderPath(RasterPaintContext* ctx, Rasterizer* ras, bool textureBlit);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RasterPaintLayer main;

  MemoryAllocator allocator;

  // Temporary path.
  DoublePath tmpPath;

  // Temporary glyph set.
  GlyphSet tmpGlyphSet;

  // RasterPaintContext that is used by single-threaded painter.
  RasterPaintContext ctx;

  // RasterPaintContext states LIFO buffer (for save() and restore() methods)
  List<RasterPaintStoredState> states;

  // If we are running in single-core environment it's better to use one
  // rasterizer for everythging.
  Rasterizer* ras;

  // Multithreading
  RasterPaintWorkerManager* workerManager;
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_PAINTERENGINE_RASTER_P_H
