#ifndef LIBCPP_TIMESTAMP_H_
#define LIBCPP_TIMESTAMP_H_

#include <chrono>
#include <string>
#include <ctime>

namespace libcpp
{
using SteadyClock = std::chrono::steady_clock;
using TimePoint = std::chrono::steady_clock::time_point;

/* time unit */
using DurationNs = std::chrono::nanoseconds;
using DurationUs = std::chrono::microseconds;
using DurationS = std::chrono::seconds;

/* cast */
using duration_cast = std::chrono::duration_cast;

class Timestamp
{
public:
  Timestamp();
  explicit Timestamp(TimePoint t);
  ~Timestamp();
  
  static TimePoint now() const;
  
  std::string toString();
  std::string toFormattedString();
  
  std::time_t microSecondsSinceEpoch() const
  {
    return duration_cast<DurationUs>(tpoint_).count();
  }
  
private:
  TimePoint tpoint_;
}

// TODO: comparation and + - operation
inline bool operator< (Timestamp lhs, Timestamp rhs)
{
  return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

}

#endif