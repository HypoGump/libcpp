#include "EventLoop.h"
#include "logging/Logging.h"

#include <assert.h>
#include <poll.h>

using namespace libcpp;

__thread EventLoop* t_loopInThisThread = nullptr;


EventLoop::EventLoop()
  : looping_(false),
    threadId_(std::this_thread::get_id())
{
  LOG_TRACE << "[EventLoop] New eventloop " << this 
            << " created in thread " << threadId_;
  if (t_loopInThisThread) {
    LOG_FATAL << "[EventLoop] Another loop " << t_loopInThisThread
                <<" already exists in this thread " << threadId_;
  }
  else {
    t_loopInThisThread = this;
  }
}

EventLoop::~EventLoop()
{
  assert(!looping_);
  t_loopInThisThread = nullptr;
}


void EventLoop::loop()
{
  assert(!looping_);
  assertInLoopThread();
  looping_ = true;
  
  ::poll(NULL, 0, 5*1000);
  LOG_TRACE << "EventLoop " << this << " stop looping";
  looping_ = false;
}


void EventLoop::abortNotInLoopThread()
{
  LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << threadId_
            << ", current thread id = " <<  std::this_thread::get_id();
}

