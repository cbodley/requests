#pragma once

#include <chrono>
#include <boost/asio/basic_waitable_timer.hpp>
#include <requests/detail/timed_handler.hpp>

namespace requests::detail {

template <typename Stream, typename Clock, typename WaitTraits>
class timed_stream {
  using stream_type = Stream;
  using timer_type = boost::asio::basic_waitable_timer<Clock, WaitTraits>;
  using duration_type = typename timer_type::duration;
  stream_type stream;
  timer_type timer;
  duration_type timeout;
 public:
  timed_stream(stream_type&& stream, timer_type&& timer, duration_type timeout)
    : stream(std::move(stream)), timer(std::move(timer)), timeout(timeout) {}

  using executor_type = typename stream_type::executor_type;
  auto get_executor() { return stream.get_executor(); }

  using next_layer_type = stream_type;
  next_layer_type& next_layer() { return stream; }

  using lowest_layer_type = typename stream_type::lowest_layer_type;
  lowest_layer_type& lowest_layer() { return stream.lowest_layer(); }

  // AsyncReadStream
  template <typename MutableBufferSequence, typename ReadHandler>
  auto async_read_some(const MutableBufferSequence& buffers,
                       ReadHandler&& token) {
    using Signature = void(boost::system::error_code, size_t);
    boost::asio::async_completion<ReadHandler, Signature> init(token);
    auto& handler = init.completion_handler;
    if (timeout.count()) {
      auto h = timed_handler(lowest_layer(), timer, timeout, std::move(handler));
      stream.async_read_some(buffers, std::move(h));
    } else {
      stream.async_read_some(buffers, std::move(handler));
    }
    return init.result.get();
  }

  // AsyncWriteStream
  template <typename ConstBufferSequence, typename WriteHandler>
  auto async_write_some(const ConstBufferSequence& buffers,
                        WriteHandler&& token) {
    using Signature = void(boost::system::error_code, size_t);
    boost::asio::async_completion<WriteHandler, Signature> init(token);
    auto& handler = init.completion_handler;
    if (timeout.count()) {
      auto h = timed_handler(lowest_layer(), timer, timeout, std::move(handler));
      stream.async_write_some(buffers, std::move(h));
    } else {
      stream.async_write_some(buffers, std::move(handler));
    }
    return init.result.get();
  }

  // TODO: SyncReadStream/SyncWriteStream
};

} // namespace requests::detail
