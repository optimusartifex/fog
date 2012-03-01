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

// [Fix]
#ifndef GGO_UNHINTED
# define GGO_UNHINTED 0x0100
#endif // GGO_UNHINTED

#ifndef CLEARTYPE_QUALITY
# define CLEARTYPE_QUALITY 5
#endif // CLEARTYPE_QUALITY

#ifndef TT_PRIM_CSPLINE
# define TT_PRIM_CSPLINE 3
#endif // TT_PRIM_CSPLINE

namespace Fog {

// ============================================================================
// [Fog::WinHDC]
// ============================================================================

struct FOG_NO_EXPORT WinHDC
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE WinHDC()
  {
    hdc = NULL;
  }

  FOG_INLINE ~WinHDC()
  {
    if (hdc != NULL)
      ::DeleteDC(hdc);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isInitialized() const
  {
    return hdc != NULL;
  }

  FOG_INLINE bool init()
  {
    FOG_ASSERT(!isInitialized());
    hdc = ::CreateCompatibleDC(NULL);

    if (hdc == NULL)
      return false;

    ::SetGraphicsMode(hdc, GM_ADVANCED);
    return true;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  HDC hdc;

private:
  _FOG_NO_COPY(WinHDC)
};

// ============================================================================
// [Fog::WinFace]
// ============================================================================

static FaceVTable WinFace_vtable;

static void FOG_CDECL WinFace_freeTable(OT_Table* table)
{
  MemMgr::free(table->getData());
}

static void FOG_CDECL WinFace_create(WinFace* self)
{
  fog_new_p(self) WinFace(&WinFace_vtable, StringW::getEmptyInstance());

  self->ot->_freeTable = WinFace_freeTable;
}

static void FOG_CDECL WinFace_destroy(Face* self_)
{
  WinFace* self = static_cast<WinFace*>(self_);

  self->~WinFace();
  MemMgr::free(self);
}

static OT_Table* FOG_CDECL WinFace_getTable(const Face* self_, uint32_t tag)
{
  const WinFace* self = static_cast<const WinFace*>(self_);

  OT_Table* table = self->ot->getTable(tag);
  if (table != NULL)
    return table;

  WinHDC scopedDC;
  if (!scopedDC.init())
    return NULL;

  HGDIOBJ oldFont = ::SelectObject(scopedDC.hdc, (HGDIOBJ)self->hFont);
  if (oldFont == (HGDIOBJ)GDI_ERROR)
    return NULL;

  uint8_t* mem;
  DWORD length = ::GetFontData(scopedDC.hdc, MemOps::bswap32be(tag), 0, NULL, 0);

  if (length == GDI_ERROR)
    goto _End;

  mem = static_cast<uint8_t*>(MemMgr::alloc(length));
  if (FOG_IS_NULL(mem))
    goto _End;

  if (::GetFontData(scopedDC.hdc, MemOps::bswap32be(tag), 0, mem, length) != length)
  {
    MemMgr::free(mem);
    goto _End;
  }

  table = const_cast<WinFace*>(self)->ot->addTable(tag, mem, length);
  if (FOG_IS_NULL(table))
  {
    MemMgr::free(mem);
    goto _End;
  }

_End:
  ::SelectObject(scopedDC.hdc, oldFont);
  return table;
}

static void FOG_CDECL WinFace_releaseTable(const Face* self_, OT_Table* table)
{
  // WinFace caches all tables, there is nothing to do.
  FOG_UNUSED(self_);
  FOG_UNUSED(table);
}

static err_t FOG_CDECL WinFace_getOutlineFromGlyphRunF(const Face* self_,
  PathF* dst, uint32_t cntOp,
  const uint32_t* glyphList, size_t itemAdvance,
  const PointF* positionList, size_t positionAdvance,
  size_t length)
{
  // TODO:
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL WinFace_getOutlineFromGlyphRunD(const Face* self_,
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

static err_t FOG_CDECL WinFontEngine_queryFace(const FontEngine* self_,
  Face** dst, const StringW* family, const FaceFeatures* features)
{
  const WinFontEngine* self = static_cast<const WinFontEngine*>(self_);
  AutoLock locked(self->lock());

  Face* face = self->cache->getExactFace(*family, *features);
  if (face != NULL)
  {
    *dst = face;
    return ERR_OK;
  }

  // TODO
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL WinFontEngine_getAvailableFaces(const FontEngine* self_,
  FaceCollection* dst)
{
  const WinFontEngine* self = static_cast<const WinFontEngine*>(self_);
  AutoLock locked(self->lock());

  return dst->setCollection(self->faceCollection());
}

static err_t FOG_CDECL WinFontEngine_getDefaultFace(const FontEngine* self_,
  FaceInfo* dst)
{
  const WinFontEngine* self = static_cast<const WinFontEngine*>(self_);
  AutoLock locked(self->lock());

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t WinFontEngine_getLogFontW(LOGFONTW* lfDst, const StringW* family)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
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
  WinFace_vtable.getTable = WinFace_getTable;
  WinFace_vtable.releaseTable = WinFace_releaseTable;
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
