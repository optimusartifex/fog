// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_ANIMATION_H
#define _FOG_GUI_ANIMATION_H

#include <Fog/Build/Build.h>

#include <Fog/Gui/Constants.h>
#include <Fog/Gui/Event.h>
#include <Fog/Gui/GuiEngine.h>
#include <Fog/Gui/Widget.h>
#include <Fog/Core/Timer.h>
#include <Fog/Core/Time.h>

namespace Fog {
struct Application;

struct FOG_API Animation : public Object
{
  FOG_DECLARE_OBJECT(Animation, Object)

  Animation(uint32_t t=ANIMATION_FIXED_STEP);
  virtual ~Animation();

  void onStart() {
    //called from Dispatcher after animation was inserted into listener queue
    _starttime = TimeTicks::highResNow();
    if(_type == ANIMATION_FIXED_TIME) {
      _endtime = _starttime + getDuration();
      //store in _step the value for 1 ms for easy calculate of _position
      //based on elapsed time since start
      _step = (float) (1.0f / getDuration().inMilliseconds());
    }
  }

  virtual void onAnimation(AnimationEvent* e) {
    if(e->_type == EVENT_ANIMATION_STEP) {
      onStep(e);
    } else {
      onFinished(e);
    }
  }

  virtual void onTimer(TimerEvent* e);

  virtual void onStep(AnimationEvent* e) {

  }
  virtual void onFinished(AnimationEvent* e) {

  }

  FOG_INLINE float getStep() const { return _step; }
  FOG_INLINE float getFps() const { return _fps; }
  FOG_INLINE uint getDirection() const { return _direction; }
  FOG_INLINE TimeDelta getDuration() const { return _duration; }

  void setStep(float step) { _type = ANIMATION_FIXED_STEP; _step = step; }
  void setFps(float fps);
  void setDirection(uint direction) { _direction = direction; }
  void setDuration(TimeDelta duration) { _type = ANIMATION_FIXED_TIME;  _duration = duration; }

  FOG_EVENT_BEGIN()
    FOG_EVENT_DEF(EVENT_ANIMATION           , onAnimation        , AnimationEvent     , OVERRIDE)
  FOG_EVENT_END()

protected:
  float _position;
  float _step;
  float _fps;

  TimeDelta _duration;

  uint32_t _direction;
  uint32_t _type;

  TimeTicks _starttime;
  TimeTicks _endtime;
};


struct FOG_API AnimationDispatcher : public Object {
  FOG_DECLARE_OBJECT(AnimationDispatcher, Object)

  AnimationDispatcher(TimeDelta ms=TimeDelta::fromMilliseconds(20));
  ~AnimationDispatcher() {

  }

  //add an Animation to dispatcher
  //the dispatcher will take the ownership of the animation-object!
  void addAnimation(Animation* a);

  //remove an Animation from dispatcher
  //the dispatcher will destroy the object itself
  void removeAnimation(Animation* a);

  virtual void onTimer(TimerEvent* e);

private:
  int _count;   //number of registered animations
  Timer _timer; //Timer object for animations

  List<Animation*> _finished;
};


struct FOG_API WidgetAnimation : public Animation
{
  FOG_DECLARE_OBJECT(WidgetAnimation, Animation)

  WidgetAnimation(Widget* widget) : _widget(widget) {

  }
  virtual ~WidgetAnimation() {

  }

  FOG_INLINE Widget* getWidget() const { return _widget; }

  void setWidget(Widget* widget) { _widget = widget; };

protected:
  Widget* _widget;
};

#ifdef FOG_OS_WINDOWS
#pragma warning(disable: 4244) // float to int reduction
#endif

//Now rudimentary Animation-Implementation follows

struct FOG_API WidgetOpacityAnimation : public WidgetAnimation
{
  FOG_DECLARE_OBJECT(WidgetOpacityAnimation, WidgetAnimation)

  WidgetOpacityAnimation(Widget* widget) : WidgetAnimation(widget) {

  }

  virtual ~WidgetOpacityAnimation() {

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

struct FOG_API WidgetPositionAnimation : public WidgetAnimation
{
  FOG_DECLARE_OBJECT(WidgetPositionAnimation, WidgetAnimation)

  WidgetPositionAnimation(Widget* widget) : WidgetAnimation(widget) {

  }

  virtual ~WidgetPositionAnimation() {

  }

  virtual void onStep(AnimationEvent* e) {    
    if (_widget) {
      IntPoint p;

      p.setX((int)_startPosition.getX() + (_endPosition.getX() - _startPosition.getX()) * _position);
      p.setY((int)_startPosition.getY() + (_endPosition.getY() - _startPosition.getY()) * _position);

      _widget->setPosition(p);
    }
  }

  FOG_INLINE IntPoint getStartPosition() const { return _startPosition; }
  FOG_INLINE IntPoint getEndPosition() const { return _endPosition; }

  void setStartPosition(const IntPoint& start) { _startPosition = start; }
  void setEndPosition(const IntPoint& end) { _endPosition = end; }

protected:
  IntPoint _startPosition;
  IntPoint _endPosition;
};

struct FOG_API WidgetSizeAnimation : public WidgetAnimation
{
  FOG_DECLARE_OBJECT(WidgetSizeAnimation, WidgetAnimation)

    WidgetSizeAnimation(Widget* widget) : WidgetAnimation(widget) {

  }

  virtual ~WidgetSizeAnimation() {

  }

  virtual void onStep(AnimationEvent* e) {    
    if (_widget) {
      IntSize p;

      p.setWidth((int)_start.getWidth() + (_end.getWidth() - _start.getWidth()) * _position);
      p.setHeight((int)_start.getHeight() + (_end.getHeight() - _start.getHeight()) * _position);

      _widget->setSize(p);
    }
  }

  FOG_INLINE IntSize getStartPosition() const { return _start; }
  FOG_INLINE IntSize getEndPosition() const { return _end; }

  void setStartPosition(const IntSize& start) { _start = start; }
  void setEndPosition(const IntSize& end) { _end = end; }

protected:
  IntSize _start;
  IntSize _end;
};


struct FOG_API WidgetGeometryAnimation : public WidgetAnimation
{
  FOG_DECLARE_OBJECT(WidgetGeometryAnimation, WidgetAnimation)

    WidgetGeometryAnimation(Widget* widget) : WidgetAnimation(widget) {

  }

  virtual ~WidgetGeometryAnimation() {

  }

  virtual void onStep(AnimationEvent* e) {    
    if (_widget) {
      IntRect p;

      p.setX((int)_start.getX() + (_end.getX() - _start.getX()) * _position);
      p.setY((int)_start.getY() + (_end.getY() - _start.getY()) * _position);

      p.setWidth((int)_start.getWidth() + (_end.getWidth() - _start.getWidth()) * _position);
      p.setHeight((int)_start.getHeight() + (_end.getHeight() - _start.getHeight()) * _position);

      _widget->setGeometry(p);
    }
  }

  FOG_INLINE IntRect getStartPosition() const { return _start; }
  FOG_INLINE IntRect getEndPosition() const { return _end; }

  void setStartPosition(const IntRect& start) { _start = start; }
  void setEndPosition(const IntRect& end) { _end = end; }

protected:
  IntRect _start;
  IntRect _end;
};

}

#endif