// [Fog-Bench]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include "BenchApp.h"
#include "BenchFog.h"

#if defined(FOG_BENCH_CAIRO)
#include "BenchCairo.h"
#endif // FOG_BENCH_CAIRO

#if defined(FOG_BENCH_GDIPLUS)
#include "BenchGdiPlus.h"
#endif // FOG_BENCH_GDIPLUS

#if defined(FOG_BENCH_QT4)
#include "BenchQt4.h"
#endif // FOG_BENCH_QT4

// [Dependencies - Resources]
#include "../Sample/Resources.h"

// [Dependencies - C]
#include <stdlib.h>

// ============================================================================
// [BenchApp - Construction / Destruction]
// ============================================================================

BenchApp::BenchApp(const Fog::SizeI& screenSize, uint32_t quantity) :
  screenSize(screenSize),
  quantity(quantity),
  saveImages(false)
{
}

BenchApp::~BenchApp()
{
  if (randomData != NULL)
    Fog::MemMgr::free(randomData);

  deleteModules();
}

// ============================================================================
// [BenchApp - Run]
// ============================================================================

bool BenchApp::hasBenchSource(uint32_t benchType)
{
  switch (benchType)
  {
    case BENCH_TYPE_FILL_RECT_I:
    case BENCH_TYPE_FILL_RECT_F:
    case BENCH_TYPE_FILL_RECT_ROTATE:
    case BENCH_TYPE_FILL_ROUND:
    case BENCH_TYPE_FILL_POLYGON:
    case BENCH_TYPE_FILL_COMPLEX:
      return true;

    default:
      return false;
  }
}

void BenchApp::runAll()
{
  Fog::ListIterator<BenchModule*> it(modules);

  while (it.isValid())
  {
    runModule(it.getItem());
    it.next();
  }
}

void BenchApp::runModule(BenchModule* module)
{
  size_t formatIndex;
  uint32_t type;

  BenchParams params;
  params.screenSize = screenSize;
  params.quantity = quantity;

  Fog::List<Fog::TimeDelta> t;
  Fog::List<uint32_t> formats = module->getSupportedPixelFormats();

  for (formatIndex = 0; formatIndex < formats.getLength(); formatIndex++)
  {
    params.format = formats[formatIndex];
    logBenchHeader(module, params);

    module->sizeTime.clear();
    for (size_t i = 0; i < sizeList.getLength(); i++)
      module->sizeTime.append(Fog::TimeDelta(0));

    for (type = 0; type < BENCH_TYPE_COUNT; type++)
    {
      if (sprites.isEmpty() && (
          type == BENCH_TYPE_BLIT_IMAGE_I ||
          type == BENCH_TYPE_BLIT_IMAGE_F ||
          type == BENCH_TYPE_BLIT_IMAGE_ROTATE))
      {
        continue;
      }

      params.type = type;
      params.source = hasBenchSource(type) ? 0 : BENCH_SOURCE_NONE;

      for (;;)
      {
        params.op = type != BENCH_TYPE_CREATE_DESTROY ? 0 : BENCH_OPERATOR_NONE;

        for (;;)
        {
          size_t sizeIndex = Fog::INVALID_INDEX;

          Fog::StringW s = getTestString(params);
          s.justify(22, Fog::CharW(' '), Fog::TEXT_JUSTIFY_LEFT);
          s.append(Fog::CharW('|'));

          if (type != BENCH_TYPE_CREATE_DESTROY)
          {
            sizeIndex = 0;
            params.shapeSize = sizeList.getAt(sizeIndex);
          }
          else
          {
            s.append(Fog::CharW(' '), (sizeList.getLength() - 1) * 8);
            params.shapeSize = 0;
          }

          for (;;)
          {
            BenchOutput output;
            module->bench(output, params);

            s.appendFormat("%7qu|", (uint64_t)output.time.getMilliseconds());

            if (sizeIndex != Fog::INVALID_INDEX)
              module->sizeTime.getDataX()[sizeIndex] += output.time;

            registerResults(module, params, output);

            if (sizeIndex >= sizeList.getLength() - 1)
              break;
            sizeIndex++;
            params.shapeSize = sizeList.getAt(sizeIndex);
          }

          s.append(Fog::CharW('\n'));
          logs(s);

          if (params.op >= BENCH_OPERATOR_COUNT - 1)
            break;
          params.op++;
        }

        if (params.source >= BENCH_SOURCE_COUNT - 1)
          break;
        params.source++;
      }
    }

    logBenchFooter(module, params);
  }
}

