#pragma once

#include <requests/request.hpp>

namespace requests {

struct cookies {
  template <typename Fields>
  void prepare(Fields& fields) const {}
};

// http session object that can persist cookies and request headers over
// several requests
template <typename Fields = boost::beast::http::fields,
          typename Cookies = cookies>
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
  session.prepare_request(request);
  requests::request(ioc, url, request, response, ec);
}

template <typename SessionFields, typename Cookies,
          typename RequestBody, typename RequestFields,
          typename ResponseBody, typename ResponseFields,
          typename CompletionToken> // signature: void(error_code)
auto async_request(boost::asio::io_context& ioc,
                   http_session<SessionFields, Cookies>& session, uri_view url,
                   boost::beast::http::request<RequestBody, RequestFields>& request,
                   boost::beast::http::response<ResponseBody, ResponseFields>& response,
                   CompletionToken&& token)
{
  session.prepare_request(request);
  return requests::request(ioc, url, request, response,
                           std::forward<CompletionToken>(token));
}

} // namespace requests
