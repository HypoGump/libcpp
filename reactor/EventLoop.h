#ifndef LIBCPP_EVENTLOOP_H_
#define LIBCPP_EVENTLOOP_H_

#include <thread>
#include <functional>

namespace libcpp
{

class EventLoop
{
public:
  EventLoop();
  ~EventLoop();
  
  void loop();
  
  void assertInThisThread()
  {
    if (!isInLoopThread()) {
      abortNotInLoopThread();
    }
  }
  
  bool isInLoopThread() const { return threadId_ == std::this_thread::get_id(); }
  
private:
  void abortNotInLoopThread();
  
  bool looping_
  std::thread::id threadId_;
};

}

#endif