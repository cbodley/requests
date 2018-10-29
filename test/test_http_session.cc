#include "requests/session.hpp"
#include <boost/beast.hpp>
#include <gtest/gtest.h>

using boost::asio::ip::tcp;

tcp::acceptor start_listener(boost::asio::io_context& context)
{
  tcp::acceptor acceptor(context);
  tcp::endpoint endpoint(tcp::v4(), 0);
  acceptor.open(endpoint.protocol());
  acceptor.bind(endpoint);
  acceptor.listen();
  return acceptor;
}

TEST(http_session, request)
{
  boost::asio::io_context ioc;
  auto acceptor = start_listener(ioc);
  const auto port = acceptor.local_endpoint().port();
  const std::string raw_url = "http://localhost:" + std::to_string(port);
  auto uri = requests::uri_view::parse(raw_url);
  ASSERT_FALSE(uri.host().empty());

  requests::http_session session;
  boost::beast::http::request<boost::beast::http::string_body> request;
  boost::beast::http::response<boost::beast::http::string_body> response;

  boost::system::error_code ec;
  requests::request(ioc, session, uri, request, response, ec);
  EXPECT_EQ(boost::system::errc::success, ec);
  EXPECT_EQ(boost::beast::http::status::ok, response.result());
}
