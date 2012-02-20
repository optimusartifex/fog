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
// [Fog::RasterPaintCmd_FillShape<ShapeT>]
// ============================================================================

template<typename ShapeT>
struct FOG_NO_EXPORT RasterPaintCmd_FillShape : public RasterPaintCmd_Fill
{
  typedef RasterPaintCmd_Fill Base;
  
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const ShapeT& shape, uint32_t fillRule)
  {
    Base::init(engine, cmd, fillRule);
    _shape.init(shape);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
    _shape.destroy();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const ShapeT& getShape() const { return _shape(); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<ShapeT> _shape;
};

// ============================================================================
// [Fog::RasterPaintCmd_FillPathF]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_FillPathF : public RasterPaintCmd_Fill
{
  typedef RasterPaintCmd_Fill Base;
  
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const PathF& path, uint32_t fillRule)
  {
    Base::init(engine, cmd, fillRule);
    _path.init(path);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
    _path.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathF> _path;
};

// ============================================================================
// [Fog::RasterPaintCmd_FillPathD]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_FillPathD : public RasterPaintCmd_Fill
{
  typedef RasterPaintCmd_Fill Base;
  
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const PathD& path, uint32_t fillRule)
  {
    Base::init(engine, cmd, fillRule);
    _path.init(path);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
    _path.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathD> _path;
};

// ============================================================================
// [Fog::RasterPaintCmd_FillNormalizedShape<ShapeT>]
// ============================================================================

template<typename ShapeT>
struct RasterPaintCmd_FillNormalizedShape : public RasterPaintCmd_Fill
{
  typedef RasterPaintCmd_Fill Base;
  
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const ShapeT& shape, uint32_t fillRule)
  {
    Base::init(engine, cmd, fillRule);
    _shape.init(shape);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
    _shape.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<ShapeT> _shape;
};

// ============================================================================
// [Fog::RasterPaintCmd_StrokeF]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_StrokeF : public RasterPaintCmd
{
  typedef RasterPaintCmd Base;
  
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const PathStrokerParamsF& params)
  {
    Base::init(engine, cmd);
    _params.init(params);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
    _params.destroy();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE PathStrokerParamsF& getParams() { return _params(); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathStrokerParamsF> _params;
};

// ============================================================================
// [Fog::RasterPaintCmd_StrokeD]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_StrokeD : public RasterPaintCmd
{
  typedef RasterPaintCmd Base;
  
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const PathStrokerParamsD& params)
  {
    Base::init(engine, cmd);
    _params.init(params);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
    _params.destroy();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE PathStrokerParamsD& getParams() { return _params(); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathStrokerParamsD> _params;
};

// ============================================================================
// [Fog::RasterPaintCmd_StrokePathF]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_StrokePathF : public RasterPaintCmd_StrokeF
{
  typedef RasterPaintCmd_StrokeF Base;
  
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const PathF& path, const PathStrokerParamsF& params)
  {
    Base::init(engine, cmd, params);
    _path.init(path);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
    _path.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathF> _path;
};

// ============================================================================
// [Fog::RasterPaintCmd_StrokePathD]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_StrokePathD : public RasterPaintCmd_StrokeD
{
  typedef RasterPaintCmd_StrokeD Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(RasterPaintEngine* engine, uint8_t cmd, const PathD& path, const PathStrokerParamsD& params)
  {
    Base::init(engine, cmd, params);
    _path.init(path);
  }

  FOG_INLINE void destroy(RasterPaintEngine* engine)
  {
    Base::destroy(engine);
    _path.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathD> _path;
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
