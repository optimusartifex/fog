#include <Fog/Core.h>
#include <Fog/Graphics.h>

#include "BenchBase.h"
#include "BenchFog.h"

#include "BenchCairo.h"
#include "BenchGdiPlus.h"
#include "BenchQt4.h"

#if defined(FOG_BENCH_GDIPLUS)
#pragma comment(lib, "gdiplus.lib")
#endif // FOG_BENCH_HDIPLUS

// ============================================================================
// [Bench]
// ============================================================================

static void writeInfo(BenchmarkMaster* master)
{
  static const char* yesno[2] = { "no", "yes" };

  Fog::String msg;
  msg.appendFormat("FogBench - Fog-Framework performance suite (version 0.3)\n");
  msg.appendFormat("\n");
  msg.appendFormat("Surface  : %dx%d\n", master->getWidth(), master->getHeight());
  msg.appendFormat("Quantity : %d\n", master->getQuantity());
  msg.appendFormat("\n");
  msg.appendFormat("Processor: %s\n", Fog::getCpuInfo()->brand);
  msg.appendFormat("Features1: MMX=%s, MMXExt=%s, 3dNow=%s, 3dNowExt=%s, SSE=%s\n",
    yesno[Fog::getCpuInfo()->hasFeature(Fog::CpuInfo::FEATURE_MMX)],
    yesno[Fog::getCpuInfo()->hasFeature(Fog::CpuInfo::FEATURE_MMXExt)],
    yesno[Fog::getCpuInfo()->hasFeature(Fog::CpuInfo::FEATURE_3dNow)],
    yesno[Fog::getCpuInfo()->hasFeature(Fog::CpuInfo::FEATURE_3dNowExt)],
    yesno[Fog::getCpuInfo()->hasFeature(Fog::CpuInfo::FEATURE_SSE)]);
  msg.appendFormat("Features2: SSE2=%s, SSE3=%s, SSSE3=%s\n",
    yesno[Fog::getCpuInfo()->hasFeature(Fog::CpuInfo::FEATURE_SSE2)],
    yesno[Fog::getCpuInfo()->hasFeature(Fog::CpuInfo::FEATURE_SSE3)],
    yesno[Fog::getCpuInfo()->hasFeature(Fog::CpuInfo::FEATURE_SSSE3)]);
  msg.appendFormat("CPU count: %u\n", Fog::getCpuInfo()->numberOfProcessors);
  msg.appendFormat("\n");
  benchLog(msg);
}

static void writeSummary(BenchmarkMaster* master)
{
  sysuint_t contextsCount = master->_contexts.getLength();
  sysuint_t testsCount = master->_testNames.getLength();

  sysuint_t i, c;

  Fog::String msg;

  msg.appendFormat("Test                    |Size      ");
  for (c = 0; c < contextsCount; c++)
    msg.appendFormat("|%-16W", &master->_contexts.at(c)->_name);
  msg.appendFormat("\n");

  msg.appendFormat("------------------------+----------");
  for (c = 0; c < contextsCount; c++)
    msg.appendFormat("+-----------");
  msg.appendFormat("\n");

  for (i = 0; i < testsCount; i++)
  {
    Fog::String name = master->_testNames.at(i);
    Fog::String type = name;
    Fog::String size;

    if (type.contains(Fog::Char(' ')))
    {
      size = type.substring(Fog::Range(type.indexOf(Fog::Char(' ')) + 1));
      type = type.substring(Fog::Range(0, type.indexOf(Fog::Char(' '))));
    }
    msg.appendFormat("%-24W|%-10W", &type, &size);

    for (c = 0; c < contextsCount; c++) 
    {
      Fog::TimeDelta delta;
      if (master->_contexts.at(c)->_values.contains(name))
        delta = master->_contexts.at(c)->_values.value(name);
      msg.appendFormat("|%10.3f ", delta.inMillisecondsF());
    }
    msg.appendFormat("\n");
  }

  msg.appendFormat("-----------------------------------");
  for (c = 0; c < contextsCount; c++)
    msg.appendFormat("+-----------");
  msg.appendFormat("\n");

  msg.appendFormat("Summary                            ");
  for (c = 0; c < contextsCount; c++)
    msg.appendFormat("|%10.3f ", master->_contexts.at(c)->_allTotal.inMillisecondsF());
  msg.appendFormat("\n");

  msg.appendFormat("\n");
  benchLog(msg);
}

static void benchAll()
{
  BenchmarkMaster master;
  writeInfo(&master);

  FogBenchmarkContext ctx_fog_st(&master, Fog::Ascii8("Fog-st"));
  ctx_fog_st.run();

  FogBenchmarkContext ctx_fog_mt(&master, Fog::Ascii8("Fog-mt"));
  ctx_fog_mt.run();

#if defined(FOG_BENCH_GDIPLUS)
  GdiPlusBenchmarkContext ctx_gdiplus(&master);
  ctx_gdiplus.run();
#endif // FOG_BENCH_GDIPLUS

#if defined(FOG_BENCH_QT4)
  Qt4BenchmarkContext ctx_qt4(&master);
  ctx_qt4.run();
#endif // FOG_BENCH_QT4

#if defined(FOG_BENCH_CAIRO)
  CairoBenchmarkContext ctx_cairo(&master);
  ctx_cairo.run();
#endif // FOG_BENCH_CAIRO

  writeSummary(&master);
}

// ============================================================================
// [MAIN]
// ============================================================================

#undef main
int main(int argc, char* argv[])
{
#if defined(FOG_BENCH_GDIPLUS)
  // Initialize GDI+
  ULONG_PTR gdiplusToken;
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
#endif // FOG_BENCH_GDIPLUS

  benchLoadSprites();
  benchAll();

#if defined(FOG_BENCH_GDIPLUS)
  // Shutdown GDI+
  Gdiplus::GdiplusShutdown(gdiplusToken);
#endif // FOG_BENCH_GDIPLUS

#if defined(FOG_OS_WINDOWS)
  system("pause");
#endif // FOG_OS_WINDOWS

  return 0;
}
