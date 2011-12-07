// [Guard]
#ifndef _HELPERS_H
#define _HELPERS_H

// [Dependencies]
#include <Fog/Core.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/G2d.h>
#include <Fog/Svg.h>
#include <Fog/UI.h>

namespace Helpers {

using namespace Fog;

// ============================================================================
// [Logger]
// ============================================================================

struct FOG_NO_EXPORT Logger
{
  Logger() {}
  ~Logger() {}

  void clear()
  {
    lines.clear();
  }

  void log(const char* fmt, ...)
  {
    StringW line;

    va_list ap;
    va_start(ap, fmt);
    line.vFormat(fmt, ap);
    va_end(ap);

    lines.append(line);
    lines.append(CharW('\n'));
  }

  void paint(Painter* p, const Font& font, const PointF& pt, const Argb32& color)
  {
    float x = pt.x;
    float y = pt.y;

    float h = font.getHeight();
    List<StringW> l = lines.split(CharW('\n'));

    PathF path;

    for (size_t i = 0; i < l.getLength(); i++)
    {
      font.getTextOutline(path, CONTAINER_OP_APPEND, PointF(x, y), l.getAt(i));
      y += h;
    }

    p->setSource(color);
    p->fillPath(path);
  }

  StringW lines;
};

static Logger logger;

// ============================================================================
// [Logger]
// ============================================================================

static void drawText(Painter* p, const PointI& pt, const Font& font, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  StringW string;
  string.vFormat(fmt, ap);

  va_end(ap);

  PathF path;
  font.getTextOutline(path, CONTAINER_OP_APPEND, PointF(pt), string);
  p->fillPath(path);
}

template<typename NumT>
static void drawPathEx(Painter* p, const NumT_(Path)& path, NumT rad, Argb32 color)
{
  size_t i = 0;
  size_t len = path.getLength();

  const uint8_t* pathCmd = path.getCommands();
  const NumT_(Point)* pathPts = path.getVertices();

  p->save();

  p->setSource(color);
  p->drawPath(path);

  Argb32 vertexColor((color.u32 & 0x00FFFFFF) | 0x2F000000);
  Argb32 helperColor((color.u32 & 0x00FFFFFF) | 0x2F000000);

  while (i < len)
  {
    switch (pathCmd[i])
    {
      case PATH_CMD_MOVE_TO:
        p->setSource(vertexColor);
        p->fillCircle(NumT_(Circle)(pathPts[i], rad));

        i++;
        break;

      case PATH_CMD_LINE_TO:
        p->setSource(vertexColor);
        p->fillCircle(NumT_(Circle)(pathPts[i], rad));

        i++;
        break;

      case PATH_CMD_QUAD_TO:
        p->setSource(helperColor);
        p->drawPolyline(pathPts + i - 1, 3);

        p->setSource(vertexColor);
        p->fillCircle(NumT_(Circle)(pathPts[i  ], rad));
        p->fillCircle(NumT_(Circle)(pathPts[i+1], rad));

        i += 2;
        break;

      case PATH_CMD_CUBIC_TO:
        p->setSource(helperColor);
        p->drawPolyline(pathPts + i - 1, 4);

        p->setSource(vertexColor);
        p->fillCircle(NumT_(Circle)(pathPts[i  ], rad));
        p->fillCircle(NumT_(Circle)(pathPts[i+1], rad));
        p->fillCircle(NumT_(Circle)(pathPts[i+2], rad));

        i += 3;
        break;

      case PATH_CMD_CLOSE:
        i++;
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }

  p->restore();
}

template<typename NumT>
static void drawPathInfo(Painter* p, const PointI& pt, const NumT_(Path)& path, const Font& font, Argb32 color)
{
  size_t i;
  size_t len = path.getLength();

  uint vertices = (uint)len;

  uint moves = 0;
  uint lines = 0;
  uint qcurves = 0;
  uint ccurves = 0;
  uint close = 0;

  for (i = 0; i < len; i++)
  {
    switch (path.getCommands()[i])
    {
      case PATH_CMD_MOVE_TO : moves++; break;
      case PATH_CMD_LINE_TO : lines++; break;
      case PATH_CMD_CLOSE   : close++; break;

      case PATH_CMD_QUAD_TO : qcurves++; i += 1; break;
      case PATH_CMD_CUBIC_TO: ccurves++; i += 2; break;
    }
  }

  p->setSource(color);
  drawText(p, pt, font, "%u commands (move-to: %d, line-to: %d, quad-to: %d, cubic-to: %d, close: %d)",
    vertices, moves, lines, qcurves, ccurves, close);
}

} // Helpers namespace

// [Guard]
#endif // _HELPERS_H
