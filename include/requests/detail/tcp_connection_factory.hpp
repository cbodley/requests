#pragma once

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <requests/detail/bind_handler.hpp>

namespace requests::detail {

class tcp_connection_factory {
  using tcp = boost::asio::ip::tcp;
  tcp::resolver resolver;
 public:
  using stream_type = tcp::socket;

  tcp_connection_factory(boost::asio::io_context& ioc) : resolver(ioc) {}

  void connect(stream_type& stream,
               boost::asio::string_view host,
               boost::asio::string_view port)
  {
    auto addrs = resolver.resolve(host, port);
    boost::asio::connect(stream, addrs);
  }
  void connect(stream_type& stream,
               boost::asio::string_view host,
               boost::asio::string_view port,
               boost::system::error_code& ec)
  {
    auto addrs = resolver.resolve(host, port, ec);
    if (!ec) {
      boost::asio::connect(stream, addrs, ec);
    }
  }
  void shutdown(stream_type& stream)
  {
    stream.shutdown(tcp::socket::shutdown_both);
  }
  void shutdown(stream_type& stream, boost::system::error_code& ec)
  {
    stream.shutdown(tcp::socket::shutdown_both, ec);
  }
  template <typename CompletionToken> // void(error_code)
  auto async_connect(stream_type& stream,
                     boost::asio::string_view host,
                     boost::asio::string_view port,
                     CompletionToken&& token)
  {
    using Signature = void(boost::system::error_code);
    boost::asio::async_completion<CompletionToken, Signature> init(token);
    auto& h = init.completion_handler;
    resolver.async_resolve(host, port,
      [this, &stream, h = std::move(h)] (boost::system::error_code ec,
                                         tcp::resolver::results_type results) {
        if (ec) {
          auto ex1 = stream.get_executor();
          auto ex2 = boost::asio::get_associated_executor(h, ex1);
          auto alloc = boost::asio::get_associated_allocator(h);
          ex2.dispatch(bind_handler(std::move(h), ec), alloc);
        } else {
          boost::asio::async_connect(stream, results,
            [this, &stream, h = std::move(h)] (boost::system::error_code ec,
                                               tcp::endpoint endpoint) {
              auto ex1 = stream.get_executor();
              auto ex2 = boost::asio::get_associated_executor(h, ex1);
              auto alloc = boost::asio::get_associated_allocator(h);
              ex2.dispatch(bind_handler(std::move(h), ec), alloc);
            });
        }
      });
    return init.result.get();
  }
  template <typename CompletionToken> // void(error_code)
  auto async_shutdown(stream_type& stream, CompletionToken&& token)
  {
    using Signature = void(boost::system::error_code);
    boost::asio::async_completion<CompletionToken, Signature> init(token);
    auto& handler = init.completion_handler;
    boost::system::error_code ec;
    stream.shutdown(stream_type::shutdown_both, ec);
    stream.close(ec);
    auto ex1 = resolver.get_executor();
    auto ex2 = boost::asio::get_associated_executor(handler, ex1);
    auto alloc = boost::asio::get_associated_allocator(handler);
    ex2.post(bind_handler(std::move(handler), ec), alloc);
    return init.result.get();
  }
  void cancel() {
    resolver.cancel();
  }
};

} // namespace requests::detail
