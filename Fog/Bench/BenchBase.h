#ifndef _FOGBENCH_BENCHBASE_H
#define _FOGBENCH_BENCHBASE_H

#include <Fog/Core.h>
#include <Fog/Graphics.h>

#include <stdio.h>
#include <stdlib.h>

// ============================================================================
// [FogBench - Configuration]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

// Benchmarks are configured in CMakeLists.txt.

// There are no GDI+ headers when using MinGW.
#if !defined(__MINGW32__)
// #define FOG_BENCH_GDIPLUS
#endif

#else

// #define FOG_BENCH_CAIRO
// #define FOG_BENCH_QT4

#endif

#define FOG_BENCH_DONT_SAVE

// ============================================================================
// [FogBench - Constants]
// ============================================================================

enum BENCH_OPERATOR
{
  BENCH_OPERATOR_NONE = 0xFFFFFFFF,

  BENCH_OPERATOR_SRC = Fog::OPERATOR_SRC,
  BENCH_OPERATOR_SRC_OVER = Fog::OPERATOR_SRC_OVER
};

enum BENCH_SOURCE
{
  BENCH_SOURCE_NONE = 0xFFFFFFFF,

  BENCH_SOURCE_ARGB = 0,
  BENCH_SOURCE_PATTERN = 1
};

// ============================================================================
// [FogBench - Logging]
// ============================================================================

void benchLog(const Fog::String& s);

// ============================================================================
// [FogBench - Sprites]
// ============================================================================

#define NUM_SPRITES 4

extern Fog::Image _sprite[NUM_SPRITES];

void benchLoadSprites();

// ============================================================================
// [FogBench - Randomizer Tools]
// ============================================================================

static FOG_INLINE double randDouble()
{
  return (double)(rand() & (0xFFFF>>1)) / (double)(0xFFFF>>1);
}

static FOG_INLINE uint32_t randColor()
{
  return (rand() & 0xFFFF) | (rand() << 16);
}

static FOG_INLINE Fog::IntRect randRect(int w, int h, int rw, int rh)
{
  return Fog::IntRect(rand() % (w - rw), rand() % (h - rh), rw, rh);
}

static FOG_INLINE Fog::IntPoint randPoint(int w, int h)
{
  return Fog::IntPoint(rand() % w, rand() % h);
}

static FOG_INLINE Fog::DoublePoint randPointD(int w, int h)
{
  return Fog::DoublePoint(randDouble() * (double)w, randDouble() * (double)h);
}

// ============================================================================
// [FogBench - Randomizer - Argb]
// ============================================================================

struct Randomizer_Argb
{
  Randomizer_Argb();
  ~Randomizer_Argb();

  void init(int quantity);
  void free();

  Fog::Argb* data;
};

// ============================================================================
// [FogBench - Randomizer - Number]
// ============================================================================

struct Randomizer_Number
{
  Randomizer_Number();
  ~Randomizer_Number();

  void init(int quantity, int min_, int max_);
  void free();

  int* data;
};

// ============================================================================
// [FogBench - Randomizer - Rect]
// ============================================================================

struct Randomizer_Rect
{
  Randomizer_Rect();
  ~Randomizer_Rect();

  void init(int quantity, int w, int h, int sw, int sh);
  void free();

  Fog::IntRect* data;
};

// ============================================================================
// [FogBench - Randomizer - Polygon]
// ============================================================================

struct Randomizer_Polygon
{
  Randomizer_Polygon();
  ~Randomizer_Polygon();

  void init(int quantity, int w, int h, int sw, int sh);
  void free();

  Fog::DoublePoint* data;
};

// ============================================================================
// [FogBench - BenchmarkMaster]
// ============================================================================

struct AbstractBenchmarkContext;

struct BenchmarkMaster
{
  BenchmarkMaster();
  ~BenchmarkMaster();

  FOG_INLINE const int getWidth() const { return _width; }
  FOG_INLINE const int getHeight() const { return _height; }
  FOG_INLINE const int getQuantity() const { return _quantity; }
  FOG_INLINE const Fog::List<Fog::IntSize>& getSizes() const { return _sizes; }

  int _width;
  int _height;
  int _quantity;
  Fog::List<Fog::IntSize> _sizes;

  Fog::List<Fog::String> _testNames;
  Fog::List<AbstractBenchmarkContext*> _contexts;
};

// ============================================================================
// [FogBench - AbstractModule]
// ============================================================================

struct AbstractModule
{
  AbstractModule(int w, int h);
  virtual ~AbstractModule();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();
  virtual void bench(int quantity) = 0;

  virtual void save(int sw, int sh);

  virtual Fog::String getEngine() = 0;
  virtual Fog::String getType() = 0;

  virtual Fog::String getName();
  virtual Fog::String getInfo();

  int w, h;
  uint32_t op;
  uint32_t source;

  Fog::Image screen;
  Fog::Image sprite[4];
};

// ============================================================================
// [[FogBench - AbstractBenchmarkContext]
// ============================================================================

struct AbstractBenchmarkContext
{
  AbstractBenchmarkContext(BenchmarkMaster* master, const Fog::String& name);
  virtual ~AbstractBenchmarkContext();

  void header();
  void footer();
  void bench(AbstractModule& mod, uint32_t op, uint32_t source, int sw, int sh, int quantity);

  virtual void run() = 0;

  FOG_INLINE BenchmarkMaster* getMaster() const { return _master; }
  FOG_INLINE const Fog::String& getName() const { return _name; }

  FOG_INLINE const Fog::TimeDelta& getSharedTotal() const { return _sharedTotal; }
  FOG_INLINE const Fog::TimeDelta& getAllTotal() const { return _allTotal; }

  BenchmarkMaster* _master;
  Fog::String _name;

  // Test name and time pair.
  Fog::UnorderedHash<Fog::String, Fog::TimeDelta> _values;
  // Total time consumed only by tests included by each benchmark context.
  Fog::TimeDelta _sharedTotal;
  // Total time consumed by all tests in this benchmark context.
  Fog::TimeDelta _allTotal;
};

#endif // _FOGBENCH_BENCHBASE_H
