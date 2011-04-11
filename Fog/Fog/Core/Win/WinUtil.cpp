// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#include <Fog/Core/Config/Config.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Win/WinUtil_p.h>

namespace Fog {
namespace WinUtil {

err_t getModuleFileName(HMODULE hModule, String& dst)
{
  dst.prepare(256);
  for (;;)
  {
    DWORD capacity = (DWORD)dst.getCapacity();
    DWORD result = GetModuleFileNameW(hModule, reinterpret_cast<wchar_t*>(dst.getDataX()), capacity + 1);

    if (result == 0) return ::GetLastError();

    if (result <= capacity)
    {
      dst.resize(result);
      return ERR_OK;
    }
    else
    {
      dst.reserve(dst.getCapacity() << 1);
    }
  }
}

} // WinUtil namespace
} // Fog namespace

// [Guard]
#endif // FOG_OS_WINDOWS
