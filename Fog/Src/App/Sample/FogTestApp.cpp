#include <Fog/Core.h>
#include <Fog/G2d.h>
#include <Fog/UI.h>

using namespace Fog;

// ============================================================================
// [SampleWindow - Declaration]
// ============================================================================

struct AppWindow : public UIEngineWindow
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  AppWindow(UIEngine* engine, uint32_t hints = 0);
  virtual ~AppWindow();

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  virtual void onEngineEvent(UIEngineEvent* ev);
  virtual void onPaint(Painter* p);
};

// ============================================================================
// [SampleWindow - Construction / Destruction]
// ============================================================================

AppWindow::AppWindow(UIEngine* engine, uint32_t hints) :
  UIEngineWindow(engine, hints)
{
}

AppWindow::~AppWindow()
{
}

// ============================================================================
// [SampleWindow - Event Handlers]
// ============================================================================

void AppWindow::onEngineEvent(UIEngineEvent* ev)
{
  switch (ev->getCode())
  {
    case UI_ENGINE_EVENT_CLOSE:
      Application::get()->quit();
      break;

    case UI_ENGINE_EVENT_PAINT:
      onPaint(static_cast<UIEnginePaintEvent*>(ev)->getPainter());
      break;
  }
}

void AppWindow::onPaint(Painter* _p)
{
  Painter& p = *_p;
  RectI geom = getClientGeometry();

  p.setSource(Argb32(0xFFFFFFFF));
  p.fillAll();

  TimeTicks startTime = TimeTicks::now();

  p.setSource(Argb32(0xFF000000));
  //p.fillRect(100, 100, 100, 100);
  //p.fillCircle(CircleF(320.0f, 240.0f, 85.0f));

  PathF path;
  Font font;
  font.setHeight(30.0f, UNIT_PX);

  //StringW msg;
  //msg.format("Size: %d %d", geom.w, geom.h);

  //font.getTextOutline(path, CONTAINER_OP_REPLACE, PointF(250.0f, 100.0f), msg);

  //p.setSource(Argb32(0xFF000000));
  //p.fillPath(path);
/*
  FeBlur feBlur(FE_BLUR_TYPE_EXPONENTIAL, 50.0f);
  feBlur.setExtendType(FE_EXTEND_COLOR);
  feBlur.setExtendColor(Argb32(0xFF00FF00));

  path.clear();
  //path.triangle(TriangleF(200.0f, 10.0f, 10.0f, 200.0f, 400.0f, 350.0f));
  path.round(RoundF(20.0f, 20.0f, float(geom.w) - 40.0f, float(geom.h) - 40.0f, 20.0f, 20.0f));
  //path.rect(RectF(60.0f, 60.0f, float(geom.w)/2.0f, float(geom.h)/2.0f));

  p.setOpacity(0.5f);
  p.filterPath(feBlur, path);
*/

/*
  Region region;
  region.union_(RectI(100, 100, 100, 100));
  region.union_(RectI(300, 100, 100, 100));
  region.union_(RectI(100, 300, 100, 100));
  region.union_(RectI(300, 300, 100, 100));
  p.setMetaParams(region, PointI(0, 0));
*/

  p.setSource(Argb32(0xFF000000));
  p.fillAll();

  p.setCompositingOperator(COMPOSITE_ADD);

  LinearGradientF lg;
  lg.setStart(100.0f, 100.0f);
  lg.setEnd(300.0f, 100.0f);
  lg.clearStops();
  lg.addStop(0.0f, Argb32(0xFF000000));
  lg.addStop(1.0f, Argb32(0xFF0000FF));
  p.setSource(lg);
  p.fillRect(RectI(100, 100, 200, 200));
  
  lg.setStart(150.0f, 150.0f);
  lg.setEnd(150.0f, 350.0f);
  lg.clearStops();
  lg.addStop(0.0f, Argb32(0xFF000000));
  lg.addStop(1.0f, Argb32(0xFFFF0000));
  p.setSource(lg);
  p.fillRect(RectI(150, 150, 200, 200));

  lg.setStart(200.0f, 200.0f);
  lg.setEnd(400.0f, 400.0f);
  lg.clearStops();
  lg.addStop(0.0f, Argb32(0xFF000000));
  lg.addStop(1.0f, Argb32(0xFF00FF00));
  p.setSource(lg);
  p.fillRect(RectI(200, 200, 200, 200));

/*
  p.setSource(Argb32(0xFFFF0000));
  p.fillCircle(CircleF(200.0f, 200.0f, 100.0f));
  
  p.setSource(Argb32(0xFF00FF00));
  p.fillCircle(CircleF(250.0f, 250.0f, 100.0f));

  p.setSource(Argb32(0xFF0000FF));
  p.fillCircle(CircleF(300.0f, 300.0f, 100.0f));
*/
/*
  p.setSource(Argb32(0xFF000000));
  p.fillRect(RectI(100, 100, 100, 100));
  p.fillRect(RectI(60, 200, 40, 100));
  p.fillRect(RectF(100.5f, 200.5f, 99.5f, 99.5f));
*/

/*
  p.setCompositingOperator(COMPOSITE_SRC_OVER);

  p.save();

  p.clipRect(CLIP_OP_REPLACE, RectI(200, 200, 200, 200));
  p.setSource(Argb32(0x7FFF0000));
  p.fillCircle(CircleF(300.0f, 300.0f, 110.0f));

  p.clipRect(CLIP_OP_REPLACE, RectI(220, 220, 200, 200));
  p.setSource(Argb32(0x7F0000FF));
  p.fillCircle(CircleF(300.0f, 300.0f, 110.0f));

  //p.fillRect(RectI(100, 100, 400, 400));
  //p.setSource(Argb32(0xFFFF00FF));
  //p.fillRect(RectI(300, 300, 200, 200));
  p.restore();
*/
  //p.setSource(Argb32(0xFF0000FF));
  //p.drawRect(0.5f, 0.5f, float(geom.w) - 1.0f, float(geom.h) - 1.0f);
  
  // p.filterAll(feBlur);

  //FeMorphology feMorphology(FE_MORPHOLOGY_TYPE_ERODE, 1.0f);
  //p.filterAll(feMorphology);

  //p.setOpacity(0.15f);
  //p.setSource(Argb32(0xFF0000FF));
  //p.fillRect(RectI(60, 60, 140, 100));

  //p.resetOpacity();
  //p.setSource(Argb32(0xFF00FF00));

  //p.drawRect(0.5f, 0.5f, float(geom.w) - 1.0f, float(geom.h) - 1.0f);

  // SvgDocument svg;
  // err_t err = svg.readFromFile(StringW::fromAscii8("C:/my/svg/tiger.svg"));
  // err_t err = svg.readFromFile(StringW::fromAscii8("/my/svg/tiger.svg"));
  //
  // if (err != ERR_OK)
  //   return;
  //
  // p.scale(PointF(0.35f, 0.35f));
  // p.translate(PointF(140.0f, 0.0f));
  // svg.render(&p);

  TimeDelta t = TimeTicks::now() - startTime;

  StringW text;
  text.format("Render: %g [ms]", t.getMillisecondsD());
  setWindowTitle(text);
}

// ============================================================================
// [FOG_UI_MAIN]
// ============================================================================

FOG_UI_MAIN()
{
  Application app(StringW::fromAscii8("UI"));
  AppWindow wnd(app.getUIEngine());

  wnd.setWindowTitle(StringW::fromAscii8("FogTestApp"));
  wnd.setWindowSize(SizeI(640, 480));
  //wnd.setWindowOpacity(0.5f);
  wnd.show();

  return app.run();
}
