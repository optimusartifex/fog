// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/Swap.h>
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
  Hash<StringW, FontFace*> copy;
  swap(copy, cache);

  HashIterator<StringW, FontFace*> it(copy);
  while (it.isValid())
  {
    FontFace* face = it.getItem();
    face->deref();
    it.next();
  }
}

FontFace* FontFaceCache::get(const StringW& family)
{
  FontFace* face = cache.get(family, NULL);
  if (FOG_IS_NULL(face)) return face;

  face->reference.inc();
  return face;
}

err_t FontFaceCache::put(const StringW& family, FontFace* face)
{
  err_t err = cache.put(family, face, false);
  if (err == ERR_OK) face->reference.inc();
  return err;
}

err_t FontFaceCache::del(const StringW& family, FontFace* face)
{
  err_t err = cache.remove(face->family);
  if (FOG_IS_ERROR(err)) return err;

  face->deref();
  return err;
}

} // Fog namespace
