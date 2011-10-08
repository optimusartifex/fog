// [Fog-Bench]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_BENCHAPP_H
#define _FOG_BENCHAPP_H

// [Dependencies]
#include <Fog/Core.h>
#include <Fog/G2d.h>

#include <stdio.h>
#include <stdlib.h>

#include "BenchConfig.h"

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct BenchModule;
struct BenchOutput;
struct BenchParams;

// ============================================================================
// [BENCH_OPERATOR]
// ============================================================================

// Compositing operators were limited to support all of the benchmarking 
// libraries. Gdi+ library contains only these two, so we do not compare
// the performance of other operators.

enum BENCH_OPERATOR
{
  BENCH_OPERATOR_SRC = 0,
  BENCH_OPERATOR_SRC_OVER = 1,

  BENCH_OPERATOR_COUNT = 2,
  BENCH_OPERATOR_NONE = 0xFFFFFFFFU
};

// ============================================================================
// [BENCH_SOURCE]
// ============================================================================

enum BENCH_SOURCE
{
  BENCH_SOURCE_SOLID = 0,
  BENCH_SOURCE_LINEAR = 1,

  BENCH_SOURCE_COUNT = 2,
  BENCH_SOURCE_NONE = 0xFFFFFFFFU
};

// ============================================================================
// [BENCH_TYPE]
// ============================================================================

enum BENCH_TYPE
{
  BENCH_TYPE_CREATE_DESTROY = 0,
  BENCH_TYPE_FILL_RECT_I = 1,
  BENCH_TYPE_FILL_RECT_F = 2,
  BENCH_TYPE_FILL_RECT_ROTATE = 3,
  BENCH_TYPE_FILL_ROUND = 4,
  BENCH_TYPE_FILL_POLYGON = 5,
  BENCH_TYPE_BLIT_IMAGE_I = 6,
  BENCH_TYPE_BLIT_IMAGE_ROTATE = 7,
  BENCH_TYPE_COUNT = 8
};

// ============================================================================
// [BenchApp]
// ============================================================================

struct BenchApp
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  BenchApp(const Fog::SizeI& screenSize, uint32_t quantity);
  ~BenchApp();

  // --------------------------------------------------------------------------
  // [Test]
  // --------------------------------------------------------------------------

  bool hasBenchSource(uint32_t benchType);

  void runAll();
  void runModule(BenchModule* module);
  void registerResults(BenchModule* module, const BenchParams& params, const BenchOutput& output);

  // --------------------------------------------------------------------------
  // [Modules]
  // --------------------------------------------------------------------------

  void addModule(BenchModule* module);
  void deleteModules();

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  void loadData();
  void makeRand();

  // --------------------------------------------------------------------------
  // [Logging]
  // --------------------------------------------------------------------------

  Fog::StringW getBenchString(uint32_t benchType) const;
  Fog::StringW getSourceString(uint32_t sourceType) const;
  Fog::StringW getOperatorString(uint32_t op) const;

  Fog::StringW getTestString(const BenchParams& params) const;

  void logInfo();

  void logBenchHeader(BenchModule* module, const BenchParams& params);
  void logBenchFooter(BenchModule* module, const BenchParams& params);
  void logBenchOutput(BenchModule* module, const BenchParams& params, const BenchOutput& output);

  void logs(const Fog::StringW& str);
  void logf(const char* fmt, ...);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // Configuration
  Fog::SizeI screenSize;
  uint32_t quantity;
  Fog::List<uint32_t> sizeList;
  bool saveImages;

  // Modules.
  Fog::List<BenchModule*> modules;

  // Random numbers (we use array so lookup is easy and predictable).
  size_t randomSize;
  uint32_t* randomData;

  // Sprites.
  Fog::List<Fog::Image> sprites;
};

// ============================================================================
// [BenchOutput]
// ============================================================================

struct BenchOutput
{
  Fog::TimeDelta time;
};

// ============================================================================
// [BenchParams]
// ============================================================================

