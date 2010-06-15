#ifndef _FOGBENCH_BENCHQT4_H
#define _FOGBENCH_BENCHQT4_H

#include "BenchBase.h"
#include "BenchFog.h"

#if defined(FOG_BENCH_QT4)

#include <Qt/QtCore>
#include <Qt/QtGui>

// ============================================================================
// [FogBench - Qt4Module]
// ============================================================================

struct Qt4Module : public FogModule
{
  Qt4Module(int w, int h);
  virtual ~Qt4Module();

  virtual Fog::String getEngine();

  virtual void configurePainter(QPainter& painter);

  QImage screen_qt4;
};

// ============================================================================
// [FogBench - Qt4Module_CreateDestroy]
// ============================================================================

struct Qt4Module_CreateDestroy : public Qt4Module
{
  Qt4Module_CreateDestroy(int w, int h);
  virtual ~Qt4Module_CreateDestroy();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - Qt4Module_FillRect]
// ============================================================================

struct Qt4Module_FillRect : public Qt4Module
{
  Qt4Module_FillRect(int w, int h);
  virtual ~Qt4Module_FillRect();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Rect r_rect;
  Randomizer_Argb r_argb;
};

// ============================================================================
// [FogBench - Qt4Module_FillRectSubPX]
// ============================================================================

struct Qt4Module_FillRectSubPX : public Qt4Module_FillRect
{
  Qt4Module_FillRectSubPX(int w, int h);
  virtual ~Qt4Module_FillRectSubPX();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - Qt4Module_FillRectAffine]
// ============================================================================

struct Qt4Module_FillRectAffine : public Qt4Module_FillRect
{
  Qt4Module_FillRectAffine(int w, int h);
  virtual ~Qt4Module_FillRectAffine();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - Qt4Module_FillRound]
// ============================================================================

struct Qt4Module_FillRound : public Qt4Module_FillRect
{
  Qt4Module_FillRound(int w, int h);
  virtual ~Qt4Module_FillRound();

  virtual void bench(int quantity);
  Fog::String getType();
};

// ============================================================================
// [FogBench - Qt4Module_FillPolygon]
// ============================================================================

struct Qt4Module_FillPolygon : public Qt4Module
{
  Qt4Module_FillPolygon(int w, int h);
  virtual ~Qt4Module_FillPolygon();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Polygon r_poly;
  Randomizer_Argb r_argb;
};

// ============================================================================
// [FogBench - Qt4Module_Image]
// ============================================================================

struct Qt4Module_Image : public Qt4Module
{
  Qt4Module_Image(int w, int h);
  virtual ~Qt4Module_Image();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual Fog::String getType();

  Randomizer_Rect r_rect;
  Randomizer_Number r_numb;

  Fog::Image images[NUM_SPRITES];
  QImage images_qt4[NUM_SPRITES];
};

// ============================================================================
// [FogBench - Qt4Module_ImageAffine]
// ============================================================================

struct Qt4Module_ImageAffine : public Qt4Module_Image
{
  Qt4Module_ImageAffine(int w, int h);
  virtual ~Qt4Module_ImageAffine();

  virtual void bench(int quantity);
  virtual Fog::String getType();
};

// ============================================================================
// [FogBench - Qt4BenchmarkContext]
// ============================================================================

struct Qt4BenchmarkContext : public AbstractBenchmarkContext
{
  Qt4BenchmarkContext(BenchmarkMaster* master);
  virtual ~Qt4BenchmarkContext();

  virtual void run();
};

#endif // FOG_BENCH_QT4

#endif // _FOGBENCH_BENCHQT4_H
