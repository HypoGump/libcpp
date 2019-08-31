#ifndef LIBCPP_LOGGING_H_
#define LIBCPP_LOGGING_H_

#include "LogStream.h"
#include "/datetime/TimeStamp.h"

#include <functional>


/*
 * Log Filename Format:
 *     process name.datetime.hostname.process id.log
 * 
 * Log Format:
 *      date time tid loglevel message - sourcefile: line
 */
namespace libcpp
{

// C++ stream style
class Logger
{
public:
  enum LogLevel
  {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };
  
  Logger(const char* file, int line);
  Logger(const char* file, int line, LogLevel level);
  Logger(const char* file, int line, LogLevel level, const char* func);
  Logger(const char* file, int line, bool toAbort);
  ~Logger();
  
  static LogLevel logLevel();
  static void setLogLevel(LogLevel level);
  
  LogStream& stream() { return impl_.stream_; }
  
  typedef std::function<void(const char*, int)> OutputFunc;
  typedef std::function<void()> FlushFunc;
  static void setOutputFunc(OutputFunc);
  static void setFlushFunc(FlushFunc);
    
private:
  class Impl
  {
   public:
    typedef Logger::LogLevel LogLevel;
    Impl(LogLevel level, int savedErrno, const char* file, int line);
    void recordFormatTime();
    void finish();

    TimeStamp timestamp_;
    LogStream stream_;
    LogLevel level_;
    int line_;
    const char* fullname_;
    const char* basename_;
  };
  
  Impl impl_;
};


/*
 *  __FILE__: the full path to the source file
 *  __LINE__: the line number of the code
 */
#define LOG_TRACE if (libcpp::Logger::logLevel() <= libcpp::Logger::TRACE) \
    libcpp::Logger(__FILE__, __LINE__, libcpp::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (libcpp::Logger::logLevel() <= libcpp::Logger::DEBUG) \
    libcpp::Logger(__FILE__, __LINE__, libcpp::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (libcpp::Logger::logLevel() <= libcpp::Logger::INFO) \
    libcpp::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN libcpp::Logger(__FILE__, __LINE__, libcpp::Logger::WARN).stream()
#define LOG_ERROR libcpp::Logger(__FILE__, __LINE__, libcpp::Logger::ERROR).stream()
#define LOG_FATAL libcpp::Logger(__FILE__, __LINE__, libcpp::Logger::FATAL).stream()
#define LOG_SYSERR libcpp::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL libcpp::Logger(__FILE__, __LINE__, true).stream()

// C style


} // libcpp

#endif