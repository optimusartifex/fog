// [Fog-Bench]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include "BenchFog.h"

// ============================================================================
// [BenchFog - Construction / Destruction]
// ============================================================================

BenchFog::BenchFog(BenchApp& app) :
  BenchModule(app),
  mt(false)
{
}

BenchFog::~BenchFog()
{
}

// ============================================================================
// [BenchFog - Methods]
// ============================================================================

Fog::StringW BenchFog::getModuleName() const
{
  Fog::StringW module;
  module.format("Fog (%s-%s)",
    mt ? "mt" : "st",
    _fog_build_info()->isReleaseVersion() ? "rel" : "dbg");
  return module;
}

Fog::List<uint32_t> BenchFog::getSupportedPixelFormats() const
{
  Fog::List<uint32_t> list;
  list.append(Fog::IMAGE_FORMAT_PRGB32);
  list.append(Fog::IMAGE_FORMAT_XRGB32);
  list.append(Fog::IMAGE_FORMAT_RGB24);
  return list;
}

void BenchFog::bench(BenchOutput& output, const BenchParams& params)
{
  if (screen.create(params.screenSize, params.format) != Fog::ERR_OK)
    return;

  switch (params.type)
  {
    case BENCH_TYPE_BLIT_IMAGE_I:
    case BENCH_TYPE_BLIT_IMAGE_F:
    case BENCH_TYPE_BLIT_IMAGE_ROTATE:
      prepareSprites(params.shapeSize);
      break;
  }

  screen.clear(Fog::Color(Fog::Argb32(0x00000000)));
  Fog::Time start(Fog::Time::now());

  switch (params.type)
  {
    case BENCH_TYPE_CREATE_DESTROY:
      runCreateDestroy(output, params);
      break;

    case BENCH_TYPE_FILL_RECT_I:
      runFillRectI(output, params);
      break;

    case BENCH_TYPE_FILL_RECT_F:
      runFillRectF(output, params);
      break;

    case BENCH_TYPE_FILL_RECT_ROTATE:
      runFillRectRotate(output, params);
      break;

    case BENCH_TYPE_FILL_ROUND:
      runFillRound(output, params);
      break;

    case BENCH_TYPE_FILL_POLYGON:
      runFillPolygon(output, params, 10);
      break;

    case BENCH_TYPE_FILL_COMPLEX:
      runFillPolygon(output, params, 100);
      break;

    case BENCH_TYPE_BLIT_IMAGE_I:
      runBlitImageI(output, params);
      break;

    case BENCH_TYPE_BLIT_IMAGE_F:
      runBlitImageF(output, params);
      break;

    case BENCH_TYPE_BLIT_IMAGE_ROTATE:
      runBlitImageRotate(output, params);
      break;
  }

  output.time = Fog::Time::now() - start;

  freeSprites();
}

// ============================================================================
// [BenchFog - Fog-Specific]
// ============================================================================

void BenchFog::configurePainter(Fog::Painter& p, const BenchParams& params)
{
  p.setCompositingOperator(params.op);
  p.setGradientQuality(Fog::GRADIENT_QUALITY_NORMAL);
}

void BenchFog::configureGradient(Fog::LinearGradientF& gradient,
  const Fog::RectI& r, const Fog::Argb32& c0, const Fog::Argb32& c1, const Fog::Argb32& c2)
{
  gradient.setStart(Fog::PointF(r.x, r.y));
  gradient.setEnd(Fog::PointF(r.x + r.w, r.y + r.h));

  gradient.clearStops();
  gradient.addStop(0.0f, c0);
  gradient.addStop(0.5f, c1);
  gradient.addStop(1.0f, c2);
}

void BenchFog::configureGradient(Fog::LinearGradientF& gradient,
  const Fog::RectF& r, const Fog::Argb32& c0, const Fog::Argb32& c1, const Fog::Argb32& c2)
{
  gradient.setStart(Fog::PointF(r.x, r.y));
  gradient.setEnd(Fog::PointF(r.x + r.w, r.y + r.h));

  gradient.clearStops();
  gradient.addStop(0.0f, c0);
  gradient.addStop(0.5f, c1);
  gradient.addStop(1.0f, c2);
}

void BenchFog::runCreateDestroy(BenchOutput& output, const BenchParams& params)
{
  uint32_t i, quantity = params.quantity;
  for (i = 0; i < quantity; i++)
  {
    Fog::Painter p(screen, Fog::NO_FLAGS);
    configurePainter(p, params);
  }
}

