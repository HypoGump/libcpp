#ifndef LIBCPP_TIMER_QUEUE_H_
#define LIBCPP_TIMER_QUEUE_H_

#include "Timer.h"
#include "TimerId.h"

#include "reactor/Channel.h"

#include <set>
#include <vector>

namespace libcpp
{
class EventLoop;
class Channel;

class TimerQueue
{
public:
  TimerQueue(EventLoop* loop);
  ~TimerQueue();
  
  TimerId addTimer(const TimerCallback& cb, TimeStamp when, double interval);
  
private:
  typedef std::pair<TimeStamp, Timer*> TimerEntry;
  typedef std::set<TimerEntry> TimerList;
  
  void handleRead();
  std::vector<TimerEntry> getExpired(TimeStamp now);
  void reset(std::vector<TimerEntry>& expired, TimeStamp now);
  
  bool insert(Timer* timer);
  
  EventLoop* loop_;
  const int timerfd_;
  Channel timerfdChannel_;
  TimerList timers_;
};

}

#endif