#include <Fog/Core.h>
#include <Fog/G2d.h>
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

  linear.addStop(ColorStop(0.00f, Argb32(0xFF0000FF)));
  linear.addStop(ColorStop(0.33f, Argb32(0xFFFFFF00)));
  linear.addStop(ColorStop(0.66f, Argb32(0xFFFF0000)));
  linear.addStop(ColorStop(1.00f, Argb32(0x00000000)));
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

  p->save();
  p->setCompositingOperator(COMPOSITE_SRC);
  /*
  p->translate(PointF(200, 200));
  p->rotate(_rotate);
  p->skew(PointF(_shearX, _shearY));
  p->translate(PointF(_subx, _suby));
  p->translate(PointF(-200, -200));
  */
  // Clear everything to white.
  p->setSource(Argb32(0xFFFFFFFF));
  p->fillAll();

  //p->setSource(Argb32(0xFF000000));

  linear.setStart(PointF(100.0f, 100.0f));
  linear.setEnd(PointF(_lastMousePoint));
  linear.setGradientSpread(GRADIENT_SPREAD_REFLECT);

  radial.setCenter(PointF(250.0f, 250.0f));
  radial.setFocal(_lastMousePoint);
  //radial.setFocal(PointF(250.0f, 250.0f));
  radial.setRadius(PointF(150.0f, 150.0f));
  radial.setGradientSpread(GRADIENT_SPREAD_REFLECT);

  //TransformF perspective;
  //perspective.setQuadToQuad(PointF(100, 100), PointF(400, 100), PointF(300, 400), PointF(150, 400), BoxF(100, 100, 400, 400));
  //p->transform(perspective);

  //p->setSource(radial);
  //p->fillAll();

  //p->fillCircle(CircleF(PointF(310.0f, 310.0f), 300.0f));
  //p->fillBox(BoxI(50, 50, 450, 450));

  p->setCompositingOperator(COMPOSITE_SRC_OVER);
  p->setSource(Argb32(0x80FFFFFF));
  //p->drawBox(BoxI(100, 100, 400, 400));

  //p->setOpacity(0.5f);
  /*{
    //ConicalGradientF gr(PointF(200, 200), MATH_PI);
    //gr.setStops(linear.getStops());
    //p->setSource(gr);

    RectangularGradientF gr(PointF(200, 200), PointF(400, 400), _lastMousePoint);
    gr.setStops(linear.getStops());
    gr.setGradientSpread(GRADIENT_SPREAD_PAD);
    p->setSource(gr);
  }

  p->fillAll();*/
  //p->setSource(radial);
  //p->setSource(Texture(i[0], TEXTURE_TILE_CLAMP, Color(Argb32(0xFFFF0000))));
  //p->fillBox(BoxF(0.0f, 0.0f, 400.0f, 400.0f));
  //p->fillBox(BoxF(200.0f, 200.0f, 400.0f, 400.0f));

  //p->setSource(Argb32(0x80FFFFFF));
  //p->drawBox(BoxF(0.0f, 0.0f, 400.0f, 400.0f));
  //p->fillCircle(CircleF(PointF(200, 200), 100.0f));

  //p->scale(PointF(2.0f, 2.0f));
  //p->blitImage(PointI(100, 100), i[0]);
  p->setSource(Argb32(0xFF000000));
  //p->fillRect(RectI(100, 100, 300, 300));

  //p->blitImage(RectI(100, 100, 100, 100), i[0]);

  {
    PathF path;

    path.moveTo(PointF(300, 300));
    //path.circle(CircleF(PointF(240.0f, 240.0f), 115.0f), PATH_DIRECTION_CW);
    //path.ellipse(EllipseF(PointF(320.0f, 320.0f), PointF(115.0f, 50.0f)), PATH_DIRECTION_CCW);

    srand(1);
    for (sysuint_t i = 0; i < 20; i++)
    {
      path.cubicTo(
        PointF(100 + rand() % 500, 100 + rand() % 500),
        PointF(100 + rand() % 500, 100 + rand() % 500),
        PointF(100 + rand() % 500, 100 + rand() % 500)
      );
      path.quadTo(
        PointF(100 + rand() % 500, 100 + rand() % 500),
        PointF(100 + rand() % 500, 100 + rand() % 500)
      );
    }
    /*
    for (sysuint_t i = 0; i < 40; i++)
    {
      if (i == 0)
        path.moveTo(PointF(100 + rand() % 400, 100 + rand() % 400));
      else
        path.lineTo(PointF(100 + rand() % 400, 100 + rand() % 400));
    }
    */

    p->setSource(Argb32(path.hitTest(_lastMousePoint, FILL_RULE_EVEN_ODD) ? 0xFFFF0000 : 0xFF000000));
    //p->setSource(Argb32(path.hitTest(PointF(_lastMousePoint.x + 0.01f, _lastMousePoint.y + 0.01f), FILL_RULE_EVEN_ODD) ? 0xFFFF0000 : 0xFF000000));
    p->setFillRule(FILL_RULE_EVEN_ODD);
    p->fillPath(path);
  }

  /*
  p->fillRect(RectI(10, 10, 200, 200));

  RectF rect(10.0f + _subx, 10.0f + _suby, 100.0f, 100.0f);

  if (_clip)
  {
    Region region;
    region.combine(RectI(  4,   4,  50,  60), REGION_OP_UNION);
    region.combine(RectI( 64,   4, 200,  60), REGION_OP_UNION);
    region.combine(RectI(  4,  70,  50, 140), REGION_OP_UNION);
    region.combine(RectI( 64,  70, 200, 140), REGION_OP_UNION);

    p->save();
    p->setFillColor(Argb32(0xFFCFCFFF));
    p->fillRegion(region);
    p->restore();
    p->clipRegion(CLIP_OP_REPLACE, region);
  }
  */

  //PathF path;
  //PathF clip;

  //static int seedNum = 13;
  //srand(seedNum++);

  //path.circle(CircleF(PointF(240.0f, 240.0f), 115.0f), PATH_DIRECTION_CW);
  //path.ellipse(EllipseF(PointF(320.0f, 320.0f), PointF(115.0f, 50.0f)), PATH_DIRECTION_CCW);
  //path._modifiedBoundingBox();


  //path.moveTo(PointF(200.0f, 100.0f));
  //path.cubicTo(PointF(400.0f, 250.0f),
  //  PointF(440.0f, 220.0f), PointF(200.0f, 400.0f));

  //path.lineTo(PointF(89.0f, 6.0f));
  //path.lineTo(PointF(222.0f, 240.0f));
  //path.lineTo(PointF(492.0f, 16.0f));
  // path.moveTo(PointF(100.0f, 100.0f));
  /*
  path.moveTo(PointF(200, 200));
  for (int i = 0; i < 300; i++)
  {
    PointF p1(rand() % 500, rand() % 400);
    PointF p2(rand() % 500, rand() % 400);
    PointF p3(rand() % 500, rand() % 400);

    path.ellipse(EllipseF(p1, p2));
    //path.cubicTo(p1, p2, p3);
  }
  */

