#pragma once

#include <utility>

#include <boost/asio/ip/tcp.hpp>

#include <boost/beast/core/multi_buffer.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/serializer.hpp>
#ifdef REQUESTS_ENABLE_SSL
#include <boost/beast/experimental/core/ssl_stream.hpp>
#endif

namespace requests {

template <typename Stream, typename DynamicBuffer = boost::beast::multi_buffer>
class basic_http_connection {
  using stream_type = Stream;
  stream_type stream;
  using buffer_type = DynamicBuffer;
  buffer_type buffer;
 public:
  basic_http_connection(stream_type&& stream,
                        buffer_type&& buffer = buffer_type())
    : stream(std::move(stream)), buffer(std::move(buffer)) {}

  // XXX: executor_type not technically part of the async stream concepts
  using executor_type = typename stream_type::executor_type;
  executor_type get_executor() { return stream.get_executor(); }

  using next_layer_type = stream_type;
  next_layer_type& next_layer() { return stream; }

  using lowest_layer_type = typename stream_type::lowest_layer_type;
  lowest_layer_type& lowest_layer() { return stream.lowest_layer(); }

  basic_http_connection(basic_http_connection&&) = default;
  basic_http_connection& operator=(basic_http_connection&&) = default;

  template <bool isRequest, typename Body, typename Fields>
  size_t read(boost::beast::http::message<isRequest, Body, Fields>& message);
  template <bool isRequest, typename Body, typename Fields>
  size_t read(boost::beast::http::message<isRequest, Body, Fields>& message,
              boost::system::error_code& ec);

  template <bool isRequest, typename Derived>
  size_t read(boost::beast::http::basic_parser<isRequest, Derived>& parser);
  template <bool isRequest, typename Derived>
  size_t read(boost::beast::http::basic_parser<isRequest, Derived>& parser,
              boost::system::error_code& ec);

  template <bool isRequest, typename Derived>
  size_t read_header(boost::beast::http::basic_parser<isRequest, Derived>& parser);
  template <bool isRequest, typename Derived>
  size_t read_header(boost::beast::http::basic_parser<isRequest, Derived>& parser,
                     boost::system::error_code& ec);

  template <bool isRequest, typename Derived>
  size_t read_some(boost::beast::http::basic_parser<isRequest, Derived>& parser);
  template <bool isRequest, typename Derived>
  size_t read_some(boost::beast::http::basic_parser<isRequest, Derived>& parser,
                   boost::system::error_code& ec);

  template <bool isRequest, typename Body, typename Fields>
  size_t write(boost::beast::http::message<isRequest, Body, Fields>& message);
  template <bool isRequest, typename Body, typename Fields>
  size_t write(boost::beast::http::message<isRequest, Body, Fields>& message,
               boost::system::error_code& ec);

  template <bool isRequest, typename Body, typename Fields>
  size_t write(boost::beast::http::serializer<isRequest, Body, Fields>& serializer);
  template <bool isRequest, typename Body, typename Fields>
  size_t write(boost::beast::http::serializer<isRequest, Body, Fields>& serializer,
               boost::system::error_code& ec);

  template <bool isRequest, typename Body, typename Fields>
  size_t write_header(boost::beast::http::serializer<isRequest, Body, Fields>& serializer);
  template <bool isRequest, typename Body, typename Fields>
  size_t write_header(boost::beast::http::serializer<isRequest, Body, Fields>& serializer,
                      boost::system::error_code& ec);

  template <bool isRequest, typename Body, typename Fields>
  size_t write_some(boost::beast::http::serializer<isRequest, Body, Fields>& serializer);
  template <bool isRequest, typename Body, typename Fields>
  size_t write_some(boost::beast::http::serializer<isRequest, Body, Fields>& serializer,
                    boost::system::error_code& ec);


  template <bool isRequest, typename Body, typename Fields, typename ReadHandler>
  auto async_read(boost::beast::http::message<isRequest, Body, Fields>& message,
                  ReadHandler&& token);

  template <bool isRequest, typename Derived, typename ReadHandler>
  auto async_read(boost::beast::http::basic_parser<isRequest, Derived>& parser,
                  ReadHandler&& token);

  template <bool isRequest, typename Derived, typename ReadHandler>
  auto async_read_header(boost::beast::http::basic_parser<isRequest, Derived>& parser,
                         ReadHandler&& token);

  template <bool isRequest, typename Derived, typename ReadHandler>
  auto async_read_some(boost::beast::http::basic_parser<isRequest, Derived>& parser,
                       ReadHandler&& token);

  template <bool isRequest, typename Body, typename Fields, typename WriteHandler>
  auto async_write(boost::beast::http::message<isRequest, Body, Fields>& message,
                   WriteHandler&& token);

  template <bool isRequest, typename Body, typename Fields, typename WriteHandler>
  auto async_write(boost::beast::http::serializer<isRequest, Body, Fields>& serializer,
                   WriteHandler&& token);

  template <bool isRequest, typename Body, typename Fields, typename WriteHandler>
  auto async_write_header(boost::beast::http::serializer<isRequest, Body, Fields>& serializer,
                          WriteHandler&& token);

  template <bool isRequest, typename Body, typename Fields, typename WriteHandler>
  auto async_write_some(boost::beast::http::serializer<isRequest, Body, Fields>& serializer,
                        WriteHandler&& token);
};

using tcp_stream = boost::asio::ip::tcp::socket;
using http_connection = basic_http_connection<tcp_stream>;

#ifdef REQUESTS_ENABLE_SSL
using ssl_stream = boost::beast::experimental::ssl_stream<tcp_socket>;
using https_connection = basic_http_connection<ssl_stream>;
#endif
} // namespace requests

#include <requests/impl/connection.ipp>
