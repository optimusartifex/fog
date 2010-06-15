// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_IMAGEDATA_WINDIB_P_H
#define _FOG_GRAPHICS_IMAGEDATA_WINDIB_P_H

#include <Fog/Core/Build.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Graphics/Image.h>

namespace Fog {

//! @addtogroup Fog_Graphics_Private
//! @{

// ============================================================================
// [Fog::WinDibImageData]
// ============================================================================

//! @internal
struct FOG_HIDDEN WinDibImageData : public ImageData
{
  WinDibImageData();
  virtual ~WinDibImageData();

  virtual ImageData* clone() const;

  virtual err_t create(int w, int h, uint32_t format);
  virtual err_t destroy();

  virtual void* getHandle();

  HDC getDC();
  void releaseDC(HDC hDC);

  //! @brief The bitmap handle.
  HBITMAP hBitmap;

  static ImageData* _createInstance();
  static HBITMAP _createDibSection(int w, int h, uint32_t format, 
    uint8_t** bits, sysint_t* stride);
};

//! @}

} // Fog namespace

// [Guard]
#endif // FOG_OS_WINDOWS
#endif // _FOG_GRAPHICS_IMAGEDATA_WINDIB_P_H
