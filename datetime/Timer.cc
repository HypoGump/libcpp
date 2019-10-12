#include "Timer.h"

using namespace libcpp;

std::atomic<long long> Timer::s_numCreated_;


void Timer::restart(TimeStamp now)
{
  if (repeat_) {
    expiration_ = addTime(now, intervalSeconds_);
  }
  else {
    expiration_ = TimeStamp::invalid();
  }
}