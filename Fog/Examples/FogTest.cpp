#include <Fog/Core.h>
#include <Fog/Graphics.h>
#include <Fog/Gui.h>
#include <Fog/Svg.h>
#include <Fog/Xml.h>

// This is for MY testing:)

using namespace Fog;

struct MyWindow : public Window
{
  // [Fog Object System]
  FOG_DECLARE_OBJECT(MyWindow, Window)

  // [Construction / Destruction]
  MyWindow(uint32_t createFlags = 0);
  virtual ~MyWindow();

  // [Event Handlers]
  virtual void onConfigure(ConfigureEvent* e);
  virtual void onKeyPress(KeyEvent* e);
  virtual void onPaint(PaintEvent* e);

  void paintImage(Painter* painter, const IntPoint& pos, const Image& im, const String& name);

  Button* button;

  Image i[2];
  double _subx;
  double _suby;
  double _rotate;
  double _shearX;
  double _shearY;
  double _scale;
  int _spread;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  setWindowTitle(Ascii8("Testing..."));

  i[0].readFromFile(Ascii8("babelfish.png"));
  i[1].readFromFile(Ascii8("kweather.png"));

  i[0].convert(IMAGE_FORMAT_PRGB32);
  i[1].convert(IMAGE_FORMAT_PRGB32);

  _subx = 0.0;
  _suby = 0.0;
  _rotate = 0.0;
  _shearX = 0.0;
  _shearY = 0.0;
  _scale = 1.0;
  _spread = PATTERN_SPREAD_REPEAT;

  button = new Button();
  add(button);
  button->setGeometry(IntRect(40, 40, 100, 20));
  button->setText(Ascii8("Push me"));
  button->show();
}

MyWindow::~MyWindow()
{
}

void MyWindow::onConfigure(ConfigureEvent* e)
{
  button->setGeometry(IntRect(getWidth() - 120, getHeight() - 40, 100, 20));
}

void MyWindow::onKeyPress(KeyEvent* e)
{
  switch (e->getKey())
  {
    case KEY_LEFT:
      _subx -= 0.05;
      break;
    case KEY_RIGHT:
      _subx += 0.05;
      break;
    case KEY_UP:
      _suby -= 0.05;
      break;
    case KEY_DOWN:
      _suby += 0.05;
      break;
    case KEY_LEFT | KEY_CTRL:
      _subx -= 5;
      break;
    case KEY_RIGHT | KEY_CTRL:
      _subx += 5;
      break;
    case KEY_UP | KEY_CTRL:
      _suby -= 5;
      break;
    case KEY_DOWN | KEY_CTRL:
      _suby += 5;
      break;
    case KEY_Q:
      _rotate -= 0.005;
      break;
    case KEY_W:
      _rotate += 0.005;
      break;
    case KEY_E:
      _shearX -= 0.01;
      break;
    case KEY_R:
      _shearX += 0.01;
      break;
    case KEY_T:
      _shearY -= 0.01;
      break;
    case KEY_Y:
      _shearY += 0.01;
      break;
    case KEY_A:
      _scale -= 0.01;
      break;
    case KEY_S:
      _scale += 0.01;
      break;
    case KEY_SPACE:
      if (++_spread >= PATTERN_SPREAD_COUNT) _spread = 0;
      break;
  }

  repaint(WIDGET_REPAINT_AREA);
  base::onKeyPress(e);
}

void MyWindow::onPaint(PaintEvent* e)
{
  TimeTicks ticks = TimeTicks::highResNow();
  Painter* p = e->getPainter();

  p->save();
  p->setOperator(OPERATOR_SRC);

  // Clear everything to white.
  p->setSource(Argb(0xFFFFFFFF));
  p->fillAll();

  p->setOperator(OPERATOR_SRC_OVER);

  {
    //Image aa = i[0].scale(IntSize(320, 320));
    //p->drawImage(IntPoint(100, 100), aa);

    //p->drawImage(DoubleRect(100, 100, 320, 320), i[0]);
    //p->drawImage(DoubleRect(130, 130, 320, 320), i[0]);
    //p->drawImage(DoubleRect(160, 160, 320, 320), i[0]);
    //p->drawImage(DoubleRect(190, 190, 320, 320), i[0]);
    //p->drawImage(DoublePoint(100, 100), i[0]);
  }

  p->restore();
  p->flush(PAINTER_FLUSH_SYNC);

  TimeDelta delta = TimeTicks::highResNow() - ticks;

  String s;
  s.format("Size: %d %d, time %g, [PARAMS: %g %g]", getWidth(), getHeight(), delta.inMillisecondsF(), _subx, _suby);
  Font font = getFont();

  p->setOperator(OPERATOR_SRC_OVER);
  p->setSource(0xFF000000);
  p->fillRect(IntRect(0, 0, getWidth(), font.getHeight()));
  p->setSource(0xFFFF0000);
  p->drawText(IntPoint(0, 0), s, font);
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
  window.setSize(IntSize(500, 400));

  window.addListener(EVENT_CLOSE, &app, &Application::quit);
  window.show();

  return app.run();
}
