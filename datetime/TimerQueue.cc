#include "TimerQueue.h"
#include "reactor/EventLoop.h"

#include "logging/Logging.h"

#include <sys/timerfd.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

namespace libcpp
{
namespace detail
{
  
struct timespec howMuchTimeFromNow(TimeStamp when)
{
  int64_t microSeconds = when.microSecondsSinceEpoch() 
                            - TimeStamp::now().microSecondsSinceEpoch();
  if (microSeconds < 100) {
    microSeconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(microSeconds 
                                  / TimeStamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
          (microSeconds % TimeStamp::kMicroSecondsPerSecond) * 1000);
  return ts;
}

int createTimerfd()
{
  int fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (fd < 0) {
    LOG_FATAL << "[TimerQueue] timerfd_create() failed";
  }
  return fd;
}

void resetTimerfd(int timerfd, TimeStamp expiration)
{
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof newValue);
  bzero(&oldValue, sizeof oldValue);
  newValue.it_value = howMuchTimeFromNow(expiration);
  /*
   * 0 means relative timer, TFD_TIMER_ABSTIME is absolute timer
   */
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if (ret) {
    LOG_ERROR << "[TimerQueue] timerfd_settime() failed";
  }
}

void readTimerfd(int timerfd, TimeStamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
  LOG_TRACE << "[TimerQueue] read timerfd timeout " << howmany 
            << " times at " << now.toString();
  if (n != sizeof(howmany)) {
    LOG_ERROR << "[TimerQueue] read " << n << "bytes instead of 8";
  }
}

} // detail
} // libcpp

using namespace libcpp;
using namespace libcpp::detail;

TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timerfd_(createTimerfd()),
    timerfdChannel_(loop, timerfd_),
    timers_()
{
  timerfdChannel_.setReadCallback(
            std::bind(&TimerQueue::handleRead, this));
  timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
  ::close(timerfd_);
  for (auto& tentry : timers_) {
    delete tentry.second;
  }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, TimeStamp when, double interval)
{
  Timer* timer = new Timer(cb, when, interval);
  loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
  return TimerId(timer);
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
  bool earliestChanged = insert(timer);
  if (earliestChanged) {
    resetTimerfd(timerfd_, timer->expiration());
  }
}

bool TimerQueue::insert(Timer* timer)
{
  bool earliestChanged = false;
  TimerList::iterator it = timers_.begin();
  TimeStamp when = timer->expiration();
  if (it == timers_.end() || when < it->first) {
    earliestChanged = true;
  }
  std::pair<TimerList::iterator, bool> ret = 
                      timers_.insert(std::make_pair(when, timer));
  assert(ret.second == true);
  return earliestChanged;
}


void TimerQueue::handleRead()
{
  loop_->assertInLoopThread();
  TimeStamp now(TimeStamp::now());
  readTimerfd(timerfd_, now);
  
  std::vector<TimerEntry> expired = getExpired(now);
  for (auto& tentry : expired) {
    tentry.second->run();
  }
  
  reset(expired, now);
}

std::vector<TimerQueue::TimerEntry> TimerQueue::getExpired(TimeStamp now)
{
  std::vector<TimerEntry> expired;
  TimerEntry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
  TimerList::iterator it = timers_.lower_bound(sentry); // FIXME: upper_bound?
  assert(it == timers_.end() || now < it->first);
  std::copy(timers_.begin(), it, back_inserter(expired));
  timers_.erase(timers_.begin(), it);
  return expired;
}

void TimerQueue::reset(std::vector<TimerEntry>& expired, TimeStamp now)
{
  TimeStamp nextExpired;
  for(auto& tentry : expired) {
    if (tentry.second->repeat()) {
      tentry.second->restart(now);
      insert(tentry.second);
    }
    else {
      delete tentry.second;
    }
  }
  
  if (!timers_.empty()) {
    nextExpired = timers_.begin()->second->expiration();
  }
  
  if (nextExpired.valid()) {
    resetTimerfd(timerfd_, nextExpired);
  }
}
