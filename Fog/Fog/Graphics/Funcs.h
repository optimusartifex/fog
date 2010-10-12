// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package


// [Guard]
#ifndef _FOG_GRAPHICS_FUNCS_H
#define _FOG_GRAPHICS_FUNCS_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Constants.h>
#include <Fog/Graphics/Constants.h>

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ApproximateCurve3DataD;
struct ApproximateCurve3DataF;
struct ApproximateCurve4DataD;
struct ApproximateCurve4DataF;

struct PathD;
struct PathF;

struct PointD;
struct PointF;

struct TransformD;
struct TransformF;

//! @addtogroup Fog_Graphics_Geometry
//! @{

// ============================================================================
// [Fog::G2dFuncs]
// ============================================================================

//! @internal
//!
//! @brief G2d functions (implementation is based on processor features).
struct G2dFuncs
{
  // --------------------------------------------------------------------------
  // [QuadCurve(3) / CubicCurve(4)]
  // --------------------------------------------------------------------------
  /*
  typedef err_t (FOG_FASTCALL *QuadCurveD_ApproximateFn)(
    const PointD* self,
    PathD& dst,
    uint8_t initialCommand,
    double approximationScale,
    double angleTolerance);
  */
  typedef err_t (FOG_FASTCALL *QuadCurveD_ApproximateFn)(
    const PointD* self,
    PathD& dst,
    uint8_t initialCommand,
    double approximationScale);
/*
  typedef err_t (FOG_FASTCALL *CubicCurveD_ApproximateFn)(
    const PointD* self,
    PathD& dst,
    uint8_t initialCommand,
    double approximationScale,
    double angleTolerance,
    double cuspLimit);
*/
  typedef err_t (FOG_FASTCALL *CubicCurveD_ApproximateFn)(
    const PointD* self,
    PathD& dst,
    uint8_t initialCommand,
    double approximationScale);

  typedef uint (FOG_FASTCALL *CubicCurveD_GetInflectionPointsFn)(
    const PointD* self,
    double* dst);

  struct _FuncsQuadCurveF
  {
  };

  struct _FuncsQuadCurveD
  {
    QuadCurveD_ApproximateFn approximate;
  };

  struct _FuncsCubicCurveF
  {
  };

  struct _FuncsCubicCurveD
  {
    CubicCurveD_ApproximateFn approximate;
    CubicCurveD_GetInflectionPointsFn getInflectionPoints;
  };

  _FuncsQuadCurveF quadcurvef;
  _FuncsQuadCurveD quadcurved;

  _FuncsCubicCurveF cubiccurvef;
  _FuncsCubicCurveD cubiccurved;

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_FASTCALL *TransformF_CreateFn)(TransformF& self, uint32_t createType, const void* params);
  typedef err_t (FOG_FASTCALL *TransformD_CreateFn)(TransformD& self, uint32_t createType, const void* params);

  typedef uint32_t (FOG_FASTCALL *TransformF_UpdateFn)(const TransformF& self);
  typedef uint32_t (FOG_FASTCALL *TransformD_UpdateFn)(const TransformD& self);

  typedef err_t (FOG_FASTCALL *TransformF_TransformFn)(TransformF& self, uint32_t transformOp, const void* params);
  typedef err_t (FOG_FASTCALL *TransformD_TransformFn)(TransformD& self, uint32_t transformOp, const void* params);

  typedef TransformF (FOG_FASTCALL *TransformF_TransformedFn)(const TransformF& self, uint32_t transformOp, const void* params);
  typedef TransformD (FOG_FASTCALL *TransformD_TransformedFn)(const TransformD& self, uint32_t transformOp, const void* params);

  typedef void (FOG_FASTCALL *TransformF_MultiplyFn)(TransformF& self, const TransformF& a, const TransformF& b);
  typedef void (FOG_FASTCALL *TransformD_MultiplyFn)(TransformD& self, const TransformD& a, const TransformD& b);

  typedef bool (FOG_FASTCALL *TransformF_InvertFn)(TransformF& self, const TransformF& a);
  typedef bool (FOG_FASTCALL *TransformD_InvertFn)(TransformD& self, const TransformD& a);

  typedef void (FOG_FASTCALL *TransformF_MapPointFFn)(const TransformF& self, PointF& dst, const PointF& src);
  typedef void (FOG_FASTCALL *TransformD_MapPointDFn)(const TransformD& self, PointD& dst, const PointD& src);

  typedef void (FOG_FASTCALL *TransformF_MapPointsFFn)(const TransformF& self, PointF* dst, const PointF* src, sysuint_t length);
  typedef void (FOG_FASTCALL *TransformD_MapPointsDFn)(const TransformD& self, PointD* dst, const PointD* src, sysuint_t length);

  typedef void (FOG_FASTCALL *TransformF_MapVectorFFn)(const TransformF& self, PointF& dst, const PointF& src);
  typedef void (FOG_FASTCALL *TransformD_MapVectorDFn)(const TransformD& self, PointD& dst, const PointD& src);

  typedef PointF (FOG_FASTCALL *TransformF_GetScalingFn)(const TransformF& self, bool absolute);
  typedef PointD (FOG_FASTCALL *TransformD_GetScalingFn)(const TransformD& self, bool absolute);

  typedef float (FOG_FASTCALL *TransformF_GetRotationFn)(const TransformF& self);
  typedef double (FOG_FASTCALL *TransformD_GetRotationFn)(const TransformD& self);

  typedef float (FOG_FASTCALL *TransformF_GetAverageScalingFn)(const TransformF& self);
  typedef double (FOG_FASTCALL *TransformD_GetAverageScalingFn)(const TransformD& self);

  struct _FuncsTransformF
  {
    TransformF_CreateFn create;
    TransformF_UpdateFn update;
    TransformF_TransformFn transform;
    TransformF_TransformedFn transformed;
    TransformF_MultiplyFn multiply;
    TransformF_InvertFn invert;
    TransformF_MapPointFFn mapPointF;
    TransformF_MapPointsFFn mapPointsF[TRANSFORM_TYPE_COUNT];
    TransformF_MapVectorFFn mapVectorF;
    TransformF_GetScalingFn getScaling;
    TransformF_GetRotationFn getRotation;
    TransformF_GetAverageScalingFn getAverageScaling;
  };

  struct _FuncsTransformD
  {
    TransformD_CreateFn create;
    TransformD_UpdateFn update;
    TransformD_TransformFn transform;
    TransformD_TransformedFn transformed;
    TransformD_MultiplyFn multiply;
    TransformD_InvertFn invert;
    TransformD_MapPointDFn mapPointD;
    TransformD_MapPointsDFn mapPointsD[TRANSFORM_TYPE_COUNT];
    TransformD_MapVectorDFn mapVectorD;
    TransformD_GetScalingFn getScaling;
    TransformD_GetRotationFn getRotation;
    TransformD_GetAverageScalingFn getAverageScaling;
  };

  _FuncsTransformF transformf;
  _FuncsTransformD transformd;
};

extern FOG_API G2dFuncs _g2d;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_FUNCS_H
