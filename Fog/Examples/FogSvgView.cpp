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
  virtual void onPaint(PaintEvent* e);

  SvgDocument svg;
  err_t error;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  setWindowTitle(Ascii8("SvgView"));
}

MyWindow::~MyWindow()
{
}

void MyWindow::onPaint(PaintEvent* e)
{
  Painter* p = e->getPainter();

  p->setSource(0xFFAFAFAF);
  p->clear();

  SvgContext context(p);
  svg.onRender(&context);
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
  window.error = window.svg.readFile(fileName);

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

  window.setSize(IntSize(w, h));
  window.show();
  window.addListener(EVENT_CLOSE, &app, &Application::quit);

  return app.run();
}
