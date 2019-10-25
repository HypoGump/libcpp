#include "http/HttpServer.h"
#include "logging/Logging.h"
#include "reactor/EventLoop.h"
#include "net/InetAddress.h"

using namespace libcpp;




int main()
{
  InetAddress addr(9981);
  EventLoop loop;
  
  HttpServer server(&loop, addr);
  server.start();
  
  loop.loop();
}