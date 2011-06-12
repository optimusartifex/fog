#include <Fog/Core.h>
#include <Fog/G2d.h>
#include <Fog/Gui.h>
#include <Fog/Svg.h>

using namespace Fog;

struct MyWindow : public Window
{
  // [Fog Object System]
  FOG_DECLARE_OBJECT(MyWindow, Window)

  // [Construction / Destruction]
  MyWindow(uint32_t createFlags = 0);
  virtual ~MyWindow();

  void recalcTransform();
  void recalcActive();

  // [Event Handlers]
  virtual void onMouse(MouseEvent* e);
  virtual void onKey(KeyEvent* e);
  virtual void onTimer(TimerEvent* e);
  virtual void onPaint(PaintEvent* e);

  SvgDocument svg;
  Timer timer;

  float fps;
  float fpsCounter;

  float rotate;
  float scale;
  PointF translate;
  TransformF transform;

  PointF lastPoint;
  SvgElement* active;

  String activeStrokeBackup;
  String activeStrokeWidthBackup;
  String activeOpacityBackup;

  Time fpsTime;

  err_t error;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  timer.setInterval(TimeDelta::fromMilliseconds(2));
  timer.addListener(EVENT_TIMER, this, &MyWindow::onTimer);

  fps = 0.0f;
  fpsCounter = 0.0f;

  rotate = 0.0f;
  scale = 1.0f;
  translate.reset();

  lastPoint.reset();
  active = NULL;

  recalcTransform();

  setWindowTitle(Ascii8("SvgView"));
}

MyWindow::~MyWindow()
{
}

void MyWindow::recalcTransform()
{
  transform.reset();

  SizeF size(getWidth(), getHeight());

  transform.translate(PointF(size.w / 2.0f, size.h / 2.0f));
  transform.rotate(rotate);
  transform.translate(PointF(-size.w / 2.0f, -size.h / 2.0f));
  transform.scale(PointF(scale, scale));

  transform.translate(PointF(100.0f, 100.0f));
  transform.translate(translate);
}

void MyWindow::recalcActive()
{
  SvgElement* e = NULL;

  List<SvgElement*> elements = svg.hitTest(lastPoint, &transform);
  if (!elements.isEmpty()) e = elements.top();

  if (active != e)
  {
    if (active != NULL)
    {
      active->setStyle(Ascii8("stroke"), activeStrokeBackup);
      active->setStyle(Ascii8("stroke-width"), activeStrokeWidthBackup);
      active->setStyle(Ascii8("opacity"), activeOpacityBackup);
    }

    active = e;

    if (active != NULL)
    {
      activeStrokeBackup = active->getStyle(Ascii8("stroke"));
      activeStrokeWidthBackup = active->getStyle(Ascii8("stroke-width"));
      activeOpacityBackup = active->getStyle(Ascii8("opacity"));

      active->setStyle(Ascii8("stroke"), Ascii8("#FF0000"));
      active->setStyle(Ascii8("stroke-width"), Ascii8("2.5"));
      active->setStyle(Ascii8("opacity"), Ascii8("1"));
    }
  }
}

void MyWindow::onMouse(MouseEvent* e)
{
  if (e->getCode() == EVENT_MOUSE_PRESS)
  {
    if (e->getButton() == BUTTON_LEFT)
    {
      lastPoint = e->getPosition();
      recalcActive();
      update(WIDGET_UPDATE_ALL);
    }
  }

  base::onMouse(e);
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

      case KEY_Q:
        scale += 0.1f;
        update(WIDGET_UPDATE_PAINT);
        break;
      case KEY_W:
        scale -= 0.1f;
        update(WIDGET_UPDATE_PAINT);
        break;

      case KEY_LEFT:
        translate.x -= .1f;
        update(WIDGET_UPDATE_PAINT);
        break;
      case KEY_RIGHT:
        translate.x += .1f;
        update(WIDGET_UPDATE_PAINT);
        break;
      case KEY_UP:
        translate.y -= .1f;
        update(WIDGET_UPDATE_PAINT);
        break;
      case KEY_DOWN:
        translate.y += .1f;
        update(WIDGET_UPDATE_PAINT);
        break;
    }
  }

  base::onKey(e);
}

void MyWindow::onTimer(TimerEvent* e)
{
  rotate += 0.008f;
  recalcTransform();

  update(WIDGET_UPDATE_PAINT);
}

void MyWindow::onPaint(PaintEvent* e)
{
  Time startTime = Time::now();

  Painter* p = e->getPainter();

  p->setSource(Argb32(0xFFFFFFFF));
  p->fillAll();

  p->save();
  p->setTransform(transform);
  svg.render(p);
  p->restore();

  if (active)
  {
    BoxF activeBBox(0.0f, 0.0f, 0.0f, 0.0f);
    SvgMeasure ctx;

    ctx.transform(transform);
    ctx.advance(active);
    active->onProcess(&ctx);
    //ctx.onVisit(active);

    activeBBox = ctx.getBoundingBox();

    if (activeBBox.isValid())
    {
      p->save();
      p->setSource(Argb32(0xFF0000FF));
      p->setLineWidth(1.5f);
      p->setOpacity(0.8f);
      p->drawBox(activeBBox);
      p->restore();
    }
  }

  // --------------------------------------------------------------------------

  p->flush(PAINTER_FLUSH_SYNC);

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

  p->resetTransform();

  for (int i = 0; i < 2; i++)
  {
    PathF path;
    Font font = getFont();

    font.setKerning(i);

    font.setHeight(25, UNIT_PX);
    font.getTextOutline(path, PointF(30, 30 + i * 30), text);
    p->setSource(Argb32(0xFFFF0000));
    p->fillPath(path);
  }
  // p->fillText(PointI(0, 0), text, getFont());
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_GUI_MAIN()
{
  Application app(Ascii8("Gui"));

  List<String> arguments = Application::getApplicationArguments();
  String fileName;

  if (arguments.getLength() >= 2)
  {
    fileName = arguments.at(1);
  }
  else
  {
    // My testing images...
    //fileName = Ascii8("/my/upload/img/svg/tiger.svg");

    //fileName = Ascii8("/my/upload/img/svg/map-krasnaya-plyana.svg");
    //fileName = Ascii8("/my/upload/img/svg/Map_Multilayer_Scaled.svg");
    //fileName = Ascii8("C:/my/svg/map-krasnaya-plyana.svg");
    //fileName = Ascii8("C:/my/svg/map-imeretinka.svg");
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
    fileName = Ascii8("C:/my/svg/Denis - map_v.0.2.svg");
  }

  MyWindow window;
  window.error = window.svg.readFromFile(fileName);

  SizeF size = window.svg.getDocumentSize();
  if (size.w < 800) size.w = 800;
  if (size.h < 500) size.h = 500;

  window.setSize(SizeI((int)size.w, (int)size.h));
  window.show();
  window.addListener(EVENT_CLOSE, &app, &Application::quit);

  return app.run();
}
