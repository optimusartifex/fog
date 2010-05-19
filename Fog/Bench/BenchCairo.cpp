#include "BenchCairo.h"

#if defined(FOG_BENCH_CAIRO)

// ============================================================================
// [FogBench - Helpers]
// ============================================================================

static cairo_pattern_t* setupCairoPatternForPoint(
  const Fog::DoublePoint& pt0, const Fog::DoublePoint& pt1, const Fog::Argb& argb)
{
  cairo_pattern_t* pattern = cairo_pattern_create_linear(pt0.x, pt0.y, pt1.x, pt1.y);

  cairo_pattern_set_extend(pattern, CAIRO_EXTEND_PAD);
  cairo_pattern_add_color_stop_rgba(pattern, 0.0, 1.0, 1.0, 1.0, 1.0);
  cairo_pattern_add_color_stop_rgba(pattern, 0.5, 1.0, 0.0, 0.0, 0.5);
  cairo_pattern_add_color_stop_rgba(pattern, 1.0,
    (double)argb.getRed  () / 255.0,
    (double)argb.getGreen() / 255.0,
    (double)argb.getBlue () / 255.0,
    (double)argb.getAlpha() / 255.0);

  return pattern;
}

static cairo_pattern_t* setupCairoPatternForRect(
  const Fog::IntRect& rect, const Fog::Argb& argb)
{
  return setupCairoPatternForPoint(
    Fog::DoublePoint(rect.x, rect.y),
    Fog::DoublePoint(rect.x + rect.w, rect.y + rect.h),
    argb);
}

// ============================================================================
// [FogBench - CairoModule]
// ============================================================================

CairoModule::CairoModule(int w, int h) : FogModule(w, h)
{
  screen_cairo = cairo_image_surface_create_for_data(
    (unsigned char*)screen.getFirst(),
    CAIRO_FORMAT_ARGB32,
    screen.getWidth(),
    screen.getHeight(),
    screen.getStride());
}

CairoModule::~CairoModule()
{
  cairo_surface_destroy(screen_cairo);
}

Fog::String CairoModule::getEngine()
{
  return Fog::Ascii8("Cairo");
}

void CairoModule::configureContext(cairo_t* cr)
{
  if (op == BENCH_OPERATOR_SRC)
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  else
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
}

// ============================================================================
// [FogBench - CairoModule_CreateDestroy]
// ============================================================================

CairoModule_CreateDestroy::CairoModule_CreateDestroy(int w, int h) : CairoModule(w, h) {}
CairoModule_CreateDestroy::~CairoModule_CreateDestroy() {}

void CairoModule_CreateDestroy::bench(int quantity)
{
  for (int a = 0; a < quantity; a++)
  {
    cairo_t* cr = cairo_create(screen_cairo);
    configureContext(cr);
    cairo_destroy(cr);
  }
}

Fog::String CairoModule_CreateDestroy::getType()
{
  return Fog::Ascii8("Create-Destroy");
}

// ============================================================================
// [FogBench - CairoModule_FillRect]
// ============================================================================

CairoModule_FillRect::CairoModule_FillRect(int w, int h) : CairoModule(w, h) {}
CairoModule_FillRect::~CairoModule_FillRect() {}

void CairoModule_FillRect::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void CairoModule_FillRect::finish()
{
  r_rect.free();
  r_argb.free();
}

void CairoModule_FillRect::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);
  configureContext(cr);

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      cairo_set_source_rgba(cr,
        (double)c.getRed  () / 255.0,
        (double)c.getGreen() / 255.0,
        (double)c.getBlue () / 255.0,
        (double)c.getAlpha() / 255.0);
      cairo_rectangle(cr, r.x, r.y, r.w, r.h);
      cairo_fill(cr);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      cairo_pattern_t* pattern = setupCairoPatternForRect(r, c);
      cairo_set_source(cr, pattern);
      cairo_rectangle(cr, r.x, r.y, r.w, r.h);
      cairo_fill(cr);
      cairo_pattern_destroy(pattern);
    }
  }

  cairo_destroy(cr);
}

Fog::String CairoModule_FillRect::getType()
{
  return Fog::Ascii8("FillRect");
}

// ============================================================================
// [FogBench - CairoModule_FillRectSubPX]
// ============================================================================

CairoModule_FillRectSubPX::CairoModule_FillRectSubPX(int w, int h) : CairoModule_FillRect(w, h) {}
CairoModule_FillRectSubPX::~CairoModule_FillRectSubPX() {}

