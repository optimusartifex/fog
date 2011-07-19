// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/BufferP.h>
#include <Fog/Core/IO/MapFile.h>
#include <Fog/Core/OS/OS.h>
#include <Fog/Core/OS/UserInfo.h>
#include <Fog/Core/Tools/Byte.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Strings.h>
#include <Fog/Core/Tools/TextCodec.h>

#if defined(FOG_OS_POSIX)
#include <stdlib.h>
#include <pwd.h>
// for getuid() and getgid()
#if defined(FOG_HAVE_UNISTD_H)
#include <unistd.h>
#include <X11/X.h>
#endif // FOG_HAVE_UNISTD_H
#endif

namespace Fog {

static err_t getHomeDirectory(String& dst);

#if defined(FOG_OS_WINDOWS)

static const char* winDirectoryNames[] =
{
  "",
  "Desktop",
  "Personal",
  "My Music",
  "My Pictures",
  "My Video"
};

// TODO: Update to WChar.
// fixed: documents, music and videos returns as home directory
static err_t registryLookupDir(String& dst, int id)
{
  FOG_ASSERT(id != 0 && (uint)id < FOG_ARRAY_SIZE(winDirectoryNames));
  FOG_ASSERT(dst.isEmpty()); // Dst should be clear.

  HKEY k;
  DWORD d = 0, v_type, size;
  const char *reg_path;
  bool tryShellFolders = true;

  BufferP<1024> bufStorage;
  BufferP<1024> expandedStorage;

  char* buf;
  char* expanded;

  getHomeDirectory(dst);

  const char* type = winDirectoryNames[id];
  reg_path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders";

_back:
  if (RegCreateKeyExA(HKEY_CURRENT_USER, reg_path,
      0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS | KEY_EXECUTE |
      KEY_QUERY_VALUE, NULL, &k, &d ) != ERROR_SUCCESS)
  {
    return false;
  }

  if (RegQueryValueExA(k, type, NULL, &v_type, NULL, &size ) == ERROR_SUCCESS && size != 0 && (v_type == REG_SZ || v_type == REG_EXPAND_SZ))
  {
    buf = (char*)bufStorage.alloc((size_t)size);
    RegQueryValueExA(k, type, NULL, NULL, (LPBYTE)buf, &size);
    if (*buf)
    {
      TextCodec::local8().decode(dst, Stub8(buf, size));
      if (v_type == REG_EXPAND_SZ)
      {
        size = ExpandEnvironmentStringsA(buf, NULL, 0);
        if (size)
        {
          size += 2;
          expanded = (char*)expandedStorage.alloc(size);

          ExpandEnvironmentStringsA(buf, expanded, size);
          if (expanded[0]) TextCodec::local8().decode(dst, Stub8(expanded, DETECT_LENGTH));
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
#elif defined(FOG_OS_POSIX)

static const char* xdgDirectoryNames[] =
{
  "",
  "XDG_DESKTOP_DIR",
  "XDG_DOCUMENTS_DIR",
  "XDG_MUSIC_DIR",
  "XDG_PICTURES_DIR",
  "XDG_VIDEOS_DIR"
};

static err_t getXdgDirectory(String& dst, int id)
{
  FOG_ASSERT(id != 0 && (uint)id < FOG_ARRAY_SIZE(xdgDirectoryNames));
  FOG_ASSERT(dst.isEmpty()); // Dst should be clear.

  err_t err;

  String home;
  String configFile;

  if ((err = UserInfo::getDirectory(home, USER_DIRECTORY_HOME))) return err;

  if ((err = OS::getEnv(Ascii8("XDG_CONFIG_HOME"), configFile)) || configFile.isEmpty())
  {
    // XDG_CONFIG_HOME variable is not set, so guess...
    configFile = home;
    configFile.append(Ascii8("/.config"));
  }
  configFile.append(Ascii8("/user-dirs.dirs"));

  const char* type = xdgDirectoryNames[id];
  size_t typeLength = strlen(type);

  const char* mark;
  const char* end;
  const char* p;
  size_t remain;
  int relative;

  MapFile file;
  if ((err = file.map(configFile, true))) goto _Fail;

  // Tokenize lines and parse them.
  p = reinterpret_cast<const char*>(file.getData());
  end = p + file.getLength();
  for (;;)
  {
    // Skip spaces and empty lines.
    while (p != end && Byte::isSpace(*p)) p++;
    if (p == end) break;

    // We are looking for XDG_SOME_DIR="VALUE".
    if ((size_t)(end - p) <= typeLength || memcmp(p, type, typeLength) != 0)
      goto _NextLine;

    p += typeLength;

    // Skip spaces and tabs.
    while (p != end && *p == ' ' || *p == '\t') p++;
    if (p == end) break;

    // Parse '='.
    if (*p != '=')
      goto _NextLine;

    // Skip spaces and tabs.
    while (p != end && *p == ' ' || *p == '\t') p++;
    if (p == end) break;

    // Parse '"'.
    if (*p != '\"') goto _NextLine;
    p++;

    mark = p;
    while (p != end && *p != '"' && *p != '\n') p++;
    if (p == end || *p == '\n') continue;

    // Ok, everything looks ok. We have XDG name and value, we need only to
    // postprocess it and return. Postprocess means to parse $HOME, maybe also
    // other environment variables?
    relative = false;
    remain = (size_t)(p - mark);
    if (remain == 0) goto _NextLine;

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

    if (relative) { dst.setDeep(home); dst.append(Char('/')); }
    return TextCodec::local8().decode(dst, Stub8(mark, remain), NULL, CONTAINER_OP_APPEND);

_NextLine:
    while (p != end && *p != '\n') p++;
    if (p == end) break;
  }

_Fail:
  // Just fallback to known defaults.
  if (id == USER_DIRECTORY_DESKTOP)
  {
    dst.set(home);
    dst.append(Ascii8("/Desktop"));
    return ERR_OK;
  }

  return ERR_USER_NO_XDG_DIRECTORY;
}
#endif

static err_t getHomeDirectory(String& dst)
{
#if defined(FOG_OS_WINDOWS)
  return OS::getEnv(fog_strings->getString(STR_PLATFORM_USERPROFILE), dst);
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  struct passwd *pwd = getpwuid(UserInfo::uid());
  if (pwd)
  {
    TextCodec::local8().decode(dst, Stub8(pwd->pw_dir, DETECT_LENGTH));
    return ERR_OK;
  }
  else
  {
    char* s;

    if ((s = getenv("HOME")) != 0)
    {
      TextCodec::local8().decode(dst, Stub8(s, DETECT_LENGTH));
      return ERR_OK;
    }
    else if ((s = getenv("TMPDIR")) != 0)
    {
      TextCodec::local8().decode(dst, Stub8(s, DETECT_LENGTH));
      return ERR_USER_NO_HOME_DIRECTORY;
    }
    else
    {
      dst.set(Ascii8("/tmp"));
      return ERR_USER_NO_HOME_DIRECTORY;
    }
  }
#endif // FOG_OS_POSIX
}

// [Fog::UserInfo]

uint32_t UserInfo::uid()
{
#if defined(FOG_OS_WINDOWS)
  return 0;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  return (uint32_t)(::getuid());
#endif // FOG_OS_POSIX
}

uint32_t UserInfo::gid()
{
#if defined(FOG_OS_WINDOWS)
  return 0;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  return (uint32_t)(::getgid());
#endif // FOG_OS_POSIX
}

err_t UserInfo::getDirectory(String& dst, int id)
{
  err_t err;

  dst.clear();
  if (FOG_UNLIKELY((uint)id >= (uint)USER_DIRECTORY_COUNT)) return ERR_RT_INVALID_ARGUMENT;

  switch (id)
  {
    case USER_DIRECTORY_HOME:
      err = getHomeDirectory(dst);
      break;

    default:
#if defined(FOG_OS_WINDOWS)
      err = registryLookupDir(dst, id);
#elif defined(FOG_OS_POSIX)
      err = getXdgDirectory(dst, id);
#else
#     warning "Fog::UserInfo::Unknown operating system"
#endif
      break;
  }

#if defined(FOG_OS_WINDOWS)
  dst.slashesToPosix();
#endif // FOG_OS_WINDOWS

  return err;
}

} // Fog namespace
