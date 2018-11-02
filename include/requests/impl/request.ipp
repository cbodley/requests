#pragma once

#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/http/read.hpp>

#include <requests/uri/view.hpp>

namespace requests {

template <typename RequestBody, typename RequestFields,
          typename ResponseBody, typename ResponseFields>
void request(boost::asio::io_context& ioc, uri_view url,
             boost::beast::http::request<RequestBody, RequestFields>& request,
             boost::beast::http::response<ResponseBody, ResponseFields>& response,
             boost::system::error_code& ec)
{
  auto host = url.host();
  auto port = url.port();
  if (port.empty()) {
    port = (url.scheme() == "https" ? "443" : "80");
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
  request.prepare_payload();
  boost::beast::http::write(socket, request, ec);
  if (ec) {
    return;
  }
  boost::beast::flat_buffer buffer;
  boost::beast::http::read(socket, buffer, response, ec);

  boost::system::error_code ec_ignored;
  socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec_ignored);
}

template <typename RequestBody, typename RequestFields,
          typename ResponseBody, typename ResponseFields,
          typename CompletionToken>
auto async_request(boost::asio::io_context& ioc, uri_view url,
                   boost::beast::http::request<RequestBody, RequestFields>& request,
                   boost::beast::http::response<ResponseBody, ResponseFields>& response,
                   CompletionToken&& token)
{
  using Signature = void(boost::system::error_code);
  boost::asio::async_completion<CompletionToken, Signature> init(token);
  auto& handler = init.completion_handler;
  auto ex2 = boost::asio::get_associated_executor(handler, ioc.get_executor());
  auto alloc = boost::asio::get_associated_allocator(handler);
  boost::system::error_code ec = boost::asio::error::operation_not_supported;
  ex2.post(std::bind(std::move(handler), ec), alloc);
  return init.result.get();
}

} // namespace requests
