#ifndef LIBCPP_REACTOR_TCPSERVER_H_
#define LIBCPP_REACTOR_TCPSERVER_H_

#include "TcpConnection.h"

#include <string>
#include <functional>
#include <memory>

namespace libcpp
{
class Acceptor;

class TcpServer
{
public:
  TcpServer(EventLoop* loop, const InetAddress& addr, 
              const std::string& name = "TcpServer");
  ~TcpServer();
  
  void start();
  
  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = cb; }
  
  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = cb; }
  
private:
  void newConnection(int sockfd, const InetAddress& addr);
  typedef std::map<std::string, TcpConnPtr> ConnectionMap;
  
  EventLoop* loop_;
  const std::string name_;
  std::unique_ptr<Acceptor> acceptor_;
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  bool started_;
  int nextConnId_;
  ConnectionMap connections_;
};

}

#endif