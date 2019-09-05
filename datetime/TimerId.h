#ifndef LIBCPP_TIMER_ID_H_
#define LIBCPP_TIMER_ID_H_

namespace libcpp
{
  
class Timer;

class TimerId
{
public:
  explicit TimerId(Timer* timer)
    : timer_(timer)
  {}

private:
  Timer* timer_;
};

}

#endif