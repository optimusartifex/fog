#include "BenchQt4.h"

#if defined(FOG_BENCH_QT4)

// ============================================================================
// [FogBench - Helpers]
// ============================================================================

static QBrush setupQt4PatternForPoint(
  const Fog::DoublePoint& pt0, const Fog::DoublePoint& pt1, const Fog::Argb& argb)
{
  QLinearGradient pattern(pt0.x, pt0.y, pt1.x, pt1.y);
  pattern.setSpread(QGradient::PadSpread);
  pattern.setColorAt(0.0, QColor(0xFF, 0xFF, 0xFF, 0xFF));
  pattern.setColorAt(0.5, QColor(0xFF, 0x00, 0x00, 0x7F));
  pattern.setColorAt(1.0, QColor(argb.getRed(), argb.getGreen(), argb.getBlue(), argb.getAlpha()));
  return QBrush(pattern);
}

static QBrush setupQt4PatternForRect(
  const Fog::IntRect& rect, const Fog::Argb& argb)
{
  return setupQt4PatternForPoint(
    Fog::DoublePoint(rect.x, rect.y),
    Fog::DoublePoint(rect.x + rect.w, rect.y + rect.h),
    argb);
}

// ============================================================================
// [FogBench - Qt4Module]
// ============================================================================

Qt4Module::Qt4Module(int w, int h) : FogModule(w, h)
{
  screen_qt4 = QImage(
    (unsigned char*)screen.getFirst(),
    screen.getWidth(),
    screen.getHeight(),
    QImage::Format_ARGB32_Premultiplied);
}

Qt4Module::~Qt4Module()
{
}

Fog::String Qt4Module::getEngine()
{
  return Fog::Ascii8("Qt4");
}

void Qt4Module::configurePainter(QPainter& painter)
{
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

  if (op == Fog::OPERATOR_SRC)
    painter.setCompositionMode(QPainter::CompositionMode_Source);
  else
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
}

// ============================================================================
// [FogBench - Qt4Module_CreateDestroy]
// ============================================================================

Qt4Module_CreateDestroy::Qt4Module_CreateDestroy(int w, int h) : Qt4Module(w, h) {}
Qt4Module_CreateDestroy::~Qt4Module_CreateDestroy() {}

void Qt4Module_CreateDestroy::bench(int quantity)
{
  for (int a = 0; a < quantity; a++)
  {
    QPainter p(&screen_qt4);
    configurePainter(p);
  }
}

Fog::String Qt4Module_CreateDestroy::getType()
{
  return Fog::Ascii8("Create-Destroy");
}

// ============================================================================
// [FogBench - Qt4Module_FillRect]
// ============================================================================

Qt4Module_FillRect::Qt4Module_FillRect(int w, int h) : Qt4Module(w, h) {}
Qt4Module_FillRect::~Qt4Module_FillRect() {}

void Qt4Module_FillRect::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void Qt4Module_FillRect::finish()
{
  r_rect.free();
  r_argb.free();
}

void Qt4Module_FillRect::bench(int quantity)
{
  QPainter p(&screen_qt4);
  configurePainter(p);

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      p.fillRect(QRect(r.x, r.y, r.w, r.h), QColor(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha()));
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      p.fillRect(QRect(r.x, r.y, r.w, r.h), setupQt4PatternForRect(r, c));
    }
  }
}

Fog::String Qt4Module_FillRect::getType()
{
  return Fog::Ascii8("FillRect");
}

// ============================================================================
// [FogBench - Qt4Module_FillRectSubPX]
// ============================================================================

Qt4Module_FillRectSubPX::Qt4Module_FillRectSubPX(int w, int h) : Qt4Module_FillRect(w, h) {}
Qt4Module_FillRectSubPX::~Qt4Module_FillRectSubPX() {}

void Qt4Module_FillRectSubPX::bench(int quantity)
{
  QPainter p(&screen_qt4);
  configurePainter(p);

  qreal sub = qreal(0.1);
  qreal inc = qreal(0.8) / (qreal)quantity;

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++, sub += inc)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      p.fillRect(QRectF(sub + r.x, sub + r.y, r.w, r.h), QColor(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha()));
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++, sub += inc)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      p.fillRect(QRectF(sub + r.x, r.y, sub + r.w, r.h), setupQt4PatternForRect(r, c));
    }
  }
}

Fog::String Qt4Module_FillRectSubPX::getType()
{
  return Fog::Ascii8("FillRectSubPX");
}

// ============================================================================
// [FogBench - Qt4Module_FillRectAffine]
// ============================================================================

