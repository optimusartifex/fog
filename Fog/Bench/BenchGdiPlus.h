#ifndef _FOGBENCH_BENCHGDIPLUS_H
#define _FOGBENCH_BENCHGDIPLUS_H

#include "BenchBase.h"
#include "BenchFog.h"

#if defined(FOG_BENCH_GDIPLUS)

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
// [FogBench - GdiPlusModule]
// ============================================================================


struct GdiPlusModule : public AbstractModule
{
  GdiPlusModule(int w, int h);
  virtual ~GdiPlusModule();

  Gdiplus::Bitmap* screen_gdip;
  Gdiplus::Bitmap* sprite_gdip[4];

  virtual Fog::String getEngine();
  virtual void configureGraphics(Gdiplus::Graphics& gr);
};

// ============================================================================
// [FogBench - GdiPlusModule_CreateDestroy]
// ============================================================================

struct GdiPlusModule_CreateDestroy : public GdiPlusModule
{
  GdiPlusModule_CreateDestroy(int w, int h);
  virtual ~GdiPlusModule_CreateDestroy();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - GdiPlusModule_FillRect]
// ============================================================================

struct GdiPlusModule_FillRect : public GdiPlusModule
{
  GdiPlusModule_FillRect(int w, int h);
  virtual ~GdiPlusModule_FillRect();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Rect r_rect;
  Randomizer_Argb r_argb;
};

// ============================================================================
// [FogBench - GdiPlusModule_FillRectAffine]
// ============================================================================

struct GdiPlusModule_FillRectAffine : public GdiPlusModule_FillRect
{
  GdiPlusModule_FillRectAffine(int w, int h);
  virtual ~GdiPlusModule_FillRectAffine();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - GdiPlusModule_FillRound]
// ============================================================================

struct GdiPlusModule_FillRound : public GdiPlusModule_FillRect
{
  GdiPlusModule_FillRound(int w, int h);
  virtual ~GdiPlusModule_FillRound();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Rect randomizer;
};

// ============================================================================
// [FogBench - GdiPlusModule_FillPolygon]
// ============================================================================

struct GdiPlusModule_FillPolygon : public GdiPlusModule
{
  GdiPlusModule_FillPolygon(int w, int h);
  virtual ~GdiPlusModule_FillPolygon();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Polygon r_poly;
  Randomizer_Argb r_argb;
};

// ============================================================================
// [FogBench - GdiPlusModule_Image]
// ============================================================================

struct GdiPlusModule_Image : public GdiPlusModule
{
  GdiPlusModule_Image(int w, int h);
  virtual ~GdiPlusModule_Image();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Rect r_rect;
  Randomizer_Number r_numb;

  Fog::Image images[NUM_SPRITES];
  Gdiplus::Bitmap* images_gdip[NUM_SPRITES];
};

// ============================================================================
// [FogBench - GdiPlusModule_ImageAffine]
// ============================================================================

struct GdiPlusModule_ImageAffine : public GdiPlusModule_Image
{
  GdiPlusModule_ImageAffine(int w, int h);
  virtual ~GdiPlusModule_ImageAffine();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - GdiPlusBenchmarkContext]
// ============================================================================

struct GdiPlusBenchmarkContext : public AbstractBenchmarkContext
{
  GdiPlusBenchmarkContext(BenchmarkMaster* master);
  virtual ~GdiPlusBenchmarkContext();

  virtual void run();
};

#endif // FOG_BENCH_GDIPLUS

#endif // _FOGBENCH_BENCHGDIPLUS_H
