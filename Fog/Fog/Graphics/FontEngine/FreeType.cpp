// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Build/Build.h>

#if defined(FOG_FONT_FREETYPE)

// [Dependencies]
#include <Fog/Core/AutoLock.h>
#include <Fog/Core/Hash.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Library.h>
#include <Fog/Core/MapFile.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/OS.h>
#include <Fog/Core/FileSystem.h>
#include <Fog/Core/FileUtil.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/Strings.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Core/UserInfo.h>
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Font.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/TextLayout.h>

#include <Fog/Graphics/FontEngine/FreeType.h>

// #undef FOG_HAVE_FONTCONFIG

// These includes means
// --------------------
// #include <freetype/freetype.h>
// #include <freetype/ftoutln.h>
// #include <freetype/ttnameid.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_TRUETYPE_IDS_H

// [include fontconfig header]
#if defined(FOG_HAVE_FONTCONFIG)
#include <fontconfig/fontconfig.h>
#endif // FOG_HAVE_FONTCONFIG

#if !defined(FT_OPEN_MEMORY)
#define FT_OPEN_MEMORY ft_open_memory
#endif // FT_OPEN_MEMORY

// Fixed point and back. Note that freetype uses 6-bits precision (26.6 FP).
#define FT_FLOOR(X) ((X & -64) / 64)
#define FT_CEIL(X) (((X + 63) & -64) / 64)

// Returns true is mactrices 'a' and 'b' are equal.
#define FT_Matrix_Equal(a, b) ( \
  (a)->xx == (b)->xx && (a)->yy == (b)->yy && \
  (a)->xy == (b)->xy && (a)->yx == (b)->yx )

namespace Fog {

// ============================================================================
// [Fog::FreeTypeLibrary]
// ============================================================================

struct FOG_HIDDEN FreeTypeLibrary
{
  FreeTypeLibrary();
  ~FreeTypeLibrary();

  err_t init();
  void close();

  enum { NUM_SYMBOLS = 12 };
  union
  {
    struct
    {
      FOG_CDECL FT_Error (*pFT_Init_FreeType)(FT_Library*);
      FOG_CDECL FT_Error (*pFT_Done_FreeType)(FT_Library);
      FOG_CDECL FT_Long  (*pFT_MulFix)(FT_Long /* a */, FT_Long /* b */);
      FOG_CDECL FT_Long  (*pFT_DivFix)(FT_Long /* a */, FT_Long /* b */);
      FOG_CDECL FT_Error (*pFT_New_Face)(FT_Library, const char* /* fileName */, FT_Long /* face_index */, FT_Face* /* face */);
      FOG_CDECL FT_Error (*pFT_Open_Face)(FT_Library, const FT_Open_Args* /* openArgs */, FT_Long /* face_index */, FT_Face* /* face */);
      FOG_CDECL FT_Error (*pFT_Done_Face)(FT_Face /* face */);
      FOG_CDECL FT_Error (*pFT_Set_Char_Size)(FT_Face /* face */, FT_F26Dot6 /* char_width */, FT_F26Dot6 /* char_height */, FT_UInt /* horz_resolution */, FT_UInt /* vert_resolution */);
      FOG_CDECL FT_Error (*pFT_Set_Pixel_Sizes)(FT_Face /* face */, FT_UInt /* pixel_width */, FT_UInt /* pixel_height */);
      FOG_CDECL FT_UInt  (*pFT_Get_Char_Index)(FT_Face /* face */, FT_ULong /* charcode */);
      FOG_CDECL FT_Error (*pFT_Load_Glyph)(FT_Face /* face */, FT_UInt /* glyph_index */, FT_Int32 /* load_flags */);
      FOG_CDECL FT_Error (*pFT_Render_Glyph)(FT_GlyphSlot /* slot */, FT_Render_Mode /* glyph_index */);
    };
    void* addr[NUM_SYMBOLS];
  };

  Library dll;
  uint32_t ok;

  FT_Library ftLibrary;

private:
  FOG_DISABLE_COPY(FreeTypeLibrary)
};

FreeTypeLibrary::FreeTypeLibrary() : ok(false) { init(); }
FreeTypeLibrary::~FreeTypeLibrary() { close(); }

err_t FreeTypeLibrary::init()
{
  static const char symbols[] =
    "FT_Init_FreeType\0"
    "FT_Done_FreeType\0"
    "FT_MulFix\0"
    "FT_DivFix\0"
    "FT_New_Face\0"
    "FT_Open_Face\0"
    "FT_Done_Face\0"
    "FT_Set_Char_Size\0"
    "FT_Set_Pixel_Sizes\0"
    "FT_Get_Char_Index\0"
    "FT_Load_Glyph\0"
    "FT_Render_Glyph\0";

  // Try to load FreeType library.
  if (dll.open(Ascii8("freetype")) != ERR_OK)
  {
    // No FreeType library found.
    return ERR_FONT_FREETYPE_NOT_LOADED;
  }

  // Try to load all symbols.
  const char* badSymbol;
  if (dll.getSymbols(addr, symbols, FOG_ARRAY_SIZE(symbols), NUM_SYMBOLS, (char**)&badSymbol) != NUM_SYMBOLS)
  {
    // Some symbol failed to load? Inform about it.
    fog_debug("Fog::FreeTypeLibrary::init() - Can't load symbol '%s'.", badSymbol);

    dll.close();
    return ERR_FONT_FREETYPE_NOT_LOADED;
  }

  FT_Error error;
  if ((error = pFT_Init_FreeType(&ftLibrary)) != 0)
  {
    fog_debug("Fog::FreeTypeLibrary::init() - Can't init FreeType, FT_Error: %u.", error);

    dll.close();
    return ERR_FONT_FREETYPE_INIT_FAILED;
  }

  ok = true;
  return ERR_OK;
}

void FreeTypeLibrary::close()
{
  if (ok)
  {
    pFT_Done_FreeType(ftLibrary);

    dll.close();
    ok = false;
  }
}

static Static<FreeTypeLibrary> _freeTypeLib;

// ============================================================================
// [Fog::FontConfigLibrary]
// ============================================================================

#if defined(FOG_HAVE_FONTCONFIG)
struct FOG_HIDDEN FontConfigLibrary
{
  FontConfigLibrary();
  ~FontConfigLibrary();

