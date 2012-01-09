// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PATHINFO_H
#define _FOG_G2D_GEOMETRY_PATHINFO_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Box.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::PathInfoFigureFlags]
// ============================================================================

union FOG_NO_EXPORT PathInfoFigureFlags
{
  struct
  {
    uint32_t isClosed : 1;
    uint32_t hasQBezier : 1;
    uint32_t hasCBezier : 1;
    uint32_t hasAcuteEdges : 1;
    uint32_t reserved : 28;
  };

  uint32_t packed;
};

// ============================================================================
// [Fog::PathInfoFigureF]
// ============================================================================

#include <Fog/Core/C++/PackQWord.h>
struct FOG_NO_EXPORT PathInfoFigureF
{
  BoxF boundingBox;
  BoxF vertexBox;

  uint start;
  uint end;

  float figureLength;
  PathInfoFigureFlags flags;
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::PathInfoFigureD]
// ============================================================================

#include <Fog/Core/C++/PackQWord.h>
struct FOG_NO_EXPORT PathInfoFigureD
{
  BoxD boundingBox;
  BoxD vertexBox;

  uint start;
  uint end;

  double figureLength;
  PathInfoFigureFlags flags;
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::PathInfoF]
// ============================================================================

//! @brief Path info (float).
struct FOG_NO_EXPORT PathInfoF
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE PathInfoF* addRef() const
  {
    _reference.inc();
    return const_cast<PathInfoF*>(this);
  }

  FOG_INLINE void release()
  {
    if (_reference.deref())
      MemMgr::free(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _reference.get(); }
  FOG_INLINE size_t getNumberOfFigures() const { return _numberOfFigures; }
  FOG_INLINE size_t getDistanceCapacity() const { return _distanceCapacity; }
  FOG_INLINE float getPathLength() const { return _pathLength; }

  FOG_INLINE const PathInfoFigureF* getFigureData() const { return _figureData; }
  FOG_INLINE const float* getDistanceData() const { return _distanceData; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE PathInfoF* generate(const PathF& path)
  {
    return fog_api.pathinfof_generate(&path);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> _reference;

  //! @brief Number of path figures.
  size_t _numberOfFigures;
  //! @brief Capacity of distanceData array.
  size_t _distanceCapacity;

  //! @brief Sum of length of all figures.
  float _pathLength;

  //! @brief Path figures.
  PathInfoFigureF* _figureData;
  //! @brief Distances between individual points.
  float* _distanceData;
};

// ============================================================================
// [Fog::PathInfoD]
// ============================================================================

//! @brief Path info (double).
struct FOG_NO_EXPORT PathInfoD
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE PathInfoD* addRef() const
  {
    _reference.inc();
    return const_cast<PathInfoD*>(this);
  }

  FOG_INLINE void release()
  {
    if (_reference.deref())
      MemMgr::free(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _reference.get(); }
  FOG_INLINE size_t getNumberOfFigures() const { return _numberOfFigures; }
  FOG_INLINE size_t getDistanceCapacity() const { return _distanceCapacity; }
  FOG_INLINE double getPathLength() const { return _pathLength; }

  FOG_INLINE const PathInfoFigureD* getFigureData() const { return _figureData; }
  FOG_INLINE const double* getDistanceData() const { return _distanceData; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE PathInfoD* generate(const PathD& path)
  {
    return fog_api.pathinfod_generate(&path);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> _reference;

  //! @brief Number of path figures.
  size_t _numberOfFigures;
  //! @brief Capacity of distanceData array.
  size_t _distanceCapacity;

  //! @brief Sum of length of all figures.
  double _pathLength;

  //! @brief Path figures.
  PathInfoFigureD* _figureData;
  //! @brief Distances between individual points.
  double* _distanceData;
};

// ============================================================================
// [Fog::PathInfoT<> / Fog::PathInfoFigureT<>]
// ============================================================================

_FOG_NUM_T(PathInfo)
_FOG_NUM_T(PathInfoFigure)
_FOG_NUM_F(PathInfo)
_FOG_NUM_F(PathInfoFigure)
_FOG_NUM_D(PathInfo)
_FOG_NUM_D(PathInfoFigure)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PATHINFO_H
