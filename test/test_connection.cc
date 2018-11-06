#include <requests/connection.hpp>
#include <thread>
#include <boost/beast.hpp>
#include <boost/asio/spawn.hpp>
#include "echo_stream.hpp"
#include "error_stream.hpp"
#include <gtest/gtest.h>

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

// stream that combines two separate streams
template <typename InStream, typename OutStream>
struct basic_iostream {
  InStream& in;
  OutStream& out;
  basic_iostream(InStream& in, OutStream& out) : in(in), out(out) {}

  using executor_type = typename InStream::executor_type;
  executor_type get_executor() const noexcept { return in.get_executor(); }

  using next_layer_type = typename InStream::next_layer_type;
  next_layer_type& next_layer() { return in; }

  using lowest_layer_type = typename InStream::lowest_layer_type;
  lowest_layer_type& lowest_layer() { return in.lowest_layer(); }

  template <typename MutableBufferSequence>
  size_t read_some(const MutableBufferSequence& buffers) {
    static_assert(boost::beast::is_sync_read_stream<InStream>::value,
                  "SyncReadStream requirements are not met.");
    return in.read_some(buffers);
  }
  template <typename MutableBufferSequence>
  size_t read_some(const MutableBufferSequence& buffers,
                   boost::system::error_code& ec) {
    static_assert(boost::beast::is_sync_read_stream<InStream>::value,
                  "SyncReadStream requirements are not met.");
    return in.read_some(buffers, ec);
  }
  template <typename ConstBufferSequence>
  size_t write_some(const ConstBufferSequence& buffers) {
    static_assert(boost::beast::is_sync_write_stream<OutStream>::value,
                  "SyncWriteStream requirements are not met.");
    return out.write_some(buffers);
  }
  template <typename ConstBufferSequence>
  size_t write_some(const ConstBufferSequence& buffers,
                   boost::system::error_code& ec) {
    static_assert(boost::beast::is_sync_write_stream<OutStream>::value,
                  "SyncWriteStream requirements are not met.");
    return out.write_some(buffers, ec);
  }
  template <typename MutableBufferSequence, typename ReadHandler>
  auto async_read_some(const MutableBufferSequence& buffers,
                       ReadHandler&& token) {
    static_assert(boost::beast::is_async_read_stream<InStream>::value,
                  "AsyncReadStream requirements are not met.");
    return in.async_read_some(buffers, std::forward<ReadHandler>(token));
  }
  template <typename ConstBufferSequence, typename WriteHandler>
  auto async_write_some(const ConstBufferSequence& buffers,
                        WriteHandler&& token) {
    static_assert(boost::beast::is_async_write_stream<OutStream>::value,
                  "AsyncWriteStream requirements are not met.");
    return out.async_write_some(buffers, std::forward<WriteHandler>(token));
  }
};


TEST(http_connection, error_stream)
{
  boost::asio::io_context ioc;
  auto in = requests::error_stream(ioc.get_executor(), boost::asio::error::eof);
  auto out = requests::error_stream(ioc.get_executor(), boost::asio::error::connection_reset);
  auto stream = basic_iostream(in, out);
  char c = 'a';
  boost::system::error_code ec;
  stream.write_some(boost::asio::buffer(&c, 1), ec);
  EXPECT_EQ(boost::asio::error::connection_reset, ec);
  stream.read_some(boost::asio::buffer(&c, 1), ec);
  EXPECT_EQ(boost::asio::error::eof, ec);
}

TEST(http_connection, test_stream)
{
  boost::asio::io_context ioc;
  auto in = requests::echo_stream(ioc.get_executor(), 8);
  auto out = requests::echo_stream(ioc.get_executor(), 8);
  auto client = basic_iostream(in, out);
  auto server = basic_iostream(out, in);
  boost::system::error_code ec;
  {
    char c = 'a';
    client.write_some(boost::asio::buffer(&c, 1), ec);
    EXPECT_FALSE(ec);
  }
  {
    char c = 0;
    server.read_some(boost::asio::buffer(&c, 1), ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ('a', c);
  }
  {
    char c = 'b';
    server.write_some(boost::asio::buffer(&c, 1), ec);
    EXPECT_FALSE(ec);
  }
  {
    char c = 0;
    client.read_some(boost::asio::buffer(&c, 1), ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ('b', c);
  }
}

TEST(http_connection, connection)
{
  boost::asio::io_context ioc;
  auto in = requests::echo_stream(ioc.get_executor());
  auto out = requests::echo_stream(ioc.get_executor());
  auto client = basic_iostream(in, out);
  auto server = basic_iostream(out, in);

  using body = boost::beast::http::string_body;
  boost::asio::spawn(ioc, [&] (boost::asio::yield_context yield) {
      requests::basic_http_connection connection(std::move(server));
      http::request<body> request;
      connection.async_read(request, yield);
      EXPECT_EQ(http::verb::get, request.method());
      EXPECT_EQ("hello", request.body());
      http::response<body> response(http::status::ok, request.version(), "world");
      response.prepare_payload();
      connection.async_write(response, yield);
    });
  std::thread thread([&ioc] { EXPECT_NO_THROW(ioc.run()); });

  boost::system::error_code ec;

  requests::basic_http_connection connection(std::move(client));

  http::request<body> request(http::verb::get, "/", 11, "hello");
  request.prepare_payload();
  connection.write(request, ec);
  ASSERT_FALSE(ec);

  http::response<body> response;
  connection.read(response, ec);
  ASSERT_FALSE(ec);
  EXPECT_EQ("world", response.body());
  EXPECT_EQ(http::status::ok, response.result());

  thread.join();
}
