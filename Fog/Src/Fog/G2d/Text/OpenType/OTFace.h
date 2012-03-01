// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OTFACE_H
#define _FOG_G2D_TEXT_OTFACE_H

// [Dependencies]
#include <Fog/Core/Memory/MemZoneAllocator.h>
#include <Fog/G2d/Text/OpenType/OTCore.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::OT_Table]
// ============================================================================

struct FOG_NO_EXPORT OT_Table
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t* getData() const { return _data; }

  FOG_INLINE uint32_t getTag() const { return _tag; }
  FOG_INLINE uint32_t getLength() const { return _length; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief 'sfnt' data.
  uint8_t* _data;

  //! @brief Name of tag the data belongs to.
  uint32_t _tag;
  //! @brief Length of @c _data.
  uint32_t _length;
};

// ============================================================================
// [Fog::OT_LinkedTable]
// ============================================================================

struct FOG_NO_EXPORT OT_LinkedTable : public OT_Table
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  OT_LinkedTable* _next;
};

// ============================================================================
// [Fog::OT_Face]
// ============================================================================

struct FOG_NO_EXPORT OT_Face
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE OT_Face() :
    _allocator(240),
    _tableData(NULL),
    _freeTable(NULL)
  {
  }

  FOG_INLINE ~OT_Face()
  {
    OT_LinkedTable* table = _tableData;
    while (table != NULL)
    {
      OT_LinkedTable* next = table->_next;
      _freeTable(table);
      table = next;
    }
  }

  // --------------------------------------------------------------------------
  // [Tables]
  // --------------------------------------------------------------------------

  FOG_INLINE OT_Table* getTable(uint32_t tag) const
  {
    OT_LinkedTable* table = _tableData;

    while (table != NULL)
    {
      if (table->_tag == tag)
        return table;
    }

    return NULL;
  }

  FOG_INLINE OT_Table* addTable(uint32_t tag, uint8_t* data, uint32_t length)
  {
    OT_LinkedTable* table = static_cast<OT_LinkedTable*>(_allocator.alloc(sizeof(OT_LinkedTable)));

    if (FOG_IS_NULL(table))
      return NULL;

    table->_data = data;
    table->_tag = tag;
    table->_length = length;

    table->_next = _tableData;
    _tableData = table;

    return table;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  MemZoneAllocator _allocator;

  OT_LinkedTable* _tableData;
  OT_TableFreeFunc _freeTable;

private:
  _FOG_NO_COPY(OT_Face)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OTFACE_H
