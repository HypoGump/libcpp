#include "EventLoop.h"
#include "TcpServer.h"
#include "Acceptor.h"

#include "logging/Logging.h"

#include <string>
#include <stdio.h>

using namespace libcpp;


TcpServer::TcpServer(EventLoop* loop, const InetAddress& addr, 
                    const std::string& name)
  : loop_(loop),
    name_(name),
    acceptor_(new Acceptor(loop, addr)),
    started_(false),
    nextConnId_(1)
{
  using namespace std::placeholders;
  acceptor_->setNewConnCallback(
        std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
  if (!started_) {
    started_ = true;
  }
  
  if (!acceptor_->listening()) {
    loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
  }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
  loop_->assertInLoopThread();
  char buf[32];
  ::snprintf(buf, sizeof(buf), "#%d", nextConnId_);
  ++nextConnId_;
  std::string connName = name_ + buf;
  
  LOG_INFO << "TcpServer [" << name_ << "] - newConnection ["
            << connName << "] from " << peerAddr.toHostPort();
  
  InetAddress localAddr(sockets::getLocalAddr(sockfd));
  /* create Tcp connection */
  TcpConnPtr conn = std::make_shared<TcpConnection>(loop_, 
                                  connName, sockfd, localAddr, peerAddr);
  connections_[connName] = conn;
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->connectEstablished();
}
