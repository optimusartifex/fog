// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/TextCodec.h>

// [Dependencies - POSIX]
#if defined(FOG_OS_POSIX)
# include <stdlib.h>
#endif // FOG_OS_POSIX

namespace Fog {

#if defined(FOG_OS_WINDOWS)

// ============================================================================
// [Fog::Environment (Windows)]
// ============================================================================

static err_t FOG_CDECL Environment_getValueStubA(const StubA* key, StringW* value)
{
  StringTmpW<TEMPORARY_LENGTH> keyW;
  FOG_RETURN_ON_ERROR(keyW.setAscii8(*key));

  return _api.environment_getValueStringW(&keyW, value);
}

static err_t FOG_CDECL Environment_getValueStringW(const StringW* key, StringW* value)
{
  if (key->getLength() == 0)
    return ERR_RT_INVALID_ARGUMENT;

  size_t capacity = 256;

  for (;;)
  {
    CharW* buf = value->_prepare(CONTAINER_OP_REPLACE, capacity);
    capacity = value->getCapacity();

    if (FOG_IS_NULL(buf))
      return ERR_RT_OUT_OF_MEMORY;

    size_t length = ::GetEnvironmentVariableW(
      reinterpret_cast<const wchar_t*>(key->getData()),
      reinterpret_cast<wchar_t*>(buf),
      static_cast<DWORD>(Math::min<size_t>(capacity, UINT32_MAX)));

    if (length == 0)
      return OSUtil::getErrFromOSLastError();

    if (length > capacity)
    {
      capacity = length;
      continue;
    }

    value->_modified(buf + length);
    return ERR_OK;
  }
}

static err_t FOG_CDECL Environment_setValueStubA(const StubA* key, const StringW* value)
{
  StringTmpW<TEMPORARY_LENGTH> keyW;
  FOG_RETURN_ON_ERROR(keyW.setAscii8(*key));

  return _api.environment_setValueStringW(&keyW, value);
}

static err_t FOG_CDECL Environment_setValueStringW(const StringW* key, const StringW* value)
{
  if (key->getLength() == 0)
    return ERR_RT_INVALID_ARGUMENT;

  if (::SetEnvironmentVariableW(
    reinterpret_cast<const wchar_t*>(key->getData()),
    reinterpret_cast<const wchar_t*>(value->getData())))
  {
    return ERR_OK;
  }
  else
  {
    return OSUtil::getErrFromOSLastError();
  }
}

#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::Environment (Posix)]
// ============================================================================

#if defined(FOG_OS_POSIX)

static err_t Environment_getValuePrivate(const char* key, StringW* value)
{
  if (key[0] == 0)
    return ERR_RT_INVALID_ARGUMENT;

  const char* var = ::getenv(key);

  if (var == NULL)
  {
    value->clear();
    return OSUtil::getErrFromOSLastError();
  }

  return TextCodec::local8().decode(*value, StubA(var, DETECT_LENGTH));

}

static err_t FOG_CDECL Environment_getValueStubA(const StubA* key, StringW* value)
{
  if (key->getLength() == DETECT_LENGTH || key->getData()[key->getLength()] == 0)
    return Environment_getValuePrivate(key->getData(), value);

  StringTmpA<TEMPORARY_LENGTH> keyA;
  FOG_RETURN_ON_ERROR(keyA.set(*key));

  return Environment_getValuePrivate(keyA.getData(), value);
}

static err_t FOG_CDECL Environment_getValueStringW(const StringW* key, StringW* value)
{
  StringTmpA<TEMPORARY_LENGTH> keyA;
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(keyA, *key));

  return Environment_getValuePrivate(keyA.getData(), value);
}

static err_t Environment_setValuePrivate(const char* key, const StringW* value)
{
  if (key[0] == 0)
    return ERR_RT_INVALID_ARGUMENT;

  int result;

  if (value == NULL)
  {
    result = ::unsetenv(key);
  }
  else
  {
    StringTmpA<TEMPORARY_LENGTH> valueA;
    FOG_RETURN_ON_ERROR(TextCodec::local8().encode(valueA, *value));

    result = ::setenv(key, valueA.getData(), 1);
  }

  if (result != 0)
    return OSUtil::getErrFromOSLastError();
  else
    return ERR_OK;
}

static err_t FOG_CDECL Environment_setValueStubA(const StubA* key, const StringW* value)
{
  if (key->getLength() == DETECT_LENGTH || key->getData()[key->getLength()] == 0)
    return Environment_setValuePrivate(key->getData(), value);

  StringTmpA<TEMPORARY_LENGTH> keyA;
  FOG_RETURN_ON_ERROR(keyA.set(*key));

  return Environment_setValuePrivate(keyA.getData(), value);
}

static err_t FOG_CDECL Environment_setValueStringW(const StringW* key, const StringW* value)
{
  StringTmpA<TEMPORARY_LENGTH> keyA;
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(keyA, *key));

  return Environment_setValuePrivate(keyA.getData(), value);
}

#endif // FOG_OS_POSIX

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Environment_init(void)
{
  _api.environment_getValueStubA = Environment_getValueStubA;
  _api.environment_getValueStringW = Environment_getValueStringW;

  _api.environment_setValueStubA = Environment_setValueStubA;
  _api.environment_setValueStringW = Environment_setValueStringW;
}

} // Fog namespace
