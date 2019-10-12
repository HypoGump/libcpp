#ifndef LIBCPP_BUFFER_H_
#define LIBCPP_BUFFER_H_

#include <vector>
#include <string>
#include <sys/types.h>
#include <assert.h>

namespace libcpp
{

class Buffer
{
public:
  static const size_t kCheapPrepend = 8;
  static const size_t kInitialSize = 1024;

  Buffer()
    : buffer_(kCheapPrepend + kInitialSize),
      readerIndex_(kCheapPrepend),
      writerIndex_(kCheapPrepend)
  {
    assert(readableBytes() == 0);
    assert(writableBytes() == kInitialSize);
    assert(prependableBytes() == kCheapPrepend);
  }
  
  size_t readableBytes() const 
  { return writerIndex_ - readerIndex_; }
  
  size_t writableBytes() const
  { return buffer_.size() - writerIndex_; }
  
  size_t prependableBytes() const
  { return readerIndex_; }
  
  const char* data() const 
  { return begin() + readerIndex_; }
  
  void retrieve(size_t len)
  {
    assert(len <= readableBytes());
    readerIndex_ += len;
  }
  
  void retrieveAll()
  {
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
  }
  
  std::string retrieveAsString()
  {
    std::string str(data(), readableBytes());
    retrieveAll();
    return str;
  }
  
  void append(const std::string& str)
  {
    append(str.data(), str.length());
  }
  
  void append(const char* data, size_t len)
  {
    ensureWritableBytes(len);
    std::copy(data, data+len, beginWrite());
    hasWritten(len);
  }
  
  void append(const void* data, size_t len)
  {
    append(static_cast<const char*>(data), len);
  }
  
  void ensureWritableBytes(size_t len)
  {
    if (writableBytes() < len) {
      makeSpace(len);
    }
    assert(writableBytes() >= len);
  }
  
  char* beginWrite()
  { return begin() + writerIndex_; }
  
  const char* beginWrite() const
  { return begin() + writerIndex_; }
  
  void hasWritten(size_t len)
  { writerIndex_ += len; }
  
  ssize_t readFd(int fd, int* savedErrno);
  
private:
  char* begin()
  { return &*buffer_.begin(); }
  
  const char* begin() const
  { return &*buffer_.begin(); }
  
  void makeSpace(size_t len)
  {
    if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
      buffer_.resize(writerIndex_+len);
    }
    else {
      // move readable data to the front, make space inside buffer
      assert(kCheapPrepend < readerIndex_);
      size_t readable = readableBytes();
      std::copy(begin()+readerIndex_,
                begin()+writerIndex_,
                begin()+kCheapPrepend);
      readerIndex_ = kCheapPrepend;
      writerIndex_ = readerIndex_ + readable;
      assert(readable == readableBytes());
    }
  }

private:
  std::vector<char> buffer_;
  size_t readerIndex_;
  size_t writerIndex_;
};

}

#endif