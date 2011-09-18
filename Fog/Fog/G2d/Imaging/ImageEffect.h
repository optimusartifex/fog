// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEEFFECT_H
#define _FOG_G2D_IMAGING_IMAGEEFFECT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ImageEffect;
struct ImageEffectData;

// ============================================================================
// [Function Prototypes]
// ============================================================================

typedef void (FOG_FASTCALL *ImageEffectDestroyFunc)(ImageEffectData* d);

// ============================================================================
// [Fog::ImageEffectData]
// ============================================================================

//! @brief The image effect data.
struct FOG_NO_EXPORT ImageEffectData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageEffectData* addRef() const
  {
    reference.inc();
    return const_cast<ImageEffectData*>(this);
  }

  FOG_INLINE void deref()
  {
    if (reference.deref())
      destroy(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Destructor function.
  ImageEffectDestroyFunc destroy;

  //! @brief Effect type.
  uint32_t type;
};

// ============================================================================
// [Fog::ImageEffect]
// ============================================================================

//! @brief The image effect.
struct FOG_API ImageEffect
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ImageEffect();
  ImageEffect(const ImageEffect& other);
  ~ImageEffect();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE uint32_t getType() const { return _d->type; }

  // --------------------------------------------------------------------------
  // [Effect]
  // --------------------------------------------------------------------------

  err_t _getEffect(uint32_t type, void* effect) const;
  err_t _setEffect(uint32_t type, const void* effect);

  err_t setEffect(const ImageEffect& effect);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageEffect& operator=(const ImageEffect& effect)
  {
    setEffect(effect);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Static<ImageEffectData> _dnull;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ImageEffectData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPE_DECLARE(Fog::ImageEffect, Fog::TYPE_CATEGORY_MOVABLE)

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEEFFECT_H
