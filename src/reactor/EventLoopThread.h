#ifndef LIBCPP_EVENTLOOPTHREAD_H_
#define LIBCPP_EVENTLOOPTHREAD_H_

#include <thread>
#include <future>

namespace libcpp
{
class EventLoop;

class EventLoopThread
{
public:
  EventLoopThread();
  ~EventLoopThread();
  
  EventLoop* startLoop();
  
private:
  void threadFunc();
  
  EventLoop* loop_;
  std::thread thread_;
  std::promise<void> oneshot_;
};

}

#endif