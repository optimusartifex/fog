// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/MemBufferTmp_p.h>
#include <Fog/Core/OS/Environment.h>
#include <Fog/Core/OS/FileMapping.h>
#include <Fog/Core/OS/FilePath.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/OS/UserUtil.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>

// [Dependencies - Windows]
#if defined(FOG_OS_WINDOWS)
# include <Objbase.h>
# include <Shlobj.h>
#endif // FOG_OS_WINDOWS

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <stdlib.h>
# include <pwd.h>
// for getuid() and getgid()
# if defined(FOG_HAVE_UNISTD_H)
#  include <unistd.h>
# endif // FOG_HAVE_UNISTD_H
#endif // FOG_OS_POSIX

namespace Fog {

#if defined(FOG_OS_WINDOWS)

// ============================================================================
// [Fog::UserUtil - UID / GID (Windows)]
// ============================================================================

static uint32_t FOG_CDECL UserUtil_getUid(void)
{
  // TODO: Is there a way how to get UID? Using SID?
  return 0;
}

static uint32_t FOG_CDECL UserUtil_getGid(void)
{
  // TODO: Is there a way how to get GID? Using SID?
  return 0;
}

// ============================================================================
// [Fog::UserUtil - Special (Windows)]
// ============================================================================

static err_t FOG_CDECL UserUtil_getSpecialDirectory(StringW* dst, DWORD csidl, const Ascii8& bail)
{
  LPITEMIDLIST item;
  HRESULT res = ::SHGetSpecialFolderLocation(NULL, csidl, &item);

  if (res == S_OK)
  {
    FOG_RETURN_ON_ERROR(dst->reserve(MAX_PATH));
    CharW *dstData = dst->getDataX();

    BOOL success = ::SHGetPathFromIDListW(item, reinterpret_cast<wchar_t*>(dstData));
    ::CoTaskMemFree(item);

    if (!success)
      goto _Fail;

    dst->_modified(dstData + StringUtil::len(dstData));
    return ERR_OK;
  }
  else
  {
_Fail:
    dst->set(bail);
    FOG_RETURN_ON_ERROR(FilePath::substituteEnvironmentVars(*dst, *dst, 
      FILE_PATH_SUBSTITUTE_FORMAT_WINDOWS));

    return ERR_OK;
  }
}

// ============================================================================
// [Fog::UserUtil - Home (Windows)]
// ============================================================================

static err_t FOG_CDECL UserUtil_getHomeDirectory(StringW* dst)
{
  return Environment::getValue(FOG_STR_(OSUTIL_USERPROFILE), *dst);
}

// ============================================================================
// [Fog::UserUtil - User Directory (Windows)]
// ============================================================================

// ${USER_DIRECTORY:BEGIN}
static const char* UserUtil_userDirectoryNames[] =
{
  NULL,
  NULL,
  NULL,
  NULL,
  "Desktop",
  "Personal",
  "My Music",
  "My Pictures",
  "My Video"
};
// ${USER_DIRECTORY:END}

// TODO: Update to WChar.
// fixed: documents, music and videos returns as home directory
static err_t FOG_CDECL UserUtil_registryLookupDir(StringW* dst, uint32_t userDirectory)
{
  FOG_ASSERT(userDirectory < FOG_ARRAY_SIZE(UserUtil_userDirectoryNames));
  FOG_ASSERT(UserUtil_userDirectoryNames[userDirectory] != NULL);

  // Dst should be empty.
  FOG_ASSERT(dst->isEmpty());

  HKEY k;
  DWORD d = 0, v_type, size;
  const char *reg_path;
  bool tryShellFolders = true;

  MemBufferTmp<512> bufStorage;
  MemBufferTmp<512> expandedStorage;

  char* buf;
  char* expanded;

  UserUtil_getHomeDirectory(dst);

  const char* type = UserUtil_userDirectoryNames[userDirectory];
  reg_path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders";

_back:
  if (RegCreateKeyExA(HKEY_CURRENT_USER, reg_path, 0, NULL, REG_OPTION_NON_VOLATILE,
      KEY_SET_VALUE          |
      KEY_CREATE_SUB_KEY     |
      KEY_ENUMERATE_SUB_KEYS |
      KEY_EXECUTE            |
      KEY_QUERY_VALUE        , NULL, &k, &d ) != ERROR_SUCCESS)
  {
    return ERR_USER_NO_DIRECTORY;
  }

  if (RegQueryValueExA(k, type, NULL, &v_type, NULL, &size ) == ERROR_SUCCESS &&
      size != 0 &&
      (v_type == REG_SZ || v_type == REG_EXPAND_SZ))
  {
    buf = (char*)bufStorage.alloc((size_t)size);
    RegQueryValueExA(k, type, NULL, NULL, (LPBYTE)buf, &size);
    if (*buf)
    {
      TextCodec::local8().decode(*dst, StubA(buf, size));
      if (v_type == REG_EXPAND_SZ)
      {
        size = ExpandEnvironmentStringsA(buf, NULL, 0);
        if (size)
        {
          size += 2;
          expanded = (char*)expandedStorage.alloc(size);

          ExpandEnvironmentStringsA(buf, expanded, size);
          if (expanded[0])
          {
            TextCodec::local8().decode(*dst, StubA(expanded, DETECT_LENGTH));
          }
        }
      }
    }
  }
  else if (tryShellFolders)
  {
    tryShellFolders = false;
    RegCloseKey(k);
    reg_path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
    goto _back;
  }

  RegCloseKey(k);
  return true;
}

static err_t FOG_CDECL UserUtil_getUserDirectory(StringW* dst, uint32_t userDirectory)
{
  dst->clear();

  if (userDirectory >= USER_DIRECTORY_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  err_t err;
  switch (userDirectory)
  {
    case USER_DIRECTORY_HOME:
      err = UserUtil_getHomeDirectory(dst);
      break;
    
    case USER_DIRECTORY_CONFIG:
      err = UserUtil_getSpecialDirectory(dst, CSIDL_LOCAL_APPDATA, Ascii8("~/Local Settings/Application Data"));
      break;

    case USER_DIRECTORY_DATA:
      err = UserUtil_getSpecialDirectory(dst, CSIDL_LOCAL_APPDATA, Ascii8("~/Local Settings/Application Data"));
      break;

    case USER_DIRECTORY_CACHE:
      err = UserUtil_getSpecialDirectory(dst, CSIDL_INTERNET_CACHE, Ascii8("~/Local Settings/Temporary Internet Files"));
      break;
    
    default:
      err = UserUtil_registryLookupDir(dst, userDirectory);
      break;
  }

  dst->normalizeSlashes(SLASH_FORM_UNIX);
  return err;
}
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)

// ============================================================================
// [Fog::UserUtil - UID / GID (Posix)]
// ============================================================================

static uint32_t FOG_CDECL UserUtil_getUid()
{
  return (uint32_t)(::getuid());
}

static uint32_t FOG_CDECL UserUtil_getGid()
{
  return (uint32_t)(::getgid());
}

// ============================================================================
// [Fog::UserUtil - EnvDirectory (Posix)]
// ============================================================================

static err_t FOG_CDECL UserUtil_getEnvDirectory(StringW* dst, const Ascii8& key, const Ascii8& bail)
{
  char* s = getenv(key.getData());
  
  if (s != NULL && s[0])
    FOG_RETURN_ON_ERROR(TextCodec::local8().decode(*dst, StubA(s)));
  else
    // Environment variable not found, so use the provided bail.
    FOG_RETURN_ON_ERROR(dst->set(bail));

  return FilePath::substituteEnvironmentVars(*dst, *dst, FILE_PATH_SUBSTITUTE_FORMAT_UNIX);
}

// ============================================================================
// [Fog::UserUtil - Home (Posix)]
// ============================================================================

static err_t FOG_CDECL UserUtil_getHomeDirectory(StringW* dst)
{
  struct passwd *pwd = getpwuid(UserUtil::getUid());

  if (pwd != NULL)
    return TextCodec::local8().decode(*dst, StubA(pwd->pw_dir, DETECT_LENGTH));

  char* s;

  if ((s = getenv("HOME")) != NULL && s[0])
    return TextCodec::local8().decode(*dst, StubA(s, DETECT_LENGTH));

  if ((s = getenv("TMPDIR")) != NULL && s[0])
    return TextCodec::local8().decode(*dst, StubA(s, DETECT_LENGTH));

  FOG_RETURN_ON_ERROR(dst->set(Ascii8("/tmp")));
  return ERR_USER_NO_DIRECTORY;
}

// ============================================================================
// [Fog::UserUtil - User Directory (Posix)]
// ============================================================================

// Based on XDG specification:
//   http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html

// ${USER_DIRECTORY:BEGIN}
static const char* UserUtil_userDirectoryNames[] =
{
  NULL,
  NULL,
  NULL,
  NULL,
  "XDG_DESKTOP_DIR",
  "XDG_DOCUMENTS_DIR",
  "XDG_MUSIC_DIR",
  "XDG_PICTURES_DIR",
  "XDG_VIDEOS_DIR"
};
// ${USER_DIRECTORY:END}

// TODO: More work needed to make it functional.
//static err_t FOG_CDECL UserUtil_getXdgConfigFile(StringW* dst)
//{
//  StringW config;
//
//  if (Environment::getValue(Ascii8("XDG_CONFIG_HOME"), configPath) != ERR_OK || configPath.isEmpty())
//    config.set()
//}

static err_t FOG_CDECL UserUtil_getXdgDirectory(StringW* dst, uint32_t userDirectory)
{
  FOG_ASSERT(userDirectory < FOG_ARRAY_SIZE(UserUtil_userDirectoryNames));
  FOG_ASSERT(UserUtil_userDirectoryNames[userDirectory] != NULL);
  
  // Dst should be empty.
  FOG_ASSERT(dst->isEmpty());

  StringW home, configPath;
  FOG_RETURN_ON_ERROR(UserUtil_getHomeDirectory(&home));

  if (Environment::getValue(Ascii8("XDG_CONFIG_HOME"), configPath) != ERR_OK || configPath.isEmpty())
  {
    // XDG_CONFIG_HOME defaults to ~/.config if not set.
    configPath.set(home);
    configPath.append(Ascii8("/.config"));
  }

  configPath.append(Ascii8("/user-dirs.dirs"));

  const char* type = UserUtil_userDirectoryNames[userDirectory];
  size_t typeLength = strlen(type);

  const char* mark;
  const char* end;
  const char* p;

  size_t remain;
  int relative;

  FileMapping fileMapping;
  err_t err = fileMapping.open(configPath, FILE_MAPPING_FLAG_LOAD_FALLBACK);
  
  if (FOG_IS_ERROR(err))
    goto _Fail;

  // Tokenize and parse the lines in XDG.
  p = reinterpret_cast<const char*>(fileMapping.getData());
  end = p + fileMapping.getLength();

  for (;;)
  {
    // Skip spaces and empty lines.
    while (p != end && CharA::isSpace(*p)) p++;
    if (p == end)
      break;

    // We are looking for XDG_SOME_DIR="VALUE".
    if ((size_t)(end - p) <= typeLength || memcmp(p, type, typeLength) != 0)
      goto _NextLine;

    p += typeLength;

    // Skip spaces and tabs.
    while (p != end && *p == ' ' || *p == '\t') p++;
    if (p == end)
      break;

    // Parse '='.
    if (*p != '=')
      goto _NextLine;

    // Skip spaces and tabs.
    while (p != end && *p == ' ' || *p == '\t') p++;
    if (p == end)
      break;

    // Parse '"'.
    if (*p != '\"')
      goto _NextLine;

    mark = p++;

    while (p != end && *p != '"' && *p != '\n') p++;
    if (p == end || *p == '\n')
      continue;

    // Everything looks OK. We have XDG name and value, we need only to
    // postprocess it and return. Postprocess means to parse $HOME, maybe also
    // other environment variables?
    relative = false;
    remain = (size_t)(p - mark);

    if (remain == 0)
      goto _NextLine;

    if (remain >= 6 && memcmp(mark, "$HOME/", 6) == 0)
    {
      mark += 6;
      remain -= 6;
      relative = true;
    }
    else if (*mark != '/')
    {
      goto _NextLine;
    }

    if (relative)
    {
      FOG_RETURN_ON_ERROR(dst->setDeep(home));
      FOG_RETURN_ON_ERROR(dst->append(CharW('/')));
    }

    return TextCodec::local8().decode(*dst, StubA(mark, remain), NULL, CONTAINER_OP_APPEND);

_NextLine:
    while (p != end && *p != '\n') p++;
    if (p == end)
      break;
  }

_Fail:
  // Just fallback to safe defaults.
  if (userDirectory == USER_DIRECTORY_DESKTOP)
  {
    FOG_RETURN_ON_ERROR(dst->set(home));
    FOG_RETURN_ON_ERROR(dst->append(Ascii8("/Desktop")));
    return ERR_OK;
  }

  return ERR_USER_NO_DIRECTORY;
}

static err_t FOG_CDECL UserUtil_getUserDirectory(StringW* dst, uint32_t userDirectory)
{
  dst->clear();

  if (userDirectory >= USER_DIRECTORY_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  switch (userDirectory)
  {
    case USER_DIRECTORY_HOME:
      return UserUtil_getHomeDirectory(dst);

    case USER_DIRECTORY_CONFIG:
      return UserUtil_getEnvDirectory(dst, Ascii8("XDG_CONFIG_HOME"), Ascii8("~/.config"));

    case USER_DIRECTORY_DATA:
      return UserUtil_getEnvDirectory(dst, Ascii8("XDG_DATA_HOME"), Ascii8("~/.local/share"));

    case USER_DIRECTORY_CACHE:
      return UserUtil_getEnvDirectory(dst, Ascii8("XDG_CACHE_HOME"), Ascii8("~/.cache"));

    default:
      return UserUtil_getXdgDirectory(dst, userDirectory);
  }
}
#endif

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void UserUtil_init(void)
{
  _api.userutil_getUid = UserUtil_getUid;
  _api.userutil_getGid = UserUtil_getGid;
  _api.userutil_getUserDirectory = UserUtil_getUserDirectory;
}

} // Fog namespace
