// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/OS/FileInfo.h>
#include <Fog/Core/OS/FilePath.h>
#include <Fog/Core/OS/FileUtil.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>

#if defined(FOG_OS_WINDOWS)
# include <Fog/Core/OS/WinUtil.h>
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
# include <sys/stat.h>
#endif // FOG_OS_POSIX

namespace Fog {

// ============================================================================
// [Fog::FileInfo - Global]
// ============================================================================

static Static<FileInfoData> FileInfo_dNull;
static Static<FileInfo> FileInfo_oNull;

// ============================================================================
// [Fog::FileInfo - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FileInfo_ctor(FileInfo* self)
{
  self->_d = FileInfo_dNull->addRef();
}

static void FOG_CDECL FileInfo_ctorCopy(FileInfo* self, const FileInfo* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL FileInfo_dtor(FileInfo* self)
{
  FileInfoData* d = self->_d;

  if (d != NULL)
    d->release();
}

// ============================================================================
// [Fog::FileInfo - Sharing]
// ============================================================================

static err_t FOG_CDECL FileInfo_detach(FileInfo* self)
{
  FileInfoData* d = self->_d;

  if (d->reference.get() == 1)
    return ERR_OK;

  FileInfoData* newd = fog_api.fileinfo_dCreate(&d->fileName);

  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->fileFlags = d->fileFlags;
  newd->filePath = d->filePath;
  newd->size = d->size;
  newd->creationTime = d->creationTime;
  newd->modifiedTime = d->modifiedTime;
  newd->accessTime = d->accessTime;

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FileInfo - FromFile (Windows)]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
static err_t FOG_CDECL FileInfo_fromWinPath(FileInfoData* d, const StringW* path)
{
  if (path->startsWith(Ascii8("\\\\?\\")))
  {
    // Path contains DIRECTORY+FILE.
    size_t length = path->getLength();
    size_t sepIdx = path->lastIndexOf(Range(4, length), CharW('\\'));

    // In case that path ends with slash.
    if (sepIdx == length - 1)
    {
      length--;
      sepIdx = path->lastIndexOf(Range(4, length), CharW('\\'));
    }

    if (sepIdx == INVALID_INDEX)
      return ERR_PATH_NAME_INVALID;

    FOG_RETURN_ON_ERROR(d->filePath->setAndNormalizeSlashes(*path, Range(4, sepIdx), SLASH_FORM_UNIX));
    FOG_RETURN_ON_ERROR(d->fileName->set(*path, Range(sepIdx + 1, DETECT_LENGTH)));
  }
  else
  {
    // Path contains DIRECTORY only (root), for example C:\ is a root.
    FOG_RETURN_ON_ERROR(d->filePath->setAndNormalizeSlashes(*path, SLASH_FORM_UNIX));
    d->fileName->clear();
  }

  return ERR_OK;
}

static FOG_INLINE void FileInfo_fromWinFileAttributes(FileInfoData* d, DWORD dwFileAttributes)
{
  uint32_t fileFlags = FILE_INFO_EXISTS;

  if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
    fileFlags |= FILE_INFO_REGULAR_FILE;
  else
    fileFlags |= FILE_INFO_DIRECTORY;

  if ((dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
    fileFlags |= FILE_INFO_SYMLINK;

  if ((dwFileAttributes & FILE_ATTRIBUTE_DEVICE) != 0)
    fileFlags |= FILE_INFO_CHAR;

  if ((dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0)
    fileFlags |= FILE_INFO_HIDDEN;

  // Windows specific file attributes.
  if ((dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) != 0)
    fileFlags |= FILE_INFO_ARCHIVE;
  if ((dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) != 0)
    fileFlags |= FILE_INFO_COMPRESSED;
  if ((dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE) != 0)
    fileFlags |= FILE_INFO_SPARSE;
  if ((dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0)
    fileFlags |= FILE_INFO_SYSTEM;

  // Windows specific file attributes.
  if ((dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0)
    fileFlags |= FILE_INFO_CAN_READ;
  else
    fileFlags |= FILE_INFO_CAN_READ | FILE_INFO_CAN_WRITE;

  d->fileFlags = fileFlags;
}

static FOG_INLINE void FileInfo_fromWinFileAttributeDataPrivate(FileInfoData* d,
  const WIN32_FILE_ATTRIBUTE_DATA* wfad)
{
  FileInfo_fromWinFileAttributes(d, wfad->dwFileAttributes);
  d->size = (((uint64_t)wfad->nFileSizeHigh) << 32) | (uint64_t)wfad->nFileSizeLow;
}

static err_t FOG_CDECL FileInfo_fromFile(FileInfo* self, const StringW* path)
{
  if (path->getLength() == 0)
    return ERR_PATH_NAME_INVALID;

  StringTmpW<TEMPORARY_LENGTH> pathW;
  FOG_RETURN_ON_ERROR(WinUtil::makeWinPath(pathW, *path));

  WIN32_FILE_ATTRIBUTE_DATA wfad;
  if (!GetFileAttributesExW(
    reinterpret_cast<const wchar_t*>(pathW.getData()),
    GetFileExInfoStandard,
    &wfad))
  {
    return WinUtil::getErrFromWinLastError();
  }

  FOG_RETURN_ON_ERROR(self->detach());
  FileInfoData* d = self->_d;

  FOG_RETURN_ON_ERROR(FileInfo_fromWinPath(d, &pathW));

  FileInfo_fromWinFileAttributeDataPrivate(d, &wfad);
  return ERR_OK;
}

static err_t FOG_CDECL FileInfo_fromFileEx(FileInfo* self, const StringW* filePath, const StringW* fileName)
{
  if (filePath->getLength() == 0)
    return ERR_PATH_NAME_INVALID;

  StringTmpW<TEMPORARY_LENGTH> pathW;

  FOG_RETURN_ON_ERROR(WinUtil::makeWinPath(pathW, *filePath));
  FOG_RETURN_ON_ERROR(pathW.append(CharW('\\')));
  FOG_RETURN_ON_ERROR(pathW.append(*fileName));

  WIN32_FILE_ATTRIBUTE_DATA wfad;
  if (!GetFileAttributesExW(
    reinterpret_cast<const wchar_t*>(pathW.getData()),
    GetFileExInfoStandard,
    &wfad))
  {
    return WinUtil::getErrFromWinLastError();
  }

  FOG_RETURN_ON_ERROR(self->detach());
  FileInfoData* d = self->_d;

  FOG_RETURN_ON_ERROR(FileInfo_fromWinPath(d, &pathW));

  FileInfo_fromWinFileAttributeDataPrivate(d, &wfad);
  return ERR_OK;
}

static err_t FOG_CDECL FileInfo_fromWinFileAttributeData(FileInfo* self, const StringW* filePath, const StringW* fileName, const void* _wfad)
{
  FOG_RETURN_ON_ERROR(self->detach());
  FileInfoData* d = self->_d;

  const WIN32_FILE_ATTRIBUTE_DATA* wfad = reinterpret_cast<const WIN32_FILE_ATTRIBUTE_DATA*>(_wfad);

  FOG_RETURN_ON_ERROR(d->filePath->set(*filePath));
  FOG_RETURN_ON_ERROR(d->fileName->set(*fileName));

  // Not fatal if these fails, they shouldn't in fact.
  d->creationTime.fromFILETIME(wfad->ftCreationTime);
  d->modifiedTime.fromFILETIME(wfad->ftLastWriteTime);
  d->accessTime.fromFILETIME(wfad->ftLastAccessTime);

  FileInfo_fromWinFileAttributeDataPrivate(d, wfad);
  return ERR_OK;
}

static err_t FOG_CDECL FileInfo_fromWinFindData(FileInfo* self, const StringW* filePath, const void* _wfd)
{
  FOG_RETURN_ON_ERROR(self->detach());
  FileInfoData* d = self->_d;

  const WIN32_FIND_DATAW* wfd = reinterpret_cast<const WIN32_FIND_DATAW*>(_wfd);

  FileInfo_fromWinFileAttributes(d, wfd->dwFileAttributes);
  d->size = ((uint64_t)(wfd->nFileSizeHigh) << 32) | ((uint64_t)(wfd->nFileSizeLow));

  FOG_RETURN_ON_ERROR(d->filePath->set(*filePath));
  FOG_RETURN_ON_ERROR(d->fileName->setWChar(wfd->cFileName));

  // Not fatal if these fails, they shouldn't in fact.
  d->creationTime.fromFILETIME(wfd->ftCreationTime);
  d->modifiedTime.fromFILETIME(wfd->ftLastWriteTime);
  d->accessTime.fromFILETIME(wfd->ftLastAccessTime);

  return ERR_OK;
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::FileInfo - FromFile (Posix)]
// ============================================================================

#if defined(FOG_OS_POSIX)
static void FileInfo_fillStatData(FileInfoData* d, const struct stat* s)
{
  uint32_t flags = FILE_INFO_EXISTS;

  // S_ISXXX are POSIX macros to get a file type.
  if (S_ISREG(s->st_mode))
    flags |= FILE_INFO_REGULAR_FILE;

  if (S_ISDIR(s->st_mode))
    flags |= FILE_INFO_DIRECTORY;

#if defined(S_ISLNK)
  if (S_ISLNK(s->st_mode))
    flags |= FILE_INFO_SYMLINK;
#endif // S_ISLNK

#if defined(S_ISCHR)
  if (S_ISCHR(s->st_mode))
    flags |= FILE_INFO_CHAR;
#endif // S_ISCHR

#if defined(S_ISBLK)
  if (S_ISBLK(s->st_mode))
    flags |= FILE_INFO_BLOCK;
#endif // S_ISBLK

#if defined(S_ISFIFO)
  if (S_ISFIFO(s->st_mode))
    flags |= FILE_INFO_FIFO;
#endif // S_ISFIFO

#if defined(S_ISSOCK)
  if (S_ISSOCK(s->st_mode))
    flags |= FILE_INFO_SOCKET;
#endif // S_ISSOCK

  d->fileFlags = flags;
  d->size = 0;

  if (flags & FILE_INFO_REGULAR_FILE)
    d->size = (uint64_t)s->st_size;

  d->creationTime.reset();
  d->modifiedTime.fromTimeT(s->st_mtime);
  d->accessTime.fromTimeT(s->st_atime);
}

static err_t FOG_CDECL FileInfo_fromFile(FileInfo* self, const StringW* path)
{
  FOG_RETURN_ON_ERROR(self->detach());
  FileInfoData* d = self->_d;

  StringTmpW<TEMPORARY_LENGTH> pathAbs;
  FOG_RETURN_ON_ERROR(FilePath::toAbsolute(pathAbs, *path));

  struct stat s;
  FOG_RETURN_ON_ERROR(FileUtil::stat(&s, *path));

  FileInfo_fillStatData(d, &s);
  FOG_RETURN_ON_ERROR(FilePath::extractDirectory(d->filePath(), pathAbs));
  FOG_RETURN_ON_ERROR(FilePath::extractFile(d->fileName(), pathAbs));
  return ERR_OK;
}

static err_t FOG_CDECL FileInfo_fromFileEx(FileInfo* self, const StringW* filePath, const StringW* fileName)
{
  FOG_RETURN_ON_ERROR(self->detach());
  FileInfoData* d = self->_d;

  StringTmpW<TEMPORARY_LENGTH> path;
  StringTmpW<TEMPORARY_LENGTH> pathAbs;

  FOG_RETURN_ON_ERROR(FilePath::join(path, *filePath, *fileName));
  FOG_RETURN_ON_ERROR(FilePath::toAbsolute(pathAbs, path));

  struct stat s;
  FOG_RETURN_ON_ERROR(FileUtil::stat(&s, pathAbs));

  FileInfo_fillStatData(d, &s);
  FOG_RETURN_ON_ERROR(FilePath::extractDirectory(d->filePath(), pathAbs));
  FOG_RETURN_ON_ERROR(FilePath::extractFile(d->fileName(), pathAbs));
  return ERR_OK;
}

static err_t FOG_CDECL FileInfo_fromStat(FileInfo* self, const StringW* filePath, const StringW* fileName, const void* s)
{
  FOG_RETURN_ON_ERROR(self->detach());
  FileInfoData* d = self->_d;

  FileInfo_fillStatData(d, reinterpret_cast<const struct stat*>(s));
  FOG_RETURN_ON_ERROR(d->filePath->set(*filePath));
  FOG_RETURN_ON_ERROR(d->fileName->set(*fileName));
  return ERR_OK;
}
#endif // FOG_OS_POSIX

// ============================================================================
// [Fog::FileInfo - Copy]
// ============================================================================

static err_t FOG_CDECL FileInfo_copy(FileInfo* self, const FileInfo* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::FileInfo - FileInfoData]
// ============================================================================

static FileInfoData* FOG_CDECL FileInfo_dCreate(const StringW* fileName)
{
  FileInfoData* d = reinterpret_cast<FileInfoData*>(MemMgr::alloc(sizeof(FileInfoData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);

  // TODO: Var support
  // d->vType = VAR_TYPE_FILE_INFO;

  d->fileFlags = NO_FLAGS;
  d->fileName.initCustom1(*fileName);
  d->size = 0;

  return d;
}

static void FOG_CDECL FileInfo_dFree(FileInfoData* d)
{
  d->fileName.destroy();
  MemMgr::free(d);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FileInfo_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.fileinfo_ctor = FileInfo_ctor;
  fog_api.fileinfo_ctorCopy = FileInfo_ctorCopy;
  fog_api.fileinfo_dtor = FileInfo_dtor;
  fog_api.fileinfo_detach = FileInfo_detach;
  fog_api.fileinfo_fromFile = FileInfo_fromFile;
  fog_api.fileinfo_fromFileEx = FileInfo_fromFileEx;

#if defined(FOG_OS_WINDOWS)
  fog_api.fileinfo_fromWinFileAttributeData = FileInfo_fromWinFileAttributeData;
  fog_api.fileinfo_fromWinFindData = FileInfo_fromWinFindData;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  fog_api.fileinfo_fromStat = FileInfo_fromStat;
#endif // FOG_OS_POSIX

  fog_api.fileinfo_copy = FileInfo_copy;

  fog_api.fileinfo_dCreate = FileInfo_dCreate;
  fog_api.fileinfo_dFree = FileInfo_dFree;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  FileInfoData* d = &FileInfo_dNull;
  d->reference.init(1);
  d->filePath.initCustom1(fog_api.stringw_oEmpty->_d);
  d->fileName.initCustom1(fog_api.stringw_oEmpty->_d);

  fog_api.fileinfo_oNull = FileInfo_oNull.initCustom1(d);
}

} // Fog namespace