Qt4Module_FillRectAffine::Qt4Module_FillRectAffine(int w, int h) : Qt4Module_FillRect(w, h) {}
Qt4Module_FillRectAffine::~Qt4Module_FillRectAffine() {}

void Qt4Module_FillRectAffine::bench(int quantity)
{
  QPainter p(&screen_qt4);
  configurePainter(p);

  double cx = (double)w / 2.0;
  double cy = (double)h / 2.0;
  double rot = 0.0;

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++, rot += 0.01)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      Fog::DoubleMatrix m;
      m.translate(cx, cy);
      m.rotate(rot);
      m.translate(-cx, -cy);

      p.setMatrix(QMatrix(m.m[0], m.m[1], m.m[2], m.m[3], m.m[4], m.m[5]));
      p.fillRect(QRect(r.x, r.y, r.w, r.h), QColor(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha()));
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++, rot += 0.01)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      Fog::DoubleMatrix m;
      m.translate(cx, cy);
      m.rotate(rot);
      m.translate(-cx, -cy);

      p.setMatrix(QMatrix(m.m[0], m.m[1], m.m[2], m.m[3], m.m[4], m.m[5]));
      p.fillRect(QRect(r.x, r.y, r.w, r.h), setupQt4PatternForRect(r, c));
    }
  }
}

Fog::String Qt4Module_FillRectAffine::getType()
{
  return Fog::Ascii8("FillRectAffine");
}

// ============================================================================
// [FogBench - Qt4Module_FillRound]
// ============================================================================

Qt4Module_FillRound::Qt4Module_FillRound(int w, int h) : Qt4Module_FillRect(w, h) {}
Qt4Module_FillRound::~Qt4Module_FillRound() {}

void Qt4Module_FillRound::bench(int quantity)
{
  QPainter p(&screen_qt4);
  configurePainter(p);

  p.setPen(QPen(Qt::NoPen));

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      p.setBrush(QBrush(QColor(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha())));
      p.drawRoundedRect(QRect(r.x, r.y, r.w, r.h), 8.0, 8.0);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++)
    {
      Fog::IntRect r(r_rect.data[a]);
      Fog::Argb c(r_argb.data[a]);

      p.setBrush(setupQt4PatternForRect(r, c));
      p.drawRoundedRect(QRect(r.x, r.y, r.w, r.h), 8.0, 8.0);
    }
  }
}

Fog::String Qt4Module_FillRound::getType()
{
  return Fog::Ascii8("FillRound");
}

// ============================================================================
// [FogBench - Qt4Module_FillPolygon]
// ============================================================================

Qt4Module_FillPolygon::Qt4Module_FillPolygon(int w, int h) : Qt4Module(w, h) {}
Qt4Module_FillPolygon::~Qt4Module_FillPolygon() {}

void Qt4Module_FillPolygon::prepare(int quantity, int sw, int sh)
{
  r_poly.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void Qt4Module_FillPolygon::finish()
{
  r_poly.free();
  r_argb.free();
}

void Qt4Module_FillPolygon::bench(int quantity)
{
  QPainter p(&screen_qt4);
  configurePainter(p);

  p.setPen(QPen(Qt::NoPen));

  if (source == BENCH_SOURCE_ARGB)
  {
    for (int a = 0; a < quantity; a++)
    {
      const Fog::DoublePoint* polyData = &r_poly.data[a*10];
      Fog::Argb c = r_argb.data[a];

      p.setBrush(QColor(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha()));
      p.drawPolygon((const QPointF*)(polyData), 10, Qt::OddEvenFill);
    }
  }
  else
  {
    for (int a = 0; a < quantity; a++)
    {
      const Fog::DoublePoint* polyData = &r_poly.data[a*10];
      Fog::Argb c = r_argb.data[a];

      p.setBrush(setupQt4PatternForPoint(polyData[0], polyData[9], c));
      p.drawPolygon((const QPointF*)(&r_poly.data[a * 10]), 10, Qt::OddEvenFill);
    }
  }
}

Fog::String Qt4Module_FillPolygon::getType()
{
  return Fog::Ascii8("FillPolygon");
}

// ============================================================================
// [FogBench - Qt4Module_Image]
// ============================================================================

Qt4Module_Image::Qt4Module_Image(int w, int h) : Qt4Module(w, h) {}
Qt4Module_Image::~Qt4Module_Image() {}

void Qt4Module_Image::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_numb.init(quantity, 0, NUM_SPRITES - 1);

  for (int a = 0; a < NUM_SPRITES; a++)
  {
    images[a] = sprite[a].scaled(Fog::IntSize(sw, sh));
    images_qt4[a] = QImage(
      (unsigned char*)images[a].getFirst(),
      images[a].getWidth(),
      images[a].getHeight(),
      QImage::Format_ARGB32_Premultiplied);
  }
}

