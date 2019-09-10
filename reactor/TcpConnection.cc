#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

#include "logging/Logging.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>

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
  if (n > 0) {
    messageCallback_(shared_from_this(), buf, n);
  }
  else if (n == 0) {
    handleClose();
  }
  else {
    handleError();
  }
}

void TcpConnection::handleClose()
{
  loop_->assertInLoopThread();
  LOG_TRACE << "TcpConnection::handleClose [" << name_ << "] "
            << "state = " << state_;
  channel_->disableAllEvents();
  closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
  int err = sockets::getSocketError(channel_->fd());
  errno = err;
  LOG_ERROR << "TcpConnection::handleError [" << name_
            << "] - SO_ERROR";
}


void TcpConnection::connectDestroyed()
{
  loop_->assertInLoopThread();
  setConnState(kDisconnected);
  channel_->disableAllEvents();
  connectionCallback_(shared_from_this());
  
  loop_->removeChannel(channel_.get());
}
