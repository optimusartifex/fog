#ifndef _FOGBENCH_BENCHCAIRO_H
#define _FOGBENCH_BENCHCAIRO_H

#include "BenchBase.h"
#include "BenchFog.h"

#if defined(FOG_BENCH_CAIRO)

#include <cairo/cairo.h>

// ============================================================================
// [FogBench - CairoModule]
// ============================================================================

struct CairoModule : public FogModule
{
  CairoModule(int w, int h);
  virtual ~CairoModule();

  virtual Fog::String getEngine();
  virtual void configureContext(cairo_t* cr);

  cairo_surface_t* screen_cairo;
};

// ============================================================================
// [FogBench - CairoModule_CreateDestroy]
// ============================================================================

struct CairoModule_CreateDestroy : public CairoModule
{
  CairoModule_CreateDestroy(int w, int h);
  virtual ~CairoModule_CreateDestroy();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - CairoModule_FillRect]
// ============================================================================

struct CairoModule_FillRect : public CairoModule
{
  CairoModule_FillRect(int w, int h);
  virtual ~CairoModule_FillRect();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Rect r_rect;
  Randomizer_Argb r_argb;
};

// ============================================================================
// [FogBench - CairoModule_FillRectAffine]
// ============================================================================

struct CairoModule_FillRectAffine : public CairoModule_FillRect
{
  CairoModule_FillRectAffine(int w, int h);
  virtual ~CairoModule_FillRectAffine();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - CairoModule_FillRound]
// ============================================================================

struct CairoModule_FillRound : public CairoModule_FillRect
{
  CairoModule_FillRound(int w, int h);
  virtual ~CairoModule_FillRound();

  virtual void bench(int quantity);
  Fog::String getType();

  void addRound(cairo_t* cr, Fog::IntRect rect, double radius);
};

// ============================================================================
// [FogBench - CairoModule_FillPolygon]
// ============================================================================

struct CairoModule_FillPolygon : public CairoModule
{
  CairoModule_FillPolygon(int w, int h);
  virtual ~CairoModule_FillPolygon();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Polygon r_poly;
  Randomizer_Argb r_argb;
};

// ============================================================================
// [FogBench - CairoModule_Image]
// ============================================================================

struct CairoModule_Image : public CairoModule
{
  CairoModule_Image(int w, int h);
  virtual ~CairoModule_Image();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Rect r_rect;
  Randomizer_Number r_numb;

  Fog::Image images[NUM_SPRITES];
  cairo_surface_t* images_cairo[NUM_SPRITES];
};

// ============================================================================
// [FogBench - CairoModule_ImageAffine]
// ============================================================================

struct CairoModule_ImageAffine : public CairoModule_Image
{
  CairoModule_ImageAffine(int w, int h);
  virtual ~CairoModule_ImageAffine();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - CairoBenchmarkContext]
// ============================================================================

struct CairoBenchmarkContext : public AbstractBenchmarkContext
{
  CairoBenchmarkContext(BenchmarkMaster* master);
  virtual ~CairoBenchmarkContext();

  virtual void run();
};

#endif // FOG_BENCH_CAIRO

#endif // _FOGBENCH_BENCHCAIRO_H