struct BenchParams
{
  Fog::SizeI screenSize;
  uint32_t quantity;
  uint32_t shapeSize;

  uint32_t type;
  uint32_t op;
  uint32_t source;
};

// ============================================================================
// [BenchModule]
// ============================================================================

struct BenchModule
{
  BenchModule(BenchApp& app);
  virtual ~BenchModule();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual Fog::StringW getModuleName() const = 0;

  virtual void bench(BenchOutput& output, const BenchParams& params) = 0;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  BenchApp& app;
  Fog::TimeDelta totalTime;
  Fog::List<Fog::TimeDelta> sizeTime;

  Fog::Image screen;
  Fog::List<Fog::Image> sprites;
};

// ============================================================================
// [BenchRandom]
// ============================================================================

struct BenchRandom
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  BenchRandom(const BenchApp& app) :
    buf(app.randomData),
    pos(app.randomData),
    end(app.randomData + app.randomSize)
  {
  }

  // --------------------------------------------------------------------------
  // [Generator]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getNumber()
  {
    uint32_t result = *pos++;
    if (pos == end) pos = buf;
    return result;
  }

  FOG_INLINE uint32_t getNumber(uint32_t min, uint32_t max)
  {
    return min + (getNumber() % (max - min));
  }

  FOG_INLINE float getFloat()
  {
    return float((int)getNumber() & 0xFFFFF) / float(0xFFFFF);
  }
  
  FOG_INLINE float getFloat(float min, float max)
  {
    return min + (float((int)getNumber() & 0xFFFFF) / float(0xFFFFF)) * (max - min);
  }

  FOG_INLINE double getDouble()
  {
    return float((int)getNumber() & 0xFFFFF) / float(0xFFFFF);
  }
    
  FOG_INLINE double getDouble(double min, double max)
  {
    return min + (double((int)getNumber() & 0xFFFFF) / double(0xFFFFF)) * (max - min);
  }

  FOG_INLINE Fog::Argb32 getRgb32()
  {
    return Fog::Argb32(getNumber() | 0xFF000000);
  }
  
  FOG_INLINE Fog::Argb32 getArgb32()
  {
    return Fog::Argb32(getNumber());
  }

  FOG_INLINE Fog::RectI getRectI(const Fog::SizeI& screenSize, int maxW, int maxH)
  {
    Fog::RectI result(Fog::UNINITIALIZED);
    result.w = getNumber(1, maxW);
    result.h = getNumber(1, maxH);
    result.x = getNumber(0, screenSize.w - result.w);
    result.y = getNumber(0, screenSize.h - result.h);
    return result;
  }

  FOG_INLINE Fog::RectF getRectF(const Fog::SizeI& screenSize, int maxW, int maxH)
  {
    Fog::RectF result(Fog::UNINITIALIZED);
    result.w = getFloat(1, maxW);
    result.h = getFloat(1, maxH);
    result.x = getFloat(0, screenSize.w - result.w);
    result.y = getFloat(0, screenSize.h - result.h);
    return result;
  }

  FOG_INLINE Fog::PointI getPointI(const Fog::SizeI& screenSize)
  {
    Fog::PointI result(Fog::UNINITIALIZED);
    result.x = getNumber(0, screenSize.w);
    result.y = getNumber(0, screenSize.h);
    return result;
  }

  FOG_INLINE Fog::PointF getPointF(const Fog::SizeI& screenSize)
  {
    Fog::PointF result(Fog::UNINITIALIZED);
    result.x = getFloat(0, screenSize.w);
    result.y = getFloat(0, screenSize.h);
    return result;
  }

  FOG_INLINE Fog::PointD getPointD(const Fog::SizeI& screenSize)
  {
    Fog::PointD result(Fog::UNINITIALIZED);
    result.x = getDouble(0, screenSize.w);
    result.y = getDouble(0, screenSize.h);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  const uint32_t* buf;
  const uint32_t* pos;
  const uint32_t* end;
};

// [Guard]
#endif // _FOG_BENCHAPP_H
