// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Text/OpenType/OTCMap.h>
#include <Fog/G2d/Text/OpenType/OTEnum.h>
#include <Fog/G2d/Text/OpenType/OTFace.h>
#include <Fog/G2d/Text/OpenType/OTHHea.h>
#include <Fog/G2d/Text/OpenType/OTHead.h>
#include <Fog/G2d/Text/OpenType/OTHmtx.h>
#include <Fog/G2d/Text/OpenType/OTKern.h>

namespace Fog {

// ============================================================================
// [OTFace - Construction / Destruction]
// ============================================================================

static void FOG_CDECL OTFace_ctor(OTFace* self)
{
  self->_face = NULL;

  self->_tableData = NULL;
  self->_freeTableDataFunc = NULL;

  self->_head = NULL;
  self->_cmap = NULL;

  self->_allocator.initCustom1(488);
}

static void FOG_CDECL OTFace_dtor(OTFace* self)
{
  OTTable* table = self->_tableData;
  OTFaceFreeTableDataFunc freeTable = self->_freeTableDataFunc;

  while (table != NULL)
  {
    OTTable* next = table->_next;

    if (table->_destroy)
      table->_destroy(table);
    freeTable(self, table->_data, table->_dataLength);

    table = next;
  }

  self->_allocator.destroy();
}

// ============================================================================
// [OTFace - Core Table Support]
// ============================================================================

static err_t FOG_CDECL OTFace_initCoreTables(OTFace* self)
{
  OTHead* head = self->_head = reinterpret_cast<OTHead*>(self->tryLoadTable(FOG_OT_TAG('h', 'e', 'a', 'd')));
  OTCMap* cmap = self->_cmap = reinterpret_cast<OTCMap*>(self->tryLoadTable(FOG_OT_TAG('c', 'm', 'a', 'p')));

  if (head == NULL || FOG_IS_ERROR(head->getStatus())) return head->getStatus();
  if (cmap == NULL || FOG_IS_ERROR(cmap->getStatus())) return cmap->getStatus();

  return ERR_OK;
}

// ============================================================================
// [OTFace - Specific Table Support]
// ============================================================================

static size_t FOG_CDECL OTFace_getTableSize(uint32_t tag)
{
  switch (tag)
  {
    case FOG_OT_TAG('h', 'e', 'a', 'd'): return sizeof(OTHead);
    case FOG_OT_TAG('h', 'h', 'e', 'a'): return sizeof(OTHHea);
    case FOG_OT_TAG('h', 'm', 't', 'x'): return sizeof(OTHmtx);
    case FOG_OT_TAG('c', 'm', 'a', 'p'): return sizeof(OTCMap);
    case FOG_OT_TAG('k', 'e', 'r', 'n'): return sizeof(OTKern);

    default: return sizeof(OTTable);
  }
}

static err_t FOG_CDECL OTFace_initTable(OTFace* self, OTTable* table)
{
  switch (table->_tag)
  {
    case FOG_OT_TAG('h', 'e', 'a', 'd'): return fog_ot_api.othead_init(static_cast<OTHead*>(table));
    case FOG_OT_TAG('h', 'h', 'e', 'a'): return fog_ot_api.othhea_init(static_cast<OTHHea*>(table));
    case FOG_OT_TAG('h', 'm', 't', 'x'): return fog_ot_api.othmtx_init(static_cast<OTHmtx*>(table));
    case FOG_OT_TAG('c', 'm', 'a', 'p'): return fog_ot_api.otcmap_init(static_cast<OTCMap*>(table));
    case FOG_OT_TAG('k', 'e', 'r', 'n'): return fog_ot_api.otkern_init(static_cast<OTKern*>(table));

    default: return ERR_OK;
  }
}

// ============================================================================
// [OTFace - Additional Tables - HasTable]
// ============================================================================

static bool FOG_CDECL OTFace_hasTable(const OTFace* self, OTTable* param)
{
  OTTable* tab = AtomicCore<OTTable*>::get(&self->_tableData);
  while (tab != NULL)
  {
    if (tab == param)
      return true;
    tab = tab->_next;
  }
  return false;
}

// ============================================================================
// [OTFace - Additional Tables - GetTable]
// ============================================================================

static OTTable* FOG_CDECL OTFace_getTable(const OTFace* self, uint32_t tag)
{
  OTTable* tab = AtomicCore<OTTable*>::get(&self->_tableData);
  while (tab != NULL)
  {
    if (tab->_tag == tag)
      return tab;
    tab = tab->_next;
  }
  return NULL;
}

// ============================================================================
// [OTFace - Additional Tables - AddTable]
// ============================================================================

static OTTable* FOG_CDECL OTFace_tryLoadTable(OTFace* self, uint32_t tag)
{
  OTTable* table = self->getTable(tag);
  if (table != NULL)
    return table;

  Face* face = self->_face;
  table = face->vtable->getOTTable(face, tag);

  if (table == NULL)
    return (OTTable*)(uintptr_t)0x1;
  else
    return table;
}

static OTTable* FOG_CDECL OTFace_addTable(OTFace* self, uint32_t tag, uint8_t* data, uint32_t length)
{
  size_t tabSize = OTFace_getTableSize(tag);
  OTTable* tab = static_cast<OTTable*>(self->_allocator->alloc(tabSize));

  if (FOG_IS_NULL(tab))
    return NULL;

  tab->_tag = tag;
  tab->_dataLength = length;
  tab->_data = data;

  tab->_face = self;
  tab->_next = NULL;

  tab->_destroy = NULL;
  tab->_status = ERR_OK;
  OTFace_initTable(self, tab);

  OTTable* old;
  do {
    old = AtomicCore<OTTable*>::get(&self->_tableData);
    tab->_next = old;
  } while (!AtomicCore<OTTable*>::cmpXchg(&self->_tableData, old, tab));

  return tab;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void OTFace_init(void)
{
  OTApi& api = fog_ot_api;

  // --------------------------------------------------------------------------
  // [OTFace]
  // --------------------------------------------------------------------------

  api.otface_ctor = OTFace_ctor;
  api.otface_dtor = OTFace_dtor;

  api.otface_initCoreTables = OTFace_initCoreTables;

  api.otface_hasTable = OTFace_hasTable;
  api.otface_getTable = OTFace_getTable;

  api.otface_tryLoadTable = OTFace_tryLoadTable;
  api.otface_addTable = OTFace_addTable;
}

} // Fog namespace