  err_t init();
  void close();

  List<String> getFontDirectories() const;
  List<String> getFontList() const;

  String resolveFontPath(const String& family, uint32_t size, const FontCaps& caps) const;

  enum { NUM_SYMBOLS = 25 };
  union
  {
    struct
    {
      FcConfig* (*pFcInitLoadConfig)(void);
      FcConfig* (*pFcInitLoadConfigAndFonts)(void);
      FcBool (*pFcInit)(void);
      void (*pFcFini)(void);
      int (*pFcGetVersion)(void);
      FcBool (*pFcInitReinitialize)(void);
      FcBool (*pFcInitBringUptoDate)(void);

      FcPattern* (*pFcPatternCreate)(void);
      void (*pFcPatternDestroy)(FcPattern *p);
      FcResult (*pFcPatternGet)(const FcPattern *p, const char *object, int id, FcValue *v);
      FcResult (*pFcPatternAddInteger)(const FcPattern *p, const char *object, int i);
      FcResult (*pFcPatternAddDouble)(const FcPattern *p, const char *object, double d);
      FcObjectSet* (*pFcObjectSetBuild)(const char *first, ...);
      void (*pFcObjectSetDestroy)(FcObjectSet *os);
      void (*pFcFontSetDestroy)(FcFontSet *s);
      FcPattern* (*pFcNameParse)(const FcChar8 *name);
      FcChar8* (*pFcNameUnparse)(FcPattern *pat);
      FcFontSet* (*pFcFontList)(FcConfig* config, FcPattern* p, FcObjectSet* os);
      FcFontSet* (*pFcFontSort)(FcConfig *config, FcPattern *p, FcBool trim, FcCharSet **csp, FcResult *result);
      FcPattern* (*pFcFontMatch)(FcConfig *config, FcPattern *p, FcResult *result);
      FcBool (*pFcConfigSubstitute)(FcConfig* config, FcPattern *p, FcMatchKind kind);
      void (*pFcDefaultSubstitute)(FcPattern *pattern);

      FcStrList* (*pFcConfigGetFontDirs)(FcConfig* config);
      FcChar8* (*pFcStrListNext)(FcStrList* list);
      void (*pFcStrListDone)(FcStrList* list);
    };
    void* addr[NUM_SYMBOLS];
  };

  Library dll;
  uint32_t ok;

private:
  FOG_DISABLE_COPY(FontConfigLibrary)
};

FontConfigLibrary::FontConfigLibrary() : ok(false) { init(); }
FontConfigLibrary::~FontConfigLibrary() { close(); }

err_t FontConfigLibrary::init()
{
  static const char symbols[] =
    "FcInitLoadConfig\0"
    "FcInitLoadConfigAndFonts\0"
    "FcInit\0"
    "FcFini\0"
    "FcGetVersion\0"
    "FcInitReinitialize\0"
    "FcInitBringUptoDate\0"
    "FcPatternCreate\0"
    "FcPatternDestroy\0"
    "FcPatternGet\0"
    "FcPatternAddInteger\0"
    "FcPatternAddDouble\0"
    "FcObjectSetBuild\0"
    "FcObjectSetDestroy\0"
    "FcFontSetDestroy\0"
    "FcNameParse\0"
    "FcNameUnparse\0"
    "FcFontList\0"
    "FcFontSort\0"
    "FcFontMatch\0"
    "FcConfigSubstitute\0"
    "FcDefaultSubstitute\0"
    "FcConfigGetFontDirs\0"
    "FcStrListNext\0"
    "FcStrListDone\0";

  // Try to load FontConfig library.
  if (dll.open(Ascii8("fontconfig")) != ERR_OK)
  {
    // No FontConfig library found.
    return ERR_FONT_FONTCONFIG_NOT_LOADED;
  }

  // Try to load all symbols.
  const char* badSymbol;
  if (dll.getSymbols(addr, symbols, FOG_ARRAY_SIZE(symbols), NUM_SYMBOLS, (char**)&badSymbol) != NUM_SYMBOLS)
  {
    // Some symbol failed to load? Inform about it.
    fog_debug("Fog::FontConfigLibrary::init() - Can't load symbol '%s'.", badSymbol);
    dll.close();
    return ERR_FONT_FONTCONFIG_NOT_LOADED;
  }

  if (!pFcInit())
  {
    fog_debug("Fog::FontConfigLibrary::init() - Can't init FontConfig.");
    dll.close();
    return ERR_FONT_FONTCONFIG_INIT_FAILED;
  }

  ok = true;
  return ERR_OK;
}

void FontConfigLibrary::close()
{
  if (ok)
  {
    pFcFini();

    dll.close();
    ok = false;
  }
}

List<String> FontConfigLibrary::getFontDirectories() const
{
  FOG_ASSERT(ok);

  List<String> result;

  FcStrList* list = pFcConfigGetFontDirs(NULL);
  char* dirNameA;
  String dirNameU;

  while ((dirNameA = (char*)pFcStrListNext(list)) != NULL)
  {
    TextCodec::local8().toUnicode(dirNameU, dirNameA);
    if (!result.contains(dirNameU))
    {
      dirNameU.squeeze();
      result.append(dirNameU);
    }
  }

  pFcStrListDone(list);
  return result;
}

List<String> FontConfigLibrary::getFontList() const
{
  FOG_ASSERT(ok);

  List<String> result;
  FcPattern* p = pFcPatternCreate();
  FcFontSet* set = NULL;
  FcObjectSet* os = pFcObjectSetBuild(FC_FAMILY, FC_STYLE, NULL);

  if (p && os) set = pFcFontList(NULL, p, os);

  if (set)
  {
    String tmp;

    for (sysint_t i = 0; i < set->nfont; i++)
    {
      FcValue val;
      pFcPatternGet(set->fonts[i], FC_FAMILY, 0, &val);
      TextCodec::local8().toUnicode(tmp, reinterpret_cast<const char*>(val.u.s));

      if (!result.contains(tmp))
      {
        tmp.squeeze();
        result.append(tmp);
      }
    }

    pFcFontSetDestroy(set);
  }

  if (p) pFcPatternDestroy(p);
  if (os) pFcObjectSetDestroy(os);

  return result;
}

String FontConfigLibrary::resolveFontPath(const String& family, uint32_t size, const FontCaps& caps) const
{
  FOG_ASSERT(ok);

  TemporaryByteArray<TEMP_LENGTH> family8;
  TextCodec::local8().appendFromUnicode(family8, family);

  FcPattern* p1;
  FcPattern* p2;
  FcValue filename;
  FcResult res;
  String result;

  p1 = pFcNameParse((FcChar8 *)family8.getData());
  pFcPatternAddDouble(p1, FC_SIZE, (double)size);

  if (caps.italic) pFcPatternAddInteger(p1, FC_SLANT, FC_SLANT_ITALIC);
  if (caps.bold) pFcPatternAddInteger(p1, FC_WEIGHT, FC_WEIGHT_BOLD);

  pFcConfigSubstitute(NULL, p1, FcMatchPattern);
  pFcDefaultSubstitute(p1);

  if ((p2 = pFcFontMatch(NULL, p1, &res)))
  {
    pFcPatternGet(p2, FC_FILE, 0, &filename);
    TextCodec::local8().toUnicode(result, (const char*)filename.u.s);
    pFcPatternDestroy(p2);
  }

  pFcPatternDestroy(p1);

  return result;
}

static Static<FontConfigLibrary> _fontConfigLib;
#endif // FOG_HAVE_FONTCONFIG

// ============================================================================
// [Fog::FontTranslator]
// ============================================================================

// FontTranslator is used if FontConfig load failed. It contains basic fonts
// thats used and it tries to translate family to file name that can be opened
// and loaded by FreeType.
struct FOG_HIDDEN FontTranslatorRecord
{
  FOG_INLINE FontTranslatorRecord() {}
  FOG_INLINE FontTranslatorRecord(String family, String fileName, bool bold, bool italic) :
    family(family),
    fileName(fileName)
  {
    attr.bold = bold;
    attr.italic = italic;
    attr.strike = false;
    attr.underline = false;
  }

