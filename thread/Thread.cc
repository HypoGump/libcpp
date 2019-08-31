#include "Thread.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <assert.h>
#include <pthread.h>

namespace libcpp
{
namespace CurrentThread
{
  __thread const char* t_threadName = "unkown";
}
}

// nameless space means static
namespace
{

__thread pid_t t_cacheTid = 0;

pid_t gettid() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

void afterFork()
{
  t_cacheTid = 0;
  t_cacheTid = gettid();
  libcpp::CurrentThread::t_threadName = "main";
  // no need to call pthread_atfork(NULL, NULL, &afterFork);
}

class ThreadNameInitializer
{
public:
  ThreadNameInitializer()
  {
    libcpp::CurrentThread::t_threadName = "main";
    pthread_atfork(NULL, NULL, &afterFork);
  }
};

ThreadNameInitializer init;

struct ThreadData
{
  typedef libcpp::Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;
  std::string name_;
  std::weak_ptr<pid_t> wkTid_;

  ThreadData(const ThreadFunc& func,
       const std::string& name,
       const std::shared_ptr<pid_t>& tid)
    : func_(func),
      name_(name),
      wkTid_(tid)
  { }

  void runInThread()
  {
    pid_t tid = libcpp::CurrentThread::tid();
    std::shared_ptr<pid_t> ptid = wkTid_.lock();

    if (ptid)
    {
      *ptid = tid;
      ptid.reset();
    }

    libcpp::CurrentThread::t_threadName = name_.empty() ? "libcppThread" : name_.c_str();
    ::prctl(PR_SET_NAME, libcpp::CurrentThread::t_threadName);
    func_(); // FIXME: surround with try-catch, see libcpp
    libcpp::CurrentThread::t_threadName = "finished";
  }
};

void* startThread(void* obj)
{
  ThreadData* data = static_cast<ThreadData*>(obj);
  data->runInThread();
  delete data;
  return NULL;
}

}


using namespace libcpp;
using namespace libcpp::CurrentThread;

pid_t CurrentThread::tid() {
  if (t_cacheTid == 0) {
    t_cacheTid = gettid();
  }
  return t_cacheTid;
}

const char* name() {
  return t_threadName;
}

bool CurrentThread::isMainThread() {
  // main thread id == process id
  return tid() == ::getpid();
}

Thread::Thread(const ThreadFunc& func, const std::string& n)
  : started_(false),
  joined_(false),
  pthreadId_(0),
  tid_(new pid_t(0)),
  func_(func),
  name_(n)
{}

Thread::~Thread()
{
  if (started_ && !joined_)
  {
    pthread_detach(pthreadId_);
  }
}

void Thread::start()
{
  assert(!started_);
  started_ = true;

  ThreadData* data = new ThreadData(func_, name_, tid_);
  if (pthread_create(&pthreadId_, NULL, &startThread, data))
  {
    started_ = false;
    delete data;
    abort();
  }
}

void Thread::join()
{
  assert(started_);
  assert(!joined_);
  joined_ = true;
  pthread_join(pthreadId_, NULL);
}