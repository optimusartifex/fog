// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Tools/Dpi.h>

namespace Fog {

// ============================================================================
// [Fog::Dpi - Helpers]
// ============================================================================

static FOG_INLINE void Dpi_setup(float* data, float dpi, float em, float ex)
{
  data[UNIT_NONE] = 1.0f;
  data[UNIT_PX] = 1.0f;

  data[UNIT_PT] = dpi * 0.013888888889f;
  data[UNIT_PC] = dpi * 0.166666666667f;

  data[UNIT_IN] = dpi;
  data[UNIT_MM] = dpi * 0.039370077778f;
  data[UNIT_CM] = dpi * 0.393700777778f;

  data[UNIT_PERCENTAGE] = 1.0f;

  data[UNIT_EM] = em;
  data[UNIT_EX] = ex;
}

// ============================================================================
// [Fog::Dpi - Reset]
// ============================================================================

static void FOG_CDECL Dpi_reset(Dpi* self)
{
  _api.dpi_setDpiEmEx(self, 72.0f, 12.0f, 6.0f);
}

// ============================================================================
// [Fog::Dpi - Set]
// ============================================================================

static err_t FOG_CDECL Dpi_setDpi(Dpi* self, float dpi)
{
  if (dpi <= 0.0f)
  {
    _api.dpi_reset(self);
    return ERR_RT_INVALID_ARGUMENT;
  }

  Dpi_setup(self->_data, dpi, 0.0f, 0.0f);
  self->resetEmEx();
  return ERR_OK;
}

static err_t FOG_CDECL Dpi_setDpiEmEx(Dpi* self, float dpi, float em, float ex)
{
  if (dpi <= 0.0f)
  {
    _api.dpi_reset(self);
    return ERR_RT_INVALID_ARGUMENT;
  }

  Dpi_setup(self->_data, dpi, em, ex);

  if (em <= 0.0f || ex <= 0.0f)
    self->resetEmEx();
  return ERR_OK;
}

// ============================================================================
// [Fog::Dpi - Copy]
// ============================================================================

static void FOG_CDECL Dpi_copy(Dpi* self, const Dpi* other)
{
  MemOps::copy_t<Dpi>(self, other);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Dpi_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.dpi_reset = Dpi_reset;
  _api.dpi_setDpi = Dpi_setDpi;
  _api.dpi_setDpiEmEx = Dpi_setDpiEmEx;
  _api.dpi_copy = Dpi_copy;
}

} // Fog namespace
