#pragma once

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace requests::detail {

class tcp_connection_factory {
  using tcp = boost::asio::ip::tcp;
 public:
  using stream_type = tcp::socket;

  void connect(stream_type& stream,
               boost::asio::string_view host,
               boost::asio::string_view port)
  {
    auto resolver = tcp::resolver(stream.get_io_context());
    auto addrs = resolver.resolve(host, port);
    boost::asio::connect(stream, addrs);
  }
  void connect(stream_type& stream,
               boost::asio::string_view host,
               boost::asio::string_view port,
               boost::system::error_code& ec)
  {
    auto resolver = tcp::resolver(stream.get_io_context());
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
                     CompletionToken&& token);
  template <typename CompletionToken> // void(error_code)
  auto async_shutdown(stream_type& stream, CompletionToken&& token);
};

} // namespace requests::detail
