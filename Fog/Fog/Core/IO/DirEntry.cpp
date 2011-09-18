// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/IO/DirEntry.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

// ============================================================================
// [Fog::DirEntry - Helpers]
// ============================================================================

static FOG_INLINE void DirEntry_copyData(DirEntry* dst, const DirEntry* src)
{
#if defined(FOG_OS_WINDOWS)
  memcpy(&dst->_winFindData, &src->_winFindData, sizeof(WIN32_FIND_DATAW));
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  memcpy(&dst->_posixStatData, &src->_posixStatData, sizeof(struct stat));
#endif // FOG_OS_POSIX
}

// ============================================================================
// [Fog::DirEntry - Construction / Destruction]
// ============================================================================

static void FOG_CDECL DirEntry_ctor(DirEntry* self)
{
  self->_name.init();
  self->_type = DIR_ENTRY_UNKNOWN;
  self->_size = 0;
}

static void FOG_CDECL DirEntry_ctorCopy(DirEntry* self, const DirEntry* other)
{
  self->_name.initCustom1(other->_name);
  self->_type = other->_type;
  self->_size = other->_size;
  DirEntry_copyData(self, other);
}

static void FOG_CDECL DirEntry_dtor(DirEntry* self)
{
  self->_name.destroy();
}

// ============================================================================
// [Fog::DirEntry - Set]
// ============================================================================

static void FOG_CDECL DirEntry_setDirEntry(DirEntry* self, const DirEntry* other)
{
  self->_name->set(other->_name);
  self->_type = other->_type;
  self->_size = other->_size;
  DirEntry_copyData(self, other);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void DirEntry_init(void)
{
  _api.direntry.ctor = DirEntry_ctor;
  _api.direntry.ctorCopy = DirEntry_ctorCopy;
  _api.direntry.dtor = DirEntry_dtor;
  _api.direntry.setDirEntry = DirEntry_setDirEntry;
}

} // Fog namespace
