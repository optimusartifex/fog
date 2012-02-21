// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERPAINTCMD_P_H
#define _FOG_G2D_PAINTING_RASTERPAINTCMD_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageFilter.h>
#include <Fog/G2d/Imaging/ImageFilterScale.h>
#include <Fog/G2d/Painting/PaintParams.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>
#include <Fog/G2d/Painting/RasterStructs_p.h>
#include <Fog/G2d/Tools/Region.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::RasterPaintCmd]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd) { _setCommand(cmd); }
  FOG_INLINE void destroy(RasterPaintEngine* engine) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t getCommand() const { return _command; }
  FOG_INLINE void _setCommand(uint8_t command) { _command = command; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Command bytecode.
  uint32_t _command : 8;
  //! @brief Command embedded data (24-bits).
  uint32_t _data24 : 24;
};

// ============================================================================
// [Fog::RasterPaintCmd_Next]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_Next : public RasterPaintCmd
{
  typedef RasterPaintCmd Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, uint8_t* ptr)
  {
    Base::init(engine, cmd);
    _setPtr(ptr);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  { 
    Base::destroy(engine);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t* getPtr() const { return _ptr; }
  FOG_INLINE void _setPtr(uint8_t* ptr) { _ptr = ptr; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t* _ptr;
};

// ============================================================================
// [Fog::RasterPaintCmd_SetPaintHints]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_SetPaintHints : public RasterPaintCmd
{
  typedef RasterPaintCmd Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd,
    const PaintHints& paintHints)
  {
    Base::init(engine, cmd);
    _setPaintHints(paintHints);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PaintHints& getPaintHints() const { return _paintHints; }
  FOG_INLINE void _setPaintHints(const PaintHints& paintHints) { _paintHints.packed = paintHints.packed; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PaintHints _paintHints;
};

// ============================================================================
// [Fog::RasterPaintCmd_SetOpacity]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_SetOpacity : public RasterPaintCmd
{
  typedef RasterPaintCmd Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, uint32_t opacity)
  {
    Base::init(engine, cmd);
    _setOpacity(opacity);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getOpacity() const { return _data24; }
  FOG_INLINE void _setOpacity(uint32_t opacity) { _data24 = opacity; }
};

// ============================================================================
// [Fog::RasterPaintCmd_SetOpacityAndPrgb32]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_SetOpacityAndPrgb32 :
  public RasterPaintCmd_SetOpacity
{
  typedef RasterPaintCmd_SetOpacity Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, uint32_t opacity, uint32_t prgb32)
  {
    Base::init(engine, cmd, opacity);
    _setPrgb32(prgb32);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getPrgb32() const { return _prgb32; }
  FOG_INLINE void _setPrgb32(uint32_t prgb32) { _prgb32 = prgb32; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _prgb32;
};

// ============================================================================
// [Fog::RasterPaintCmd_SetOpacityAndPattern]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_SetOpacityAndPattern :
  public RasterPaintCmd_SetOpacity
{
  typedef RasterPaintCmd_SetOpacity Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, uint32_t opacity, RasterPattern* pc)
  {
    Base::init(engine, cmd, opacity);
    
    FOG_ASSERT(pc != NULL);
    _pc = pc;
    _pc->_reference.inc();
  }

  // Implemented-Later: RasterPaintEngine_p.h
  FOG_INLINE void destroy(RasterPaintEngine* engine);

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE RasterPattern* getPatternContext() const { return _pc; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RasterPattern* _pc;
};

// ============================================================================
// [Fog::RasterPaintCmd_Fill]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_Fill : public RasterPaintCmd
{
  typedef RasterPaintCmd Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, uint32_t fillRule)
  {
    Base::init(engine, cmd);
    _setFillRule(fillRule); 
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getFillRule() const { return _data24; }
  FOG_INLINE void _setFillRule(uint32_t fillRule) { _data24 = fillRule; }
};

// ============================================================================
// [Fog::RasterPaintCmd_Fill]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_FillAll : public RasterPaintCmd_Fill
{
  typedef RasterPaintCmd_Fill Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd)
  {
    Base::init(engine, cmd, FILL_RULE_NON_ZERO);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
  }
};

// ============================================================================
// [Fog::RasterPaintCmd_FillNormalizedBoxI]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_FillNormalizedBoxI : public RasterPaintCmd_Fill
{
  typedef RasterPaintCmd_Fill Base;
  
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const BoxI& box)
  {
    Base::init(engine, cmd, FILL_RULE_NON_ZERO);
    _box.init(box);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxI& getPath() const { return _box(); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<BoxI> _box;
};

// ============================================================================
// [Fog::RasterPaintCmd_FillNormalizedBoxF]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_FillNormalizedBoxF : public RasterPaintCmd_Fill
{
  typedef RasterPaintCmd_Fill Base;
  
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const BoxF& box)
  {
    Base::init(engine, cmd, FILL_RULE_NON_ZERO);
    _box.init(box);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxF& getPath() const { return _box(); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<BoxF> _box;
};

// ============================================================================
// [Fog::RasterPaintCmd_FillNormalizedBoxD]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_FillNormalizedBoxD : public RasterPaintCmd_Fill
{
  typedef RasterPaintCmd_Fill Base;
  
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const BoxD& box)
  {
    Base::init(engine, cmd, FILL_RULE_NON_ZERO);
    _box.init(box);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxD& getPath() const { return _box(); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<BoxD> _box;
};

// ============================================================================
// [Fog::RasterPaintCmd_FillNormalizedPathF]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_FillNormalizedPathF : public RasterPaintCmd_Fill
{
  typedef RasterPaintCmd_Fill Base;
  
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const PathF& path, const PointF& pt, uint32_t fillRule)
  {
    Base::init(engine, cmd, fillRule);
    _path.init(path);
    _pt.init(pt);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
    _path.destroy();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PathF& getPath() const { return _path(); }
  FOG_INLINE const PointF& getPoint() const { return _pt(); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathF> _path;
  Static<PointF> _pt;
};

// ============================================================================
// [Fog::RasterPaintCmd_FillNormalizedPathD]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_FillNormalizedPathD : public RasterPaintCmd_Fill
{
  typedef RasterPaintCmd_Fill Base;
  
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const PathD& path, const PointD& pt, uint32_t fillRule)
  {
    Base::init(engine, cmd, fillRule);
    _path.init(path);
    _pt.init(pt);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
    _path.destroy();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PathD& getPath() const { return _path(); }
  FOG_INLINE const PointD& getPoint() const { return _pt(); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathD> _path;
  Static<PointD> _pt;
};

// ============================================================================
// [Fog::RasterPaintCmd_SetClipBox]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_SetClipBox : public RasterPaintCmd
{
  typedef RasterPaintCmd Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const BoxI& clipBox)
  {
    Base::init(engine, cmd);
    _clipBox.init(clipBox);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxI& getClipBox() const { return _clipBox(); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<BoxI> _clipBox;
};

// ============================================================================
// [Fog::RasterPaintCmd_SetClipRegion]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_SetClipRegion : public RasterPaintCmd
{
  typedef RasterPaintCmd Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const Region& clipRegion)
  {
    Base::init(engine, cmd);
    _clipRegion.init(clipRegion);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
    _clipRegion.destroy();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const Region& getClipRegion() const { return _clipRegion(); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<Region> _clipRegion;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTCMD_P_H
