#pragma once

#include <string_view>

namespace requests::detail {

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

  template <typename CharT, typename Traits>
  void parse(std::basic_string_view<CharT, Traits> str);
};

template <typename CharT, typename Traits>
void uri_parts::parse(const std::basic_string_view<CharT, Traits> str)
{
  size_t pos = 0;

  // scheme:
  scheme.end = str.find(':');
  if (scheme.end == std::string_view::npos) {
    scheme.end = 0;
  } else {
    // if : comes after / it isn't part of the scheme
    auto slash = str.find('/');
    if (slash != std::string_view::npos && slash < scheme.end) {
      scheme.end = 0;
    } else {
      pos = scheme.end + 1;
    }
  }
  // //authority
  if (str.find("//", pos) == pos) {
    userinfo.begin = pos + 2;
    port.end = str.find('/', userinfo.begin); // end of authority
    if (port.end == std::string_view::npos)
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
      host.end = port.end;
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
  if (path.end != std::string_view::npos) {
    query.begin = path.end + 1; // after ?
    query.end = str.find('#', path.begin);
    if (query.end == std::string_view::npos) {
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
    if (path.end != std::string_view::npos) {
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

} // namespace requests::detail
