// [Fog-Bench]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#include "BenchApp.h"
#if defined(FOG_BENCH_QT4)

// [Dependencies]
#include "BenchQt4.h"

// ============================================================================
// [BenchQt4 - Construction / Destruction]
// ============================================================================

BenchQt4::BenchQt4(BenchApp& app) :
  BenchModule(app)
{
}

BenchQt4::~BenchQt4()
{
}

// ============================================================================
// [BenchQt4 - Methods]
// ============================================================================

static const double sc = 1.0 / 255.0;

Fog::StringW BenchQt4::getModuleName() const
{
  return Fog::StringW::fromAscii8("Qt4");
}

void BenchQt4::bench(BenchOutput& output, const BenchParams& params)
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

  screenQt = new QImage(
    (unsigned char*)screen.getFirst(),
    screen.getWidth(),
    screen.getHeight(),
    QImage::Format_ARGB32_Premultiplied);

  if (screenQt == NULL)
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

  delete screenQt;
  screenQt = NULL;

  freeSprites();
}

void BenchQt4::prepareSprites(int size)
{
  BenchModule::prepareSprites(size);

  size_t i, length = sprites.getLength();
  for (i = 0; i < length; i++)
  {
    QImage* sprite = new QImage(
      (unsigned char*)sprites[i].getFirst(),
      sprites[i].getWidth(),
      sprites[i].getHeight(),
      QImage::Format_ARGB32_Premultiplied);

    if (sprite == NULL)
    {
      freeSprites();
      return;
    }

    spritesQt.append(sprite);
  }
}

void BenchQt4::freeSprites()
{
  size_t i, length = spritesQt.getLength();

  for (i = 0; i < length; i++)
    delete spritesQt[i];

  spritesQt.clear();
  BenchModule::freeSprites();
}

// ============================================================================
// [BenchQt4 - Cairo-Specific]
// ============================================================================

void BenchQt4::configurePainter(QPainter& p, const BenchParams& params)
{
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::SmoothPixmapTransform, true);

  if (params.op == BENCH_OPERATOR_SRC)
    p.setCompositionMode(QPainter::CompositionMode_Source);
  else
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
}

QBrush BenchQt4::createLinearGradient(
  const Fog::PointF& pt0, const Fog::PointF& pt1,
  const Fog::Argb32& c0, const Fog::Argb32& c1, const Fog::Argb32& c2)
{
  QLinearGradient g(pt0.x, pt0.y, pt1.x, pt1.y);

  g.setSpread(QGradient::PadSpread);
  g.setColorAt(0.0, QColor(c0.getRed(), c0.getGreen(), c0.getBlue(), c0.getAlpha()));
  g.setColorAt(0.5, QColor(c1.getRed(), c1.getGreen(), c1.getBlue(), c1.getAlpha()));
  g.setColorAt(1.0, QColor(c2.getRed(), c2.getGreen(), c2.getBlue(), c2.getAlpha()));

  return QBrush(g);
}

void BenchQt4::runCreateDestroy(BenchOutput& output, const BenchParams& params)
{
  uint32_t i, quantity = params.quantity;
  for (i = 0; i < quantity; i++)
  {
    QPainter p(screenQt);
    configurePainter(p, params);
  }
}

void BenchQt4::runFillRectI(BenchOutput& output, const BenchParams& params)
{
  QPainter p(screenQt);
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

      p.fillRect(
        QRect(r.x, r.y, r.w, r.h),
        QColor(c0.getRed(), c0.getGreen(), c0.getBlue(), c0.getAlpha()));
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

      p.fillRect(
        QRect(r.x, r.y, r.w, r.h),
        createLinearGradient(Fog::PointF(r.x, r.y), Fog::PointF(r.x + r.w, r.y + r.h), c0, c1, c2));
    }
  }
}

void BenchQt4::runFillRectF(BenchOutput& output, const BenchParams& params)
{
  QPainter p(screenQt);
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

      p.fillRect(
        QRect(r.x, r.y, r.w, r.h),
        QColor(c0.getRed(), c0.getGreen(), c0.getBlue(), c0.getAlpha()));
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

      p.fillRect(
        QRect(r.x, r.y, r.w, r.h),
        createLinearGradient(Fog::PointF(r.x, r.y), Fog::PointF(r.x + r.w, r.y + r.h), c0, c1, c2));
    }
  }
}

