// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Animation.h>
#include <Fog/Core/Application.h>
#include <Fog/Core.h>

FOG_IMPLEMENT_OBJECT(Fog::Animation)
FOG_IMPLEMENT_OBJECT(Fog::AnimationDispatcher)
FOG_IMPLEMENT_OBJECT(Fog::WidgetAnimation)
FOG_IMPLEMENT_OBJECT(Fog::WidgetOpacityAnimation)

namespace Fog {

Animation::Animation(uint32_t t) : _type(t), _step(10), _duration(TimeDelta::fromMilliseconds(200)), _position(0.0f) {
  
}

Animation::~Animation() {

}

void Animation::onTimer(TimerEvent* e)
{
  if(_position >= 1.0) {
    //Animation is finished! should not happen
    return;
  }

  bool finished = false;

  if(_type == ANIMATION_FIXED_STEP) {
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
  } else {
    //dynamically calculate position based on time elapsed since start    
    TimeTicks now = TimeTicks::highResNow();
    if(now >= _endtime) {
      if(_direction == ANIMATION_BACKWARD) {
        _position = 0.0f;
      } else {
        _position = 1.0f;
      }
      finished = true;
    } else {
      TimeDelta delta = now - _starttime;
      //in _step the value for one ms is stored
      _position = delta.inMilliseconds() * _step;

      if(_position > 1.0f) {
        _position = 1.0f;
      }

      if(_direction == ANIMATION_BACKWARD) {
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

void AnimationDispatcher::onTimer(TimerEvent* e) {
  //notify all registered handler
  sendEvent(e);
  //or directly call doUpdate() ??  

  while(_finished.getLength() > 0) {
    Animation* e = _finished.takeFirst();
    //who is responsible to destruct?
    //current answer: call destroy on Animation
    removeListener(e);
    e->destroy();
  }

 Application::getInstance()->getGuiEngine()->update();
}

AnimationDispatcher::AnimationDispatcher(TimeDelta ms) : _count(0) {
  _timer.setInterval(ms);
  _timer.addListener(EVENT_TIMER,this,&AnimationDispatcher::onTimer);
  //No local onTimer-Event
  //The Time Events are dispatched directly to AnimationInstances
}

void AnimationDispatcher::addAnimation(Animation* a) {
  addListener(EVENT_TIMER,a,&Animation::onTimer);
  ++_count;
  if(_count == 1) {
    //So the Timer is only running if a animation is in there
    _timer.start();
  }

  //To set up start parameter, or should we call this on first time event 
  //or maybe on construction?
  a->onStart();
}

void AnimationDispatcher::removeAnimation(Animation* e) {
  _finished.append(e);  
  --_count;
  if(_count == 0) {
    _timer.stop();
  }
}

}