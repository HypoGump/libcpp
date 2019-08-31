#include "LogStream.h"

#include <algorithm>


namespace libcpp
{
namespace utils
{

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
static_assert(sizeof(digits) == 20, "[Host] char is not a byte.");

const char digitsHex[] = "0123456789abcdef";
static_assert(sizeof digitsHex == 17, "[Host] char is not a byte.");

template<typename T>
size_t convert(char buf[], T val) {
  T i = val;
  char* p = buf;
  
  do {
    int d = i % 10;
    i /= 10;
    *p++ = zero[d];
  } while (i != 0);
  
  if (val < 0) {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);
  
  return (p - buf);
}


size_t convertHex(char buf[], uintptr_t val) {
  uintptr_t i = val;
  char* p = buf;
  
  do {
    int d = i % 16;
    i /= 16;
    *p++ = zero[d];
  } while (i != 0);
  
  *p = '\0';
  std::reverse(buf, p);
  
  return (p - buf);
}

} // utils
} // libcpp


using namespace libcpp;
using namespace libcpp::utils;

// FixedBuffer explicit instantiations
template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;


template<typename T>
void LogStream::formatInteger(T v) {
  // FIXME: if space not enough?
  size_t len = convert(buffer_.current(), v);
  buffer_.add(len);
}

// LogStream overload operator<<
LogStream& LogStream::operator<<(bool v) {
  v ? buffer_.append("true", 4) : buffer_.append("false", 5);
  return *this;
}
  
LogStream& LogStream::operator<<(short v) {
  *this << static_cast<int>(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned short v) {
  *this << static_cast<unsigned int>(v);
  return *this;
}

LogStream& LogStream::operator<<(int v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned int v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) {
  formatInteger(v);
  return *this;
}


LogStream& LogStream::operator<<(float v) {
  *this << static_cast<double>(v);
  return *this;
}

LogStream& LogStream::operator<<(double v) {
  // FIXME: if space not enough? use vector?
  int len = snprintf(buffer_.current(), buffer_.avail(), "%.12g", v);
  buffer_.add(len);
  return *this;
}


LogStream& LogStream::operator<<(char v) {
  buffer_.append(&v, 1);
  return *this;
}

LogStream& LogStream::operator<<(const char* v) {
  buffer_.append(v, strlen(v));
  return *this;
}

LogStream& LogStream::operator<<(std::string& v) {
  buffer_.append(v.c_str(), v.size());
  return *this;
}


LogStream& LogStream::operator<<(const void* p) {
  // FIXME: if space not enough? use vector?
  uintptr_t i = reinterpret_cast<uintptr_t>(p);
  char* buf = buffer_.current();
  buf[0] = '0';
  buf[1] = 'x';
  size_t len = convertHex(buf+2, i);
  buffer_.add(len+2);
  return *this;
}

LogStream& LogStream::operator<<(T v) {
  buffer_.append(v.str_, v.len_);
  return *this;
}

// Format explicit instantiations
template Format::Format(const char* fmt, char);

template Format::Format(const char* fmt, short);
template Format::Format(const char* fmt, unsigned short);
template Format::Format(const char* fmt, int);
template Format::Format(const char* fmt, unsigned int);
template Format::Format(const char* fmt, long);
template Format::Format(const char* fmt, unsigned long);
template Format::Format(const char* fmt, long long);
template Format::Format(const char* fmt, unsigned long long);

template Format::Format(const char* fmt, float);
template Format::Format(const char* fmt, double);