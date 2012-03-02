// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/G2d/Text/OpenType/OTCore.h>

// ============================================================================
// [fog_ot_api]
// ============================================================================

FOG_CVAR_DECLARE Fog::OT_Api fog_ot_api;

namespace Fog {

// ============================================================================
// [OT_CheckSum]
// ============================================================================

static uint32_t FOG_CDECL OT_CheckSum_calc(const OT_ULONG* pTable, uint32_t length)
{
  // OT-Spec: This function implies that the length of a table must be a multiple
  // of four bytes. While this is not a requirement for the TrueType scaler,
  // itself it is suggested that all tables begin on four byte boundries, and
  // pad any remaining space between tables with zeros. The length of all tables
  // should be recorded in the table directory with their actual length.
  uint32_t sum = 0;

  // Align length to 4-bytes.
  length = (length + 3) >> 2;

  if (((uintptr_t)pTable & 0x3) == 0)
  {
    for (uint32_t i = 0; i < length; i++)
      sum += pTable[i].getValueA();
  }
  else
  {
    for (uint32_t i = 0; i < length; i++)
      sum += pTable[i].getValueU();
  }

  return sum;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OTCore_init(void)
{
  OT_Api& api = fog_ot_api;

  // --------------------------------------------------------------------------
  // [OT_CheckSum]
  // --------------------------------------------------------------------------

  api.ot_checksum_calc = OT_CheckSum_calc;
}


} // Fog namespace
