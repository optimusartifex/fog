#include "BenchFog.h"

// ============================================================================
// [FogBench - Helpers]
// ============================================================================

static void setupFogPatternForPoint(
  Fog::Pattern& pattern, const Fog::PointD& pt0, const Fog::PointD& pt1, const Fog::ArgbI& argb)
{
  pattern.setPoints(pt0, pt1);
  pattern.resetStops();
  pattern.addStop(Fog::ArgbStop(0.0f, 0xFFFFFFFF));
  pattern.addStop(Fog::ArgbStop(0.5f, 0x7FFF0000));
  pattern.addStop(Fog::ArgbStop(1.0f, argb));
}

static void setupFogPatternForRect(
  Fog::Pattern& pattern, const Fog::RectI& rect, const Fog::ArgbI& argb)
{
  setupFogPatternForPoint(pattern,
    Fog::PointD(rect.x, rect.y),
    Fog::PointD(rect.x+rect.w, rect.y+rect.h),
    argb);
}

// ============================================================================
// [FogBench - FogModule]
// ============================================================================

FogModule::FogModule(int w, int h) : AbstractModule(w, h)
{
  engine = Fog::PAINT_ENGINE_RASTER_ST;
}

FogModule::~FogModule()
{
}

Fog::String FogModule::getEngine()
{
  Fog::String info;
  info.format("Fog-%s", engine == Fog::PAINT_ENGINE_RASTER_MT ? "mt" : "st");
  return info;
}

void FogModule::setEngine(int engine)
{
  this->engine = engine;
}

void FogModule::configurePainter(Fog::Painter& p)
{
  p.setEngine(engine);
  p.setOperator(op);
  p.setHint(Fog::PAINTER_HINT_COLOR_INTERPOLATION, Fog::COLOR_INTERPOLATION_NEAREST);
}

// ============================================================================
// [FogBench - FogModule_CreateDestroy]
// ============================================================================

FogModule_CreateDestroy::FogModule_CreateDestroy(int w, int h) : FogModule(w, h) {}
FogModule_CreateDestroy::~FogModule_CreateDestroy() {}

void FogModule_CreateDestroy::bench(int quantity)
{
  for (int a = 0; a < quantity; a++)
  {
    Fog::Painter p(screen, Fog::NO_FLAGS);
    configurePainter(p);
  }
}

Fog::String FogModule_CreateDestroy::getType()
{
  return Fog::Ascii8("Create-Destroy");
}

// ============================================================================
// [FogBench - FogModule_FillRect]
// ============================================================================

FogModule_FillRect::FogModule_FillRect(int w, int h) : FogModule(w, h) {}
FogModule_FillRect::~FogModule_FillRect() {}

void FogModule_FillRect::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void FogModule_FillRect::finish()
{
  r_rect.free();
  r_argb.free();
}

void FogModule_FillRect::bench(int quantity)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p);

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++)
    {
      p.setSource(r_argb.data[a]);
      p.fillRect(r_rect.data[a]);
    }
  }
  else
  {
    Fog::Pattern pattern;
    pattern.setType(Fog::PATTERN_LINEAR_GRADIENT);
    pattern.setSpread(Fog::PATTERN_SPREAD_PAD);

    for (int a = 0; a < quantity; a++)
    {
      setupFogPatternForRect(pattern, r_rect.data[a], r_argb.data[a]);
      p.setSource(pattern);
      p.fillRect(r_rect.data[a]);
      p.resetSource();
    }
  }
}

Fog::String FogModule_FillRect::getType()
{
  return Fog::Ascii8("FillRect");
}

// ============================================================================
// [FogBench - FogModule_FillRectSubPX]
// ============================================================================

FogModule_FillRectSubPX::FogModule_FillRectSubPX(int w, int h) : FogModule_FillRect(w, h) {}
FogModule_FillRectSubPX::~FogModule_FillRectSubPX() {}

void FogModule_FillRectSubPX::bench(int quantity)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p);

  double sub = 0.1;
  double inc = 0.8 / (double)quantity;

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++, sub += inc)
    {
      Fog::RectI r = r_rect.data[a];

      p.setSource(r_argb.data[a]);
      p.fillRect(Fog::RectD(sub + r.x, sub + r.y, (double)r.w, (double)r.h));
    }
  }
  else
  {
    Fog::Pattern pattern;
    pattern.setType(Fog::PATTERN_LINEAR_GRADIENT);
    pattern.setSpread(Fog::PATTERN_SPREAD_PAD);

    for (int a = 0; a < quantity; a++, sub += inc)
    {
      Fog::RectI r = r_rect.data[a];

      setupFogPatternForRect(pattern, r_rect.data[a], r_argb.data[a]);
      p.setSource(pattern);
      p.fillRect(Fog::RectD(sub + r.x, sub + r.y, (double)r.w, (double)r.h));
      p.resetSource();
    }
  }
}

