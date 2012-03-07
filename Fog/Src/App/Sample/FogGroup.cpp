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

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
  
  Image background;
};

// ============================================================================
// [SampleWindow - Construction / Destruction]
// ============================================================================

AppWindow::AppWindow(UIEngine* engine, uint32_t hints) :
  UIEngineWindow(engine, hints)
{
  background.create(SizeI(40, 40), IMAGE_FORMAT_XRGB32);
  background.fillRect(RectI( 0,  0, 20, 20), Argb32(0xFFFFFFFF));
  background.fillRect(RectI(20,  0, 20, 20), Argb32(0xFFCFCFCF));
  background.fillRect(RectI( 0, 20, 20, 20), Argb32(0xFFCFCFCF));
  background.fillRect(RectI(20, 20, 20, 20), Argb32(0xFFFFFFFF));
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

  p.setSource(Texture(background));
  p.fillAll();

  TimeTicks startTime = TimeTicks::now();

  p.setOpacity(0.5f);
  p.beginGroup();
    LinearGradientF lg;
    lg.setStart(100.0f, 100.0f);
    lg.setEnd(100.0f, 300.0f);
    lg.addStop(0.0f, Argb32(0xFFFFFF00));
    lg.addStop(1.0f, Argb32(0xFF00FF00));
    p.setSource(lg);
    p.fillRect(RectI(100, 100, 200, 200));

    p.setSource(Argb32(0xFFFF0000));
    p.fillRound(RoundF(150.0f, 150.0f, 200.0f, 200.0f, 50.0f));

    p.setSource(Argb32(0xFF0000FF));
    p.fillRect(RectI(200, 200, 200, 200));
  p.paintGroup();
  p.resetOpacity();

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

  wnd.setWindowTitle(StringW::fromAscii8("FogTest"));
  wnd.setWindowSize(SizeI(510, 530));
  wnd.show();

  return app.run();
}
