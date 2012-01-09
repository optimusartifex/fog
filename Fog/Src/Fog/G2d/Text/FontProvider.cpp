// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Text/FontManager.h>
#include <Fog/G2d/Text/FontProvider.h>

namespace Fog {

// ============================================================================
// [Fog::FontProviderData - Construction / Destruction]
// ============================================================================

FontProviderData::FontProviderData()
{
  reference.init(1);
  id = FONT_PROVIDER_NULL;
}

FontProviderData::~FontProviderData()
{
}

// ============================================================================
// [Fog::FontProvider - Construction / Destruction]
// ============================================================================

FontProvider::FontProvider() :
  _d(NULL)
{
}

FontProvider::FontProvider(const FontProvider& other) :
  _d(other._d)
{
  if (FOG_LIKELY(_d != NULL))
    _d->reference.inc();
}

FontProvider::~FontProvider()
{
  if (FOG_LIKELY(_d != NULL))
    _d->deref();
}

// ============================================================================
// [Fog::FontProvider - Accessors]
// ============================================================================

StringW FontProvider::getName() const
{
  if (FOG_IS_NULL(_d)) return StringW();

  return _d->name;
}

uint32_t FontProvider::getId() const
{
  if (FOG_IS_NULL(_d)) return FONT_PROVIDER_NULL;

  return _d->id;
}

// ============================================================================
// [Fog::FontProvider - Interface]
// ============================================================================

err_t FontProvider::getFontFace(FontFace** dst, const StringW& fontFamily) const
{
  if (FOG_IS_NULL(_d)) return ERR_RT_INVALID_STATE;

  return _d->getFontFace(dst, fontFamily);
}

err_t FontProvider::getFontList(List<StringW>& dst) const
{
  if (FOG_IS_NULL(_d)) return ERR_RT_INVALID_STATE;

  return _d->getFontList(dst);
}

StringW FontProvider::getDefaultFamily() const
{
  if (FOG_IS_NULL(_d))
    return StringW();

  return _d->getDefaultFamily();
}

// ============================================================================
// [Fog::FontProvider - Operator Overload]
// ============================================================================

const FontProvider& FontProvider::operator=(const FontProvider& other)
{
  FontProviderData* newd = other._d;
  if (FOG_LIKELY(newd != NULL)) newd->reference.inc();

  FontProviderData* oldd = atomicPtrXchg(&_d, newd);
  if (FOG_LIKELY(oldd != NULL)) oldd->deref();

  return *this;
}

} // Fog namespace
