// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERPAINTGROUP_P_H
#define _FOG_G2D_PAINTING_RASTERPAINTGROUP_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Memory/MemBuffer.h>
#include <Fog/Core/Memory/MemZoneAllocator.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageFilter.h>
#include <Fog/G2d/Imaging/ImageFilterScale.h>
#include <Fog/G2d/Painting/PaintParams.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterStructs_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Source/Pattern.h>
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

  FOG_INLINE void init(uint8_t cmd) { _setCommand(cmd); }
  FOG_INLINE void destroy() {}

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

  FOG_INLINE void init(uint8_t cmd, uint8_t* ptr)
  {
    Base::init(cmd);
    _setPtr(ptr);
  }

  FOG_INLINE void destroy()
  { 
    Base::destroy();
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
// [Fog::RasterPaintCmd_FillSource]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_Source : public RasterPaintCmd
{
  typedef RasterPaintCmd Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(uint8_t cmd)
  {
    Base::init(cmd);
  }

  FOG_INLINE void destroy()
  {
    Base::destroy();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getOpacity() const { return _data24; }
  FOG_INLINE void _setOpacity(uint32_t opacity) { _data24 = opacity; }
};

// ============================================================================
// [Fog::RasterPaintCmd_FillSourcePrgb32]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_SourcePrgb32 : public RasterPaintCmd_Source
{
  typedef RasterPaintCmd_Source Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(uint8_t cmd, uint32_t prgb32)
  {
    Base::init(cmd);
    _setPrgb32(prgb32);
  }

  FOG_INLINE void destroy()
  {
    Base::destroy();
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
// [Fog::RasterPaintCmd_Fill]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_Fill : public RasterPaintCmd
{
  typedef RasterPaintCmd Base;

  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  FOG_INLINE void init(uint8_t cmd, uint32_t fillRule)
  {
    Base::init(cmd);
    _setFillRule(fillRule); 
  }

  FOG_INLINE void destroy()
  {
    Base::destroy();
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

  FOG_INLINE void init(uint8_t cmd)
  {
    Base::init(cmd, FILL_RULE_NON_ZERO);
  }

  FOG_INLINE void destroy()
  {
    Base::destroy();
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

  FOG_INLINE void init(uint8_t cmd, const ShapeT& shape, uint32_t fillRule)
  {
    Base::init(cmd, fillRule);
    _shape.init(shape);
  }

  FOG_INLINE void destroy()
  {
    Base::destroy();
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

  FOG_INLINE void init(uint8_t cmd, const PathF& path, uint32_t fillRule)
  {
    Base::init(cmd, fillRule);
    _path.init(path);
  }

  FOG_INLINE void destroy()
  {
    Base::destroy();
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

  FOG_INLINE void init(uint8_t cmd, const PathD& path, uint32_t fillRule)
  {
    Base::init(cmd, fillRule);
    _path.init(path);
  }

  FOG_INLINE void destroy()
  {
    Base::destroy();
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

  FOG_INLINE void init(uint8_t cmd, const ShapeT& shape, uint32_t fillRule)
  {
    Base::init(cmd, fillRule);
    _shape.init(shape);
  }

  FOG_INLINE void destroy()
  {
    Base::destroy();
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

  FOG_INLINE void init(uint8_t cmd, const PathStrokerParamsF& params)
  {
    Base::init(cmd);
    _params.init(params);
  }

  FOG_INLINE void destroy()
  {
    Base::destroy();
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

  FOG_INLINE void init(uint8_t cmd, const PathStrokerParamsD& params)
  {
    Base::init(cmd);
    _params.init(params);
  }

  FOG_INLINE void destroy()
  {
    Base::destroy();
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

  FOG_INLINE void init(uint8_t cmd, const PathF& path, const PathStrokerParamsF& params)
  {
    Base::init(cmd, params);
    _path.init(path);
  }

  FOG_INLINE void destroy()
  {
    Base::destroy();
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

  FOG_INLINE void init(uint8_t cmd, const PathD& path, const PathStrokerParamsD& params)
  {
    Base::init(cmd, params);
    _path.init(path);
  }

  FOG_INLINE void destroy()
  {
    Base::destroy();
    _path.destroy();
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathD> _path;
};

// ============================================================================
// [Fog::RasterPaintGroup]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintGroup
{
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    top = NULL;
    flags = NO_FLAGS;

    numGroups = 0;
    opacityF = 1.0f;

    boundingBox.reset();

    groupRecord = NULL;
    cmdRecord = NULL;
    cmdStart = NULL;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Link to the top group.
  RasterPaintGroup* top;

  //! @brief Group flags (types of commands used in this group).
  uint32_t flags;
  //! @brief How many groups are inside of this group.
  uint32_t numGroups;

#if FOG_ARCH_BITS >= 64
  //! @brief The group bounding box (it can only grow).
  BoxI boundingBox;
  //! @brief The group opacity;
  float opacityF;
#else
  //! @brief The group opacity;
  float opacityF;
  //! @brief The group bounding box (it can only grow).
  BoxI boundingBox;
#endif

  //! @brief Group record (recorded position in groupAllocator).
  MemZoneRecord* groupRecord;
  //! @brief Commands record (recorded position in cmdAllocator).
  MemZoneRecord* cmdRecord;
  //! @brief Commands start pointer.
  uint8_t* cmdStart;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTGROUP_P_H