/*
  path.moveTo(PointF(200.0f, 100.0f));
  path.lineTo(PointF(300.0f, 300.0f));
  path.lineTo(PointF(100.0f, 300.0f));
  path.lineTo(PointF(200.0f, 200.0f));
  path.lineTo(PointF(200.0f, 350.0f));
  path.lineTo(PointF(150.0f, 220.0f));
*/

  //p->fillPath(path);
  /*
  PathClipperF clipper(RectF(150.0f, 150.0f, 200.0f, 200.0f));
  clipper.clipPath(clip, path);

  p->rotate(_rotate);

  p->setSource(Argb32(0xFF0000FF));
  p->fillPath(path);

  p->setSource(Argb32(0xFFFF0000));
  p->fillPath(clip);
  */

  /*
  p->setSource(Argb32(0xFFFFFF00));
  p->fillEllipse(EllipseF(PointF(300, 300), PointF(150, 150)));

  p->setSource(Argb32(0xFF000000));
  p->fillEllipse(EllipseF(PointF(300, 300), PointF(135, 135)));
  */

  //p->setSource(Argb32(0xFFFFFF00));
  //p->fillRect(RectI(50, 50, 300, 300));
/*
  p->setOpacity(0.1f);
  p->fillRect(RectI(150, 150, 200, 200));
  p->resetOpacity();
*/
  //p->fillRect(rect);

  /*
  PathD path;
  path.moveTo(150, 100);
  path.lineTo(250, 105);
  path.lineTo(200, 200);
  path.lineTo(100, 200);
  p->setSource(Argb32(0xFF000000));
  p->fillPath(path);
  */

  p->restore();

  /*
  p->save();
  p->skew(PointF(1.05f, 1.05f));
  p->setStrokeColor(Argb32(0xFF000000));
  p->strokeRect(RectI(50, 50, 128, 128));
  p->blitImage(PointI(50, 50), i[0]);
  p->restore();
  */

  Time lastTime = Time::now();

  TimeDelta frameDelta = lastTime - startTime;
  TimeDelta fpsDelta = lastTime - fpsTime;

  if (fpsDelta.inMillisecondsF() >= 1000.0f)
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
  text.format("FPS: %g, Time: %g", fps, frameDelta.inMillisecondsF());
  setWindowTitle(text);
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_GUI_MAIN()
{
  Application app(Ascii8("Gui"));

  MyWindow window(WINDOW_TYPE_DEFAULT);

  window.addListener(EVENT_CLOSE, &app, &Application::quit);
  window.setSize(SizeI(1200, 800));
  window.show();

  return app.run();
}
