#ifndef LIBCPP_TIMER_H_
#define LIBCPP_TIMER_H_

#include "TimeStamp.h"
#include <functional>
#include <atomic>
#include <stdint.h>

namespace libcpp
{
using TimerCallback = std::function<void()>;

class Timer
{
public:
  Timer(const TimerCallback& cb, TimeStamp when, double interval)
    : callback_(cb),
      expiration_(when),
      intervalSeconds_(interval),
      repeat_(interval > 0.0),
      sequence_(s_numCreated_++)
  {
  }
  
  void run() const { callback_(); }
  
  TimeStamp expiration() const { return expiration_; }
  int64_t sequence() const { return sequence_; }
  bool repeat() const { return repeat_; }
  
  void restart(TimeStamp now);
  
private:
  const TimerCallback callback_;
  TimeStamp expiration_;
  const double intervalSeconds_;
  const bool repeat_;
  const int64_t sequence_;
  
  static std::atomic<long long> s_numCreated_;
};

}

#endif