  FOG_INLINE ~FontTranslatorRecord() {}

  // Font family (lowercased, should be case-insensitive).
  String family;
  // File name (case sensitive).
  String fileName;
  // Only bold and italic caps are used.
  FontCaps attr;
};

// This is ugly...
}
FOG_DECLARE_TYPEINFO(Fog::FontTranslatorRecord, Fog::TYPEINFO_MOVABLE)
namespace Fog {

struct FOG_HIDDEN FontTranslator
{
  FontTranslator();
  ~FontTranslator();

  err_t initCustomDefinitions();
  err_t addCustomDefinitions(const char* defs);
  void freeCustomDefinitions();

  String resolveFontPath(const String& family, uint32_t size, const FontCaps& attr) const;

  List<FontTranslatorRecord> _records;

private:
  FOG_DISABLE_COPY(FontTranslator)
};

FontTranslator::FontTranslator() {}
FontTranslator::~FontTranslator() { freeCustomDefinitions(); }

err_t FontTranslator::initCustomDefinitions()
{
  static const char defs[] =
    // SYNTAX: "Font Family\0" "R|B|I|BI\0" "fontfile.ttf\0"

    // Basic definitions (corefonts or msttcorefonts).
    "andale mono\0"              "R\0"  "andalemo.ttf\0"
    "arial\0"                    "R\0"  "arial.ttf\0"
    "arial\0"                    "B\0"  "arialbd.ttf\0"
    "arial\0"                    "I\0"  "ariali.ttf\0"
    "arial\0"                    "BI\0" "arialbi.ttf\0"
    "arial black\0"              "R\0"  "ariblk.ttf\0"
    "comic sans\0"               "R\0"  "comic.ttf\0"
    "comic sans\0"               "B\0"  "comicbd.ttf\0"
    "comic sans ms\0"            "R\0"  "comic.ttf\0"
    "comic sans ms\0"            "B\0"  "comicbd.ttf\0"
    "courier\0"                  "R\0"  "cour.ttf\0"
    "courier\0"                  "B\0"  "courbd.ttf\0"
    "courier\0"                  "I\0"  "couri.ttf\0"
    "courier\0"                  "BI\0" "courbi.ttf\0"
    "courier new\0"              "R\0"  "cour.ttf\0"
    "courier new\0"              "B\0"  "courbd.ttf\0"
    "courier new\0"              "I\0"  "couri.ttf\0"
    "courier new\0"              "BI\0" "courbi.ttf\0"
    "georgia\0"                  "R\0"  "georgia.ttf\0"
    "georgia\0"                  "B\0"  "georgiab.ttf\0"
    "georgia\0"                  "I\0"  "georgiai.ttf\0"
    "georgia\0"                  "BI\0" "georgiaz.ttf\0"
    "impact\0"                   "R\0"  "impact.ttf\0"
    "tahoma\0"                   "R\0"  "tahoma.ttf\0"
    "tahoma\0"                   "B\0"  "tahomabd.ttf\0"
    "times\0"                    "R\0"  "times.ttf\0"
    "times\0"                    "B\0"  "timesbd.ttf\0"
    "times\0"                    "I\0"  "timesi.ttf\0"
    "times\0"                    "BI\0" "timesbi.ttf\0"
    "times new roman\0"          "R\0"  "times.ttf\0"
    "times new roman\0"          "B\0"  "timesbd.ttf\0"
    "times new roman\0"          "I\0"  "timesi.ttf\0"
    "times new roman\0"          "BI\0" "timesbi.ttf\0"
    "trebuchet\0"                "R\0"  "trebuc.ttf\0"
    "trebuchet\0"                "B\0"  "trebucbd.ttf\0"
    "trebuchet\0"                "I\0"  "trebucit.ttf\0"
    "trebuchet\0"                "BI\0" "trebucbi.ttf\0"
    "trebuchet ms\0"             "R\0"  "trebuc.ttf\0"
    "trebuchet ms\0"             "B\0"  "trebucbd.ttf\0"
    "trebuchet ms\0"             "I\0"  "trebucit.ttf\0"
    "trebuchet ms\0"             "BI\0" "trebucbi.ttf\0"
    "verdana\0"                  "R\0"  "verdana.ttf\0"
    "verdana\0"                  "B\0"  "verdanab.ttf\0"
    "verdana\0"                  "I\0"  "verdanai.ttf\0"
    "verdana\0"                  "BI\0" "verdanaz.ttf\0"

    // Bitstream Vera Sans.
    "bitstream vera\0"           "R\0"  "Vera.ttf\0"
    "bitstream vera sans\0"      "R\0"  "Vera.ttf\0"
    "bitstream vera sans mono\0" "R\0"  "VeraMono.ttf\0"
    "bitstream vera serif\0"     "R\0"  "VeraSe.ttf\0"
    "vera\0"                     "R\0"  "Vera.ttf\0"

    // Here can be added others standard fonts in Linux/BSD distros.
    "tahoma\0"                   "R\0"  "tahoma.ttf\0";

  return addCustomDefinitions(defs);
}

err_t FontTranslator::addCustomDefinitions(const char* defs)
{
  const char* cur = defs;
  String s0;
  String s1;

  for (;;)
  {
    // This means that first character in defs (defs[0]) is zero terminator or
    // we catched double null terminator that describes end of definitions.
    if (cur[0] == '\0') break;

    sysuint_t len;
    uint8_t bold = false;
    uint8_t italic = false;

    len = strlen(cur);
    s0.set(Ascii8(cur, len));
    cur += len + 1;

    len = strlen(cur);
    if (len != 1 && len != 2) return ERR_RT_INVALID_ARGUMENT;

    if (cur[0] == 'B' || cur[1] == 'B') bold = true;
    if (cur[0] == 'I' || cur[1] == 'I') italic = true;

    cur += len + 1;
    len = strlen(cur);

    s1.set(Ascii8(cur, len));
    cur += len + 1;

    err_t err = _records.append(FontTranslatorRecord(s0, s1, bold, italic));
    if (err) return err;
  }

  return ERR_OK;
}

void FontTranslator::freeCustomDefinitions()
{
  _records.free();
}

String FontTranslator::resolveFontPath(const String& family, uint32_t size, const FontCaps& attr) const
{
  String result;

  // Custom definitions bypasses FontConfig.
  String familyLower = family.lowered();

  List<String> fontPaths = FontManager::getPathList();
  List<FontTranslatorRecord> records(_records);
  List<FontTranslatorRecord>::ConstIterator it(records);

  for (it.toStart(); it.isValid(); it.toNext())
  {
    const FontTranslatorRecord& rec = it.value();

    if (rec.family == familyLower && rec.attr.bold == attr.bold && rec.attr.italic == attr.italic)
    {
      if (FileSystem::findFile(fontPaths, rec.fileName, result)) return result;
    }
  }

  // Not found. If FontConfig support is compiled it's time to try it.
#if defined(FOG_HAVE_FONTCONFIG)
  if (_fontConfigLib->ok) result = _fontConfigLib->resolveFontPath(family, size, attr);
#endif // FOG_HAVE_FONTCONFIG

  // Not found...
  return result;
}

static Static<FontTranslator> _fontTranslator;

// ============================================================================
// [Init]
// ============================================================================

FOG_INIT_DECLARE err_t initFTFontEngine(void)
{
  using namespace Fog;

#if defined(FOG_HAVE_FONTCONFIG)
  _fontConfigLib.init();
#endif // FOG_HAVE_FONTCONFIG
  _freeTypeLib.init();
  _fontTranslator.init();

  // Initialize custom translator definitions if FontConfig failed to load or
  // if it's not supported (compiled).
#if defined(FOG_HAVE_FONTCONFIG)
  if (!_fontConfigLib->ok) 
  {
#endif // FOG_HAVE_FONTCONFIG

  _fontTranslator->initCustomDefinitions();

#if defined(FOG_HAVE_FONTCONFIG)
  }
#endif // FOG_HAVE_FONTCONFIG

  return ERR_OK;
}

FOG_INIT_DECLARE void shutdownFTFontEngine(void)
{
  using namespace Fog;

  _fontTranslator.destroy();
  _freeTypeLib.destroy();
#if defined(FOG_HAVE_FONTCONFIG)
  _fontConfigLib.destroy();
#endif // FOG_HAVE_FONTCONFIG
}

// ============================================================================
// [Fog::FTFile]
// ============================================================================

// FTFile represends one freetype file and it's face object. It's allowed to
// load and uload faces on-the-fly.
struct FOG_HIDDEN FTFontFile
{
  // [Construction / Destruction]
  FTFontFile(const String& fileName, const String& family);
  ~FTFontFile();
  
