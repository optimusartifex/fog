// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_LAYOUTHINT_H
#define _FOG_GUI_LAYOUT_LAYOUTHINT_H

// [Dependencies]
#include <Fog/Graphics/Geometry.h>

//! @addtogroup Fog_Gui
//! @{

namespace Fog {

// ============================================================================
// [Fog::LayoutHint]
// ============================================================================

struct FOG_HIDDEN LayoutHint
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

  FOG_INLINE LayoutHint(const IntSize& sizeHint, const IntSize& minimumSize, const IntSize& maximumSize) :
    _sizeHint(sizeHint),
    _minimumSize(minimumSize),
    _maximumSize(maximumSize)
  {
  }

  FOG_INLINE LayoutHint(_DONT_INITIALIZE dontInitialize)
  {
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE IntSize getSizeHint() const { return _sizeHint; }
  FOG_INLINE IntSize getMinimumSize() const { return _minimumSize; }
  FOG_INLINE IntSize getMaximumSize() const { return _maximumSize; }

  FOG_INLINE void setSizeHint(const IntSize& sizeHint) { _sizeHint = sizeHint; }
  FOG_INLINE void setMinimumSize(const IntSize& minimumSize) { _minimumSize = minimumSize; }
  FOG_INLINE void setMaximumSize(const IntSize& maximumSize) { _maximumSize = maximumSize; }

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

  IntSize _sizeHint;
  IntSize _minimumSize;
  IntSize _maximumSize;
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GUI_LAYOUT_LAYOUTHINT_H
