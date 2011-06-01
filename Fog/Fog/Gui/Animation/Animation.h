// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_ANIMATION_ANIMATION_H
#define _FOG_GUI_ANIMATION_ANIMATION_H

// [Dependencies]
#include <Fog/Core/DateTime/Time.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/System/Object.h>
#include <Fog/Core/System/Timer.h>
#include <Fog/Gui/Global/Constants.h>
#include <Fog/Gui/Engine/GuiEngine.h>
#include <Fog/Gui/Widget/Event.h>
#include <Fog/Gui/Widget/Widget.h>

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Animation;
struct Application;

// ============================================================================
// [Fog::AnimationDispatcher]
// ============================================================================

//! @brief Animation dispatcher.
struct FOG_API AnimationDispatcher : public Object
{
  FOG_DECLARE_OBJECT(AnimationDispatcher, Object)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  AnimationDispatcher(TimeDelta ms = TimeDelta::fromMilliseconds(20));
  virtual ~AnimationDispatcher();

  // --------------------------------------------------------------------------
  // [Animation Management]
  // --------------------------------------------------------------------------

  //! @brief Add an Animation to dispatcher.
  //!
  //! The dispatcher will take the ownership of the animation-object.
  void addAnimation(Animation* a);

  //! @brief Remove an Animation from dispatcher.
  //!
  //! The dispatcher will destroy the object itself
  void removeAnimation(Animation* a);

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  //! @brief Timer event.
  virtual void onTimer(TimerEvent* e);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief Number of registered animations.
  int _count;
  //! @brief Timer object for animations.
  Timer _timer;
  //! @brief List of finished animations.
  List<Animation*> _finished;

private:
  _FOG_CLASS_NO_COPY(AnimationDispatcher)
};

// ============================================================================
// [Fog::Animation]
// ============================================================================

//! @brief Animation base class.
struct FOG_API Animation : public Object
{
  FOG_DECLARE_OBJECT(Animation, Object)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Animation(uint32_t t = ANIMATION_FIXED_STEP);
  virtual ~Animation();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getStep() const { return _step; }
  FOG_INLINE float getFps() const { return _fps; }
  FOG_INLINE uint getDirection() const { return _direction; }
  FOG_INLINE TimeDelta getDuration() const { return _duration; }

  void setStep(float step) { _type = ANIMATION_FIXED_STEP; _step = step; }
  void setFps(float fps);
  void setDirection(uint direction) { _direction = direction; }
  void setDuration(TimeDelta duration) { _type = ANIMATION_FIXED_TIME;  _duration = duration; }

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  // ANIMATION TODO: Why onStart is not called through an event dispatcher?
  virtual void onStart();
  virtual void onAnimation(AnimationEvent* e);
  virtual void onTimer(TimerEvent* e);
  virtual void onStep(AnimationEvent* e);
  virtual void onFinished(AnimationEvent* e);

  FOG_EVENT_BEGIN()
    FOG_EVENT_DEF(EVENT_ANIMATION           , onAnimation        , AnimationEvent     , OVERRIDE)
  FOG_EVENT_END()

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  float _position;
  float _step;
  float _fps;

  TimeDelta _duration;

  uint32_t _direction;
  uint32_t _type;

  TimeTicks _starttime;
  TimeTicks _endtime;

private:
  _FOG_CLASS_NO_COPY(Animation)
};

// ============================================================================
// [Fog::WidgetAnimation]
// ============================================================================

struct FOG_API WidgetAnimation : public Animation
{
  FOG_DECLARE_OBJECT(WidgetAnimation, Animation)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  // TODO: Move to .cpp
  WidgetAnimation(Widget* widget = NULL, uint32_t flags = ANIMATION_WIDGET_NO_FLAGS, uint32_t visibility = WIDGET_VISIBLE);
  virtual ~WidgetAnimation();

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  virtual void onStart();
  virtual void onFinished(AnimationEvent* e);

  FOG_INLINE Widget* getWidget() const { return _widget; }
  void setWidget(Widget* widget) { _widget = widget; };
  uint32_t getFlags() const { return _flags; }
  void setFlags(uint32_t f) { _flags = f; }

protected:
  Widget* _widget;
  uint32_t _flags;
  //! @brief To make possible to show widget in fullscreen/minimized/maximized
  //! mode.
  uint32_t _visibility;
};

// GUI TODO: Remove this hack.
#ifdef FOG_OS_WINDOWS
#pragma warning(disable: 4244) // float to int reduction
#endif

// ============================================================================
// [Fog::WidgetOpacityAnimation]
// ============================================================================

//Now rudimentary Animation-Implementation follows

struct FOG_API WidgetOpacityAnimation : public WidgetAnimation
{
  FOG_DECLARE_OBJECT(WidgetOpacityAnimation, WidgetAnimation)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WidgetOpacityAnimation(Widget* widget) : WidgetAnimation(widget) {
  }

  virtual ~WidgetOpacityAnimation() {
  }

