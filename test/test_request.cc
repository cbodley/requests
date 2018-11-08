#include <requests/request.hpp>
#include <requests/uri_string.hpp>
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

TEST(request, request)
{
  boost::asio::io_context ioc;
  auto acceptor = start_listener(ioc);

  using body = boost::beast::http::string_body;
  boost::asio::spawn(ioc, [&] (boost::asio::yield_context yield) {
      tcp::socket socket(ioc);
      acceptor.async_accept(socket, yield);
      http::request<body> request;
      boost::beast::flat_buffer buffer;
      http::async_read(socket, buffer, request, yield);
      EXPECT_EQ("hello", request.body());
      http::response<body> response(http::status::ok, request.version(), "world");
      response.prepare_payload();
      http::async_write(socket, response, yield);
      socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    });
  std::thread thread([&ioc] { EXPECT_NO_THROW(ioc.run()); });

  const auto uri = requests::uri_string::parse("http://localhost")
      .port(std::to_string(acceptor.local_endpoint().port()));

  http::request<body> request(http::verb::get, "/", 11, "hello");
  request.prepare_payload();
  ASSERT_EQ("hello", request.body());
  http::response<body> response;
  boost::system::error_code ec;
  requests::request(ioc, uri, request, response, ec);
  EXPECT_EQ("world", response.body());
  ASSERT_FALSE(ec);
  EXPECT_EQ(boost::beast::http::status::ok, response.result());

  thread.join();
}
