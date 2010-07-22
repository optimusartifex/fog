#include "BenchGdiPlus.h"

#if defined(FOG_BENCH_GDIPLUS)

// ============================================================================
// [FogBench - Helpers]
// ============================================================================

static Gdiplus::Brush* setupGdiPlusPatternForPoint(
  const Fog::DoublePoint& pt0, const Fog::DoublePoint& pt1, const Fog::Argb& argb)
{
  Gdiplus::Color clr[3];
  Gdiplus::REAL stops[3];

  stops[0] = 0.0; clr[0].SetValue(0xFFFFFFFF);
  stops[1] = 0.5; clr[1].SetValue(0x7FFF0000);
  stops[2] = 1.0; clr[2].SetValue(argb.getValue());

  Gdiplus::LinearGradientBrush* brush = fog_new Gdiplus::LinearGradientBrush(
    Gdiplus::PointF((Gdiplus::REAL)pt0.x, (Gdiplus::REAL)pt0.y),
    Gdiplus::PointF((Gdiplus::REAL)pt1.x, (Gdiplus::REAL)pt1.y),
    Gdiplus::Color(0xFFFFFFFF), Gdiplus::Color(0xFF000000));
  brush->SetInterpolationColors(clr, stops, 3);

  return brush;
}

static Gdiplus::Brush* setupGdiPlusPatternForRect(
  const Fog::IntRect& rect, const Fog::Argb& argb)
{
  return setupGdiPlusPatternForPoint(
    Fog::DoublePoint(rect.x, rect.y),
    Fog::DoublePoint(rect.x+rect.w, rect.y+rect.h),
    argb);
}

// ============================================================================
// [FogBench - GdiPlusModule]
// ============================================================================

GdiPlusModule::GdiPlusModule(int w, int h) : AbstractModule(w, h)
{
  screen_gdip = fog_new Gdiplus::Bitmap(
    screen.getWidth(),
    screen.getHeight(),
    screen.getStride(),
    PixelFormat32bppPARGB,
    (BYTE*)screen.getData());

  for (int a = 0; a < NUM_SPRITES; a++)
  {
    sprite_gdip[a] = fog_new Gdiplus::Bitmap(
      sprite[a].getWidth(),
      sprite[a].getHeight(),
      sprite[a].getStride(),
      PixelFormat32bppPARGB,
      (BYTE*)sprite[a].getData());
  }
}

GdiPlusModule::~GdiPlusModule()
{
  fog_delete(screen_gdip);

  for (int a = 0; a < NUM_SPRITES; a++) fog_delete(sprite_gdip[a]);
}

Fog::String GdiPlusModule::getEngine()
{
  return Fog::Ascii8("GDI+");
}

void GdiPlusModule::configureGraphics(Gdiplus::Graphics& gr)
{
  if (op == Fog::OPERATOR_SRC)
    gr.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
  else
    gr.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

  gr.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
  gr.SetInterpolationMode(Gdiplus::InterpolationModeBilinear);
}

// ============================================================================
// [FogBench - GdiPlusModule_CreateDestroy]
// ============================================================================

GdiPlusModule_CreateDestroy::GdiPlusModule_CreateDestroy(int w, int h) : GdiPlusModule(w, h) {}
GdiPlusModule_CreateDestroy::~GdiPlusModule_CreateDestroy() {}

void GdiPlusModule_CreateDestroy::bench(int quantity)
{
  for (int a = 0; a < quantity; a++)
  {
    Gdiplus::Graphics gr(screen_gdip);
    configureGraphics(gr);
  }
}

Fog::String GdiPlusModule_CreateDestroy::getType()
{
  return Fog::Ascii8("Create-Destroy");
}

// ============================================================================
// [FogBench - GdiPlusModule_FillRect]
// ============================================================================

GdiPlusModule_FillRect::GdiPlusModule_FillRect(int w, int h) : GdiPlusModule(w, h) {}
GdiPlusModule_FillRect::~GdiPlusModule_FillRect() {}

void GdiPlusModule_FillRect::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void GdiPlusModule_FillRect::finish()
{
  r_rect.free();
  r_argb.free();
}

