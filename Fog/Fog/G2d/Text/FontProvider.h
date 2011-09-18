// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_FONTPROVIDER_H
#define _FOG_G2D_TEXT_FONTPROVIDER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Text/Font.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::FontProviderData]
// ============================================================================

struct FOG_API FontProviderData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FontProviderData();
  virtual ~FontProviderData();

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE FontProviderData* addRef() const
  {
    reference.inc();
    return const_cast<FontProviderData*>(this);
  }

  FOG_INLINE void deref()
  {
    if (reference.deref()) fog_delete(this);
  }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  virtual err_t getFontFace(FontFace** dst, const StringW& fontFamily) = 0;
  virtual err_t getFontList(List<StringW>& dst) = 0;

  virtual StringW getDefaultFamily() = 0;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;
  //! @brief Lock for thread safety.
  Lock lock;

  //! @brief Name of this provider.
  StringW name;
  //! @brief Id of this provider.
  uint32_t id;

private:
  _FOG_NO_COPY(FontProviderData)
};

// ============================================================================
// [Fog::FontProvider]
// ============================================================================

//! @brief Font provider.
struct FOG_API FontProvider
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FontProvider();
  FontProvider(const FontProvider& other);
  ~FontProvider();

  explicit FOG_INLINE FontProvider(FontProviderData* d) : _d(d) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  StringW getName() const;
  uint32_t getId() const;

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  err_t getFontFace(FontFace** dst, const StringW& fontFamily) const;
  err_t getFontList(List<StringW>& dst) const;

  StringW getDefaultFamily() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  const FontProvider& operator=(const FontProvider& other);

  FOG_INLINE bool operator==(const FontProvider& other) const { return _d == other._d; }
  FOG_INLINE bool operator!=(const FontProvider& other) const { return _d != other._d; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(FontProviderData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_FONTPROVIDER_H