void BenchFog::runFillRectI(BenchOutput& output, const BenchParams& params)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p, params);

  BenchRandom rRect(app);
  BenchRandom rArgb(app);

  if (params.source == BENCH_SOURCE_SOLID)
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectI r(rRect.getRectI(params.screenSize, params.shapeSize, params.shapeSize));
      Fog::Argb32 c0(rArgb.getArgb32());

      p.setSource(c0);
      p.fillRect(r);
    }
  }
  else
  {
    Fog::LinearGradientF gradient;
    gradient.setGradientSpread(Fog::GRADIENT_SPREAD_PAD);

    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectI r(rRect.getRectI(params.screenSize, params.shapeSize, params.shapeSize));
      Fog::Argb32 c0(rArgb.getArgb32());
      Fog::Argb32 c1(rArgb.getArgb32());
      Fog::Argb32 c2(rArgb.getArgb32());

      configureGradient(gradient, r, c0, c1, c2);
      p.setSource(gradient);
      p.fillRect(r);
      p.setSource();
    }
  }
}

void BenchFog::runFillRectF(BenchOutput& output, const BenchParams& params)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p, params);

  BenchRandom rRect(app);
  BenchRandom rArgb(app);

  if (params.source == BENCH_SOURCE_SOLID)
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));
      Fog::Argb32 c0(rArgb.getArgb32());

      p.setSource(c0);
      p.fillRect(r);
    }
  }
  else
  {
    Fog::LinearGradientF gradient;
    gradient.setGradientSpread(Fog::GRADIENT_SPREAD_PAD);

    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));
      Fog::Argb32 c0(rArgb.getArgb32());
      Fog::Argb32 c1(rArgb.getArgb32());
      Fog::Argb32 c2(rArgb.getArgb32());

      configureGradient(gradient, r, c0, c1, c2);
      p.setSource(gradient);
      p.fillRect(r);
      p.setSource();
    }
  }
}

void BenchFog::runFillRectRotate(BenchOutput& output, const BenchParams& params)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p, params);

  BenchRandom rRect(app);
  BenchRandom rArgb(app);

  float cx = (float)params.screenSize.w * 0.5f;
  float cy = (float)params.screenSize.h * 0.5f;
  float angle = 0.0f;

  if (params.source == BENCH_SOURCE_SOLID)
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++, angle += 0.01f)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));
      Fog::Argb32 c0(rArgb.getArgb32());

      Fog::TransformF transform;
      transform.translate(Fog::PointF(cx, cy));
      transform.rotate(angle);
      transform.translate(Fog::PointF(-cx, -cy));

      p.setTransform(transform);
      p.setSource(c0);
      p.fillRect(r);
    }
  }
  else
  {
    Fog::LinearGradientF gradient;
    gradient.setGradientSpread(Fog::GRADIENT_SPREAD_PAD);

    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++, angle += 0.01f)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));
      Fog::Argb32 c0(rArgb.getArgb32());
      Fog::Argb32 c1(rArgb.getArgb32());
      Fog::Argb32 c2(rArgb.getArgb32());

      Fog::TransformF transform;
      transform.translate(Fog::PointF(cx, cy));
      transform.rotate(angle);
      transform.translate(Fog::PointF(-cx, -cy));

      configureGradient(gradient, r, c0, c1, c2);
      p.setTransform(transform);
      p.setSource(gradient);
      p.fillRect(r);
      p.setSource();
    }
  }
}

void BenchFog::runFillRound(BenchOutput& output, const BenchParams& params)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p, params);

  BenchRandom rRect(app);
  BenchRandom rArgb(app);
  BenchRandom rRadius(app);

  if (params.source == BENCH_SOURCE_SOLID)
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));
      float rad = rRadius.getFloat(4.0f, 40.0f);

      Fog::Argb32 c0(rArgb.getArgb32());

      p.setSource(c0);
      p.fillRound(Fog::RoundF(r, rad));
    }
  }
  else
  {
    Fog::LinearGradientF gradient;
    gradient.setGradientSpread(Fog::GRADIENT_SPREAD_PAD);

    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));
      float rad = rRadius.getFloat(4.0f, 40.0f);

      Fog::Argb32 c0(rArgb.getArgb32());
      Fog::Argb32 c1(rArgb.getArgb32());
      Fog::Argb32 c2(rArgb.getArgb32());

      configureGradient(gradient, r, c0, c1, c2);
      p.setSource(gradient);
      p.fillRound(Fog::RoundF(r, rad));
      p.setSource();
    }
  }
}

