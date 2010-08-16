#include "BenchBase.h"

// ============================================================================
// [FogBench - Logging]
// ============================================================================

void benchLog(const Fog::String& s)
{
  Fog::ByteArray b;
  Fog::TextCodec::local8().fromUnicode(b, s);
  fprintf(stderr, "%s", b.getData());
}

// ============================================================================
// [FogBench - Sprites]
// ============================================================================

Fog::Image _sprite[NUM_SPRITES];

void benchLoadSprites()
{
  static const char* spriteNames[NUM_SPRITES] =
  {
    "babelfish.png",
    "blockdevice.png",
    "drop.png",
    "kweather.png"
  };

  bool spritesNotFound = false;
  int i;

  for (i = 0; i < NUM_SPRITES; i++)
  {
    if (_sprite[i].readFromFile(Fog::Ascii8(spriteNames[i])) != Fog::ERR_OK)
    {
      fog_debug("Warning: Can't load sprite './%s'", spriteNames[i]);
      spritesNotFound = true;
    }
    else
    {
      _sprite[i].premultiply();
    }
  }

  if (spritesNotFound)
  {
    fog_debug("\nDownload sprites from these locations and place them to 'FogBench' working directory:");
    for (i = 0; i < NUM_SPRITES; i++)
    {
      fog_debug("  http://kobalicek.com/data/fog/sprites/%s", spriteNames[i]);
    }
    fog_debug("\nAll image based tests will be incorrect!\n");
  }
}

// ============================================================================
// [FogBench - Randomizer - Argb]
// ============================================================================

Randomizer_Argb::Randomizer_Argb() : data(NULL) {}
Randomizer_Argb::~Randomizer_Argb() { free(); }

void Randomizer_Argb::init(int quantity)
{
  free();
  data = (Fog::Argb*)Fog::Memory::alloc(sizeof(Fog::Argb) * quantity);
  for (int a = 0; a < quantity; a++) data[a] = randColor();
}

void Randomizer_Argb::free()
{
  Fog::Memory::free(data);
  data = NULL;
}

// ============================================================================
// [FogBench - Randomizer -Number]
// ============================================================================

Randomizer_Number::Randomizer_Number() : data(NULL) {}
Randomizer_Number::~Randomizer_Number() { free(); }

void Randomizer_Number::init(int quantity, int min_, int max_)
{
  free();
  data = (int*)Fog::Memory::alloc(sizeof(int) * quantity);

  for (int a = 0; a < quantity; a++)
  {
    data[a] = (int)(randDouble() * (double)(max_ - min_) + min_);
  }
}

void Randomizer_Number::free()
{
  Fog::Memory::free(data);
  data = NULL;
}

// ============================================================================
// [FogBench - Randomizer - Rect]
// ============================================================================

Randomizer_Rect::Randomizer_Rect() : data(NULL) {}
Randomizer_Rect::~Randomizer_Rect() { free(); }

void Randomizer_Rect::init(int quantity, int w, int h, int sw, int sh)
{
  free();
  data = (Fog::IntRect*)Fog::Memory::alloc(sizeof(Fog::IntRect) * quantity);
  for (int a = 0; a < quantity; a++) data[a] = randRect(w, h, sw, sh);
}

void Randomizer_Rect::free()
{
  Fog::Memory::free(data);
  data = NULL;
}

// ============================================================================
// [FogBench - Randomizer - Polygon]
// ============================================================================

Randomizer_Polygon::Randomizer_Polygon() : data(NULL) {}
Randomizer_Polygon::~Randomizer_Polygon() { free(); }

void Randomizer_Polygon::init(int quantity, int w, int h, int sw, int sh)
{
  free();
  data = (Fog::DoublePoint*)Fog::Memory::alloc(sizeof(Fog::DoublePoint) * quantity * 10);

  double x = 0.0;
  double y = 0.0;

  for (int a = 0; a < quantity * 10; a++)
  {
    if ((a % 10) == 0)
    {
      x = randDouble() * (double)(w - sw);
      y = randDouble() * (double)(h - sh);
    }
    data[a] = randPointD(sw, sh).translated(x, y);
  }
}

void Randomizer_Polygon::free()
{
  Fog::Memory::free(data);
  data = NULL;
}

// ============================================================================
// [FogBench - AbstractModule]
// ============================================================================

AbstractModule::AbstractModule(int w, int h) : 
  w(w),
  h(h),
  op(Fog::OPERATOR_SRC),
  source(BENCH_SOURCE_NONE)
{
  screen.create(w, h, Fog::IMAGE_FORMAT_PRGB32);
  screen.clear(0x00000000);

  for (int a = 0; a < NUM_SPRITES; a++) sprite[a] = _sprite[a];
}

