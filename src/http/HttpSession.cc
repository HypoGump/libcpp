#include "HttpSession.h"
#include "logging/Logging.h"

using namespace libcpp;

HttpSession::HttpSession(TcpConnSptr tcpConn)
 :  HttpParser(HTTP_REQUEST),
    tcpConn_(tcpConn)
{
  
}

HttpSession::~HttpSession()
{
  
}


void HttpSession::onMessage(const TcpConnSptr& conn, Buffer* buf, TimeStamp receivedTime)
{
  execute(buf->data(), buf->readableBytes());
  LOG_TRACE << "[HttpSession] receive data at " << receivedTime.toFormattedString();
}


int HttpSession::onMessageBegin()
{
  LOG_TRACE << "Message Begin";
  return 0;
}

int HttpSession::onUrl(const char* at, size_t length)
{
  LOG_TRACE << std::string(at, length);
  return 0;
}

int HttpSession::onStatus(const char* at, size_t length)
{
  LOG_TRACE << "status";
  return 0;
}

int HttpSession::onHeaderField(const char* at, size_t length)
{
  LOG_TRACE << std::string(at, length);
  return 0;
}

int HttpSession::onHeaderValue(const char* at, size_t length)
{
  LOG_TRACE << std::string(at, length);
  return 0;
}

int HttpSession::onHeadersComplete()
{
  LOG_TRACE << "Header Complete";
  return 0;
}

int HttpSession::onBody(const char* at, size_t length)
{
  LOG_TRACE << std::string(at, length);
  return 0;
}

int HttpSession::onMessageComplete()
{
  LOG_TRACE << "Message Complete";
  return 0;
}

