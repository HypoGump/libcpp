#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

#include "logging/Logging.h"

#include <unistd.h>
#include <assert.h>

using namespace libcpp;


TcpConnection::TcpConnection(EventLoop* loop,
                      const std::string& name,
                      int sockfd,
                      const InetAddress& localAddr,
                      const InetAddress& peerAddr)
  : loop_(loop),
    name_(name),
    state_(kConnecting),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr)
{
  LOG_DEBUG << "TcpConnection::ctor[" <<  name_ << "] at " << this
            << " fd=" << sockfd;
  channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
}


TcpConnection::~TcpConnection()
{
  LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this
            << " fd=" << channel_->fd();
}


void TcpConnection::connectEstablished()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnecting);
  setConnState(kConnected);
  channel_->enableReading();

  connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead()
{
  char buf[65536];
  ssize_t n = ::read(channel_->fd(), buf, sizeof buf);
  messageCallback_(shared_from_this(), buf, n);
}

