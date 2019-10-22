#ifndef LIBCPP_HTTPSESISON_H_
#define LIBCPP_HTTPSESISON_H_

extern "C"
{
#include "http_parser.h"
}

#include "utils/noncopyable.h"
#include "net/TcpConnection.h"

#include <memory>

namespace libcpp
{

/*
 * HttpSession: 
 *  a HttpSession is over a TcpConnection.
 */
class HttpSession : public utils::noncopyable
{
  friend class HttpParserSettings;
public:
  /*
   * @param parserType
   * - HTTP_REQUEST means parse http request messages
   * - HTTP_RESPONSE means parse http response messages
   * - HTTP_BOTH means parse both messages
   */
  HttpSession(const TcpConnSptr& tcpConn, http_parser_type parserType);
  ~HttpSession();
  
  // FIXME: use Buffer class?
  size_t execute(const char *buf, size_t length);
  
protected:
  int onMessageBegin();
  int onUrl(const char*, size_t);
  int onStatus(const char*, size_t);
  int onHeaderField(const char*, size_t);
  int onHeaderValue(const char*, size_t);
  int onHeadersComplete();
  int onBody(const char*, size_t);
  int onMessageComplete();
  
  int onChunkHeader() { return 0; }
  int onChunkComplete() { return 0; }
  //int onUpgrade(const char*, size_t) override;
  //int onError(const HttpError& error) override;
  
private:
  TcpConnSptr tcpConn_;
  /* need message buffer? */
  http_parser parser_;
  std::string curField_;
  std::string curValue_;
};

}

#endif