void Qt4Module_Image::finish()
{
  r_rect.free();
  r_numb.free();

  for (int a = 0; a < NUM_SPRITES; a++)
  {
    images_qt4[a] = QImage();
  }
}

void Qt4Module_Image::bench(int quantity)
{
  QPainter p(&screen_qt4);
  configurePainter(p);

  for (int a = 0; a < quantity; a++)
  {
    int x = r_rect.data[a].x;
    int y = r_rect.data[a].y;
    p.drawImage(QPoint(x, y), images_qt4[r_numb.data[a]]);
  }
}

Fog::String Qt4Module_Image::getType()
{
  return Fog::Ascii8("Image");
}

// ============================================================================
// [FogBench - Qt4Module_ImageAffine]
// ============================================================================

Qt4Module_ImageAffine::Qt4Module_ImageAffine(int w, int h) : Qt4Module_Image(w, h) {}
Qt4Module_ImageAffine::~Qt4Module_ImageAffine() {}

void Qt4Module_ImageAffine::bench(int quantity)
{
  QPainter p(&screen_qt4);
  configurePainter(p);

  double cx = (double)w / 2.0;
  double cy = (double)h / 2.0;
  double rot = 0.0;

  for (int a = 0; a < quantity; a++, rot += 0.01)
  {
    int x = r_rect.data[a].x;
    int y = r_rect.data[a].y;

    Fog::DoubleMatrix m;
    m.translate(cx, cy);
    m.rotate(rot);
    m.translate(-cx, -cy);

    p.setMatrix(QMatrix(m.m[0], m.m[1], m.m[2], m.m[3], m.m[4], m.m[5]));
    p.drawImage(QPoint(x, y), images_qt4[r_numb.data[a]]);
  }
}

Fog::String Qt4Module_ImageAffine::getType()
{
  return Fog::Ascii8("ImageAffine");
}

// ============================================================================
// [FogBench - Qt4BenchmarkContext]
// ============================================================================

Qt4BenchmarkContext::Qt4BenchmarkContext(BenchmarkMaster* master) :
  AbstractBenchmarkContext(master, Fog::Ascii8("Qt4"))
{
}

Qt4BenchmarkContext::~Qt4BenchmarkContext()
{
}

#define DO_BENCH(__Module__, __op__, __source__, __w__, __h__) \
  { \
    __Module__ mod(_master->getWidth(), _master->getHeight()); \
    bench(mod, __op__, __source__, __w__, __h__, _master->getQuantity()); \
  }

void Qt4BenchmarkContext::run()
{
  header();

  const Fog::IntSize* sizes = _master->getSizes().getData();
  int quantity = _master->_quantity;
  sysuint_t s;

  // Create-Destroy
  DO_BENCH(Qt4Module_CreateDestroy, BENCH_OPERATOR_NONE, BENCH_SOURCE_NONE, 0, 0)

  for (s = 0; s < _master->getSizes().getLength(); s++)
  {
    DO_BENCH(Qt4Module_FillRect      , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillRect      , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillRect      , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillRect      , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(Qt4Module_FillRectSubPX , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillRectSubPX , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillRectSubPX , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillRectSubPX , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(Qt4Module_FillRectAffine, BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillRectAffine, BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillRectAffine, BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillRectAffine, BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(Qt4Module_FillRound     , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillRound     , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillRound     , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillRound     , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(Qt4Module_FillPolygon   , BENCH_OPERATOR_SRC     , BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillPolygon   , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_ARGB   , sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillPolygon   , BENCH_OPERATOR_SRC     , BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_FillPolygon   , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_PATTERN, sizes[s].w, sizes[s].h)

    DO_BENCH(Qt4Module_Image         , BENCH_OPERATOR_SRC     , BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_Image         , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)

    DO_BENCH(Qt4Module_ImageAffine   , BENCH_OPERATOR_SRC     , BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)
    DO_BENCH(Qt4Module_ImageAffine   , BENCH_OPERATOR_SRC_OVER, BENCH_SOURCE_NONE   , sizes[s].w, sizes[s].h)
  }

  _master->_contexts.append(this);
  footer();
}

#endif // FOG_BENCH_QT4
