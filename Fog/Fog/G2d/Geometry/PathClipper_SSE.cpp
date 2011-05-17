// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Guard]
#include <Fog/Core/Config/Config.h>
#if defined(FOG_OPTIMIZE_SSE)

// [Dependencies]
#include <Fog/Core/Face/Face_SSE.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Internals_p.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

// ============================================================================
// [Fog::PathClipper - InitPath]
// ============================================================================

static uint32_t FOG_CDECL _G2d_PathClipperF_initPath_SSE(PathClipperF& self, const PathF& src)
{
  self._lastIndex = INVALID_INDEX;

  Face::m128f xmmClipBox;
  Face::m128f xmmClipMin;
  Face::m128f xmmClipMax;

  Face::m128fLoad16u(xmmClipBox, &self._clipBox);
  Face::m128fShuffle<1, 0, 1, 0>(xmmClipMin, xmmClipBox);
  Face::m128fShuffle<3, 2, 3, 2>(xmmClipMax, xmmClipBox);

  // If path bounding box is not dirty then we can simply use it instead of
  // performing the calculation.
  if (src._hasBoundingBox())
  {
    Face::m128f xmm0;
    Face::m128f xmm1;
    int msk;

    Face::m128fLoad16u(xmm0,  &src._d->boundingBox);
    Face::m128fCopy(xmm1, xmm0);

    Face::m128fCmpGePS(xmm0, xmm0, xmmClipMin);
    Face::m128fCmpLePS(xmm1, xmm1, xmmClipMax);

    Face::m128fAnd(xmm0, xmm0, xmm1);
    Face::m128fMoveMask(msk, xmm0);

    if (msk == 0xF)
    {
      _FOG_PATH_VERIFY_BOUNDING_BOX(src);
      return CLIPPER_INIT_ALREADY_CLIPPED;
    }
  }

  Face::m128fXor(xmmClipBox, xmmClipBox, FOG_SSE_GET_CONST_PS(m128f_p0_p0_sn_sn));

  sysuint_t i = src.getLength();
  const uint8_t* cmd = src.getCommands();
  const PointF* pts = src.getVertices();
  const BoxF& clipBox = self._clipBox;

  bool hasInitial = false;

  while (i)
  {
    Face::m128f xmm0;

    Face::m128fZero(xmm0);
    Face::m128fLoad8Lo(xmm0, pts);

    int msk;

    switch (cmd[0])
    {
      // ----------------------------------------------------------------------
      // [Move-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_MOVE_TO:
        Face::m128fStore8Lo(&self._lastMoveTo, xmm0);
        hasInitial = true;

      // ----------------------------------------------------------------------
      // [Line-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_LINE_TO:
      {
        if (FOG_UNLIKELY(!hasInitial)) goto _Invalid;

        Face::m128fShuffle<1, 0, 1, 0>(xmm0, xmm0, xmm0);
        Face::m128fXor(xmm0, xmm0, FOG_SSE_GET_CONST_PS(m128f_p0_p0_sn_sn));
        Face::m128fCmpLePS(xmm0, xmm0, xmmClipBox);

        Face::m128fMoveMask(msk, xmm0);
        if (msk != 0xF) goto _MustClip;

        i--;
        cmd++;
        pts++;
        break;
      }

      // ----------------------------------------------------------------------
      // [Quad-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_QUAD_TO:
      {
        Face::m128f xmm1;

        FOG_ASSERT(i >= 2);
        if (FOG_UNLIKELY(!hasInitial)) goto _Invalid;

        Face::m128fLoad8Hi(xmm0, pts + 2);

        Face::m128fCopy(xmm1, xmm0);
        Face::m128fCmpGePS(xmm0, xmm0, xmmClipMin);
        Face::m128fCmpLePS(xmm1, xmm1, xmmClipMax);

        Face::m128fAnd(xmm0, xmm0, xmm1);

        Face::m128fMoveMask(msk, xmm0);
        if (msk != 0xF) goto _MustClip;

        i   -= 2;
        cmd += 2;
        pts += 2;
        break;
      }

      // ----------------------------------------------------------------------
      // [Cubic-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_CUBIC_TO:
      {
        Face::m128f xmm1, xmm2;

        FOG_ASSERT(i >= 3);
        if (FOG_UNLIKELY(!hasInitial)) goto _Invalid;

        Face::m128fZero(xmm2);
        Face::m128fLoad8Hi(xmm0, pts + 2);
        Face::m128fLoad8Lo(xmm2, pts + 4);

        Face::m128fShuffle<1, 0, 1, 0>(xmm2, xmm2, xmm2);
        Face::m128fXor(xmm2, xmm2, FOG_SSE_GET_CONST_PS(m128f_p0_p0_sn_sn));

        Face::m128fCopy(xmm1, xmm0);
        Face::m128fCmpGePS(xmm0, xmm0, xmmClipMin);
        Face::m128fCmpLePS(xmm1, xmm1, xmmClipMax);
        Face::m128fCmpLePS(xmm2, xmm2, xmmClipBox);

        Face::m128fAnd(xmm0, xmm0, xmm1);
        Face::m128fAnd(xmm0, xmm0, xmm2);

        Face::m128fMoveMask(msk, xmm0);
        if (msk != 0xF) goto _MustClip;

        i   -= 3;
        cmd += 3;
        pts += 3;
        break;
      }

      // ----------------------------------------------------------------------
      // [Close]
      // ----------------------------------------------------------------------

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

  // Path don't need to be clipped.
  return CLIPPER_INIT_ALREADY_CLIPPED;

_MustClip:
  self._lastIndex = (sysuint_t)(cmd - src.getCommands());
  return CLIPPER_INIT_NOT_CLIPPED;

_Invalid:
  return CLIPPER_INIT_INVALID_PATH;
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_pathclipper_init_sse(void)
{
  _g2d.pathclipperf.initPath = _G2d_PathClipperF_initPath_SSE;
}

} // Fog namespace

// [Guard]
#endif // FOG_OPTIMIZE_SSE
