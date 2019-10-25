#include "HttpSession.h"
#include "logging/Logging.h"


namespace libcpp
{
http_parser_settings g_parser_settings;

class HttpParserSettings
{
public:
  HttpParserSettings()
  {
    g_parser_settings.on_message_begin = &onMessageBegin;
    g_parser_settings.on_url = &onUrl;
    g_parser_settings.on_status = &onStatus;
    g_parser_settings.on_header_field = &onHeaderField;
    g_parser_settings.on_header_value = &onHeaderValue;
    g_parser_settings.on_headers_complete = &onHeadersComplete;
    g_parser_settings.on_body = &onBody;
    g_parser_settings.on_message_complete = &onMessageComplete;
    g_parser_settings.on_chunk_header = &onChunkHeader;
    g_parser_settings.on_chunk_complete = &onChunkComplete;
  }
  
  /*
   * process callbacks
   */
  static int onMessageBegin(http_parser *parser);
  static int onUrl(http_parser *parser, const char *at, size_t length);
  static int onStatus(http_parser *parser, const char *at, size_t length);
  static int onHeaderField(http_parser *parser, const char *at, size_t length);
  static int onHeaderValue(http_parser *parser, const char *at, size_t length);
  static int onHeadersComplete(http_parser *parser);
  static int onBody(http_parser *parser, const char *at, size_t length);
  static int onMessageComplete(http_parser *parser);
  static int onChunkHeader(http_parser *parser);
  static int onChunkComplete(http_parser *parser);
};

int HttpParserSettings::onMessageBegin(http_parser *parser)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onMessageBegin();
}

int HttpParserSettings::onUrl(http_parser *parser, const char *at, size_t length)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onUrl(at, length);
}

int HttpParserSettings::onStatus(http_parser *parser, const char *at, size_t length)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onStatus(at, length);
}

int HttpParserSettings::onHeaderField(http_parser *parser, const char *at, size_t length)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onHeaderField(at, length);
}

int HttpParserSettings::onHeaderValue(http_parser *parser, const char *at, size_t length)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onHeaderValue(at, length);
}

int HttpParserSettings::onHeadersComplete(http_parser *parser)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onHeadersComplete();
}

int HttpParserSettings::onBody(http_parser *parser, const char *at, size_t length)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onBody(at, length);
}

int HttpParserSettings::onMessageComplete(http_parser *parser)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onMessageComplete();
}

int HttpParserSettings::onChunkHeader(http_parser *parser)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onChunkHeader();
}

int HttpParserSettings::onChunkComplete(http_parser *parser)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onChunkComplete();
}

HttpParserSettings initObj;

} /* namespace libcpp */



using namespace libcpp;

HttpSession::HttpSession(const TcpConnSptr& conn, http_parser_type parserType)
 :  tcpConn_(conn)
{
  http_parser_init(&parser_, parserType);
  parser_.data = this;
}

HttpSession::~HttpSession()
{
}

size_t HttpSession::execute(const char *buf , size_t length)
{
  /* it will call the callbacks */
  size_t n = http_parser_execute(&parser_, &g_parser_settings, buf, length);
  
  if (parser_.upgrade) {
    /* handle new protocol */
  }
  else if (n != length) {
    /* handle error. Usually close the connection */
  }
  return 0;
}


int HttpSession::onMessageBegin()
{
  LOG_TRACE << "Message Begin";
  curField_.clear();
  curValue_.clear();
  headers_.clear();
  lastIsValue_ = true;
  return 0;
}

int HttpSession::onUrl(const char* at, size_t length)
{
  LOG_TRACE << std::string(at, length);
  return 0;
}

int HttpSession::onStatus(const char* at, size_t length)
{
  LOG_TRACE << std::string(at, length);
  return 0;
}

int HttpSession::onHeaderField(const char* at, size_t length)
{
  LOG_TRACE << std::string(at, length);
  
  if (lastIsValue_) {
    if (!curField_.empty()) {
      // FIXME: here maybe process field-value pair immediately
      headers_[curField_] = curValue_;
    }
    curField_.clear();
    curValue_.clear();
  }
  
  curField_.append(at, length);
  lastIsValue_ = false;
  return 0;
}

int HttpSession::onHeaderValue(const char* at, size_t length)
{
  LOG_TRACE << std::string(at, length);
  curValue_.append(at, length);
  lastIsValue_ = true;
  return 0;
}

int HttpSession::onHeadersComplete()
{
  LOG_TRACE << "Header Complete";
  return 0;
}

int HttpSession::onBody(const char* at, size_t length)
{
  LOG_TRACE << "on body";
  /* callback to process body */
  return 0;
}

int HttpSession::onMessageComplete()
{
  LOG_TRACE << "Message Complete";
  /* response */
  return 0;
}

