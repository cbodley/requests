#include <requests/connection_pool.hpp>
#include <thread>
#include <requests/detail/tcp_connection_factory.hpp>
#include <boost/asio/spawn.hpp>
#ifdef REQUESTS_ENABLE_SSL
#include <requests/detail/ssl_connection_factory.hpp>
#include <boost/asio/ssl.hpp>
#endif
#include <gtest/gtest.h>

namespace requests {

using boost::asio::ip::tcp;
using tcp_connection_pool = basic_connection_pool<detail::tcp_connection_factory>;
namespace http = boost::beast::http;
static const boost::system::error_code ok{};

tcp::acceptor start_listener(boost::asio::io_context& context)
{
  tcp::acceptor acceptor(context);
  tcp::endpoint endpoint(tcp::v4(), 0);
  acceptor.open(endpoint.protocol());
  acceptor.bind(endpoint);
  acceptor.listen();
  return acceptor;
}

TEST(tcp_connection_factory, connect)
{
  boost::asio::io_context ioc;
  auto acceptor = start_listener(ioc);
  auto port = std::to_string(acceptor.local_endpoint().port());

  detail::tcp_connection_factory factory;
  {
    auto stream = tcp::socket(ioc);
    boost::system::error_code ec;
    factory.connect(stream, "localhost", port, ec);
    ASSERT_FALSE(ec);
    factory.shutdown(stream, ec);
    EXPECT_FALSE(ec);
  }
  {
    auto stream = tcp::socket(ioc);
    ASSERT_NO_THROW(factory.connect(stream, "localhost", port));
    EXPECT_NO_THROW(factory.shutdown(stream));
  }
}

#ifdef REQUESTS_ENABLE_SSL
TEST(ssl_connection_factory, connect)
{
  boost::asio::io_context ioc;
  auto acceptor = start_listener(ioc);
  auto port = std::to_string(acceptor.local_endpoint().port());

  namespace ssl = boost::asio::ssl;
  using ssl_stream = ssl::stream<tcp::socket>;

  boost::asio::spawn(ioc, [&] (boost::asio::yield_context yield) {
      auto ctx = ssl::context{ssl::context::sslv23};
      auto stream = ssl_stream(ioc, ctx);
      boost::system::error_code ec;
      stream.set_verify_mode(ssl::verify_none, ec);
      ASSERT_EQ(ok, ec);

      acceptor.async_accept(stream.next_layer(), yield[ec]);
      ASSERT_EQ(ok, ec);
      stream.async_handshake(ssl_stream::server, yield[ec]);
      ASSERT_EQ(ok, ec);
    });
  std::thread thread([&ioc] { ioc.run(); });

  detail::ssl_connection_factory<ssl_stream> factory;
  auto ctx = ssl::context{ssl::context::sslv23};
  auto stream = ssl_stream(ioc, ctx);
  boost::system::error_code ec;
  stream.set_verify_mode(ssl::verify_none, ec);
  ASSERT_EQ(ok, ec);

  factory.connect(stream, "localhost", port, ec);
  ASSERT_EQ(ok, ec);
  factory.shutdown(stream, ec);
  ASSERT_EQ(ok, ec);

  thread.join();
}
#endif

TEST(http_connection_pool, construct)
{
  boost::asio::io_context ioc;

  struct move_only_factory {
    move_only_factory() = default;
    move_only_factory(move_only_factory&&) = default;
    move_only_factory& operator=(move_only_factory&&) = default;
    move_only_factory(const move_only_factory&) = delete;
    move_only_factory& operator=(const move_only_factory&) = delete;
    using stream_type = int;
  };
  {
    move_only_factory factory;
    auto pool = basic_connection_pool<move_only_factory&>(ioc, "", "", 0, factory);
    auto p2 = std::move(pool);
  }
  {
    move_only_factory factory;
    auto pool = basic_connection_pool<move_only_factory>(ioc, "", "", 0, std::move(factory));
    auto p2 = std::move(pool);
  }
  {
    auto pool = basic_connection_pool<move_only_factory>(ioc, "", "", 0);
    auto p2 = std::move(pool);
  }
}

TEST(http_connection_pool, get)
{
  boost::asio::io_context ioc;
  auto acceptor = start_listener(ioc);
  auto port = std::to_string(acceptor.local_endpoint().port());

  auto pool = tcp_connection_pool(ioc, "localhost", port, 10);
  auto c1 = pool.get();
  auto c2 = pool.get();
}

TEST(http_connection_pool, put)
{
  boost::asio::io_context ioc;
  auto acceptor = start_listener(ioc);
  auto port = std::to_string(acceptor.local_endpoint().port());

  auto pool = tcp_connection_pool(ioc, "localhost", port, 10);
  auto c1 = pool.get();
  pool.put(std::move(c1), ok);
  auto c2 = pool.get();
  pool.put(std::move(c2), ok);
  {
    auto c3 = pool.get();
  }
  pool.close();
}

} // namespace requests
