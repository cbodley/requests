#include <requests/connection.hpp>
#include <thread>
#include <boost/beast.hpp>
#include <boost/asio/spawn.hpp>
#include <gtest/gtest.h>

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

tcp::acceptor start_listener(boost::asio::io_context& context)
{
  tcp::acceptor acceptor(context);
  tcp::endpoint endpoint(tcp::v4(), 0);
  acceptor.open(endpoint.protocol());
  acceptor.bind(endpoint);
  acceptor.listen();
  return acceptor;
}

TEST(http_connection, connection)
{
  boost::asio::io_context ioc;
  auto acceptor = start_listener(ioc);

  using body = boost::beast::http::string_body;
  boost::asio::spawn(ioc, [&] (boost::asio::yield_context yield) {
      tcp::socket socket(ioc);
      acceptor.async_accept(socket, yield);
      requests::http_connection connection(std::move(socket));
      http::request<body> request;
      connection.async_read(request, yield);
      EXPECT_EQ(http::verb::get, request.method());
      EXPECT_EQ("hello", request.body());
      http::response<body> response(http::status::ok, request.version(), "world");
      response.prepare_payload();
      connection.async_write(response, yield);
      connection.lowest_layer().shutdown(tcp::socket::shutdown_both);
    });
  std::thread thread([&ioc] { EXPECT_NO_THROW(ioc.run()); });

  tcp::socket socket(ioc);
  boost::system::error_code ec;
  socket.connect(acceptor.local_endpoint(), ec);
  ASSERT_FALSE(ec);

  requests::http_connection connection(std::move(socket));

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
