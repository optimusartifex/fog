// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_PAINTER_H
#define _FOG_G2D_PAINTING_PAINTER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageFilter.h>
#include <Fog/G2d/Painting/PaintEngine.h>
#include <Fog/G2d/Painting/PaintParams.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ImageBits;

// ============================================================================
// [Fog::Painter]
// ============================================================================

//! @brief Painter.
//!
//! The painter is high level class that can be used to draw into the image or
//! raw memory buffer. The painter is a state based machine where each paint
//! command depends on the current state. The current state can be changed by
//! methods like @c setFillRule(), @c setLineWidth(), @c setLineCaps()
//! @c setCompositingOperator(), and many others.
//!
//! @section Beginning and finalizing.
//!
//! To start painting to an image or raw memory buffer use @c Painter constructor
//! or call @c begin() method with your raw buffer or @c Image instance. After
//! you called @c begin() method that succeeded (check if return value is
//! @c Fog::ERR_OK) you can start painting. If you no longer need painter
//! instance delete it (by @c delete operator or static destructor) or call
//! @c end() method.
//!
//! It's very important to destroy painter instance or call @c end() method,
//! because rendering can be asynchronous and destroying or enging will ensure
//! that all painting operations are visible to the target image.
//!
//! @section Meta and user regions, clipping.
//!
//! Painter supports two indenpendent region and origin information that can
//! be used to affect painter origin and clipping. First region and origin
//! information are stored as meta region and meta origin. These variables
//! shouldn't be changed during rendering and are used usually by windowing
//! system to set correct origin and window clipping. If you are using Fog-Gui
//! library, never change these variables in @c Widget::onPaint() event.
//!
//! Also the meta region and origin is generally immutable. When you set it
//! you are forced to use it in all your painting code. Reseting these regions
//! will also reset all clipping and painter states. To set meta region and
//! origin use ... .
//!
//! Second region and origin information are stored as user region and user
//! origin. These variables are designed to be changeable by user during
//! rendering, so use them if it's useable for you.
//!
//! Origin and region clipping is pixel based and it's not affected by affine
//! transformations (so resetting transformations will not reset user origin).
//!
//! @section Transformations.
//!
//! TODO
struct FOG_NO_EXPORT Painter
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a null painter.
  FOG_INLINE Painter()
  {
    _engine = fog_api.painter_getNullEngine();
    _vtable = _engine->vtable;
  }

  //! @brief Create a raster painter using the @a image as a destination.
  //!
  //! @sa begin().
  FOG_INLINE Painter(Image& image, uint32_t initFlags = NO_FLAGS)
  {
    _engine = NULL;
    fog_api.painter_beginImage(this, &image, NULL, initFlags);
  }

  //! @overload
  FOG_INLINE Painter(Image& image, const RectI& rect, uint32_t initFlags = NO_FLAGS)
  {
    _engine = NULL;
    fog_api.painter_beginImage(this, &image, &rect, initFlags);
  }

  //! @brief Create a raster painter using the @a raw image buffer as a destination.
  //!
  //! @sa begin().
  FOG_INLINE Painter(const ImageBits& imageBits, uint32_t initFlags = NO_FLAGS)
  {
    _engine = NULL;
    fog_api.painter_beginIBits(this, &imageBits, NULL, initFlags);
  }

  //! @overload
  FOG_INLINE Painter(const ImageBits& imageBits, const RectI& rect, uint32_t initFlags = NO_FLAGS)
  {
    _engine = NULL;
    fog_api.painter_beginIBits(this, &imageBits, &rect, initFlags);
  }

  //! @brief Destroy the painter (waiting to complete all painter commands).
  //!
  //! @sa end().
  FOG_INLINE ~Painter()
  {
    _vtable->release(this);
  }

  // --------------------------------------------------------------------------
  // [Begin / End]
  // --------------------------------------------------------------------------

  //! @brief Begin painting into the @a image.
  //!
  //! Always call @c Painter::end() or destroy painter before you try to
  //! destroy or work with image. If you want to use the image you provided,
  //! but you want to continue paniting on it then call @c Painter::flush()
  //! with @c PAINTER_FLUSH_SYNC flag to make content in the image ready.
  FOG_INLINE err_t begin(Image& image, uint32_t initFlags = NO_FLAGS)
  {
    return fog_api.painter_beginImage(this, &image, NULL, initFlags);
  }

  //! @overload
  FOG_INLINE err_t begin(Image& image, const RectI& rect, uint32_t initFlags = NO_FLAGS)
  {
    return fog_api.painter_beginImage(this, &image, &rect, initFlags);
  }

  //! @brief Begin painting to the custom raw image @a raw.
  //!
  //! This method can be used to paint to custom image buffer (WinGDI, Cairo,
  //! Qt4, SDL_Surface, and others).
  FOG_INLINE err_t begin(const ImageBits& imageBits, uint32_t initFlags = NO_FLAGS)
  {
    return fog_api.painter_beginIBits(this, &imageBits, NULL, initFlags);
  }

  //! @overload
  FOG_INLINE err_t begin(const ImageBits& imageBits, const RectI& rect, uint32_t initFlags = NO_FLAGS)
  {
    return fog_api.painter_beginIBits(this, &imageBits, &rect, initFlags);
  }

  //! @brief Wait for completition of all painter commands, unlock the
  //! destination image and destroy the associated painter engine.
  FOG_INLINE err_t end()
  {
    return _vtable->release(this);
  }

  // --------------------------------------------------------------------------
  // [SwitchTo]
  // --------------------------------------------------------------------------

  //! @brief Finish painting and switch the destination image to @a image,
  //! keeping the painter engine configuration as is.
  //!
  //! @note Calling switchTo() will destroy all layers and saved states, it's
  //! like calling @c end(), but reusing the painter engine and its resources.
  FOG_INLINE err_t switchTo(Image& image)
  {
    return fog_api.painter_switchToImage(this, &image, NULL);
  }

  //! @overload
  FOG_INLINE err_t switchTo(Image& image, const RectI& rect)
  {
    return fog_api.painter_switchToImage(this, &image, &rect);
  }

  //! @brief Finish painting and switch the destination image to @a raw buffer,
  //! keeping the painter engine configuration as is.
  //!
  //! @note Calling switchTo() will destroy all layers and saved states, it's
  //! like calling @c end(), but reusing the painter engine and its resources.
  FOG_INLINE err_t switchTo(const ImageBits& imageBits)
  {
    return fog_api.painter_switchToIBits(this, &imageBits, NULL);
  }

  //! @overload
  FOG_INLINE err_t switchTo(const ImageBits& imageBits, const RectI& rect)
  {
    return fog_api.painter_switchToIBits(this, &imageBits, &rect);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Meta-Params]
  // --------------------------------------------------------------------------

  //! @brief Get meta variables (meta origin and meta region).
  FOG_INLINE err_t getMetaParams(Region& region, PointI& origin) const
  {
    return _vtable->getMetaParams(this, region, origin);
  }

  //! @brief Set painter meta variables (meta origin and meta region).
  //!
  //! Calling this method will also reset all user variables (user origin and
  //! user region).
  //!
  //! This function was designed as an optimization for windowing systems to
  //! set all important origin and clipping information per one function call
  //! (usually called per widget.
  FOG_INLINE err_t setMetaParams(const Region& region, const PointI& origin)
  {
    return _vtable->setMetaParams(this, region, origin);
  }

  //! @brief Reset meta variables (setting meta region to infinite and meta
  //! origin to zero).
  //!
  //! This function also resets all clipping and states like @c setMetaParams()
  //! do.
  FOG_INLINE err_t resetMetaParams()
  {
    return _vtable->resetMetaParams(this);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Size, Format, Engine]
  // --------------------------------------------------------------------------

  //! @brief Get the size in pixels or paint-engine units.
  FOG_INLINE err_t getSize(SizeI& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_SIZE_I, &val);
  }

  //! @overload
  FOG_INLINE err_t getSize(SizeF& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_SIZE_F, &val);
  }

  //! @overload
  FOG_INLINE err_t getSize(SizeD& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_SIZE_D, &val);
  }

  //! @brief Get the image format.
  FOG_INLINE err_t getFormat(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_FORMAT_I, &val);
  }

  //! @brief Get the painter-engine type (see @c PAINT_DEVICE).
  FOG_INLINE err_t getDeviceId(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_DEVICE_I, &val);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Multithreading]
  // --------------------------------------------------------------------------

  //! @brief Get the number of threads the paint-engine can use for rendering.
  FOG_INLINE err_t getMultithreadedMode(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_MULTITHREADED_I, &val);
  }

  //! @brief Set the number of threads the paint-engine can use for rendering.
  FOG_INLINE err_t setMultithreadedMode(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_MULTITHREADED_I, &val);
  }

  //! @brief Get the number of threads the paint-engine can use for rendering.
  FOG_INLINE err_t getMaxThreads(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_MAX_THREADS_I, &val);
  }

  //! @brief Set the number of threads the paint-engine can use for rendering.
  FOG_INLINE err_t setMaxThreads(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_MAX_THREADS_I, &val);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Paint Params]
  // --------------------------------------------------------------------------

  //! @brief Get the paint parameters (float).
  FOG_INLINE err_t getPaintParams(PaintParamsF& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_PAINT_PARAMS_F, &val);
  }

  //! @brief Get the paint parameters (double).
  FOG_INLINE err_t getPaintParams(PaintParamsD& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_PAINT_PARAMS_D, &val);
  }

  //! @brief Set the paint parameters (float).
  FOG_INLINE err_t setPaintParams(const PaintParamsF& val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_PAINT_PARAMS_F, &val);
  }

  //! @brief Set the paint parameters (double).
  FOG_INLINE err_t setPaintParams(const PaintParamsD& val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_PAINT_PARAMS_D, &val);
  }

  //! @brief Reset the paint parameters.
  FOG_INLINE err_t resetPaintParams()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_PAINT_PARAMS_F);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Paint Hints - All]
  // --------------------------------------------------------------------------

  //! @brief Get paint hints.
  FOG_INLINE err_t getPaintHints(PaintHints& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_PAINT_HINTS, &val);
  }

  //! @brief Set paint hints.
  FOG_INLINE err_t setPaintHints(const PaintHints& val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_PAINT_HINTS, &val);
  }

  //! @brief Reset paint hints.
  FOG_INLINE err_t resetPaintHints()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_PAINT_HINTS);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Paint Hints - Compositing Operator]
  // --------------------------------------------------------------------------

  //! @brief Get the compositing operator.
  FOG_INLINE err_t getCompositingOperator(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_COMPOSITING_OPERATOR_I, &val);
  }

  //! @brief Set the compositing operator.
  FOG_INLINE err_t setCompositingOperator(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_COMPOSITING_OPERATOR_I, &val);
  }

  //! @brief Reset the compositing operator.
  FOG_INLINE err_t resetCompositingOperator()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_COMPOSITING_OPERATOR_I);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Paint Hints - Render Quality]
  // --------------------------------------------------------------------------

  //! @brief Get the render-quality hint (see @c RENDER_QUALITY).
  FOG_INLINE err_t getRenderQuality(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_RENDER_QUALITY_I, &val);
  }

  //! @brief Set the render-quality hint (see @c RENDER_QUALITY).
  FOG_INLINE err_t setRenderQuality(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_RENDER_QUALITY_I, &val);
  }

  //! @brief Reset the render-quality hint.
  FOG_INLINE err_t resetRenderQuality()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_RENDER_QUALITY_I);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Paint Hints - Image Quality]
  // --------------------------------------------------------------------------

  //! @brief Get the image-quality hint (see @c IMAGE_QUALITY).
  FOG_INLINE err_t getImageQuality(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_IMAGE_QUALITY_I, &val);
  }

  //! @brief Set the image-quality hint (see @c IMAGE_QUALITY).
  FOG_INLINE err_t setImageQuality(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_IMAGE_QUALITY_I, &val);
  }

  //! @brief Reset the image-quality hint.
  FOG_INLINE err_t resetImageQuality()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_IMAGE_QUALITY_I);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Paint Hints - Gradient Quality]
  // --------------------------------------------------------------------------

  //! @brief Get the gradient-quality hint (see @c GRADIENT_QUALITY).
  FOG_INLINE err_t getGradientQuality(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_GRADIENT_QUALITY_I, &val);
  }

  //! @brief Set the gradient-quality hint (see @c GRADIENT_QUALITY).
  FOG_INLINE err_t setGradientQuality(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_GRADIENT_QUALITY_I, &val);
  }

  //! @brief Reset the gradient-quality hint.
  FOG_INLINE err_t resetGradientQuality()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_GRADIENT_QUALITY_I);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Paint Hints - Outlined Text Hint]
  // --------------------------------------------------------------------------

  //! @brief Get the outlined-text hint.
  FOG_INLINE err_t getOutlinedTextHint(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_OUTLINED_TEXT_I, &val);
  }

  //! @brief Set the outlined-text hint.
  FOG_INLINE err_t setOutlinedTextHint(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_OUTLINED_TEXT_I, &val);
  }

  //! @brief Reset the outlined-text hint.
  FOG_INLINE err_t resetOutlinedTextHint()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_OUTLINED_TEXT_I);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Paint Hints - Fast Line Hint]
  // --------------------------------------------------------------------------

  //! @brief Get the fast-line hint.
  FOG_INLINE err_t getFastLineHint(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_FAST_LINE_I, &val);
  }

  //! @brief Set the fast-line hint.
  FOG_INLINE err_t setFastLineHint(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_FAST_LINE_I, &val);
  }

  //! @brief Reset the fast-line hint.
  FOG_INLINE err_t resetFastLineHint()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_FAST_LINE_I);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Paint Hints - Geometric Precision Hint]
  // --------------------------------------------------------------------------

  //! @brief Get the geometric-precision hint.
  FOG_INLINE err_t getGeometricPrecisionHint(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_GEOMETRIC_PRECISION_I, &val);
  }

  //! @brief Set the geometric-precision hint.
  FOG_INLINE err_t setGeometricPrecisionHint(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_GEOMETRIC_PRECISION_I, &val);
  }

  //! @brief Reset the geometric-precision hint.
  FOG_INLINE err_t resetGeometricPrecisionHint()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_GEOMETRIC_PRECISION_I);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Paint Opacity]
  // --------------------------------------------------------------------------

  //! @brief Get the opacity (float).
  FOG_INLINE err_t getOpacity(float& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_OPACITY_F, &val);
  }

  //! @brief Get the opacity (double).
  FOG_INLINE err_t getOpacity(double& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_OPACITY_D, &val);
  }

  //! @brief Set the opacity (float).
  FOG_INLINE err_t setOpacity(float val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_OPACITY_F, &val);
  }

  //! @brief Set the opacity (double).
  FOG_INLINE err_t setOpacity(double val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_OPACITY_D, &val);
  }

  //! @brief Reset the opacity.
  FOG_INLINE err_t resetOpacity()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_OPACITY_F);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Fill Params - Fill Rule]
  // --------------------------------------------------------------------------

  //! @brief Get the fill rule (see @c FILL_RULE).
  FOG_INLINE err_t getFillRule(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_FILL_RULE_I, &val);
  }

  //! @brief Set the fill rule (see @c FILL_RULE).
  FOG_INLINE err_t setFillRule(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_FILL_RULE_I, &val);
  }

  //! @brief Reset the fill rule.
  FOG_INLINE err_t resetFillRule()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_FILL_RULE_I);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Stroke Params - All]
  // --------------------------------------------------------------------------

  //! @brief Get the stroke parameters (float).
  FOG_INLINE err_t getStrokeParams(PathStrokerParamsF& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_STROKE_PARAMS_F, &val);
  }

  //! @brief Get the stroke parameters (double).
  FOG_INLINE err_t getStrokeParams(PathStrokerParamsD& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_STROKE_PARAMS_D, &val);
  }

  //! @brief Set the stroke parameters (float).
  FOG_INLINE err_t setStrokeParams(const PathStrokerParamsF& val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_STROKE_PARAMS_F, &val);
  }

  //! @brief Set the stroke parameters (double).
  FOG_INLINE err_t setStrokeParams(const PathStrokerParamsD& val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_STROKE_PARAMS_D, &val);
  }

  //! @brief Reset the stroke parameters.
  FOG_INLINE err_t resetStrokeParams()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_STROKE_PARAMS_F);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Stroke Params - Line Width]
  // --------------------------------------------------------------------------

  //! @brief Get the line width (float).
  FOG_INLINE err_t getLineWidth(float& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_LINE_WIDTH_F, &val);
  }

  //! @brief Get the line width (double).
  FOG_INLINE err_t getLineWidth(double& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_LINE_WIDTH_D, &val);
  }

  //! @brief Set the line width (float).
  FOG_INLINE err_t setLineWidth(float val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_LINE_WIDTH_F, &val);
  }

  //! @brief Set the line width (double).
  FOG_INLINE err_t setLineWidth(double val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_LINE_WIDTH_D, &val);
  }

  //! @brief Reset the line width.
  FOG_INLINE err_t resetLineWidth()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_LINE_WIDTH_F);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Stroke Params - Line Join]
  // --------------------------------------------------------------------------

  //! @brief Get the line join (see @c LINE_JOIN).
  FOG_INLINE err_t getLineJoin(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_LINE_JOIN_I, &val);
  }

  //! @brief Set the line join (see @c LINE_JOIN).
  FOG_INLINE err_t setLineJoin(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_LINE_JOIN_I, &val);
  }

  //! @brief Reset the line join.
  FOG_INLINE err_t resetLineJoin()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_LINE_JOIN_I);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Stroke Params - Line Caps]
  // --------------------------------------------------------------------------

  //! @brief Get the start cap (see @c LINE_CAP).
  FOG_INLINE err_t getStartCap(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_START_CAP_I, &val);
  }

  //! @brief Set the start cap (see @c LINE_CAP).
  FOG_INLINE err_t setStartCap(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_START_CAP_I, &val);
  }

  //! @brief Reset the start cap.
  FOG_INLINE err_t resetStartCap()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_START_CAP_I);
  }

  //! @brief Get the end cap (see @c LINE_CAP).
  FOG_INLINE err_t getEndCap(uint32_t& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_END_CAP_I, &val);
  }

  //! @brief Set the end cap (see @c LINE_CAP).
  FOG_INLINE err_t setEndCap(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_END_CAP_I, &val);
  }

  //! @brief Reset the start cap.
  FOG_INLINE err_t resetEndCap()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_END_CAP_I);
  }

  //! @brief Get both, the start cap and the end cap.
  //!
  //! @note This function only succeeds if the start and end caps are the same.
  FOG_INLINE err_t getLineCaps(uint32_t val)
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_LINE_CAPS_I, &val);
  }

  //! @brief Set both, the start cap and the end cap.
  FOG_INLINE err_t setLineCaps(uint32_t val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_LINE_CAPS_I, &val);
  }

  //! @brief Reset both, the start cap and the end cap.
  FOG_INLINE err_t resetLineCpas()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_LINE_CAPS_I);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Stroke Params - Miter Limit]
  // --------------------------------------------------------------------------

  //! @brief Get the miter limit (float).
  FOG_INLINE err_t getMiterLimit(float& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_MITER_LIMIT_F, &val);
  }

  //! @brief Get the miter limit (double).
  FOG_INLINE err_t getMiterLimit(double& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_MITER_LIMIT_D, &val);
  }

  //! @brief Set the miter limit (float).
  FOG_INLINE err_t setMiterLimit(float val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_MITER_LIMIT_F, &val);
  }

  //! @brief Set the miter limit (double).
  FOG_INLINE err_t setMiterLimit(double val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_MITER_LIMIT_D, &val);
  }

  //! @brief Reset the miter limit.
  FOG_INLINE err_t resetMiterLimit()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_MITER_LIMIT_F);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Stroke Params - Dash Offset]
  // --------------------------------------------------------------------------

  //! @brief Get the dash-offset (float).
  FOG_INLINE err_t getDashOffset(float& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_DASH_OFFSET_F, &val);
  }

  //! @brief Get the dash-offset (double).
  FOG_INLINE err_t getDashOffset(double& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_DASH_OFFSET_D, &val);
  }

  //! @brief Set the dash-offset (float).
  FOG_INLINE err_t setDashOffset(float val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_DASH_OFFSET_F, &val);
  }

  //! @brief Set the dash-offset (double).
  FOG_INLINE err_t setDashOffset(double val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_DASH_OFFSET_D, &val);
  }

  //! @brief Reset the dash-offset.
  FOG_INLINE err_t resetDashOffset()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_DASH_OFFSET_F);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Stroke Params - Dash List]
  // --------------------------------------------------------------------------

  //! @brief Get the dash-data (float).
  FOG_INLINE err_t getDashList(List<float>& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_DASH_LIST_F, &val);
  }

  //! @brief Get the dash-data (double).
  FOG_INLINE err_t getDashList(List<double>& val) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_DASH_LIST_D, &val);
  }

  //! @brief Set the dash-list (float).
  FOG_INLINE err_t setDashList(const List<float>& val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_DASH_LIST_F, &val);
  }

  //! @brief Set the dash-list (double).
  FOG_INLINE err_t setDashList(const List<double>& val)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_DASH_LIST_D, &val);
  }

  //! @brief Reset the dash-list.
  FOG_INLINE err_t resetDashList()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_DASH_LIST_F);
  }

  // --------------------------------------------------------------------------
  // [Parameters - Filter-Scale]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getFilterScale(ImageFilterScaleF& filterScale) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_FILTER_SCALE_F, &filterScale);
  }

  FOG_INLINE err_t getFilterScale(ImageFilterScaleD& filterScale) const
  {
    return _vtable->getParameter(this, PAINTER_PARAMETER_FILTER_SCALE_D, &filterScale);
  }

  FOG_INLINE err_t setFilterScale(ImageFilterScaleF& filterScale)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_FILTER_SCALE_F, &filterScale);
  }

  FOG_INLINE err_t setFilterScale(ImageFilterScaleD& filterScale)
  {
    return _vtable->setParameter(this, PAINTER_PARAMETER_FILTER_SCALE_D, &filterScale);
  }

  FOG_INLINE err_t reserFilterScale()
  {
    return _vtable->resetParameter(this, PAINTER_PARAMETER_FILTER_SCALE_F);
  }

  // --------------------------------------------------------------------------
  // [Source - Type]
  // --------------------------------------------------------------------------

  //! @brief Get the source type (see @c PATTERN_TYPE).
  FOG_INLINE err_t getSourceType(uint32_t& val) const
  {
    return _vtable->getSourceType(this, val);
  }

  // --------------------------------------------------------------------------
  // [Source - Get]
  // --------------------------------------------------------------------------

  //! @brief Get the source color into @a val (see @c Color).
  FOG_INLINE err_t getSource(Color& val) const
  {
    return _vtable->getSourceColor(this, val);
  }

  //! @brief Get the source pattern into @a val (see @c Pattern).
  FOG_INLINE err_t getSource(Pattern& val) const
  {
    return _vtable->getSourcePattern(this, val);
  }

  // --------------------------------------------------------------------------
  // [Source - Set]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setSource()
  {
    return _vtable->setSourceNone(this);
  }

  //! @brief Set the painter source color to a given @c Argb32 color @a val.
  FOG_INLINE err_t setSource(const Argb32& val)
  {
    return _vtable->setSourceArgb32(this, val.getPacked32());
  }

  //! @brief Set the painter source color to a given @c Argb64 color @a val.
  FOG_INLINE err_t setSource(const Argb64& val)
  {
    return _vtable->setSourceArgb64(this, val);
  }

  //! @brief Set the painter source to a @c Color @a val.
  FOG_INLINE err_t setSource(const Color& val)
  {
    return _vtable->setSourceColor(this, val);
  }

  //! @brief Set the painter source to a @c Texture @a val.
  FOG_INLINE err_t setSource(const Texture& val)
  {
    return _vtable->setSourceAbstract(this, PAINTER_SOURCE_TEXTURE_F, &val, NULL);
  }

  //! @brief Set the painter source to a transformed @c Texture @a val.
  FOG_INLINE err_t setSource(const Texture& val, const TransformF& tr)
  {
    return _vtable->setSourceAbstract(this, PAINTER_SOURCE_TEXTURE_F, &val, &tr);
  }

  //! @brief Set the painter source to a transformed @c Texture @a val.
  FOG_INLINE err_t setSource(const Texture& val, const TransformD& tr)
  {
    return _vtable->setSourceAbstract(this, PAINTER_SOURCE_TEXTURE_D, &val, &tr);
  }

  //! @brief Set the painter source to a GradientF @a val.
  FOG_INLINE err_t setSource(const GradientF& val)
  {
    return _vtable->setSourceAbstract(this, PAINTER_SOURCE_GRADIENT_F, &val, NULL);
  }

  //! @brief Set the painter source to a GradientD @a val.
  FOG_INLINE err_t setSource(const GradientD& val)
  {
    return _vtable->setSourceAbstract(this, PAINTER_SOURCE_GRADIENT_D, &val, NULL);

 }
  //! @brief Set the painter source to a transformed @c GradientF @a val.
  FOG_INLINE err_t setSource(const GradientF& val, const TransformF& tr)
  {
    return _vtable->setSourceAbstract(this, PAINTER_SOURCE_GRADIENT_F, &val, &tr);
  }

  //! @brief Set the painter source to a transformed @c GradientD @a val.
  FOG_INLINE err_t setSource(const GradientD& val, const TransformD& tr)
  {
    return _vtable->setSourceAbstract(this, PAINTER_SOURCE_GRADIENT_D, &val, &tr);
  }

  //! @brief Set the painter source to a Pattern @a val.
  FOG_INLINE err_t setSource(const Pattern& val)
  {
    return _vtable->setSourcePattern(this, val);
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  //! @brief Get the user transformation matrix into @a tr.
  FOG_INLINE err_t getTransform(TransformF& tr) const
  {
    return _vtable->getTransformF(this, tr);
  }

  //! @overload
  FOG_INLINE err_t getTransform(TransformD& tr) const
  {
    return _vtable->getTransformD(this, tr);
  }

  //! @brief Set the transformation matrix to @a tr.
  FOG_INLINE err_t setTransform(const TransformF& tr)
  {
    return _vtable->setTransformF(this, tr);
  }

  //! @overload
  FOG_INLINE err_t setTransform(const TransformD& tr)
  {
    return _vtable->setTransformD(this, tr);
  }

  // --------------------------------------------------------------------------
  // [Transform - Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t translate(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _vtable->applyTransform(this, TRANSFORM_OP_TRANSLATEF | (order << 4), &p);
  }

  FOG_INLINE err_t translate(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _vtable->applyTransform(this, TRANSFORM_OP_TRANSLATED | (order << 4), &p);
  }

  // --------------------------------------------------------------------------
  // [Transform - Scale]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t scale(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _vtable->applyTransform(this, TRANSFORM_OP_SCALEF | (order << 4), &p);
  }

  FOG_INLINE err_t scale(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _vtable->applyTransform(this, TRANSFORM_OP_SCALED | (order << 4), &p);
  }

  // --------------------------------------------------------------------------
  // [Transform - Skew]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t skew(const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _vtable->applyTransform(this, TRANSFORM_OP_SKEWF | (order << 4), &p);
  }

  FOG_INLINE err_t skew(const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _vtable->applyTransform(this, TRANSFORM_OP_SKEWD | (order << 4), &p);
  }

  // --------------------------------------------------------------------------
  // [Transform - Rotate]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t rotate(float angle, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _vtable->applyTransform(this, TRANSFORM_OP_ROTATEF | (order << 4), &angle);
  }

  FOG_INLINE err_t rotate(double angle, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _vtable->applyTransform(this, TRANSFORM_OP_ROTATED | (order << 4), &angle);
  }

  FOG_INLINE err_t rotate(float angle, const PointF& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    float params[3] = { angle, p.x, p.y };
    return _vtable->applyTransform(this, TRANSFORM_OP_ROTATE_POINTF | (order << 4), params);
  }

  FOG_INLINE err_t rotate(double angle, const PointD& p, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    double params[3] = { angle, p.x, p.y };
    return _vtable->applyTransform(this, TRANSFORM_OP_ROTATE_POINTD | (order << 4), params);
  }

  // --------------------------------------------------------------------------
  // [Transform - Multiply]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t transform(const TransformF& tr, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _vtable->applyTransform(this, TRANSFORM_OP_MULTIPLYF | (order << 4), &tr);
  }

  FOG_INLINE err_t transform(const TransformD& tr, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    return _vtable->applyTransform(this, TRANSFORM_OP_MULTIPLYD | (order << 4), &tr);
  }

  // --------------------------------------------------------------------------
  // [Transform - Reset]
  // --------------------------------------------------------------------------

  //! @brief Reset the transformation matrix to identity.
  FOG_INLINE err_t resetTransform()
  {
    return _vtable->resetTransform(this);
  }

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  //! @brief Save the current painter state.
  FOG_INLINE err_t save()
  {
    return _vtable->save(this);
  }

  //! @brief Restore the current painter state.
  FOG_INLINE err_t restore()
  {
    return _vtable->restore(this);
  }

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  //! @brief Convert the world coordinate into the screen coordinates.
  FOG_INLINE err_t mapUserToDevice(PointF& p) const
  {
    return _vtable->mapPointF(this, PAINTER_MAP_USER_TO_DEVICE, p);
  };

  //! @overload
  FOG_INLINE err_t mapUserToDevice(PointD& p) const
  {
    return _vtable->mapPointD(this, PAINTER_MAP_USER_TO_DEVICE, p);
  };

  //! @brief Convert the screen coordinate into the world coordinates.
  FOG_INLINE err_t mapDeviceToUser(PointF& p) const
  {
    return _vtable->mapPointF(this, PAINTER_MAP_DEVICE_TO_USER, p);
  };

  //! @overload
  FOG_INLINE err_t mapDeviceToUser(PointD& p) const
  {
    return _vtable->mapPointD(this, PAINTER_MAP_DEVICE_TO_USER, p);
  };

  // --------------------------------------------------------------------------
  // [Draw]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t drawBox(const BoxI& box) { return drawRect(RectI(box)); }
  FOG_INLINE err_t drawBox(const BoxF& box) { return drawRect(RectF(box)); }
  FOG_INLINE err_t drawBox(const BoxD& box) { return drawRect(RectD(box)); }

  FOG_INLINE err_t drawBox(int x0, int y0, int x1, int y1) { return drawRect(RectI(x0, y0, x1 - x0, y1 - y0)); }
  FOG_INLINE err_t drawBox(float x0, float y0, float x1, float y1) { return drawRect(RectF(x0, y0, x1 - x0, y1 - y0)); }
  FOG_INLINE err_t drawBox(double x0, double y0, double x1, double y1) { return drawRect(RectD(x0, y0, x1 - x0, y1 - y0)); }

  FOG_INLINE err_t drawRect(const RectI& r) { return _vtable->drawRectI(this, r); }
  FOG_INLINE err_t drawRect(const RectF& r) { return _vtable->drawRectF(this, r); }
  FOG_INLINE err_t drawRect(const RectD& r) { return _vtable->drawRectD(this, r); }

  FOG_INLINE err_t drawRect(int x, int y, int w, int h) { return drawRect(RectI(x, y, w, h)); }
  FOG_INLINE err_t drawRect(float x, float y, float w, float h) { return drawRect(RectF(x, y, w, h)); }
  FOG_INLINE err_t drawRect(double x, double y, double w, double h) { return drawRect(RectD(x, y, w, h)); }

  FOG_INLINE err_t drawPolyline(const PointI* p, size_t count) { return _vtable->drawPolylineI(this, p, count); }
  FOG_INLINE err_t drawPolyline(const PointF* p, size_t count) { return _vtable->drawPolylineF(this, p, count); }
  FOG_INLINE err_t drawPolyline(const PointD* p, size_t count) { return _vtable->drawPolylineD(this, p, count); }

  FOG_INLINE err_t drawPolygon(const PointI* p, size_t count) { return _vtable->drawPolygonI(this, p, count); }
  FOG_INLINE err_t drawPolygon(const PointF* p, size_t count) { return _vtable->drawPolygonF(this, p, count); }
  FOG_INLINE err_t drawPolygon(const PointD* p, size_t count) { return _vtable->drawPolygonD(this, p, count); }

  FOG_INLINE err_t drawLine(const LineF& line) { return _vtable->drawShapeF(this, SHAPE_TYPE_LINE, &line); }
  FOG_INLINE err_t drawLine(const LineD& line) { return _vtable->drawShapeD(this, SHAPE_TYPE_LINE, &line); }

  FOG_INLINE err_t drawLine(const PointF& p0, const PointF& p1) { return drawLine(LineF(p0, p1)); }
  FOG_INLINE err_t drawLine(const PointD& p0, const PointD& p1) { return drawLine(LineD(p0, p1)); }

  FOG_INLINE err_t drawLine(float x0, float y0, float x1, float y1) { return drawLine(LineF(x0, y0, x1, y1)); }
  FOG_INLINE err_t drawLine(double x0, double y0, double x1, double y1) { return drawLine(LineD(x0, y0, x1, y1)); }

  FOG_INLINE err_t drawQBezier(const QBezierF& quad) { return _vtable->drawShapeF(this, SHAPE_TYPE_QBEZIER, &quad); }
  FOG_INLINE err_t drawQBezier(const QBezierD& quad) { return _vtable->drawShapeD(this, SHAPE_TYPE_QBEZIER, &quad); }

  FOG_INLINE err_t drawQBezier(float x0, float y0, float x1, float y1, float x2, float y2) { return drawQBezier(QBezierF(x0, y0, x1, y1, x2, y2)); }
  FOG_INLINE err_t drawQBezier(double x0, double y0, double x1, double y1, double x2, double y2) { return drawQBezier(QBezierD(x0, y0, x1, y1, x2, y2)); }

  FOG_INLINE err_t drawCBezier(const CBezierF& cubic) { return _vtable->drawShapeF(this, SHAPE_TYPE_CBEZIER, &cubic); }
  FOG_INLINE err_t drawCBezier(const CBezierD& cubic) { return _vtable->drawShapeD(this, SHAPE_TYPE_CBEZIER, &cubic); }

  FOG_INLINE err_t drawCBezier(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3) { return drawCBezier(CBezierF(x0, y0, x1, y1, x2, y2, x3, y3)); }
  FOG_INLINE err_t drawCBezier(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3) { return drawCBezier(CBezierD(x0, y0, x1, y1, x2, y2, x3, y3)); }

  FOG_INLINE err_t drawArc(const ArcF& arc) { return _vtable->drawShapeF(this, SHAPE_TYPE_ARC, &arc); }
  FOG_INLINE err_t drawArc(const ArcD& arc) { return _vtable->drawShapeD(this, SHAPE_TYPE_ARC, &arc); }

  FOG_INLINE err_t drawCircle(const CircleF& circle) { return _vtable->drawShapeF(this, SHAPE_TYPE_CIRCLE, &circle); }
  FOG_INLINE err_t drawCircle(const CircleD& circle) { return _vtable->drawShapeD(this, SHAPE_TYPE_CIRCLE, &circle); }

  FOG_INLINE err_t drawEllipse(const EllipseF& ellipse) { return _vtable->drawShapeF(this, SHAPE_TYPE_ELLIPSE, &ellipse); }
  FOG_INLINE err_t drawEllipse(const EllipseD& ellipse) { return _vtable->drawShapeD(this, SHAPE_TYPE_ELLIPSE, &ellipse); }

  FOG_INLINE err_t drawRound(const RoundF& round) { return _vtable->drawShapeF(this, SHAPE_TYPE_ROUND, &round); }
  FOG_INLINE err_t drawRound(const RoundD& round) { return _vtable->drawShapeD(this, SHAPE_TYPE_ROUND, &round); }

  FOG_INLINE err_t drawChord(const ArcF& chord) { return _vtable->drawShapeF(this, SHAPE_TYPE_CHORD, &chord); }
  FOG_INLINE err_t drawChord(const ArcD& chord) { return _vtable->drawShapeD(this, SHAPE_TYPE_CHORD, &chord); }

  FOG_INLINE err_t drawPie(const ArcF& pie) { return _vtable->drawShapeF(this, SHAPE_TYPE_PIE, &pie); }
  FOG_INLINE err_t drawPie(const ArcD& pie) { return _vtable->drawShapeD(this, SHAPE_TYPE_PIE, &pie); }

  FOG_INLINE err_t drawTriangle(const TriangleF& triangle) { return _vtable->drawShapeF(this, SHAPE_TYPE_TRIANGLE, &triangle); }
  FOG_INLINE err_t drawTriangle(const TriangleD& triangle) { return _vtable->drawShapeD(this, SHAPE_TYPE_TRIANGLE, &triangle); }

  FOG_INLINE err_t drawShape(const ShapeF& shape) { return _vtable->drawShapeF(this, shape.getType(), shape.getData()); }
  FOG_INLINE err_t drawShape(const ShapeD& shape) { return _vtable->drawShapeD(this, shape.getType(), shape.getData()); }

  FOG_INLINE err_t drawPath(const PathF& p) { return _vtable->drawPathF(this, p); }
  FOG_INLINE err_t drawPath(const PathD& p) { return _vtable->drawPathD(this, p); }

  // --------------------------------------------------------------------------
  // [Fill]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t fillAll() { return _vtable->fillAll(this); }

  FOG_INLINE err_t fillBox(const BoxI& box) { return fillRect(RectI(box)); }
  FOG_INLINE err_t fillBox(const BoxF& box) { return fillRect(RectF(box)); }
  FOG_INLINE err_t fillBox(const BoxD& box) { return fillRect(RectD(box)); }

  FOG_INLINE err_t fillBox(int x0, int y0, int x1, int y1) { return fillRect(RectI(x0, y0, x1 - x0, y1 - y0)); }
  FOG_INLINE err_t fillBox(float x0, float y0, float x1, float y1) { return fillRect(RectF(x0, y0, x1 - x0, y1 - y0)); }
  FOG_INLINE err_t fillBox(double x0, double y0, double x1, double y1) { return fillRect(RectD(x0, y0, x1 - x0, y1 - y0)); }

  FOG_INLINE err_t fillRect(const RectI& r) { return _vtable->fillRectI(this, r); }
  FOG_INLINE err_t fillRect(const RectF& r) { return _vtable->fillRectF(this, r); }
  FOG_INLINE err_t fillRect(const RectD& r) { return _vtable->fillRectD(this, r); }

  FOG_INLINE err_t fillRect(int x, int y, int w, int h) { return fillRect(RectI(x, y, w, h)); }
  FOG_INLINE err_t fillRect(float x, float y, float w, float h) { return fillRect(RectF(x, y, w, h)); }
  FOG_INLINE err_t fillRect(double x, double y, double w, double h) { return fillRect(RectD(x, y, w, h)); }

  FOG_INLINE err_t fillRects(const RectI* r, size_t count) { return _vtable->fillRectsI(this, r, count); }
  FOG_INLINE err_t fillRects(const RectF* r, size_t count) { return _vtable->fillRectsF(this, r, count); }
  FOG_INLINE err_t fillRects(const RectD* r, size_t count) { return _vtable->fillRectsD(this, r, count); }

  FOG_INLINE err_t fillPolygon(const PointI* p, size_t count) { return _vtable->fillPolygonI(this, p, count); }
  FOG_INLINE err_t fillPolygon(const PointF* p, size_t count) { return _vtable->fillPolygonF(this, p, count); }
  FOG_INLINE err_t fillPolygon(const PointD* p, size_t count) { return _vtable->fillPolygonD(this, p, count); }

  FOG_INLINE err_t fillQBezier(const QBezierF& quad) { return _vtable->fillShapeF(this, SHAPE_TYPE_QBEZIER, &quad); }
  FOG_INLINE err_t fillQBezier(const QBezierD& quad) { return _vtable->fillShapeD(this, SHAPE_TYPE_QBEZIER, &quad); }

  FOG_INLINE err_t fillQBezier(float x0, float y0, float x1, float y1, float x2, float y2) { return fillQBezier(QBezierF(x0, y0, x1, y1, x2, y2)); }
  FOG_INLINE err_t fillQBezier(double x0, double y0, double x1, double y1, double x2, double y2) { return fillQBezier(QBezierD(x0, y0, x1, y1, x2, y2)); }

  FOG_INLINE err_t fillCBezier(const CBezierF& cubic) { return _vtable->fillShapeF(this, SHAPE_TYPE_CBEZIER, &cubic); }
  FOG_INLINE err_t fillCBezier(const CBezierD& cubic) { return _vtable->fillShapeD(this, SHAPE_TYPE_CBEZIER, &cubic); }

  FOG_INLINE err_t fillCBezier(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3) { return fillCBezier(CBezierF(x0, y0, x1, y1, x2, y2, x3, y3)); }
  FOG_INLINE err_t fillCBezier(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3) { return fillCBezier(CBezierD(x0, y0, x1, y1, x2, y2, x3, y3)); }

  FOG_INLINE err_t fillArc(const ArcF& arc) { return _vtable->fillShapeF(this, SHAPE_TYPE_ARC, &arc); }
  FOG_INLINE err_t fillArc(const ArcD& arc) { return _vtable->fillShapeD(this, SHAPE_TYPE_ARC, &arc); }

  FOG_INLINE err_t fillCircle(const CircleF& circle) { return _vtable->fillShapeF(this, SHAPE_TYPE_CIRCLE, &circle); }
  FOG_INLINE err_t fillCircle(const CircleD& circle) { return _vtable->fillShapeD(this, SHAPE_TYPE_CIRCLE, &circle); }

  FOG_INLINE err_t fillEllipse(const EllipseF& ellipse) { return _vtable->fillShapeF(this, SHAPE_TYPE_ELLIPSE, &ellipse); }
  FOG_INLINE err_t fillEllipse(const EllipseD& ellipse) { return _vtable->fillShapeD(this, SHAPE_TYPE_ELLIPSE, &ellipse); }

  FOG_INLINE err_t fillRound(const RoundF& round) { return _vtable->fillShapeF(this, SHAPE_TYPE_ROUND, &round); }
  FOG_INLINE err_t fillRound(const RoundD& round) { return _vtable->fillShapeD(this, SHAPE_TYPE_ROUND, &round); }

  FOG_INLINE err_t fillChord(const ArcF& chord) { return _vtable->fillShapeF(this, SHAPE_TYPE_CHORD, &chord); }
  FOG_INLINE err_t fillChord(const ArcD& chord) { return _vtable->fillShapeD(this, SHAPE_TYPE_CHORD, &chord); }

  FOG_INLINE err_t fillPie(const ArcF& pie) { return _vtable->fillShapeF(this, SHAPE_TYPE_PIE, &pie); }
  FOG_INLINE err_t fillPie(const ArcD& pie) { return _vtable->fillShapeD(this, SHAPE_TYPE_PIE, &pie); }

  FOG_INLINE err_t fillTriangle(const TriangleF& triangle) { return _vtable->fillShapeF(this, SHAPE_TYPE_TRIANGLE, &triangle); }
  FOG_INLINE err_t fillTriangle(const TriangleD& triangle) { return _vtable->fillShapeD(this, SHAPE_TYPE_TRIANGLE, &triangle); }

  FOG_INLINE err_t fillShape(const ShapeF& shape) { return _vtable->fillShapeF(this, shape.getType(), shape.getData()); }
  FOG_INLINE err_t fillShape(const ShapeD& shape) { return _vtable->fillShapeD(this, shape.getType(), shape.getData()); }

  FOG_INLINE err_t fillPath(const PathF& p) { return _vtable->fillPathF(this, p); }
  FOG_INLINE err_t fillPath(const PathD& p) { return _vtable->fillPathD(this, p); }

  FOG_INLINE err_t fillText(const PointI& p, const StringW& text, const Font& font, const RectI* clip = NULL) { return _vtable->fillTextAtI(this, p, text, font, clip); }
  FOG_INLINE err_t fillText(const PointF& p, const StringW& text, const Font& font, const RectF* clip = NULL) { return _vtable->fillTextAtF(this, p, text, font, clip); }
  FOG_INLINE err_t fillText(const PointD& p, const StringW& text, const Font& font, const RectD* clip = NULL) { return _vtable->fillTextAtD(this, p, text, font, clip); }

  FOG_INLINE err_t fillText(const TextRectI& r, const StringW& text, const Font& font, const RectI* clip = NULL) { return _vtable->fillTextInI(this, r, text, font, clip); }
  FOG_INLINE err_t fillText(const TextRectF& r, const StringW& text, const Font& font, const RectF* clip = NULL) { return _vtable->fillTextInF(this, r, text, font, clip); }
  FOG_INLINE err_t fillText(const TextRectD& r, const StringW& text, const Font& font, const RectD* clip = NULL) { return _vtable->fillTextInD(this, r, text, font, clip); }

  FOG_INLINE err_t fillMask(const PointI& p, const Image& mask) { return _vtable->fillMaskAtI(this, p, mask, NULL); }
  FOG_INLINE err_t fillMask(const PointF& p, const Image& mask) { return _vtable->fillMaskAtF(this, p, mask, NULL); }
  FOG_INLINE err_t fillMask(const PointD& p, const Image& mask) { return _vtable->fillMaskAtD(this, p, mask, NULL); }

  FOG_INLINE err_t fillMask(const PointI& p, const Image& mask, const RectI& mFragment) { return _vtable->fillMaskAtI(this, p, mask, &mFragment); }
  FOG_INLINE err_t fillMask(const PointF& p, const Image& mask, const RectI& mFragment) { return _vtable->fillMaskAtF(this, p, mask, &mFragment); }
  FOG_INLINE err_t fillMask(const PointD& p, const Image& mask, const RectI& mFragment) { return _vtable->fillMaskAtD(this, p, mask, &mFragment); }

  FOG_INLINE err_t fillMask(const RectI& r, const Image& mask) { return _vtable->fillMaskInI(this, r, mask, NULL); }
  FOG_INLINE err_t fillMask(const RectF& r, const Image& mask) { return _vtable->fillMaskInF(this, r, mask, NULL); }
  FOG_INLINE err_t fillMask(const RectD& r, const Image& mask) { return _vtable->fillMaskInD(this, r, mask, NULL); }

  FOG_INLINE err_t fillMask(const RectI& r, const Image& mask, const RectI& mFragment) { return _vtable->fillMaskInI(this, r, mask, &mFragment); }
  FOG_INLINE err_t fillMask(const RectF& r, const Image& mask, const RectI& mFragment) { return _vtable->fillMaskInF(this, r, mask, &mFragment); }
  FOG_INLINE err_t fillMask(const RectD& r, const Image& mask, const RectI& mFragment) { return _vtable->fillMaskInD(this, r, mask, &mFragment); }

  FOG_INLINE err_t fillRegion(const Region& r) { return _vtable->fillRegion(this, r); }

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t blitImage(const PointI& p, const Image& src) { return _vtable->blitImageAtI(this, p, src, NULL); }
  FOG_INLINE err_t blitImage(const PointF& p, const Image& src) { return _vtable->blitImageAtF(this, p, src, NULL); }
  FOG_INLINE err_t blitImage(const PointD& p, const Image& src) { return _vtable->blitImageAtD(this, p, src, NULL); }

  FOG_INLINE err_t blitImage(const RectI& r, const Image& src) { return _vtable->blitImageInI(this, r, src, NULL); }
  FOG_INLINE err_t blitImage(const RectF& r, const Image& src) { return _vtable->blitImageInF(this, r, src, NULL); }
  FOG_INLINE err_t blitImage(const RectD& r, const Image& src) { return _vtable->blitImageInD(this, r, src, NULL); }

  FOG_INLINE err_t blitImage(const PointI& p, const Image& src, const RectI& sFragment) { return _vtable->blitImageAtI(this, p, src, &sFragment); }
  FOG_INLINE err_t blitImage(const PointF& p, const Image& src, const RectI& sFragment) { return _vtable->blitImageAtF(this, p, src, &sFragment); }
  FOG_INLINE err_t blitImage(const PointD& p, const Image& src, const RectI& sFragment) { return _vtable->blitImageAtD(this, p, src, &sFragment); }

  FOG_INLINE err_t blitImage(const RectI& r, const Image& src, const RectI& sFragment) { return _vtable->blitImageInI(this, r, src, &sFragment); }
  FOG_INLINE err_t blitImage(const RectF& r, const Image& src, const RectI& sFragment) { return _vtable->blitImageInF(this, r, src, &sFragment); }
  FOG_INLINE err_t blitImage(const RectD& r, const Image& src, const RectI& sFragment) { return _vtable->blitImageInD(this, r, src, &sFragment); }

  FOG_INLINE err_t blitMaskedImage(const PointI& p, const Image& src, const Image& mask) { return _vtable->blitMaskedImageAtI(this, p, src, mask, NULL, NULL); }
  FOG_INLINE err_t blitMaskedImage(const PointF& p, const Image& src, const Image& mask) { return _vtable->blitMaskedImageAtF(this, p, src, mask, NULL, NULL); }
  FOG_INLINE err_t blitMaskedImage(const PointD& p, const Image& src, const Image& mask) { return _vtable->blitMaskedImageAtD(this, p, src, mask, NULL, NULL); }

  FOG_INLINE err_t blitMaskedImage(const RectI& r, const Image& src, const Image& mask) { return _vtable->blitMaskedImageInI(this, r, src, mask, NULL, NULL); }
  FOG_INLINE err_t blitMaskedImage(const RectF& r, const Image& src, const Image& mask) { return _vtable->blitMaskedImageInF(this, r, src, mask, NULL, NULL); }
  FOG_INLINE err_t blitMaskedImage(const RectD& r, const Image& src, const Image& mask) { return _vtable->blitMaskedImageInD(this, r, src, mask, NULL, NULL); }

  FOG_INLINE err_t blitMaskedImage(const PointI& p, const Image& src, const Image& mask, const RectI& sFragment, const RectI& mFragment) { return _vtable->blitMaskedImageAtI(this, p, src, mask, &sFragment, &mFragment); }
  FOG_INLINE err_t blitMaskedImage(const PointF& p, const Image& src, const Image& mask, const RectI& sFragment, const RectI& mFragment) { return _vtable->blitMaskedImageAtF(this, p, src, mask, &sFragment, &mFragment); }
  FOG_INLINE err_t blitMaskedImage(const PointD& p, const Image& src, const Image& mask, const RectI& sFragment, const RectI& mFragment) { return _vtable->blitMaskedImageAtD(this, p, src, mask, &sFragment, &mFragment); }

  FOG_INLINE err_t blitMaskedImage(const RectI& r, const Image& src, const Image& mask, const RectI& sFragment, const RectI& mFragment) { return _vtable->blitMaskedImageInI(this, r, src, mask, &sFragment, &mFragment); }
  FOG_INLINE err_t blitMaskedImage(const RectF& r, const Image& src, const Image& mask, const RectI& sFragment, const RectI& mFragment) { return _vtable->blitMaskedImageInF(this, r, src, mask, &sFragment, &mFragment); }
  FOG_INLINE err_t blitMaskedImage(const RectD& r, const Image& src, const Image& mask, const RectI& sFragment, const RectI& mFragment) { return _vtable->blitMaskedImageInD(this, r, src, mask, &sFragment, &mFragment); }

  // --------------------------------------------------------------------------
  // [Filter]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t filterAll(const ImageFilter& filter) { return _vtable->filterAll(this, filter.getFeData()); }
  FOG_INLINE err_t filterAll(const FeBase& feBase) { return _vtable->filterAll(this, &feBase); }

  FOG_INLINE err_t filterRect(const ImageFilter& filter, const RectI& r) { return _vtable->filterRectI(this, filter.getFeData(), r); }
  FOG_INLINE err_t filterRect(const ImageFilter& filter, const RectF& r) { return _vtable->filterRectF(this, filter.getFeData(), r); }
  FOG_INLINE err_t filterRect(const ImageFilter& filter, const RectD& r) { return _vtable->filterRectD(this, filter.getFeData(), r); }

  FOG_INLINE err_t filterRect(const FeBase& feBase, const RectI& r) { return _vtable->filterRectI(this, &feBase, r); }
  FOG_INLINE err_t filterRect(const FeBase& feBase, const RectF& r) { return _vtable->filterRectF(this, &feBase, r); }
  FOG_INLINE err_t filterRect(const FeBase& feBase, const RectD& r) { return _vtable->filterRectD(this, &feBase, r); }

  FOG_INLINE err_t filterPath(const ImageFilter& filter, const PathF& p) { return _vtable->filterPathF(this, filter.getFeData(), p); }
  FOG_INLINE err_t filterPath(const ImageFilter& filter, const PathD& p) { return _vtable->filterPathD(this, filter.getFeData(), p); }

  FOG_INLINE err_t filterPath(const FeBase& feBase, const PathF& p) { return _vtable->filterPathF(this, &feBase, p); }
  FOG_INLINE err_t filterPath(const FeBase& feBase, const PathD& p) { return _vtable->filterPathD(this, &feBase, p); }

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t clipBox(uint32_t clipOp, const BoxI& r) { return _vtable->clipRectI(this, clipOp, RectI(r)); }
  FOG_INLINE err_t clipBox(uint32_t clipOp, const BoxF& r) { return _vtable->clipRectF(this, clipOp, RectF(r)); }
  FOG_INLINE err_t clipBox(uint32_t clipOp, const BoxD& r) { return _vtable->clipRectD(this, clipOp, RectD(r)); }

  FOG_INLINE err_t clipRect(uint32_t clipOp, const RectI& r) { return _vtable->clipRectI(this, clipOp, r); }
  FOG_INLINE err_t clipRect(uint32_t clipOp, const RectF& r) { return _vtable->clipRectF(this, clipOp, r); }
  FOG_INLINE err_t clipRect(uint32_t clipOp, const RectD& r) { return _vtable->clipRectD(this, clipOp, r); }

  FOG_INLINE err_t clipRects(uint32_t clipOp, const RectI* r, size_t count) { return _vtable->clipRectsI(this, clipOp, r, count); }
  FOG_INLINE err_t clipRects(uint32_t clipOp, const RectF* r, size_t count) { return _vtable->clipRectsF(this, clipOp, r, count); }
  FOG_INLINE err_t clipRects(uint32_t clipOp, const RectD* r, size_t count) { return _vtable->clipRectsD(this, clipOp, r, count); }

  FOG_INLINE err_t clipPolygon(uint32_t clipOp, const PointI* p, size_t count) { return _vtable->clipPolygonI(this, clipOp, p, count); }
  FOG_INLINE err_t clipPolygon(uint32_t clipOp, const PointF* p, size_t count) { return _vtable->clipPolygonF(this, clipOp, p, count); }
  FOG_INLINE err_t clipPolygon(uint32_t clipOp, const PointD* p, size_t count) { return _vtable->clipPolygonD(this, clipOp, p, count); }

  FOG_INLINE err_t clipLine(uint32_t clipOp, const LineF& line) { return _vtable->clipShapeF(this, clipOp, SHAPE_TYPE_LINE, &line); }
  FOG_INLINE err_t clipLine(uint32_t clipOp, const LineD& line) { return _vtable->clipShapeD(this, clipOp, SHAPE_TYPE_LINE, &line); }

  FOG_INLINE err_t clipQBezier(uint32_t clipOp, const QBezierF& quad) { return _vtable->clipShapeF(this, clipOp, SHAPE_TYPE_QBEZIER, &quad); }
  FOG_INLINE err_t clipQBezier(uint32_t clipOp, const QBezierD& quad) { return _vtable->clipShapeD(this, clipOp, SHAPE_TYPE_QBEZIER, &quad); }

  FOG_INLINE err_t clipCBezier(uint32_t clipOp, const CBezierF& cubic) { return _vtable->clipShapeF(this, clipOp, SHAPE_TYPE_CBEZIER, &cubic); }
  FOG_INLINE err_t clipCBezier(uint32_t clipOp, const CBezierD& cubic) { return _vtable->clipShapeD(this, clipOp, SHAPE_TYPE_CBEZIER, &cubic); }

  FOG_INLINE err_t clipArc(uint32_t clipOp, const ArcF& arc) { return _vtable->clipShapeF(this, clipOp, SHAPE_TYPE_ARC, &arc); }
  FOG_INLINE err_t clipArc(uint32_t clipOp, const ArcD& arc) { return _vtable->clipShapeD(this, clipOp, SHAPE_TYPE_ARC, &arc); }

  FOG_INLINE err_t clipCircle(uint32_t clipOp, const CircleF& circle) { return _vtable->clipShapeF(this, clipOp, SHAPE_TYPE_CIRCLE, &circle); }
  FOG_INLINE err_t clipCircle(uint32_t clipOp, const CircleD& circle) { return _vtable->clipShapeD(this, clipOp, SHAPE_TYPE_CIRCLE, &circle); }

  FOG_INLINE err_t clipEllipse(uint32_t clipOp, const EllipseF& ellipse) { return _vtable->clipShapeF(this, clipOp, SHAPE_TYPE_ELLIPSE, &ellipse); }
  FOG_INLINE err_t clipEllipse(uint32_t clipOp, const EllipseD& ellipse) { return _vtable->clipShapeD(this, clipOp, SHAPE_TYPE_ELLIPSE, &ellipse); }

  FOG_INLINE err_t clipRound(uint32_t clipOp, const RoundF& round) { return _vtable->clipShapeF(this, clipOp, SHAPE_TYPE_ROUND, &round); }
  FOG_INLINE err_t clipRound(uint32_t clipOp, const RoundD& round) { return _vtable->clipShapeD(this, clipOp, SHAPE_TYPE_ROUND, &round); }

  FOG_INLINE err_t clipChord(uint32_t clipOp, const ArcF& chord) { return _vtable->clipShapeF(this, clipOp, SHAPE_TYPE_CHORD, &chord); }
  FOG_INLINE err_t clipChord(uint32_t clipOp, const ArcD& chord) { return _vtable->clipShapeD(this, clipOp, SHAPE_TYPE_CHORD, &chord); }

  FOG_INLINE err_t clipPie(uint32_t clipOp, const ArcF& pie) { return _vtable->clipShapeF(this, clipOp, SHAPE_TYPE_PIE, &pie); }
  FOG_INLINE err_t clipPie(uint32_t clipOp, const ArcD& pie) { return _vtable->clipShapeD(this, clipOp, SHAPE_TYPE_PIE, &pie); }

  FOG_INLINE err_t clipTriangle(uint32_t clipOp, const TriangleF& triangle) { return _vtable->clipShapeF(this, clipOp, SHAPE_TYPE_TRIANGLE, &triangle); }
  FOG_INLINE err_t clipTriangle(uint32_t clipOp, const TriangleD& triangle) { return _vtable->clipShapeD(this, clipOp, SHAPE_TYPE_TRIANGLE, &triangle); }

  FOG_INLINE err_t clipShape(uint32_t clipOp, const ShapeF& shape) { return _vtable->clipShapeF(this, clipOp, shape.getType(), shape.getData()); }
  FOG_INLINE err_t clipShape(uint32_t clipOp, const ShapeD& shape) { return _vtable->clipShapeD(this, clipOp, shape.getType(), shape.getData()); }

  FOG_INLINE err_t clipPath(uint32_t clipOp, const PathF& p) { return _vtable->clipPathF(this, clipOp, p); }
  FOG_INLINE err_t clipPath(uint32_t clipOp, const PathD& p) { return _vtable->clipPathD(this, clipOp, p); }

  FOG_INLINE err_t clipText(uint32_t clipOp, const PointI& p, const StringW& text, const Font& font, const RectI* clip = NULL) { return _vtable->clipTextAtI(this, clipOp, p, text, font, clip); }
  FOG_INLINE err_t clipText(uint32_t clipOp, const PointF& p, const StringW& text, const Font& font, const RectF* clip = NULL) { return _vtable->clipTextAtF(this, clipOp, p, text, font, clip); }
  FOG_INLINE err_t clipText(uint32_t clipOp, const PointD& p, const StringW& text, const Font& font, const RectD* clip = NULL) { return _vtable->clipTextAtD(this, clipOp, p, text, font, clip); }

  FOG_INLINE err_t clipText(uint32_t clipOp, const TextRectI& r, const StringW& text, const Font& font, const RectI* clip = NULL) { return _vtable->clipTextInI(this, clipOp, r, text, font, clip); }
  FOG_INLINE err_t clipText(uint32_t clipOp, const TextRectF& r, const StringW& text, const Font& font, const RectF* clip = NULL) { return _vtable->clipTextInF(this, clipOp, r, text, font, clip); }
  FOG_INLINE err_t clipText(uint32_t clipOp, const TextRectD& r, const StringW& text, const Font& font, const RectD* clip = NULL) { return _vtable->clipTextInD(this, clipOp, r, text, font, clip); }

  FOG_INLINE err_t clipMask(uint32_t clipOp, const PointI& p, const Image& mask) { return _vtable->clipMaskAtI(this, clipOp, p, mask, NULL); }
  FOG_INLINE err_t clipMask(uint32_t clipOp, const PointF& p, const Image& mask) { return _vtable->clipMaskAtF(this, clipOp, p, mask, NULL); }
  FOG_INLINE err_t clipMask(uint32_t clipOp, const PointD& p, const Image& mask) { return _vtable->clipMaskAtD(this, clipOp, p, mask, NULL); }

  FOG_INLINE err_t clipMask(uint32_t clipOp, const PointI& p, const Image& mask, const RectI& mFragment) { return _vtable->clipMaskAtI(this, clipOp, p, mask, &mFragment); }
  FOG_INLINE err_t clipMask(uint32_t clipOp, const PointF& p, const Image& mask, const RectI& mFragment) { return _vtable->clipMaskAtF(this, clipOp, p, mask, &mFragment); }
  FOG_INLINE err_t clipMask(uint32_t clipOp, const PointD& p, const Image& mask, const RectI& mFragment) { return _vtable->clipMaskAtD(this, clipOp, p, mask, &mFragment); }

  FOG_INLINE err_t clipMask(uint32_t clipOp, const RectI& r, const Image& mask) { return _vtable->clipMaskInI(this, clipOp, r, mask, NULL); }
  FOG_INLINE err_t clipMask(uint32_t clipOp, const RectF& r, const Image& mask) { return _vtable->clipMaskInF(this, clipOp, r, mask, NULL); }
  FOG_INLINE err_t clipMask(uint32_t clipOp, const RectD& r, const Image& mask) { return _vtable->clipMaskInD(this, clipOp, r, mask, NULL); }

  FOG_INLINE err_t clipMask(uint32_t clipOp, const RectI& r, const Image& mask, const RectI& mFragment) { return _vtable->clipMaskInI(this, clipOp, r, mask, &mFragment); }
  FOG_INLINE err_t clipMask(uint32_t clipOp, const RectF& r, const Image& mask, const RectI& mFragment) { return _vtable->clipMaskInF(this, clipOp, r, mask, &mFragment); }
  FOG_INLINE err_t clipMask(uint32_t clipOp, const RectD& r, const Image& mask, const RectI& mFragment) { return _vtable->clipMaskInD(this, clipOp, r, mask, &mFragment); }

  FOG_INLINE err_t clipRegion(uint32_t clipOp, const Region& r) { return _vtable->clipRegion(this, clipOp, r); }

  //! @brief Reset clipping.
  FOG_INLINE err_t resetClip() { return _vtable->resetClip(this); }

  // --------------------------------------------------------------------------
  // [Layer]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t beginLayer(uint32_t layerFlags) { return _vtable->beginLayer(this, layerFlags); }
  FOG_INLINE err_t endLayer() { return _vtable->endLayer(this); }

  // --------------------------------------------------------------------------
  // [Flush]
  // --------------------------------------------------------------------------

  //! @brief Flush painter, see @c PAINTER_FLUSH.
  FOG_INLINE err_t flush(uint32_t flags)
  {
    return _vtable->flush(this, flags);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The painter virtual-table (it's link to _engine->vtable for faster
  //! access).
  const PaintEngineVTable* _vtable;

  //! @brief The painter engine.
  PaintEngine* _engine;

private:
  _FOG_NO_COPY(Painter);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_PAINTER_H
