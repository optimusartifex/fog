// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Text/OpenType/OTFace.h>
#include <Fog/G2d/Text/OpenType/OTCMap.h>

namespace Fog {

// ============================================================================
// [OTFace - Construction / Destruction]
// ============================================================================

static void FOG_CDECL OTFace_ctor(OTFace* self)
{
  self->_face = NULL;

  self->_tableData = NULL;
  self->_freeTableDataFunc = NULL;

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
// [OTFace - Specific Table Support]
// ============================================================================

static size_t OTFace_getTableSize(uint32_t tag)
{
  switch (tag)
  {
    case FOG_OT_TAG('c', 'm', 'a', 'p'): return sizeof(OTCMapTable);

    default: return sizeof(OTTable);
  }
}

static err_t OTFace_initTable(OTFace* self, OTTable* table)
{
  switch (table->_tag)
  {
    case FOG_OT_TAG('c', 'm', 'a', 'p'): return fog_ot_api.otcmaptable_init(static_cast<OTCMapTable*>(table));

    default: return ERR_OK;
  }
}

// ============================================================================
// [OTFace - HasTable]
// ============================================================================

static bool FOG_CDECL OTFace_hasTable(const OTFace* self, OTTable* param)
{
  OTTable* tab = AtomicCore<OTTable*>::get(&self->_tableData);
  while (tab != NULL)
  {
    if (tab == param)
      return true;
  }
  return false;
}

// ============================================================================
// [OTFace - GetTable]
// ============================================================================

static OTTable* FOG_CDECL OTFace_getTable(const OTFace* self, uint32_t tag)
{
  OTTable* tab = AtomicCore<OTTable*>::get(&self->_tableData);
  while (tab != NULL)
  {
    if (tab->_tag == tag)
      return tab;
  }
  return NULL;
}

// ============================================================================
// [OTFace - AddTable]
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

  api.otface_hasTable = OTFace_hasTable;
  api.otface_getTable = OTFace_getTable;

  api.otface_tryLoadTable = OTFace_tryLoadTable;
  api.otface_addTable = OTFace_addTable;
}

} // Fog namespace