Fog::String FogModule_FillRectSubPX::getType()
{
  return Fog::Ascii8("FillRectSubPX");
}

// ============================================================================
// [FogBench - FogModule_FillRectAffine]
// ============================================================================

FogModule_FillRectAffine::FogModule_FillRectAffine(int w, int h) : FogModule_FillRect(w, h) {}
FogModule_FillRectAffine::~FogModule_FillRectAffine() {}

void FogModule_FillRectAffine::bench(int quantity)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p);

  double cx = (double)w / 2.0;
  double cy = (double)h / 2.0;
  double rot = 0.0;

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++, rot += 0.01)
    {
      Fog::TransformD t;
      t.translate(cx, cy);
      t.rotate(rot);
      t.translate(-cx, -cy);

      p.setTransform(t);
      p.setSource(r_argb.data[a]);
      p.fillRect(r_rect.data[a]);
    }
  }
  else
  {
    Fog::Pattern pattern;
    pattern.setType(Fog::PATTERN_LINEAR_GRADIENT);
    pattern.setSpread(Fog::PATTERN_SPREAD_PAD);

    for (int a = 0; a < quantity; a++, rot += 0.01)
    {
      Fog::TransformD t;
      t.translate(cx, cy);
      t.rotate(rot);
      t.translate(-cx, -cy);
      p.setTransform(t);

      setupFogPatternForRect(pattern, r_rect.data[a], r_argb.data[a]);
      p.setSource(pattern);
      p.fillRect(r_rect.data[a]);
      p.resetSource();
    }
  }
}

Fog::String FogModule_FillRectAffine::getType()
{
  return Fog::Ascii8("FillRectAffine");
}

// ============================================================================
// [FogBench - FogModule_FillRound]
// ============================================================================

FogModule_FillRound::FogModule_FillRound(int w, int h) : FogModule_FillRect(w, h) {}
FogModule_FillRound::~FogModule_FillRound() {}

void FogModule_FillRound::bench(int quantity)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p);

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++)
    {
      p.setSource(r_argb.data[a]);
      p.fillRound(r_rect.data[a], Fog::PointI(8, 8));
    }
  }
  else
  {
    Fog::Pattern pattern;
    pattern.setType(Fog::PATTERN_LINEAR_GRADIENT);
    pattern.setSpread(Fog::PATTERN_SPREAD_PAD);

    for (int a = 0; a < quantity; a++)
    {
      setupFogPatternForRect(pattern, r_rect.data[a], r_argb.data[a]);
      p.setSource(pattern);
      p.fillRound(r_rect.data[a], Fog::PointI(8, 8));
      p.resetSource();
    }
  }
}

Fog::String FogModule_FillRound::getType()
{
  return Fog::Ascii8("FillRound");
}

// ============================================================================
// [FogBench - FogModule_FillPolygon]
// ============================================================================

FogModule_FillPolygon::FogModule_FillPolygon(int w, int h) : FogModule(w, h) {}
FogModule_FillPolygon::~FogModule_FillPolygon() {}

void FogModule_FillPolygon::prepare(int quantity, int sw, int sh)
{
  r_poly.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void FogModule_FillPolygon::finish()
{
  r_poly.free();
  r_argb.free();
}

void FogModule_FillPolygon::bench(int quantity)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p);

  p.setFillRule(Fog::FILL_EVEN_ODD);
  Fog::PathD path;

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++)
    {
      const Fog::PointD* polyData = &r_poly.data[a * 10];

      path.clear();
      path.addPolygon(polyData, 10);

      p.setSource(r_argb.data[a]);
      p.fillPath(path);
    }
  }
  else
  {
    Fog::Pattern pattern;
    pattern.setType(Fog::PATTERN_LINEAR_GRADIENT);
    pattern.setSpread(Fog::PATTERN_SPREAD_PAD);

    for (int a = 0; a < quantity; a++)
    {
      const Fog::PointD* polyData = &r_poly.data[a * 10];

      path.clear();
      path.addPolygon(polyData, 10);

      setupFogPatternForPoint(pattern, polyData[0], polyData[9], r_argb.data[a]);

      p.setSource(pattern);
      p.fillPath(path);
      p.resetSource();
    }
  }
}

Fog::String FogModule_FillPolygon::getType()
{
  return Fog::Ascii8("FillPolygon");
}

