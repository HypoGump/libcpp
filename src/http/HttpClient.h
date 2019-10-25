#ifndef LIBCPP_HTTPCLIENT_H_
#define LIBCPP_HTTPCLIENT_H_

#include "net/TcpClient.h"

namespace libcpp
{

class HttpClient
{
public:
  HttpClient();
  ~HttpClient();

private:
  TcpClient tcpClient_;
};

}

#endif