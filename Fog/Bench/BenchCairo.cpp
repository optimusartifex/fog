// [Fog-Bench]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#include "BenchApp.h"
#if defined(FOG_BENCH_CAIRO)

// [Dependencies]
#include "BenchCairo.h"

// ============================================================================
// [BenchCairo - Construction / Destruction]
// ============================================================================

BenchCairo::BenchCairo(BenchApp& app) :
  BenchModule(app)
{
}

BenchCairo::~BenchCairo()
{
}

// ============================================================================
// [BenchCairo - Methods]
// ============================================================================

static const double sc = 1.0 / 255.0;

Fog::StringW BenchCairo::getModuleName() const
{
  return Fog::StringW::fromAscii8("Cairo");
}

void BenchCairo::bench(BenchOutput& output, const BenchParams& params)
{
  if (screen.create(params.screenSize, Fog::IMAGE_FORMAT_PRGB32) != Fog::ERR_OK)
    return;

  switch (params.type)
  {
    case BENCH_TYPE_BLIT_IMAGE_I:
    case BENCH_TYPE_BLIT_IMAGE_ROTATE:
      prepareSprites(params.shapeSize);
      break;
  }

  screenCairo = cairo_image_surface_create_for_data(
    (unsigned char*)screen.getFirstX(), CAIRO_FORMAT_ARGB32,
    screen.getWidth(), screen.getHeight(), (int)screen.getStride());
  if (screenCairo == NULL)
    return;

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

    case BENCH_TYPE_BLIT_IMAGE_ROTATE:
      runBlitImageRotate(output, params);
      break;
  }

  output.time = Fog::Time::now() - start;

  cairo_surface_destroy(screenCairo);
  screenCairo = NULL;

  freeSprites();
}

void BenchCairo::prepareSprites(int size)
{
  BenchModule::prepareSprites(size);

  size_t i, length = sprites.getLength();
  for (i = 0; i < length; i++)
  {
    cairo_surface_t* sprite = cairo_image_surface_create_for_data(
    (unsigned char*)sprites[i].getFirst(), CAIRO_FORMAT_ARGB32,
    sprites[i].getWidth(), sprites[i].getHeight(), (int)sprites[i].getStride());

    if (sprite == NULL)
    {
      freeSprites();
      return;
    }

    spritesCairo.append(sprite);
  }
}

void BenchCairo::freeSprites()
{
  size_t i, length = spritesCairo.getLength();

  for (i = 0; i < length; i++)
    cairo_surface_destroy(spritesCairo[i]);

  spritesCairo.clear();
  BenchModule::freeSprites();
}

// ============================================================================
// [BenchCairo - Cairo-Specific]
// ============================================================================

void BenchCairo::configureContext(cairo_t* cr, const BenchParams& params)
{
  if (params.op == BENCH_OPERATOR_SRC)
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  else
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
}

cairo_pattern_t* BenchCairo::createLinearGradient(
  double x0, double y0,
  double x1, double y1,
  const Fog::Argb32& c0, const Fog::Argb32& c1, const Fog::Argb32& c2)
{
  cairo_pattern_t* pattern = cairo_pattern_create_linear(x0, y0, x1, y1);

  if (pattern == NULL)
    return NULL;

  cairo_pattern_set_extend(pattern, CAIRO_EXTEND_PAD);

  cairo_pattern_add_color_stop_rgba(pattern, 0.0, double(c0.r) * sc, double(c0.g) * sc, double(c0.b) * sc, double(c0.a) * sc);
  cairo_pattern_add_color_stop_rgba(pattern, 0.5, double(c1.r) * sc, double(c1.g) * sc, double(c1.b) * sc, double(c1.a) * sc);
  cairo_pattern_add_color_stop_rgba(pattern, 1.0, double(c2.r) * sc, double(c2.g) * sc, double(c2.b) * sc, double(c2.a) * sc);

  return pattern;
}

void BenchCairo::addRound(cairo_t* cr, const Fog::RectF& r, float radius)
{
  double rw2 = r.w * double(0.5);
  double rh2 = r.h * double(0.5);

  double rx = Fog::Math::min<double>(Fog::Math::abs(radius), rw2);
  double ry = Fog::Math::min<double>(Fog::Math::abs(radius), rh2);

  double rxKappaInv = rx * double(Fog::MATH_1_MINUS_KAPPA);
  double ryKappaInv = ry * double(Fog::MATH_1_MINUS_KAPPA);

  double x0 = r.x;
  double y0 = r.y;
  double x1 = r.x + r.w;
  double y1 = r.y + r.h;

  cairo_move_to (cr, x0 + rx        , y0             );
  cairo_line_to (cr, x1 - rx        , y0             );
  cairo_curve_to(cr, x1 - rxKappaInv, y0             ,
                     x1             , y0 + ryKappaInv,
                     x1             , y0 + ry        );

  cairo_line_to (cr, x1             , y1 - ry        );
  cairo_curve_to(cr, x1             , y1 - ryKappaInv,
                     x1 - rxKappaInv, y1             ,
                     x1 - rx        , y1             );

  cairo_line_to (cr, x0 + rx        , y1             );
  cairo_curve_to(cr, x0 + rxKappaInv, y1             ,
                     x0             , y1 - ryKappaInv,
                     x0             , y1 - ry        );

  cairo_line_to (cr, x0             , y0 + ry        );
  cairo_curve_to(cr, x0             , y0 + ryKappaInv,
                     x0 + rxKappaInv, y0             ,
                     x0 + rx        , y0             );

  cairo_close_path(cr);
}