  virtual void onStart() {
    if (_widget) {
      _widget->setTransparency(getDirection() == ANIMATION_FORWARD? _startOpacity : _endOpacity);
    }

    //will show Widget if the flag is set.
    WidgetAnimation::onStart();
  }

  virtual void onStep(AnimationEvent* e) {
    float opacity = _startOpacity + (_endOpacity - _startOpacity) * _position;
    if (_widget) {
      _widget->setTransparency(opacity);
    }
  }

  FOG_INLINE float getStartOpacity() const { return _startOpacity; }
  FOG_INLINE float getEndOpacity() const { return _endOpacity; }

  void setStartOpacity(float startOpacity) { _startOpacity = startOpacity; }
  void setEndOpacity(float endOpacity) { _endOpacity = endOpacity; }

protected:
  float _startOpacity;
  float _endOpacity;
};

// TODO: Move implementation to .cpp,
// TODO: Shouldn't we join all of these, I think that WidgetGeometryAnimation
// will be enough.

// ============================================================================
// [Fog::WidgetPositionAnimation]
// ============================================================================

struct FOG_API WidgetPositionAnimation : public WidgetAnimation
{
  FOG_DECLARE_OBJECT(WidgetPositionAnimation, WidgetAnimation)

  WidgetPositionAnimation(Widget* widget) : WidgetAnimation(widget) {

  }

  virtual ~WidgetPositionAnimation() {

  }

  virtual void onStart() {
    if (_widget) {
      _widget->setPosition(getDirection() == ANIMATION_FORWARD? _start : _end);
    }

    //will show Widget if the flag is set.
    WidgetAnimation::onStart();
  }

  virtual void onStep(AnimationEvent* e) {
    if (_widget) {
      PointI p;

      p.setX((int)_start.getX() + (_end.getX() - _start.getX()) * _position);
      p.setY((int)_start.getY() + (_end.getY() - _start.getY()) * _position);

      _widget->setPosition(p);
    }
  }

  FOG_INLINE PointI getStartPosition() const { return _start; }
  FOG_INLINE PointI getEndPosition() const { return _end; }

  void setStartPosition(const PointI& start) { _start = start; }
  void setEndPosition(const PointI& end) { _end = end; }

protected:
  PointI _start;
  PointI _end;
};

// ============================================================================
// [Fog::WidgetSizeAnimation]
// ============================================================================

struct FOG_API WidgetSizeAnimation : public WidgetAnimation
{
  FOG_DECLARE_OBJECT(WidgetSizeAnimation, WidgetAnimation)

    WidgetSizeAnimation(Widget* widget) : WidgetAnimation(widget) {

  }

  virtual ~WidgetSizeAnimation() {

  }

  virtual void onStart() {
    if (_widget) {
      _widget->setSize(getDirection() == ANIMATION_FORWARD? _start : _end);
    }

    //will show Widget if the flag is set.
    WidgetAnimation::onStart();
  }

  virtual void onStep(AnimationEvent* e) {
    if (_widget) {
      SizeI p;

      p.setWidth((int)_start.getWidth() + (_end.getWidth() - _start.getWidth()) * _position);
      p.setHeight((int)_start.getHeight() + (_end.getHeight() - _start.getHeight()) * _position);

      _widget->setSize(p);
    }
  }

  FOG_INLINE SizeI getStartPosition() const { return _start; }
  FOG_INLINE SizeI getEndPosition() const { return _end; }

  void setStartPosition(const SizeI& start) { _start = start; }
  void setEndPosition(const SizeI& end) { _end = end; }

protected:
  SizeI _start;
  SizeI _end;
};

// ============================================================================
// [Fog::WidgetGeometryAnimation]
// ============================================================================

struct FOG_API WidgetGeometryAnimation : public WidgetAnimation
{
  FOG_DECLARE_OBJECT(WidgetGeometryAnimation, WidgetAnimation)

    WidgetGeometryAnimation(Widget* widget) : WidgetAnimation(widget) {

  }

  virtual ~WidgetGeometryAnimation() {

  }

  virtual void onStart() {
    if (_widget) {
      _widget->setGeometry(getDirection() == ANIMATION_FORWARD? _start : _end);
    }

    //will show Widget if the flag is set.
    WidgetAnimation::onStart();
  }

  virtual void onStep(AnimationEvent* e) {
    if (_widget) {
      RectI p;

      p.setX((int)_start.getX() + (_end.getX() - _start.getX()) * _position);
      p.setY((int)_start.getY() + (_end.getY() - _start.getY()) * _position);

      p.setWidth((int)_start.getWidth() + (_end.getWidth() - _start.getWidth()) * _position);
      p.setHeight((int)_start.getHeight() + (_end.getHeight() - _start.getHeight()) * _position);

      _widget->setGeometry(p);
    }
  }

  FOG_INLINE RectI getStartPosition() const { return _start; }
  FOG_INLINE RectI getEndPosition() const { return _end; }

  void setStartPosition(const RectI& start) { _start = start; }
  void setEndPosition(const RectI& end) { _end = end; }

protected:
  RectI _start;
  RectI _end;
};

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_ANIMATION_ANIMATION_H
