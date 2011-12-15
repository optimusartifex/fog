// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_FBEVENT_H
#define _FOG_UI_ENGINE_FBEVENT_H

// [Dependencies]
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Time.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

//! @brief Frame-buffer base event.
struct FOG_NO_EXPORT FbEvent
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _type;
};

//! @brief Frame-buffer geometry event.
struct FOG_NO_EXPORT FbGeometryEvent
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RectI _windowGeometry;
  RectI _clientGeometry;
};

//! @brief Frame-buffer key event.
struct FOG_NO_EXPORT FbKeyEvent
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
};

//! @brief Frame-buffer mouse event.
struct FOG_NO_EXPORT FbMouseEvent
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_FBEVENT_H