void GdiPlusModule_FillRect::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);
  configureGraphics(gr);

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++)
    {
      Fog::IntRect r = r_rect.data[a];

      Gdiplus::Color c(r_argb.data[a]);
      Gdiplus::SolidBrush br(c);

      gr.FillRectangle((Gdiplus::Brush*)&br, r.x, r.y, r.w, r.h);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++)
    {
      Fog::IntRect r = r_rect.data[a];

      Gdiplus::Brush* brush = setupGdiPlusPatternForRect(r, r_argb.data[a]);
      gr.FillRectangle(brush, r.x, r.y, r.w, r.h);
      fog_delete(brush);
    }
  }
}

Fog::String GdiPlusModule_FillRect::getType()
{
  return Fog::Ascii8("FillRect");
}

// ============================================================================
// [FogBench - GdiPlusModule_FillRectSubPX]
// ============================================================================

GdiPlusModule_FillRectSubPX::GdiPlusModule_FillRectSubPX(int w, int h) : GdiPlusModule_FillRect(w, h) {}
GdiPlusModule_FillRectSubPX::~GdiPlusModule_FillRectSubPX() {}

void GdiPlusModule_FillRectSubPX::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);
  configureGraphics(gr);

  float sub = 0.1f;
  float inc = 0.8f / (float)quantity;

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++, sub += inc)
    {
      Fog::IntRect r = r_rect.data[a];

      Gdiplus::Color c(r_argb.data[a]);
      Gdiplus::SolidBrush br(c);

      gr.FillRectangle((Gdiplus::Brush*)&br, (float)r.x + sub, (float)r.y + sub, (float)r.w, (float)r.h);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++, sub += inc)
    {
      Fog::IntRect r = r_rect.data[a];

      Gdiplus::Brush* brush = setupGdiPlusPatternForRect(r, r_argb.data[a]);
      gr.FillRectangle(brush, (float)r.x + sub, (float)r.y + sub, (float)r.w, (float)r.h);
      fog_delete(brush);
    }
  }
}

Fog::String GdiPlusModule_FillRectSubPX::getType()
{
  return Fog::Ascii8("FillRectSubPX");
}

// ============================================================================
// [FogBench - GdiPlusModule_FillRectAffine]
// ============================================================================

GdiPlusModule_FillRectAffine::GdiPlusModule_FillRectAffine(int w, int h) : GdiPlusModule_FillRect(w, h) {}
GdiPlusModule_FillRectAffine::~GdiPlusModule_FillRectAffine() {}

void GdiPlusModule_FillRectAffine::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);
  configureGraphics(gr);

  float cx = (float)w / 2.0f;
  float cy = (float)h / 2.0f;
  float rot = 0.0f;

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++, rot += 0.01f)
    {
      Fog::IntRect r = r_rect.data[a];

      gr.ResetTransform();
      gr.TranslateTransform((Gdiplus::REAL)cx, (Gdiplus::REAL)cy);
      gr.RotateTransform(Fog::Math::rad2deg(rot));
      gr.TranslateTransform((Gdiplus::REAL)-cx, (Gdiplus::REAL)-cy);

      Gdiplus::Color c(r_argb.data[a]);
      Gdiplus::SolidBrush br(c);

      gr.FillRectangle((Gdiplus::Brush*)&br, r.x, r.y, r.w, r.h);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++, rot += 0.01f)
    {
      Fog::IntRect r = r_rect.data[a];

      gr.ResetTransform();
      gr.TranslateTransform((Gdiplus::REAL)cx, (Gdiplus::REAL)cy);
      gr.RotateTransform(Fog::Math::rad2deg(rot));
      gr.TranslateTransform((Gdiplus::REAL)-cx, (Gdiplus::REAL)-cy);

      Gdiplus::Brush* brush = setupGdiPlusPatternForRect(r, r_argb.data[a]);
      gr.FillRectangle(brush, r.x, r.y, r.w, r.h);
      fog_delete(brush);
    }
  }
}

Fog::String GdiPlusModule_FillRectAffine::getType()
{
  return Fog::Ascii8("FillRectAffine");
}

// ============================================================================
// [FogBench - GdiPlusModule_FillRound]
// ============================================================================

GdiPlusModule_FillRound::GdiPlusModule_FillRound(int w, int h) : GdiPlusModule_FillRect(w, h) {}
GdiPlusModule_FillRound::~GdiPlusModule_FillRound() {}

