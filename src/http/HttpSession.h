#ifndef LIBCPP_HTTPSESISON_H_
#define LIBCPP_HTTPSESISON_H_

extern "C"
{
#include "http_parser.h"
}

#include "net/TcpConnection.h"
#include "HttpParser.h"

#include <memory>

namespace libcpp
{

/*
 * HttpSession: 
 *  a HttpSession is over a TcpConnection.
 */
class HttpSession : public HttpParser
{
public:
  HttpSession(TcpConnSptr tcpConn);
  ~HttpSession();
  
  void onMessage(const TcpConnSptr&, Buffer*, TimeStamp);

protected:
  int onMessageBegin() override;
  int onUrl(const char*, size_t) override;
  int onStatus(const char*, size_t) override;
  int onHeaderField(const char*, size_t) override;
  int onHeaderValue(const char*, size_t) override;
  int onHeadersComplete() override;
  int onBody(const char*, size_t) override;
  int onMessageComplete() override;
  
  //int onUpgrade(const char*, size_t) override;
  //int onError(const HttpError& error) override;
  
private:
  TcpConnSptr tcpConn_;
  /* need message buffer? */
};

}

#endif