// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OPENTYPE_OTAPI_H
#define _FOG_G2D_TEXT_OPENTYPE_OTAPI_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

// ============================================================================
// [Debugging]
// ============================================================================

#define FOG_OT_DEBUG

// ============================================================================
// [API]
// ============================================================================

// OpenType support was added to Fog-Framework to ensure that font files are
// interpreted using the same way on all platforms. OpenType structures are
// prefixed by 'OT', and the whole API, used by all classes is stored in 
// fog_ot_api structure.
//
// Fog-Framework generally follows the harfbuzz design, so many tables are 
// interpreted in original format performing conversion from big-endian to
// native-endian on-the-fly. This design ensures that font files can be loaded
// using mmap() on platforms where this feature is supported and where is 
// direct access to font files (API to locate font files). However, sometimes
// it's better to build a cache or to parse some specific parts of tables, so
// word 'Parsed' should be added to structures like these (eg. OTCMapParsedGroup).

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
struct OTCMap;
struct OTCMapContext;

// TrueType/OpenType 'hhea' support.
struct OTHHea;

// TrueType/OpenType 'head' support.
struct OTHead;

// TrueType/OpenType 'hmtx' support.
struct OTHmtx;

// TrueType/OpenType 'kern' support.
struct OTKern;

// ============================================================================
// [Funcs]
// ============================================================================

//! @addtogroup Fog_G2d_Text_OpenType
//! @{

// OTFace.
typedef void (FOG_CDECL* OTFaceFreeTableDataFunc)(OTFace* self, uint8_t* data, size_t dataLength);

// OTTable.
typedef void (FOG_CDECL* OTTableDestroyFunc)(OTTable* table);

// OTCMap aka 'cmap'.
typedef err_t (FOG_CDECL* OTCMapInitContextFunc)(OTCMapContext* ctx, const OTCMap* table, uint32_t encodingId);
typedef size_t (FOG_CDECL* OTCMapGetGlyphPlacementFunc)(OTCMapContext* ctx, uint32_t* glyphList, size_t glyphAdvance, const uint16_t* sData, size_t sLength);

//! @}

// ============================================================================
// [Defs]
// ============================================================================

//! @addtogroup Fog_G2d_Text_OpenType
//! @{

#define FOG_OT_TAG_NONE (0x00000000U)

//! @brief Create DWORD of TrueType/OpenType tag, which consists of 4 chars.
#define FOG_OT_TAG(_C0_, _C1_, _C2_, _C3_) uint32_t( \
  ((uint32_t)(_C0_) << 24) | \
  ((uint32_t)(_C1_) << 16) | \
  ((uint32_t)(_C2_) <<  8) | \
  ((uint32_t)(_C3_)      ) )

#define FOG_OT_LOADED(_Table_) ((uintptr_t)(_Table_) > 0x1)

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

  FOG_CAPI_METHOD(err_t, otface_initCoreTables)(OTFace* self);

  FOG_CAPI_METHOD(bool, otface_hasTable)(const OTFace* self, OTTable* param);
  FOG_CAPI_METHOD(OTTable*, otface_getTable)(const OTFace* self, uint32_t tag);

  FOG_CAPI_METHOD(OTTable*, otface_tryLoadTable)(OTFace* self, uint32_t tag);
  FOG_CAPI_METHOD(OTTable*, otface_addTable)(OTFace* self, uint32_t tag, uint8_t* data, uint32_t length);

  // --------------------------------------------------------------------------
  // [OTHead]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, othead_init)(OTHead* table);

  // --------------------------------------------------------------------------
  // [OTHHea]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, othhea_init)(OTHHea* table);

  // --------------------------------------------------------------------------
  // [OTHmtx]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, othmtx_init)(OTHmtx* table);

  // --------------------------------------------------------------------------
  // [OTCMap]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, otcmap_init)(OTCMap* table);

  // --------------------------------------------------------------------------
  // [OTKern]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, otkern_init)(OTKern* table);
};

} // Fog namespace

// ============================================================================
// [fog_ot_api]
// ============================================================================

//! @brief Fog-CAPI interface.
FOG_CVAR_EXTERN Fog::OTApi fog_ot_api;

// [Guard]
#endif // _FOG_G2D_TEXT_OPENTYPE_OTAPI_H
