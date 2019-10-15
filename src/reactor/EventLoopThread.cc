#include "EventLoopThread.h"
#include "EventLoop.h"
#include <functional>

using namespace libcpp;


EventLoopThread::EventLoopThread()
  : loop_(nullptr),
    thread_(std::bind(&EventLoopThread::threadFunc, this)),
    oneshot_()
{
}

EventLoopThread::~EventLoopThread()
{
  thread_.join();
  loop_->quit();
}

EventLoop* EventLoopThread::startLoop()
{
  oneshot_.get_future().wait();
  return loop_;
}

void EventLoopThread::threadFunc()
{
  EventLoop loop;
  
  loop_ = &loop;
  oneshot_.set_value();
  
  loop.loop();
}