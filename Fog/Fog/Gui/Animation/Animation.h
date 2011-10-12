// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_ANIMATION_ANIMATION_H
#define _FOG_GUI_ANIMATION_ANIMATION_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Kernel/Timer.h>
#include <Fog/Core/Tools/Time.h>
#include <Fog/Gui/Engine/GuiEngine.h>
#include <Fog/Gui/Widget/Event.h>
#include <Fog/Gui/Widget/Widget.h>

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Animation;

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
  _FOG_NO_COPY(AnimationDispatcher)
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
  FOG_INLINE uint32_t getDirection() const { return _direction; }
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

  float _position;
  float _step;
  float _fps;

  TimeDelta _duration;

  uint32_t _direction;
  uint32_t _type;

  TimeTicks _starttime;
  TimeTicks _endtime;

private:
  _FOG_NO_COPY(Animation)
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

  WidgetAnimation(Widget* widget = NULL, uint32_t flags = ANIMATION_WIDGET_NO_FLAGS, uint32_t visibility = WIDGET_VISIBLE);
  virtual ~WidgetAnimation();

  // --------------------------------------------------------------------------
  // [Event Handlers]
  // --------------------------------------------------------------------------

  virtual void onStart();
  virtual void onFinished(AnimationEvent* e);

  FOG_INLINE Widget* getWidget() const { return _widget; }
  FOG_INLINE void setWidget(Widget* widget) { _widget = widget; };

  FOG_INLINE uint32_t getFlags() const { return _flags; }
  FOG_INLINE void setFlags(uint32_t flags) { _flags = flags; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Widget* _widget;
  uint32_t _flags;

  //! @brief To make possible to show widget in fullscreen/minimized/maximized mode.
  uint32_t _visibility;
};

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

  WidgetOpacityAnimation(Widget* widget);
  virtual ~WidgetOpacityAnimation();

  virtual void onStart();
  virtual void onStep(AnimationEvent* e);

  FOG_INLINE float getStartOpacity() const { return _startOpacity; }
  FOG_INLINE float getEndOpacity() const { return _endOpacity; }

  FOG_INLINE void setStartOpacity(float startOpacity) { _startOpacity = startOpacity; }
  FOG_INLINE void setEndOpacity(float endOpacity) { _endOpacity = endOpacity; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

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

  WidgetPositionAnimation(Widget* widget);
  ~WidgetPositionAnimation();

  virtual void onStart();
  virtual void onStep(AnimationEvent* e);

  FOG_INLINE const PointI& getStartPosition() const { return _start; }
  FOG_INLINE const PointI& getEndPosition() const { return _end; }

  FOG_INLINE void setStartPosition(const PointI& start) { _start = start; }
  FOG_INLINE void setEndPosition(const PointI& end) { _end = end; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PointI _start;
  PointI _end;
};

// ============================================================================
// [Fog::WidgetSizeAnimation]
// ============================================================================

struct FOG_API WidgetSizeAnimation : public WidgetAnimation
{
  FOG_DECLARE_OBJECT(WidgetSizeAnimation, WidgetAnimation)

  WidgetSizeAnimation(Widget* widget);
  virtual ~WidgetSizeAnimation();

  virtual void onStart();
  virtual void onStep(AnimationEvent* e);

  FOG_INLINE const SizeI& getStartPosition() const { return _start; }
  FOG_INLINE const SizeI& getEndPosition() const { return _end; }

  FOG_INLINE void setStartPosition(const SizeI& start) { _start = start; }
  FOG_INLINE void setEndPosition(const SizeI& end) { _end = end; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  SizeI _start;
  SizeI _end;
};

// ============================================================================
// [Fog::WidgetGeometryAnimation]
// ============================================================================

struct FOG_API WidgetGeometryAnimation : public WidgetAnimation
{
  FOG_DECLARE_OBJECT(WidgetGeometryAnimation, WidgetAnimation)

  WidgetGeometryAnimation(Widget* widget);
  virtual ~WidgetGeometryAnimation();

  virtual void onStart();
  virtual void onStep(AnimationEvent* e);

  FOG_INLINE const RectI& getStartPosition() const { return _start; }
  FOG_INLINE const RectI& getEndPosition() const { return _end; }

  FOG_INLINE void setStartPosition(const RectI& start) { _start = start; }
  FOG_INLINE void setEndPosition(const RectI& end) { _end = end; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RectI _start;
  RectI _end;
};

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_ANIMATION_ANIMATION_H
