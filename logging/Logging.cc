#include "Logging.h"
#include "/datetime/TimeStamp.h"
#include "/thread/Thread.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>

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

Logger::Impl::Impl(LogLevel level, int savedErrno, const char* file, int line) 
  : timestamp_(TimeStamp::now()),
    stream_(),
    level_(level),
    line_(line),
    fullname_(file),
    basename_(NULL)
{
  const char* base_start_pos = strchr(fullname_, '/');
  basename_ = (base_start_pos != NULL) ? base_start_pos + 1 : fullname_;
  
  recordFormatTime();
  // record ThreadId
  Format tid("%05d ", CurrentThread::tid());
  assert(tid.length() == 6);
  stream_ << T(tid.data(), 6);
  // record LogLevel
  stream_ << T(LogLevelName[level_], 6);
  // record old errno
  if (savedErrno != 0) {
    // libcpp::strerror, not syscall
    stream_ << strerror(savedErrno) << " (errno=" << savedErrno << ") ";
  }
}

void Logger::Impl::recordFormatTime() {
  time_t seconds = static_cast<time_t>(timestamp_.seconds());
  int microseconds = static_cast<int>(timestamp_.useconds());
  
  if (t_seconds != seconds) {
    t_seconds = seconds;
    struct tm tm_time;
    ::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime

    int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    assert(len == 17); (void)len;
  }
  Format us(".%06dZ ", microseconds);
  stream_ << T(t_time, 17) << T(us.data(), 9);
}

void Logger::Impl::finish() {
  stream_ << " - " << basename_ << ':' << line_ << '\n';
}

Logger::Logger(const char* file, int line)
  : impl_(INFO, 0, file, line)
{}

Logger::Logger(const char* file, int line, LogLevel level)
  : impl_(level, 0, file, line)
{}

Logger::Logger(const char* file, int line, LogLevel level, const char* func)
  : impl_(level, 0, file, line)
{
  impl_.stream_ << func << ": ";
}

Logger::Logger(const char* file, int line, bool toAbort)
  : impl_(toAbort?FATAL:ERROR, errno, file, line)
{}

Logger::~Logger() {
  impl_.finish();
  const LogStream::Buffer& buf(stream().data()); // Why need copy?
  g_output(buf.data(), buf.length());
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