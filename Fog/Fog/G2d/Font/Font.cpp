// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Hash.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Debug.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/OS/OS.h>
#include <Fog/Core/OS/UserInfo.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Font/Font.h>
#include <Fog/G2d/Font/Glyph.h>
#include <Fog/G2d/Font/GlyphCache.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Source/Color.h>

#include <Fog/G2d/Font/NullFontEngine_p.h>

#if defined(FOG_FONT_WINDOWS)
# include <Fog/G2d/Font/WinFontEngine.h>
#endif // FOG_FONT_WINDOWS

#if defined(FOG_FONT_FREETYPE)
# include <Fog/G2d/Font/FTFontEngine.h>
#endif // FOG_FONT_FREETYPE

#if defined(FOG_FONT_MAC)
# include <Fog/G2d/Font/MacFontEngine.h>
#endif // FOG_FONT_MAC

namespace Fog {

// ============================================================================
// [Fog::FontFaceEntry]
// ============================================================================

// TODO: FontFaceEntry contains duplicated members thats also in FontFace. We
// need better hash structure that will allow to use custom Accessor instance.

struct FOG_NO_EXPORT FontFaceEntry
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FontFaceEntry() : size(0.0f)
  {
  }

  FOG_INLINE FontFaceEntry(const FontFaceEntry& other) :
    family(other.family),
    size(other.size),
    options(other.options),
    transform(other.transform)
  {
  }

  FOG_INLINE FontFaceEntry(
    const String& family,
    float size,
    const FontOptions& options,
    const TransformF& transform) :
      family(family),
      size(size),
      options(options),
      transform(transform)
  {
  }

  FOG_INLINE ~FontFaceEntry() {}

