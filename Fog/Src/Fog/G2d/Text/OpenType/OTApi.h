// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OTAPI_H
#define _FOG_G2D_TEXT_OTAPI_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

// TrueType/OpenType font and data.
struct OTFace;
struct OTTable;

// TrueType/OpenType data members.
struct OTInt8;
struct OTInt16;
struct OTInt32;
struct OTUInt8;
struct OTUInt16;
struct OTUInt24;
struct OTUInt32;
struct OTIndex;

typedef OTInt16 OTFixed2x14;
typedef OTInt32 OTFixed16x16;

typedef OTUInt16 OTOffset16;
typedef OTUInt32 OTOffset32;

typedef OTUInt16 OTGlyphID;
typedef OTUInt32 OTFixedVersion;
typedef OTUInt32 OTTag;

// TrueType/OpenType 'cmap' support.
struct OTCMapContext;
struct OTCMapTable;

// ============================================================================
// [Funcs]
// ============================================================================

//! @addtogroup Fog_G2d_Text_OpenType
//! @{

typedef void (FOG_CDECL* OTFaceFreeTableDataFunc)(uint8_t* data, size_t dataLength);
typedef void (FOG_CDECL* OTTableDestroyFunc)(OTTable* table);

typedef err_t (FOG_CDECL* OTCMapInitContextFunc)(OTCMapContext* ctx, const OTCMapTable* table, uint32_t language);
typedef size_t (FOG_CDECL* OTCMapGetGlyphPlacementFunc)(OTCMapContext* ctx, uint32_t* glyphList, const uint16_t* str, size_t length);

//! @}

// ============================================================================
// [Defs]
// ============================================================================

//! @addtogroup Fog_G2d_Text_OpenType
//! @{

#define FOG_OT_TAG(_C0_, _C1_, _C2_, _C3_) uint32_t( \
  ((uint32_t)(_C0_) << 24) | \
  ((uint32_t)(_C1_) << 16) | \
  ((uint32_t)(_C2_) <<  8) | \
  ((uint32_t)(_C3_)      ) )

//! @}

// ============================================================================
// [Fog::OTApi]
// ============================================================================

struct FOG_NO_EXPORT OTApi
{
  // --------------------------------------------------------------------------
  // [OTCheckSum]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(uint32_t, otchecksum_calc)(const OTUInt32* pTable, uint32_t length);

  // --------------------------------------------------------------------------
  // [OTTable]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(otface_ctor)(OTFace* self);
  FOG_CAPI_DTOR(otface_dtor)(OTFace* self);
  FOG_CAPI_METHOD(bool, otface_hasTable)(const OTFace* self, OTTable* param);
  FOG_CAPI_METHOD(OTTable*, otface_getTable)(const OTFace* self, uint32_t tag);
  FOG_CAPI_METHOD(OTTable*, otface_addTable)(OTFace* self, uint32_t tag, uint8_t* data, uint32_t length);

  // --------------------------------------------------------------------------
  // [OTCMapTable]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, otcmaptable_init)(OTCMapTable* table);
};

} // Fog namespace

// ============================================================================
// [fog_ot_api]
// ============================================================================

//! @brief Fog-CAPI interface.
FOG_CVAR_EXTERN Fog::OTApi fog_ot_api;

// [Guard]
#endif // _FOG_G2D_TEXT_OTAPI_H
