// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_COLORLUT_H
#define _FOG_GRAPHICS_COLORLUT_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/Constants.h>
#include <Fog/Graphics/Constants.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Argb;

// ============================================================================
// [Fog::ColorLutData]
// ============================================================================

//! @brief Color lut data.
struct ColorLutData
{
  //! @brief Lookup table for red component.
  uint8_t r[256];
  //! @brief Lookup table for green component.
  uint8_t g[256];
  //! @brief Lookup table for blue component.
  uint8_t b[256];
  //! @brief Lookup table for alpha component.
  uint8_t a[256];
};

// ============================================================================
// [Fog::ColorLutFilter]
// ============================================================================

//! @brief Class that can be extended to make custom color lut filter. See
//! ColorLut::filter() method.
struct FOG_API ColorLutFilter
{
  ColorLutFilter();
  virtual ~ColorLutFilter();

  virtual err_t filter(uint8_t* data) const = 0;

private:
  FOG_DISABLE_COPY(ColorLutFilter)
};

// ============================================================================
// [Fog::ColorLut]
// ============================================================================

//! @brief Color lut.
//!
//! Color lut is class that stores information about color channel
//! transformations that can be applied to an image data.
struct FOG_API ColorLut
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ColorLut();
  ColorLut(const ColorLut& other);
  FOG_INLINE ColorLut(_DONT_INITIALIZE _linkerInitialized) { FOG_UNUSED(_linkerInitialized); }
  ~ColorLut();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorLutData* getData() { return &_data; }
  FOG_INLINE const ColorLutData* getData() const { return &_data; }

  err_t setData(const ColorLutData* data);

  // --------------------------------------------------------------------------
  // [Filters]
  // --------------------------------------------------------------------------

  err_t reset(int channel);
  err_t saturate(int channel, int minThreshold, int maxThreshold);
  err_t multiply(int channel, double by);
  err_t add(int channel, int value);
  err_t invert(int channel);
  err_t filter(int channel, const ColorLutFilter& filter);

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  ColorLut& operator=(const ColorLut& other);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  ColorLutData _data;
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_COLORLUT_H