void BenchApp::registerResults(BenchModule* module, const BenchParams& params, const BenchOutput& output)
{
  module->totalTime += output.time;

  if (saveImages && params.type != BENCH_TYPE_CREATE_DESTROY)
  {
    Fog::StringW fileName;

    Fog::StringW moduleString = module->getModuleName();
    Fog::StringW formatString = getFormatString(params.format);
    Fog::StringW testString = getTestString(params);

    fileName.append(Fog::Ascii8("FogBench-Dump/"));
    fileName.append(moduleString);
    fileName.append(Fog::Ascii8("-"));
    fileName.append(formatString);
    fileName.append(Fog::Ascii8("-"));
    fileName.append(testString);

    if (params.shapeSize)
      fileName.appendFormat("-[%03ux%03u]", params.shapeSize, params.shapeSize);
    fileName.append(Fog::Ascii8(".bmp"));

    module->screen.forceFormat(Fog::IMAGE_FORMAT_XRGB32);
    module->screen.writeToFile(fileName);
    module->screen.forceFormat(Fog::IMAGE_FORMAT_PRGB32);
  }
}

// ============================================================================
// [BenchApp - Modules]
// ============================================================================

void BenchApp::addModule(BenchModule* module)
{
  modules.append(module);
}

void BenchApp::deleteModules()
{
  Fog::ListIterator<BenchModule*> it(modules);

  while (it.isValid())
  {
    delete it.getItem();
    it.next();
  }

  modules.reset();
}

// ============================================================================
// [BenchApp - Data]
// ============================================================================

void BenchApp::loadData()
{
  Fog::Image sprite;

  sprite.readFromBuffer(_resource_ktip_png, FOG_ARRAY_SIZE(_resource_ktip_png));
  sprites.append(sprite);

  sprite.readFromBuffer(_resource_ksplash_png, FOG_ARRAY_SIZE(_resource_ksplash_png));
  sprites.append(sprite);

  sprite.readFromBuffer(_resource_babelfish_png, FOG_ARRAY_SIZE(_resource_babelfish_png));
  sprites.append(sprite);

  sprite.readFromBuffer(_resource_firewall_png, FOG_ARRAY_SIZE(_resource_firewall_png));
  sprites.append(sprite);
}

void BenchApp::makeRand()
{
  randomSize = 13033;
  randomData = reinterpret_cast<uint32_t*>(Fog::MemMgr::alloc(randomSize * sizeof(uint32_t)));

  if (randomData == NULL)
  {
    logf("Can't initialize random numbers table, out of memory.\n");
    exit(1);
  }

  for (uint32_t i = 0; i < randomSize; i++)
  {
    randomData[i] = ((rand() & 0xFFFF)      ) ^
                    ((rand() & 0xFFFF) <<  8) ^
                    ((rand() & 0xFFFF) << 16) ;
  }
}

// ============================================================================
// [BenchApp - Logging]
// ============================================================================

Fog::StringW BenchApp::getBenchString(uint32_t bench) const
{
  static const char* data[] = {
    "Create/Destroy",
    "FillRectI",
    "FillRectF",
    "FillRectRot",
    "FillRound",
    "FillPolygon",
    "FillComplex",
    "BlitImageI",
    "BlitImageF",
    "BlitImageRot"
  };

  if (bench < BENCH_TYPE_COUNT)
    return Fog::StringW::fromAscii8(data[bench]);
  else
    return Fog::StringW();
}

Fog::StringW BenchApp::getFormatString(uint32_t format) const
{
  switch (format)
  {
    case Fog::IMAGE_FORMAT_PRGB32: return Fog::StringW::fromAscii8("PRGB32");
    case Fog::IMAGE_FORMAT_XRGB32: return Fog::StringW::fromAscii8("XRGB32");
    case Fog::IMAGE_FORMAT_RGB24 : return Fog::StringW::fromAscii8("RGB24");
    case Fog::IMAGE_FORMAT_A8    : return Fog::StringW::fromAscii8("A8");
    default:
      return Fog::StringW();
  }
}

