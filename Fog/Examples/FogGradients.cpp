#include <Fog/Core.h>
#include <Fog/Graphics.h>
#include <Fog/Gui.h>

using namespace Fog;

struct MyWindow : public Window
{
  // [Fog Object System]
  FOG_DECLARE_OBJECT(MyWindow, Window)

  // [Construction / Destruction]
  MyWindow(uint32_t createFlags = 0);
  virtual ~MyWindow();

  // [Event Handlers]
  virtual void onPaint(PaintEvent* e);

  // Helper to paint pattern in rect with title.
  void paintPattern(Painter* p, const IntPoint& pos, const DoubleSize& size, const Pattern& pattern, const String& name);
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  setWindowTitle(Ascii8("Fog - Gradients"));
}

MyWindow::~MyWindow()
{
}

void MyWindow::onPaint(PaintEvent* e)
{
  static const char* spread_names[] = { "none", "pad", "repeat", "reflect" };

  Painter* p = e->getPainter();

  p->setOperator(OPERATOR_SRC);
  p->setSource(0xFF000000);
  p->clear();

  p->setOperator(OPERATOR_SRC_OVER);

  Pattern pat;
  DoubleSize size(128.0, 128.0);
  String s;

  pat.setType(PATTERN_LINEAR_GRADIENT);
  pat.addStop(ArgbStop(0.0, 0xFFFF0000));
  pat.addStop(ArgbStop(0.5, 0xFFFFFF00));
  pat.addStop(ArgbStop(1.0, 0xFF000000));

  for (int y = 0; y < 4; y++)
  {
    pat.setType(PATTERN_LINEAR_GRADIENT);
    pat.setSpread(y);

    s.format("Linear %s", spread_names[y]);

    pat.setPoints(DoublePoint(48.0, 48.0), DoublePoint(80.0, 80.0));
    paintPattern(p, IntPoint(0, y), size, pat, s);

    pat.setPoints(DoublePoint(64.0, 40.0), DoublePoint(64.0, 80.0));
    paintPattern(p, IntPoint(1, y), size, pat, s);

    pat.setType(PATTERN_RADIAL_GRADIENT);
    s.format("Radial %s", spread_names[y]);

    pat.setPoints(DoublePoint(64.0, 64.0), DoublePoint(40.0, 40.0));
    pat.setRadius(40.0);
    paintPattern(p, IntPoint(2, y), size, pat, s);

    pat.setPoints(DoublePoint(64.0, 64.0), DoublePoint(20.0, 20.0));
    pat.setRadius(40.0);
    paintPattern(p, IntPoint(3, y), size, pat, s);

    pat.setType(PATTERN_CONICAL_GRADIENT);
    s.format("Conical %s", spread_names[y]);

    pat.setPoints(DoublePoint(64.0, 64.0), DoublePoint(128.0, 64.0));
    pat.setRadius(40.0);
    paintPattern(p, IntPoint(4, y), size, pat, s);

  }
}

void MyWindow::paintPattern(Painter* p, const IntPoint& pos, const DoubleSize& size, const Pattern& pattern, const String& name)
{
  double x = 10 + pos.x * (size.w + 20.0);
  double y = 10 + pos.y * (size.h + 20.0);

  p->setSource(0xFFFFFFFF);
  p->drawText(IntRect((int)x, (int)y, (int)size.w, 20), name, getFont(), TEXT_ALIGN_CENTER);
  p->drawRect(IntRect((int)x, (int)y + 20, (int)size.w + 2, (int)size.h + 2));

  Pattern copy(pattern);
  copy.translate(x + 1.0, y + 21.0);
  p->setSource(copy);
  p->fillRect(IntRect((int)x + 1, (int)y + 21, (int)size.w, (int)size.h));
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_GUI_MAIN()
{
  Application app(Ascii8("Gui"));

  MyWindow window;
  window.setSize(IntSize(800, 640));
  window.show();
  window.addListener(EVENT_CLOSE, &app, &Application::quit);

  return app.run();
}
