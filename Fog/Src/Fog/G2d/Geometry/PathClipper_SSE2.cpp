// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Acc/AccSse.h>
#include <Fog/Core/Acc/AccSse2.h>
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

  __m128d xmmClipBoxMin;
  __m128d xmmClipBoxMax;

  Acc::m128dLoad16u(xmmClipBoxMin, &self->_clipBox.x0);
  Acc::m128dLoad16u(xmmClipBoxMax, &self->_clipBox.x1);

  // If path bounding box is not dirty then we can simply use it instead of
  // performing the calculation.
  if (src->_hasBoundingBox())
  {
    __m128d xmmX0Y0;
    __m128d xmmX1Y1;

    Acc::m128dLoad16u(xmmX0Y0,  &src->_d->boundingBox.x0);
    Acc::m128dLoad16u(xmmX1Y1,  &src->_d->boundingBox.x1);

    Acc::m128dCmpGePD(xmmX0Y0, xmmX0Y0, xmmClipBoxMin);
    Acc::m128dCmpLePD(xmmX1Y1, xmmX1Y1, xmmClipBoxMax);

    int msk;
    Acc::m128dAnd(xmmX0Y0, xmmX0Y0, xmmX1Y1);
    Acc::m128dMoveMaskPD(msk, xmmX0Y0);

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
  __m128d xmmLastMoveTo = _mm_setzero_pd();

  // Current command with _PATH_CMD_HAS_INITIAL flag included. Instead of using
  // variable for 'hasInitial' flag and command separately, we combined it into
  // single variable, thus one branch has been eliminated within the loop.
  uint32_t c = 0;

  if (((size_t)pts & 0xF) == 0)
  {
    while (i)
    {
      __m128d xmmSrc0a;
      __m128d xmmSrc0b;
      int msk;

      Acc::m128dLoad16a(xmmSrc0a, pts);

      c |= static_cast<uint32_t>(cmd[0]);
      switch (c)
      {
        // --------------------------------------------------------------------
        // [Move-To]
        // --------------------------------------------------------------------

        case PATH_CMD_MOVE_TO:
        case PATH_CMD_MOVE_TO | _PATH_CMD_HAS_INITIAL:
          c = _PATH_CMD_HAS_INITIAL;

          xmmLastMoveTo = xmmSrc0a;
          // ... Fall through ...

        // --------------------------------------------------------------------
        // [Line-To]
        // --------------------------------------------------------------------

        case PATH_CMD_LINE_TO | _PATH_CMD_HAS_INITIAL:
        {
          c = _PATH_CMD_HAS_INITIAL;

          xmmSrc0b = xmmSrc0a;
          Acc::m128dCmpGePD(xmmSrc0a, xmmSrc0a, xmmClipBoxMin);
          Acc::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);

          Acc::m128dMoveMaskPD(msk, xmmSrc0a);
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

        case PATH_CMD_QUAD_TO | _PATH_CMD_HAS_INITIAL:
        {
          __m128d xmmSrc1a;
          __m128d xmmSrc1b;

          c = _PATH_CMD_HAS_INITIAL;
          FOG_ASSERT(i >= 2);

          Acc::m128dLoad16a(xmmSrc1a, pts + 1);
          xmmSrc0b = xmmSrc0a;
          xmmSrc1b = xmmSrc1a;

          Acc::m128dCmpGePD(xmmSrc0a, xmmSrc0a, xmmClipBoxMin);
          Acc::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);

          Acc::m128dCmpGePD(xmmSrc1a, xmmSrc1a, xmmClipBoxMin);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);
          Acc::m128dCmpLePD(xmmSrc1b, xmmSrc1b, xmmClipBoxMax);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1a);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1b);

          Acc::m128dMoveMaskPD(msk, xmmSrc0a);
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

        case PATH_CMD_CUBIC_TO | _PATH_CMD_HAS_INITIAL:
        {
          __m128d xmmSrc1a;
          __m128d xmmSrc1b;
          __m128d xmmSrc2a;

          c = _PATH_CMD_HAS_INITIAL;
          FOG_ASSERT(i >= 2);

          Acc::m128dLoad16a(xmmSrc1a, pts + 1);
          Acc::m128dLoad16a(xmmSrc2a, pts + 2);
          xmmSrc0b = xmmSrc0a;
          xmmSrc1b = xmmSrc1a;

          Acc::m128dCmpGePD(xmmSrc0a, xmmSrc0a, xmmClipBoxMin);
          Acc::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);

          Acc::m128dCmpGePD(xmmSrc1a, xmmSrc1a, xmmClipBoxMin);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);
          Acc::m128dCmpLePD(xmmSrc1b, xmmSrc1b, xmmClipBoxMax);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1a);

          xmmSrc0b = xmmSrc2a;
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1b);

          Acc::m128dCmpGePD(xmmSrc2a, xmmSrc2a, xmmClipBoxMin);
          Acc::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc2a);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);

          Acc::m128dMoveMaskPD(msk, xmmSrc0a);
          if (msk != 0x3)
            goto _Unbounded;

          i   -= 3;
          cmd += 3;
          pts += 3;
          break;
        }

        // ----------------------------------------------------------------------
        // [Close]
        // ----------------------------------------------------------------------

        case PATH_CMD_CLOSE:
        case PATH_CMD_CLOSE | _PATH_CMD_HAS_INITIAL:
          c = 0;

          i--;
          cmd++;
          pts++;
          break;

        // ----------------------------------------------------------------------
        // [Error]
        // ----------------------------------------------------------------------

        case PATH_CMD_LINE_TO:
        case PATH_CMD_QUAD_TO:
        case PATH_CMD_CUBIC_TO:
          goto _Invalid;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }
  }
  else
  {
    while (i)
    {
      __m128d xmmSrc0a;
      __m128d xmmSrc0b;
      int msk;

      Acc::m128dLoad16u(xmmSrc0a, pts);

      c |= static_cast<uint32_t>(cmd[0]);
      switch (c)
      {
        // --------------------------------------------------------------------
        // [Move-To]
        // --------------------------------------------------------------------

        case PATH_CMD_MOVE_TO:
        case PATH_CMD_MOVE_TO | _PATH_CMD_HAS_INITIAL:
          c = _PATH_CMD_HAS_INITIAL;

          xmmLastMoveTo = xmmSrc0a;
          // ... Fall through ...

        // --------------------------------------------------------------------
        // [Line-To]
        // --------------------------------------------------------------------

        case PATH_CMD_LINE_TO | _PATH_CMD_HAS_INITIAL:
        {
          c = _PATH_CMD_HAS_INITIAL;

          xmmSrc0b = xmmSrc0a;
          Acc::m128dCmpGePD(xmmSrc0a, xmmSrc0a, xmmClipBoxMin);
          Acc::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);

          Acc::m128dMoveMaskPD(msk, xmmSrc0a);
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

        case PATH_CMD_QUAD_TO | _PATH_CMD_HAS_INITIAL:
        {
          __m128d xmmSrc1a;
          __m128d xmmSrc1b;

          c = _PATH_CMD_HAS_INITIAL;
          FOG_ASSERT(i >= 2);

          Acc::m128dLoad16u(xmmSrc1a, pts + 1);
          xmmSrc0b = xmmSrc0a;
          xmmSrc1b = xmmSrc1a;

          Acc::m128dCmpGePD(xmmSrc0a, xmmSrc0a, xmmClipBoxMin);
          Acc::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);

          Acc::m128dCmpGePD(xmmSrc1a, xmmSrc1a, xmmClipBoxMin);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);
          Acc::m128dCmpLePD(xmmSrc1b, xmmSrc1b, xmmClipBoxMax);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1a);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1b);

          Acc::m128dMoveMaskPD(msk, xmmSrc0a);
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

        case PATH_CMD_CUBIC_TO | _PATH_CMD_HAS_INITIAL:
        {
          __m128d xmmSrc1a;
          __m128d xmmSrc1b;
          __m128d xmmSrc2a;

          c = _PATH_CMD_HAS_INITIAL;
          FOG_ASSERT(i >= 2);

          Acc::m128dLoad16u(xmmSrc1a, pts + 1);
          Acc::m128dLoad16u(xmmSrc2a, pts + 2);
          xmmSrc0b = xmmSrc0a;
          xmmSrc1b = xmmSrc1a;

          Acc::m128dCmpGePD(xmmSrc0a, xmmSrc0a, xmmClipBoxMin);
          Acc::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);

          Acc::m128dCmpGePD(xmmSrc1a, xmmSrc1a, xmmClipBoxMin);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);
          Acc::m128dCmpLePD(xmmSrc1b, xmmSrc1b, xmmClipBoxMax);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1a);

          xmmSrc0b = xmmSrc2a;
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc1b);

          Acc::m128dCmpGePD(xmmSrc2a, xmmSrc2a, xmmClipBoxMin);
          Acc::m128dCmpLePD(xmmSrc0b, xmmSrc0b, xmmClipBoxMax);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc2a);
          Acc::m128dAnd(xmmSrc0a, xmmSrc0a, xmmSrc0b);

          Acc::m128dMoveMaskPD(msk, xmmSrc0a);
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
        case PATH_CMD_CLOSE | _PATH_CMD_HAS_INITIAL:
          c = 0;

          i--;
          cmd++;
          pts++;
          break;

        // ----------------------------------------------------------------------
        // [Error]
        // ----------------------------------------------------------------------

        case PATH_CMD_LINE_TO:
        case PATH_CMD_QUAD_TO:
        case PATH_CMD_CUBIC_TO:
          goto _Invalid;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
    }
  }
  return PATH_CLIPPER_MEASURE_BOUNDED;

_Unbounded:
  Acc::m128dStore16u(&self->_lastMoveTo, xmmLastMoveTo);
  self->_lastIndex = (size_t)(cmd - src->getCommands());
  return PATH_CLIPPER_MEASURE_UNBOUNDED;

_Invalid:
  Acc::m128dStore16u(&self->_lastMoveTo, xmmLastMoveTo);
  return PATH_CLIPPER_MEASURE_INVALID;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void PathClipper_init_SSE2(void)
{
  fog_api.pathclipperd_measurePath = PathClipperD_measurePath_SSE2;
}

} // Fog namespace