Fog::StringW BenchApp::getSourceString(uint32_t source) const
{
  static const char* data[] = {
    "Solid",
    "LinGr"
  };

  if (source < BENCH_SOURCE_COUNT)
    return Fog::StringW::fromAscii8(data[source]);
  else
    return Fog::StringW();
}

Fog::StringW BenchApp::getOperatorString(uint32_t op) const
{
  static const char* data[] = {
    "Copy",
    "Over"
  };

  if (op < BENCH_OPERATOR_COUNT)
    return Fog::StringW::fromAscii8(data[op]);
  else
    return Fog::StringW();
}

Fog::StringW BenchApp::getTestString(const BenchParams& params) const
{
  Fog::StringW s = getBenchString(params.type);

  if (params.source != BENCH_SOURCE_NONE)
  {
    s += Fog::CharW('-');
    s += getSourceString(params.source);
  }

  if (params.op != BENCH_OPERATOR_NONE)
  {
    s += Fog::CharW('-');
    s += getOperatorString(params.op);
  }

  return s;
}

void BenchApp::logInfo()
{
  static const char* yesno[] = { "no", "yes" };

  logf("FogBench - Fog-Framework performance suite (version 0.4)\n");
  logf("\n");

  logf("Surface  : 640x480\n", screenSize.w, screenSize.h);
  logf("Quantity : %u\n", quantity);
  logf("\n");

  logf("Processor: %s\n", Fog::Cpu::get()->getBrand());
  logf("Features1: MMX=%s, MMXExt=%s, 3dNow=%s, 3dNowExt=%s\n",
    yesno[Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_MMX)],
    yesno[Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_MMX_EXT)],
    yesno[Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_3DNOW)],
    yesno[Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_3DNOW_EXT)]);
  logf("Features2: SSE=%s, SSE2=%s, SSE3=%s, SSSE3=%s SSE4.1=%s, SSE4.2=%s\n",
    yesno[Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_SSE)],
    yesno[Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_SSE2)],
    yesno[Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_SSE3)],
    yesno[Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_SSSE3)],
    yesno[Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_SSE4_1)],
    yesno[Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_SSE4_2)]);
  logf("Features3: AVX=%s\n",
    yesno[Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_AVX)]);
  logf("CPU Count: %u\n", Fog::Cpu::get()->getNumberOfProcessors());
  logf("\n");
}

void BenchApp::logBenchHeader(BenchModule* module, const BenchParams& params)
{
  Fog::StringW s;
  Fog::StringW l;

  s.append(module->getModuleName());
  s.append(Fog::CharW(' '));
  s.append(getFormatString(params.format));

  s.justify(22, Fog::CharW(' '), Fog::TEXT_JUSTIFY_LEFT);
  l.justify(22, Fog::CharW('-'), Fog::TEXT_JUSTIFY_LEFT);

  s.append(Fog::CharW('|'));
  l.append(Fog::CharW('+'));

  for (size_t i = 0; i < sizeList.getLength(); i++)
  {
    Fog::StringW cell;
    uint32_t size = sizeList.getAt(i);

    cell.appendFormat("%ux%u", size, size);
    cell.justify(7, Fog::CharW(' '), Fog::TEXT_JUSTIFY_RIGHT);

    s.append(cell);
    l.append(Fog::CharW('-'), 7);

    s.append(Fog::CharW('|'));
    l.append(Fog::CharW('+'));
  }

  s.append(Fog::CharW('\n'));
  l.append(Fog::CharW('\n'));

  logs(s);
  logs(l);
}

