#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include "datetime/TimeStamp.h"
#include "logging/Logging.h"

#include <assert.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace libcpp;

__thread EventLoop* t_loopInThisThread = nullptr;

static const int kPollTimeMs = 10000;

static int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0)
  {
    LOG_ERROR << "Failed in eventfd";
    abort();
  }
  return evtfd;
}


/*
 * Note: initialize in declaration order
 *        threadId_ must be initialize before other objects
 */
EventLoop::EventLoop()
  : looping_(false),
    quit_(false),
    doingPendingFunctors_(false),
    threadId_(std::this_thread::get_id()),
    poller_(new Poller(this)),
    timerQueue_(new TimerQueue(this)),
    wakeupFd_(createEventfd()),
    wakeupChannel_(new Channel(this, wakeupFd_))
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
  wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleWakeup, this));
  wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
  assert(!looping_);
  t_loopInThisThread = nullptr;
}

void EventLoop::runInLoop(const Functor& cb)
{
  if (isInLoopThread()) {
    cb();
  }
  else {
    queueInLoop(cb);
  }
}

void EventLoop::queueInLoop(const Functor& cb) 
{
  {
  std::lock_guard<std::mutex> lock(mutex_);
  pendingFunctors_.push_back(cb);
  }
  
  if (!isInLoopThread() || doingPendingFunctors_) {
    wakeup();
  }
}

TimerId EventLoop::runAt(const TimeStamp& time, const TimerCallback& cb)
{
  return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
  TimeStamp time(addTime(TimeStamp::now(), delay));
  return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
  TimeStamp time(addTime(TimeStamp::now(), interval));
  return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::quit()
{
  quit_ = true;
  if (!isInLoopThread()) {
    wakeup();
  }
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
    /*
     * Note: don't call doPendingFunctors() in handleWakeup()
     *    because not all pending functors need wakeup
     */
    doPendingFunctors();
  }
  
  LOG_TRACE << "EventLoop " << this << " stop looping";
  looping_ = false;
}

void EventLoop::updateChannel(Channel* channel)
{
  assertInLoopThread();
  poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
  assertInLoopThread();
  poller_->removeChannel(channel);
}

void EventLoop::wakeup()
{
  uint64_t one = 1;
  ssize_t n = ::write(wakeupFd_, &one, sizeof one); // +1
  if (n != sizeof one)
  {
    LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
  }
}

void EventLoop::handleWakeup()
{
  uint64_t one = 1;
  ssize_t n = ::read(wakeupFd_, &one, sizeof one); // -1
  if (n != sizeof one)
  {
    LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
  }
}

void EventLoop::doPendingFunctors()
{
  std::vector<Functor> functors;
  doingPendingFunctors_ = true;
  
  {
  std::lock_guard<std::mutex> lock(mutex_);
  functors.swap(pendingFunctors_);
  }
  
  for (auto& func : functors) {
    func();
  }
  
  doingPendingFunctors_ = false;
}

void EventLoop::abortNotInLoopThread()
{
  LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << threadId_
            << ", current thread id = " <<  std::this_thread::get_id();
}

