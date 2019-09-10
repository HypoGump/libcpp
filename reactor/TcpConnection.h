#ifndef LIBCPP_REACTOR_TCPCONNECTION_H_
#define LIBCPP_REACTOR_TCPCONNECTION_H_

#include "InetAddress.h"

#include <map>
#include <string>
#include <functional>
#include <memory>

namespace libcpp
{
class EventLoop;
class TcpConnection;
class Socket;
class Channel;

typedef std::shared_ptr<TcpConnection> TcpConnPtr;
typedef std::function<void(const TcpConnPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnPtr&,
                            const char* data,
                            ssize_t len)> MessageCallback;
typedef std::function<void(const TcpConnPtr&)> CloseCallback;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
  TcpConnection(EventLoop* loop,
                const std::string& name,
                int sockfd,
                const InetAddress& localAddr,
                const InetAddress& peerAddr);
                
  ~TcpConnection();
  
  bool connected() const { return state_ == kConnected; }
  const std::string& name() const { return name_; }
  const InetAddress& peerAddress() const { return peerAddr_; }
  const InetAddress& localAddress() const { return localAddr_; }
  
  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = cb; }

  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = cb; }
  
  void setCloseCallback(const CloseCallback& cb)
  { closeCallback_ = cb; }
  
  void connectEstablished();
  void connectDestroyed();

private:
  enum ConnState {kConnecting, kConnected, kDisconnected};
  
  void handleRead();
  void handleWrite();
  void handleClose();
  void handleError();
  void setConnState(ConnState s) { state_ = s; }
  
  EventLoop* loop_;
  const std::string name_;
  ConnState state_;
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;
  InetAddress localAddr_;
  InetAddress peerAddr_;
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  CloseCallback closeCallback_;
};


}

#endif