void CairoModule_FillRectSubPX::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);
  configureContext(cr);

  double sub = 0.1;
  double inc = 0.8 / (double)quantity;

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++, sub += inc)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      cairo_set_source_rgba(cr,
        (double)c.getRed() / 255.0,
        (double)c.getGreen() / 255.0,
        (double)c.getBlue() / 255.0,
        (double)c.getAlpha() / 255.0);
      cairo_rectangle(cr, sub + r.x, sub + r.y, r.w, r.h);
      cairo_fill(cr);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++, sub += inc)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      cairo_pattern_t* pattern = setupCairoPatternForRect(r, c);
      cairo_set_source(cr, pattern);
      cairo_rectangle(cr, sub + r.x, sub + r.y, r.w, r.h);
      cairo_fill(cr);
      cairo_pattern_destroy(pattern);
    }
  }

  cairo_destroy(cr);
}

Fog::String CairoModule_FillRectSubPX::getType()
{
  return Fog::Ascii8("FillRectSubPX");
}

// ============================================================================
// [FogBench - CairoModule_FillRectAffine]
// ============================================================================

CairoModule_FillRectAffine::CairoModule_FillRectAffine(int w, int h) : CairoModule_FillRect(w, h) {}
CairoModule_FillRectAffine::~CairoModule_FillRectAffine() {}

void CairoModule_FillRectAffine::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);
  configureContext(cr);

  double cx = (double)w / 2.0;
  double cy = (double)h / 2.0;
  double rot = 0.0;

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++, rot += 0.01)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      cairo_identity_matrix(cr);
      cairo_translate(cr, cx, cy);
      cairo_rotate(cr, rot);
      cairo_translate(cr, -cx, -cy);

      cairo_set_source_rgba(cr,
        (double)c.getRed() / 255.0,
        (double)c.getGreen() / 255.0,
        (double)c.getBlue() / 255.0,
        (double)c.getAlpha() / 255.0);
      cairo_rectangle(cr, r.x, r.y, r.w, r.h);
      cairo_fill(cr);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++, rot += 0.01)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      cairo_identity_matrix(cr);
      cairo_translate(cr, cx, cy);
      cairo_rotate(cr, rot);
      cairo_translate(cr, -cx, -cy);

      cairo_pattern_t* pattern = setupCairoPatternForRect(r, c);
      cairo_set_source(cr, pattern);
      cairo_rectangle(cr, r.x, r.y, r.w, r.h);
      cairo_fill(cr);
      cairo_pattern_destroy(pattern);
    }
  }

  cairo_destroy(cr);
}

Fog::String CairoModule_FillRectAffine::getType()
{
  return Fog::Ascii8("FillRectAffine");
}

// ============================================================================
// [FogBench - CairoModule_FillRound]
// ============================================================================

CairoModule_FillRound::CairoModule_FillRound(int w, int h) : CairoModule_FillRect(w, h) {}
CairoModule_FillRound::~CairoModule_FillRound() {}

void CairoModule_FillRound::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);
  configureContext(cr);

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      cairo_set_source_rgba(cr,
        (double)c.getRed() / 255.0,
        (double)c.getGreen() / 255.0,
        (double)c.getBlue() / 255.0,
        (double)c.getAlpha() / 255.0);
      addRound(cr, r, 8);
      cairo_fill(cr);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      cairo_pattern_t* pattern = setupCairoPatternForRect(r, c);
      cairo_set_source(cr, pattern);
      addRound(cr, r, 8);
      cairo_fill(cr);
      cairo_pattern_destroy(pattern);
    }
  }

  cairo_destroy(cr);
}

Fog::String CairoModule_FillRound::getType()
{
  return Fog::Ascii8("FillRound");
}

