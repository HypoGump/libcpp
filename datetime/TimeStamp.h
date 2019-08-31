#ifndef LIBCPP_TimeStamp_H_
#define LIBCPP_TimeStamp_H_

#include <stdint.h>
#include <string>

namespace libcpp
{

class TimeStamp
{
public:
    TimeStamp();
    explicit TimeStamp(int64_t sec, int64_t usec_);
    
    std::string toString() const;
    std::string toFormattedString() const;
    
    int64_t microSecondsSinceEpoch() const { 
        return sec_ * kMicroSecondsPerSecond + usec_; 
    }
    
    int64_t seconds() const { return sec_; }
    int64_t useconds() const { return usec_; }
    
    static TimeStamp now();
    static const int kMicroSecondsPerSecond = 1000 * 1000;
    
private:
    int64_t sec_;
    int64_t usec_;
};

inline bool operator< (TimeStamp lhs, TimeStamp rhs)
{
  return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator== (TimeStamp lhs, TimeStamp rhs)
{
  return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline double timeInterval(TimeStamp high, TimeStamp low)
{
  int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
  return static_cast<double>(diff) / TimeStamp::kMicroSecondsPerSecond;
}

} // libcpp

#endif // libcpp_TimeStamp_H_