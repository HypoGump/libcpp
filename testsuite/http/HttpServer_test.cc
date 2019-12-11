#include "http/HttpServer.h"
#include "logging/Logging.h"
#include "reactor/EventLoop.h"
#include "net/InetAddress.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace libcpp;


int onHeaders(const HttpSessionSptr& sess, HttpMessage* msg)
{
  std::string path = ".";
  path += msg->getPath();
  LOG_TRACE << "file path = " << path;
  int fd = open(path.data(), O_RDONLY);

  char buf[512];
  memset(buf, 0, 512);
  read(fd, buf, 512);

  HttpMessage resp;
  resp.appendBody(buf);

  if (sess->shouldKeepAlive()) {
    LOG_TRACE << "Wow! Keep alive";
  }
  else {
    resp.addHeader("Connection", "close");
  }

  sess->send(resp.getResponseAsString());
  return 0;
}

int main()
{
  InetAddress addr(9981);
  EventLoop loop;

  HttpServer server(&loop, addr);
  server.setOnHeadersCallback(onHeaders);
  server.start();

  loop.loop();
}