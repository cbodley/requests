#pragma once

#include "fwd.hpp"

namespace boost::asio { class io_context; }

namespace requests {

// request() overloads:
// - request as none, fields, fields+body, or request<>
// - response as none, fields, fields+body, body, or response<>
// - error code or throw
// - sync or async

template <typename RequestBody, typename RequestFields,
          typename ResponseBody, typename ResponseFields>
void request(boost::asio::io_context& ioc, uri_view url,
             boost::beast::http::request<RequestBody, RequestFields>& request,
             boost::beast::http::response<ResponseBody, ResponseFields>& response,
             boost::system::error_code& ec);

template <typename RequestBody, typename RequestFields,
          typename ResponseBody, typename ResponseFields,
          typename CompletionToken>
void async_request(boost::asio::io_context& ioc, uri_view url,
                   boost::beast::http::request<RequestBody, RequestFields>& request,
                   boost::beast::http::response<ResponseBody, ResponseFields>& response,
                   CompletionToken&& token); // signature: void(error_code)

} // namespace requests

#include <requests/impl/request.ipp>