void BenchCairo::runCreateDestroy(BenchOutput& output, const BenchParams& params)
{
  uint32_t i, quantity = params.quantity;
  for (i = 0; i < quantity; i++)
  {
    cairo_t* cr = cairo_create(screenCairo);
    configureContext(cr, params);
    cairo_destroy(cr);
  }
}

void BenchCairo::runFillRectI(BenchOutput& output, const BenchParams& params)
{
  cairo_t* cr = cairo_create(screenCairo);
  configureContext(cr, params);

  BenchRandom rRect(app);
  BenchRandom rArgb(app);

  if (params.source == BENCH_SOURCE_SOLID)
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectI r(rRect.getRectI(params.screenSize, params.shapeSize, params.shapeSize));
      Fog::Argb32 c0(rArgb.getArgb32());

      cairo_set_source_rgba(cr, double(c0.r) * sc, double(c0.g) * sc, double(c0.b) * sc, double(c0.a) * sc);
      cairo_rectangle(cr, r.x, r.y, r.w, r.h);
      cairo_fill(cr);
    }
  }
  else
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectI r(rRect.getRectI(params.screenSize, params.shapeSize, params.shapeSize));
      Fog::Argb32 c0(rArgb.getArgb32());
      Fog::Argb32 c1(rArgb.getArgb32());
      Fog::Argb32 c2(rArgb.getArgb32());

      cairo_pattern_t* pattern = createLinearGradient(r.x, r.y, r.x + r.w, r.y + r.h, c0, c1, c2);
      cairo_set_source(cr, pattern);

      cairo_rectangle(cr, r.x, r.y, r.w, r.h);
      cairo_fill(cr);

      cairo_pattern_destroy(pattern);
    }
  }

  cairo_destroy(cr);
}

void BenchCairo::runFillRectF(BenchOutput& output, const BenchParams& params)
{
  cairo_t* cr = cairo_create(screenCairo);
  configureContext(cr, params);

  BenchRandom rRect(app);
  BenchRandom rArgb(app);

  if (params.source == BENCH_SOURCE_SOLID)
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));
      Fog::Argb32 c0(rArgb.getArgb32());

      cairo_set_source_rgba(cr, double(c0.r) * sc, double(c0.g) * sc, double(c0.b) * sc, double(c0.a) * sc);
      cairo_rectangle(cr, r.x, r.y, r.w, r.h);
      cairo_fill(cr);
    }
  }
  else
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));
      Fog::Argb32 c0(rArgb.getArgb32());
      Fog::Argb32 c1(rArgb.getArgb32());
      Fog::Argb32 c2(rArgb.getArgb32());

      cairo_pattern_t* pattern = createLinearGradient(r.x, r.y, r.x + r.w, r.y + r.h, c0, c1, c2);
      cairo_set_source(cr, pattern);

      cairo_rectangle(cr, r.x, r.y, r.w, r.h);
      cairo_fill(cr);

      cairo_pattern_destroy(pattern);
    }
  }

  cairo_destroy(cr);
}

void BenchCairo::runFillRectRotate(BenchOutput& output, const BenchParams& params)
{
  cairo_t* cr = cairo_create(screenCairo);
  configureContext(cr, params);

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

      cairo_translate(cr, cx, cy);
      cairo_rotate(cr, angle);
      cairo_translate(cr, -cx, -cy);

      cairo_set_source_rgba(cr, double(c0.r) * sc, double(c0.g) * sc, double(c0.b) * sc, double(c0.a) * sc);
      cairo_rectangle(cr, r.x, r.y, r.w, r.h);
      cairo_fill(cr);

      cairo_identity_matrix(cr);
    }
  }
  else
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++, angle += 0.01f)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));
      Fog::Argb32 c0(rArgb.getArgb32());
      Fog::Argb32 c1(rArgb.getArgb32());
      Fog::Argb32 c2(rArgb.getArgb32());

      cairo_translate(cr, cx, cy);
      cairo_rotate(cr, angle);
      cairo_translate(cr, -cx, -cy);

      cairo_pattern_t* pattern = createLinearGradient(r.x, r.y, r.x + r.w, r.y + r.h, c0, c1, c2);
      cairo_set_source(cr, pattern);

      cairo_rectangle(cr, r.x, r.y, r.w, r.h);
      cairo_fill(cr);

      cairo_identity_matrix(cr);
      cairo_pattern_destroy(pattern);
    }
  }

  cairo_destroy(cr);
}

