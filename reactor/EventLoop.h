#ifndef LIBCPP_EVENTLOOP_H_
#define LIBCPP_EVENTLOOP_H_

#include "datetime/TimerQueue.h"

#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <functional>

namespace libcpp
{
class Channel;
class Poller;

class EventLoop
{
public:
  typedef std::function<void()> Functor;
  EventLoop();
  ~EventLoop();
  
  void loop();
  void quit();
  
  void runInLoop(const Functor& cb);
  void queueInLoop(const Functor& cb);
  void wakeup();
  
  TimerId runAt(const TimeStamp& time, const TimerCallback& cb);
  TimerId runAfter(double delay, const TimerCallback& cb);
  TimerId runEvery(double interval, const TimerCallback& cb);
  
  /* 
   * Internal function: Channel(update) -> EventLoop -> Poller
   */
  void updateChannel(Channel* channel);
  
  void assertInLoopThread()
  {
    if (!isInLoopThread()) {
      abortNotInLoopThread();
    }
  }
  
  bool isInLoopThread() const { return threadId_ == std::this_thread::get_id(); }
  
private:
  void abortNotInLoopThread();
  void handleWakeup();
  void doPendingFunctors();
  
  typedef std::vector<Channel*> ChannelList;
  
  bool looping_;
  bool quit_;
  bool doingPendingFunctors_;
  const std::thread::id threadId_;
  std::unique_ptr<Poller> poller_;
  std::unique_ptr<TimerQueue> timerQueue_;
  ChannelList activeChannels_;
  
  int wakeupFd_;
  std::unique_ptr<Channel> wakeupChannel_;
  std::mutex mutex_;
  std::vector<Functor> pendingFunctors_;
};

}

#endif