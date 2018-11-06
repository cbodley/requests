#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>

#include <boost/asio/basic_streambuf.hpp>

#include <requests/detail/completion.hpp>

namespace requests {

// a stream that buffers writes and echos them back on reads
template <typename Executor, typename Allocator = std::allocator<char>>
class echo_stream {
  Executor ex;
  std::mutex mutex;
  boost::asio::basic_streambuf<Allocator> buffer;

  struct waiter {
    virtual ~waiter() {}
    virtual void complete(boost::system::error_code ec) = 0;
    virtual void destroy() = 0;
  };
  waiter* reader = nullptr;

 public:
  echo_stream(const Executor& ex,
              size_t maximum_size = std::numeric_limits<size_t>::max(),
              const Allocator& allocator = Allocator())
    : ex(ex), buffer(maximum_size, allocator)
  {}
  ~echo_stream() {
    if (reader) {
      reader->destroy();
    }
  }

  using executor_type = Executor;
  executor_type get_executor() const noexcept { return ex; }

  using next_layer_type = echo_stream;
  next_layer_type& next_layer() { return *this; }

  using lowest_layer_type = echo_stream;
  lowest_layer_type& lowest_layer() { return *this; }

  template <typename MutableBufferSequence>
  size_t read_some(const MutableBufferSequence& buffers) {
    boost::system::error_code ec;
    read_some(buffers, ec);
    if (ec)
      throw boost::system::system_error(ec);
  }
  template <typename MutableBufferSequence>
  size_t read_some(const MutableBufferSequence& buffers,
                   boost::system::error_code& ec) {
    struct sync_waiter : waiter {
      std::mutex& mutex;
      std::condition_variable cond;
      std::optional<boost::system::error_code> ec;

      sync_waiter(std::mutex& mutex) : mutex(mutex) {}

      void complete(boost::system::error_code ec) override {
        auto lock = std::scoped_lock(mutex);
        this->ec = ec;
        cond.notify_one();
      }
      void destroy() override {} // noop, destroyed by the stack
    };

    auto lock = std::unique_lock(mutex);

    if (!buffer.size()) {
      assert(reader == nullptr);
      sync_waiter w(mutex);
      reader = &w;
      w.cond.wait(lock, [&w] { return w.ec; });
      ec = *w.ec;
    }

    auto bytes = boost::asio::buffer_copy(buffers, buffer.data());
    buffer.consume(bytes);
    return bytes;
  }

  template <typename ConstBufferSequence>
  size_t write_some(const ConstBufferSequence& buffers) {
    boost::system::error_code ec;
    size_t bytes = write_some(buffers, ec);
    if (ec)
      throw boost::system::system_error(ec);
    return bytes;
  }
  template <typename ConstBufferSequence>
  size_t write_some(const ConstBufferSequence& buffers,
                   boost::system::error_code& ec) {
    auto target = buffer.prepare(boost::asio::buffer_size(buffers));
    size_t bytes = boost::asio::buffer_copy(target, buffers);
    buffer.commit(bytes);
    if (buffer.size() && reader) {
      auto p = std::exchange(reader, nullptr);
      p->complete({});
    }
    return bytes;
  }

  template <typename MutableBufferSequence, typename ReadHandler>
  auto async_read_some(const MutableBufferSequence& buffers,
                       ReadHandler&& token) {
    using Signature = void(boost::system::error_code, size_t);

    struct async_waiter : waiter {
      const MutableBufferSequence& target;
      boost::asio::basic_streambuf<Allocator>& source;

      using Completion = detail::completion<Signature, detail::as_base<async_waiter>>;

      async_waiter(const MutableBufferSequence& target,
                   boost::asio::basic_streambuf<Allocator>& source)
        : target(target), source(source) {}

      void complete(boost::system::error_code ec) override {
        auto c = static_cast<Completion*>(this);
        if (ec) {
          Completion::dispatch(std::unique_ptr<Completion>{c}, ec, 0);
        } else {
          size_t bytes = boost::asio::buffer_copy(target, source.data());
          source.consume(bytes);
          Completion::dispatch(std::unique_ptr<Completion>{c}, ec, bytes);
        }
      }
      void destroy() override {
        delete static_cast<Completion*>(this);
      }
    };

    boost::asio::async_completion<ReadHandler, Signature> init(token);
    auto& handler = init.completion_handler;
    {
      auto lock = std::scoped_lock(mutex);

      if (buffer.size()) {
        size_t bytes = boost::asio::buffer_copy(buffers, buffer.data());
        buffer.consume(bytes);

        auto ex2 = boost::asio::get_associated_executor(handler, get_executor());
        auto alloc = boost::asio::get_associated_allocator(handler);
        boost::system::error_code ec;
        ex2.post(detail::bind_handler(std::move(handler), ec, bytes), alloc);
      } else {
        auto c = async_waiter::Completion::create(get_executor(),
                                                  std::move(handler),
                                                  buffers, buffer);
        reader = c.release();
      }
    }
    return init.result.get();
  }

  template <typename ConstBufferSequence, typename WriteHandler>
  auto async_write_some(const ConstBufferSequence& buffers,
                        WriteHandler&& token) {
    using Signature = void(boost::system::error_code, size_t);
    boost::asio::async_completion<WriteHandler, Signature> init(token);
    auto& handler = init.completion_handler;

    boost::system::error_code ec;
    size_t bytes = write_some(buffers, ec);

    auto ex2 = boost::asio::get_associated_executor(handler, get_executor());
    auto alloc = boost::asio::get_associated_allocator(handler);
    ex2.post(detail::bind_handler(std::move(handler), ec, bytes), alloc);
    return init.result.get();
  }
};

} // namespace requests
