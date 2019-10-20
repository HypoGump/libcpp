#ifndef LIBCPP_HTTPPARSER_H_
#define LIBCPP_HTTPPARSER_H_

extern "C"
{
#include "http_parser.h"
}

#include "utils/noncopyable.h"

#include <string>

namespace libcpp
{

/*
 * HttpParser: a wrapper to use http_parser
 */
class HttpParser : public utils::noncopyable
{
  friend class HttpParserSettings;
public:
  /*
   * @param parserType
   * - HTTP_REQUEST means parse http request messages
   * - HTTP_RESPONSE means parse http response messages
   * - HTTP_BOTH means parse both messages
   */
  explicit HttpParser(http_parser_type parserType);
  virtual ~HttpParser();
  
  /*
   * corresponding to settings' callbacks
   */
  enum class Event {
    MSG_BEGIN,
    URL,
    STATUS,
    HEADER_FIELD,
    HEADER_VALUE,
    HEADERS_COMPLETE,
    BODY,
    MSG_COMPLETE,
    CHUNK_HEADER,
    CHUNK_COMPLETE
  };
  
  // FIXME: use Buffer class?
  size_t execute(const char *buf, size_t length);
  
  /* setting callbacks interface */
  
protected:
  virtual int onMessageBegin() { return 0; }
  virtual int onUrl(const char*, size_t) { return 0; }
  virtual int onStatus(const char*, size_t) { return 0; }
  virtual int onHeaderField(const char*, size_t) { return 0; }
  virtual int onHeaderValue(const char*, size_t) { return 0; }
  virtual int onHeadersComplete() { return 0; }
  virtual int onBody(const char*, size_t) { return 0; }
  virtual int onMessageComplete() { return 0; }
  
  /* virtual int onChunkHeader() { return 0; }
  virtual int onChunkComplete() { return 0; }
  
  virtual int onUpgrade(const char*, size_t) { return 0; }
  //virtual int onError(const HttpError& error) { return 0; }
  virtual int onHeader(const std::string& field, const std::string& value) { return 0; } */

private:
  /* process parser events */
  int onParser(Event event, const char *at, size_t length);

private:
  http_parser parser_;
};

}

#endif