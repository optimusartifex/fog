#include <Fog/Core.h>
#include <Fog/G2d.h>
#include <Fog/UI.h>

// ============================================================================
// [FogTest]
// ============================================================================

using namespace Fog;

struct MyWindow : public Window
{
  // [Fog Object System]
  FOG_DECLARE_OBJECT(MyWindow, Window)

  // [Construction / Destruction]
  MyWindow(uint32_t createFlags = 0);
  virtual ~MyWindow();

  // [Event Handlers]
  virtual void onKey(KeyEvent* e);
  virtual void onMouse(MouseEvent* e);
  virtual void onPaint(PaintEvent* e);

  // [Helpers]
  BoxI randomBox();
  void randomizeRegion(Region& dst);

  // [Members]
  Region a;
  Region b;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  setWindowTitle(StringW::fromAscii8("Region playground."));

  Font font = getFont();
  font.setHeight(20, UNIT_PX);
  setFont(font);

  a.union_(BoxI(100, 100, 200, 200));
  a.union_(BoxI(150, 200, 200, 250));

  b.union_(BoxI(100, 100, 125, 125));
  b.union_(BoxI(125, 125, 150, 150));
  b.union_(BoxI(150, 100, 175, 125));
  b.union_(BoxI(175, 125, 200, 150));
  b.union_(BoxI(200, 100, 225, 125));
  b.union_(BoxI(225, 125, 250, 150));
  b.union_(BoxI(250, 100, 275, 125));
  b.union_(BoxI(275, 125, 200, 150));
}

MyWindow::~MyWindow()
{
}

void MyWindow::onKey(KeyEvent* e)
{
  if (e->getCode() == EVENT_KEY_PRESS)
  {
    if (e->getKey() == KEY_P)
    {
      Region r;
      for (size_t i = 0; i < 10000; i++)
      {
        BoxI box(randomBox());
        r.union_(box);

        if ((i % 10) == 0)
          r.clear();
      }
    }

    if (e->getKey() == KEY_A)
    {
      a.union_(randomBox());
      update(WIDGET_UPDATE_ALL);
    }

    if (e->getKey() == KEY_B)
    {
      b.union_(randomBox());
      update(WIDGET_UPDATE_ALL);
    }

    if (e->getKey() == KEY_Q)
    {
      a.intersect(b);
      b.clear();
      update(WIDGET_UPDATE_ALL);
    }

    if (e->getKey() == KEY_0)
    {
      Region::intersectAndClip(a, a, b, BoxI(150, 150, 350, 350));
      b.clear();
      update(WIDGET_UPDATE_ALL);
    }

    if (e->getKey() == KEY_W)
    {
      a.union_(b);
      b.clear();
      update(WIDGET_UPDATE_ALL);
    }

    if (e->getKey() == KEY_E)
    {
      a.xor_(b);
      b.clear();
      update(WIDGET_UPDATE_ALL);
    }

    if (e->getKey() == KEY_R)
    {
      a.subtract(b);
      b.clear();
      update(WIDGET_UPDATE_ALL);
    }

    if (e->getKey() == KEY_C)
    {
      a.clear();
      b.clear();
      update(WIDGET_UPDATE_ALL);
    }
  }

  base::onKey(e);
}

void MyWindow::onMouse(MouseEvent* e)
{
  if (e->getCode() == EVENT_MOUSE_PRESS && e->getButton() == BUTTON_LEFT)
  {
  }

  if (e->getCode() == EVENT_MOUSE_RELEASE && e->getButton() == BUTTON_LEFT)
  {
  }

  if (e->getCode() == EVENT_MOUSE_MOVE)
  {
  }

  base::onMouse(e);
}

void MyWindow::onPaint(PaintEvent* e)
{
  Painter* p = e->getPainter();

  p->setSource(Argb32(0xFFFFFFFF));
  p->fillAll();

  p->setSource(Argb32(0x5FFF0000));
  p->fillRegion(a);

  p->setSource(Argb32(0x5F0000FF));
  p->fillRegion(b);

  Region c;
  Region::intersect(c, a, b);
  p->setSource(Argb32(0xFF000000));
  p->fillRegion(c);
}

BoxI MyWindow::randomBox()
{
  int x0 = rand() % 900;
  int y0 = rand() % 600;
  int w = (rand() % 180) + 16;
  int h = (rand() % 180) + 16;

  return BoxI(x0, y0, x0+w, y0+h);
}

void MyWindow::randomizeRegion(Region& dst)
{
  dst.clear();

  for (size_t i = 0; i < 10; i++)
  {
    dst.union_(randomBox());
  }
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_UI_MAIN()
{
  Application app(StringW::fromAscii8("UI"));
  MyWindow window(WINDOW_TYPE_DEFAULT);

  window.addListener(EVENT_CLOSE, &app, &Application::quit);
  window.setSize(SizeI(400, 300));
  window.show();

  return app.run();
}
