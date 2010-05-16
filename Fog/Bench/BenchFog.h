#ifndef _FOGBENCH_BENCHFOG_H
#define _FOGBENCH_BENCHFOG_H

#include "BenchBase.h"

// ============================================================================
// [FogBench - FogModule]
// ============================================================================

struct FogModule : public AbstractModule
{
  FogModule(int w, int h);
  virtual ~FogModule();

  virtual Fog::String getEngine();
  void setEngine(int engine);

  virtual void configurePainter(Fog::Painter& p);

  int engine;
};

// ============================================================================
// [FogBench - FogModule_CreateDestroy]
// ============================================================================

struct FogModule_CreateDestroy : public FogModule
{
  FogModule_CreateDestroy(int w, int h);
  virtual ~FogModule_CreateDestroy();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - FogModule_FillRect]
// ============================================================================

struct FogModule_FillRect : public FogModule
{
  FogModule_FillRect(int w, int h);
  virtual ~FogModule_FillRect();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Rect r_rect;
  Randomizer_Argb r_argb;
};

// ============================================================================
// [FogBench - FogModule_FillRectAffine]
// ============================================================================

struct FogModule_FillRectAffine : public FogModule_FillRect
{
  FogModule_FillRectAffine(int w, int h);
  virtual ~FogModule_FillRectAffine();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - FogModule_FillRound]
// ============================================================================

struct FogModule_FillRound : public FogModule_FillRect
{
  FogModule_FillRound(int w, int h);
  virtual ~FogModule_FillRound();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Rect randomizer;
};

// ============================================================================
// [FogBench - FogModule_FillPolygon]
// ============================================================================

struct FogModule_FillPolygon : public FogModule
{
  FogModule_FillPolygon(int w, int h);
  virtual ~FogModule_FillPolygon();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Polygon r_poly;
  Randomizer_Argb r_argb;
};

// ============================================================================
// [FogBench - FogModule_Image]
// ============================================================================

struct FogModule_Image : public FogModule
{
  FogModule_Image(int w, int h);
  virtual ~FogModule_Image();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Rect r_rect;
  Randomizer_Number r_numb;

  Fog::Image images[NUM_SPRITES];
};

// ============================================================================
// [FogBench - FogModule_ImageAffine]
// ============================================================================

struct FogModule_ImageAffine : public FogModule_Image
{
  FogModule_ImageAffine(int w, int h);
  virtual ~FogModule_ImageAffine();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - FogModule_RasterText]
// ============================================================================

struct FogModule_RasterText : public FogModule
{
  FogModule_RasterText(int w, int h);
  virtual ~FogModule_RasterText();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Rect r_rect;
  Randomizer_Argb r_argb;
};

// ============================================================================
// [FogBench - FogBenchmarkContext]
// ============================================================================

struct FogBenchmarkContext : public AbstractBenchmarkContext
{
  FogBenchmarkContext(BenchmarkMaster* master, const Fog::String& name);
  virtual ~FogBenchmarkContext();

  virtual void run();
};

#endif // _FOGBENCH_BENCHFOG_H
