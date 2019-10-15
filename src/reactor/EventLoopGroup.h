#ifndef LIBCPP_EVENTLOOPGROUP_H_
#define LIBCPP_EVENTLOOPGROUP_H_

#include "EventLoopThread.h"
#include <vector>
#include <memory>

namespace libcpp
{
class EventLoop;

class EventLoopGroup
{
public:
  EventLoopGroup(EventLoop* baseLoop);
  ~EventLoopGroup();
  
  void setThreadNum(int num) { numThreads_ = num; }
  void start();
  EventLoop* getNextLoop();
  
private:
  using EventLoopThreadPtr = std::unique_ptr<EventLoopThread>;
  EventLoop* baseLoop_;
  bool started_;
  int numThreads_;
  size_t next_;
  
  std::vector<EventLoopThreadPtr> threads_;
  std::vector<EventLoop*> loops_;
};

}

#endif