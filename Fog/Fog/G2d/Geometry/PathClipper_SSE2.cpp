// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Face/Face_SSE.h>
#include <Fog/Core/Face/Face_SSE2.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Internals_p.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

// ============================================================================
// [Fog::PathClipper - MeasurePath]
// ============================================================================

static uint32_t FOG_CDECL PathClipperD_measurePath_SSE2(PathClipperD* self, const PathD* src)
{
  self->_lastIndex = INVALID_INDEX;

  Face::m128d xmmClipBoxMin;
  Face::m128d xmmClipBoxMax;

  Face::m128dLoad16u(xmmClipBoxMin, &self->_clipBox.x0);
  Face::m128dLoad16u(xmmClipBoxMax, &self->_clipBox.x1);

  // If path bounding box is not dirty then we can simply use it instead of
  // performing the calculation.
  if (src->_hasBoundingBox())
  {
    Face::m128d xmmX0Y0;
    Face::m128d xmmX1Y1;

    Face::m128dLoad16u(xmmX0Y0,  &src->_d->boundingBox.x0);
    Face::m128dLoad16u(xmmX1Y1,  &src->_d->boundingBox.x1);

    Face::m128dCmpGePD(xmmX0Y0, xmmX0Y0, xmmClipBoxMin);
    Face::m128dCmpLePD(xmmX1Y1, xmmX1Y1, xmmClipBoxMax);

    int msk;
    Face::m128dAnd(xmmX0Y0, xmmX0Y0, xmmX1Y1);
    Face::m128dMoveMaskPD(msk, xmmX0Y0);

    if (msk == 0x3)
    {
      _FOG_PATH_VERIFY_BOUNDING_BOX(*src);
      return PATH_CLIPPER_MEASURE_BOUNDED;
    }
  }

  size_t i = src->getLength();
  const uint8_t* cmd = src->getCommands();
  const PointD* pts = src->getVertices();

  bool hasInitial = false;
  Face::m128d xmmLastMoveTo;

  if (((size_t)pts & 0xF) == 0)
  {
    while (i)
    {
      Face::m128d xmmSrc0a;
      Face::m128d xmmSrc0b;

      Face::m128dLoad16a(xmmSrc0a, pts);
      int msk;

      switch (cmd[0])
      {
        // --------------------------------------------------------------------
        // [Move-To]
        // --------------------------------------------------------------------

        case PATH_CMD_MOVE_TO:
          xmmLastMoveTo = xmmSrc0a;
          hasInitial = true;

        // --------------------------------------------------------------------
        // [Line-To]
        // --------------------------------------------------------------------

        case PATH_CMD_LINE_TO:
        {
          if (FOG_UNLIKELY(!hasInitial))
            goto _Invalid;

          xmmSrc0b = xmmSrc0a;
          Face::m128dCmpGePD(xmmSrc0a, xmmSrc0a, xmmClipBoxMin);
          Face::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);

          Face::m128dMoveMaskPD(msk, xmmSrc0a);
          if (msk != 0x3)
            goto _Unbounded;

          i--;
          cmd++;
          pts++;
          break;
        }

        // --------------------------------------------------------------------
        // [Quad-To]
        // --------------------------------------------------------------------

        case PATH_CMD_QUAD_TO:
        {
          Face::m128d xmmSrc1a;
          Face::m128d xmmSrc1b;

          FOG_ASSERT(i >= 2);
          if (FOG_UNLIKELY(!hasInitial))
            goto _Invalid;

          Face::m128dLoad16a(xmmSrc1a, pts + 1);
          xmmSrc0b = xmmSrc0a;
          xmmSrc1b = xmmSrc1a;

          Face::m128dCmpGePD(xmmSrc0a, xmmSrc0a, xmmClipBoxMin);
          Face::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);

          Face::m128dCmpGePD(xmmSrc1a, xmmSrc1a, xmmClipBoxMin);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);
          Face::m128dCmpLePD(xmmSrc1b, xmmSrc1b, xmmClipBoxMax);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1a);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1b);

          Face::m128dMoveMaskPD(msk, xmmSrc0a);
          if (msk != 0x3)
            goto _Unbounded;

          i   -= 2;
          cmd += 2;
          pts += 2;
          break;
        }

        // --------------------------------------------------------------------
        // [Cubic-To]
        // --------------------------------------------------------------------

        case PATH_CMD_CUBIC_TO:
        {
          Face::m128d xmmSrc1a;
          Face::m128d xmmSrc1b;
          Face::m128d xmmSrc2a;

          FOG_ASSERT(i >= 2);
          if (FOG_UNLIKELY(!hasInitial))
            goto _Invalid;

          Face::m128dLoad16a(xmmSrc1a, pts + 1);
          Face::m128dLoad16a(xmmSrc2a, pts + 2);
          xmmSrc0b = xmmSrc0a;
          xmmSrc1b = xmmSrc1a;

          Face::m128dCmpGePD(xmmSrc0a, xmmSrc0a, xmmClipBoxMin);
          Face::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);

          Face::m128dCmpGePD(xmmSrc1a, xmmSrc1a, xmmClipBoxMin);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);
          Face::m128dCmpLePD(xmmSrc1b, xmmSrc1b, xmmClipBoxMax);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1a);

          xmmSrc0b = xmmSrc2a;
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1b);

          Face::m128dCmpGePD(xmmSrc2a, xmmSrc2a, xmmClipBoxMin);
          Face::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc2a);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);

          Face::m128dMoveMaskPD(msk, xmmSrc0a);
          if (msk != 0x3)
            goto _Unbounded;

          i   -= 3;
          cmd += 3;
          pts += 3;
          break;
        }

        // --------------------------------------------------------------------
        // [Close]
        // --------------------------------------------------------------------

        case PATH_CMD_CLOSE:
          hasInitial = false;

          i--;
          cmd++;
          pts++;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }
  }
  else
  {
    while (i)
    {
      Face::m128d xmmSrc0a;
      Face::m128d xmmSrc0b;

      Face::m128dLoad16u(xmmSrc0a, pts);
      int msk;

      switch (cmd[0])
      {
        // --------------------------------------------------------------------
        // [Move-To]
        // --------------------------------------------------------------------

        case PATH_CMD_MOVE_TO:
          xmmLastMoveTo = xmmSrc0a;
          hasInitial = true;

        // --------------------------------------------------------------------
        // [Line-To]
        // --------------------------------------------------------------------

        case PATH_CMD_LINE_TO:
        {
          if (FOG_UNLIKELY(!hasInitial))
            goto _Invalid;

          xmmSrc0b = xmmSrc0a;
          Face::m128dCmpGePD(xmmSrc0a, xmmSrc0a, xmmClipBoxMin);
          Face::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);

          Face::m128dMoveMaskPD(msk, xmmSrc0a);
          if (msk != 0x3)
            goto _Unbounded;

          i--;
          cmd++;
          pts++;
          break;
        }

        // --------------------------------------------------------------------
        // [Quad-To]
        // --------------------------------------------------------------------

        case PATH_CMD_QUAD_TO:
        {
          Face::m128d xmmSrc1a;
          Face::m128d xmmSrc1b;

          FOG_ASSERT(i >= 2);
          if (FOG_UNLIKELY(!hasInitial))
            goto _Invalid;

          Face::m128dLoad16u(xmmSrc1a, pts + 1);
          xmmSrc0b = xmmSrc0a;
          xmmSrc1b = xmmSrc1a;

          Face::m128dCmpGePD(xmmSrc0a, xmmSrc0a, xmmClipBoxMin);
          Face::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);

          Face::m128dCmpGePD(xmmSrc1a, xmmSrc1a, xmmClipBoxMin);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);
          Face::m128dCmpLePD(xmmSrc1b, xmmSrc1b, xmmClipBoxMax);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1a);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1b);

          Face::m128dMoveMaskPD(msk, xmmSrc0a);
          if (msk != 0x3)
            goto _Unbounded;

          i   -= 2;
          cmd += 2;
          pts += 2;
          break;
        }

        // --------------------------------------------------------------------
        // [Cubic-To]
        // --------------------------------------------------------------------

        case PATH_CMD_CUBIC_TO:
        {
          Face::m128d xmmSrc1a;
          Face::m128d xmmSrc1b;
          Face::m128d xmmSrc2a;

          FOG_ASSERT(i >= 2);
          if (FOG_UNLIKELY(!hasInitial))
            goto _Invalid;

          Face::m128dLoad16u(xmmSrc1a, pts + 1);
          Face::m128dLoad16u(xmmSrc2a, pts + 2);
          xmmSrc0b = xmmSrc0a;
          xmmSrc1b = xmmSrc1a;

          Face::m128dCmpGePD(xmmSrc0a, xmmSrc0a, xmmClipBoxMin);
          Face::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);

          Face::m128dCmpGePD(xmmSrc1a, xmmSrc1a, xmmClipBoxMin);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);
          Face::m128dCmpLePD(xmmSrc1b, xmmSrc1b, xmmClipBoxMax);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1a);

          xmmSrc0b = xmmSrc2a;
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1b);

          Face::m128dCmpGePD(xmmSrc2a, xmmSrc2a, xmmClipBoxMin);
          Face::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc2a);
          Face::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);

          Face::m128dMoveMaskPD(msk, xmmSrc0a);
          if (msk != 0x3)
            goto _Unbounded;

          i   -= 3;
          cmd += 3;
          pts += 3;
          break;
        }

        // --------------------------------------------------------------------
        // [Close]
        // --------------------------------------------------------------------

        case PATH_CMD_CLOSE:
          hasInitial = false;

          i--;
          cmd++;
          pts++;
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }
  }
  return PATH_CLIPPER_MEASURE_BOUNDED;

_Unbounded:
  self->_lastIndex = (size_t)(cmd - src->getCommands());
  return PATH_CLIPPER_MEASURE_UNBOUNDED;

_Invalid:
  return PATH_CLIPPER_MEASURE_INVALID;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void PathClipper_init_SSE2(void)
{
  _api.pathclipperd.measurePath = PathClipperD_measurePath_SSE2;
}

} // Fog namespace