void BenchQt4::runFillRectRotate(BenchOutput& output, const BenchParams& params)
{
  QPainter p(screenQt);
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

      Fog::TransformF t;
      t.translate(Fog::PointF(cx, cy));
      t.rotate(angle);
      t.translate(Fog::PointF(-cx, -cy));

      p.setMatrix(QMatrix(t[0], t[1], t[3], t[4], t[5], t[6]));
      p.fillRect(
        QRect(r.x, r.y, r.w, r.h),
        QColor(c0.getRed(), c0.getGreen(), c0.getBlue(), c0.getAlpha()));
      p.resetMatrix();
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

      Fog::TransformF t;
      t.translate(Fog::PointF(cx, cy));
      t.rotate(angle);
      t.translate(Fog::PointF(-cx, -cy));

      p.setMatrix(QMatrix(t[0], t[1], t[3], t[4], t[5], t[6]));
      p.fillRect(
        QRect(r.x, r.y, r.w, r.h),
        createLinearGradient(Fog::PointF(r.x, r.y), Fog::PointF(r.x + r.w, r.y + r.h), c0, c1, c2));
      p.resetMatrix();
    }
  }
}

void BenchQt4::runFillRound(BenchOutput& output, const BenchParams& params)
{
  QPainter p(screenQt);
  configurePainter(p, params);

  p.setPen(QPen(Qt::NoPen));

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

      p.setBrush(QBrush(QColor(c0.getRed(), c0.getGreen(), c0.getBlue(), c0.getAlpha())));
      p.drawRoundedRect(QRectF(r.x, r.y, r.w, r.h),
        Fog::Math::min(r.w * 0.5f, rad),
        Fog::Math::min(r.h * 0.5f, rad));
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

      p.setBrush(createLinearGradient(Fog::PointF(r.x, r.y), Fog::PointF(r.x + r.w, r.y + r.h), c0, c1, c2));
      p.drawRoundedRect(QRectF(r.x, r.y, r.w, r.h),
        Fog::Math::min(r.w * 0.5f, rad),
        Fog::Math::min(r.h * 0.5f, rad));
    }
  }
}

void BenchQt4::runFillPolygon(BenchOutput& output, const BenchParams& params, uint32_t complexity)
{
  QPainter p(screenQt);
  configurePainter(p, params);

  p.setPen(QPen(Qt::NoPen));

  BenchRandom rPts(app);
  BenchRandom rArgb(app);

  Fog::SizeI polyScreen(
    params.screenSize.w - params.shapeSize,
    params.screenSize.h - params.shapeSize);
  float polySize = (int)params.shapeSize;

  QPointF points[128];
  FOG_ASSERT(complexity < FOG_ARRAY_SIZE(points));

  if (params.source == BENCH_SOURCE_SOLID)
  {
    uint32_t i, quantity = params.quantity;
    for (i = 0; i < quantity; i++)
    {
      Fog::PointF base(rPts.getPointF(polyScreen));
      Fog::Argb32 c0(rArgb.getArgb32());

      for (uint32_t j = 0; j < complexity; j++)
      {
        points[j].setX(rPts.getFloat(base.x, base.x + polySize));
        points[j].setY(rPts.getFloat(base.y, base.y + polySize));
      }

      p.setBrush(QBrush(QColor(c0.getRed(), c0.getGreen(), c0.getBlue(), c0.getAlpha())));
      p.drawPolygon(points, complexity, Qt::WindingFill);
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

      for (uint32_t j = 0; j < complexity; j++)
      {
        points[j].setX(rPts.getFloat(base.x, base.x + polySize));
        points[j].setY(rPts.getFloat(base.y, base.y + polySize));
      }

      p.setBrush(createLinearGradient(Fog::PointF(base.x, base.y), Fog::PointF(base.x + polySize, base.y + polySize), c0, c1, c2));
      p.drawPolygon(points, complexity, Qt::WindingFill);
    }
  }
}

void BenchQt4::runBlitImageI(BenchOutput& output, const BenchParams& params)
{
  QPainter p(screenQt);
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
    // TODO:

    if (++spriteIndex >= spritesLength)
      spriteIndex = 0;
  }
}

void BenchQt4::runBlitImageRotate(BenchOutput& output, const BenchParams& params)
{
  QPainter p(screenQt);
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
    // TODO:

    if (++spriteIndex >= spritesLength)
      spriteIndex = 0;
  }
}

// [Guard]
#endif // FOG_BENCH_QT4
