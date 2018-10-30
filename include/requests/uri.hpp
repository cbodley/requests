#pragma once

#include <string_view>

namespace requests {

using string_view = std::string_view;

// uri_string for mutation, storage and c_str()

// a structured representation of the parts
struct uri_parts {
  struct part { size_t begin = 0; size_t end = 0; };
  part scheme;
  part userinfo;
  part host;
  part port;
  part path;
  part query;
  part fragment;

  void parse(string_view str);
};

// an immutable uri view
class uri_view {
  string_view value;
  uri_parts parts;
  uri_view(string_view value, const uri_parts& parts)
    : value(value), parts(parts) {}
  string_view part(const uri_parts::part& p) const noexcept {
    return value.substr(p.begin, p.end - p.begin);
  }
 public:
  uri_view() = default;
  uri_view(const uri_view&) = default;
  uri_view& operator=(const uri_view&) = default;
  uri_view(uri_view&&) = default;
  uri_view& operator=(uri_view&&) = default;

  string_view get() const noexcept { return value; }
  bool empty() const noexcept {
    return parts.scheme.begin == parts.fragment.end;
  }

  string_view scheme() const noexcept { return part(parts.scheme); }
  string_view authority() const noexcept {
    return value.substr(parts.userinfo.begin,
                        parts.port.end - parts.userinfo.begin);
  }
  string_view userinfo() const noexcept { return part(parts.userinfo); }
  string_view host() const noexcept { return part(parts.host); }
  string_view port() const noexcept { return part(parts.port); }
  string_view path() const noexcept { return part(parts.path); }
  string_view query() const noexcept { return part(parts.query); }
  string_view fragment() const noexcept { return part(parts.fragment); }

  static uri_view parse(string_view str);
};

uri_view uri_view::parse(const string_view str)
{
  uri_parts p;
  p.parse(str);
  return uri_view(str, p);
}

void uri_parts::parse(string_view str)
{
  size_t pos = 0;

  // scheme:
  scheme.end = str.find(':');
  if (scheme.end == string_view::npos) {
    scheme.end = 0;
  } else {
    // if : comes after / it isn't part of the scheme
    auto slash = str.find('/');
    if (slash != string_view::npos && slash < scheme.end) {
      scheme.end = 0;
    } else {
      pos = scheme.end + 1;
    }
  }
  // //authority
  if (str.find("//", pos) == pos) {
    userinfo.begin = pos + 2;
    port.end = str.find('/', userinfo.begin); // end of authority
    if (port.end == string_view::npos)
      port.end = str.size();
    // userinfo@
    userinfo.end = str.find('@', userinfo.begin);
    if (userinfo.end > port.end) { // @ outside of authority
      userinfo.end = userinfo.begin; // no userinfo
      host.begin = userinfo.begin;
    } else {
      host.begin = userinfo.end + 1; // after @
    }
    // host:port
    host.end = str.find(':', host.begin);
    if (host.end < port.end) {
      port.begin = host.end + 1; // after :
    } else {
      port.begin = port.end; // no port
    }
    path.begin = port.end;
  } else {
    // no authority
    userinfo.begin = userinfo.end = pos;
    host.begin = host.end = pos;
    port.begin = port.end = pos;
    path.begin = pos;
  }
  // path?query#fragment
  path.end = str.find('?', path.begin);
  if (path.end != string_view::npos) {
    query.begin = path.end + 1; // after ?
    query.end = str.find('#', path.begin);
    if (query.end == string_view::npos) {
      query.end = str.size();
      fragment.begin = fragment.end = query.end; // no fragment
    } else if (query.end < path.end) { // # before ?
      path.end = query.begin = query.end; // no query
      fragment.begin = query.end + 1; // after #
    } else {
      fragment.begin = query.end + 1; // after #
      fragment.end = str.size();
    }
  } else {
    // no query
    path.end = str.find('#', path.begin);
    if (path.end != string_view::npos) {
      query.begin = query.end = path.end; // no query
      fragment.begin = path.end + 1; // after #
      fragment.end = str.size();
    } else {
      path.end = str.size();
      query.begin = query.end = path.end; // no query
      fragment.begin = fragment.end = path.end; // no fragment
    }
  }
}

} // namespace requests
