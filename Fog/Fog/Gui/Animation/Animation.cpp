// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Gui/Animation/Animation.h>

FOG_IMPLEMENT_OBJECT(Fog::Animation)
FOG_IMPLEMENT_OBJECT(Fog::AnimationDispatcher)
FOG_IMPLEMENT_OBJECT(Fog::WidgetAnimation)
FOG_IMPLEMENT_OBJECT(Fog::WidgetOpacityAnimation)
FOG_IMPLEMENT_OBJECT(Fog::WidgetPositionAnimation)
FOG_IMPLEMENT_OBJECT(Fog::WidgetGeometryAnimation)
FOG_IMPLEMENT_OBJECT(Fog::WidgetSizeAnimation)

namespace Fog {

// ============================================================================
// [Fog::AnimationDispatcher]
// ============================================================================

AnimationDispatcher::AnimationDispatcher(TimeDelta ms) :
  _count(0)
{
  _timer.setInterval(ms);
  _timer.addListener(EVENT_TIMER,this,&AnimationDispatcher::onTimer);
  //No local onTimer-Event
  //The Time Events are dispatched directly to AnimationInstances
}

AnimationDispatcher::~AnimationDispatcher()
{
}

void AnimationDispatcher::addAnimation(Animation* a)
{
  addListener(EVENT_TIMER,a,&Animation::onTimer);
  ++_count;
  if (_count == 1)
  {
    //So the Timer is only running if a animation is in there
    _timer.start();
  }

  //To set up start parameter, or should we call this on first time event
  //or maybe on construction?
  a->onStart();
}

void AnimationDispatcher::removeAnimation(Animation* e)
{
  _finished.append(e);
  --_count;

  if (_count == 0)
  {
    _timer.stop();
  }
}

void AnimationDispatcher::onTimer(TimerEvent* e)
{
  // Notify all registered handlers.
  sendEvent(e);

  // TODO: Or directly call doUpdate() ??

  while (_finished.getLength() > 0)
  {
    Animation* e = _finished.getFirst();
    _finished.removeFirst();

    // TODO:
    // Who is responsible to destruct?
    // Current answer: call destroy on Animation
    removeListener(e);
    e->release();
  }

  Application::getInstance()->getGuiEngine()->update();
}

// ============================================================================
// [Fog::Animation]
// ============================================================================

Animation::Animation(uint32_t t) :
  _position(0.0f),
  _step(10),
  _duration(TimeDelta::fromMilliseconds(200)),
  _direction(ANIMATION_FORWARD),
  _type(t)
{
}

Animation::~Animation()
{
}

void Animation::onStart()
{
  //called from Dispatcher after animation was inserted into listener queue
  _starttime = TimeTicks::now(TICKS_PRECISION_HIGH);

  if (_type == ANIMATION_FIXED_TIME)
  {
    _endtime = _starttime + getDuration();
    // Store in _step the value for 1 ms for easy calculation of _position
    // based on elapsed time since start.
    _step = 1.0f / getDuration().getMillisecondsF();
  }
}

void Animation::onAnimation(AnimationEvent* e)
{
  if (e->_type == EVENT_ANIMATION_STEP)
  {
    onStep(e);
  }
  else
  {
    onFinished(e);
  }
}

void Animation::onTimer(TimerEvent* e)
{
  if (_position >= 1.0)
  {
    //Animation is finished! should not happen
    return;
  }

  bool finished = false;

  if (_type == ANIMATION_FIXED_STEP)
  {
    if (_direction == ANIMATION_FORWARD)
    {
      _position += _step;
      if (_position >= 1.0f) { _position = 1.0f; finished = true; }
    }
    else
    {
      _position -= _step;
      if (_position <= 0.0f) { _position = 0.0f; finished = true; }
    }
  }
  else
  {
    // Dynamically calculate position based on time elapsed since start.
    TimeTicks now = TimeTicks::now(TICKS_PRECISION_HIGH);
    if (now >= _endtime)
    {
      if (_direction == ANIMATION_BACKWARD)
      {
        _position = 0.0f;
      }
      else
      {
        _position = 1.0f;
      }
      finished = true;
    }
    else
    {
      TimeDelta delta = now - _starttime;
      //in _step the value for one ms is stored
      _position = _step * delta.getMillisecondsF();

      if (_position > 1.0f)
      {
        _position = 1.0f;
      }

      if (_direction == ANIMATION_BACKWARD)
      {
        _position = 1.0f - _position;
      }
    }
  }

  // Call onAnimation() and onFinished() if animation is finished.
  AnimationEvent event;

  event._type = EVENT_ANIMATION_STEP;
  sendEvent(&event);

  if (finished)
  {
    event._type = EVENT_ANIMATION_FINISHED;
    sendEvent(&event);

    //should we call this here? Or call it at destructor?
    Application::getInstance()->getAnimationDispatcher()->removeAnimation(this);
  }
}

void Animation::onStep(AnimationEvent* e)
{
}

void Animation::onFinished(AnimationEvent* e)
{
}

// ============================================================================
// [Fog::WidgetAnimation]
// ============================================================================

WidgetAnimation::WidgetAnimation(Widget* widget, uint32_t flags, uint32_t visibility) :
  _widget(widget),
  _visibility(visibility)
{
  // Call here because method may handle wrong flag settings
  setFlags(flags);
}

WidgetAnimation::~WidgetAnimation()
{
}

void WidgetAnimation::onStart()
{
  Animation::onStart();

  if (_widget)
  {
    if (_flags & ANIMATION_WIDGET_SHOW_ON_START)
    {
      _widget->show(_visibility);
    }
  }
}

void WidgetAnimation::onFinished(AnimationEvent* e)
{
  Animation::onFinished(e);

  if (_widget)
  {
    if (_flags & ANIMATION_WIDGET_HIDE_ON_END)
    {
      _widget->hide();
    }

    if (_flags & ANIMATION_WIDGET_DESTROY_ON_END)
    {
      _widget->release();
    }
  }

  _widget = NULL;
}

// ============================================================================
// [Fog::WidgetOpacityAnimation]
// ============================================================================

WidgetOpacityAnimation::WidgetOpacityAnimation(Widget* widget) :
  WidgetAnimation(widget)
{
}

WidgetOpacityAnimation::~WidgetOpacityAnimation()
{
}

void WidgetOpacityAnimation::onStart()
{
  if (_widget)
  {
    _widget->setTransparency(getDirection() == ANIMATION_FORWARD? _startOpacity : _endOpacity);
  }

  // Will show Widget if the flag is set.
  WidgetAnimation::onStart();
}

void WidgetOpacityAnimation::onStep(AnimationEvent* e)
{
  float opacity = _startOpacity + (_endOpacity - _startOpacity) * _position;

  if (_widget)
  {
    _widget->setTransparency(opacity);
  }
}

// ============================================================================
// [Fog::WidgetPositionAnimation]
// ============================================================================

WidgetPositionAnimation::WidgetPositionAnimation(Widget* widget) :
  WidgetAnimation(widget)
{
}

WidgetPositionAnimation::~WidgetPositionAnimation()
{
}

void WidgetPositionAnimation::onStart()
{
  if (_widget)
  {
    _widget->setPosition(getDirection() == ANIMATION_FORWARD? _start : _end);
  }

  // Will show Widget if the flag is set.
  WidgetAnimation::onStart();
}

void WidgetPositionAnimation::onStep(AnimationEvent* e)
{
  if (_widget)
  {
    PointI p;

    p.setX((int)(double(_start.getX()) + double(_end.getX() - _start.getX()) * _position));
    p.setY((int)(double(_start.getY()) + double(_end.getY() - _start.getY()) * _position));

    _widget->setPosition(p);
  }
}

// ============================================================================
// [Fog::WidgetSizeAnimation]
// ============================================================================

WidgetSizeAnimation::WidgetSizeAnimation(Widget* widget) :
  WidgetAnimation(widget)
{
}

WidgetSizeAnimation::~WidgetSizeAnimation()
{
}

void WidgetSizeAnimation::onStart()
{
  if (_widget)
  {
    _widget->setSize(getDirection() == ANIMATION_FORWARD? _start : _end);
  }

  // Will show Widget if the flag is set.
  WidgetAnimation::onStart();
}

void WidgetSizeAnimation::onStep(AnimationEvent* e)
{
  if (_widget)
  {
    SizeI p;

    p.setWidth((int)(double(_start.getWidth()) + double(_end.getWidth() - _start.getWidth()) * _position));
    p.setHeight((int)(double(_start.getHeight()) + double(_end.getHeight() - _start.getHeight()) * _position));

    _widget->setSize(p);
  }
}

// ============================================================================
// [Fog::WidgetGeometryAnimation]
// ============================================================================

WidgetGeometryAnimation::WidgetGeometryAnimation(Widget* widget) : WidgetAnimation(widget)
{
}

WidgetGeometryAnimation::~WidgetGeometryAnimation()
{
}

void WidgetGeometryAnimation::onStart()
{
  if (_widget)
  {
    _widget->setGeometry(getDirection() == ANIMATION_FORWARD? _start : _end);
  }

  //will show Widget if the flag is set.
  WidgetAnimation::onStart();
}

void WidgetGeometryAnimation::onStep(AnimationEvent* e)
{
  if (_widget)
  {
    RectI p;

    p.setX((int)(double(_start.getX()) + double(_end.getX() - _start.getX()) * _position));
    p.setY((int)(double(_start.getY()) + double(_end.getY() - _start.getY()) * _position));

    p.setWidth((int)(double(_start.getWidth()) + double(_end.getWidth() - _start.getWidth()) * _position));
    p.setHeight((int)(double(_start.getHeight()) + double(_end.getHeight() - _start.getHeight()) * _position));

    _widget->setGeometry(p);
  }
}

} // Fog namespace
