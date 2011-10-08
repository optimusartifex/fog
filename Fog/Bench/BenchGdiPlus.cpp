// [Fog-Bench]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#include "BenchApp.h"
#if defined(FOG_BENCH_GDIPLUS)

// [Dependencies]
#include "BenchGdiPlus.h"

#if defined(FOG_CC_MSC)
#pragma comment(lib, "gdiplus.lib")
#endif // FOG_CC_MSC

// ============================================================================
// [BenchGdiPlus - Construction / Destruction]
// ============================================================================

BenchGdiPlus::BenchGdiPlus(BenchApp& app) :
  BenchModule(app)
{
  // Initialize GDI+.
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

  // Cache the gradient stop positions. We use only these in our tests.
  gradientStopCache[0] = 0.0f;
  gradientStopCache[1] = 0.5f;
  gradientStopCache[2] = 1.0f;
}

BenchGdiPlus::~BenchGdiPlus()
{
  // Shutdown GDI+.
  Gdiplus::GdiplusShutdown(gdiplusToken);
}

// ============================================================================
// [BenchGdiPlus - Methods]
// ============================================================================

Fog::StringW BenchGdiPlus::getModuleName() const
{
  return Fog::StringW::fromAscii8("GdiPlus");
}

void BenchGdiPlus::bench(BenchOutput& output, const BenchParams& params)
{
  if (screen.create(params.screenSize, Fog::IMAGE_FORMAT_PRGB32) != Fog::ERR_OK)
    return;
  screen.clear(Fog::Color(Fog::Argb32(0x00000000)));

  screenGdi = new Gdiplus::Bitmap(screen.getWidth(), screen.getHeight(), (INT)screen.getStride(), PixelFormat32bppPARGB, (BYTE*)screen.getFirstX());
  if (screenGdi == NULL)
    return;

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
      runFillPolygon(output, params);
      break;

    case BENCH_TYPE_BLIT_IMAGE_I:
      runBlitImageI(output, params);
      break;

    case BENCH_TYPE_BLIT_IMAGE_ROTATE:
      runBlitImageRotate(output, params);
      break;
  }

  output.time = Fog::Time::now() - start;

  delete screenGdi;
  screenGdi = NULL;
}

// ============================================================================
// [BenchGdiPlus - GdiPlus-Specific]
// ============================================================================

void BenchGdiPlus::configureGraphics(Gdiplus::Graphics& gr, const BenchParams& params)
{
  if (params.op == BENCH_OPERATOR_SRC)
    gr.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
  else
    gr.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

  gr.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
  gr.SetInterpolationMode(Gdiplus::InterpolationModeBilinear);
  gr.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
}

void BenchGdiPlus::addRound(Gdiplus::GraphicsPath& p, const Fog::RectF& r, float radius)
{
  float rw2 = r.w * float(0.5);
  float rh2 = r.h * float(0.5);

  float rx = Fog::Math::min<float>(Fog::Math::abs(radius), rw2);
  float ry = Fog::Math::min<float>(Fog::Math::abs(radius), rh2);

  float rxKappaInv = rx * float(Fog::MATH_1_MINUS_KAPPA);
  float ryKappaInv = ry * float(Fog::MATH_1_MINUS_KAPPA);

  float x0 = r.x;
  float y0 = r.y;
  float x1 = r.x + r.w;
  float y1 = r.y + r.h;

  p.AddLine  (x0 + rx        , y0             ,
              x1 - rx        , y0             );
  p.AddBezier(x1 - rx        , y0             ,
              x1 - rxKappaInv, y0             ,
              x1             , y0 + ryKappaInv,
              x1             , y0 + ry        );

  p.AddLine  (x1             , y0 + ry        ,
              x1             , y1 - ry        );
  p.AddBezier(x1             , y1 - ry        ,
              x1             , y1 - ryKappaInv,
              x1 - rxKappaInv, y1             ,
              x1 - rx        , y1             );

  p.AddLine  (x1 - rx        , y1             ,
              x0 + rx        , y1             );
  p.AddBezier(x0 + rx        , y1             ,
              x0 + rxKappaInv, y1             ,
              x0             , y1 - ryKappaInv,
              x0             , y1 - ry        );

  p.AddLine  (x0             , y1 - ry        ,
              x0             , y0 + ry        );
  p.AddBezier(x0             , y0 + ry        ,
              x0             , y0 + ryKappaInv,
              x0 + rxKappaInv, y0             ,
              x0 + rx        , y0             );

  p.CloseFigure();
}

void BenchGdiPlus::runCreateDestroy(BenchOutput& output, const BenchParams& params)
{
  uint32_t i, quantity = params.quantity;
  for (i = 0; i < quantity; i++)
  {
    Gdiplus::Graphics gr(screenGdi);
    configureGraphics(gr, params);
  }
}

