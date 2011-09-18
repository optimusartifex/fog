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
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Tools/Locale.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>

// [Dependencies - C]
#include <wchar.h>
#include <locale.h>

namespace Fog {

// ============================================================================
// [Fog::Locale - Global]
// ============================================================================

static Static<LocaleData> Locale_dPosix;
static Static<LocaleData> Locale_dUser;

static Static<Locale> Locale_oPosix;
static Static<Locale> Locale_oUser;

// ============================================================================
// [Fog::Locale - Helpers]
// ============================================================================

static void Locale_setPosix(LocaleData* d)
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

static void Locale_setLConv(LocaleData* d, const struct lconv* conv)
{
  Locale_setPosix(d);
  if (FOG_IS_NULL(conv))
    return;

  wchar_t buf[4];

  if (mbstowcs(buf, conv->decimal_point, 3) == 1) d->data[LOCALE_CHAR_DECIMAL_POINT  ] = (uint16_t)buf[0];
  if (mbstowcs(buf, conv->thousands_sep, 3) == 1) d->data[LOCALE_CHAR_THOUSANDS_GROUP] = (uint16_t)buf[0];
  if (mbstowcs(buf, conv->positive_sign, 3) == 1) d->data[LOCALE_CHAR_PLUS           ] = (uint16_t)buf[0];
  if (mbstowcs(buf, conv->negative_sign, 3) == 1) d->data[LOCALE_CHAR_MINUS          ] = (uint16_t)buf[0];
}

// ============================================================================
// [Fog::Locale - Construction / Destruction]
// ============================================================================

static void FOG_CDECL Locale_ctor(Locale* self)
{
  self->_d = Locale_dPosix->addRef();
}

static void FOG_CDECL Locale_ctorCopy(Locale* self, const Locale* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL Locale_ctorString(Locale* self, const StringW* name)
{
  self->_d = Locale_dPosix->addRef();
  _api.locale.create(self, name);
}

static void FOG_CDECL Locale_dtor(Locale* self)
{
  self->_d->release();
}

// ============================================================================
// [Fog::Locale - Sharing]
// ============================================================================

static err_t FOG_CDECL Locale_detach(Locale* self)
{
  LocaleData* d = self->_d;
  if (d->reference.get() == 1)
    return ERR_OK;

  LocaleData* newd = _api.locale.dCreate();
  if (FOG_IS_NULL(newd))
    return ERR_RT_OUT_OF_MEMORY;

  newd->name->set(d->name);
  MemOps::copy(newd->data, d->data, LOCALE_CHAR_COUNT * sizeof(CharW));

  atomicPtrXchg(&self->_d, newd)->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Locale - Reset]
// ============================================================================

static void FOG_CDECL Locale_reset(Locale* self)
{
  atomicPtrXchg(&self->_d, Locale_dPosix->addRef())->release();
}

// ============================================================================
// [Fog::Locale - SetLocale]
// ============================================================================

static void FOG_CDECL Locale_setLocale(Locale* self, const Locale* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
}

// ============================================================================
// [Fog::Locale - SetChar]
// ============================================================================

static err_t FOG_CDECL Locale_setChar(Locale* self, uint32_t id, uint16_t ch)
{
  LocaleData* d = self->_d;

  if (id >= (uint)LOCALE_CHAR_COUNT)
    return ERR_RT_INVALID_ARGUMENT;

  if (d->data[id] == ch)
    return ERR_OK;

  FOG_RETURN_ON_ERROR(self->detach());

  self->_d->data[id] = ch;
  return ERR_OK;
}

// ============================================================================
// [Fog::Locale - Create]
// ============================================================================

static err_t FOG_CDECL Locale_create(Locale* self, const StringW* name)
{
  char nameA[512];

  if (!StringUtil::latinFromUnicode(nameA, name->getData(), name->getLength()))
    return ERR_RT_INVALID_ARGUMENT;

  err_t err = ERR_OK;
  StringTmpA<128> savedLocale(StubA(setlocale(LC_ALL, NULL)));

  if (setlocale(LC_ALL, nameA))
  {
    err = self->detach();
    if (FOG_IS_ERROR(err))
      goto _End;

    self->_d->name->set(*name);
    Locale_setLConv(self->_d, localeconv());
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
// [Fog::Locale - LocaleData]
// ============================================================================

static LocaleData* FOG_CDECL Locale_dCreate(void)
{
  LocaleData* d = reinterpret_cast<LocaleData*>(MemMgr::alloc(sizeof(LocaleData)));
  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->vType = VAR_TYPE_LOCALE;
  FOG_PADDING_ZERO_64(d->padding0_32);
  d->name.init();
  
  return d;
}

static void FOG_CDECL Locale_dFree(LocaleData* d)
{
  d->name.destroy();

  if ((d->vType & VAR_FLAG_STATIC) == 0)
    MemMgr::free(d);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Locale_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.locale.ctor = Locale_ctor;
  _api.locale.ctorCopy = Locale_ctorCopy;
  _api.locale.ctorString = Locale_ctorString;
  _api.locale.dtor = Locale_dtor;
  _api.locale.detach = Locale_detach;
  _api.locale.reset = Locale_reset;
  _api.locale.create = Locale_create;
  _api.locale.setLocale = Locale_setLocale;
  _api.locale.setChar = Locale_setChar;

  _api.locale.dCreate = Locale_dCreate;
  _api.locale.dFree = Locale_dFree;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  LocaleData* d;

  d = &Locale_dPosix;
  d->reference.init(1);
  d->vType = VAR_TYPE_LOCALE;
  d->name.initCustom1(Ascii8("POSIX"));

  _api.locale.oPosix = Locale_oPosix.initCustom1(d);
  Locale_setPosix(d);

  d = &Locale_dUser;
  d->reference.init(1);
  d->vType = VAR_TYPE_LOCALE;
  d->name.init();

  _api.locale.oUser = Locale_oPosix.initCustom1(d);
  Locale_setLConv(d, localeconv());
}

FOG_NO_EXPORT void Locale_fini(void)
{
  LocaleData* d;

  d = &Locale_dUser;
  d->name.destroy();

  d = &Locale_dPosix;
  d->name.destroy();
}

} // Fog namespace
