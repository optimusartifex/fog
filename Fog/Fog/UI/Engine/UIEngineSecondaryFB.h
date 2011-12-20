// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_UIENGINESECONDARYFB_H
#define _FOG_UI_ENGINE_UIENGINESECONDARYFB_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Imaging/ImageConverter.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::UIEngineSecondaryFB]
// ============================================================================

//! @brief Secondary double-buffer.
//!
//! Secondary double-buffer is used by @ref UIEngineWindow in case that it's unable 
//! to allocate single double-buffer which can be used for painting and for 
//! blitting into the screen.
struct FOG_NO_EXPORT UIEngineSecondaryFB
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE UIEngineSecondaryFB()
  {
    reset();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t* getData() const { return _data; }
  FOG_INLINE uint32_t getDepth() const { return _depth; }
  FOG_INLINE ssize_t getStride() const { return _stride; }
  FOG_INLINE ImageConverterBlitLineFunc getConvertFunc() const { return _convertFunc; }

  FOG_INLINE void setData(uint8_t* data) { _data = data; }
  FOG_INLINE void setDepth(uint32_t depth) { _depth = depth; }
  FOG_INLINE void setStride(size_t stride) { _stride = stride; }
  FOG_INLINE void setConvertFunc(ImageConverterBlitLineFunc func) { _convertFunc = func; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    MemOps::zero(this, sizeof(UIEngineSecondaryFB));
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Secondary pixel array (native pixel format of target engine).
  uint8_t* _data;
  //! @brief Secondary pixel stride.
  ssize_t _stride;

  //! @brief Converter used to convert pixels primery to secondary buffer.
  ImageConverterBlitLineFunc _convertFunc;
  //! @brief Secondary pixel depth.
  uint32_t _depth;

#if FOG_ARCH_BITS >= 64
  uint32_t _padding;
#endif // FOG_ARCH_BITS >= 64
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_UIENGINESECONDARYFB_H
