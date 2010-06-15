#include <Fog/Core.h>
#include <Fog/Graphics.h>
#include <Fog/Gui.h>

using namespace Fog;

struct MyPoint
{
  double posx;
  double posy;

  double stepx;
  double stepy;

  void setPos(double x, double y) { posx = x; posy = y; }
  void setStep(double x, double y) { stepx = x; stepy = y; }

  void randomize(int w, int h)
  {
    posx = rand() % w;
    posy = rand() % h;

    stepx = (double)(rand() % 300) / 150.0 + 1.0;
    stepy = (double)(rand() % 300) / 150.0 + 1.0;

    if (rand() % 1000 > 500) stepx = -stepx;
    if (rand() % 1000 > 500) stepy = -stepy;
  }

  void move(const DoubleRect& bounds)
  {
    posx += stepx;
    posy += stepy;

    if (posx >= bounds.getX2() || posx <= bounds.getX1()) { stepx = -stepx; posx = Math::bound(posx, bounds.getX1(), bounds.getX2()); }
    if (posy >= bounds.getY2() || posy <= bounds.getY1()) { stepy = -stepy; posy = Math::bound(posy, bounds.getY1(), bounds.getY2()); }
  }
};

struct MyColor
{
  double c[3];
  double step[3];

  void randomize()
  {
    for (int i = 0; i < 3; i++)
    {
      c[i] = (double)(rand() % 255);
      step[i] = (double)(2.5 - (double)(rand() % 100) / 20.0);
    }
  }

  void move()
  {
    for (int i = 0; i < 3; i++)
    {
      c[i] += step[i];
      if (c[i] > 255.0) { c[i] = 255.0; step[i] = -step[i]; }
      if (c[i] < 0.0)   { c[i] = 0.0; step[i] = -step[i]; }

      if ((rand() % 255) == 0) step[i] = (double)(5.0 - (rand() % 100) / 20.0);
    }
  }
  Argb toArgb() { return Argb(0xFF, (int)c[0], (int)c[1], (int)c[2]); }
};

struct MyWindow : public Window
{
  // [Fog Object System]
  FOG_DECLARE_OBJECT(MyWindow, Window)

  // [Construction / Destruction]
  MyWindow(uint32_t createFlags = 0);
  virtual ~MyWindow();

  // [Event Handlers]
  virtual void onPaint(PaintEvent* e);
  virtual void onTimer(TimerEvent* e);

  // [Members]
  Timer timer;

  enum { NumPoints = 4 };
  MyPoint mp[NumPoints];
  MyColor clr;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  setWindowTitle(Ascii8("Testing..."));

  for (sysuint_t i = 0; i < NumPoints; i++) mp[i].randomize(200, 200);
  clr.randomize();

  timer.setInterval(TimeDelta::fromMilliseconds(20));
  timer.addListener(EVENT_TIMER, this, &MyWindow::onTimer);
  timer.start();
}

MyWindow::~MyWindow()
{
}

void MyWindow::onPaint(PaintEvent* e)
{
  Painter* p = e->getPainter();

  DoublePath path;

  for (sysuint_t i = 0; i < NumPoints; i += 2)
  {
    if (i == 0)
      path.moveTo(mp[i].posx, mp[i].posy);
    else
      path.curveTo(mp[i].posx, mp[i].posy, mp[i+1].posx, mp[i+1].posy);
  }

  path.curveTo(mp[NumPoints-1].posx, mp[NumPoints-1].posy, mp[0].posx, mp[0].posy);
  path.closePolygon();

  p->setOperator(OPERATOR_SUBTRACT);
  p->setSource(0x01FFFFFF);
  p->fillRect(IntRect(0, 0, getWidth(), getHeight()));

  p->setSource((clr.toArgb() & 0x00FFFFFF) | 0x0F000000);
  p->setOperator(OPERATOR_SCREEN);
  p->setFillRule(FILL_EVEN_ODD);
  p->setLineWidth(3.0);
  p->drawPath(path);
}

void MyWindow::onTimer(TimerEvent* e)
{
  double w = getWidth(), h = getHeight();
  DoubleRect bounds(0.0, 0.0, w, h);

  for (sysuint_t i = 0; i < NumPoints; i++) mp[i].move(bounds);
  clr.move();

  update(WIDGET_UPDATE_PAINT);
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_GUI_MAIN()
{
  Application app(Ascii8("Gui"));

  MyWindow window;
  window.setSize(IntSize(515, 455));
  window.show();
  window.addListener(EVENT_CLOSE, &app, &Application::quit);

  return app.run();
}
