// [Fog-Bench]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_BENCHGDIPLUS_H
#define _FOG_BENCHGDIPLUS_H

#include "BenchApp.h"
#if defined(FOG_BENCH_GDIPLUS)

// [Dependencies - GdiPlus]
#include <windows.h>

// Fix...
#if !defined(min)
#define min(a, b) ((a < b) ? (a) : (b))
#define max(a, b) ((a > b) ? (a) : (b))
#endif
#include <gdiplus.h>
#undef min
#undef max

// ============================================================================
// [BenchGdiPlus]
// ============================================================================

struct BenchGdiPlus : public BenchModule
{
  BenchGdiPlus(BenchApp& app);
  virtual ~BenchGdiPlus();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual Fog::StringW getModuleName() const;
  virtual void bench(BenchOutput& output, const BenchParams& params);

  // --------------------------------------------------------------------------
  // [GdiPlus-Specific]
  // --------------------------------------------------------------------------

  void configureGraphics(Gdiplus::Graphics& gr, const BenchParams& params);
  void addRound(Gdiplus::GraphicsPath& p, const Fog::RectF& rect, float radius);

  void runCreateDestroy(BenchOutput& output, const BenchParams& params);
  void runFillRectI(BenchOutput& output, const BenchParams& params);
  void runFillRectF(BenchOutput& output, const BenchParams& params);
  void runFillRectRotate(BenchOutput& output, const BenchParams& params);
  void runFillRound(BenchOutput& output, const BenchParams& params);
  void runFillPolygon(BenchOutput& output, const BenchParams& params, uint32_t complexity);
  void runBlitImageI(BenchOutput& output, const BenchParams& params);
  void runBlitImageRotate(BenchOutput& output, const BenchParams& params);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  ULONG_PTR gdiplusToken;

  Gdiplus::Bitmap* screenGdi;
  Gdiplus::REAL gradientStopCache[3];

  Fog::List<Gdiplus::Bitmap*> spritesGdi;
};

// [Guard]
#endif // FOG_BENCH_GDIPLUS
#endif // _FOG_BENCHGDIPLUS_H
