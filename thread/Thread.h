#ifndef LIBCPP_THREAD_H_
#define LIBCPP_THREAD_H_

#include <thread>
#include <string>
#include <functional>

namespace libcpp
{

// FIXME: use std::thread?
class Thread
{
public:
  typedef std::function<void()> ThreadFunc;
  explicit Thread(const ThreadFunc&, const std::string& name = std::string());
  ~Thread();
  
  void start();
  void join();
  
  pid_t tid() const { return *tid_; }
  const std::string& name() const { return name_; }
    
private:
  bool started_;
  bool joined_;
  pthread_t pthreadId_;
  std::shared_ptr<pid_t> tid_;  // why ptr?
  ThreadFunc func_;
  std::string name_;
};

namespace CurrentThread
{
  pid_t tid();
  const char* tname();
  bool isMainThread();
}

}

#endif