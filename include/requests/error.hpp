#pragma once

#ifdef REQUESTS_USE_BOOST_SYSTEM_ERROR
#include <boost/system/error_code.hpp>
#else
#include <system_error>
#endif
#include <type_traits>

namespace requests::http {

#ifdef REQUESTS_USE_BOOST_SYSTEM_ERROR
#include <boost/system/error_code.hpp>
using error_code = boost::system::error_code;
using error_condition = boost::system::error_condition;
using error_category = boost::system::error_category;
#else
using error_code = std::error_code;
using error_condition = std::error_condition;
using error_category = std::error_category;
#endif

/// http error codes
enum class error {
  continue_ = 100,
  switching_protocols = 101,

  ok = 200,
  created = 201,
  accepted = 202,
  non_authoritative_information = 203,
  no_content = 204,
  reset_content = 205,
  partial_content = 206,

  multiple_choices = 300,
  moved_permanently = 301,
  found = 302,
  see_other = 303,
  not_modified = 304,
  use_proxy = 305,
  temporary_redirect = 307,

  bad_request = 400,
  unauthorized = 401,
  payment_required = 402,
  forbidden = 403,
  not_found = 404,
  method_not_allowed = 405,
  not_acceptable = 406,
  proxy_authentication_requried = 407,
  request_timeout = 408,
  conflict = 409,
  gone = 410,
  length_required = 411,
  precondition_faield = 412,
  payload_too_large = 413,
  uri_too_long = 414,
  unsupported_media_type = 415,
  range_not_satisfiable = 416,
  expectation_failed = 417,
  upgrade_required = 426,

  internal_server_error = 500,
  not_implemented = 501,
  bad_gateway = 502,
  service_unavailable = 503,
  gateway_timeout = 504,
  http_verion_not_supported = 505,
};

/// http error category
const error_category& http_category()
{
  struct category : public error_category {
    const char* name() const noexcept override {
      return "requests::http";
    }
    std::string message(int ev) const override {
      switch (static_cast<error>(ev)) {
        case error::continue_: return "Continue";
        case error::switching_protocols: return "Switching Protocols";

        case error::ok: return "OK";
        case error::created: return "Created";
        case error::accepted: return "Accepted";
        case error::non_authoritative_information: return "Non-Authoritative Information";
        case error::no_content: return "No Content";
        case error::reset_content: return "Reset Content";
        case error::partial_content: return "Partial Content";

        case error::multiple_choices: return "Multiple Choices";
        case error::moved_permanently: return "Moved Permanently";
        case error::found: return "Found";
        case error::see_other: return "See Other";
        case error::not_modified: return "Not Modified";
        case error::use_proxy: return "Use Proxy";
        case error::temporary_redirect: return "Temporary Redirect";

        case error::bad_request: return "Bad Request";
        case error::unauthorized: return "Unauthorized";
        case error::payment_required: return "Payment Required";
        case error::forbidden: return "Forbidden";
        case error::not_found: return "Not Found";
        case error::method_not_allowed: return "Method Not Allowed";
        case error::not_acceptable: return "Not Acceptable";
        case error::proxy_authentication_requried: return "Proxy Authentication Required";
        case error::request_timeout: return "Request Timeout";
        case error::conflict: return "Conflict";
        case error::gone: return "Gone";
        case error::length_required: return "Length Required";
        case error::precondition_faield: return "Precondition Failed";
        case error::payload_too_large: return "Payload Too Large";
        case error::uri_too_long: return "URI Too Long";
        case error::unsupported_media_type: return "Unsupported Media Type";
        case error::range_not_satisfiable: return "Range Not Satisfiable";
        case error::expectation_failed: return "Expectation Failed";
        case error::upgrade_required: return "Upgrade Required";

        case error::internal_server_error: return "Internal Server Error";
        case error::not_implemented: return "Not Implemented";
        case error::bad_gateway: return "Bad Gateway";
        case error::service_unavailable: return "Service Unavailable";
        case error::gateway_timeout: return "Gateway Timeout";
        case error::http_verion_not_supported: return "HTTP Version Not Supported";
        default: return "Unknown";
      }
    }
  };
  static category instance;
  return instance;
}

inline error_code make_error_code(error e)
{
  return {static_cast<int>(e), http_category()};
}

inline error_condition make_error_condition(error e)
{
  return {static_cast<int>(e), http_category()};
}


/// generic http error category
enum class errc {
  informational = 1, // 1xx
  successful    = 2, // 2xx
  redirection   = 3, // 3xx
  client_error  = 4, // 4xx
  server_error  = 5, // 5xx
};

const error_category& generic_category()
{
  struct category : error_category {
    const char* name() const noexcept override {
      return "requests::generic";
    }
    std::string message(int ev) const override {
      switch (static_cast<errc>(ev)) {
        case errc::informational: return "Informational";
        case errc::successful: return "Successful";
        case errc::redirection: return "Redirection";
        case errc::client_error: return "Client Error";
        case errc::server_error: return "Server Error";
        default: return "Unknown";
      }
    }
    bool equivalent(const error_code& code,
                    int condition) const noexcept override {
      if (code.category() == http_category()) {
        switch (static_cast<errc>(condition)) {
          case errc::informational:
            return code.value() >= 100 && code.value() < 200;
          case errc::successful:
            return code.value() >= 200 && code.value() < 300;
          case errc::redirection:
            return code.value() >= 300 && code.value() < 400;
          case errc::client_error:
            return code.value() >= 400 && code.value() < 500;
          case errc::server_error:
            return code.value() >= 500 && code.value() < 600;
        }
      }
      return error_category::equivalent(code, condition);
    }
  };
  static category instance;
  return instance;
}

inline error_condition make_error_condition(errc e)
{
  return {static_cast<int>(e), generic_category()};
}

} // namespace requests::http

#ifdef REQUESTS_USE_BOOST_SYSTEM_ERROR
namespace boost::system {
#else
namespace std {
#endif
/// enables implicit conversion to error_code/condition
template <>
struct is_error_code_enum<requests::http::error> : std::true_type {};
template <>
struct is_error_condition_enum<requests::http::errc> : std::true_type {};

} // namespace