  // [Reference Counting]

  FTFontFile* ref();
  void deref();
  
  // [Use / Unuse]

  // use() and unuse() are public methods that's used by engine.
  bool use();
  void unuse();

  // [Load / Unload]

  bool load();
  void unload();

  FOG_INLINE bool isLoaded() const;
  
  // [Setup]

  bool setupSize(uint32_t size);
  FontFace* createFace(uint32_t size, const FontCaps& caps);

  // [Members]

  //! @brief Reference count.
  Atomic<sysuint_t> refCount;
  //! @brief Use count.
  Atomic<sysuint_t> used;

  //! @brief Absolute file path.
  String fileName;
  //! @brief Family.
  String family;

  //! @brief Map file object.
  MapFile mapFile;

  //! @brief Freetype face.
  FT_Face currentFace;
  //! @brief Loading error.
  FT_Error loadError;

  //! @brief Unload after use.
  uint32_t unloadAfterUse;
  //! @brief Fixed width index.
  uint32_t fixedWidthIndex;

private:
  FOG_DISABLE_COPY(FTFontFile)
};

// ============================================================================
// [Fog::FTFontEngine]
// ============================================================================

FTFontEngine::FTFontEngine() :
  FontEngine(Ascii8("FreeType"))
{
  err_t err = initFTFontEngine();
  if (err)
  {
    fog_debug("Fog::FTFontEngine::FTFontEngine() - Can't load or initialize FreeType, error %u.", err);
  }
}

FTFontEngine::~FTFontEngine()
{
  close();
  shutdownFTFontEngine();
}

List<String> FTFontEngine::getFontList()
{
  // TODO:
  List<String> fonts;
  return fonts;
}

FontFace* FTFontEngine::createDefaultFace()
{
  // Bail if FreeType library is not loaded.
  if (!_freeTypeLib->ok) return NULL;

  FontCaps caps;
  int size = 12;

  memset(&caps, 0, sizeof(FontCaps));
  
  FontFace* face = NULL;

#if defined(FOG_HAVE_FONTCONFIG)
  face = FontManager::getFace(Ascii8("default"), size, caps);
#endif

  // If face wasn't loaded, try some generic font names.
  if (!face)
  {
    static const char defaultFaceNames[] =
      "arial\0"
      "times\0"
      "times new roman\0"
      "verdana\0"
      "bitstream vera\0"
      "vera\0";

    const char* p = defaultFaceNames;

    while (*p)
    {
      sysuint_t plen = strlen(p);
      face = FontManager::getFace(Ascii8(p, plen), size, caps);
      if (face) break;

      p += plen + 1;
    }
  }

  return face;
}

FontFace* FTFontEngine::createFace(
  const String& family, uint32_t size,
  const FontCaps& caps)
{
  // Bail if FreeType library is not loaded.
  if (!_freeTypeLib->ok) return NULL;

  String fileName = _fontTranslator->resolveFontPath(family, size, caps);

  if (!fileName.isEmpty())
  {
    // First look if this file is in cache.
    FTFontFile* file = _ftCache.value(fileName, NULL);

    if (!file)
    {
      file = new(std::nothrow) FTFontFile(fileName, family);

      // Ensure that file is OK.
      if (!file) return NULL;

      if (file->loadError)
      {
        delete file;
        return NULL;
      }

      // Put to cache, reference count is now 1 thats correct.
      // FIXME: Not true, reference count is still zero and must be
      _ftCache.put(fileName, file);
    }

    // If face was created, reference count is increased too.
    return file->createFace(size, caps);
  }

  return NULL;
}

List<String> FTFontEngine::getDefaultFontDirectories()
{
#if defined(FOG_HAVE_FONTCONFIG)
  if (_fontConfigLib->ok) return _fontConfigLib->getFontDirectories();
#endif // FOG_HAVE_FONTCONFIG

  return FontEngine::getDefaultFontDirectories();
}

void FTFontEngine::close()
{
  // Bail if FreeType library is not loaded.
  if (!_freeTypeLib->ok) return;

  UnorderedHash<String, FTFontFile*>::MutableIterator it(_ftCache);

  for (it.toStart(); it.isValid(); it.remove())
  {
    FTFontFile* file = it.value();
    if (file->refCount.get())
    {
      fog_stderr_msg("Fog::FTFontEngine", "close", "FTFile not dereferenced (refCount %ld)", (long)file->refCount.get());
    }

    delete file;
  }
}

// ============================================================================
// [Fog::FTFontFace - Helpers]
// ============================================================================

static FOG_INLINE double fx26p6ToDouble(int p) { return double(p) / 64.0; }

static err_t decompose_ft_glyph_outline(const FT_Outline& outline, bool flipY, const Matrix* mtx, Path& path)
{
  int n;         // Index of contour in outline.
  int first = 0; // Index of first point in contour.

  err_t err = ERR_OK;
  sysuint_t subPathIndex = path.getLength();

  for (n = 0; n < outline.n_contours; n++)
  {
    // Index of last point in contour.
    int last  = outline.contours[n];
    FT_Vector* limit = outline.points + last;

    FT_Vector v_start   = outline.points[first];
    FT_Vector v_last    = outline.points[last];
    FT_Vector v_control = v_start;

    FT_Vector* point    = outline.points + first;
    char* tags          = outline.tags + first;
    char tag            = FT_CURVE_TAG(tags[0]);

    double x1, y1, x2, y2, x3, y3;

    // A contour cannot start with a cubic control point!
    if (tag == FT_CURVE_TAG_CUBIC) { err = ERR_FONT_INVALID_DATA; goto end; }

    // Check first point to determine origin.
    if (tag == FT_CURVE_TAG_CONIC)
    {
      // First point is conic control. Yes, this happens.
      if (FT_CURVE_TAG(outline.tags[last]) == FT_CURVE_TAG_ON)
      {
        // Start at last point if it is on the curve.
        v_start = v_last;
        limit--;
      }
      else
      {
        // If both first and last points are conic, start at their middle and
        // record its position for closure.
        v_start.x = (v_start.x + v_last.x) / 2;
        v_start.y = (v_start.y + v_last.y) / 2;

        v_last = v_start;
      }
      point--;
      tags--;
    }

    x1 = fx26p6ToDouble(v_start.x);
    y1 = fx26p6ToDouble(v_start.y);
    if (flipY) y1 = -y1;
    if ((err = path.moveTo(x1, y1))) goto end;

    while (point < limit)
    {
      point++;
      tags++;

      tag = FT_CURVE_TAG(tags[0]);
      switch(tag)
      {
        case FT_CURVE_TAG_ON:  // emit a single line_to
        {
          x1 = fx26p6ToDouble(point->x);
          y1 = fx26p6ToDouble(point->y);
          if (flipY) y1 = -y1;
          if ((err = path.lineTo(x1, y1))) goto end;
          continue;
        }

        case FT_CURVE_TAG_CONIC:  // consume conic arcs
        {
          v_control.x = point->x;
          v_control.y = point->y;
doConic:
          if (point < limit)
          {
            FT_Vector vec;
            FT_Vector v_middle;

            point++;
            tags++;
            tag = FT_CURVE_TAG(tags[0]);

            vec.x = point->x;
            vec.y = point->y;

            if (tag == FT_CURVE_TAG_ON)
            {
              x1 = fx26p6ToDouble(v_control.x);
              y1 = fx26p6ToDouble(v_control.y);
              x2 = fx26p6ToDouble(vec.x);
              y2 = fx26p6ToDouble(vec.y);
              if (flipY) { y1 = -y1; y2 = -y2; }
              if ((err = path.curveTo(x1, y1, x2, y2))) goto end;
              continue;
            }

            if (tag != FT_CURVE_TAG_CONIC) { err = ERR_FONT_INVALID_DATA; goto end; }

            v_middle.x = (v_control.x + vec.x) / 2;
            v_middle.y = (v_control.y + vec.y) / 2;

            x1 = fx26p6ToDouble(v_control.x);
            y1 = fx26p6ToDouble(v_control.y);
            x2 = fx26p6ToDouble(v_middle.x);
            y2 = fx26p6ToDouble(v_middle.y);
            if (flipY) { y1 = -y1; y2 = -y2; }
            if ((err = path.curveTo(x1, y1, x2, y2))) goto end;

            v_control = vec;
            goto doConic;
          }

          x1 = fx26p6ToDouble(v_control.x);
          y1 = fx26p6ToDouble(v_control.y);
          x2 = fx26p6ToDouble(v_start.x);
          y2 = fx26p6ToDouble(v_start.y);
          if (flipY) { y1 = -y1; y2 = -y2; }
          path.curveTo(x1, y1, x2, y2);
          goto doClose;
        }

        case FT_CURVE_TAG_CUBIC:
        {
          FT_Vector vec1, vec2;

          if (point + 1 > limit || FT_CURVE_TAG(tags[1]) != FT_CURVE_TAG_CUBIC)
          {
            err = ERR_FONT_INVALID_DATA;
            goto end;
          }

          vec1.x = point[0].x;
          vec1.y = point[0].y;
          vec2.x = point[1].x;
          vec2.y = point[1].y;

          point += 2;
          tags  += 2;

          if (point <= limit)
          {
            FT_Vector vec;

            vec.x = point->x;
            vec.y = point->y;

            x1 = fx26p6ToDouble(vec1.x);
            y1 = fx26p6ToDouble(vec1.y);
            x2 = fx26p6ToDouble(vec2.x);
            y2 = fx26p6ToDouble(vec2.y);
            x3 = fx26p6ToDouble(vec.x);
            y3 = fx26p6ToDouble(vec.y);
            if (flipY) { y1 = -y1; y2 = -y2; y3 = -y3; }
            if ((err = path.cubicTo(x1, y1, x2, y2, x3, y3))) goto end;
            continue;
          }

          x1 = fx26p6ToDouble(vec1.x);
          y1 = fx26p6ToDouble(vec1.y);
          x2 = fx26p6ToDouble(vec2.x);
          y2 = fx26p6ToDouble(vec2.y);
          x3 = fx26p6ToDouble(v_start.x);
          y3 = fx26p6ToDouble(v_start.y);

          if (flipY) { y1 = -y1; y2 = -y2; y3 = -y3; }
          if ((err = path.cubicTo(x1, y1, x2, y2, x3, y3))) goto end;

          goto doClose;
        }

        default:
        {
          err = ERR_FONT_INVALID_DATA;
          goto end;
        }
      }
    }
    path.closePolygon();

doClose:
    first = last + 1;
  }

end:
  if (mtx) path.applyMatrix(*mtx, Range(subPathIndex, DETECT_LENGTH));
  return err;
}

// ============================================================================
// [Fog::FTFontFace]
// ============================================================================

FTFontFace::FTFontFace() :
  file(NULL)
{
}

FTFontFace::~FTFontFace()
{
  if (file) file->deref();
}

err_t FTFontFace::getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet)
{
  if (length == DETECT_LENGTH) length = StringUtil::len(str);
  if (length == 0) return ERR_OK;

  // This really shouldn't happen!
  if (str[0].isTrailSurrogate()) return ERR_STRING_INVALID_UTF16;

  err_t err;
  if ((err = glyphSet.begin(length))) return err;

  AutoLock locked(lock);
  GlyphData* glyphd;

  sysuint_t remain = length;
  do {
    // Get unicode character, it's also needed to handle surrogate pairs.
    uint32_t uc = str[0].ch();
    str++;
    remain--;

    if (Char::isSurrogatePair((uint16_t)uc))
    {
      if (!remain) return ERR_STRING_TRUNCATED;
      uc = Char::fromSurrogate((uint16_t)uc, str[0].ch());

      str++;
      remain--;
    }

    // First try cache.
    glyphd = glyphCache.get(uc);
    if (FOG_UNLIKELY(!glyphd))
    {
      glyphd = renderGlyph(uc);
      if (glyphd != NULL) glyphCache.set(uc, glyphd);
    }

    if (FOG_LIKELY(glyphd)) glyphSet._add(glyphd->ref());
  } while (remain);

  err = glyphSet.end();
  return err;
}

