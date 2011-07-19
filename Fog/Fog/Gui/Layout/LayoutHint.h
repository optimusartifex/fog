// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_LAYOUTHINT_H
#define _FOG_GUI_LAYOUT_LAYOUTHINT_H

// [Dependencies]
#include <Fog/G2d/Geometry/Size.h>

namespace Fog {

//! @addtogroup Fog_Gui_Layout
//! @{

// ============================================================================
// [Fog::LayoutHint]
// ============================================================================

struct FOG_NO_EXPORT LayoutHint
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Construct empty layout hint, members are initialized to safe defaults.
  FOG_INLINE LayoutHint() :
    _sizeHint(0, 0),
    _minimumSize(INT_MIN, INT_MIN),
    _maximumSize(INT_MAX, INT_MAX)
  {
  }

  FOG_INLINE LayoutHint(const LayoutHint& other) :
    _sizeHint(other._sizeHint),
    _minimumSize(other._minimumSize),
    _maximumSize(other._maximumSize)
  {
  }

  FOG_INLINE LayoutHint(const SizeI& sizeHint, const SizeI& minimumSize, const SizeI& maximumSize) :
    _sizeHint(sizeHint),
    _minimumSize(minimumSize),
    _maximumSize(maximumSize)
  {
  }

  explicit FOG_INLINE LayoutHint(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE SizeI getSizeHint() const { return _sizeHint; }
  FOG_INLINE SizeI getMinimumSize() const { return _minimumSize; }
  FOG_INLINE SizeI getMaximumSize() const { return _maximumSize; }

  FOG_INLINE void setSizeHint(const SizeI& sizeHint) { _sizeHint = sizeHint; }
  FOG_INLINE void setMinimumSize(const SizeI& minimumSize) { _minimumSize = minimumSize; }
  FOG_INLINE void setMaximumSize(const SizeI& maximumSize) { _maximumSize = maximumSize; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE LayoutHint& operator=(const LayoutHint& other)
  {
    _sizeHint = other._sizeHint;
    _minimumSize = other._minimumSize;
    _maximumSize = other._maximumSize;

    return *this;
  }

  FOG_INLINE bool operator==(const LayoutHint& other) const
  {
    return (_sizeHint    == other._sizeHint   ) &&
           (_minimumSize == other._minimumSize) &&
           (_maximumSize == other._maximumSize) ;
  }

  FOG_INLINE bool operator!=(const LayoutHint& other) const
  {
    return !(*this == other);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  SizeI _sizeHint;
  SizeI _minimumSize;
  SizeI _maximumSize;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_LAYOUT_LAYOUTHINT_H
