// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERPAINTFUNCS_P_H
#define _FOG_G2D_PAINTING_RASTERPAINTFUNCS_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterPaintConstants_p.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>
#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting_Raster
//! @{

// ============================================================================
// [Fog::RasterFuncs]
// ============================================================================

struct FOG_NO_EXPORT RasterFuncs
{
  struct FOG_NO_EXPORT _EngineFuncs
  {
    typedef void (FOG_FASTCALL *DoRectI)(RasterPaintEngine* self, const BoxI& box);
    typedef void (FOG_FASTCALL *DoRectF)(RasterPaintEngine* self, const BoxF& box);
    typedef void (FOG_FASTCALL *DoRectD)(RasterPaintEngine* self, const BoxD& box);

    typedef void (FOG_FASTCALL *DoPathF)(RasterPaintEngine* self, const PathF& path);
    typedef void (FOG_FASTCALL *DoPathD)(RasterPaintEngine* self, const PathD& path);

    // --------------------------------------------------------------------------
    // [Fill]
    // --------------------------------------------------------------------------

    struct _FillFuncs
    {
      DoRectI rectI;
      DoRectF rectF;
      DoRectD rectD;

      DoPathF pathF;
      DoPathD pathD;
    } fill;

    // --------------------------------------------------------------------------
    // [Stroke]
    // --------------------------------------------------------------------------

    struct _StrokeFuncs
    {
      DoPathF pathF;
      DoPathD pathD;
    } stroke;

    // --------------------------------------------------------------------------
    // [Paint]
    // --------------------------------------------------------------------------

    struct _PaintFuncs
    {
      DoPathF pathF;
      DoPathD pathD;
    } paint;
  } engine;

  struct _OpCodeFuncs
  {
    struct _CmdFuncs
    {
      RasterRunFn run[RASTER_CMD_OPCODE_COUNT];
      RasterFinalizeFn finalize[RASTER_CMD_OPCODE_COUNT];
    } cmd;

    struct _CalcFuncs
    {
      RasterRunFn run[RASTER_CALC_OPCODE_COUNT];
      RasterFinalizeFn finalize[RASTER_CALC_OPCODE_COUNT];
    } calc;
  } opCode;
};

extern FOG_API RasterFuncs _raster;

// ============================================================================
// [Fog::RasterCmd]
// ============================================================================

//! @internal
//!
//! @brief Raster painter engine command.
//!
//! Command is action that is executed by each painter's thread. To make sure
//! that threads are working with own data the @c run() method must use per
//! thread context (@c RasterContext class).
//!
//! Commands are executed in FIFO (first-in, first-out) order for each thread,
//! but threads are not synchronized. This means that threads may process
//! different commands in parallel, but commands order per thread is preserved.
//!
//! Command can depend to calculation (@c RasterCalc class). If calculation
//! is not @c NULL and calculation status is @c RASTER_CALC_STATUS_WAIT then
//! thread can't process the command until the status is set into
//! @c RASTER_CALC_STATUS_READY or @c RASTER_CALC_STATUS_SKIP (that means skip
//! this command). Instead of waiting the worker can work on different
//! calculation, see @c RasterWorker::run() implementation. Notice that
//! calculation status is set by @c RasterCalc if used.
//!
//! After command is processed the @c RasterCmd::finalize() method is called.
//! To create own command the method must be implemented and you must ensure
//! to call @c RasterCmd::_releasePattern() for painting commands. When
//! writing command concentrate only to @c RasterCmd::release() method,
//! because all other stuff should be done by a worker.
struct FOG_NO_EXPORT RasterCmd
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getOpCode() const
  {
    return opCode;
  }

  FOG_INLINE void* getData() const
  {
    return (void*)( (uint8_t*)this + sizeof(*this) );
  }

  // --------------------------------------------------------------------------
  // [Run / Finalize]
  // --------------------------------------------------------------------------

  FOG_INLINE void run(RasterContext* ctx)
  {
    _raster.opCode.cmd.run[opCode](ctx, getData());
  }

  FOG_INLINE void finalize(RasterContext* ctx)
  {
    _raster.opCode.cmd.finalize[opCode](ctx, getData());
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t opCode;
};

// ============================================================================
// [Fog::RasterCalc]
// ============================================================================

//! @internal
//!
//! @brief Raster painter engine calculation.
//!
//! Calculation is complex action (usually sequental) that can be run only by
//! one thread. To improve the performance of such actions, raster painter
//! engine can execute several calculations in different threads.
struct FOG_NO_EXPORT RasterCalc
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getOpCode() const
  {
    return opCode;
  }

  FOG_INLINE void* getData() const
  {
    return (void*)( (uint8_t*)this + sizeof(*this) );
  }

  // --------------------------------------------------------------------------
  // [Run / Finalize]
  // --------------------------------------------------------------------------

  FOG_INLINE void run(RasterContext* ctx)
  {
    _raster.opCode.calc.run[opCode](ctx, getData());
  }

  FOG_INLINE void finalize(RasterContext* ctx)
  {
    _raster.opCode.calc.finalize[opCode](ctx, getData());
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t opCode;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTFUNCS_P_H