void CairoModule_FillRound::addRound(cairo_t* cr, Fog::IntRect rect, double radius)
{
  double x0 = rect.x;
  double y0 = rect.y;
  double rect_width = rect.w;
  double rect_height = rect.h;

  double x1 = x0 + rect_width;
  double y1 = y0 + rect_height;

  radius *= 2.0;

  if (!rect_width || !rect_height) return;

  if (rect_width / 2 < radius)
  {
    if (rect_height / 2 < radius)
    {
      cairo_move_to(cr, x0, (y0 + y1)/2);
      cairo_curve_to(cr, x0 ,y0, x0, y0, (x0 + x1)/2, y0);
      cairo_curve_to(cr, x1, y0, x1, y0, x1, (y0 + y1)/2);
      cairo_curve_to(cr, x1, y1, x1, y1, (x1 + x0)/2, y1);
      cairo_curve_to(cr, x0, y1, x0, y1, x0, (y0 + y1)/2);
    }
    else
    {
      cairo_move_to(cr, x0, y0 + radius);
      cairo_curve_to(cr, x0 ,y0, x0, y0, (x0 + x1)/2, y0);
      cairo_curve_to(cr, x1, y0, x1, y0, x1, y0 + radius);
      cairo_line_to(cr, x1 , y1 - radius);
      cairo_curve_to(cr, x1, y1, x1, y1, (x1 + x0)/2, y1);
      cairo_curve_to(cr, x0, y1, x0, y1, x0, y1- radius);
    }
  }
  else
  {
    if (rect_height / 2 < radius)
    {
      cairo_move_to(cr, x0, (y0 + y1)/2);
      cairo_curve_to(cr, x0 , y0, x0 , y0, x0 + radius, y0);
      cairo_line_to(cr, x1 - radius, y0);
      cairo_curve_to(cr, x1, y0, x1, y0, x1, (y0 + y1)/2);
      cairo_curve_to(cr, x1, y1, x1, y1, x1 - radius, y1);
      cairo_line_to(cr, x0 + radius, y1);
      cairo_curve_to(cr, x0, y1, x0, y1, x0, (y0 + y1)/2);
    }
    else
    {
      cairo_move_to(cr, x0, y0 + radius);
      cairo_curve_to(cr, x0 , y0, x0 , y0, x0 + radius, y0);
      cairo_line_to(cr, x1 - radius, y0);
      cairo_curve_to(cr, x1, y0, x1, y0, x1, y0 + radius);
      cairo_line_to(cr, x1 , y1 - radius);
      cairo_curve_to(cr, x1, y1, x1, y1, x1 - radius, y1);
      cairo_line_to(cr, x0 + radius, y1);
      cairo_curve_to(cr, x0, y1, x0, y1, x0, y1- radius);
    }
  }
  cairo_close_path(cr);
}

// ============================================================================
// [FogBench - CairoModule_FillPolygon]
// ============================================================================

CairoModule_FillPolygon::CairoModule_FillPolygon(int w, int h) : CairoModule(w, h) {}
CairoModule_FillPolygon::~CairoModule_FillPolygon() {}

void CairoModule_FillPolygon::prepare(int quantity, int sw, int sh)
{
  r_poly.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void CairoModule_FillPolygon::finish()
{
  r_poly.free();
  r_argb.free();
}

void CairoModule_FillPolygon::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);
  configureContext(cr);
  cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++)
    {
      Fog::Argb c = r_argb.data[a];
      const Fog::DoublePoint* polyData = &r_poly.data[a*10];

      cairo_set_source_rgba(cr,
        (double)c.getRed() / 255.0,
        (double)c.getGreen() / 255.0,
        (double)c.getBlue() / 255.0,
        (double)c.getAlpha() / 255.0);

      for (int i = 0; i < 10; i++)
      {
        if (i == 0)
          cairo_move_to(cr, polyData[i].x, polyData[i].y);
        else
          cairo_line_to(cr, polyData[i].x, polyData[i].y);
      }
      cairo_close_path(cr);
      cairo_fill(cr);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++)
    {
      Fog::Argb c = r_argb.data[a];
      const Fog::DoublePoint* polyData = &r_poly.data[a*10];

      cairo_pattern_t* pattern = setupCairoPatternForPoint(polyData[0], polyData[9], c);
      cairo_set_source(cr, pattern);

      for (int i = 0; i < 10; i++)
      {
        if (i == 0)
          cairo_move_to(cr, polyData[i].x, polyData[i].y);
        else
          cairo_line_to(cr, polyData[i].x, polyData[i].y);
      }
      cairo_close_path(cr);
      cairo_fill(cr);
      cairo_pattern_destroy(pattern);
    }
  }

  cairo_destroy(cr);
}

Fog::String CairoModule_FillPolygon::getType()
{
  return Fog::Ascii8("FillPolygon");
}

// ============================================================================
// [FogBench - CairoModule_Image]
// ============================================================================

CairoModule_Image::CairoModule_Image(int w, int h) : CairoModule(w, h) {}
CairoModule_Image::~CairoModule_Image() {}

void CairoModule_Image::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_numb.init(quantity, 0, NUM_SPRITES - 1);

  for (int a = 0; a < NUM_SPRITES; a++)
  {
    images[a] = sprite[a].scale(Fog::IntSize(sw, sh));
    images_cairo[a] = cairo_image_surface_create_for_data(
      (unsigned char*)images[a].getFirst(),
      CAIRO_FORMAT_ARGB32,
      images[a].getWidth(),
      images[a].getHeight(),
      images[a].getStride());
  }
}

void CairoModule_Image::finish()
{
  r_rect.free();
  r_numb.free();

  for (int a = 0; a < NUM_SPRITES; a++)
  {
    cairo_surface_destroy(images_cairo[a]);
  }
}

