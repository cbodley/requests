#include <requests/detail/timed_stream.hpp>
#include <requests/detail/completion.hpp>
#include <boost/asio/buffer.hpp>
#include <gtest/gtest.h>

namespace requests {

using namespace std::chrono_literals;

using Clock = std::chrono::steady_clock;
using Timer = boost::asio::basic_waitable_timer<Clock>;

// an AsyncReadStream/AsyncWriteStream that does nothing until cancelation
class NullAsyncStream {
  boost::asio::io_context& context;
  using Signature = void(boost::system::error_code, size_t);
  using Completion = detail::completion<Signature>;
  std::unique_ptr<Completion> completion;
 public:
  NullAsyncStream(boost::asio::io_context& context)
    : context(context) {}

  using executor_type = boost::asio::io_context::executor_type;
  auto get_executor() { return context.get_executor(); }

  using lowest_layer_type = NullAsyncStream;
  lowest_layer_type& lowest_layer() { return *this; }

  template <typename MutableBufferSequence, typename CompletionToken>
  auto async_read_some(const MutableBufferSequence& buffers,
                       CompletionToken&& token) {
    boost::asio::async_completion<CompletionToken, Signature> init(token);
    completion = Completion::create(context.get_executor(),
                                    std::move(init.completion_handler));
    return init.result.get();
  }
  template <typename ConstBufferSequence, typename CompletionToken>
  auto async_write_some(const ConstBufferSequence& buffers,
                        CompletionToken&& token) {
    boost::asio::async_completion<CompletionToken, Signature> init(token);
    completion = Completion::create(context.get_executor(),
                                    std::move(init.completion_handler));
    return init.result.get();
  }
  void cancel() {
    boost::system::error_code ec = boost::asio::error::operation_aborted;
    if (completion) {
      Completion::dispatch(std::move(completion), ec, 0);
    }
  }
};

auto capture(std::optional<boost::system::error_code>& ec) {
  return [&] (boost::system::error_code e, size_t b) { ec = e; };
}

TEST(timed_stream, async_read_timeout)
{
  boost::asio::io_context context;
  detail::timed_stream stream(NullAsyncStream{context}, Timer{context}, 1ms);

  std::array<char, 4> buffer;
  std::optional<boost::system::error_code> ec;
  stream.async_read_some(boost::asio::buffer(buffer), capture(ec));

  EXPECT_NE(0u, context.run_for(10ms));
  EXPECT_TRUE(context.stopped());

  ASSERT_TRUE(ec);
  EXPECT_EQ(boost::asio::error::operation_aborted, *ec);
}

TEST(timed_stream, async_write_timeout)
{
  boost::asio::io_context context;
  detail::timed_stream stream(NullAsyncStream{context}, Timer{context}, 1ms);

  std::vector<char> buffer(16, 'a');
  std::optional<boost::system::error_code> ec;
  stream.async_write_some(boost::asio::buffer(buffer), capture(ec));

  EXPECT_NE(0u, context.run_for(10ms));
  EXPECT_TRUE(context.stopped());

  ASSERT_TRUE(ec);
  EXPECT_EQ(boost::asio::error::operation_aborted, *ec);
}


} // namespace requests
