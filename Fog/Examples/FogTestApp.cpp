#include <Fog/Core.h>
#include <Fog/G2d.h>
#include <Fog/Svg.h>
#include <Fog/UI.h>

using namespace Fog;

// ============================================================================
// [SampleWindow - Declaration]
// ============================================================================

struct SampleWindow : public UIEngineWindow
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SampleWindow(UIEngine* engine, uint32_t hints = 0);
  virtual ~SampleWindow();

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  virtual void onEngineEvent(UIEngineEvent* ev);
  virtual void onPaint(Painter* p);
};

// ============================================================================
// [SampleWindow - Construction / Destruction]
// ============================================================================

SampleWindow::SampleWindow(UIEngine* engine, uint32_t hints) :
  UIEngineWindow(engine, hints)
{
}

SampleWindow::~SampleWindow()
{
}

// ============================================================================
// [SampleWindow - Event Handlers]
// ============================================================================

void SampleWindow::onEngineEvent(UIEngineEvent* ev)
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

void SampleWindow::onPaint(Painter* p)
{
  p->setSource(Argb32(0xFFFFFFFF));
  p->fillAll();

  p->setSource(Argb32(0xFF000000));
  p->fillCircle(CircleF(100.0f, 100.0f, 50.0f));
}

// ============================================================================
// [FOG_UI_MAIN]
// ============================================================================

FOG_UI_MAIN()
{
  FloatBits f;
  f.f = Math::getSNanF();
  f.i32 |= 0x80000000;
  printf("%X", f.i32);
  f.f += 1.0f;
  printf("%X", f.i32);
  f.f += 1.0f;
  printf("%X", f.i32);
  return 0;


  Application app(StringW::fromAscii8("UI"));
  SampleWindow window(app.getUIEngine());

  window.setWindowSize(SizeI(640, 480));
  window.show();

  return app.run();
}
