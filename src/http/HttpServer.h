#ifndef LIBCPP_HTTPSERVER_H_
#define LIBCPP_HTTPSERVER_H_

#include "utils/noncopyable.h"
#include "net/TcpConnection.h"
#include "net/TcpServer.h"
#include "HttpSession.h"

#include "utils/ModernCpp.h"
#include <map>
#include <string>

namespace libcpp
{
class EventLoop;
/*
 * HttpServer: a HttpServer is over a TcpServer
 */
class HttpServer : public utils::noncopyable
{
public:
  HttpServer(EventLoop* loop, const InetAddress& addr,
              const std::string& name = "HttpServer");
  ~HttpServer();

  void start();
  void stop();

  void setOnHeadersCallback(const HttpCallback& cb)
  { onHeadersCallback_ = cb; }
  void setOnMessageCallback(const HttpCallback& cb)
  { onMessageCallback_ = cb; }

private:
  void onConnection(const TcpConnSptr& conn);
  void onMessage(const TcpConnSptr&, Buffer*, TimeStamp);

private:
  using HttpSessionSptr = std::shared_ptr<HttpSession>;
  using SessionMap = std::map<TcpConnSptr, HttpSessionSptr>;

  TcpServer tcpServer_;
  SessionMap sessions_;

  HttpCallback onMessageCallback_;
  HttpCallback onHeadersCallback_;
};

}

#endif