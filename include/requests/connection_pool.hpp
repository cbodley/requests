#pragma once

#include <atomic>
#include <memory>
#include <type_traits>

#include <boost/beast/core/multi_buffer.hpp>
#include <boost/intrusive/list.hpp>

#include <requests/connection.hpp>

namespace requests {

template <typename StreamFactory>
class basic_connection_pool;

namespace detail {

// describes the StreamFactory concept
struct model_stream_factory final {
  using stream_type = int;
  void connect(stream_type& stream,
               boost::asio::string_view host,
               boost::asio::string_view port);
  void connect(stream_type& stream,
               boost::asio::string_view host,
               boost::asio::string_view port,
               boost::system::error_code& ec);
  void shutdown(stream_type& stream);
  void shutdown(stream_type& stream,
                boost::system::error_code& ec);
  template <typename CompletionToken> // void(error_code)
  auto async_connect(stream_type& stream,
                     boost::asio::string_view host,
                     boost::asio::string_view port,
                     CompletionToken&& token);
  template <typename CompletionToken> // void(error_code)
  auto async_shutdown(stream_type& stream,
                      CompletionToken&& token);
  void cancel();
};

template <typename Stream, typename DynamicBuffer>
struct pool_connection_impl : boost::intrusive::list_base_hook<> {
  Stream stream;
  DynamicBuffer buffer;
  std::atomic<bool> closed = false;

  pool_connection_impl(Stream&& stream, DynamicBuffer&& buffer = DynamicBuffer())
    : stream(std::move(stream)), buffer(std::move(buffer))
  {}
};

template <typename StreamFactory, typename DynamicBuffer>
class basic_pool_connection {
  friend class basic_connection_pool<StreamFactory>;
  using pool_type = basic_connection_pool<StreamFactory>;
  pool_type* pool;

  using stream_type = typename pool_type::stream_type;
  using impl_type = pool_connection_impl<stream_type, DynamicBuffer>;
  using impl_ptr = std::unique_ptr<impl_type>;
  impl_ptr impl;

  basic_pool_connection(pool_type* pool, impl_ptr&& impl) noexcept
    : pool(pool), impl(std::move(impl)) {}
 public:
  basic_pool_connection(basic_pool_connection&&) = default;
  basic_pool_connection& operator=(basic_pool_connection&&) = default;

  ~basic_pool_connection() {
    // dont access pool unless impl is still open
    if (impl && !impl->closed.exchange(true)) {
      pool->shutdown(*impl);
    }
  }
};

} // namespace detail

template <typename StreamFactory>
class basic_connection_pool {
  boost::asio::io_context& ioc;
  std::string host;
  std::string port;
  size_t max_size;
  StreamFactory factory;
  // TODO: timeouts

  using stream_type = typename std::decay_t<StreamFactory>::stream_type;
  using buffer_type = boost::beast::multi_buffer;
  using connection_type = detail::basic_pool_connection<StreamFactory, buffer_type>;
  friend class detail::basic_pool_connection<StreamFactory, buffer_type>;
  using connection_impl = typename connection_type::impl_type;

  boost::intrusive::list<connection_impl> idle;
  boost::intrusive::list<connection_impl> outstanding;

  std::unique_ptr<connection_impl> reuse_idle() {
    while (!idle.empty()) {
      auto c = std::unique_ptr<connection_impl>(&idle.front());
      idle.pop_front();
      boost::system::error_code ec;
      c->stream.available(ec); //  test for errors
      if (!ec) {
        return c;
      }
    }
    return {};
  }
  void shutdown(connection_impl& c) {
    outstanding.erase(outstanding.iterator_to(c));
    boost::system::error_code ec;
    factory.shutdown(c.stream, ec);
  }
 public:
  template <typename ...FactoryArgs>
  basic_connection_pool(boost::asio::io_context& ioc,
                        std::string host, std::string port,
                        size_t max_size, FactoryArgs&& ...args)
    : ioc(ioc), host(std::move(host)), port(std::move(port)),
      max_size(max_size), factory(std::forward<FactoryArgs>(args)...)
  {}
  basic_connection_pool(basic_connection_pool&&) = default;
  basic_connection_pool& operator=(basic_connection_pool&&) = default;
  ~basic_connection_pool() {
    assert(outstanding.empty());
  }

  connection_type get() {
    auto conn = reuse_idle();
    if (!conn) {
      stream_type stream(ioc);
      factory.connect(stream, host, port);
      conn = std::make_unique<connection_impl>(std::move(stream));
    }
    outstanding.push_back(*conn);
    return {this, std::move(conn)};
  }
  connection_type get(boost::system::error_code& ec) {
    auto conn = reuse_idle();
    if (!conn) {
      stream_type stream(ioc);
      factory.connect(stream, host, port, ec);
      conn = std::make_unique<connection_impl>(std::move(stream));
    }
    outstanding.push_back(*conn);
    return {this, std::move(conn)};
  }

  void put(connection_type connection,
           boost::system::error_code ec) {
    outstanding.erase(outstanding.iterator_to(*connection.impl));
    if (!ec) {
      idle.push_back(*connection.impl.release());
    } else if (!connection.impl->closed) {
      factory.shutdown(connection.impl->stream, ec);
    }
  }

  void close() {
    while (!outstanding.empty()) {
      auto& c = outstanding.front();
      if (!c.closed.exchange(true)) {
        outstanding.pop_front();
        factory.shutdown(c.stream);
      }
    }
    while (!idle.empty()) {
      auto& c = idle.front();
      factory.shutdown(c.stream);
      idle.pop_front_and_dispose(std::default_delete<connection_impl>{});
    }
  }
  void close(boost::system::error_code& ec) {
    while (!outstanding.empty()) {
      auto& c = outstanding.front();
      if (!c.closed.exchange(true)) {
        outstanding.pop_front();
        factory.shutdown(c.stream, ec);
      }
    }
    while (!idle.empty()) {
      auto& c = idle.front();
      factory.shutdown(c.stream, ec);
      idle.pop_front_and_dispose(std::default_delete<connection_impl>{});
    }
  }
};

} // namespace requests
