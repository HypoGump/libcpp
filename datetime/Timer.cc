#include "Timer.h"

using namespace libcpp;

void Timer::restart(TimeStamp now)
{
  if (repeat_) {
    expiration_ = addTime(now, intervalSeconds_);
  }
  else {
    expiration_ = TimeStamp::invalid();
  }
}