// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_FONTMANAGER_H
#define _FOG_G2D_TEXT_FONTMANAGER_H

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Text/Font.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::FontManagerData]
// ============================================================================

struct FOG_API FontManagerData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FontManagerData();
  FontManagerData(const FontManagerData& other);
  ~FontManagerData();

  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  FOG_INLINE FontManagerData* ref() const
  {
    refCount.inc();
    return const_cast<FontManagerData*>(this);
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref()) fog_delete(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> refCount;

  //! @brief List of font providers associated with this font-manager data.
  List<FontProvider> providers;

  //! @brief Cached list of font-faces get from all providers.
  List<String> fontListCache;
  //! @brief Whether the fontListCache is dirty (needs update).
  bool fontListDirty;

  //! @brief Default font data (including font-face).
  Font defaultFont;
};

// ============================================================================
// [Fog::FontManager]
// ============================================================================

//! @brief Font manager.
//!
//! Font manager is class that can be used to customize font management at
//! global or local level. The global font-manager can be retrieved using
//! @c FontManager::getGlobal(), and then custom settings can be applied.
//! It is possible to create a weak/deep copy of @c FontManager to create
//! customized font-manager at local level.
//!
//! Local font-manager can be used to add new fonts which will be available
//! only to consumers of that manager, this includes using custom CSS/SVG
//! fonts or to customize the missing-glyph outline which can be set to all
//! fonts created by the local font-manager.
struct FOG_API FontManager
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FontManager();
  FontManager(const FontManager& other);
  ~FontManager();

  explicit FOG_INLINE FontManager(FontManagerData* d) : _d(d) {}

  // --------------------------------------------------------------------------
  // [Provider Management]
  // --------------------------------------------------------------------------

  err_t addProvider(const FontProvider& provider, uint32_t order);
  err_t removeProvider(const FontProvider& provider);

  bool hasProvider(const FontProvider& provider) const;
  bool hasProvider(const String& name) const;

  // --------------------------------------------------------------------------
  // [Fonts]
  // --------------------------------------------------------------------------

  FontFace* getFontFace(const String& fontFamily) const;
  List<String> getFontList() const;

  void _initDefaultFont();

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  const FontManager& operator=(const FontManager& other);

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FontManager* _global;
  static FOG_INLINE FontManager& getGlobal() { return *_global; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(FontManagerData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::FontManager, Fog::TYPEINFO_MOVABLE)

// ============================================================================
// [Fog::Swap]
// ============================================================================

_FOG_SWAP_D(Fog::FontManager)

// [Guard]
#endif // _FOG_G2D_TEXT_FONTMANAGER_H
