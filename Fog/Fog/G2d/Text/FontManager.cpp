// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/G2d/Text/FontManager.h>
#include <Fog/G2d/Text/FontProvider.h>
#include <Fog/G2d/Text/NullFontFace_p.h>

#if defined(FOG_FONT_WINDOWS)
#include <Fog/G2d/Text/WinFontProvider.h>
#endif // FOG_FONT_WINDOWS

#if defined(FOG_FONT_MAC)
#include <Fog/G2d/Text/MacFontProvider.h>
#endif // FOG_FONT_MAC

#if defined(FOG_FONT_FREETYPE)
#include <Fog/G2d/Text/FTFontProvider.h>
#endif // FOG_FONT_FREETYPE

namespace Fog {

// ============================================================================
// [Fog::FontManagerData - Construction / Destruction]
// ============================================================================

FontManagerData::FontManagerData() :
  fontListDirty(true),
  defaultFont(reinterpret_cast<FontData*>(NULL))
{
  reference.init(1);
}

FontManagerData::FontManagerData(const FontManagerData& other) :
  providers(other.providers),
  fontListCache(other.fontListCache),
  fontListDirty(other.fontListDirty),
  defaultFont(other.defaultFont)
{
  reference.init(1);
}

FontManagerData::~FontManagerData()
{
}

// ============================================================================
// [Fog::FontManager - Helpers]
// ============================================================================

static Static<Lock> FontManager_lock;
static Static<FontManagerData> FontManager_globalD;
static Static<FontManager> FontManager_globalInstance;

// ============================================================================
// [Fog::FontManager - Construction / Destruction]
// ============================================================================

FontManager::FontManager()
{
  AutoLock locked(FontManager_lock);

  _d = _global->_d->addRef();
}

FontManager::FontManager(const FontManager& other)
{
  AutoLock locked(FontManager_lock);

  _d = other._d->addRef();
}

FontManager::~FontManager()
{
  AutoLock locked(FontManager_lock);

  _d->deref();
}

// ============================================================================
// [Fog::FontManager - Provider Management]
// ============================================================================

err_t FontManager::addProvider(const FontProvider& provider, uint32_t order)
{
  if (FOG_IS_NULL(provider._d))
    return ERR_RT_INVALID_STATE;

  AutoLock locked(FontManager_lock);

  size_t i = _d->providers.indexOf(provider);
  size_t w = (order == FONT_ORDER_FIRST) ? 0 : _d->providers.getLength();

  if (i == w)
    return ERR_OK;

  if (_d->reference.get() > 1)
  {
    FontManagerData* newd = fog_new FontManagerData(*_d);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    if (i != INVALID_INDEX && FOG_IS_ERROR(newd->providers.removeAt(i)))
    {
      newd->deref();
      return ERR_RT_OUT_OF_MEMORY;
    }

    if (FOG_IS_ERROR(newd->providers.insert(w, provider)))
    {
      newd->deref();
      return ERR_RT_OUT_OF_MEMORY;
    }

    _d->reference.dec();
    _d = newd;
  }
  else
  {
    if (i != INVALID_INDEX && FOG_IS_ERROR(_d->providers.removeAt(i)))
      return ERR_RT_OUT_OF_MEMORY;

    if (FOG_IS_ERROR(_d->providers.insert(w, provider)))
      return ERR_RT_OUT_OF_MEMORY;
  }

  return ERR_OK;
}

err_t FontManager::removeProvider(const FontProvider& provider)
{
  if (FOG_IS_NULL(provider._d))
    return ERR_RT_INVALID_STATE;

  AutoLock locked(FontManager_lock);

  size_t i = _d->providers.indexOf(provider);
  if (i == INVALID_INDEX) return ERR_RT_OBJECT_NOT_FOUND;

  if (_d->reference.get() > 1)
  {
    FontManagerData* newd = fog_new FontManagerData(*_d);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    if (FOG_IS_ERROR(newd->providers.removeAt(i)))
    {
      newd->deref();
      return ERR_RT_OUT_OF_MEMORY;
    }

    _d->reference.dec();
    _d = newd;
  }
  else
  {
    if (FOG_IS_ERROR(_d->providers.removeAt(i)))
      return ERR_RT_OUT_OF_MEMORY;
  }

  return ERR_OK;
}

bool FontManager::hasProvider(const FontProvider& provider) const
{
  AutoLock locked(FontManager_lock);

  return _d->providers.contains(provider);
}

bool FontManager::hasProvider(const StringW& name) const
{
  AutoLock locked(FontManager_lock);

  ListIterator<FontProvider> it(_d->providers);
  while (it.isValid())
  {
    if (it.getItem()._d->name == name) return true;
    it.next();
  }

  return false;
}

// ============================================================================
// [Fog::FontManager - Fonts]
// ============================================================================

FontFace* FontManager::getFontFace(const StringW& fontFamily) const
{
  AutoLock locked(FontManager_lock);

  ListIterator<FontProvider> it(_d->providers);
  FontFace* face = NULL;

  while (it.isValid())
  {
    if (it.getItem().getFontFace(&face, fontFamily) == ERR_OK)
      goto _Done;
    it.next();
  }

_Done:
  return face;
}

List<StringW> FontManager::getFontList() const
{
  AutoLock locked(FontManager_lock);

  if (_d->fontListDirty == true)
  {
    _d->fontListCache.clear();

    ListIterator<FontProvider> it(_d->providers);
    while (it.isValid())
    {
      if (it.getItem().getFontList(_d->fontListCache) != ERR_OK)
        goto _End;
      it.next();
    }

    _d->fontListDirty = false;
  }

_End:
  return _d->fontListCache;
}

void FontManager::_initDefaultFont()
{
  AutoLock locked(FontManager_lock);

  ListIterator<FontProvider> it(_d->providers);
  while (it.isValid())
  {
    const FontProvider& provider = it.getItem();
    StringW fontFamily = provider.getDefaultFamily();

    if (!fontFamily.isEmpty())
    {
      FontFace* face;
      if (provider.getFontFace(&face, fontFamily) == ERR_OK)
      {
        if (_d->defaultFont._fromFace(face, 12, UNIT_PT) == ERR_OK)
          break;
      }
    }

    it.next();
  }
}

// ============================================================================
// [Fog::FontManager - Operator Overload]
// ============================================================================

const FontManager& FontManager::operator=(const FontManager& other)
{
  atomicPtrXchg(&_d, other._d->addRef())->deref();
  return *this;
}

// ============================================================================
// [Fog::FontManager - Statics]
// ============================================================================

FontManager* FontManager::_global;

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FontFace_initNull(void);
FOG_NO_EXPORT void FontFace_finiNull(void);

FOG_NO_EXPORT void FontManager_init(void)
{
  FontFace_initNull();

  // Initialize the font in default FontManager to font using NullFontFace.
  // NullFontFace is only used when something failed and system fonts are
  // inaccessible - This can probably happen only under linux if fontconfig
  // configuration is broken and Fog-Framework can't find any other suitable 
  // font.
  FontManager_lock.init();

  FontManager_globalD.init();
  FontManager_globalInstance.initCustom1(&FontManager_globalD);
  FontManager::_global = &FontManager_globalInstance;

  FontData* fd = reinterpret_cast<FontData*>(MemMgr::alloc(sizeof(FontData)));
  FOG_ASSERT(fd != NULL);

  fd->reference.init(1);

  fd->unit = UNIT_NONE;
  fd->letterSpacingMode = FONT_SPACING_MODE_PERCENTAGE;
  fd->wordSpacingMode = FONT_SPACING_MODE_PERCENTAGE;
  fd->dataFlags = NO_FLAGS;

  fd->letterSpacing = 1.0f;
  fd->wordSpacing = 1.0f;

  fd->hints.reset();
  fd->transform.reset();
  fd->metrics.reset();

  fd->forceCaching = false;
  fd->face = NullFontFace::_dnull->addRef();

  FontManager::_global->_d->defaultFont._d = fd;

#if defined(FOG_FONT_WINDOWS)
  FontManager::_global->addProvider(FontProvider(fog_new WinFontProviderData()), FONT_ORDER_LAST);
#endif // FOG_FONT_WINDOWS

#if defined(FOG_FONT_MAC)
  FontManager::_global->addProvider(FontProvider(fog_new MacFontProviderData()), FONT_ORDER_LAST);
#endif // FOG_FONT_MAC

#if defined(FOG_FONT_FREETYPE)
  FontManager::_global->addProvider(FontProvider(fog_new FTFontProviderData()), FONT_ORDER_LAST);
#endif // FOG_FONT_FREETYPE

  FontManager::_global->_initDefaultFont();
}

FOG_NO_EXPORT void FontManager_fini(void)
{
  FontManager_globalD.destroy();
  FontManager_lock.destroy();

  FontManager::_global = NULL;
  FontFace_finiNull();
}

} // Fog namespace