err_t FTFontFace::getOutline(const Char* str, sysuint_t length, Path& dst)
{
  if (scalable == 0) return ERR_FONT_CANT_GET_OUTLINE;

  if (length == DETECT_LENGTH) length = StringUtil::len(str);
  if (length == 0) return ERR_OK;

  // This really shouldn't happen!
  if (str[0].isTrailSurrogate()) return ERR_STRING_INVALID_UTF16;

  err_t err = ERR_OK;
  AutoLock locked(lock);

  sysuint_t remain = length;
  do {
    // Get unicode character, it's also needed to handle surrogate pairs.
    uint32_t uc = str[0].ch();
    str++;
    remain--;

    if (Char::isSurrogatePair((uint16_t)uc))
    {
      if (!remain) return ERR_STRING_TRUNCATED;
      uc = Char::fromSurrogate((uint16_t)uc, str[0].ch());

      str++;
      remain--;
    }
  } while (remain);

  return err;
}

err_t FTFontFace::getTextExtents(const Char* str, sysuint_t length, TextExtents& extents)
{
  TemporaryGlyphSet<128> glyphSet;
  err_t err = getGlyphSet(str, length, glyphSet);

  if (err)
  {
    memset(&extents, 0, sizeof(TextExtents));
    return err;
  }
  else
  {
    extents.advance = glyphSet.getAdvance();
    extents.beginWidth = 0;
    extents.endWidth = 0;
    return ERR_OK;
  }
}

