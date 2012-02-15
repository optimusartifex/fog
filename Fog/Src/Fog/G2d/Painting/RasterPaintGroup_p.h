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
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t getCommand() const { return _cmd; }
  FOG_INLINE void setCommand(uint8_t cmd) { _cmd = cmd; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Command bytecode.
  uint32_t _cmd : 8;
  //! @brief Command embedded data (24-bits).
  uint32_t _data24 : 24;
};

// ============================================================================
// [Fog::RasterPaintCmd_FillSourcePrgb32]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_FillSourcePrgb32 : public RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getPrgb32() const { return _prgb32; }
  FOG_INLINE uint32_t getOpacity() const { return _data24; }

  FOG_INLINE void setPrgb32(uint32_t prgb32) { _prgb32 = prgb32; }
  FOG_INLINE void setOpacity(uint32_t opacity) { _data24 = opacity; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _prgb32;
};

// ============================================================================
// [Fog::RasterPaintCmd_FillShapeF]
// ============================================================================

template<typename ShapeT>
struct FOG_NO_EXPORT RasterPaintCmd_FillShape : public RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<ShapeT> shape;
};

// ============================================================================
// [Fog::RasterPaintCmd_FillPathF]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_FillPathF : public RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathF> path;
};

// ============================================================================
// [Fog::RasterPaintCmd_FillPathD]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_FillPathD : public RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathD> path;
};

// ============================================================================
// [Fog::RasterPaintCmd_StrokePathF]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_StrokePathF : public RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathF> path;
  Static<PathStrokerParamsF> params;
};

// ============================================================================
// [Fog::RasterPaintCmd_StrokePathD]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintCmd_StrokePathD : public RasterPaintCmd
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathD> path;
  Static<PathStrokerParamsD> params;
};

/*
struct FOG_NO_EXPORT RasterPaintGroup
{
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
  float opacity;
#else
  //! @brief The group opacity;
  float opacity;
  //! @brief The group bounding box (it can only grow).
  BoxI boundingBox;
#endif
};
*/

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTGROUP_P_H