void GdiPlusModule_FillRound::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);
  configureGraphics(gr);

  for (int a = 0; a < quantity; a++)
  {
    Fog::IntRect r = r_rect.data[a];

    Gdiplus::REAL r_x = (Gdiplus::REAL)r.x - (Gdiplus::REAL)0.5;
    Gdiplus::REAL r_y = (Gdiplus::REAL)r.y - (Gdiplus::REAL)0.5;
    Gdiplus::REAL r_w = (Gdiplus::REAL)r.w;
    Gdiplus::REAL r_h = (Gdiplus::REAL)r.h;
    Gdiplus::REAL d = 8;

    Gdiplus::GraphicsPath path;
    path.AddArc(r_x, r_y, d, d, 180.0f, 90.0f);
    path.AddArc(r_x + r_w - d, r_y, d, d, 270.0f, 90.0f);
    path.AddArc(r_x + r_w - d, r_y + r_h - d, d, d, 0.0f, 90.0f);
    path.AddArc(r_x, r_y + r_h - d, d, d, 90.0f, 90.0f);
    path.AddLine(r_x, r_y + r_h - d, r_x, r_y + d/2);

    if (source == BENCH_SOURCE_ARGB)
    {
      Gdiplus::Color c(r_argb.data[a]);
      Gdiplus::SolidBrush br(c);
      gr.FillPath((Gdiplus::Brush*)&br, &path);
    }
    else
    {
      Gdiplus::Brush* brush = setupGdiPlusPatternForRect(r, r_argb.data[a]);
      gr.FillPath(brush, &path);
      fog_delete(brush);
    }
  }
}

Fog::String GdiPlusModule_FillRound::getType()
{
  return Fog::Ascii8("FillRound");
}

// ============================================================================
// [FogBench - GdiPlusModule_FillPolygon]
// ============================================================================

GdiPlusModule_FillPolygon::GdiPlusModule_FillPolygon(int w, int h) : GdiPlusModule(w, h) {}
GdiPlusModule_FillPolygon::~GdiPlusModule_FillPolygon() {}

void GdiPlusModule_FillPolygon::prepare(int quantity, int sw, int sh)
{
  r_poly.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void GdiPlusModule_FillPolygon::finish()
{
  r_poly.free();
  r_argb.free();
}

void GdiPlusModule_FillPolygon::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);
  configureGraphics(gr);

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++)
    {
      const Fog::DoublePoint* polyData = &r_poly.data[a*10];

      Gdiplus::GraphicsPath path;
      Gdiplus::PointF lines[10];
      for (int i = 0; i < 10; i++)
      {
        lines[i].X = (Gdiplus::REAL)polyData[i].x - (Gdiplus::REAL)0.5;
        lines[i].Y = (Gdiplus::REAL)polyData[i].y - (Gdiplus::REAL)0.5;
      }
      path.AddLines(lines, 10);
      path.CloseFigure();

      Gdiplus::Color c(r_argb.data[a]);
      Gdiplus::SolidBrush br(c);

      gr.FillPath((Gdiplus::Brush*)&br, &path);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++)
    {
      const Fog::DoublePoint* polyData = &r_poly.data[a*10];

      Gdiplus::GraphicsPath path;
      Gdiplus::PointF lines[10];
      for (int i = 0; i < 10; i++)
      {
        lines[i].X = (Gdiplus::REAL)polyData[i].x - (Gdiplus::REAL)0.5;
        lines[i].Y = (Gdiplus::REAL)polyData[i].y - (Gdiplus::REAL)0.5;
      }
      path.AddLines(lines, 10);
      path.CloseFigure();

      Gdiplus::Brush* brush = setupGdiPlusPatternForPoint(polyData[0], polyData[9], r_argb.data[a]);
      gr.FillPath(brush, &path);
      fog_delete(brush);
    }
  }
}

Fog::String GdiPlusModule_FillPolygon::getType()
{
  return Fog::Ascii8("FillPolygon");
}

// ============================================================================
// [FogBench - GdiPlusModule_Image]
// ============================================================================

GdiPlusModule_Image::GdiPlusModule_Image(int w, int h) : GdiPlusModule(w, h) {}
GdiPlusModule_Image::~GdiPlusModule_Image() {}

