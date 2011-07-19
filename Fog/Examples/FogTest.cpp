#include <Fog/Core.h>
#include <Fog/G2d.h>
#include <Fog/Gui.h>
#include <Fog/Svg.h>

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
  virtual void onKey(KeyEvent* e);
  virtual void onMouse(MouseEvent* e);
  virtual void onTimer(TimerEvent* e);
  virtual void onPaint(PaintEvent* e);

  void paintImage(Painter* painter, const PointI& pos, const Image& im, const String& name);

  Image i[2];
  LinearGradientF linear;
  RadialGradientF radial;

  float _subx;
  float _suby;
  float _rotate;
  float _shearX;
  float _shearY;
  float _scale;
  int _spread;
  bool _clip;

  // FPS...
  Timer timer;
  float fps;
  float fpsCounter;
  Time fpsTime;

  PointF _lastMousePoint;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  timer.setInterval(TimeDelta::fromMilliseconds(2));
  timer.addListener(EVENT_TIMER, this, &MyWindow::onTimer);
  fps = 0.0f;
  fpsCounter = 0.0f;

  setWindowTitle(Ascii8("Testing..."));

  i[0].readFromFile(Ascii8("C:/My/Devel/Sprites/babelfish.png"));
  i[1].readFromFile(Ascii8("C:/My/Devel/Sprites/kweather.png"));

  _subx = 0.0f;
  _suby = 0.0f;
  _rotate = 0.0f;
  _shearX = 0.0f;
  _shearY = 0.0f;
  _scale = 1.0f;
  _spread = GRADIENT_SPREAD_REPEAT;

  _clip = false;
  //testVBoxLayout();
  //testFrame();
  //setContentRightMargin(0);

  linear.addStop(ColorStop(0.00f, Argb32(0xFFFFFFFF)));
  linear.addStop(ColorStop(0.33f, Argb32(0xFFFFFF00)));
  linear.addStop(ColorStop(0.66f, Argb32(0xFFFF0000)));
  linear.addStop(ColorStop(1.00f, Argb32(0xFF000000)));
  radial.setStops(linear.getStops());

  _lastMousePoint.set(250, 250);
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
      case KEY_SPACE:
        if (timer.isRunning())
        {
          timer.stop();
        }
        else
        {
          timer.start();
          fps = 0.0f;
          fpsCounter = 0.0f;
          fpsTime = Time::now();
        }
        break;

      case KEY_UP   : _suby -= 0.2f; update(WIDGET_UPDATE_ALL); break;
      case KEY_DOWN : _suby += 0.2f; update(WIDGET_UPDATE_ALL); break;
      case KEY_LEFT : _subx -= 0.2f; update(WIDGET_UPDATE_ALL); break;
      case KEY_RIGHT: _subx += 0.2f; update(WIDGET_UPDATE_ALL); break;

      case KEY_U: _suby -= 10.0f; update(WIDGET_UPDATE_ALL); break;
      case KEY_J: _suby += 10.0f; update(WIDGET_UPDATE_ALL); break;
      case KEY_H: _subx -= 10.0f; update(WIDGET_UPDATE_ALL); break;
      case KEY_K: _subx += 10.0f; update(WIDGET_UPDATE_ALL); break;

      case KEY_Q: _rotate -= 0.05f; update(WIDGET_UPDATE_ALL); break;
      case KEY_W: _rotate += 0.05f; update(WIDGET_UPDATE_ALL); break;

      case KEY_A: _shearX -= 0.05f; update(WIDGET_UPDATE_ALL); break;
      case KEY_S: _shearX += 0.05f; update(WIDGET_UPDATE_ALL); break;

      case KEY_Z: _shearY -= 0.05f; update(WIDGET_UPDATE_ALL); break;
      case KEY_X: _shearY += 0.05f; update(WIDGET_UPDATE_ALL); break;

      case KEY_C: _clip = !_clip; update(WIDGET_UPDATE_ALL); break;

      case KEY_ENTER: update(WIDGET_UPDATE_ALL); break;
    }
  }

  base::onKey(e);
}

