#pragma once

#include <string_view>

namespace requests {

using string_view = std::string_view;

// uri_view for immutable view
// uri_string for mutation, storage and c_str()

struct uri_parts {
  string_view scheme;
  string_view userinfo;
  string_view host;
  string_view port;
  string_view path;
  string_view query;
  string_view fragment;

  uri_parts() = default;
  uri_parts(string_view scheme, string_view userinfo, string_view host,
            string_view port, string_view path, string_view query,
            string_view fragment) noexcept
    : scheme(scheme), userinfo(userinfo), host(host),
      port(port), path(path), query(query), fragment(fragment) {}
};

class uri_view {
  uri_parts parts;
  uri_view(const uri_parts& parts) : parts(parts) {}
 public:
  bool valid() const noexcept;
  bool empty() const noexcept;

  string_view scheme() const noexcept { return parts.scheme; }
  string_view authority() const noexcept {
    auto begin = parts.userinfo.data();
    auto end = parts.port.data() + parts.port.size();
    return {begin, static_cast<size_t>(end - begin)};
  }

  string_view userinfo() const noexcept { return parts.userinfo; }
  string_view host() const noexcept { return parts.host; }
  string_view port() const noexcept { return parts.port; }

  string_view path() const noexcept { return parts.path; }
  string_view query() const noexcept { return parts.query; }
  string_view fragment() const noexcept { return parts.fragment; }

  string_view get() const noexcept {
    auto begin = parts.scheme.data();
    auto end = parts.fragment.data() + parts.fragment.size();
    return {begin, static_cast<size_t>(end - begin)};
  }

  static uri_view parse(string_view str);
};

uri_view uri_view::parse(const string_view str)
{
  uri_parts parts;
  bool absolute = true;
  size_t pos = 0;

  // scheme:
  auto scheme_end = str.find(':');
  if (scheme_end == string_view::npos) {
    scheme_end = 0;
    absolute = false;
  } else {
    auto slash = str.find('/');
    if (slash != string_view::npos && slash < scheme_end) {
      scheme_end = 0;
      absolute = false;
    } else {
      pos = scheme_end + 1;
    }
  }
  parts.scheme = str.substr(0, scheme_end);
  // //authority
  if (str.find("//", pos) == pos) {
    const auto auth_begin = pos + 2;
    auto auth_end = str.find('/', auth_begin);
    if (auth_end == string_view::npos)
      auth_end = str.size();
    // userinfo@
    auto userinfo_end = str.find('@', auth_begin);
    auto host_begin = auth_begin;
    if (userinfo_end > auth_end) {
      userinfo_end = auth_begin;
    } else {
      host_begin = userinfo_end + 1;
    }
    parts.userinfo = str.substr(auth_begin, userinfo_end - auth_begin);
    // host:port
    const auto host_end = str.find(':', host_begin);
    if (host_end < auth_end) {
      parts.port = str.substr(host_end + 1, auth_end - (host_end + 1));
    } else {
      parts.port = str.substr(auth_end, 0);
    }
    parts.host = str.substr(host_begin, host_end - host_begin);
    pos = auth_end;
  } else {
    parts.userinfo = parts.host = parts.port = str.substr(pos, 0);
  }
  // path?query#fragment
  const auto path_begin = pos;
  auto path_end = str.find('?', path_begin);
  if (path_end != string_view::npos) {
    const auto query_begin = path_end + 1;
    auto query_end = str.find('#', query_begin);
    if (query_end != string_view::npos) {
      parts.fragment = str.substr(query_end + 1);
    } else {
      query_end = str.size();
      parts.fragment = str.substr(query_end, 0);
    }
    parts.query = str.substr(query_begin, query_end - query_begin);
  } else {
    // no query
    path_end = str.find('#', path_begin);
    if (path_end != string_view::npos) {
      parts.query = str.substr(path_end + 1, 0);
      parts.fragment = str.substr(path_end + 1);
    } else {
      path_end = str.size();
      parts.query = parts.fragment = str.substr(path_end, 0);
    }
  }
  parts.path = str.substr(path_begin, path_end - path_begin);
  return uri_view(parts);
}

} // namespace requests
