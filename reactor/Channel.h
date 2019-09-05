#ifndef LIBCPP_CHANNEL_H_
#define LIBCPP_CHANNEL_H_

#include <functional>

namespace libcpp
{
class EventLoop;

class Channel
{
public:
  typedef std::function<void()> EventCallback;
  
  Channel(EventLoop* loop, int fd);
  
  void handleEvent();
  
  void setReadCallback(const EventCallback& cb)
  { readCallback_ = cb; }
  void setWriteCallback(const EventCallback& cb)
  { writeCallback_ = cb; }
  void setErrorCallback(const EventCallback& cb)
  { errorCallback_ = cb; }
  
  void enableReading()
  { events_ |= kReadEvent; update(); }
  void disableReading()
  { events_ &= ~kReadEvent; update(); }
  void enableWriting()
  { events_ |= kWriteEvent; update(); }
  void disableWriting()
  { events_ &= ~kWriteEvent; update(); }
  
  void disableAllEvent()
  { events_ = kNoneEvent; update(); }
  
  int fd() const { return fd_; }
  int events() const { return events_; }
  bool isNoneEvent() const { return events_ == kNoneEvent; }
  
  int index() const { return index_; }
  void set_index(int idx) { index_ = idx; }
  void set_revents(int revents) { revents_ = revents; }
  
private:
  void update();
  
  static const int kReadEvent;
  static const int kWriteEvent;
  static const int kNoneEvent;
  
  EventLoop* loop_;
  
  const int fd_;
  int events_;
  int revents_;
  int index_;
  
  EventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback errorCallback_;
};

}

#endif