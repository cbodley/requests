#include <requests/connection.hpp>
#include <thread>
#include <boost/beast.hpp>
#include <boost/asio/spawn.hpp>
#include "echo_stream.hpp"
#include "error_stream.hpp"
#include "joined_stream.hpp"
#include <gtest/gtest.h>

using boost::asio::ip::tcp;
namespace http = boost::beast::http;
using boost::asio::error::connection_reset;
using body = http::string_body;

namespace requests {

TEST(http_connection, sync)
{
  boost::asio::io_context ioc;
  auto in = echo_stream(ioc.get_executor());
  auto out = echo_stream(ioc.get_executor());
  auto client = basic_http_connection(join_streams(in, out));
  auto server = basic_http_connection(join_streams(out, in));

  boost::system::error_code ec;
  {
    http::request<body> request(http::verb::get, "/", 11, "hello");
    request.prepare_payload();
    client.write(request, ec);
    ASSERT_FALSE(ec);
  }
  {
    http::request<body> request;
    server.read(request, ec);
    ASSERT_FALSE(ec);
    EXPECT_EQ(http::verb::get, request.method());
    EXPECT_EQ("hello", request.body());

    http::response<body> response(http::status::ok, request.version(), "world");
    response.prepare_payload();
    server.write(response, ec);
    ASSERT_FALSE(ec);
  }
  {
    http::response<body> response;
    client.read(response, ec);
    ASSERT_FALSE(ec);
    EXPECT_EQ("world", response.body());
    EXPECT_EQ(http::status::ok, response.result());
  }
}

TEST(http_connection, async)
{
  boost::asio::io_context ioc;
  auto in = echo_stream(ioc.get_executor());
  auto out = echo_stream(ioc.get_executor());
  auto client = basic_http_connection(join_streams(in, out));
  auto server = basic_http_connection(join_streams(out, in));

  boost::asio::spawn(ioc, [&server] (boost::asio::yield_context yield) {
      http::request<body> request;
      server.async_read(request, yield);
      EXPECT_EQ(http::verb::get, request.method());
      EXPECT_EQ("hello", request.body());

      http::response<body> response(http::status::ok, request.version(), "world");
      response.prepare_payload();
      server.async_write(response, yield);
    });

  boost::asio::spawn(ioc, [&client] (boost::asio::yield_context yield) {
      http::request<body> request(http::verb::get, "/", 11, "hello");
      request.prepare_payload();
      client.async_write(request, yield);

      http::response<body> response;
      client.async_read(response, yield);
      EXPECT_EQ("world", response.body());
      EXPECT_EQ(http::status::ok, response.result());
    });

  EXPECT_NO_THROW(ioc.run());
}

static boost::system::error_code& clear_ec(boost::system::error_code& ec)
{
  ec.clear();
  return ec;
}

TEST(http_connection, read_errors)
{
  boost::asio::io_context ioc;
  auto connection = basic_http_connection(
      error_stream(ioc.get_executor(), connection_reset));

  http::request<body> request;
  http::response<body> response;
  http::parser<true, body> parser(request);

  boost::system::error_code ec;

  EXPECT_THROW(connection.read(request), boost::system::system_error);
  connection.read(request, ec);
  EXPECT_EQ(connection_reset, ec);

  EXPECT_THROW(connection.read(response), boost::system::system_error);
  connection.read(response, clear_ec(ec));
  EXPECT_EQ(connection_reset, ec);

  EXPECT_THROW(connection.read(parser), boost::system::system_error);
  connection.read(parser, clear_ec(ec));
  EXPECT_EQ(connection_reset, ec);

  EXPECT_THROW(connection.read_header(parser), boost::system::system_error);
  connection.read_header(parser, clear_ec(ec));
  EXPECT_EQ(connection_reset, ec);

  EXPECT_THROW(connection.read_some(parser), boost::system::system_error);
  connection.read_some(parser, clear_ec(ec));
  EXPECT_EQ(connection_reset, ec);

  boost::asio::spawn(ioc, [&] (boost::asio::yield_context yield) {
      connection.async_read(request, yield[clear_ec(ec)]);
      EXPECT_EQ(connection_reset, ec);

      connection.async_read(response, yield[clear_ec(ec)]);
      EXPECT_EQ(connection_reset, ec);

      connection.async_read(parser, yield[clear_ec(ec)]);
      EXPECT_EQ(connection_reset, ec);

      connection.async_read_header(parser, yield[clear_ec(ec)]);
      EXPECT_EQ(connection_reset, ec);

      connection.async_read_some(parser, yield[clear_ec(ec)]);
      EXPECT_EQ(connection_reset, ec);
    });
  EXPECT_NO_THROW(ioc.run());
}

TEST(http_connection, write_errors)
{
  boost::asio::io_context ioc;
  auto connection = basic_http_connection(
      error_stream(ioc.get_executor(), connection_reset));

  http::request<body> request;
  http::response<body> response;
  http::serializer<false, body> serializer(response);

  boost::system::error_code ec;

  EXPECT_THROW(connection.write(request), boost::system::system_error);
  connection.write(request, ec);
  EXPECT_EQ(connection_reset, ec);

  EXPECT_THROW(connection.write(response), boost::system::system_error);
  connection.write(response, clear_ec(ec));
  EXPECT_EQ(connection_reset, ec);

  EXPECT_THROW(connection.write(serializer), boost::system::system_error);
  connection.write(serializer, clear_ec(ec));
  EXPECT_EQ(connection_reset, ec);

  EXPECT_THROW(connection.write_header(serializer), boost::system::system_error);
  connection.write_header(serializer, clear_ec(ec));
  EXPECT_EQ(connection_reset, ec);

  EXPECT_THROW(connection.write_some(serializer), boost::system::system_error);
  connection.write_some(serializer, clear_ec(ec));
  EXPECT_EQ(connection_reset, ec);

  boost::asio::spawn(ioc, [&] (boost::asio::yield_context yield) {
      connection.async_write(request, yield[clear_ec(ec)]);
      EXPECT_EQ(connection_reset, ec);

      connection.async_write(response, yield[clear_ec(ec)]);
      EXPECT_EQ(connection_reset, ec);

      connection.async_write(serializer, yield[clear_ec(ec)]);
      EXPECT_EQ(connection_reset, ec);

      connection.async_write_header(serializer, yield[clear_ec(ec)]);
      EXPECT_EQ(connection_reset, ec);

      connection.async_write_some(serializer, yield[clear_ec(ec)]);
      EXPECT_EQ(connection_reset, ec);
    });
  EXPECT_NO_THROW(ioc.run());
}

} // namespace requests