  // --------------------------------------------------------------------------
  // [HashCode]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getHashCode() const
  {
    return HashUtil::combineHash(
      family.getHashCode(),
      HashUtil::getHashCode(size),
      options.getHashCode(),
      transform.getHashCode());
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FontFaceEntry& operator=(const FontFaceEntry& other)
  {
    family = other.family;
    size = other.size;
    options = other.options;
    transform = other.transform;
    return *this;
  }

  FOG_INLINE bool operator==(const FontFaceEntry& other) const
  {
    return (family == other.family) && (size == other.size) && (options == other.options) && (transform == other.transform);
  }

  FOG_INLINE bool operator!=(const FontFaceEntry& other) const
  {
    return (family != other.family) || (size != other.size) || (options != other.options) || (transform != other.transform);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Font family name.
  String family;
  //! @brief Font size.
  float size;
  //! @brief Font caps.
  FontOptions options;
  //! @brief Transformation matrix.
  TransformF transform;
};

// ============================================================================
// [Fog::Font_Local]
// ============================================================================

struct FOG_NO_EXPORT Font_Local
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Font_Local();
  ~Font_Local();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FontFace* getFaceFromCache(
    const String& family,
    float size,
    const FontOptions& options,
    const TransformF& transform);

  err_t putFaceToCache(FontFace* face);

  void resetFaceCache();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Lock lock;

  List<String> paths;
  List<String> list;
  bool listInitialized;

  UnorderedHash<FontFaceEntry, FontFace*> faceCache;

  NullFontFace nullFace;

  FontEngine* defaultEngine;
  FontFace* defaultFace;
};

Font_Local::Font_Local() :
  listInitialized(false),
  defaultEngine(NULL),
  defaultFace(NULL)
{
  defaultEngine = NULL;
  defaultFace = nullFace.ref();
}

Font_Local::~Font_Local()
{
  resetFaceCache();
  if (defaultFace) { fog_delete(defaultFace); defaultFace = NULL; }
  if (defaultEngine) { fog_delete(defaultEngine); defaultEngine = NULL; }
}

FontFace* Font_Local::getFaceFromCache(
  const String& family,
  float size,
  const FontOptions& options,
  const TransformF& transform)
{
  AutoLock locked(lock);
  FontFace* face = faceCache.value(FontFaceEntry(family, size, options, transform), NULL);

  if (face) face = face->ref();
  return face;
}

err_t Font_Local::putFaceToCache(FontFace* face)
{
  AutoLock locked(lock);
  err_t err = faceCache.put(
    FontFaceEntry(
      face->family, 
      face->metrics.getSize(),
      face->options,
      face->transform),
    face, false);

  if (err == ERR_OK) face->ref();
  return err;
}

void Font_Local::resetFaceCache()
{
  AutoLock locked(lock);
  UnorderedHash<FontFaceEntry, FontFace*>::MutableIterator it(faceCache);

  for (it.toStart(); it.isValid(); it.remove()) it.value()->deref();
}

static Static<Font_Local> _g2d_font_local;

// ============================================================================
// [Fog::FontFace]
// ============================================================================

FontFace::FontFace()
{
  refCount.init(1);
  type = FONT_FACE_NONE;
  flags = 0;
  metrics.reset();
}

FontFace::~FontFace()
{
}

FontFace* FontFace::ref() const
{
  refCount.inc();
  return const_cast<FontFace*>(this);
}

void FontFace::deref()
{
  if (refCount.deref()) fog_delete(this);
}

uint32_t FontFace::getHashCode() const
{
  return HashUtil::combineHash(
    family.getHashCode(),
    HashUtil::getHashCode(metrics.getSize()),
    options.getHashCode(),
    transform.getHashCode());
}

// ============================================================================
// [Fog::FontEngine]
// ============================================================================

FontEngine::FontEngine(uint32_t type) :
  _type(type)
{
}

FontEngine::~FontEngine()
{
}

// TODO: Move this out.
List<String> FontEngine::getDefaultFontDirectories()
{
  List<String> paths;

#if defined(FOG_OS_WINDOWS)
  // Add WIN font directory.
  String winFonts = OS::getWindowsDirectory();
  FileSystem::joinPath(winFonts, winFonts, Ascii8("fonts"));
  paths.append(winFonts);
#elif defined(FOG_OS_MAC)
  // Add MAC font directories.
  String home;

  if (UserInfo::getDirectory(home, USER_DIRECTORY_HOME) == ERR_OK)
  {
    paths.append(home + Ascii8("/Library/Fonts"));
  }

  paths.append(Ascii8("/System/Library/Fonts"));
  paths.append(Ascii8("/Library/Fonts"));
#elif defined(FOG_OS_POSIX)
  // add $HOME and $HOME/fonts directories.
  {
    String home;

    if (UserInfo::getDirectory(home, USER_DIRECTORY_HOME) == ERR_OK)
    {
      paths.append(home);
      paths.append(home + Ascii8("/fonts"));
    }
  }

  // Add font directories found in linux/unix systems...

  // Gentoo default font paths:
  paths.append(Ascii8("/usr/share/fonts"));
  paths.append(Ascii8("/usr/share/fonts/TTF"));
  paths.append(Ascii8("/usr/share/fonts/corefonts"));
  paths.append(Ascii8("/usr/share/fonts/local"));
  paths.append(Ascii8("/usr/share/fonts/ttf-bitstream-vera"));
  paths.append(Ascii8("/usr/local/share/fonts"));
  paths.append(Ascii8("/usr/local/share/fonts/TTF"));
  paths.append(Ascii8("/usr/local/share/fonts/corefonts"));
  paths.append(Ascii8("/usr/local/share/fonts/local"));
  paths.append(Ascii8("/usr/local/share/fonts/ttf-bitstream-vera"));

  // Ubuntu default truetype font paths:
  paths.append(Ascii8("/usr/share/fonts/truetype/msttcorefonts"));
#else
# warning "Fog::FontEngine::Unknown operating system target"
#endif // FOG_OS_POSIX

  return paths;
}

// ============================================================================
// [Fog::Font - Construction / Destruction]
// ============================================================================

Font::Font() :
  _d(_g2d_font_local->defaultFace->ref())
{
}

Font::Font(const Font& other) :
  _d(other._d->ref())
{
}

Font::~Font()
{
  _d->deref();
}

// ============================================================================
// [Fog::Font - Implicit Sharing]
// ============================================================================

void Font::reset()
{
  atomicPtrXchg(&_d, _g2d_font_local->defaultFace->ref())->deref();
}

// ============================================================================
// [Fog::Font - Face Family / Options / Metrics / Matrix]
// ============================================================================

err_t Font::setFamily(const String& family)
{
  return setFont(family, getSize(), getOptions(), getTransform());
}

err_t Font::setOptions(const FontOptions& options)
{
  return setFont(getFamily(), getSize(), options, getTransform());
}

err_t Font::setWeight(uint32_t weight)
{
  if (getWeight() == weight) return ERR_OK;

  FontOptions options = getOptions();
  options.setWeight(weight);
  return setFont(getFamily(), getSize(), options, getTransform());
}

err_t Font::setStyle(uint32_t style)
{
  if (getStyle() == style) return ERR_OK;

  FontOptions options = getOptions();
  options.setStyle(style);
  return setFont(getFamily(), getSize(), options, getTransform());
}

err_t Font::setDecoration(uint32_t decoration)
{
  if (getDecoration() == decoration) return ERR_OK;

  FontOptions options = getOptions();
  options.setDecoration(decoration);
  return setFont(getFamily(), getSize(), options, getTransform());
}

err_t Font::setKerning(uint32_t kerning)
{
  if (getKerning() == kerning) return ERR_OK;

  FontOptions options = getOptions();
  options.setKerning(kerning);
  return setFont(getFamily(), getSize(), options, getTransform());
}

err_t Font::setHinting(uint32_t hinting)
{
  if (getHinting() == hinting) return ERR_OK;

  FontOptions options = getOptions();
  options.setHinting(hinting);
  return setFont(getFamily(), getSize(), options, getTransform());
}

err_t Font::setSize(float size)
{
  if (getSize() == size) return ERR_OK;

  return setFont(getFamily(), size, getOptions(), getTransform());
}

err_t Font::setTransform(const TransformF& transform)
{
  return setFont(getFamily(), getSize(), getOptions(), transform);
}

err_t Font::setFont(const Font& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return ERR_OK;
}

err_t Font::setFont(const String& family, float size)
{
  return setFont(family, size, getOptions(), getTransform());
}

err_t Font::setFont(const String& family, float size, const FontOptions& options)
{
  return setFont(family, size, options, getTransform());
}

err_t Font::setFont(const String& family, float size, const FontOptions& options, const TransformF& transform)
{
  FontFace* face;

  // Try to get the font face from cache.
  face = _g2d_font_local->getFaceFromCache(family, size, options, transform);
  if (face)
  {
    atomicPtrXchg(&_d, face->ref())->deref();
    return ERR_OK;
  }

  // If we failed, try to create the font face through font engine.
  face = _g2d_font_local->defaultEngine->createFace(family, size, options, transform);
  if (!face) return ERR_FONT_NOT_MATCHED;

  // And put it to the cache.
  err_t err = _g2d_font_local->putFaceToCache(face);
  if (err == ERR_OK)
  {
    // Everything is OK. We increased reference count before adding face to cache
    // and we can't do it again!
    atomicPtrXchg(&_d, face)->deref();
    return ERR_OK;
  }

  // This can happen if there is concurrency. We created font face that was
  // created by another thread and put into the cache. Everything we need is
  // to delete our one and use font face that is in cache already.
  //
  // Second case is that engine created face that is close to demanded one but
  // not the same.
  if (err == ERR_RT_OBJECT_ALREADY_EXISTS)
  {
    FontFace* other = _g2d_font_local->getFaceFromCache(
      face->family, face->metrics.getSize(), face->options, face->transform);
    // It can also happen that cached face was deleted, in this case we use the
    // out one again :)
    if (other) { face->deref(); face = other; }
  }

  atomicPtrXchg(&_d, face)->deref();
  return ERR_OK;
}

// ============================================================================
// [Fog::Font - Face Methods]
// ============================================================================

err_t Font::getGlyphSet(const String& str, GlyphSet& glyphSet) const
{
  return _d->getGlyphSet(str.getData(), str.getLength(), glyphSet);
}

err_t Font::getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet) const
{
  return _d->getGlyphSet(str, length, glyphSet);
}

