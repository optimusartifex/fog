// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Text/FontFaceCache.h>

namespace Fog {

// ============================================================================
// [Fog::FontFaceCache - Construction / Destruction]
// ============================================================================

FontFaceCache::FontFaceCache()
{
}

FontFaceCache::~FontFaceCache()
{
  reset();
}

// ============================================================================
// [Fog::FontFaceCache - Methods]
// ============================================================================

void FontFaceCache::reset()
{
  UnorderedHash<String, FontFace*> copy;
  swap(copy, cache);

  UnorderedHash<String, FontFace*>::ConstIterator it(copy);
  for (it.toStart(); it.isValid(); it.toNext())
  {
    FontFace* face = it.value();
    face->deref();
  }
}

FontFace* FontFaceCache::get(const String& family)
{
  FontFace* face = cache.value(family, NULL);
  if (FOG_IS_NULL(face)) return face;

  face->refCount.inc();
  return face;
}

err_t FontFaceCache::put(const String& family, FontFace* face)
{
  err_t err = cache.put(family, face, false);
  if (err == ERR_OK) face->refCount.inc();
  return err;
}

err_t FontFaceCache::del(const String& family, FontFace* face)
{
  err_t err = cache.remove(face->family);
  if (FOG_IS_ERROR(err)) return err;

  face->deref();
  return err;
}

} // Fog namespace
