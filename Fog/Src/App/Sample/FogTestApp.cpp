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

  p.setSource(Argb32(0xFF000000));
  //p.fillRect(100, 100, 100, 100);
  p.fillCircle(CircleF(160.0f, 120.0f, 85.0f));

  PathF path;
  Font font;
  font.setHeight(10.0f, UNIT_PX);

  StringW msg;
  msg.format("Size: %d %d", geom.w, geom.h);

  font.getTextOutline(path, CONTAINER_OP_REPLACE, PointF(250.0f, 100.0f), msg);

  p.setSource(Argb32(0xFFFF0000));
  p.fillPath(path);

  FeBlur feBlur(FE_BLUR_TYPE_BOX, 5.0f);
  feBlur.setExtendType(FE_EXTEND_COLOR);
  feBlur.setExtendColor(Argb32(0xFF00FF00));

  //p.filterRect(feBlur, RectI(60, 60, 200, 200));
  p.filterAll(feBlur);

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
}

// ============================================================================
// [FOG_UI_MAIN]
// ============================================================================

FOG_UI_MAIN()
{
  Application app(StringW::fromAscii8("UI"));
  AppWindow wnd(app.getUIEngine());

  wnd.setWindowTitle(StringW::fromAscii8("FogTestApp"));
  wnd.setWindowSize(SizeI(340, 240));
  //wnd.setWindowOpacity(0.5f);
  wnd.show();

  return app.run();
}
