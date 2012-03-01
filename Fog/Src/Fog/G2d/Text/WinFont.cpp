// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Text/WinFont.h>

namespace Fog {

// ============================================================================
// [Fog::WinAbstractEnumContext]
// ============================================================================

template<typename T>
struct WinAbstractEnumContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE WinAbstractEnumContext()
  {
    hdc = CreateCompatibleDC(NULL);
    if (hdc != NULL) SetGraphicsMode(hdc, GM_ADVANCED);
  }

  FOG_INLINE ~WinAbstractEnumContext()
  {
    if (hdc != NULL) DeleteDC(hdc);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isValid() const
  {
    return hdc != NULL;
  }

  FOG_INLINE int enumFontFamilies(LOGFONTW* lf)
  {
    return ::EnumFontFamiliesExW(hdc, lf, (FONTENUMPROCW)_OnEnumProc, (LPARAM)this, 0);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static int CALLBACK _OnEnumProc(const LOGFONTW* plf, const TEXTMETRICW* ptm, DWORD fontType, LPARAM lParam)
  {
    T* ctx = (T*)lParam;
    return ctx->onEntry(plf, ptm, fontType);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  HDC hdc;

private:
  _FOG_NO_COPY(WinAbstractEnumContext)
};

// ============================================================================
// [Fog::WinFontEnumContext]
// ============================================================================

struct FOG_NO_EXPORT WinFontEnumContext : public WinAbstractEnumContext<WinFontEnumContext>
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE WinFontEnumContext(List<StringW>& fonts) :
    fonts(fonts),
    err(ERR_OK)
  {
  }

  FOG_INLINE ~WinFontEnumContext()
  {
  }

  // --------------------------------------------------------------------------
  // [Events]
  // --------------------------------------------------------------------------

  FOG_INLINE int onEntry(const LOGFONTW* plf, const TEXTMETRICW* ptm, DWORD fontType)
  {
    const WCHAR* faceName = plf->lfFaceName;

    // Accept only true-type fonts.
    if ((fontType & TRUETYPE_FONTTYPE) == 0)
      return 1;

    // Reject '@'.
    if (faceName[0] == L'@')
      return 1;

    // Windows sends more fonts that are needed, but equal fonts
    // are usually sent together, so we will simply copy this
    // font to buffer and compare it with previous. If this will
    // match - reject it immediately to save CPU cycles.
    curFaceName.setWChar(faceName);

    if (curFaceName != lastFaceName)
    {
      if (fonts.contains(curFaceName))
      {
        swap(lastFaceName, curFaceName);
      }
      else
      {
        lastFaceName = curFaceName;
        lastFaceName.squeeze();
        fonts.append(lastFaceName);
      }
    }

    // Return 1 to continue listing.
    return 1;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  err_t err;

  List<StringW>& fonts;
  StringW curFaceName;
  StringW lastFaceName;

private:
  _FOG_NO_COPY(WinFontEnumContext)
};

// ============================================================================
// [Fog::WinFontInfoContext]
// ============================================================================

struct FOG_NO_EXPORT WinFontInfoContext : public WinAbstractEnumContext<WinFontInfoContext>
{
  FOG_INLINE WinFontInfoContext() :
    acceptable(0),
    designEmSquare(0)
  {
  }

  // --------------------------------------------------------------------------
  // [Events]
  // --------------------------------------------------------------------------

  FOG_INLINE int onEntry(const LOGFONTW* plf, const TEXTMETRICW* ptm, DWORD fontType)
  {
    // Accept only true-type fonts.
    if ((fontType & TRUETYPE_FONTTYPE) == 0) return 1;

    const NEWTEXTMETRICEXW* pntm = reinterpret_cast<const NEWTEXTMETRICEXW*>(ptm);
    acceptable = true;
    designEmSquare = pntm->ntmTm.ntmCellHeight;
    MemOps::copy(&logFont, plf, sizeof(LOGFONTW));

    return 0;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t acceptable;
  uint32_t designEmSquare;
  LOGFONTW logFont;
};








































// ============================================================================
// [Fog::WinFace]
// ============================================================================

static FaceVTable WinFace_vtable;

static void FOG_CDECL WinFace_create(WinFace* self)
{
  fog_new_p(self) WinFace(&WinFace_vtable, StringW::getEmptyInstance());
}

static void FOG_CDECL WinFace_destroy(Face* self)
{
  reinterpret_cast<WinFace*>(self)->~WinFace();
}

static err_t FOG_CDECL WinFace_getOutlineFromGlyphRunF(const Face* self,
  PathF* dst, uint32_t cntOp,
  const uint32_t* glyphList, size_t itemAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  // TODO:
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL WinFace_getOutlineFromGlyphRunD(const Face* self,
  PathD* dst, uint32_t cntOp,
  const uint32_t* glyphList, size_t glyphAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  // TODO:
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::WinFontEngine]
// ============================================================================

static Static<WinFontEngine> WinFontEngine_oInstance;
static FontEngineVTable WinFontEngine_vtable;

static void WinFontEngine_create(WinFontEngine* self)
{
  fog_new_p(self) WinFontEngine(&WinFontEngine_vtable);
  // self->defaultFont->_d = Font_oWin->_d;
}

static void WinFontEngine_destroy(FontEngine* self)
{
  static_cast<WinFontEngine*>(self)->~WinFontEngine();
  FOG_UNUSED(self);
}

static err_t FOG_CDECL WinFontEngine_queryFace(const FontEngine* self,
  Face** dst, const StringW* family, const FaceFeatures* features)
{
  FOG_UNUSED(self);
  FOG_UNUSED(dst);
  FOG_UNUSED(family);
  FOG_UNUSED(features);

  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL WinFontEngine_getAvailableFaces(const FontEngine* self,
  FaceCollection* dst)
{
  FOG_UNUSED(self);

  dst->clear();
  return ERR_OK;
}

static err_t FOG_CDECL WinFontEngine_getDefaultFace(const FontEngine* self,
  FaceInfo* dst)
{
  
  return ERR_OK;
}

static err_t WinFontEngine_getLogFontW(LOGFONTW* lfDst, const StringW* family)
{
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Font_init_win(void)
{
  // --------------------------------------------------------------------------
  // [WinFace / WinFontEngine]
  // --------------------------------------------------------------------------

  WinFace_vtable.destroy = WinFace_destroy;
  WinFace_vtable.getOutlineFromGlyphRunF = WinFace_getOutlineFromGlyphRunF;
  WinFace_vtable.getOutlineFromGlyphRunD = WinFace_getOutlineFromGlyphRunD;

  WinFontEngine_vtable.destroy = WinFontEngine_destroy;
  WinFontEngine_vtable.getAvailableFaces = WinFontEngine_getAvailableFaces;
  WinFontEngine_vtable.getDefaultFace = WinFontEngine_getDefaultFace;
  WinFontEngine_vtable.queryFace = WinFontEngine_queryFace;
  WinFontEngine_create(&WinFontEngine_oInstance);

  fog_api.fontengine_oGlobal = &WinFontEngine_oInstance;
}

} // Fog namespace