err_t Font::getOutline(const String& str, PathD& dst) const
{
  return _d->getOutline(str.getData(), str.getLength(), dst);
}

err_t Font::getOutline(const Char* str, sysuint_t length, PathD& dst) const
{
  return _d->getOutline(str, length, dst);
}

err_t Font::getTextExtents(const String& str, TextExtents& extents) const
{
  return _d->getTextExtents(str.getData(), str.getLength(), extents);
}

err_t Font::getTextExtents(const Char* str, sysuint_t length, TextExtents& extents) const
{
  return _d->getTextExtents(str, length, extents);
}

// ============================================================================
// [Fog::Font - Operator Overload]
// ============================================================================

Font& Font::operator=(const Font& other)
{
  setFont(other);
  return *this;
}

// ============================================================================
// [Fog::Font - Font Path Management]
// ============================================================================

bool Font::addFontPath(const String& path)
{
  AutoLock locked(_g2d_font_local->lock);

  if (!_g2d_font_local->paths.contains(path) && FileSystem::isDirectory(path))
  {
    _g2d_font_local->paths.append(path);
    return true;
  }
  else
    return false;
}

void Font::addFontPaths(const List<String>& paths)
{
  AutoLock locked(_g2d_font_local->lock);

  List<String>::ConstIterator it(paths);
  for (; it.isValid(); it.toNext())
  {
    String path = it.value();
    if (!_g2d_font_local->paths.contains(path) && FileSystem::isDirectory(path))
    {
      _g2d_font_local->paths.append(path);
    }
  }
}

