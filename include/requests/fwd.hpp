#pragma once

#include <boost/beast/http/message.hpp>

namespace requests {

class uri_view;
class uri_string;

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
