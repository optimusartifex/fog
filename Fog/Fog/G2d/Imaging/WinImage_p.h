// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_WINIMAGE_P_H
#define _FOG_G2D_IMAGING_WINIMAGE_P_H

#include <Fog/Core/Config/Config.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/G2d/Imaging/Image.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::WinDibImageData]
// ============================================================================

//! @internal
struct FOG_API WinDibImageData : public ImageData
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinDibImageData(const SizeI& size, uint32_t format, uint8_t* bits, sysint_t stride, HBITMAP hBitmap);
  virtual ~WinDibImageData();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual ImageData* clone() const;
  virtual void* getHandle();

  virtual void paletteModified(const Range& range);

  // --------------------------------------------------------------------------
  // [Windows Specific]
  // --------------------------------------------------------------------------

  HDC getDC();
  void releaseDC(HDC hDC);

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static err_t _createDibSection(const SizeI& size, uint32_t format, HBITMAP* dst, uint8_t** bits, sysint_t* stride);
  static err_t _createDibImage(const SizeI& size, uint32_t format, ImageData** dst);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The bitmap handle.
  HBITMAP hBitmap;
};

//! @}

} // Fog namespace

// [Guard]
#endif // FOG_OS_WINDOWS
#endif // _FOG_G2D_IMAGING_WINIMAGE_P_H
