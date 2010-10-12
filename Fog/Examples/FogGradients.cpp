#include <Fog/Core.h>
#include <Fog/Graphics.h>
#include <Fog/Gui.h>

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
  virtual void onPaint(PaintEvent* e);

  // Helper to paint pattern in rect with title.
  void paintPattern(Painter* p, const PointI& pos, const SizeD& size, const Pattern& pattern, const String& name);

  bool useRegion;
  double _rotate;
  double _scale;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  setWindowTitle(Ascii8("Fog - Gradients"));
  useRegion = false;
  _rotate = 0.0;
  _scale = 1.0;
}

MyWindow::~MyWindow()
{
}

void MyWindow::onKey(KeyEvent* e)
{
  if (e->getCode() == EVENT_KEY_PRESS)
  {
    if (e->getKey() == KEY_SPACE)
    {
      useRegion ^= 1;
      update(WIDGET_UPDATE_PAINT);
    }
    else if (e->getKey() == KEY_UP)
    {
      _rotate -= 0.002;
      update(WIDGET_UPDATE_PAINT);
    }
    else if (e->getKey() == KEY_DOWN)
    {
      _rotate += 0.002;
      update(WIDGET_UPDATE_PAINT);
    }
    else if (e->getKey() == KEY_LEFT)
    {
      _scale -= 0.1;
      update(WIDGET_UPDATE_PAINT);
    }
    else if (e->getKey() == KEY_RIGHT)
    {
      _scale += 0.1;
      update(WIDGET_UPDATE_PAINT);
    }
  }

  base::onKey(e);
}

void MyWindow::onPaint(PaintEvent* e)
{
  static const char* spread_names[] = { "none", "pad", "repeat", "reflect" };

  Painter* p = e->getPainter();

  p->setOperator(OPERATOR_SRC);
  p->setSource(0xFF000000);
  p->fillAll();

  p->setOperator(OPERATOR_SRC_OVER);

  Pattern pat;
  SizeD size(128.0, 128.0);
  String s;

  pat.setType(PATTERN_LINEAR_GRADIENT);
  pat.addStop(ArgbStop(0.0, 0xFFFF0000));
  pat.addStop(ArgbStop(0.5, 0xFFFFFF00));
  pat.addStop(ArgbStop(1.0, 0xFFFFFFFF));

  //p->setAlpha(0.9f);

  p->translate(-getWidth()/2, -getHeight()/2, MATRIX_ORDER_APPEND);
  p->rotate(_rotate, MATRIX_ORDER_APPEND);
  p->translate(getWidth()/2, getHeight()/2, MATRIX_ORDER_APPEND);
  p->scale(_scale, _scale);

  for (int y = 0; y < 4; y++)
  {
    pat.setType(PATTERN_LINEAR_GRADIENT);
    pat.setSpread(y);

    s.format("Linear %s", spread_names[y]);

    pat.setPoints(PointD(48.0, 48.0), PointD(80.0, 80.0));
    paintPattern(p, PointI(0, y), size, pat, s);

    pat.setPoints(PointD(64.0, 40.0), PointD(64.0, 80.0));
    paintPattern(p, PointI(1, y), size, pat, s);

    pat.setType(PATTERN_RADIAL_GRADIENT);
    s.format("Radial %s", spread_names[y]);

    pat.setPoints(PointD(64.0, 64.0), PointD(40.0, 40.0));
    pat.setRadius(40.0);
    paintPattern(p, PointI(2, y), size, pat, s);

    pat.setPoints(PointD(64.0, 64.0), PointD(20.0, 20.0));
    pat.setRadius(40.0);
    paintPattern(p, PointI(3, y), size, pat, s);

    pat.setType(PATTERN_CONICAL_GRADIENT);
    s.format("Conical %s", spread_names[y]);

    pat.setPoints(PointD(64.0, 64.0), PointD(128.0, 64.0));
    pat.setRadius(40.0);
    paintPattern(p, PointI(4, y), size, pat, s);
  }
}

void MyWindow::paintPattern(Painter* p, const PointI& pos, const SizeD& size, const Pattern& pattern, const String& name)
{
  double x = 10 + pos.x * (size.w + 20.0);
  double y = 10 + pos.y * (size.h + 20.0);

  Pattern copy(pattern);
  copy.translate(x + 1.0, y + 21.0);
  p->setSource(copy);

  if (useRegion)
  {
    Region r;
    int xx, yy;
    for (yy = 0; yy < size.h; yy += 20)
    {
      for (xx = 0; xx < size.w; xx += 20)
      {
        r.combine(RectI(x+xx+1, y+21+yy, 10, 10), REGION_OP_UNION);
      }
    }
    p->fillRegion(r);
  }
  else
  {
    p->fillRect(RectI((int)x, (int)y + 20, (int)size.w + 2, (int)size.h + 2));
  }

  p->save();
  p->setAlpha(1.0f);

  p->setSource(0xFFFFFFFF);
  p->drawText(RectI((int)x, (int)y, (int)size.w, 20), name, getFont(), TEXT_ALIGN_CENTER);
  p->drawRect(RectI((int)x - 1, (int)y + 19, (int)size.w + 3, (int)size.h + 3));

  p->restore();
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_GUI_MAIN()
{
  Application app(Ascii8("Gui"));

  MyWindow window;
  window.setSize(SizeI(800, 640));
  window.show();
  window.addListener(EVENT_CLOSE, &app, &Application::quit);

  return app.run();
}