void BenchCairo::runFillRound(BenchOutput& output, const BenchParams& params)
{
  cairo_t* cr = cairo_create(screenCairo);
  configureContext(cr, params);

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
      cairo_set_source_rgba(cr, double(c0.r) * sc, double(c0.g) * sc, double(c0.b) * sc, double(c0.a) * sc);

      addRound(cr, r, rad);
      cairo_fill(cr);
    }
  }
  else
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));
      float rad = rRadius.getFloat(4.0f, 40.0f);

      Fog::Argb32 c0(rArgb.getArgb32());
      Fog::Argb32 c1(rArgb.getArgb32());
      Fog::Argb32 c2(rArgb.getArgb32());

      cairo_pattern_t* pattern = createLinearGradient(r.x, r.y, r.x + r.w, r.y + r.h, c0, c1, c2);
      cairo_set_source(cr, pattern);

      addRound(cr, r, rad);
      cairo_fill(cr);

      cairo_pattern_destroy(pattern);
    }
  }

  cairo_destroy(cr);
}

void BenchCairo::runFillPolygon(BenchOutput& output, const BenchParams& params, uint32_t complexity)
{
  cairo_t* cr = cairo_create(screenCairo);
  configureContext(cr, params);

  BenchRandom rPts(app);
  BenchRandom rArgb(app);

  Fog::SizeI polyScreen(
    params.screenSize.w - params.shapeSize,
    params.screenSize.h - params.shapeSize);
  float polySize = (int)params.shapeSize;

  if (params.source == BENCH_SOURCE_SOLID)
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::PointF base(rPts.getPointF(polyScreen));
      Fog::Argb32 c0(rArgb.getArgb32());

      cairo_set_source_rgba(cr, double(c0.r) * sc, double(c0.g) * sc, double(c0.b) * sc, double(c0.a) * sc);

      for (uint32_t j = 0; j < complexity; j++)
      {
        float x = rPts.getFloat(base.x, base.x + polySize);
        float y = rPts.getFloat(base.y, base.y + polySize);

        if (j == 0)
          cairo_move_to(cr, x, y);
        else
          cairo_line_to(cr, x, y);

      }
      cairo_close_path(cr);
      cairo_fill(cr);
    }
  }
  else
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::PointF base(rPts.getPointF(polyScreen));
      Fog::Argb32 c0(rArgb.getArgb32());
      Fog::Argb32 c1(rArgb.getArgb32());
      Fog::Argb32 c2(rArgb.getArgb32());

      cairo_pattern_t* pattern = createLinearGradient(base.x, base.y, base.x + polySize, base.y + polySize, c0, c1, c2);
      cairo_set_source(cr, pattern);

      for (uint32_t j = 0; j < complexity; j++)
      {
        float x = rPts.getFloat(base.x, base.x + polySize);
        float y = rPts.getFloat(base.y, base.y + polySize);

        if (j == 0)
          cairo_move_to(cr, x, y);
        else
          cairo_line_to(cr, x, y);

      }
      cairo_close_path(cr);
      cairo_fill(cr);
      cairo_pattern_destroy(pattern);
    }
  }

  cairo_destroy(cr);
}

void BenchCairo::runBlitImageI(BenchOutput& output, const BenchParams& params)
{
  cairo_t* cr = cairo_create(screenCairo);
  configureContext(cr, params);

  BenchRandom rPts(app);

  Fog::SizeI screenSize(
    params.screenSize.w - params.shapeSize,
    params.screenSize.h - params.shapeSize);

  uint32_t spriteIndex = 0;
  uint32_t spritesLength = (uint32_t)sprites.getLength();

  uint32_t i, quantity = params.quantity;
  for (i = 0; i < quantity; i++)
  {
    // TODO:

    if (++spriteIndex >= spritesLength)
      spriteIndex = 0;
  }

  cairo_destroy(cr);
}

void BenchCairo::runBlitImageRotate(BenchOutput& output, const BenchParams& params)
{
  cairo_t* cr = cairo_create(screenCairo);
  configureContext(cr, params);

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
    // TODO:

    if (++spriteIndex >= spritesLength)
      spriteIndex = 0;
  }

  cairo_destroy(cr);
}

#endif // FOG_BENCH_CAIRO
