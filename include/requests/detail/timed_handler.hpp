#pragma once

#include <boost/asio/associated_allocator.hpp>
#include <boost/asio/associated_executor.hpp>

namespace requests::detail {

// a handler wrapper that cancels a timer on completion
template <typename Handler, typename Timer>
struct timed_handler {
  Handler handler;
  Timer& timer;
  timed_handler(Handler&& h, Timer& t) : handler(std::move(h)), timer(t) {}
  template <typename Canceler>
  timed_handler(Canceler& canceler, Timer& timer,
                typename Timer::time_point expires_at, Handler&& handler)
    : handler(std::move(handler)), timer(timer) {
    timer.expires_at(expires_at);
    timer.async_wait([&canceler] (boost::system::error_code ec) {
        if (ec != boost::asio::error::operation_aborted) canceler.cancel();
      });
  }
  template <typename Canceler>
  timed_handler(Canceler& canceler, Timer& timer,
                typename Timer::duration expires_after, Handler&& handler)
    : handler(std::move(handler)), timer(timer) {
    timer.expires_after(expires_after);
    timer.async_wait([&canceler] (boost::system::error_code ec) {
        if (ec != boost::asio::error::operation_aborted) canceler.cancel();
      });
  }
  template <typename ...Args>
  void operator()(Args&& ...args) {
    timer.cancel();
    handler(std::forward<Args>(args)...);
  }
  using allocator_type = boost::asio::associated_allocator_t<Handler>;
  allocator_type get_allocator() const noexcept {
    return boost::asio::get_associated_allocator(handler);
  }
};

} // namespace requests::detail

namespace boost::asio {

// associated_executor trait for requests::detail::basic_timed_handler
template <typename Handler, typename Timer, typename Executor>
struct associated_executor<requests::detail::timed_handler<Handler, Timer>, Executor> {
  using type = associated_executor_t<Handler, Executor>;

  static type get(const requests::detail::timed_handler<Handler, Timer>& h,
                  const Executor& ex = Executor()) noexcept {
    return get_associated_executor(h.handler, ex);
  }
};

} // namespace boost::asio
