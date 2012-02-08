#include <Fog/Core.h>
#include <Fog/G2d.h>
#include <Fog/UI.h>

using namespace Fog;

// ============================================================================
// [SvgWindow - Declaration]
// ============================================================================

struct SvgWindow : public UIEngineWindow
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgWindow(UIEngine* engine, uint32_t hints = 0);
  virtual ~SvgWindow();

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  virtual void onEngineEvent(UIEngineEvent* ev);
  virtual void onPaint(Painter* p);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  SvgDocument svgDocument;

  float fpsTotal;
  float fpsCounter;
  Time fpsTime;
};

// ============================================================================
// [SvgWindow - Construction / Destruction]
// ============================================================================

SvgWindow::SvgWindow(UIEngine* engine, uint32_t hints) :
  UIEngineWindow(engine, hints)
{
}

SvgWindow::~SvgWindow()
{
}

// ============================================================================
// [SvgWindow - Event Handlers]
// ============================================================================

void SvgWindow::onEngineEvent(UIEngineEvent* ev)
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

void SvgWindow::onPaint(Painter* p)
{
  Time startTime = Time::now();

  p->setSource(Argb32(0xFFFFFFFF));
  p->fillAll();

  p->save();
  svgDocument.render(p);
  p->restore();

  p->flush(PAINTER_FLUSH_SYNC);

  Time endTime = Time::now();

  TimeDelta frameDelta = endTime - startTime;
  TimeDelta fpsDelta = endTime - fpsTime;

  if (fpsDelta.getMillisecondsD() >= 1000.0)
  {
    fpsTotal = fpsCounter;
    fpsCounter = 0.0f;
    fpsTime = endTime;

    StringW text;
    text.format("FPS: %g, Time: %g", fpsTotal, frameDelta.getMillisecondsD());
    setWindowTitle(text);
  }
  else
  {
    fpsCounter++;
  }

  p->resetTransform();
}

// ============================================================================
// [FOG_UI_MAIN]
// ============================================================================

FOG_UI_MAIN()
{
  Application app(StringW::fromAscii8("UI"));
  SvgWindow wnd(app.getUIEngine());

  List<StringW> arguments = Application::getApplicationArguments();
  StringW fileName;

  if (arguments.getLength() >= 2)
  {
    fileName = arguments.getAt(1);
  }
  else
  {
    // My testing images...
    //fileName = Ascii8("/my/upload/img/svg/tiger.svg");
    //fileName = Ascii8("/my/upload/img/svg/map-krasnaya-plyana.svg");
    //fileName = Ascii8("/my/upload/img/svg/Map_Multilayer_Scaled.svg");

    //fileName = Ascii8("C:/my/svg/map-krasnaya-plyana.svg");
    fileName = Ascii8("C:/my/svg/map-imeretinka.svg");
    //fileName = Ascii8("C:/my/svg/Map_Multilayer_Scaled.svg");
    //fileName = Ascii8("C:/my/svg/froggy.svg");
    //fileName = Ascii8("C:/my/svg/fire_engine.svg");
    //fileName = Ascii8("C:/my/svg/tommek_Car.svg");
    //fileName = Ascii8("C:/my/svg/TestFOGFeatures.svg");
    //fileName = Ascii8("C:/My/svg/linear3.svg");
    //fileName = Ascii8("C:/my/svg/ISO_12233-reschart.svg");
    //fileName = Ascii8("C:/my/svg/lorem_ipsum_compound.svg");
    //fileName = Ascii8("C:/my/svg/tiger.svg");
    //fileName = Ascii8("C:/my/svg/lion.svg");
    //fileName = Ascii8("C:/my/svg/Minimap_fixed.svg");
    //fileName = Ascii8("C:/my/svg/path-lines-BE-01.svg");
    //fileName = Ascii8("C:/my/svg/gradPatt-linearGr-BE-01.svg");
    //fileName = Ascii8("C:/my/svg/brown_fish_01.svg");
    //fileName = Ascii8("C:/my/svg/pattern.svg");
    //fileName = Ascii8("C:/my/svg/paint-fill-BE-01.svg");

    //fileName = Ascii8("C:/my/svg/jean_victor_balin_check.svg");
    //fileName = Ascii8("C:/my/svg/PatternTest.svg");
    //fileName = Ascii8("C:/my/svg/Denis - map_v.0.2.svg");

    //fileName = Ascii8("/Users/petr/Workspace/Files/SVG/filters.svg");
    //fileName = Ascii8("/Users/petr/Workspace/Files/SVG/map-imeretinka.svg");
    //fileName = Ascii8("/Users/petr/Workspace/Files/SVG/PatternTest.svg");
    //fileName = Ascii8("/Users/petr/Workspace/Files/SVG/tiger.svg");
    //fileName = Ascii8("/Users/petr/Workspace/Files/SVG/gradient.svg");
    //fileName = Ascii8("/Users/petr/Workspace/Files/SVG/pservers-grad-13-b.svg");
    //fileName = Ascii8("/Users/petr/Workspace/Files/SVG/paths-data-06-t.svg");
    //fileName = Ascii8("/Users/petr/Workspace/Files/SVG/EiffelTower.svg");
    //fileName = Ascii8("/Users/petr/Workspace/Files/SVG/svg-cards.svg");
    //fileName = Ascii8("/Users/petr/Workspace/Files/SVG/turbulence_filters.svg");
  }

/*
  {
    TimeTicks start = TimeTicks::now();
    SvgDocument svg[1000];
    fprintf(stderr, "CREATE\n");
    for (size_t i = 0; i < 1000; i++)
      svg[i].readFromFile(fileName);
    
    fprintf(stderr, "DONE: Time %g\n", (TimeTicks::now() - start).getMillisecondsF());
    fprintf(stderr, "Mem: allocated=%d used=%d\n", (int)svg[0]._gc.getAllocatedMemory(), (int)svg[0]._gc.getUsedMemory());
    fprintf(stderr, "DESTROY\n");
  }
  fprintf(stderr, "DESTROYED\n");
*/

  err_t err = wnd.svgDocument.readFromFile(fileName);
  SizeF size = wnd.svgDocument.getDocumentSize();

  if (size.w < 200) size.w = 200;
  if (size.h < 200) size.h = 200;

  if (size.w > 900) size.w = 900;
  if (size.h > 720) size.h = 720;

  wnd.setWindowSize(SizeI((int)size.w, (int)size.h));
  wnd.show();

  return app.run();
}
