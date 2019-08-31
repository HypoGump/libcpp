#ifndef LIBCPP_ASYNCLOGGINGDOUBLEBUFFERING_H_
#define LIBCPP_ASYNCLOGGINGDOUBLEBUFFERING_H_

#include "LogStream.h"
#include "LogFile.h"

#include "thread/Mutex.h"
#include "thread/Condition.h"
#include "thread/CountDownLatch.h"
#include "thread/Thread.h"

#include <memory>
#include <vector>

namespace libcpp
{

class AsyncLoggingDoubleBuffering
{
public:
  typedef FixedBuffer<libcpp::kLargeBuffer> Buffer;
  typedef std::unique_ptr<Buffer> BufferPtr;
  typedef std::vector<BufferPtr> BufferVector;

  AsyncLoggingDoubleBuffering(const std::string& basename, size_t rollSize, 
                              int flushInterval = 3)
  : basename_(basename),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    mutex_(),
    cond_(mutex_),
    latch_(1),
    thread_(std::bind(&AsyncLoggingDoubleBuffering::threadFunc, this), "Logging"),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_()
  {
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
  }
  
  ~AsyncLoggingDoubleBuffering() {
    if (running_) stop();
  }
  
  void start() {
    running_ = true;
    thread_.start();
    // must wait until threadFunc run
    latch_.wait();
  }
  
  void stop() {
    running_ = false;
    cond_.notify(); // notify what? 
    thread_.join();
  }
  
  // bind to logger's outputFunc
  void append(const char* logline, int len) 
  {
    libcpp::MutexLockGuard lock(mutex_);
    if (currentBuffer_->avail() > len) {
      currentBuffer_->append(logline, len);
    }
    else {
      buffers_.push_back(BufferPtr(currentBuffer_.release()));
      if (nextBuffer_) {
        currentBuffer_ = std::move(nextBuffer_);
      }
      else {
        currentBuffer_.reset(new Buffer);
      }
      currentBuffer_->append(logline, len);
      
      // notify backend there have filled buffer to save
      cond_.notify();
    }
  }
    
private:
  // backend, write to file
  void threadFunc() 
  {
    latch_.countDown();
    
    LogFile output(basename_, rollSize_, false);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    
    while (running_) {
      { // critical zone
        libcpp::MutexLockGuard lock(mutex_);
        if (buffers_.empty()) {
          cond_.waitUntilTimeout(flushInterval_); // block
        }
        // change ownership, not copy
        buffers_.push_back(BufferPtr(currentBuffer_.release()));
        currentBuffer_ = std::move(newBuffer1);
        buffersToWrite.swap(buffers_);
        if (!nextBuffer_) nextBuffer_ = std::move(newBuffer2);
      }
        
      // write buffersToWrite to file
      for (decltype(buffersToWrite.size()) i = 0; i < buffersToWrite.size(); ++i) 
      {
        output.append((*buffersToWrite[i]).data(), (*buffersToWrite[i]).length());
      }
      buffersToWrite.clear();
        
      // refill newBuffer
      BufferPtr newBuffer1(new Buffer);
      BufferPtr newBuffer2(new Buffer);
      newBuffer1->bzero();
      newBuffer2->bzero();
      
      output.flush();
    }
    
    output.flush();
  }
    
  bool running_;
  std::string basename_;
  size_t rollSize_;
  const int flushInterval_;
  
  Mutex mutex_;
  Condition cond_;
  Thread thread_;  // FIXME: std::thread?
  
  // make sure backend start before logger
  CountDownLatch latch_;
  
  BufferPtr currentBuffer_;
  BufferPtr nextBuffer_;
  BufferVector buffers_;  // filled buffer, ready to save in file
};

} // libcpp


#endif