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
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Tools/ByteArray.h>
#include <Fog/Core/Tools/ByteArrayTmp_p.h>
#include <Fog/Core/Tools/Locale.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>

#include <wchar.h>
#include <locale.h>

namespace Fog {

// ============================================================================
// [Fog::Locale - Helpers]
// ============================================================================

static Static<LocaleData> _Locale_dnull;
static Static<LocaleData> _Locale_dposix;
static Static<LocaleData> _Locale_duser;

static FOG_INLINE LocaleData* _Locale_ref(LocaleData* d)
{
  d->refCount.inc();
  return d;
}

static FOG_INLINE void _Locale_deref(LocaleData* d)
{
  if (d->refCount.deref()) fog_delete(d);
}

static void _Locale_setDefaults(LocaleData* d)
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

static void _Locale_setLConv(LocaleData* d, const struct lconv* conv)
{
  _Locale_setDefaults(d);
  if (FOG_IS_NULL(conv)) return;

  wchar_t buf[4];

  if (mbstowcs(buf, conv->decimal_point, 3) == 1) d->data[LOCALE_CHAR_DECIMAL_POINT  ] = (uint16_t)buf[0];
  if (mbstowcs(buf, conv->thousands_sep, 3) == 1) d->data[LOCALE_CHAR_THOUSANDS_GROUP] = (uint16_t)buf[0];
  if (mbstowcs(buf, conv->positive_sign, 3) == 1) d->data[LOCALE_CHAR_PLUS           ] = (uint16_t)buf[0];
  if (mbstowcs(buf, conv->negative_sign, 3) == 1) d->data[LOCALE_CHAR_MINUS          ] = (uint16_t)buf[0];
}

Locale::Locale() :
  _d(_Locale_ref(_Locale_dnull.instancep()))
{
}

Locale::Locale(const Locale& other) :
  _d(_Locale_ref(other._d))
{
}

Locale::Locale(const String& locale) :
  _d(_Locale_ref(_Locale_dnull.instancep()))
{
  create(locale);
}

Locale::~Locale()
{
  _Locale_deref(_d);
}

// ============================================================================
// [Fog::Locale - Sharing]
// ============================================================================

err_t Locale::_detach()
{
  if (isDetached()) return ERR_OK;

  LocaleData* newd = fog_new LocaleData();
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  newd->refCount.init(1);
  newd->locale = _d->locale;
  memcpy(newd->data, _d->data, LOCALE_CHAR_COUNT * sizeof(Char));

  _Locale_deref(atomicPtrXchg(&_d, newd));
  return ERR_OK;
}

// ============================================================================
// [Fog::Locale - Accessors]
// ============================================================================

err_t Locale::setChar(uint32_t id, Char value)
{
  if (id >= (uint)LOCALE_CHAR_COUNT) return ERR_RT_INVALID_ARGUMENT;
  if (_d->data[id] == value) return ERR_OK;

  FOG_RETURN_ON_ERROR(detach());

  _d->data[id] = value;
  return ERR_OK;
}

// ============================================================================
// [Fog::Locale - Create]
// ============================================================================

err_t Locale::create(const String& locale)
{
  char localeA[512];
  if (!StringUtil::unicodeToLatin1(localeA, locale.getData(), locale.getLength()))
    return ERR_RT_INVALID_ARGUMENT;

  err_t err = ERR_OK;
  ByteArrayTmp<128> savedLocale(Stub8(setlocale(LC_ALL, NULL)));

  if (setlocale(LC_ALL, localeA))
  {
    err = detach();
    if (FOG_IS_ERROR(err)) goto _End;

    _d->locale = locale;
    _Locale_setLConv(_d, localeconv());
  }
  else
  {
    err = ERR_LOCALE_NOT_MATCHED;
  }

_End:
  setlocale(LC_ALL, savedLocale.getData());
  return err;
}

// ============================================================================
// [Fog::Locale - Reset]
// ============================================================================

void Locale::reset()
{
  _Locale_deref(
    atomicPtrXchg(&_d, _Locale_ref(_Locale_dnull.instancep())));
}

// ============================================================================
// [Fog::Locale - Operator Overload]
// ============================================================================

Locale& Locale::operator=(const Locale& other)
{
  _Locale_deref(
    atomicPtrXchg(&_d, _Locale_ref(other._d)));
  return *this;
}

// ============================================================================
// [Fog::Locale - Statics]
// ============================================================================

Locale* Locale::_dnull;
Locale* Locale::_dposix;
Locale* Locale::_duser;

static Static<Locale> _Locale_dnull_instance;
static Static<Locale> _Locale_dposix_instance;
static Static<Locale> _Locale_duser_instance;

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Locale_init(void)
{
  _Locale_dnull.init();
  _Locale_dnull->refCount.init(1);
  _Locale_dnull->locale = Ascii8("NULL");
  _Locale_setDefaults(_Locale_dnull.instancep());

  _Locale_dposix.init();
  _Locale_dposix->refCount.init(1);
  _Locale_dposix->locale= Ascii8("POSIX");
  _Locale_setDefaults(_Locale_dposix.instancep());

  _Locale_duser.init();
  _Locale_duser->refCount.init(1);
  _Locale_setLConv(_Locale_duser.instancep(), localeconv());

  Locale::_dnull = _Locale_dnull_instance.instancep();
  Locale::_dnull->_d = _Locale_dnull.instancep();

  Locale::_dposix = _Locale_dposix_instance.instancep();
  Locale::_dposix->_d = _Locale_dposix.instancep();

  Locale::_duser = _Locale_duser_instance.instancep();
  Locale::_duser->_d = _Locale_duser.instancep();
}

} // Fog namespace