void BenchApp::logBenchFooter(BenchModule* module, const BenchParams& params)
{
  Fog::StringW s;
  Fog::StringW l;

  s.append(Fog::Ascii8("Total"));

  s.justify(22, Fog::CharW(' '), Fog::TEXT_JUSTIFY_LEFT);
  l.justify(22, Fog::CharW('-'), Fog::TEXT_JUSTIFY_LEFT);

  s.append(Fog::CharW('|'));
  l.append(Fog::CharW('+'));

  for (size_t i = 0; i < module->sizeTime.getLength(); i++)
  {
    Fog::StringW cell;

    cell.appendFormat("%u", module->sizeTime.getAt(i).getMilliseconds());
    cell.justify(7, Fog::CharW(' '), Fog::TEXT_JUSTIFY_RIGHT);

    s.append(cell);
    l.append(Fog::CharW('-'), 7);

    s.append(Fog::CharW('|'));
    l.append(Fog::CharW('+'));
  }

  s.append(Fog::CharW('\n'));
  l.append(Fog::CharW('\n'));

  logs(l);
  logs(s);

  logf("\n");
}

void BenchApp::logBenchOutput(BenchModule* module, const BenchParams& params, const BenchOutput& output)
{
#if 0
  Fog::StringW moduleString = module->getModuleName();
  //Fog::StringW paramsString = getParamsString(params);

  Fog::StringW s;
  s += getBenchString(params.type);

  if (params.source != BENCH_SOURCE_NONE)
  {
    s += Fog::CharW('-');
    s += getSourceString(params.source);
  }

  if (params.op != BENCH_OPERATOR_NONE)
  {
    s += Fog::CharW('-');
    s += getOperatorString(params.op);
  }

  s.justify(21, Fog::CharW(' '), Fog::TEXT_JUSTIFY_LEFT);
  s.append(Fog::CharW('|'));

  if (params.shapeSize != 0)
  {
    s += Fog::CharW(' ');
    s.appendFormat("%u", params.shapeSize);
  }

  s.appendFormat("%8qu [ms]", (uint64_t)output.time.getMilliseconds());

  s += Fog::CharW('\n');
  logs(s);
#endif
}

void BenchApp::logs(const Fog::StringW& str)
{
  Fog::StringA str8;
  Fog::TextCodec::local8().encode(str8, str);
  fputs(str8.getData(), stderr);
}

void BenchApp::logf(const char* fmt, ...)
{
  Fog::StringW str;

  va_list ap;
  va_start(ap, fmt);
  str.vFormat(fmt, ap);
  va_end(ap);

  logs(str);
}

// ============================================================================
// [BenchModule]
// ============================================================================

BenchModule::BenchModule(BenchApp& app) :
  app(app),
  totalTime(0)
{
}

BenchModule::~BenchModule()
{
}

void BenchModule::prepareSprites(int size)
{
  size_t i, count = app.sprites.getLength();
  for (i = 0; i < count; i++)
  {
    Fog::Image sprite;
    
    if (Fog::Image::resize(sprite, Fog::SizeI(size, size), app.sprites[i], Fog::IMAGE_RESIZE_BILINEAR) != Fog::ERR_OK)
    {
      freeSprites();
      return;
    }
    
    sprites.append(sprite);
  }
}

void BenchModule::freeSprites()
{
  sprites.clear();
}

// ============================================================================
// [Main]
// ============================================================================

int main(int argc, char* argv[])
{
  BenchApp app(Fog::SizeI(600, 600), 5000);

  // Testing...
  app.saveImages = true;

  // Show FogBench info.
  app.logInfo();

  // Load data.
  app.loadData();
  app.makeRand();

  // Create a size list for tests.
  app.sizeList.append(8);
  app.sizeList.append(16);
  app.sizeList.append(32);
  app.sizeList.append(64);
  app.sizeList.append(128);

  // Add modules.
  app.addModule(new BenchFog(app));

#if defined(FOG_BENCH_CAIRO)
  app.addModule(new BenchCairo(app));
#endif // FOG_BENCH_CAIRO

#if defined(FOG_BENCH_GDIPLUS)
  app.addModule(new BenchGdiPlus(app));
#endif // FOG_BENCH_GDIPLUS

#if defined(FOG_BENCH_QT4)
  app.addModule(new BenchQt4(app));
#endif // FOG_BENCH_QT4

  // Run the tests.
  app.runAll();

#if defined(FOG_OS_WINDOWS)
  system("pause");
#endif // FOG_OS_WINDOWS

  return 0;
}
