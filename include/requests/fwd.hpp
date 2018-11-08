#pragma once

#include <type_traits>

#include <boost/beast/http/message.hpp>

namespace requests {

template <typename CharT, typename Traits, typename Allocator>
class basic_uri_string;
using uri_string = basic_uri_string<char, std::char_traits<char>, std::allocator<char>>;

template <typename CharT, typename Traits>
class basic_uri_view;
using uri_view = basic_uri_view<char, std::char_traits<char>>;

class http_connection;
class https_connection;

class http_connection_pool;
class https_connection_pool;

// io_context service to manage connection pools
class connection_pool_service;

// http session object that can persist cookies and request headers over
// several requests
template <typename Cookies, typename Fields>
class http_session;

} // namespace requests
