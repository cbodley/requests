#pragma once

#include <requests/detail/tcp_connection_factory.hpp>

namespace requests::detail {

template <typename Stream>
class ssl_connection_factory : private tcp_connection_factory {
 public:
  using stream_type = Stream;

  void connect(stream_type& stream,
               boost::asio::string_view host,
               boost::asio::string_view port)
  {
    tcp_connection_factory::connect(stream.next_layer(), host, port);
    stream.handshake(stream_type::client);
  }
  void connect(stream_type& stream,
               boost::asio::string_view host,
               boost::asio::string_view port,
               boost::system::error_code& ec)
  {
    tcp_connection_factory::connect(stream.next_layer(), host, port, ec);
    if (!ec) {
      stream.handshake(stream_type::client, ec);
    }
  }
  void shutdown(stream_type& stream)
  {
    stream.shutdown();
    tcp_connection_factory::shutdown(stream.next_layer());
  }
  void shutdown(stream_type& stream, boost::system::error_code& ec)
  {
    stream.shutdown(ec);
    tcp_connection_factory::shutdown(stream.next_layer(), ec);
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