void GdiPlusModule_Image::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_numb.init(quantity, 0, NUM_SPRITES - 1);

  for (int a = 0; a < 4; a++)
  {
    images[a] = sprite[a].scaled(Fog::IntSize(sw, sh));
    images_gdip[a] = fog_new Gdiplus::Bitmap(
      images[a].getWidth(),
      images[a].getHeight(),
      images[a].getStride(),
      PixelFormat32bppPARGB,
      (BYTE*)images[a].getData());
  }
}

void GdiPlusModule_Image::finish()
{
  r_rect.free();
  r_numb.free();

  for (int a = 0; a < 4; a++)
  {
    fog_delete(images_gdip[a]);
  }
}

void GdiPlusModule_Image::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);
  configureGraphics(gr);

  for (int a = 0; a < quantity; a++)
  {
    int x = r_rect.data[a].x;
    int y = r_rect.data[a].y;
    gr.DrawImage(images_gdip[r_numb.data[a]], x, y);
  }
}

Fog::String GdiPlusModule_Image::getType()
{
  return Fog::Ascii8("Image");
}

// ============================================================================
// [FogBench - GdiPlusModule_ImageAffine]
// ============================================================================

GdiPlusModule_ImageAffine::GdiPlusModule_ImageAffine(int w, int h) : GdiPlusModule_Image(w, h) {}
GdiPlusModule_ImageAffine::~GdiPlusModule_ImageAffine() {}

void GdiPlusModule_ImageAffine::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);
  configureGraphics(gr);

  float cx = (float)w / 2.0f;
  float cy = (float)h / 2.0f;
  float rot = 0.0f;

  for (int a = 0; a < quantity; a++, rot += 0.01f)
  {
    int x = r_rect.data[a].x;
    int y = r_rect.data[a].y;

    gr.ResetTransform();
    gr.TranslateTransform((Gdiplus::REAL)cx, (Gdiplus::REAL)cy);
    gr.RotateTransform(Fog::Math::rad2deg(rot));
    gr.TranslateTransform((Gdiplus::REAL)-cx, (Gdiplus::REAL)-cy);

    gr.DrawImage(images_gdip[r_numb.data[a]], x, y);
  }
}

Fog::String GdiPlusModule_ImageAffine::getType()
{
  return Fog::Ascii8("ImageAffine");
}

// ============================================================================
// [FogBench - GdiPlusBenchmarkContext]
// ============================================================================

GdiPlusBenchmarkContext::GdiPlusBenchmarkContext(BenchmarkMaster* master) :
  AbstractBenchmarkContext(master, Fog::Ascii8("Gdi+"))
{
}

GdiPlusBenchmarkContext::~GdiPlusBenchmarkContext()
{
}

#define DO_BENCH(__Module__, __op__, __source__, __w__, __h__) \
  { \
    __Module__ mod(_master->getWidth(), _master->getHeight()); \
    bench(mod, __op__, __source__, __w__, __h__, _master->getQuantity()); \
  }

void GdiPlusBenchmarkContext::run()
{
  header();

  const Fog::IntSize* sizes = _master->getSizes().getData();
  int quantity = _master->_quantity;
  sysuint_t s;

  // GdiPlus - CreateDestroy
  DO_BENCH(GdiPlusModule_CreateDestroy, BENCH_OPERATOR_NONE, BENCH_SOURCE_NONE, 0, 0)

  for (s = 0; s < _master->getSizes().getLength(); s++)
  {
    DO_BENCH(GdiPlusModule_FillRect      , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillRect      , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillRect      , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillRect      , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(GdiPlusModule_FillRectSubPX , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillRectSubPX , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillRectSubPX , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillRectSubPX , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(GdiPlusModule_FillRectAffine, BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillRectAffine, BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillRectAffine, BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillRectAffine, BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(GdiPlusModule_FillRound     , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillRound     , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillRound     , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillRound     , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(GdiPlusModule_FillPolygon   , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillPolygon   , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillPolygon   , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_FillPolygon   , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(GdiPlusModule_Image         , BENCH_OPERATOR_SRC     , BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_Image         , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)

    DO_BENCH(GdiPlusModule_ImageAffine   , BENCH_OPERATOR_SRC     , BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)
    DO_BENCH(GdiPlusModule_ImageAffine   , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)
  }

  _master->_contexts.append(this);
  footer();
}

#endif // FOG_BENCH_GDIPLUS