// ============================================================================
// [FogBench - FogModule_Image]
// ============================================================================

FogModule_Image::FogModule_Image(int w, int h) : FogModule(w, h) {}
FogModule_Image::~FogModule_Image() {}

void FogModule_Image::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_numb.init(quantity, 0, NUM_SPRITES - 1);

  for (int a = 0; a < 4; a++) images[a] = sprite[a].scaled(Fog::SizeI(sw, sh));
}

void FogModule_Image::finish()
{
  r_rect.free();
  r_numb.free();
}

void FogModule_Image::bench(int quantity)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p);

  for (int a = 0; a < quantity; a++)
  {
    p.drawImage(r_rect.data[a].getPosition(), images[r_numb.data[a]]);
  }
}

Fog::String FogModule_Image::getType()
{
  return Fog::Ascii8("Image");
}

// ============================================================================
// [FogBench - FogModule_ImageAffine]
// ============================================================================

FogModule_ImageAffine::FogModule_ImageAffine(int w, int h) : FogModule_Image(w, h) {}
FogModule_ImageAffine::~FogModule_ImageAffine() {}

void FogModule_ImageAffine::bench(int quantity)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p);

  double cx = (double)w / 2.0;
  double cy = (double)h / 2.0;
  double rot = 0.0;

  for (int a = 0; a < quantity; a++, rot += 0.01)
  {
    Fog::TransformD t;
    t.translate(cx, cy);
    t.rotate(rot);
    t.translate(-cx, -cy);

    p.setTransform(t);
    p.drawImage(r_rect.data[a].getPosition(), images[r_numb.data[a]]);
  }
}

Fog::String FogModule_ImageAffine::getType()
{
  return Fog::Ascii8("ImageAffine");
}

// ============================================================================
// [FogBench - FogModule_RasterText]
// ============================================================================

FogModule_RasterText::FogModule_RasterText(int w, int h) : FogModule(w, h) {}
FogModule_RasterText::~FogModule_RasterText() {}

void FogModule_RasterText::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void FogModule_RasterText::finish()
{
  r_rect.free();
  r_argb.free();
}

void FogModule_RasterText::bench(int quantity)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p);

  Fog::String text(Fog::Ascii8("abcdef"));
  Fog::Font font;

  for (int a = 0; a < quantity; a++)
  {
    p.setSource(r_argb.data[a]);
    p.drawText(r_rect.data[a].getPosition(), text, font);
  }
}

Fog::String FogModule_RasterText::getType()
{
  return Fog::Ascii8("RasterText");
}

// ============================================================================
// [FogBench - FogBenchmarkContext]
// ============================================================================

FogBenchmarkContext::FogBenchmarkContext(BenchmarkMaster* master, const Fog::String& name) :
  AbstractBenchmarkContext(master, name)
{
}

FogBenchmarkContext::~FogBenchmarkContext()
{
}

#define DO_BENCH(__Module__, __op__, __source__, __w__, __h__) \
  { \
    __Module__ mod(_master->getWidth(), _master->getHeight()); \
    mod.setEngine(engine); \
    bench(mod, __op__, __source__, __w__, __h__, _master->getQuantity()); \
  }

void FogBenchmarkContext::run()
{
  header();

  const Fog::SizeI* sizes = _master->getSizes().getData();
  sysuint_t s;

  int engine = (getName() == Fog::Ascii8("Fog-st"))
    ? Fog::PAINT_ENGINE_RASTER_ST
    : Fog::PAINT_ENGINE_RASTER_MT;

  // Create-Destroy
  DO_BENCH(FogModule_CreateDestroy, BENCH_OPERATOR_NONE, BENCH_SOURCE_NONE, 0, 0)

  for (s = 0; s < _master->getSizes().getLength(); s++)
  {
    DO_BENCH(FogModule_FillRect      , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillRect      , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillRect      , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillRect      , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(FogModule_FillRectSubPX , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillRectSubPX , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillRectSubPX , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillRectSubPX , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(FogModule_FillRectAffine, BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillRectAffine, BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillRectAffine, BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillRectAffine, BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(FogModule_FillRound     , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillRound     , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillRound     , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillRound     , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(FogModule_FillPolygon   , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillPolygon   , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillPolygon   , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_FillPolygon   , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(FogModule_Image         , BENCH_OPERATOR_SRC     , BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_Image         , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)

    DO_BENCH(FogModule_ImageAffine   , BENCH_OPERATOR_SRC     , BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)
    DO_BENCH(FogModule_ImageAffine   , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)
  }

  _master->_contexts.append(this);
  footer();
}
