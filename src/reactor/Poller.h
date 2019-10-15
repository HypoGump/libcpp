#ifndef LIBCPP_POLLER_H_
#define LIBCPP_POLLER_H_

#include "datetime/TimeStamp.h"

#include <vector>
#include <map>

/*
 * int   fd;        
 * short events;     
 * short revents; 
 */
struct pollfd;

namespace libcpp
{
class EventLoop;
class Channel;

class Poller
{
public:
  typedef std::vector<Channel*> ChannelList;
  
  Poller(EventLoop* loop);
  TimeStamp poll(int timeoutMs, ChannelList* activeChannels);
  
  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);
  
  void assertInLoopThread();
  
private:
  void fillActiveChannels(int activeNums, ChannelList* activeChannels);
  
  typedef std::vector<struct pollfd> PollfdList;
  typedef std::map<int, Channel*> ChannelMap;
  
  EventLoop* ownerLoop_;
  PollfdList pollfds_;
  ChannelMap channels_;
};

}

#endif