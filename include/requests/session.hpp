#pragma once

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/http/read.hpp>

#include "uri.hpp"

namespace boost::asio {
class io_context;
}

namespace requests {

struct cookies {
  template <typename Fields>
  void prepare(Fields& fields) const {}
};

// http session object that can persist cookies and request headers over
// several requests
template <typename Fields = boost::beast::http::fields, typename Cookies = cookies>
class http_session {
  using fields_type = Fields;
  fields_type fields_;
  using cookies_type = Cookies;
  cookies_type cookies_;
 public:
  fields_type& fields() { return fields_; }
  const fields_type& fields() const { return fields_; }

  cookies_type& cookies() { return cookies_; }
  const cookies_type& cookies() const { return cookies_; }

  // apply the persistent session state to the given request fields
  template <typename UFields>
  void prepare_request(UFields& fields) const {
    cookies_.prepare(fields);
    // TODO: set fields_
  }
};

template <typename SessionFields, typename Cookies,
          typename RequestBody, typename RequestFields,
          typename ResponseBody, typename ResponseFields>
void request(boost::asio::io_context& ioc,
             http_session<SessionFields, Cookies>& session, uri_view url,
             boost::beast::http::request<RequestBody, RequestFields>& request,
             boost::beast::http::response<ResponseBody, ResponseFields>& response,
             boost::system::error_code& ec)
{
  auto host = url.host();
  auto port = url.port();
  if (port.empty()) {
    auto scheme = url.scheme();
    port = (scheme == "https" ? "443" : "80");
  }
  auto resolver = boost::asio::ip::tcp::resolver(ioc);
  auto addrs = resolver.resolve(
      boost::asio::string_view{host.data(), host.size()},
      boost::asio::string_view{port.data(), port.size()}, ec);
  if (ec) {
    return;
  }
  auto socket = boost::asio::ip::tcp::socket(ioc);
  auto endpoint = boost::asio::connect(socket, addrs, ec);
  if (ec) {
    return;
  }
  session.prepare_request(request);
  boost::beast::http::write(socket, request, ec);
  if (ec) {
    return;
  }
}

template <typename SessionFields, typename Cookies,
          typename RequestBody, typename RequestFields,
          typename ResponseBody, typename ResponseFields,
          typename CompletionToken> // signature: void(error_code)
auto async_request(boost::asio::io_context& ioc,
                   http_session<SessionFields, Cookies>& session,
                   boost::asio::string_view url,
                   boost::beast::http::request<RequestBody, RequestFields>& request,
                   boost::beast::http::response<ResponseBody, ResponseFields>& response,
                   CompletionToken&& token);

} // namespace requests
