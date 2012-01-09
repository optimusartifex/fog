// [Fog-Bench]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_BENCHFOG_H
#define _FOG_BENCHFOG_H

// [Dependencies]
#include "BenchApp.h"

// ============================================================================
// [BenchFog]
// ============================================================================

struct BenchFog : public BenchModule
{
  BenchFog(BenchApp& app);
  virtual ~BenchFog();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual Fog::StringW getModuleName() const;
  virtual void bench(BenchOutput& output, const BenchParams& params);

  // --------------------------------------------------------------------------
  // [Fog-Specific]
  // --------------------------------------------------------------------------

  void configurePainter(Fog::Painter& p, const BenchParams& params);

  void configureGradient(Fog::LinearGradientF& gradient, const Fog::RectI& r,
    const Fog::Argb32& c0, const Fog::Argb32& c1, const Fog::Argb32& c2);
  void configureGradient(Fog::LinearGradientF& gradient, const Fog::RectF& r,
    const Fog::Argb32& c0, const Fog::Argb32& c1, const Fog::Argb32& c2);

  // --------------------------------------------------------------------------
  // [Run]
  // --------------------------------------------------------------------------

  virtual void runCreateDestroy(BenchOutput& output, const BenchParams& params);
  virtual void runFillRectI(BenchOutput& output, const BenchParams& params);
  virtual void runFillRectF(BenchOutput& output, const BenchParams& params);
  virtual void runFillRectRotate(BenchOutput& output, const BenchParams& params);
  virtual void runFillRound(BenchOutput& output, const BenchParams& params);
  virtual void runFillPolygon(BenchOutput& output, const BenchParams& params, uint32_t complexity);
  virtual void runBlitImageI(BenchOutput& output, const BenchParams& params);
  virtual void runBlitImageF(BenchOutput& output, const BenchParams& params);
  virtual void runBlitImageRotate(BenchOutput& output, const BenchParams& params);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t mt;
};

// [Guard]
#endif // _FOG_BENCHFOG_H
