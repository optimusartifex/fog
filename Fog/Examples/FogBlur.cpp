#include <Fog/Core.h>
#include <Fog/Graphics.h>
#include <Fog/Gui.h>
#include <Fog/Svg.h>
#include <Fog/Xml.h>

using namespace Fog;

struct MyWindow : public Window
{
  // [Fog Object System]
  FOG_DECLARE_OBJECT(MyWindow, Window)

  // [Construction / Destruction]
  MyWindow(uint32_t createFlags = 0);
  virtual ~MyWindow();

  // [Event Handlers]
  virtual void onKeyPress(KeyEvent* e);
  virtual void onPaint(PaintEvent* e);

  void paintImage(Painter* painter, const IntPoint& pos, const Image& im, const String& name);

  Image i[2];
  float _baseRadius;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  setWindowTitle(Ascii8("Testing..."));

  i[0].readFile(Ascii8("babelfish.png"));
  i[1].readFile(Ascii8("kweather.png"));

  _baseRadius = 1.0f;
}

MyWindow::~MyWindow()
{
}

void MyWindow::onKeyPress(KeyEvent* e)
{
  switch (e->getKey())
  {
    case KEY_Q:
      _baseRadius = Math::max<float>(_baseRadius - 1.0f, 0.0f);
      break;
    case KEY_W:
      _baseRadius = Math::min<float>(_baseRadius + 1.0f, 128.0f);
      break;
  }

  repaint(WIDGET_REPAINT_AREA);
  base::onKeyPress(e);
}

void MyWindow::onPaint(PaintEvent* e)
{
  Painter* p = e->getPainter();

  p->setOperator(OPERATOR_SRC);
  p->setSource(0xFF000000);
  p->fillAll();

  p->setOperator(OPERATOR_SRC_OVER);
  p->setSource(0xFFFFFFFF);

  int y = 0;
  int x;
  float radius;
  String s;

  for (x = 0, radius = _baseRadius; x < 3; x++, radius += 5.0f)
  {
    Image im(i[0]);
    im.filter(ImageFilter(BlurParams(IMAGE_FILTER_BLUR_BOX, radius, radius, IMAGE_FILTER_EXTEND_COLOR, 0x00000000)));
    s.format("Box blur %g", radius);
    paintImage(p, IntPoint(x, y), im, s);
  }

  y++;

  for (x = 0, radius = _baseRadius; x < 3; x++, radius += 5.0f)
  {
    Image im(i[0]);
    im.filter(ImageFilter(BlurParams(IMAGE_FILTER_BLUR_LINEAR, radius, radius, IMAGE_FILTER_EXTEND_COLOR, 0x00000000)));
    s.format("Linear blur %g", radius);
    paintImage(p, IntPoint(x, y), im, s);
  }

  y++;

  for (x = 0, radius = _baseRadius; x < 3; x++, radius += 5.0f)
  {
    Image im(i[0]);
    im.filter(ImageFilter(BlurParams(IMAGE_FILTER_BLUR_GAUSSIAN, radius, radius, IMAGE_FILTER_EXTEND_COLOR, 0x00000000)));
    s.format("Gaussian blur %g", radius);
    paintImage(p, IntPoint(x, y), im, s);
  }
}

void MyWindow::paintImage(Painter* p, const IntPoint& pos, const Image& im, const String& name)
{
  int x = 10 + pos.x * 152;
  int y = 10 + pos.y * 152;

  p->drawText(IntRect(x, y, 130, 20), name, getFont(), TEXT_ALIGN_CENTER);
  p->drawRect(IntRect(x, y + 20, 130, 130));
  p->blitImage(IntPoint(x + 1, y + 21), im);
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_GUI_MAIN()
{
  Application app(Ascii8("Gui"));

  MyWindow window;
  window.setSize(IntSize(20 + 3 * 152 - 22, 20 + 3 * 152));

  window.addListener(EVENT_CLOSE, &app, &Application::quit);
  window.show();

  return app.run();
}
