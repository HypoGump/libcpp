#include "Logging.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <thread>
#include <assert.h>

using namespace std::chrono;

namespace libcpp
{

__thread char t_errnobuf[512];
__thread char t_time[32];
__thread time_t t_seconds;

const char* strerror(int savedErrno) {
  // for thread safe
  return strerror_r(savedErrno, t_errnobuf, sizeof(t_errnobuf));
}

// config log level
Logger::LogLevel initLogLevel()
{
  return Logger::TRACE;
  /* if (::getenv("libcpp_LOG_DEBUG"))
      return Logger::DEBUG;
  else
      return Logger::INFO; */
}

Logger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

void defaultOutput(const char* msg, int len)
{
  // 1 byte a char, len is the number of char
  size_t n = ::fwrite(msg, 1, len, stdout);
  assert(n == static_cast<size_t>(len));
  (void)n;
}

void defaultFlush()
{
  ::fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

} // libcpp


using namespace libcpp;

Logger::Impl::Impl(LogLevel level, int savedErrno, const char* file, const char* func, int line) 
  : timestamp_(TimeStamp::now()),
    stream_(),
    level_(level),
    line_(line),
    func_(func),
    fullname_(file),
    basename_(NULL)
{
  const char* base_start_pos = strrchr(fullname_, '/');
  basename_ = (base_start_pos != NULL) ? base_start_pos + 1 : fullname_;
  
  recordFormatTime();
  // record ThreadId
  stream_ << std::this_thread::get_id() << ' ';
  // record LogLevel
  stream_ << std::string(LogLevelName[level_]);
  // record file/func:line 
  stream_ << basename_ << " - " << func_ << ':' << line_ << ' ';
  // record old errno
  if (savedErrno != 0) {
    // libcpp::strerror, not syscall
    stream_ << strerror(savedErrno) << " (errno=" << savedErrno << ") ";
  }
}

void Logger::Impl::recordFormatTime() {
  int64_t microSecondsSinceEpoch = timestamp_.microSecondsSinceEpoch();
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch 
                                        / TimeStamp::kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch 
                                        % TimeStamp::kMicroSecondsPerSecond);
  
  if (t_seconds != seconds) {
    t_seconds = seconds;
    struct tm tm_time;
    ::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime

    int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    assert(len == 17); (void)len;
  }
  ::snprintf(t_time+17, 15, ".%06dZ ", microseconds);
  stream_ << t_time;
}



Logger::Logger(const char* file, const char* func, int line, LogLevel level)
  : impl_(level, 0, file, func, line)
{
}

Logger::Logger(const char* file, const char* func, int line, bool toAbort)
  : impl_(toAbort?FATAL:ERROR, errno, file, func, line)
{}

Logger::~Logger() {
  impl_.stream_ << '\n';
  std::string buf = impl_.stream_.str();
  g_output(buf.c_str(), buf.size());
  if (impl_.level_ == FATAL) {
    g_flush();
    abort();
  }
}

Logger::LogLevel Logger::logLevel() {
  return g_logLevel;
}

// configure log level in user program
void Logger::setLogLevel(Logger::LogLevel level) {
  g_logLevel = level;
}

void Logger::setOutputFunc(OutputFunc out)
{
  g_output = out;
}

void Logger::setFlushFunc(FlushFunc flush)
{
  g_flush = flush;
}