AbstractModule::~AbstractModule()
{
}

void AbstractModule::prepare(int quantity, int sw, int sh) {}
void AbstractModule::finish() {}

void AbstractModule::save(int sw, int sh)
{
#if !defined(FOG_BENCH_DONT_SAVE)
  Fog::String fileName;
  Fog::String info = getInfo();

  fileName.set(Fog::Ascii8("Images/Bench - "));
  fileName.append(info);
  if (sw && sh) fileName.appendFormat(" [%0.3dx%0.3d]", sw, sh);
  fileName.append(Fog::Ascii8(".bmp"));

  Fog::Image t(screen);
  t.forceFormat(Fog::IMAGE_FORMAT_XRGB32);
  t.writeToFile(fileName);
#endif
}

Fog::String AbstractModule::getName()
{
  Fog::String name = getType();

  // Append source to name.
  if (source != BENCH_SOURCE_NONE)
  {
    name.append(source == BENCH_SOURCE_ARGB ? Fog::Ascii8("-Argb") : Fog::Ascii8("-Grad"));
  }

  // Append operator to name.
  if (op != BENCH_OPERATOR_NONE)
  {
    name.append(op == Fog::OPERATOR_SRC ? Fog::Ascii8("-Copy") : Fog::Ascii8("-Over"));
  }

  return name;
}

Fog::String AbstractModule::getInfo()
{
  Fog::String info = getEngine();
  info.append(Fog::Ascii8(" - "));
  info.append(getName());
  return info;
}

// ============================================================================
// [FogBench - BenchmarkMaster]
// ============================================================================

BenchmarkMaster::BenchmarkMaster()
{
  _width = 640;
  _height = 480;
  _quantity = 1000;

  _sizes.append(Fog::IntSize(8  , 8  ));
  _sizes.append(Fog::IntSize(16 , 16 ));
  _sizes.append(Fog::IntSize(32 , 32 ));
  _sizes.append(Fog::IntSize(64 , 64 ));
  _sizes.append(Fog::IntSize(128, 128));
  _sizes.append(Fog::IntSize(256, 256));
}

BenchmarkMaster::~BenchmarkMaster()
{
}

// ============================================================================
// [[FogBench - AbstractBenchmarkContext]
// ============================================================================

AbstractBenchmarkContext::AbstractBenchmarkContext(BenchmarkMaster* master, const Fog::String& name) :
  _master(master),
  _name(name)
{
}

AbstractBenchmarkContext::~AbstractBenchmarkContext()
{
}

void AbstractBenchmarkContext::header()
{
  Fog::String msg;
  msg.appendFormat("%-24W|Size      |Time\n", &_name);
  msg.appendFormat("------------------------+----------+---------------\n");
  benchLog(msg);
}

void AbstractBenchmarkContext::footer()
{
  Fog::String msg;
  msg.appendFormat("-----------------------------------+----------\n");
  msg.appendFormat("Summary                            |%10.3f\n", _allTotal.inMillisecondsF());
  msg.appendFormat("\n");
  benchLog(msg);
}

void AbstractBenchmarkContext::bench(AbstractModule& mod, uint32_t op, uint32_t source, int sw, int sh, int quantity)
{
  // Clear random seed (so all tests will behave identically).
  srand(43);

  mod.op = op;
  mod.source = source;

  mod.prepare(quantity, sw, sh);
  Fog::TimeTicks ticks = Fog::TimeTicks::highResNow();
  mod.bench(quantity);
  Fog::TimeDelta delta = Fog::TimeTicks::highResNow() - ticks;
  mod.finish();

  // Add the test to the _values hash and register test name in master instance.
  {
    Fog::String name = mod.getName();
    if (sw != 0 && sh != 0) name.appendFormat(" %dx%d", sw, sh);
    _values.put(name, delta);

    if (!_master->_testNames.contains(name)) _master->_testNames.append(name);
  }

  // Write the test output.
  {
    Fog::String msg;
    Fog::String name = mod.getName();
    Fog::String size;
    Fog::String time;

    if (sw != 0 && sh != 0) size.format("%dx%d", sw, sh);
    time.format("%10.3f", delta.inMillisecondsF());

    msg.appendFormat("%-24W|%-10W|%W\n", &name, &size, &time);
    benchLog(msg);
  }

  mod.save(sw, sh);

  _allTotal += delta;
}