GlyphData* FTFontFace::renderGlyph(uint32_t uc)
{
  GlyphData* glyphd = NULL;

  // If glyph is not in cache, it's needed to render it.
  FT_GlyphSlot slot;
  FT_Face face;
  FT_Error error;

  uint y;
  uint index;

  file->use();

  face = file->currentFace;
  // Setup font size (mainly for scalable fonts).
  file->setupSize(metrics.size);

  // Get glyph index.
  if ((index = _freeTypeLib->pFT_Get_Char_Index(face, uc)) == 0) goto end;
  // Try to load the glyph.
  if ((error = _freeTypeLib->pFT_Load_Glyph(face, index, FT_LOAD_DEFAULT)) != 0) goto end;

  // Shortcuts...
  slot = face->glyph;

  {
    int offsetX;
    int offsetY;
    int advance;
    int width = 0;

    // calculate offsets for X and Y coordinates.
    // freetype returns only bitmap/pixmap that's needed to
    // sucessfully render glyph with anti-aliasing or without,
    // space is never larger, so we must use these values and
    // we are using them in our rendering process too.
    offsetX = FT_FLOOR(slot->metrics.horiBearingX);
    offsetY = metrics.ascent - FT_FLOOR(slot->metrics.horiBearingY) - 1;
    advance = FT_CEIL(slot->metrics.horiAdvance);

    /* Adjust for bold and italic text */
    //if (Param->Weight >= 5) Width += int(m->GlyphOverhang);

    // we can italize normal font if italic variant is not present.
    /*
    if ((face->style_flags & FT_STYLE_FLAG_ITALIC) == 0 &&
      (caps & Font::Attribute_Italic) != 0)
    {
      FT_Matrix shear;

      Width += (int)ceil(m->GlyphItalics);

      shear.xx = 1 << 16;
      shear.xy = (int)(m->GlyphItalics * ( 1 << 16 ) ) / m->Height;
      shear.yx = 0;
      shear.yy = 1 << 16;

      FT_Outline_Transform(&slot->outline, &shear);
    }
    */

    if ((error = _freeTypeLib->pFT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL)) != 0)
    {
      goto end;
    }

    width += slot->bitmap.width;

    glyphd = new(std::nothrow) GlyphData();
    if (glyphd == NULL) goto end;

    if (width != 0 && glyphd->bitmap.create(width, slot->bitmap.rows, PIXEL_FORMAT_A8) != ERR_OK)
    {
      delete glyphd;
      glyphd = NULL;
      goto end;
    }

    glyphd->offset.set(offsetX, offsetY);
    glyphd->beginWidth = 0;
    glyphd->endWidth = 0;
    glyphd->advance = advance;
  }

  // If there is not glyph (spaces) just ignore copying step.
  if (!glyphd->bitmap.isEmpty())
  {
    // copy FT_Bitmap to our image and clean bytes over width
    sysuint_t p, pCount = glyphd->bitmap.getStride() - slot->bitmap.width;

    uint8_t *dstPtr = glyphd->bitmap._d->first;
    const uint8_t *srcPtr = slot->bitmap.buffer;

    for (y = 0; y != (uint)slot->bitmap.rows; y++)
    {
      if (slot->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
      {
        // 1 bit mono fonts are always rendered as 1 bit images, so
        // it's needed to convert these manually.
        uint x;
        uint pix8;

        uint8_t* dstCur = dstPtr;
        const uint8_t* srcCur = srcPtr;

        for (x = 0; x != (uint)slot->bitmap.width; x++)
        {
          if ((x & 7) == 0) pix8 = *srcCur++;
          *dstCur++ = (pix8 & 0x80) ? 0xFF : 0x00;
          pix8 <<= 1;
        }
      }
      else
      {
        memcpy(dstPtr, srcPtr, slot->bitmap.width);
      }

      dstPtr += slot->bitmap.width;
      srcPtr += slot->bitmap.pitch;

      p = pCount;
      while (p--) *dstPtr++ = 0;
    }
  }

  /* Handle the bold style ? */
  /*
  if (Param->Weight > WPP_Text_Weight_Normal && g.Width())
  {
    // For conversion between floating point is better signed number
    int Offset;
    uint8_t *Data;

    // The pixmap is a little hard, we have to add and clamp
    for (Y = 0; Y != g.Height(); Y++)
    {
      Data = (uint8_t *)g.Data()->Pixels + Y * g.Stride();
      for (Offset = 1; Offset <= m->GlyphOverhang; Offset++)
      {
        for (X = g.Width() - 1; X != 0; X--)
        {
          uint Pixel = uint(Data[X]) + uint(Data[X-1]);
          if (Pixel > 0xFF) Pixel = 0xFF;
          Data[X] = uint8_t(Pixel);
        }
      }
    }
  }
  */

#if 0
  // debug
  {
    fprintf(stderr, "GLYPH: '%lc' (width=%u, height=%u, stride=%ld, advance=%d, offx=%d, offy=%d)\n",
      ch,
      imaged->_width,
      imaged->_height,
      imaged->_stride,
      glyphd->_advance,
      glyphd->_offsetX,
      glyphd->_offsetY);

    for (y = 0; y != imaged->_height; y++)
    {
      for (x = 0; x != imaged->_width; x++)
      {
        fprintf(stderr, "%.2X ", imaged->_base[y * imaged->_stride + x]);
      }
      fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
  }
#endif

end:
  file->unuse();
  return glyphd;
}

// ============================================================================
// [Fog::FTFile]
// ============================================================================

// FIXME: Multithreading ? Locking ? Why this is not locked?
FTFontFile::FTFontFile(const String& fileName, const String& family) :
  fileName(fileName),
  family(family),
  currentFace(NULL),
  loadError(0),
  unloadAfterUse(false),
  fixedWidthIndex(0)
{
  //fog_debug("FTFile::new()");
  refCount.init(0);
  used.init(0);

  load();
}

FTFontFile::~FTFontFile()
{
  //fog_debug("FTFile::delete()");
  FOG_ASSERT((refCount.get() == 0 && used.get() == 0));
  unload();
}

FTFontFile* FTFontFile::ref()
{
  //fog_debug("FTFile::ref() - RefCount=%lld", (long long)refCount.get());
  refCount.inc();;
  return this;
}

void FTFontFile::deref()
{
  //fog_debug("FTFile::deref() - RefCount=%lld", (long long)refCount.get());
  refCount.dec();
}

// use() and unuse() are public methods that's used by engine.
bool FTFontFile::use()
{
  //fog_debug("FTFile::use()");
  used.inc();
  if (used.get() == 1 && !isLoaded() && !load())
  {
    used.dec();
    return false;
  }
  else
  {
    return true;
  }
}

void FTFontFile::unuse()
{
  //fog_debug("FTFile::unuse()");
  if (used.deref() && unloadAfterUse) unload();
}

bool FTFontFile::load()
{
  if (currentFace) return true;

  // Try to mmap file, if this fail, use FT_New_Face function that will open
  // it manually.
  if (mapFile.map(fileName, false) == ERR_OK)
  {
    FT_Open_Args ftArgs;

    memset(&ftArgs, 0, sizeof(FT_Open_Args));
    ftArgs.memory_base = (const FT_Byte*)mapFile.getData();
    ftArgs.memory_size = mapFile.getLength();
    ftArgs.flags = FT_OPEN_MEMORY;

    loadError = _freeTypeLib->pFT_Open_Face(_freeTypeLib->ftLibrary, &ftArgs, 0, &currentFace);
  }
  else
  {
    TemporaryByteArray<TEMP_LENGTH> fileName8;
    TextCodec::local8().appendFromUnicode(fileName8, fileName);

    loadError = _freeTypeLib->pFT_New_Face(_freeTypeLib->ftLibrary, fileName8.getData(), 0, &currentFace);
  }

  return (loadError == 0);
}

void FTFontFile::unload()
{
  FOG_ASSERT(used.get() == 0);

  if (currentFace)
  {
    _freeTypeLib->pFT_Done_Face(currentFace);
    currentFace = NULL;
  }
}

FOG_INLINE bool FTFontFile::isLoaded() const
{
  return currentFace != NULL;
}

bool FTFontFile::setupSize(uint32_t size)
{
  FT_Error error = 0;
  FOG_ASSERT(isLoaded());

  // Make sure that font face is scalable.
  if (FT_IS_SCALABLE(currentFace))
  {
    // Set the character size and use default DPI (72)
    error = _freeTypeLib->pFT_Set_Char_Size(currentFace, 0, size * 64, 0, 0);
  }
  else
  {
    uint i;
    uint fs = currentFace->num_fixed_sizes;

    fixedWidthIndex = 0;

    for (i = 0; i != fs; i++)
    {
      uint32_t s = (currentFace->available_sizes[i].size + 31) >> 6;

      if (s == size)
      {
        error = _freeTypeLib->pFT_Set_Pixel_Sizes(
          currentFace,
          currentFace->available_sizes[i].width,
          currentFace->available_sizes[i].height);
        fixedWidthIndex = i;
        goto done;
      }
    }

    return false;
  }
done:
  return error == 0;
}

FontFace* FTFontFile::createFace(uint32_t size, const FontCaps& caps)
{
  FTFontFace* face = NULL;

  use();

  if (setupSize(size))
  {
    // freetype not returned an error, so create new face and fill it
    face = new(std::nothrow) FTFontFace();
    if (!face) goto end;

    face->file = this->ref();
    face->family = family;
    face->metrics.size = size;
    face->caps = caps;

    if (FT_IS_SCALABLE(currentFace))
    {
      // Get the scalable font metrics for this font
      FT_Fixed scale = currentFace->size->metrics.y_scale;

      face->metrics.height = FT_CEIL(_freeTypeLib->pFT_MulFix(currentFace->bbox.yMax - currentFace->bbox.yMin, scale)) - 1;
      face->metrics.ascent = (FT_CEIL(_freeTypeLib->pFT_MulFix(currentFace->bbox.yMax, scale)));
      face->metrics.descent = face->metrics.height - face->metrics.ascent;

      /*
      face->metrics.lineSkip = FT_CEIL(_freetypeLib->pFT_MulFix(face->height, scale));
      face->metrics.underlineOffset = FT_FLOOR(_freetypeLib->pFT_MulFix(face->underline_position, scale));
      face->metrics.underlineHeight = FT_FLOOR(_freetypeLib->pFT_MulFix(face->underline_thickness, scale));
      if (face->metrics.underlineHeight < 1) face->metrics.underlineHeight = 1;

      face->metrics.glyphOverhang = face->size->metrics.y_ppem / 10;
      face->metrics.glyphItalics = 0.207f * float(face->metrics.height);
      */
    }
    else
    {
      // This is bitmapped font. I don't know why freetype2 library
      // handles differently different font files, but this workaround
      // should be smart.

      face->metrics.height = currentFace->available_sizes[fixedWidthIndex].height;
      face->metrics.ascent = FT_FLOOR(currentFace->available_sizes[fixedWidthIndex].y_ppem);
      if (face->metrics.ascent == face->metrics.height)
        face->metrics.ascent = FT_FLOOR(currentFace->size->metrics.ascender);
      face->metrics.descent = face->metrics.height - face->metrics.ascent;

      /*
      face->metrics.lineSkip = FT_CEIL(face->metrics.ascent);
      face->metrics.underlineOffset = FT_FLOOR(face->underline_position);
      face->metrics.underlineHeight = FT_FLOOR(face->underline_thickness);
      face->metrics.underlineHeight < 1) face->metrics.underlineHeight = 1;

      face->metrics.glyphOverhang = face->size->metrics.y_ppem / 10;
      face->metrics.glyphItalics = 0.207f * (float)face->metrics.height;
      */
    }

    //Fog_FontFaceCache_put(face);
  }

end:
  unuse();
  return face;
}

// ---------------------------------------------------------------------------
// freetype font list
// ---------------------------------------------------------------------------
/*
static void Fog_Font_ftAppendFontsToList(List<String>* list)
{
  if (Fog_FontConfig)
  {
    list->append(Fog_FontConfig->fontsList());
  }
  else
  {
    // fontconfig is not present are we should to locate fonts manually.
    // this is not preferred case and this is only small workaround.

    // Make common fonts list, but check if they are available
    UnorderedHash<String, String>::ConstIterator iterator(Fog_Ft->ftTranslator);
    String path;

    for (iterator.begin(); iterator.exist(); iterator.next())
    {
      if (Font::findFontFile(iterator.value(), path))
      {
        list->append(iterator.key());
      }
    }
  }
}
*/

} // Fog namespace

#endif // FOG_FONT_FREETYPE
