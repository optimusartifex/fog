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
  virtual void onKey(KeyEvent* e);
  virtual void onPaint(PaintEvent* e);

  void paintImage(Painter* painter, const IntPoint& pos, const Image& im, const String& name);

  Image i[2];
  int _opacity;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  setWindowTitle(Ascii8("Fog - Composite"));

  i[0].readFromFile(Ascii8("babelfish.png"));
  i[1].readFromFile(Ascii8("kweather.png"));

  i[0].premultiply();
  i[1].premultiply();

  _opacity = 255;
}

MyWindow::~MyWindow()
{
}

void MyWindow::onKey(KeyEvent* e)
{
  if (e->getCode() == EVENT_KEY_PRESS)
  {
    switch (e->getKey())
    {
      case KEY_Q:
        _opacity = Math::max(_opacity - 1, 0);
        break;
      case KEY_W:
        _opacity = Math::min(_opacity + 1, 255);
        break;
    }
  }

  update(WIDGET_UPDATE_PAINT);
  base::onKey(e);
}

void MyWindow::onPaint(PaintEvent* e)
{
  Painter* p = e->getPainter();

  p->setOperator(OPERATOR_SRC);
  p->setSource(0xFF000000);
  p->fillAll();

  p->setOperator(OPERATOR_SRC_OVER);
  p->setSource(0xFFFFFFFF);

  const char* opname[OPERATOR_COUNT];

  opname[OPERATOR_SRC       ] = "src";
  opname[OPERATOR_DST       ] = "dst";
  opname[OPERATOR_SRC_OVER  ] = "src-over";
  opname[OPERATOR_DST_OVER  ] = "dst-over";
  opname[OPERATOR_SRC_IN    ] = "src-in";
  opname[OPERATOR_DST_IN    ] = "dst-in";
  opname[OPERATOR_SRC_OUT   ] = "src-out";
  opname[OPERATOR_DST_OUT   ] = "dst-out";
  opname[OPERATOR_SRC_ATOP  ] = "src-atop";
  opname[OPERATOR_DST_ATOP  ] = "dst-atop";
  opname[OPERATOR_XOR       ] = "xor";
  opname[OPERATOR_CLEAR     ] = "clear";
  opname[OPERATOR_ADD       ] = "add";
  opname[OPERATOR_SUBTRACT  ] = "subtract";
  opname[OPERATOR_MULTIPLY  ] = "multiply";
  opname[OPERATOR_SCREEN    ] = "screen";
  opname[OPERATOR_DARKEN    ] = "darken";
  opname[OPERATOR_LIGHTEN   ] = "lighten";
  opname[OPERATOR_DIFFERENCE] = "difference";
  opname[OPERATOR_EXCLUSION ] = "exclusion";
  opname[OPERATOR_INVERT    ] = "invert";
  opname[OPERATOR_INVERT_RGB] = "invert-rgb";

  int x = 0, y = 0;
  for (int a = 0; a < OPERATOR_COUNT; a++)
  {
    Image im(i[1]);
    im.drawImage(IntPoint(0, 0), i[0], a, _opacity);
    paintImage(p, IntPoint(x, y), im, Ascii8(opname[a]));

    if (++x == 6) { x = 0; y++; }
  }
}

void MyWindow::paintImage(Painter* p, const IntPoint& pos, const Image& im, const String& name)
{
  int x = 10 + pos.x * 152;
  int y = 10 + pos.y * 152;

  p->drawText(IntRect(x, y, 130, 20), name, getFont(), TEXT_ALIGN_CENTER);
  p->drawRect(IntRect(x, y + 20, 130, 130));
  p->drawImage(IntPoint(x + 1, y + 21), im);
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_GUI_MAIN()
{
  Application app(Ascii8("Gui"));

  MyWindow window;
  window.setSize(IntSize(20 + 6 * 152 - 22, 20 + 4 * 152));
  window.show();
  window.addListener(EVENT_CLOSE, &app, &Application::quit);

  return app.run();
}