void BenchFog::runFillPolygon(BenchOutput& output, const BenchParams& params, uint32_t complexity)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p, params);

  BenchRandom rPts(app);
  BenchRandom rArgb(app);

  Fog::SizeI polyScreen(
    params.screenSize.w - params.shapeSize,
    params.screenSize.h - params.shapeSize);
  float polySize = (float)params.shapeSize;

  Fog::PointF points[128];
  FOG_ASSERT(complexity < FOG_ARRAY_SIZE(points));

  if (params.source == BENCH_SOURCE_SOLID)
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::PointF base(rPts.getPointF(polyScreen));

      for (uint32_t j = 0; j < complexity; j++)
      {
        float x = rPts.getFloat(base.x, base.x + polySize);
        float y = rPts.getFloat(base.y, base.y + polySize);
        points[j].set(x, y);
      }
      Fog::Argb32 c0(rArgb.getArgb32());

      p.setSource(c0);
      p.fillPolygon(points, complexity);
    }
  }
  else
  {
    Fog::LinearGradientF gradient;
    gradient.setGradientSpread(Fog::GRADIENT_SPREAD_PAD);

    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::PointF base(rPts.getPointF(polyScreen));

      for (uint32_t j = 0; j < complexity; j++)
      {
        float x = rPts.getFloat(base.x, base.x + polySize);
        float y = rPts.getFloat(base.y, base.y + polySize);
        points[j].set(x, y);
      }

      Fog::Argb32 c0(rArgb.getArgb32());
      Fog::Argb32 c1(rArgb.getArgb32());
      Fog::Argb32 c2(rArgb.getArgb32());

      configureGradient(gradient, Fog::RectF(base.x, base.y, polySize, polySize), c0, c1, c2);
      p.setSource(gradient);
      p.fillPolygon(points, complexity);
      p.setSource();
    }
  }
}

void BenchFog::runBlitImageI(BenchOutput& output, const BenchParams& params)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p, params);

  BenchRandom rPts(app);

  Fog::SizeI screenSize(
    params.screenSize.w - params.shapeSize,
    params.screenSize.h - params.shapeSize);

  uint32_t spriteIndex = 0;
  uint32_t spritesLength = (uint32_t)sprites.getLength();

  uint32_t i, quantity = params.quantity;
  for (i = 0; i < quantity; i++)
  {
    Fog::PointI pt(rPts.getPointI(screenSize));
    p.blitImage(pt, sprites[spriteIndex]);

    if (++spriteIndex >= spritesLength)
      spriteIndex = 0;
  }
}

void BenchFog::runBlitImageF(BenchOutput& output, const BenchParams& params)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p, params);

  BenchRandom rPts(app);

  Fog::SizeI screenSize(
    params.screenSize.w - params.shapeSize,
    params.screenSize.h - params.shapeSize);

  uint32_t spriteIndex = 0;
  uint32_t spritesLength = (uint32_t)sprites.getLength();

  uint32_t i, quantity = params.quantity;
  for (i = 0; i < quantity; i++)
  {
    Fog::PointF pt(rPts.getPointF(screenSize));
    p.blitImage(pt, sprites[spriteIndex]);

    if (++spriteIndex >= spritesLength)
      spriteIndex = 0;
  }
}
void BenchFog::runBlitImageRotate(BenchOutput& output, const BenchParams& params)
{
  Fog::Painter p(screen, Fog::NO_FLAGS);
  configurePainter(p, params);

  float cx = (float)params.screenSize.w * 0.5f;
  float cy = (float)params.screenSize.h * 0.5f;
  float angle = 0.0f;

  BenchRandom rPts(app);

  Fog::SizeI screenSize(
    params.screenSize.w - params.shapeSize,
    params.screenSize.h - params.shapeSize);

  uint32_t spriteIndex = 0;
  uint32_t spritesLength = (uint32_t)sprites.getLength();

  uint32_t i, quantity = params.quantity;
  for (i = 0; i < quantity; i++, angle += 0.01f)
  {
    Fog::TransformF transform;

    transform.translate(Fog::PointF(cx, cy));
    transform.rotate(angle);
    transform.translate(Fog::PointF(-cx, -cy));

    Fog::PointI pt(rPts.getPointI(screenSize));

    p.setTransform(transform);
    p.blitImage(pt, sprites[spriteIndex]);

    if (++spriteIndex >= spritesLength)
      spriteIndex = 0;
  }
}
