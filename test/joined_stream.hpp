#pragma once

#include <boost/beast/core/type_traits.hpp>

namespace requests {

// stream that combines two separate streams
template <typename ReadStream, typename WriteStream>
class joined_stream {
  ReadStream& in;
  WriteStream& out;
 public:
  joined_stream(ReadStream& in, WriteStream& out) : in(in), out(out) {}

  using executor_type = typename ReadStream::executor_type;
  executor_type get_executor() const noexcept { return in.get_executor(); }

  using next_layer_type = typename ReadStream::next_layer_type;
  next_layer_type& next_layer() { return in; }

  using lowest_layer_type = typename ReadStream::lowest_layer_type;
  lowest_layer_type& lowest_layer() { return in.lowest_layer(); }

  template <typename MutableBufferSequence>
  size_t read_some(const MutableBufferSequence& buffers) {
    static_assert(boost::beast::is_sync_read_stream<ReadStream>::value,
                  "SyncReadStream requirements are not met.");
    return in.read_some(buffers);
  }
  template <typename MutableBufferSequence>
  size_t read_some(const MutableBufferSequence& buffers,
                   boost::system::error_code& ec) {
    static_assert(boost::beast::is_sync_read_stream<ReadStream>::value,
                  "SyncReadStream requirements are not met.");
    return in.read_some(buffers, ec);
  }
  template <typename ConstBufferSequence>
  size_t write_some(const ConstBufferSequence& buffers) {
    static_assert(boost::beast::is_sync_write_stream<WriteStream>::value,
                  "SyncWriteStream requirements are not met.");
    return out.write_some(buffers);
  }
  template <typename ConstBufferSequence>
  size_t write_some(const ConstBufferSequence& buffers,
                   boost::system::error_code& ec) {
    static_assert(boost::beast::is_sync_write_stream<WriteStream>::value,
                  "SyncWriteStream requirements are not met.");
    return out.write_some(buffers, ec);
  }
  template <typename MutableBufferSequence, typename ReadHandler>
  auto async_read_some(const MutableBufferSequence& buffers,
                       ReadHandler&& token) {
    static_assert(boost::beast::is_async_read_stream<ReadStream>::value,
                  "AsyncReadStream requirements are not met.");
    return in.async_read_some(buffers, std::forward<ReadHandler>(token));
  }
  template <typename ConstBufferSequence, typename WriteHandler>
  auto async_write_some(const ConstBufferSequence& buffers,
                        WriteHandler&& token) {
    static_assert(boost::beast::is_async_write_stream<WriteStream>::value,
                  "AsyncWriteStream requirements are not met.");
    return out.async_write_some(buffers, std::forward<WriteHandler>(token));
  }
};

template <typename ReadStream, typename WriteStream>
auto join_streams(ReadStream&& in, WriteStream&& out)
{
  return joined_stream(std::forward<ReadStream>(in),
                       std::forward<WriteStream>(out));
}

} // namespace requests
