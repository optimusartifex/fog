// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_UIENGINEDISPLAYINFO_H
#define _FOG_UI_ENGINE_UIENGINEDISPLAYINFO_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Size.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::UIEngineDisplayInfo]
// ============================================================================

//! @brief Frame-buffer display info.
struct FOG_NO_EXPORT UIEngineDisplayInfo
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineDisplayInfo()
  {
    reset();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const SizeI& getSize() const { return _size; }
  FOG_INLINE uint32_t getDepth() const { return _depth; }

  FOG_INLINE uint32_t getRMask32() const { return (uint32_t)_rMask; }
  FOG_INLINE uint32_t getGMask32() const { return (uint32_t)_gMask; }
  FOG_INLINE uint32_t getBMask32() const { return (uint32_t)_bMask; }
  FOG_INLINE uint32_t getAMask32() const { return (uint32_t)_aMask; }

  FOG_INLINE uint64_t getRMask64() const { return (uint64_t)_rMask; }
  FOG_INLINE uint64_t getGMask64() const { return (uint64_t)_gMask; }
  FOG_INLINE uint64_t getBMask64() const { return (uint64_t)_bMask; }
  FOG_INLINE uint64_t getAMask64() const { return (uint64_t)_aMask; }

  FOG_INLINE uint32_t is16BppSwapped() const { return _is16BppSwapped; }

  FOG_INLINE void setSize(const SizeI& size) { _size = size; }
  FOG_INLINE void setDepth(uint32_t depth) { _depth = depth; }

  FOG_INLINE void setAMask32(uint32_t aMask32) { _aMask = (uint64_t)aMask32; }
  FOG_INLINE void setRMask32(uint32_t rMask32) { _rMask = (uint64_t)rMask32; }
  FOG_INLINE void setGMask32(uint32_t gMask32) { _gMask = (uint64_t)gMask32; }
  FOG_INLINE void setBMask32(uint32_t bMask32) { _bMask = (uint64_t)bMask32; }

  FOG_INLINE void setAMask64(uint64_t aMask64) { _aMask = (uint64_t)aMask64; }
  FOG_INLINE void setRMask64(uint64_t rMask64) { _rMask = (uint64_t)rMask64; }
  FOG_INLINE void setGMask64(uint64_t gMask64) { _gMask = (uint64_t)gMask64; }
  FOG_INLINE void setBMask64(uint64_t bMask64) { _bMask = (uint64_t)bMask64; }
  
  FOG_INLINE void setIs16BppSwapped(uint32_t val) { _is16BppSwapped = val; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _size.reset();

    _aMask = FOG_UINT64_C(0);
    _rMask = FOG_UINT64_C(0);
    _gMask = FOG_UINT64_C(0);
    _bMask = FOG_UINT64_C(0);

    _depth = 0;
    _is16BppSwapped = 0;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const UIEngineDisplayInfo& other) const
  {
    return MemOps::eq(this, &other, sizeof(UIEngineDisplayInfo));
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineDisplayInfo& operator=(const UIEngineDisplayInfo& other)
  {
    MemOps::copy(this, &other, sizeof(UIEngineDisplayInfo));
    return *this;
  }

  FOG_INLINE bool operator==(const UIEngineDisplayInfo& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const UIEngineDisplayInfo& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Screen width.
  SizeI _size;

  //! @brief Screen alpha mask.
  uint64_t _aMask;
  //! @brief Screen red mask.
  uint64_t _rMask;
  //! @brief Screen green mask.
  uint64_t _gMask;
  //! @brief Screen blue mask.
  uint64_t _bMask;

  //! @brief Screen depth.
  uint32_t _depth;

  //! @brief If true, 16 bit depth is byte-swapped (X Server). In other bit 
  //! depths it's likely to byte-swap instead image component masks instead
  //! of the whole image.
  uint32_t _is16BppSwapped;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_UIENGINEDISPLAYINFO_H
