#include <Fog/Core.h>
#include <Fog/G2d.h>
#include <Fog/UI.h>

#include "Resources.h"

using namespace Fog;

// ============================================================================
// [AppWindow - Declaration]
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
  Image sprite[2];
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

  sprite[0].readFromBuffer(_resource_babelfish_png, FOG_ARRAY_SIZE(_resource_babelfish_png));
  sprite[1].readFromBuffer(_resource_firewall_png, FOG_ARRAY_SIZE(_resource_firewall_png));
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

  p.setCompositingOperator(COMPOSITE_SRC);
  p.setSource(Texture(background, TEXTURE_TILE_REPEAT));
  p.fillAll();

  ColorStopList stops;
  stops.add(0.00f, Argb32(0xFFFFFFFF));
  stops.add(0.25f, Argb32(0xFFFFFF00));
  stops.add(0.50f, Argb32(0xFFFF0000));
  stops.add(0.75f, Argb32(0xFF000000));
  stops.add(1.00f, Argb32(0x00000000));

  SizeI s(128, 128);
  int x = 0;
  int y = 0;

  p.setCompositingOperator(COMPOSITE_SRC_OVER);

  LinearGradientF linear;
  RadialGradientF radial;
  ConicalGradientF conical;
  RectangularGradientF rectangular;

  linear.setStops(stops);
  linear.setStart(32.0f, 32.0f);
  linear.setEnd(96.0f, 96.0f);

  radial.setStops(stops);
  radial.setCenter(64.0f, 64.0f);
  radial.setFocal(40.0f, 40.0f);
  radial.setRadius(50.0f, 50.0f);

  conical.setStops(stops);
  conical.setCenter(64.0f, 64.0f);
  conical.setAngle(0.0f);

  rectangular.setStops(stops);
  rectangular.setFirst(32.0f, 32.0f);
  rectangular.setSecond(92.0f, 92.0f);
  rectangular.setFocal(72.0f, 72.0f);

  for (uint32_t type = 0; type < GRADIENT_TYPE_COUNT; type++)
  {
    GradientF* g = NULL;

    switch (type)
    {
      case GRADIENT_TYPE_LINEAR: 
        g = &linear;
        break;
      case GRADIENT_TYPE_RADIAL:
        g = &radial;
        break;
      case GRADIENT_TYPE_CONICAL:
        g = &conical;
        break;
      case GRADIENT_TYPE_RECTANGULAR:
        g = &rectangular;
        break;
    }

    for (uint32_t spread = 0; spread < GRADIENT_SPREAD_COUNT; spread++)
    {
      g->setGradientSpread(spread);
      p.save();
      p.translate(PointF(
        5.0f + float(spread) * (128.0f + 5.0f), 
        5.0f + float(type) * (128.0f + 5.0f)));
      p.setSource(*g);
      p.fillRect(RectI(0, 0, 128, 128));
      p.restore();
    }
  }
}

// ============================================================================
// [FOG_UI_MAIN]
// ============================================================================

FOG_UI_MAIN()
{
  Application app(StringW::fromAscii8("UI"));
  AppWindow wnd(app.getUIEngine());

  wnd.setWindowTitle(StringW::fromAscii8("FogGradient"));
  wnd.setWindowSize(SizeI(700, 700));
  wnd.show();

  return app.run();
}