void CairoModule_Image::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);
  configureContext(cr);

  // If operator is SRC then we need to clip to rectangle, because cairo will
  // otherwise clear everything outside.
  if (op == BENCH_OPERATOR_SRC)
  {
    double w = cairo_image_surface_get_width(images_cairo[0]);
    double h = cairo_image_surface_get_height(images_cairo[0]);

    for (int a = 0; a < quantity; a++)
    {
      int x = r_rect.data[a].x;
      int y = r_rect.data[a].y;
      cairo_set_source_surface(cr, images_cairo[r_numb.data[a]], x, y);
      cairo_rectangle(cr, x, y, w, h);
      cairo_fill(cr);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++)
    {
      int x = r_rect.data[a].x;
      int y = r_rect.data[a].y;
      cairo_set_source_surface(cr, images_cairo[r_numb.data[a]], x, y);
      cairo_paint(cr);
    }
  }

  cairo_destroy(cr);
}

Fog::String CairoModule_Image::getType()
{
  return Fog::Ascii8("Image");
}

// ============================================================================
// [FogBench - CairoModule_ImageAffine]
// ============================================================================

CairoModule_ImageAffine::CairoModule_ImageAffine(int w, int h) : CairoModule_Image(w, h) {}
CairoModule_ImageAffine::~CairoModule_ImageAffine() {}

void CairoModule_ImageAffine::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);
  configureContext(cr);

  double cx = (double)w / 2.0;
  double cy = (double)h / 2.0;
  double rot = 0.0;

  // If operator is SRC then we need to clip to rectangle, because cairo will
  // otherwise clear everything outside.
  if (op == BENCH_OPERATOR_SRC)
  {
    double w = cairo_image_surface_get_width(images_cairo[0]);
    double h = cairo_image_surface_get_height(images_cairo[0]);

    for (int a = 0; a < quantity; a++, rot += 0.01)
    {
      int x = r_rect.data[a].x;
      int y = r_rect.data[a].y;

      cairo_identity_matrix(cr);
      cairo_translate(cr, cx, cy);
      cairo_rotate(cr, rot);
      cairo_translate(cr, -cx, -cy);

      cairo_set_source_surface(cr, images_cairo[r_numb.data[a]], x, y);
      cairo_rectangle(cr, x, y, w, h);
      cairo_fill(cr);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++, rot += 0.01)
    {
      int x = r_rect.data[a].x;
      int y = r_rect.data[a].y;

      cairo_identity_matrix(cr);
      cairo_translate(cr, cx, cy);
      cairo_rotate(cr, rot);
      cairo_translate(cr, -cx, -cy);

      cairo_set_source_surface(cr, images_cairo[r_numb.data[a]], x, y);
      cairo_paint(cr);
    }
  }

  cairo_destroy(cr);
}

Fog::String CairoModule_ImageAffine::getType()
{
  return Fog::Ascii8("ImageAffine");
}

// ============================================================================
// [FogBench - CairoBenchmarkContext]
// ============================================================================

CairoBenchmarkContext::CairoBenchmarkContext(BenchmarkMaster* master) :
  AbstractBenchmarkContext(master, Fog::Ascii8("Cairo"))
{
}

CairoBenchmarkContext::~CairoBenchmarkContext()
{
}

#define DO_BENCH(__Module__, __op__, __source__, __w__, __h__) \
  { \
    __Module__ mod(_master->getWidth(), _master->getHeight()); \
    bench(mod, __op__, __source__, __w__, __h__, _master->getQuantity()); \
  }

void CairoBenchmarkContext::run()
{
  header();

  const Fog::IntSize* sizes = _master->getSizes().getData();
  int quantity = _master->_quantity;
  sysuint_t s;

  // Create-Destroy
  DO_BENCH(CairoModule_CreateDestroy, BENCH_OPERATOR_NONE, BENCH_SOURCE_NONE, 0, 0)

  for (s = 0; s < _master->getSizes().getLength(); s++)
  {
    DO_BENCH(CairoModule_FillRect      , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillRect      , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillRect      , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillRect      , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(CairoModule_FillRectSubPX , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillRectSubPX , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillRectSubPX , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillRectSubPX , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(CairoModule_FillRectAffine, BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillRectAffine, BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillRectAffine, BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillRectAffine, BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(CairoModule_FillRound     , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillRound     , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillRound     , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillRound     , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(CairoModule_FillPolygon   , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillPolygon   , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillPolygon   , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_FillPolygon   , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(CairoModule_Image         , BENCH_OPERATOR_SRC     , BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_Image         , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)

    DO_BENCH(CairoModule_ImageAffine   , BENCH_OPERATOR_SRC     , BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)
    DO_BENCH(CairoModule_ImageAffine   , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)
  }

  _master->_contexts.append(this);
  footer();
}

#endif // FOG_BENCH_CAIRO