bool Font::removeFontPath(const String& path)
{
  AutoLock locked(_g2d_font_local->lock);
  return _g2d_font_local->paths.remove(path) != 0;
}

bool Font::hasFontPath(const String& path)
{
  AutoLock locked(_g2d_font_local->lock);
  return _g2d_font_local->paths.contains(path);
}

bool Font::findFontFile(const String& fileName, String& dest)
{
  AutoLock locked(_g2d_font_local->lock);
  return FileSystem::findFile(_g2d_font_local->paths, fileName, dest);
}

List<String> Font::getPathList()
{
  AutoLock locked(_g2d_font_local->lock);
  return _g2d_font_local->paths;
}

// ============================================================================
// [Fog::Font - Font List Management]
// ============================================================================

List<String> Font::getFontList()
{
  AutoLock locked(_g2d_font_local->lock);
  if (!_g2d_font_local->listInitialized)
  {
    _g2d_font_local->list.append(_g2d_font_local->defaultEngine->getFontList());
    _g2d_font_local->listInitialized = true;
  }
  return _g2d_font_local->list;
}

// ============================================================================
// [Fog::Font - Font Face Management]
// ============================================================================

err_t Font::putFace(FontFace* face)
{
  return _g2d_font_local->putFaceToCache(face);
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_font_init(void)
{
  FontFace* face = NULL;

  _g2d_font_local.init();

  // Font Engine.
#if defined(FOG_FONT_WINDOWS)
  if (_g2d_font_local->defaultEngine == NULL)
  {
    _g2d_font_local->defaultEngine = fog_new WinFontEngine();
    if (_g2d_font_local->defaultEngine == NULL) goto _Fail;
  }
#endif // FOG_FONT_WINDOWS

#if defined(FOG_FONT_FREETYPE)
  if (_g2d_font_local->defaultEngine == NULL)
  {
    _g2d_font_local->defaultEngine = fog_new FTFontEngine();
    if (_g2d_font_local->defaultEngine == NULL) goto _Fail;
  }
#endif // FOG_FONT_FREETYPE

#if defined(FOG_FONT_MAC)
  if (_g2d_font_local->defaultEngine == NULL)
  {
    _g2d_font_local->defaultEngine = fog_new MacFontEngine();
    if (_g2d_font_local->defaultEngine == NULL) goto _Fail;
  }
#endif  // FOG_FONT_MAC

  // Add the engine specific font directories.
  Font::addFontPaths(
    _g2d_font_local->defaultEngine->getDefaultFontDirectories());

  // Set the engine specific default font face to the default one.
  face = _g2d_font_local->defaultEngine->createDefaultFace();
  if (face != NULL)
  {
    _g2d_font_local->defaultFace->deref();
    _g2d_font_local->defaultFace = face;
  }
  return;

_Fail:
  Debug::dbgFunc("Fog::Font", "init", 
    "Error when creating a default font face, setting to null.\n");
}

FOG_NO_EXPORT void _g2d_font_fini(void)
{
  // Destroy font manager.
  _g2d_font_local.destroy();
}

} // Fog namespace
