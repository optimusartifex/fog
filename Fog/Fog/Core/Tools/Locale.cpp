// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Init_Core_p.h>
#include <Fog/Core/Tools/ByteArray.h>
#include <Fog/Core/Tools/Locale.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>

#include <wchar.h>
#include <locale.h>

#if defined(FOG_HAVE_XLOCALE_H) && defined(FOG_HAVE_XLOCALE_H_LOCALECONV_L)
#include <xlocale.h>
#endif

namespace Fog {

// ============================================================================
// [Fog::Locale]
// ============================================================================

Static<Locale::Data> Locale::_dnull;
Static<Locale::Data> Locale::sharedPosix;
Static<Locale::Data> Locale::sharedUser;

Locale* Locale::_dnullLocale;
Locale* Locale::sharedPosixLocale;
Locale* Locale::sharedUserLocale;

static Static<Locale> _dnullL;
static Static<Locale> sharedPosixL;
static Static<Locale> sharedUserL;

static void _setDefaults(Locale::Data* d)
{
  d->data[LOCALE_CHAR_DECIMAL_POINT] = '.';
  d->data[LOCALE_CHAR_THOUSANDS_GROUP] = '\0';
  d->data[LOCALE_CHAR_ZERO] = '0';
  d->data[LOCALE_CHAR_PLUS] = '+';
  d->data[LOCALE_CHAR_MINUS] = '-';
  d->data[LOCALE_CHAR_SPACE] = ' ';
  d->data[LOCALE_CHAR_EXPONENTIAL] = 'e';
  d->data[LOCALE_CHAR_FIRST_THOUSANDS_GROUP] = 3;
  d->data[LOCALE_CHAR_NEXT_THOUSANDS_GROUP] = 3;
  d->data[LOCALE_CHAR_RESERVED] = 0;
}

static void _setLConv(Locale::Data* d, const struct lconv* conv)
{
  _setDefaults(d);

  if (conv)
  {
    wchar_t buf[4];

    if (mbstowcs(buf, conv->decimal_point, 3) == 1) d->data[LOCALE_CHAR_DECIMAL_POINT  ] = (uint32_t)buf[0];
    if (mbstowcs(buf, conv->thousands_sep, 3) == 1) d->data[LOCALE_CHAR_THOUSANDS_GROUP] = (uint32_t)buf[0];
    if (mbstowcs(buf, conv->positive_sign, 3) == 1) d->data[LOCALE_CHAR_PLUS           ] = (uint32_t)buf[0];
    if (mbstowcs(buf, conv->negative_sign, 3) == 1) d->data[LOCALE_CHAR_MINUS          ] = (uint32_t)buf[0];
  }
}

Locale::Locale() :
  _d(_dnull->ref())
{
}

Locale::Locale(const Locale& other) :
  _d(other._d->ref())
{
}

Locale::Locale(Data* d) :
  _d(d)
{
}

Locale::Locale(const String& name) :
  _d(_dnull->ref())
{
  set(name);
}

Locale::~Locale()
{
  _d->deref();
}

err_t Locale::_detach()
{
  if (isDetached()) return ERR_OK;

  Data* newd = fog_new Data(*_d);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

void Locale::reset()
{
  atomicPtrXchg(&_d, _dnull->ref())->deref();
}

// [Set]

bool Locale::set(const String& name)
{
  char lcNameA[512];
  if (!StringUtil::unicodeToLatin1(lcNameA, name.getData(), name.getLength())) return false;

  TemporaryByteArray<128> savedLocale(Stub8(setlocale(LC_ALL, NULL)));

  if (setlocale(LC_ALL, lcNameA))
  {
    detach();
    _d->name = name;
    _setLConv(_d, localeconv());
  }
  else
  {
    reset();
  }

  setlocale(LC_ALL, savedLocale.getData());
  return !isNull();
}

bool Locale::set(const Locale& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return true;
}

err_t Locale::setValue(int id, uint32_t value)
{
  if ((uint)id >= (uint)LOCALE_CHAR_INVALID) return ERR_RT_INVALID_ARGUMENT;
  if (_d->data[id] == value) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->data[id] = value;
  return ERR_OK;
}

// ============================================================================
// [Fog::Locale::Data]
// ============================================================================

Locale::Data::Data()
{
  refCount.init(1);
  memset(data, 0, sizeof(uint32_t) * LOCALE_CHAR_INVALID);
}

Locale::Data::Data(const Data& other) :
  name(other.name)
{
  refCount.init(1);
  memcpy(data, other.data, sizeof(uint32_t) * LOCALE_CHAR_INVALID);
}

Locale::Data::~Data()
{
}

void Locale::Data::deref()
{
  if (refCount.deref()) fog_delete(this);
}

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_locale_init(void)
{
  Locale::_dnull.init();
  Locale::_dnull->name = Ascii8("NULL");
  _setDefaults(Locale::_dnull.instancep());

  Locale::sharedPosix.init();
  Locale::sharedPosix->name= Ascii8("POSIX");
  _setDefaults(Locale::sharedPosix.instancep());

  Locale::sharedUser.init();
  _setLConv(Locale::sharedUser.instancep(), localeconv());

  _dnullL.initCustom1(Locale::_dnull.instancep());
  sharedPosixL.initCustom1(Locale::sharedPosix.instancep());
  sharedUserL.initCustom1(Locale::sharedUser.instancep());

  Locale::_dnullLocale = _dnullL.instancep();
  Locale::sharedPosixLocale = sharedPosixL.instancep();
  Locale::sharedUserLocale = sharedUserL.instancep();
}

FOG_NO_EXPORT void _core_locale_fini(void)
{
  Locale::_dnull.destroy();
  Locale::sharedPosix.destroy();
  Locale::sharedUser.destroy();
}

} // Fog namespace
