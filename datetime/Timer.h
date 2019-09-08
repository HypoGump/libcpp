#ifndef LIBCPP_TIMER_H_
#define LIBCPP_TIMER_H_

#include "TimeStamp.h"
#include <functional>

namespace libcpp
{
typedef std::function<void()> TimerCallback;

class Timer
{
public:
  Timer(const TimerCallback& cb, TimeStamp when, double interval)
    : callback_(cb),
      expiration_(when),
      intervalSeconds_(interval),
      repeat_(interval > 0.0)
  {
  }
  
  void run() const { callback_(); }
  
  TimeStamp expiration() const { return expiration_; }
  bool repeat() const { return repeat_; }
  
  void restart(TimeStamp now);
  
private:
  const TimerCallback callback_;
  TimeStamp expiration_;
  const double intervalSeconds_;
  bool repeat_;
};

}

#endif