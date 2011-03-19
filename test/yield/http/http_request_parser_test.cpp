#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_request_parser.hpp"
#include "yunit.hpp"

TEST_SUITE(HTTPRequestParser);

namespace yield {
namespace http {
TEST(HTTPRequestParser, MalformedHTTPVersionMissing) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET /\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, MalformedHTTPVersionMissingHTTP) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / /1.0\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, MalformedHTTPVersionMissingMinorVersion) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, MalformedHTTPVersionMissingTrailingCRLF) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.1Host: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, MalformedMethodMissing) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("/ HTTP/1.0\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, MalformedURIEmbeddedLF) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET /\r HTTP/1.1\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, MalformedURIMissing) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET HTTP/1.1\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, WellFormedChunk1Body) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n0\r\n\r\n");
  vector<HTTPRequest*> http_requests;

  for (uint32_t i = 0; i < 2; i++) {
    HTTPRequest* http_request = object_cast<HTTPRequest>(http_request_parser.parse());

    if (http_request != NULL)
      http_requests.push_back(http_request);
    else {
      while (!http_requests.empty()) {
        HTTPRequest::dec_ref(*http_requests.back());
        http_requests.pop_back();
      }

      throw_assert(false);
    }
  }

  throw_assert_eq(http_requests.size(), 2);
  for (size_t i = 0; i < 2; i++) { throw_assert_eq(http_requests[i]->get_method(), HTTPRequest::METHOD_GET); }
  for (size_t i = 0; i < 2; i++) { throw_assert_ne(http_requests[i]->get_body(), NULL); }
  throw_assert_eq(http_requests[0]->get_content_length(), 1);
  throw_assert_eq(http_requests[1]->get_content_length(), 0);
  for (size_t i = 0; i < 2; i++) HTTPRequest::dec_ref(*http_requests[i]);
}

TEST(HTTPRequestParser, WellFormedChunk2Body) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n1\r\ny\r\n0\r\n\r\n");
  vector<HTTPRequest*> http_requests;

  for (uint32_t i = 0; i < 3; i++) {
    HTTPRequest* http_request = object_cast<HTTPRequest>(http_request_parser.parse());

    if (http_request != NULL)
      http_requests.push_back(http_request);
    else {
      while (!http_requests.empty()) {
        HTTPRequest::dec_ref(*http_requests.back());
        http_requests.pop_back();
      }

      throw_assert(false);
    }
  }

  throw_assert_eq(http_requests.size(), 3);
  for (size_t i = 0; i < 3; i++) { throw_assert_eq(http_requests[i]->get_method(), HTTPRequest::METHOD_GET); }
  for (size_t i = 0; i < 3; i++) { throw_assert_ne(http_requests[i]->get_body(), NULL); }
  throw_assert_eq(http_requests[0]->get_content_length(), 1);
  throw_assert_eq(http_requests[1]->get_content_length(), 1);
  throw_assert_eq(http_requests[2]->get_content_length(), 0);
  for (size_t i = 0; i < 3; i++) HTTPRequest::dec_ref(*http_requests[i]);
}

TEST(HTTPRequestParser, WellFormedNoBody) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_eq(http_request->get_method(), HTTPRequest::METHOD_GET);
  throw_assert_eq(http_request->get_http_version(), 1.1F);
  throw_assert_eq((*http_request)["Host"], "localhost");
  throw_assert_eq(http_request->get_body(), NULL);
  HTTPRequest::dec_ref(*http_request);
}

TEST(HTTPRequestParser, WellFormedNoFields) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.1\r\n\r\n").parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_eq(http_request->get_http_version(), 1.1F);
  throw_assert_eq(http_request->get_body(), NULL);
  HTTPRequest::dec_ref(*http_request);
}

TEST(HTTPRequestParser, WellFormedNormalBody) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12").parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_ne(http_request->get_body(), NULL);
  throw_assert_eq(http_request->get_content_length(), 2);
  HTTPRequest::dec_ref(*http_request);
}

TEST(HTTPRequestParser, WellFormedPipelinedNoBody) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\n\r\nGET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
  vector<HTTPRequest*> http_requests;

  for (uint32_t i = 0; i < 2; i++) {
    HTTPRequest* http_request = object_cast<HTTPRequest>(http_request_parser.parse());

    if (http_request != NULL)
      http_requests.push_back(http_request);
    else {
      while (!http_requests.empty()) {
        HTTPRequest::dec_ref(*http_requests.back());
        http_requests.pop_back();
      }

      throw_assert(false);
    }
  }

  throw_assert_eq(http_requests.size(), 2);
  for (size_t i = 0; i < 2; i++) { throw_assert_eq(http_requests[i]->get_method(), HTTPRequest::METHOD_GET); }
  for (size_t i = 0; i < 2; i++) { throw_assert_eq(http_requests[i]->get_body(), NULL); }
  for (size_t i = 0; i < 2; i++) HTTPRequest::dec_ref(*http_requests[i]);
}

TEST(HTTPRequestParser, WellFormedPipelinedNormalBody) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12");
  vector<HTTPRequest*> http_requests;

  for (uint32_t i = 0; i < 2; i++) {
    HTTPRequest* http_request = object_cast<HTTPRequest>(http_request_parser.parse());

    if (http_request != NULL)
      http_requests.push_back(http_request);
    else {
      while (!http_requests.empty()) {
        HTTPRequest::dec_ref(*http_requests.back());
        http_requests.pop_back();
      }

      throw_assert(false);
    }
  }

  throw_assert_eq(http_requests.size(), 2);
  for (size_t i = 0; i < 2; i++) { throw_assert_eq(http_requests[i]->get_method(), HTTPRequest::METHOD_GET); }
  for (size_t i = 0; i < 2; i++) { throw_assert_ne(http_requests[i]->get_body(), NULL); }
  for (size_t i = 0; i < 2; i++) { throw_assert_eq(http_requests[i]->get_content_length(), 2); }
  for (size_t i = 0; i < 2; i++) HTTPRequest::dec_ref(*http_requests[i]);
}

}
}

