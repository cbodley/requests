#pragma once

#include <boost/asio/associated_allocator.hpp>
#include <boost/asio/associated_executor.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/system/system_error.hpp>

#include <requests/detail/completion.hpp>

namespace requests {

// stream that fails all operations with the given error code
template <typename Executor>
struct error_stream {
  Executor ex;
  boost::system::error_code ec;
  error_stream(const Executor& ex, boost::system::error_code ec)
    : ex(ex), ec(ec) {}

  using executor_type = Executor;
  executor_type get_executor() const noexcept { return ex; }

  using next_layer_type = error_stream;
  next_layer_type& next_layer() { return *this; }

  using lowest_layer_type = error_stream;
  lowest_layer_type& lowest_layer() { return *this; }

  template <typename MutableBufferSequence>
  size_t read_some(const MutableBufferSequence& buffers) {
    throw boost::system::system_error(ec);
  }
  template <typename MutableBufferSequence>
  size_t read_some(const MutableBufferSequence& buffers,
                   boost::system::error_code& ec) {
    ec = this->ec;
    return 0;
  }

  template <typename ConstBufferSequence>
  size_t write_some(const ConstBufferSequence& buffers) {
    throw boost::system::system_error(ec);
  }
  template <typename ConstBufferSequence>
  size_t write_some(const ConstBufferSequence& buffers,
                   boost::system::error_code& ec) {
    ec = this->ec;
    return 0;
  }

  template <typename MutableBufferSequence, typename ReadHandler>
  auto async_read_some(const MutableBufferSequence& buffers,
                       ReadHandler&& token) {
    using Signature = void(boost::system::error_code, size_t);
    boost::asio::async_completion<ReadHandler, Signature> init(token);
    auto& handler = init.completion_handler;
    auto ex2 = boost::asio::get_associated_executor(handler, get_executor());
    auto alloc = boost::asio::get_associated_allocator(handler);
    ex2.post(requests::detail::bind_handler(std::move(handler), ec, 0), alloc);
    return init.result.get();
  }

  template <typename ConstBufferSequence, typename WriteHandler>
  auto async_write_some(const ConstBufferSequence& buffers,
                        WriteHandler&& token) {
    using Signature = void(boost::system::error_code, size_t);
    boost::asio::async_completion<WriteHandler, Signature> init(token);
    auto& handler = init.completion_handler;
    auto ex2 = boost::asio::get_associated_executor(handler, get_executor());
    auto alloc = boost::asio::get_associated_allocator(handler);
    ex2.post(requests::detail::bind_handler(std::move(handler), ec, 0), alloc);
    return init.result.get();
  }
};

} // namespace requests
