#include "Helpers.h"

Helpers::Logger logger;

// ============================================================================
// [...]
// ============================================================================

namespace Fog {

// Get the angular direction indicated by the provided dx/dy ratio.
template<typename NumT>
FOG_STATIC_INLINE_T NumT getDirection(NumT dx, NumT dy)
{
  return Math::atan2(dy, dx);
}

template<typename NumT>
FOG_STATIC_T void strokePath(PathF& dst, const PathF& src, float distance)
{
  size_t length = src.getLength();
  if (length == 0) return;

  const NumT_(Point)* p = src.getVertices();

  NumT_(Point) last;
  bool isFirst = true;

  NumT angle[2];
  NumT as[2];
  NumT ac[2];

  for (size_t i = 1; i < length; i++)
  {
    if (isFirst)
    {
      NumT_(Point) v0 = p[i] - p[i - 1];

      angle[0] = getDirection<NumT>(v0.x, v0.y);
      Math::sincos(angle[0] + MATH_HALF_PI, &as[0], &ac[0]);

      as[0] *= distance;
      ac[0] *= distance;

      last.set(p[i-1].x + ac[0], p[i-1].y + as[0]);
      dst.moveTo(last);

      last.set(p[i].x + ac[0], p[i].y + as[0]);
      isFirst = false;
    }
    else
    {
      NumT_(Point) v0 = p[i - 1] - p[i - 2];
      NumT_(Point) v1 = p[i    ] - p[i - 1];

      angle[1] = getDirection<NumT>(v1.x, v1.y);
      Math::sincos(angle[1] + MATH_HALF_PI, &as[1], &ac[1]);

      as[1] *= distance;
      ac[1] *= distance;

      NumT_(Point) pt(p[i-1].x + ac[0], p[i-1].y + as[0]);
      LineF::intersect(pt,
        NumT_(Line)(p[i-2].x + ac[0], p[i-2].y + as[0], p[i-1].x + ac[0], p[i-1].y + as[0]),
        NumT_(Line)(p[i-1].x + ac[1], p[i-1].y + as[1], p[i  ].x + ac[1], p[i  ].y + as[1]));
      dst.lineTo(pt);

      logger.log("Poly-Angle=%f %f diff=%f ", Math::rad2deg(angle[0]), Math::rad2deg(angle[1]), Math::rad2deg(angle[1] - angle[0]));

      last.set(p[i].x + ac[1], p[i].y + as[1]);
      //dst.lineTo(last);

      angle[0] = angle[1];
      as[0] = as[1];
      ac[0] = ac[1];
    }
  }

  dst.lineTo(last);
}

} // Fog namespace

// ============================================================================
// [FogTest]
// ============================================================================

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
  virtual void onPaint(PaintEvent* e);

  PathF _path;
  size_t _pathSelectedIndex;
  PointF _pathSelectedOffset;

  PointF _nearestPoint;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  setWindowTitle(StringW::fromAscii8("Poly-Offset playground."));

  _path.moveTo(PointF(100.0f, 100.0f));
  _path.lineTo(PointF(200.0f, 200.0f));
  _path.lineTo(PointF(300.0f, 100.0f));

  _pathSelectedIndex = INVALID_INDEX;

  Font font = getFont();
  font.setHeight(20, UNIT_PX);
  setFont(font);
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
      update(WIDGET_UPDATE_ALL);
    }
  }

  base::onKey(e);
}

void MyWindow::onMouse(MouseEvent* e)
{
  if (e->getCode() == EVENT_MOUSE_PRESS && e->getButton() == BUTTON_RIGHT)
  {
    PointF last = _path.getVertex(_path.getLength() - 1);
    PointF mouse = PointF(e->getPosition());

    if (last != mouse)
    {
      _path.lineTo(mouse);
      update(WIDGET_UPDATE_ALL);
    }
  }

  if (e->getCode() == EVENT_MOUSE_PRESS && e->getButton() == BUTTON_LEFT)
  {
    size_t ptIndex = _path.getClosestVertex(PointF(e->getPosition()), 5.0f);
    if (ptIndex != INVALID_INDEX)
    {
      _pathSelectedIndex = ptIndex;
      _pathSelectedOffset = _path.getVertices()[ptIndex] - PointF(e->getPosition());

      update(WIDGET_UPDATE_ALL);
    }
  }

  if (e->getCode() == EVENT_MOUSE_RELEASE && e->getButton() == BUTTON_LEFT)
  {
    if (_pathSelectedIndex != INVALID_INDEX)
    {
      _pathSelectedIndex = INVALID_INDEX;
      update(WIDGET_UPDATE_ALL);
    }
  }

  if (e->getCode() == EVENT_MOUSE_MOVE)
  {
    if (_pathSelectedIndex != INVALID_INDEX)
    {
      _path.setVertex(_pathSelectedIndex, PointF(e->getPosition()) + _pathSelectedOffset);
      update(WIDGET_UPDATE_ALL);
    }
  }

  base::onMouse(e);
}

void MyWindow::onPaint(PaintEvent* e)
{
  Time startTime = Time::now();
  Painter* p = e->getPainter();

  p->setSource(Argb32(0xFFFFFFFF));
  p->fillAll();

  PathF strokedPath;
  strokePath<float>(strokedPath, _path, 15.0f);

  Helpers::drawPathEx<float>(p, _path      , 4.0f, Argb32(0xFFFF0000));
  Helpers::drawPathEx<float>(p, strokedPath, 2.5f, Argb32(0xFF000000));

  logger.paint(p, getFont(), PointF(5, 40), Argb32(0xFF0000FF));
  logger.clear();
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_UI_MAIN()
{
  Application app(StringW::fromAscii8("UI"));
  MyWindow window(WINDOW_TYPE_DEFAULT);

  window.addListener(EVENT_CLOSE, &app, &Application::quit);
  window.setSize(SizeI(400, 300));
  window.show();

  return app.run();
}