void BenchGdiPlus::runFillRectI(BenchOutput& output, const BenchParams& params)
{
  Gdiplus::Graphics gr(screenGdi);
  configureGraphics(gr, params);

  BenchRandom rRect(app);
  BenchRandom rArgb(app);

  if (params.source == BENCH_SOURCE_SOLID)
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectI r(rRect.getRectI(params.screenSize, params.shapeSize, params.shapeSize));

      Gdiplus::Color color(rArgb.getArgb32().getPacked32());
      Gdiplus::SolidBrush brush(color);

      gr.FillRectangle((Gdiplus::Brush*)&brush, r.x, r.y, r.w, r.h);
    }
  }
  else
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectI r(rRect.getRectI(params.screenSize, params.shapeSize, params.shapeSize));

      Gdiplus::Color colors[3];
      colors[0] = Gdiplus::Color(rArgb.getArgb32().getPacked32());
      colors[1] = Gdiplus::Color(rArgb.getArgb32().getPacked32());
      colors[2] = Gdiplus::Color(rArgb.getArgb32().getPacked32());

      Gdiplus::LinearGradientBrush brush(
        Gdiplus::Point(r.x, r.y),
        Gdiplus::Point(r.x + r.w, r.y + r.h),
        colors[0],
        colors[2]);
      brush.SetInterpolationColors(colors, gradientStopCache, 3);

      gr.FillRectangle((Gdiplus::Brush*)&brush, r.x, r.y, r.w, r.h);
    }
  }
}

void BenchGdiPlus::runFillRectF(BenchOutput& output, const BenchParams& params)
{
  Gdiplus::Graphics gr(screenGdi);
  configureGraphics(gr, params);

  BenchRandom rRect(app);
  BenchRandom rArgb(app);

  if (params.source == BENCH_SOURCE_SOLID)
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));

      Gdiplus::Color color(rArgb.getArgb32().getPacked32());
      Gdiplus::SolidBrush brush(color);

      gr.FillRectangle((Gdiplus::Brush*)&brush, r.x, r.y, r.w, r.h);
    }
  }
  else
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));

      Gdiplus::Color colors[3];
      colors[0] = Gdiplus::Color(rArgb.getArgb32().getPacked32());
      colors[1] = Gdiplus::Color(rArgb.getArgb32().getPacked32());
      colors[2] = Gdiplus::Color(rArgb.getArgb32().getPacked32());

      Gdiplus::LinearGradientBrush brush(
        Gdiplus::PointF(r.x, r.y),
        Gdiplus::PointF(r.x + r.w, r.y + r.h),
        colors[0],
        colors[2]);
      brush.SetInterpolationColors(colors, gradientStopCache, 3);

      gr.FillRectangle((Gdiplus::Brush*)&brush, r.x, r.y, r.w, r.h);
    }
  }
}

void BenchGdiPlus::runFillRectRotate(BenchOutput& output, const BenchParams& params)
{
  Gdiplus::Graphics gr(screenGdi);
  configureGraphics(gr, params);

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

      gr.ResetTransform();
      gr.TranslateTransform(cx, cy);
      gr.RotateTransform(Fog::Math::rad2deg(angle));
      gr.TranslateTransform(-cx, -cy);

      Gdiplus::Color color(rArgb.getArgb32().getPacked32());
      Gdiplus::SolidBrush brush(color);

      gr.FillRectangle((Gdiplus::Brush*)&brush, r.x, r.y, r.w, r.h);
    }
  }
  else
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));

      Gdiplus::Color colors[3];
      colors[0] = Gdiplus::Color(rArgb.getArgb32().getPacked32());
      colors[1] = Gdiplus::Color(rArgb.getArgb32().getPacked32());
      colors[2] = Gdiplus::Color(rArgb.getArgb32().getPacked32());

      Gdiplus::LinearGradientBrush brush(
        Gdiplus::PointF(r.x, r.y),
        Gdiplus::PointF(r.x + r.w, r.y + r.h),
        colors[0],
        colors[2]);
      brush.SetInterpolationColors(colors, gradientStopCache, 3);

      gr.TranslateTransform((Gdiplus::REAL)cx, (Gdiplus::REAL)cy);
      gr.RotateTransform(Fog::Math::rad2deg(angle));
      gr.TranslateTransform((Gdiplus::REAL)-cx, (Gdiplus::REAL)-cy);

      gr.FillRectangle((Gdiplus::Brush*)&brush, r.x, r.y, r.w, r.h);

      gr.ResetTransform();
    }
  }
}

