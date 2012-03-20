// [Fog-Bench]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_BENCHGDIPLUS_H
#define _FOG_BENCHGDIPLUS_H

#include "BenchApp.h"

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
  virtual Fog::List<uint32_t> getSupportedPixelFormats() const;

  virtual void bench(BenchOutput& output, const BenchParams& params);

  virtual void prepareSprites(int size);
  virtual void freeSprites();

  // --------------------------------------------------------------------------
  // [GdiPlus-Specific]
  // --------------------------------------------------------------------------

  void configureGraphics(Gdiplus::Graphics& gr, const BenchParams& params);
  void addRound(Gdiplus::GraphicsPath& p, const Fog::RectF& rect, float radius);

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

  ULONG_PTR gdiplusToken;

  Gdiplus::Bitmap* screenGdi;
  Gdiplus::REAL gradientStopCache[3];

  Fog::List<Gdiplus::Bitmap*> spritesGdi;
};

// [Guard]
#endif // _FOG_BENCHGDIPLUS_H
