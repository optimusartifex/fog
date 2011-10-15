// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/G2d/Source/Gradient.h>

namespace Fog {

// ============================================================================
// [Fog::GradientF - Construction / Destruction]
// ============================================================================

static void FOG_CDECL GradientF_ctor(GradientF* self, uint32_t gradientType)
{
  self->_gradientType = gradientType;
  self->_gradientSpread = GRADIENT_SPREAD_DEFAULT;
  self->_stops.init();
  self->_pts[0].reset();
  self->_pts[1].reset();
  self->_pts[2].reset();
}

static void FOG_CDECL GradientF_ctorCopyF(GradientF* self, const GradientF* other)
{
  self->_gradientType = other->_gradientType;
  self->_gradientSpread = other->_gradientSpread;
  self->_stops.initCustom1(other->_stops());
  self->_pts[0] = other->_pts[0];
  self->_pts[1] = other->_pts[1];
  self->_pts[2] = other->_pts[2];
}

static void FOG_CDECL GradientF_ctorCopyD(GradientF* self, const GradientD* other)
{
  self->_gradientType = other->_gradientType;
  self->_gradientSpread = other->_gradientSpread;
  self->_stops.initCustom1(other->_stops());
  self->_pts[0] = other->_pts[0];
  self->_pts[1] = other->_pts[1];
  self->_pts[2] = other->_pts[2];
}

static void FOG_CDECL GradientF_dtor(GradientF* self)
{
  self->_stops.destroy();
}

static err_t FOG_CDECL GradientF_copyF(GradientF* self, const GradientF* other)
{
  self->_gradientType = other->_gradientType;
  self->_gradientSpread = other->_gradientSpread;
  self->_stops() = other->_stops();

  self->_pts[0] = other->_pts[0];
  self->_pts[1] = other->_pts[1];
  self->_pts[2] = other->_pts[2];

  return ERR_OK;
}

static err_t FOG_CDECL GradientF_copyD(GradientF* self, const GradientD* other)
{
  self->_gradientType = other->_gradientType;
  self->_gradientSpread = other->_gradientSpread;
  self->_stops() = other->_stops();

  self->_pts[0] = other->_pts[0];
  self->_pts[1] = other->_pts[1];
  self->_pts[2] = other->_pts[2];

  return ERR_OK;
}

static void FOG_CDECL GradientF_reset(GradientF* self)
{
  self->_gradientType = GRADIENT_TYPE_INVALID;
  self->_gradientSpread = GRADIENT_SPREAD_DEFAULT;
  self->_stops().reset();

  self->_pts[0].reset();
  self->_pts[1].reset();
  self->_pts[2].reset();
}

static bool FOG_CDECL GradientF_eq(const GradientF* a, const GradientF* b)
{
  return a->_gradientType == b->_gradientType &&
         a->_gradientSpread == b->_gradientSpread &&
         a->_stops() == b->_stops() &&
         MemOps::eq(a->_pts, b->_pts, 3 * sizeof(PointF));
}

// ============================================================================
// [Fog::GradientD]
// ============================================================================

static void FOG_CDECL GradientD_ctor(GradientD* self, uint32_t gradientType)
{
  self->_gradientType = gradientType;
  self->_gradientSpread = GRADIENT_SPREAD_DEFAULT;
  self->_stops.init();
  self->_pts[0].reset();
  self->_pts[1].reset();
  self->_pts[2].reset();
}

static void FOG_CDECL GradientD_ctorCopyF(GradientD* self, const GradientF* other)
{
  self->_gradientType = other->_gradientType;
  self->_gradientSpread = other->_gradientSpread;
  self->_stops.initCustom1(other->_stops());
  self->_pts[0] = other->_pts[0];
  self->_pts[1] = other->_pts[1];
  self->_pts[2] = other->_pts[2];
}

static void FOG_CDECL GradientD_ctorCopyD(GradientD* self, const GradientD* other)
{
  self->_gradientType = other->_gradientType;
  self->_gradientSpread = other->_gradientSpread;
  self->_stops.initCustom1(other->_stops());
  self->_pts[0] = other->_pts[0];
  self->_pts[1] = other->_pts[1];
  self->_pts[2] = other->_pts[2];
}

static void FOG_CDECL GradientD_dtor(GradientD* self)
{
  self->_stops.destroy();
}

static err_t FOG_CDECL GradientD_copyF(GradientD* self, const GradientF* other)
{
  self->_gradientType = other->_gradientType;
  self->_gradientSpread = other->_gradientSpread;
  self->_stops() = other->_stops();

  self->_pts[0] = other->_pts[0];
  self->_pts[1] = other->_pts[1];
  self->_pts[2] = other->_pts[2];

  return ERR_OK;
}

static err_t FOG_CDECL GradientD_copyD(GradientD* self, const GradientD* other)
{
  self->_gradientType = other->_gradientType;
  self->_gradientSpread = other->_gradientSpread;
  self->_stops() = other->_stops();

  self->_pts[0] = other->_pts[0];
  self->_pts[1] = other->_pts[1];
  self->_pts[2] = other->_pts[2];

  return ERR_OK;
}

static void FOG_CDECL GradientD_reset(GradientD* self)
{
  self->_gradientType = GRADIENT_TYPE_INVALID;
  self->_gradientSpread = GRADIENT_SPREAD_DEFAULT;
  self->_stops().reset();

  self->_pts[0].reset();
  self->_pts[1].reset();
  self->_pts[2].reset();
}

static bool FOG_CDECL GradientD_eq(const GradientD* a, const GradientD* b)
{
  return a->_gradientType == b->_gradientType &&
         a->_gradientSpread == b->_gradientSpread &&
         a->_stops() == b->_stops() &&
         MemOps::eq(a->_pts, b->_pts, 3 * sizeof(PointD));
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Gradient_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.gradientf_ctor = GradientF_ctor;
  fog_api.gradientf_ctorCopyF = GradientF_ctorCopyF;
  fog_api.gradientf_ctorCopyD = GradientF_ctorCopyD;
  fog_api.gradientf_dtor = GradientF_dtor;
  fog_api.gradientf_copyF = GradientF_copyF;
  fog_api.gradientf_copyD = GradientF_copyD;
  fog_api.gradientf_reset = GradientF_reset;
  fog_api.gradientf_eq = GradientF_eq;

  fog_api.gradientd_ctor = GradientD_ctor;
  fog_api.gradientd_ctorCopyF = GradientD_ctorCopyF;
  fog_api.gradientd_ctorCopyD = GradientD_ctorCopyD;
  fog_api.gradientd_dtor = GradientD_dtor;
  fog_api.gradientd_copyF = GradientD_copyF;
  fog_api.gradientd_copyD = GradientD_copyD;
  fog_api.gradientd_reset = GradientD_reset;
  fog_api.gradientd_eq = GradientD_eq;
}

} // Fog namespace
