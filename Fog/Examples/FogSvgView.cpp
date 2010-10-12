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
  virtual void onKey(KeyEvent* e);
  virtual void onTimer(TimerEvent* e);
  virtual void onPaint(PaintEvent* e);

  SvgDocument svg;
  Timer timer;

  float fps;
  float fpsCounter;
  double rotate;
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
  rotate = 0.0;

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
    }
  }

  base::onKey(e);
}

void MyWindow::onTimer(TimerEvent* e)
{
  rotate += 0.005;
  update(WIDGET_UPDATE_PAINT);
}

void MyWindow::onPaint(PaintEvent* e)
{
  Time startTime = Time::now();

  Painter* p = e->getPainter();

  p->setSource(0xFFFFFFFF);
  p->fillAll();

  p->save();

  if (1)
  {
    p->translate(p->getWidth() / 2, p->getHeight() / 2);
    p->rotate(rotate);
    p->translate(-p->getWidth() / 2, -p->getHeight() / 2);

    //TransformD perspective;
    //perspective.setQuadToQuad(PointD(0, 0), PointD(400, 270), PointD(320, 400), PointD(0, 400), BoxD(0, 0, 400, 400));
    //p->setTransform(perspective);
  }

  SvgContext context(p);
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

  p->setSource(ArgbI(0xFFFF0000));
  p->drawText(PointI(0, 0), text, getFont());
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_GUI_MAIN()
{
  Application app(Ascii8("Gui"));

  List<String> arguments = Application::getApplicationArguments();
  String fileName;

  if (arguments.getLength() < 2) return 1;
  fileName = arguments.at(1);

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

  if (w < 200) w = 200;
  if (h < 60) h = 60;

  window.setSize(SizeI(w, h));
  window.show();
  window.addListener(EVENT_CLOSE, &app, &Application::quit);

  return app.run();
}
