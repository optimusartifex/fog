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
  virtual void onKeyPress(KeyEvent* e);
  virtual void onPaint(PaintEvent* e);
  virtual void onConfigure(ConfigureEvent* e) {
    uint32_t vis = getVisibility();

    if(vis == WIDGET_VISIBLE) {
      setWindowTitle(Ascii8("STATE: VISIBLE"));
    } else if(vis == WIDGET_VISIBLE_MAXIMIZED){
      setWindowTitle(Ascii8("STATE: MAXIMIZED"));
    } else if(vis == WIDGET_VISIBLE_MINIMIZED){
      setWindowTitle(Ascii8("STATE: MINIMIZED"));
    } else if(vis == WIDGET_HIDDEN){
      setWindowTitle(Ascii8("STATE: HIDDEN"));
    }    
  }

  void onButtonClick(MouseEvent* e) {
//     if(getWindowFlags() & WINDOW_NATIVE) {
//       setWindowFlags(WINDOW_TYPE_TOOL);
//     } else {
//       setWindowFlags(WINDOW_TYPE_DEFAULT);
//     }
    show(WIDGET_VISIBLE_MAXIMIZED);
  }

  void paintImage(Painter* painter, const IntPoint& pos, const Image& im, const String& name);

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

  i[0].readFile(Ascii8("babelfish.png"));
  i[1].readFile(Ascii8("kweather.png"));

  i[0].convert(PIXEL_FORMAT_ARGB32);
  i[1].convert(PIXEL_FORMAT_ARGB32);

/*
  {
    Painter p(i[0]);

    p.setSource(0xFFFFFFFF);
    p.setOperator(OPERATOR_DST_OVER);
    p.clear();
  }
*/
  // i[0].readFile(Ascii8("texture0.bmp"));
  //i[0].readFile(Ascii8("/my/upload/bmpsuite/g04.bmp"));
  //i[0].readFile(Ascii8("/my/upload/bmpsuite/icons_fullset.png"));

  //i[0] = i[0].scale(Size(32, 32), INTERPOLATION_SMOOTH);

  _subx = 0.0;
  _suby = 0.0;
  _rotate = 0.0;
  _shearX = 0.0;
  _shearY = 0.0;
  _scale = 1.0;
  _spread = SPREAD_REPEAT;

  Button* button = new Button();
  add(button);
  button->setGeometry(IntRect(40, 40, 100, 20));
  button->setText(Ascii8("Push me"));
  button->show();

  button->addListener(EVENT_CLICK, this, &MyWindow::onButtonClick);
}

MyWindow::~MyWindow()
{
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
      if (++_spread >= SPREAD_COUNT) _spread = 0;
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
  p->clear();
  
  p->setSource(Argb(0xFF000000));
  p->drawRect(IntRect(15, 15, getWidth() - 25, getHeight() - 25));

/*
  Region reg;
  {
    for (int y = 0; y < getHeight(); y += 10)
    {
      for (int x = (y % 20) == 10 ? 10 : 0; x < getWidth(); x += 20)
      {
        reg.unite(Rect(x, y, 10, 10));
      }
    }
  }
  p->setUserRegion(reg);

  p->setSource(Argb(0xFF00007F));
  p->clear();
*/

#if 1

  // These coordinates describe boundary of object we want to paint.
  double x = 40.5;
  double y = 40.5;
  double w = 300.0;
  double h = 300.0;

  p->translate(_subx, _suby);

  // Create path that will contain rounded rectangle.
  DoublePath path;
  path.moveTo(100, 100);
  path.cubicTo(150, 120, 180, 100, 200, 200);
  path.lineTo(50, 230);
  path.lineTo(140, 280);

  // Create linear gradient pattern.
  Pattern pattern;
  pattern.setType(PATTERN_LINEAR_GRADIENT);
  pattern.setPoints(DoublePoint(x, y), DoublePoint(x + w, y + h));
  pattern.addStop(ArgbStop(0.0, Argb(0xFF000000)));
  pattern.addStop(ArgbStop(1.0, Argb(0xFFFF0000)));

  p->translate((double)getWidth()/2, (double)getHeight()/2);
  p->rotate(_rotate);
  p->translate(-(double)getWidth()/2, -(double)getHeight()/2);

  // Fill path with linear gradient we created.
  p->setSource(pattern);
  p->fillPath(path);

  // Stroke path using solid black color.
  p->setSource(Argb(0xFF000000));
  p->setLineWidth(2);
  p->drawPath(path);

  p->setOperator(OPERATOR_SRC_OVER);
  //p->translate(100, 100);
  //p->translate(-100, -100);
  {
    Image im(i[0]);
    ColorMatrix cm;
    cm.rotateHue((float)_rotate * 3.0f);
    im.filter(cm);
    p->blitImage(DoublePoint(50.0, 50.0), im);
    p->blitImage(DoublePoint(250.0, 50.0), i[0]);
  }
#endif

  p->restore();
  p->flush(PAINTER_FLUSH_SYNC);

  TimeDelta delta = TimeTicks::highResNow() - ticks;

  p->setOperator(OPERATOR_SRC_OVER);
  p->setSource(0xFF000000);
  p->fillRect(IntRect(0, 0, getWidth(), getFont().getHeight()));
  p->setSource(0xFFFF0000);

  String s;
  s.format("Size: %d %d, time %g, [PARAMS: %g %g]", getWidth(), getHeight(), delta.inMillisecondsF(), _subx, _suby);
  p->drawText(IntPoint(0, 0), s, getFont());
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
  window.setSize(IntSize(500, 400));

  window.addListener(EVENT_CLOSE, &app, &Application::quit);
  window.show();

  return app.run();
}
