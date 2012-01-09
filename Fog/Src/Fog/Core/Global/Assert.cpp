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
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Tools/Logger.h>

// [Dependencies - C]
#include <stdio.h>

static FOG_INLINE FILE* fog_assert_get_file()
{
  return stderr;
}

#if defined(FOG_OS_WINDOWS) 
# define snprintf _snprintf
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_WINDOWS)
// When a Fog-App is build using WIN32, there is no standard IO. So we can show
// a message-box before the application crashes. This can help us when debugging.
static void fog_assert_MessageBox(const char* title, const char* msg)
{
  ::MessageBoxA(NULL, msg, title, MB_OK);
}
#endif // FOG_OS_WINDOWS

FOG_CAPI_DECLARE void fog_assert_message(const char* file, int line, const char* msg)
{
  char buf[1024];

  snprintf(buf, 1023, "Assertion failure: %s (line %d): %s", file, line, msg);
  buf[1023] = '\0';

  if (fog_application_safe_to_log)
  {
    Fog::Logger::assertion(NULL, NULL, "%s", buf);
  }
  else
  {
    FILE* fp = fog_assert_get_file();
    fputs(buf, fp);
    fflush(fp);
  }

#if defined(FOG_OS_WINDOWS)
  snprintf(buf, 1023, "File: %s\nLine: %d\n\n%s\n", file, line, msg);
  fog_assert_MessageBox("Assertion Failure", buf);
#endif // FOG_OS_WINDOWS

  Fog::Application::terminate(-1);
}

FOG_CAPI_DECLARE void fog_assert_format(const char* file, int line, const char* fmt, ...)
{
  char msg[1024];

  va_list ap;
  va_start(ap, fmt);
    vsnprintf(msg, 1023, fmt, ap);
    msg[1023] = '\0';
  va_end(ap);

  fog_assert_message(file, line, msg);
}
