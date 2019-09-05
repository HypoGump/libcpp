#ifndef LIBCPP_EVENTLOOP_H_
#define LIBCPP_EVENTLOOP_H_

#include <vector>
#include <thread>
#include <memory>
#include <functional>

namespace libcpp
{
class Channel;
class Poller;

class EventLoop
{
public:
  EventLoop();
  ~EventLoop();
  
  void loop();
  void quit();
  
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
  
  
  typedef std::vector<Channel*> ChannelList;
  
  bool looping_;
  bool quit_;
  std::unique_ptr<Poller> poller_;
  ChannelList activeChannels_;
  std::thread::id threadId_;
};

}

#endif