#ifndef LIBCPP_HTTPMESSAGE_H_
#define LIBCPP_HTTPMESSAGE_H_


#include <unordered_map>

namespace libcpp
{
static std::string unkownMessage = "Unkown";
/*
 * enum name is HTTP_STATUS_#name
 */
class HttpStatus
{
public:
  HttpStatus()
  {
    httpStatus_[100] = "Continue";
    httpStatus_[101] = "Switching Protocols";
    httpStatus_[200] = "OK";
    httpStatus_[201] = "Created";
    httpStatus_[202] = "Accepted";
    httpStatus_[203] = "Non-Authoritative Information";
    httpStatus_[204] = "No Content";
    httpStatus_[205] = "Reset Content";
    httpStatus_[206] = "Partial Content";
    httpStatus_[300] = "Multiple Choices";
    httpStatus_[301] = "Moved Permanently";
    httpStatus_[302] = "Found";
    httpStatus_[303] = "See Other";
    httpStatus_[304] = "Not Modified";
    httpStatus_[305] = "Use Proxy";
    httpStatus_[307] = "Temporary Redirect";
    httpStatus_[400] = "Bad Request";
    httpStatus_[401] = "Unauthorized";
    httpStatus_[402] = "Payment Required";
    httpStatus_[403] = "Forbidden";
    httpStatus_[404] = "Not Found";
    httpStatus_[405] = "Method Not Allowed";
    httpStatus_[406] = "Not Acceptable";
    httpStatus_[407] = "Proxy Authentication Required";
    httpStatus_[408] = "Request Time-out";
    httpStatus_[409] = "Conflict";
    httpStatus_[410] = "Gone";
    httpStatus_[411] = "Length Required";
    httpStatus_[412] = "Precondition Failed";
    httpStatus_[413] = "Request Entity Too Large";
    httpStatus_[414] = "Request-URI Too Large";
    httpStatus_[415] = "Unsupported Media Type";
    httpStatus_[416] = "Requested range not satisfiable";
    httpStatus_[417] = "Expectation Failed";
    httpStatus_[500] = "Internal Server Error";
    httpStatus_[501] = "Not Implemented";
    httpStatus_[502] = "Bad Gateway";
    httpStatus_[503] = "Service Unavailable";
    httpStatus_[504] = "Gateway Time-out";
    httpStatus_[505] = "HTTP Version not supported"; 
  }
  
  const std::string& getMessage(int code) 
  {
    if (httpStatus_.count(code) == 0) {
      return unkownMessage;
    }
    return httpStatus_[statusCode];
  }
  
private:
  std::unordered_map<int, std::string> httpStatus_;
};

static HttpStatus g_httpStatusStrMap;

/*
 * generate http messages
 */
struct HttpRequest
{

};

struct HttpResponse
{

};

}

#endif