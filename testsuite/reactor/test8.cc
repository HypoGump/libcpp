#include "reactor/TcpServer.h"
#include "reactor/EventLoop.h"
#include "reactor/InetAddress.h"
#include <stdio.h>

void onConnection(const libcpp::TcpConnPtr& conn)
{
  if (conn->connected())
  {
    printf("onConnection(): new connection [%s] from %s\n",
           conn->name().c_str(),
           conn->peerAddress().toHostPort().c_str());
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn->name().c_str());
  }
}

void onMessage(const libcpp::TcpConnPtr& conn,
               const char* data,
               ssize_t len)
{
  printf("onMessage(): received %zd bytes from connection [%s]\n",
         len, conn->name().c_str());
}

int main()
{
  printf("main()\n");

  libcpp::InetAddress listenAddr(9981);
  libcpp::EventLoop loop;

  libcpp::TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();

  loop.loop();
}