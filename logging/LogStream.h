#ifndef LIBCPP_LOGSTREAM_H_
#define LIBCPP_LOGSTREAM_H_

#include <assert.h>
#include <string.h>
#include <string>

namespace labip
{

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template<int SIZE>
class FixedBuffer
{
public:
  FixedBuffer() : cur_(buf_) {}
  ~FixedBuffer() {}
  
  int avail() const { return (end() - cur_); }
  
  // FIXME: if space not enough?
  void append(const char* data, int len) {
    if (avail() > len) {
      memcpy(cur_, data, len);
      cur_ += len;
    }
  }
  
  const char* data() const { return buf_; }
  int length() const { return (cur_ - buf_); }
  char* current() const { return cur_; }
  void add(size_t len) { cur_ += len; }
  
  void bzero() { ::bzero(buf_, sizeof(buf_)); }
  void reset() { cur_ = buf_; }
  
private:
  char buf_[SIZE]; 
  char* cur_;
  const char* end() const { return (buf_ + sizeof(buf_)); }
};


// helper class for other data type using LogStream
class T
{
public: 
  T(const char* str, int len) : str_(str), len_(len) {
    assert(strlen(str) == len_);
  }
  
  const char* str_;
  const size_t len_;
};

class LogStream
{
  typedef LogStream self;
public:
  typedef FixedBuffer<kSmallBuffer> Buffer;
  
  self& operator<<(bool);
  
  self& operator<<(short);
  self& operator<<(unsigned short);
  self& operator<<(int);
  self& operator<<(unsigned int);
  self& operator<<(long);
  self& operator<<(unsigned long);
  self& operator<<(long long);
  self& operator<<(unsigned long long);
  
  self& operator<<(float);
  self& operator<<(double);
  
  self& operator<<(char);
  self& operator<<(const char*);
  self& operator<<(std::string&);
  
  self& operator<<(const void* p);
  self& operator<<(T);
  
  void append(const char* data, int len) { buffer_.append(data, len); }
  const Buffer& data() const { return buffer_; }
  void resetBuffer() { buffer_.reset(); }
  
private:
  template<typename T> void formatInteger(T v);
  Buffer buffer_;
};


// utility for C-style formated
class Format
{
public:
  template<typename T>
  Format(const char* fmt, T val) {
    static_assert(std::is_arithmetic<T>::value == true, "[Format] T is not arithmetic.");
    length_ = snprintf(buf_, sizeof(buf_), fmt, val);
    assert(static_cast<size_t>(length_) < sizeof(buf_));
  }
  
  const char* data() const { return buf_; }
  int length() const { return length_; }
private:
  char buf_[32];
  int length_;
};


inline LogStream& operator<<(LogStream& os, const Format& fmt) {
  os.append(fmt.data(), fmt.length());
  return os;
}

} // labip

#endif