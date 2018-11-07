#pragma once

#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>

namespace requests {

template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields>
size_t basic_http_connection<Stream, Buffer>::read(
    boost::beast::http::message<isRequest, Body, Fields>& message)
{
  static_assert(boost::beast::is_sync_read_stream<stream_type>::value,
                "SyncReadStream requirements are not met.");
  return boost::beast::http::read(stream, buffer, message);
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields>
size_t basic_http_connection<Stream, Buffer>::read(
    boost::beast::http::message<isRequest, Body, Fields>& message,
    boost::system::error_code& ec)
{
  static_assert(boost::beast::is_sync_read_stream<stream_type>::value,
                "SyncReadStream requirements are not met.");
  return boost::beast::http::read(stream, buffer, message, ec);
}


template <typename Stream, typename Buffer>
template <bool isRequest, typename Derived>
size_t basic_http_connection<Stream, Buffer>::read(
    boost::beast::http::basic_parser<isRequest, Derived>& parser)
{
  static_assert(boost::beast::is_sync_read_stream<stream_type>::value,
                "SyncReadStream requirements are not met.");
  return boost::beast::http::read(stream, buffer, parser);
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Derived>
size_t basic_http_connection<Stream, Buffer>::read(
    boost::beast::http::basic_parser<isRequest, Derived>& parser,
    boost::system::error_code& ec)
{
  static_assert(boost::beast::is_sync_read_stream<stream_type>::value,
                "SyncReadStream requirements are not met.");
  return boost::beast::http::read(stream, buffer, parser, ec);
}


template <typename Stream, typename Buffer>
template <bool isRequest, typename Derived>
size_t basic_http_connection<Stream, Buffer>::read_header(
    boost::beast::http::basic_parser<isRequest, Derived>& parser)
{
  static_assert(boost::beast::is_sync_read_stream<stream_type>::value,
                "SyncReadStream requirements are not met.");
  return boost::beast::http::read_header(stream, buffer, parser);
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Derived>
size_t basic_http_connection<Stream, Buffer>::read_header(
    boost::beast::http::basic_parser<isRequest, Derived>& parser,
    boost::system::error_code& ec)
{
  static_assert(boost::beast::is_sync_read_stream<stream_type>::value,
                "SyncReadStream requirements are not met.");
  return boost::beast::http::read_header(stream, buffer, parser, ec);
}


template <typename Stream, typename Buffer>
template <bool isRequest, typename Derived>
size_t basic_http_connection<Stream, Buffer>::read_some(
    boost::beast::http::basic_parser<isRequest, Derived>& parser)
{
  static_assert(boost::beast::is_sync_read_stream<stream_type>::value,
                "SyncReadStream requirements are not met.");
  return boost::beast::http::read_some(stream, buffer, parser);
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Derived>
size_t basic_http_connection<Stream, Buffer>::read_some(
    boost::beast::http::basic_parser<isRequest, Derived>& parser,
    boost::system::error_code& ec)
{
  static_assert(boost::beast::is_sync_read_stream<stream_type>::value,
                "SyncReadStream requirements are not met.");
  return boost::beast::http::read_some(stream, buffer, parser, ec);
}


template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields>
size_t basic_http_connection<Stream, Buffer>::write(
    boost::beast::http::message<isRequest, Body, Fields>& message)
{
  static_assert(boost::beast::is_sync_write_stream<stream_type>::value,
                "SyncWriteStream requirements are not met.");
  return boost::beast::http::write(stream, message);
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields>
size_t basic_http_connection<Stream, Buffer>::write(
    boost::beast::http::message<isRequest, Body, Fields>& message,
    boost::system::error_code& ec)
{
  static_assert(boost::beast::is_sync_write_stream<stream_type>::value,
                "SyncWriteStream requirements are not met.");
  return boost::beast::http::write(stream, message, ec);
}


template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields>
size_t basic_http_connection<Stream, Buffer>::write(
    boost::beast::http::serializer<isRequest, Body, Fields>& serializer)
{
  static_assert(boost::beast::is_sync_write_stream<stream_type>::value,
                "SyncWriteStream requirements are not met.");
  return boost::beast::http::write(stream, serializer);
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields>
size_t basic_http_connection<Stream, Buffer>::write(
    boost::beast::http::serializer<isRequest, Body, Fields>& serializer,
    boost::system::error_code& ec)
{
  static_assert(boost::beast::is_sync_write_stream<stream_type>::value,
                "SyncWriteStream requirements are not met.");
  return boost::beast::http::write(stream, serializer, ec);
}


template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields>
size_t basic_http_connection<Stream, Buffer>::write_header(
    boost::beast::http::serializer<isRequest, Body, Fields>& serializer)
{
  static_assert(boost::beast::is_sync_write_stream<stream_type>::value,
                "SyncWriteStream requirements are not met.");
  return boost::beast::http::write_header(stream, serializer);
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields>
size_t basic_http_connection<Stream, Buffer>::write_header(
    boost::beast::http::serializer<isRequest, Body, Fields>& serializer,
    boost::system::error_code& ec)
{
  static_assert(boost::beast::is_sync_write_stream<stream_type>::value,
                "SyncWriteStream requirements are not met.");
  return boost::beast::http::write_header(stream, serializer, ec);
}


template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields>
size_t basic_http_connection<Stream, Buffer>::write_some(
    boost::beast::http::serializer<isRequest, Body, Fields>& serializer)
{
  static_assert(boost::beast::is_sync_write_stream<stream_type>::value,
                "SyncWriteStream requirements are not met.");
  return boost::beast::http::write_some(stream, serializer);
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields>
size_t basic_http_connection<Stream, Buffer>::write_some(
    boost::beast::http::serializer<isRequest, Body, Fields>& serializer,
    boost::system::error_code& ec)
{
  static_assert(boost::beast::is_sync_write_stream<stream_type>::value,
                "SyncWriteStream requirements are not met.");
  return boost::beast::http::write_some(stream, serializer, ec);
}


template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields, typename ReadHandler>
auto basic_http_connection<Stream, Buffer>::async_read(
    boost::beast::http::message<isRequest, Body, Fields>& message,
    ReadHandler&& token)
{
  static_assert(boost::beast::is_async_read_stream<stream_type>::value,
                "AsyncReadStream requirements are not met.");
  return boost::beast::http::async_read(
      stream, buffer, message, std::forward<ReadHandler>(token));
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Derived, typename ReadHandler>
auto basic_http_connection<Stream, Buffer>::async_read(
    boost::beast::http::basic_parser<isRequest, Derived>& parser,
    ReadHandler&& token)
{
  static_assert(boost::beast::is_async_read_stream<stream_type>::value,
                "AsyncReadStream requirements are not met.");
  return boost::beast::http::async_read(
      stream, buffer, parser, std::forward<ReadHandler>(token));
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Derived, typename ReadHandler>
auto basic_http_connection<Stream, Buffer>::async_read_header(
    boost::beast::http::basic_parser<isRequest, Derived>& parser,
    ReadHandler&& token)
{
  static_assert(boost::beast::is_async_read_stream<stream_type>::value,
                "AsyncReadStream requirements are not met.");
  return boost::beast::http::async_read_header(
      stream, buffer, parser, std::forward<ReadHandler>(token));
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Derived, typename ReadHandler>
auto basic_http_connection<Stream, Buffer>::async_read_some(
    boost::beast::http::basic_parser<isRequest, Derived>& parser,
    ReadHandler&& token)
{
  static_assert(boost::beast::is_async_read_stream<stream_type>::value,
                "AsyncReadStream requirements are not met.");
  return boost::beast::http::async_read_some(
      stream, buffer, parser, std::forward<ReadHandler>(token));
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields, typename WriteHandler>
auto basic_http_connection<Stream, Buffer>::async_write(
    boost::beast::http::message<isRequest, Body, Fields>& message,
    WriteHandler&& token)
{
  static_assert(boost::beast::is_async_write_stream<stream_type>::value,
                "AsyncWriteStream requirements are not met.");
  return boost::beast::http::async_write(
      stream, message, std::forward<WriteHandler>(token));
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields, typename WriteHandler>
auto basic_http_connection<Stream, Buffer>::async_write(
    boost::beast::http::serializer<isRequest, Body, Fields>& serializer,
    WriteHandler&& token)
{
  static_assert(boost::beast::is_async_write_stream<stream_type>::value,
                "AsyncWriteStream requirements are not met.");
  return boost::beast::http::async_write(
      stream, serializer, std::forward<WriteHandler>(token));
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields, typename WriteHandler>
auto basic_http_connection<Stream, Buffer>::async_write_header(
    boost::beast::http::serializer<isRequest, Body, Fields>& serializer,
    WriteHandler&& token)
{
  static_assert(boost::beast::is_async_write_stream<stream_type>::value,
                "AsyncWriteStream requirements are not met.");
  return boost::beast::http::async_write_header(
      stream, serializer, std::forward<WriteHandler>(token));
}

template <typename Stream, typename Buffer>
template <bool isRequest, typename Body, typename Fields, typename WriteHandler>
auto basic_http_connection<Stream, Buffer>::async_write_some(
    boost::beast::http::serializer<isRequest, Body, Fields>& serializer,
    WriteHandler&& token)
{
  static_assert(boost::beast::is_async_write_stream<stream_type>::value,
                "AsyncWriteStream requirements are not met.");
  return boost::beast::http::async_write_some(
      stream, serializer, std::forward<WriteHandler>(token));
}

} // namespace requests
