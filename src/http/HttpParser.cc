#include "HttpParser.h"
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
  HttpParser *p = static_cast<HttpParser*>(parser->data);
  return p->onParser(HttpParser::Event::MSG_BEGIN, nullptr, 0);
}

int HttpParserSettings::onUrl(http_parser *parser, const char *at, size_t length)
{
  HttpParser *p = static_cast<HttpParser*>(parser->data);
  return p->onParser(HttpParser::Event::URL, at, length);
}

int HttpParserSettings::onStatus(http_parser *parser, const char *at, size_t length)
{
  HttpParser *p = static_cast<HttpParser*>(parser->data);
  return p->onParser(HttpParser::Event::STATUS, at, length);
}

int HttpParserSettings::onHeaderField(http_parser *parser, const char *at, size_t length)
{
  HttpParser *p = static_cast<HttpParser*>(parser->data);
  return p->onParser(HttpParser::Event::HEADER_FIELD, at, length);
}

int HttpParserSettings::onHeaderValue(http_parser *parser, const char *at, size_t length)
{
  HttpParser *p = static_cast<HttpParser*>(parser->data);
  return p->onParser(HttpParser::Event::HEADER_VALUE, at, length);
}

int HttpParserSettings::onHeadersComplete(http_parser *parser)
{
  HttpParser *p = static_cast<HttpParser*>(parser->data);
  return p->onParser(HttpParser::Event::HEADERS_COMPLETE, nullptr, 0);
}

int HttpParserSettings::onBody(http_parser *parser, const char *at, size_t length)
{
  HttpParser *p = static_cast<HttpParser*>(parser->data);
  return p->onParser(HttpParser::Event::BODY, at, length);
}

int HttpParserSettings::onMessageComplete(http_parser *parser)
{
  HttpParser *p = static_cast<HttpParser*>(parser->data);
  return p->onParser(HttpParser::Event::MSG_COMPLETE, nullptr, 0);
}

int HttpParserSettings::onChunkHeader(http_parser *parser)
{
  HttpParser *p = static_cast<HttpParser*>(parser->data);
  return p->onParser(HttpParser::Event::CHUNK_HEADER, nullptr, 0);
}

int HttpParserSettings::onChunkComplete(http_parser *parser)
{
  HttpParser *p = static_cast<HttpParser*>(parser->data);
  return p->onParser(HttpParser::Event::CHUNK_COMPLETE, nullptr, 0);
}

HttpParserSettings initObj;

} /* namespace libcpp */

using namespace libcpp;

HttpParser::HttpParser(http_parser_type parserType)
{
  http_parser_init(&parser_, parserType);
  parser_.data = this;
}

HttpParser::~HttpParser()
{
}

size_t HttpParser::execute(const char *buf , size_t length)
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

int HttpParser::onParser(Event event, const char *at, size_t length)
{
  switch(event) {
    case Event::MSG_BEGIN:
      return onMessageBegin();
    case Event::URL:
      return onUrl(at, length);
    case Event::STATUS:
      return onStatus(at, length);
    case Event::HEADER_FIELD:
      return onHeaderField(at, length);
    case Event::HEADER_VALUE:
      return onHeaderValue(at, length);
    case Event::HEADERS_COMPLETE:
      return onHeadersComplete();
    case Event::BODY:
      return onBody(at, length);
    case Event::MSG_COMPLETE:
      return onMessageComplete();
    /* case Event::CHUNK_HEADER:
      return onChunkHeader();
    case Event::CHUNK_COMPLETE:
      return onChunkComplete(); */
    default:
      break;
  }
  
  return 0;
}