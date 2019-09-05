#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include "logging/Logging.h"

#include <assert.h>
#include <poll.h>

using namespace libcpp;

__thread EventLoop* t_loopInThisThread = nullptr;

static const int kPollTimeMs = 10000;

EventLoop::EventLoop()
  : looping_(false),
    quit_(false),
    poller_(new Poller(this)),
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

void EventLoop::quit()
{
  quit_ = true;
}

void EventLoop::loop()
{
  assert(!looping_);
  assertInLoopThread();
  looping_ = true;
  quit_ = false;
  
  while (!quit_) {
    activeChannels_.clear();
    poller_->poll(kPollTimeMs, &activeChannels_);
    for (auto& channel : activeChannels_) {
      channel->handleEvent();
    }
  }
  
  LOG_TRACE << "EventLoop " << this << " stop looping";
  looping_ = false;
}

void EventLoop::updateChannel(Channel* channel)
{
  assertInLoopThread();
  poller_->updateChannel(channel);
}


void EventLoop::abortNotInLoopThread()
{
  LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << threadId_
            << ", current thread id = " <<  std::this_thread::get_id();
}

