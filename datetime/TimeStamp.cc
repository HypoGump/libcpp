#include "TimeStamp.h"

#include <sys/time.h>
#include <stdio.h>
#include <inttypes.h>

using namespace libcpp;

// compiling check
static_assert(sizeof(TimeStamp) == 2*sizeof(int64_t), 
                "[Compiling ERROR] Only support 64-bit platform.");
                
TimeStamp::TimeStamp()
  : sec_(0), usec_(0)
{}

TimeStamp::TimeStamp(int64_t sec, int64_t usec)
  : sec_(sec), usec_(usec)
{}


std::string TimeStamp::toString() const
{
    char buf[32] = {0};
    snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", sec_, usec_);
    return buf;
}

std::string TimeStamp::toFormattedString() const
{
    char buf[32] = {0};
    time_t seconds = static_cast<time_t>(sec_);
    struct tm tm_time;
    ::gmtime_r(&seconds, &tm_time);
    
    // year-month-day hour:min:sec.microseconds, 'Z' means UTC time
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06dZ",
      tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
      tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
      static_cast<int>(usec_));
    return buf;
}



/* struct timeval
 * {
 * __time_t tv_sec;        // Seconds.
 * __suseconds_t tv_usec;  // Microseconds.
 * }; 
 */
TimeStamp TimeStamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return TimeStamp(static_cast<int64_t>(tv.tv_sec), static_cast<int64_t>(tv.tv_usec));
}