void MyWindow::onMouse(MouseEvent* e)
{
  if (e->getCode() == EVENT_MOUSE_PRESS)
  {
    if (e->getButton() == BUTTON_LEFT)
    {
      _lastMousePoint = e->getPosition();
      update(WIDGET_UPDATE_ALL);
    }
  }

  base::onMouse(e);
}

void MyWindow::onTimer(TimerEvent* e)
{
  _rotate += 0.005f;
  update(WIDGET_UPDATE_PAINT);
}

void MyWindow::onPaint(PaintEvent* e)
{
  Time startTime = Time::now();
  Painter* p = e->getPainter();

  p->setSource(Argb32(0xFFFFFFFF));
  p->fillAll();

#if 1
  PieD pie(PointD(0, 0), PointD(200, 200), 0.0, MATH_ONE_HALF_PI);
  BoxD box;

  TransformD tr;
  //tr.skew(PointF(1.0f, 0.2f));
  tr.setQuadToQuad(PointD(200, 200), PointD(500, 300), PointD(400, 500), PointD(250, 500), BoxD(-300, -300, 0, 0));
  tr.rotate(_rotate);
  tr.translate(PointD(100, 100));

  p->save();
  p->transform(tr);

  p->setSource(Argb32(0xFF000000));
  //p->fillPie(pie);

  if (pie.getBoundingBox(box) == ERR_OK)
  {
    p->setSource(Argb32(0x8F0000FF));
    p->setLineWidth(1.0);
    p->fillBox(box);
  }

  linear.setStart(PointF(0, -200));
  linear.setEnd(PointF(0, 200));
  p->setSource(linear);
  p->fillPie(pie);

  p->setSource(Argb32(0xFF00FFFF));
  p->setLineWidth(1.0);
  p->drawPie(pie);

  p->restore();

  /*
  {
    uint i, j;
    p->setLineWidth(1.5f);

    for (i = 10; i < 100; i++)
    {
      PathF p0;

      for (j = 0; j < i; j++)
      {
        PointF pt0((rand() % 1000) - 200, (rand() % 1000) - 200);
        PointF pt1((rand() % 1000) - 200, (rand() % 1000) - 200);
        PointF pt2((rand() % 1000) - 200, (rand() % 1000) - 200);

        if (j == 0)
          p0.moveTo(pt0);
        else
          p0.cubicTo(pt0, pt1, pt2);
      }

      p->setSource(Argb32(0xFFFF0000));
      p->fillPath(p0);
      p->setSource(Argb32(0xFF000000));
      p->drawPath(p0);
    }
  }
  */

  if (pie.getBoundingBox(box, tr) == ERR_OK)
  {
    p->setSource(Argb32(0xFF000000));
    p->drawBox(box);
  }
#endif

  // --------------------------------------------------------------------------
  Time lastTime = Time::now();

  TimeDelta frameDelta = lastTime - startTime;
  TimeDelta fpsDelta = lastTime - fpsTime;

  if (fpsDelta.getMillisecondsD() >= 1000.0f)
  {
    fps = fpsCounter;
    fpsCounter = 0.0f;
    fpsTime = lastTime;
  }
  else
  {
    fpsCounter++;
  }

  String text;
  text.format("FPS: %g, Time: %g", fps, frameDelta.getMillisecondsD());

  {
    PathF path;
    Font font = getFont();
    font.getTextOutline(path, PointF(30, 30), text);
    p->fillPath(path);
  }

  setWindowTitle(text);
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_UI_MAIN()
{
  Application app(Ascii8("Gui"));
  MyWindow window(WINDOW_TYPE_DEFAULT);

  window.addListener(EVENT_CLOSE, &app, &Application::quit);
  window.setSize(SizeI(1000, 700));
  window.show();

  return app.run();
}
