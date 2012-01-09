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

  p.setSource(Argb32(0xFFFFFFFF));
  p.fillAll();
  
  p.setSource(Argb32(0xFF000000));
  p.fillCircle(CircleF(160.0f, 120.0f, 50.0f));

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
  StringW str;
  str.append(CharW(0x1E9B));
  str.append(CharW(0x0323));
  str.normalize(CHAR_NFKC);

  uint16_t data[16];
  for (size_t i = 0; i < str.getLength(); i++)
    data[i] = str.getAt(i);
  
  //uint16_t data[3] = { 0x0071, 0x307, 0x323 };
  //StringUtil::makeCanonical(data, 3, CHAR_UNICODE_VERSION_DEFAULT);

  Application app(StringW::fromAscii8("UI"));
  AppWindow wnd(app.getUIEngine());

  wnd.setWindowTitle(StringW::fromAscii8("FogTestApp"));
  wnd.setWindowSize(SizeI(340, 240));
  wnd.setWindowOpacity(0.5f);
  wnd.show();

  return app.run();
}