void BenchGdiPlus::runFillRound(BenchOutput& output, const BenchParams& params)
{
  Gdiplus::Graphics gr(screenGdi);
  configureGraphics(gr, params);

  BenchRandom rRect(app);
  BenchRandom rArgb(app);
  BenchRandom rRadius(app);

  uint32_t i, quantity = params.quantity;
  for (i = 0; i < quantity; i++)
  {
    Fog::RectF r(rRect.getRectF(params.screenSize, params.shapeSize, params.shapeSize));
    float rad = rRadius.getFloat(4.0f, 40.0f);

    Gdiplus::GraphicsPath path;
    addRound(path, r, rad);

    if (params.source == BENCH_SOURCE_SOLID)
    {
      Gdiplus::Color color(rArgb.getArgb32().getPacked32());
      Gdiplus::SolidBrush brush(color);
      gr.FillPath((Gdiplus::Brush*)&brush, &path);
    }
    else
    {
      Gdiplus::Color colors[3];
      colors[0] = Gdiplus::Color(rArgb.getArgb32().getPacked32());
      colors[1] = Gdiplus::Color(rArgb.getArgb32().getPacked32());
      colors[2] = Gdiplus::Color(rArgb.getArgb32().getPacked32());
      
      Gdiplus::LinearGradientBrush brush(
        Gdiplus::PointF(r.x, r.y),
        Gdiplus::PointF(r.x + r.w, r.y + r.h),
        colors[0],
        colors[2]);
      brush.SetInterpolationColors(colors, gradientStopCache, 3);
      
      gr.FillPath((Gdiplus::Brush*)&brush, &path);
    }
  }
}

void BenchGdiPlus::runFillPolygon(BenchOutput& output, const BenchParams& params)
{
  Gdiplus::Graphics gr(screenGdi);
  configureGraphics(gr, params);

  BenchRandom rPts(app);
  BenchRandom rArgb(app);

  Fog::SizeI polyScreen(
    params.screenSize.w - params.shapeSize,
    params.screenSize.h - params.shapeSize);
  float polySize = (int)params.shapeSize;

  Gdiplus::PointF points[10];

  if (params.source == BENCH_SOURCE_SOLID)
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::PointF base(rPts.getPointF(polyScreen));

      for (uint32_t j = 0; j < FOG_ARRAY_SIZE(points); j++)
      {
        points[j].X = rPts.getFloat(base.x, base.x + polySize);
        points[j].Y = rPts.getFloat(base.y, base.y + polySize);
      }

      Gdiplus::Color color(rArgb.getArgb32().getPacked32());
      Gdiplus::SolidBrush brush(color);

      gr.FillPolygon((Gdiplus::Brush*)&brush, points, FOG_ARRAY_SIZE(points), Gdiplus::FillModeAlternate);
    }
  }
  else
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::PointF base(rPts.getPointF(polyScreen));

      for (uint32_t j = 0; j < FOG_ARRAY_SIZE(points); j++)
      {
        points[j].X = rPts.getFloat(base.x, base.x + polySize);
        points[j].Y = rPts.getFloat(base.y, base.y + polySize);
      }

      Gdiplus::Color colors[3];
      colors[0] = Gdiplus::Color(rArgb.getArgb32().getPacked32());
      colors[1] = Gdiplus::Color(rArgb.getArgb32().getPacked32());
      colors[2] = Gdiplus::Color(rArgb.getArgb32().getPacked32());

      Gdiplus::LinearGradientBrush brush(
        Gdiplus::PointF(base.x, base.y),
        Gdiplus::PointF(base.x + polySize, base.y + polySize),
        colors[0],
        colors[2]);
      brush.SetInterpolationColors(colors, gradientStopCache, 3);

      gr.FillPolygon((Gdiplus::Brush*)&brush, points, FOG_ARRAY_SIZE(points), Gdiplus::FillModeAlternate);
    }
  }
}

void BenchGdiPlus::runBlitImageI(BenchOutput& output, const BenchParams& params)
{
  Gdiplus::Graphics gr(screenGdi);
  configureGraphics(gr, params);

  BenchRandom rPts(app);

  Fog::SizeI screenSize(
    params.screenSize.w - params.shapeSize,
    params.screenSize.h - params.shapeSize);

  uint32_t spriteIndex = 0;
  uint32_t spritesLength = (uint32_t)sprites.getLength();

  uint32_t i, quantity = params.quantity;
  for (i = 0; i < quantity; i++)
  {
    // TODO: FogBench (Gdiplus).

    if (++spriteIndex >= spritesLength)
      spriteIndex = 0;
  }
}

void BenchGdiPlus::runBlitImageRotate(BenchOutput& output, const BenchParams& params)
{
  Gdiplus::Graphics gr(screenGdi);
  configureGraphics(gr, params);

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
    gr.TranslateTransform((Gdiplus::REAL)cx, (Gdiplus::REAL)cy);
    gr.RotateTransform(Fog::Math::rad2deg(angle));
    gr.TranslateTransform((Gdiplus::REAL)-cx, (Gdiplus::REAL)-cy);

    // TODO: FogBench (Gdiplus).

    gr.ResetTransform();

    if (++spriteIndex >= spritesLength)
      spriteIndex = 0;
  }
}

// [Guard]
#endif // FOG_BENCH_GDIPLUS
