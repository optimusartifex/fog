#include <Fog/Core.h>
#include <Fog/G2d.h>
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

  setWindowTitle(Ascii8("SvgView"));
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
  rotate += 0.005f;
  update(WIDGET_UPDATE_PAINT);
}

void MyWindow::onPaint(PaintEvent* e)
{
  Time startTime = Time::now();

  Painter* p = e->getPainter();

  p->setSource(Argb32(0xFFFFFFFF));
  p->clear();

  p->save();

  if (1)
  {
    SizeF size;
    p->getSize(size);

    p->translate(PointF(size.w / 2.0f, size.h / 2.0f));
    p->rotate(rotate);
    p->translate(PointF(-size.w / 2.0f, -size.h / 2.0f));
    p->scale(PointF(scale, scale));
  }

  p->translate(PointF(100.0f, 100.0f));
  p->translate(translate);

  SvgRenderContext context(p);
  svg.onRender(&context);

  p->restore();

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
  {
    PathD path;
    Font font = getFont();
    font.setSize(25);
    font.getOutline(text, path);
    p->setSource(Argb32(0xFFFF0000));
    path.translate(PointD(30, 30));
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
    fileName = Ascii8("C:/my/svg/pattern.svg");
  }

  MyWindow window;
  window.error = window.svg.readFromFile(fileName);

  int w = 0, h = 0;
  XmlElement* root = window.svg.documentRoot();

  // TODO: There is no API to get SVG width/height.
  if (root)
  {
    root->getAttribute(Ascii8("width")).atoi32(&w);
    root->getAttribute(Ascii8("height")).atoi32(&h);
  }

  if (w < 800) w = 800;
  if (h < 500) h = 500;

  window.setSize(SizeI(w, h));
  window.show();
  window.addListener(EVENT_CLOSE, &app, &Application::quit);

  return app.run();
}
