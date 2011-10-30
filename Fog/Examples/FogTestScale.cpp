#include "Helpers.h"  

using namespace Fog;

// ============================================================================
// [FogTest]
// ============================================================================

static void stdlog(const StringW& s)
{
  StringA local;
  TextCodec::local8().encode(local, s);
  fputs(local.getData(), stderr);
  fputc('\n', stderr);
}

static void drawText(Painter* p, const PointI& pt, const Font& font, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  StringW str;
  str.vFormat(fmt, ap);

  va_end(ap);

  PathF path;
  font.getTextOutline(path, CONTAINER_OP_APPEND, PointF(pt), str);
  p->fillPath(path);
}

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
  virtual void onPaint(PaintEvent* e);

  Image img;
  int mode;
  int scaleTo;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  setWindowTitle(StringW::fromAscii8("Testing..."));

  //img.readFromFile(StringW::fromAscii8("C:/My/Devel/Sprites/babelfish.png"));

  img.create(SizeI(3, 3), IMAGE_FORMAT_PRGB32);
  reinterpret_cast<uint32_t*>(img.getFirstX())[0] = 0xFFFFFFFF;
  reinterpret_cast<uint32_t*>(img.getFirstX())[1] = 0xFFFFFF00;
  reinterpret_cast<uint32_t*>(img.getFirstX())[2] = 0xFFFF00FF;

  reinterpret_cast<uint32_t*>(img.getFirstX())[3] = 0xFF00FFFF;
  reinterpret_cast<uint32_t*>(img.getFirstX())[4] = 0xFF000000;
  reinterpret_cast<uint32_t*>(img.getFirstX())[5] = 0xFF0000FF;

  reinterpret_cast<uint32_t*>(img.getFirstX())[6] = 0xFF000000;
  reinterpret_cast<uint32_t*>(img.getFirstX())[7] = 0xFFFFFFFF;
  reinterpret_cast<uint32_t*>(img.getFirstX())[8] = 0xFF00FF00;

  mode = 0;
  scaleTo = 413;
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
        mode = (mode + 1) % (IMAGE_RESIZE_COUNT + 2);
        update(WIDGET_UPDATE_ALL);
        break;

      case KEY_UP:
        scaleTo++;
        update(WIDGET_UPDATE_ALL);
        break;

      case KEY_UP | KEY_SHIFT:
        scaleTo+=10;
        update(WIDGET_UPDATE_ALL);
        break;

      case KEY_DOWN:
        scaleTo = Math::max(scaleTo - 1, 1);
        update(WIDGET_UPDATE_ALL);
        break;

      case KEY_DOWN | KEY_SHIFT:
        scaleTo = Math::max(scaleTo - 10, 1);
        update(WIDGET_UPDATE_ALL);
        break;

      case KEY_ENTER:
        update(WIDGET_UPDATE_ALL);
        break;
    }
  }

  base::onKey(e);
}

void MyWindow::onMouse(MouseEvent* e)
{
  if (e->getCode() == EVENT_MOUSE_PRESS)
  {
  }

  base::onMouse(e);
}

void MyWindow::onPaint(PaintEvent* e)
{
  Painter* p = e->getPainter();

  p->setSource(Argb32(0xFFFFFFFF));
  p->fillAll();

  PointI pos(100, 100);
  SizeI size(scaleTo, scaleTo);

  Time t = Time::now();
  TimeDelta elapsed;

  if (mode < IMAGE_RESIZE_COUNT)
  {
    Image scaled;
    Image::resize(scaled, size, img, mode);
    elapsed = Time::now() - t;

    p->blitImage(pos, scaled);
  }
  else if (mode == IMAGE_RESIZE_COUNT)
  {
    p->setImageQuality(IMAGE_QUALITY_BILINEAR);
    p->blitImage(RectI(pos.x, pos.y, size.w, size.h), img);
    elapsed = Time::now() - t;
  }
  else if (mode == IMAGE_RESIZE_COUNT + 1)
  {
    p->setImageQuality(IMAGE_QUALITY_NEAREST);
    p->blitImage(RectI(pos.x, pos.y, size.w, size.h), img);
    elapsed = Time::now() - t;
  }

  p->setSource(Argb32(0xFF0000FF));
  p->setLineJoin(LINE_JOIN_MITER);
  p->drawRect(RectF(float(pos.x) - 0.5f, float(pos.y) - 0.5f, float(size.w) + 1.0f, float(size.h) + 1.0f));

  p->setSource(Argb32(0xFF000000));
  drawText(p, PointI(0, 20), getFont(), "Mode=%d, ScaleTo=%d", mode, scaleTo);
  drawText(p, PointI(0, 40), getFont(), "Time: %d [ms]", (int)elapsed.getMilliseconds());
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_UI_MAIN()
{
  Application app(StringW::fromAscii8("UI"));
  MyWindow window(WINDOW_TYPE_DEFAULT);

  window.addListener(EVENT_CLOSE, &app, &Application::quit);
  window.setSize(SizeI(1000, 700));
  window.show();

  return app.run();
}
