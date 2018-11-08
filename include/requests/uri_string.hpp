#pragma once

#include <algorithm>
#include <string>

#include <requests/detail/uri_parser.hpp>
#include <requests/uri_view.hpp>

namespace requests {

// mutable uri string
template <typename CharT, typename Traits = std::char_traits<CharT>,
          typename Allocator = std::allocator<CharT>>
class basic_uri_string {
  using string_type = std::basic_string<CharT, Traits, Allocator>;
  using view_type = std::basic_string_view<CharT, Traits>;
  string_type value;
  detail::uri_parts parts;
  basic_uri_string(string_type&& value, const detail::uri_parts& parts)
    : value(std::move(value)), parts(parts) {}
  view_type part(const detail::uri_parts::part& p) const noexcept {
    return get().substr(p.begin, p.end - p.begin);
  }
 public:
  basic_uri_string() = default;
  explicit basic_uri_string(const Allocator& alloc) noexcept : value(alloc) {}
  basic_uri_string(const basic_uri_string&) = default;
  basic_uri_string& operator=(const basic_uri_string&) = default;
  basic_uri_string(basic_uri_string&&) = default;
  basic_uri_string& operator=(basic_uri_string&&) = default;

  basic_uri_string(const uri_view& view)
    : value(view.value), parts(view.parts) {}
  basic_uri_string& operator=(const uri_view& view) {
    value = view.value;
    parts = view.parts;
    return *this;
  }

  view_type get() const noexcept { return value; }
  bool empty() const noexcept {
    return parts.scheme.begin == parts.fragment.end;
  }

  operator uri_view() & { return uri_view(value, parts); }
  operator uri_view() const & { return uri_view(value, parts); }
  // no conversion for &&

  view_type scheme() const noexcept { return part(parts.scheme); }
  view_type authority() const noexcept {
    return get().substr(parts.userinfo.begin,
                        parts.port.end - parts.userinfo.begin);
  }
  view_type userinfo() const noexcept { return part(parts.userinfo); }
  view_type host() const noexcept { return part(parts.host); }
  view_type port() const noexcept { return part(parts.port); }
  view_type path() const noexcept { return part(parts.path); }
  view_type query() const noexcept { return part(parts.query); }
  view_type fragment() const noexcept { return part(parts.fragment); }

  basic_uri_string& scheme(view_type str);
  basic_uri_string& userinfo(view_type str);
  basic_uri_string& host(view_type str);
  basic_uri_string& port(view_type str);
  basic_uri_string& path(view_type str);
  basic_uri_string& query(view_type str);
  basic_uri_string& query(view_type key, view_type val);
  basic_uri_string& add_query(view_type str);
  basic_uri_string& add_query(view_type key, view_type val);
  basic_uri_string& fragment(view_type str);

  static basic_uri_string parse(string_type str);
 private:
  void replace(const detail::uri_parts::part& p, view_type str) {
    value.replace(p.begin, p.end - p.begin, str);
  }
  template <typename ...Strings>
  auto replace(const detail::uri_parts::part& part, Strings&& ...str) {
    const ssize_t offset = (std::size(str) + ...);
    // replace the existing part with zeros
    value.replace(part.begin, part.end - part.begin, offset, 0);
    // copy each string
    auto p = value.begin() + part.begin;
    ((p = std::copy(std::begin(str), std::end(str), p)), ...);
    return p;
  }
  void shift_part(ssize_t offset, detail::uri_parts::part& p) {
    p.begin += offset;
    p.end += offset;
  }
  template <typename ...Parts>
  void shift(ssize_t offset, Parts& ...p) {
    (shift_part(offset, p), ...);
  }
};

template <typename C, typename T, typename A>
basic_uri_string<C, T, A>&
basic_uri_string<C, T, A>::scheme(view_type str)
{
  // add : if scheme was empty
  const bool first = parts.scheme.begin == parts.scheme.end;
  const view_type colon(":", first ? 1 : 0);
  // or remove : when clearing scheme
  if (!first && str.empty())
    parts.scheme.end++;

  ssize_t offset = str.size() - (parts.scheme.end - parts.scheme.begin);
  replace(parts.scheme, str, colon);
  parts.scheme.end += offset;
  offset += colon.size();
  shift(offset, parts.userinfo, parts.host, parts.port,
        parts.path, parts.query, parts.fragment);
  return *this;
}
template <typename C, typename T, typename A>
basic_uri_string<C, T, A>&
basic_uri_string<C, T, A>::userinfo(const view_type str)
{
  // add @ if userinfo was empty
  const bool first = parts.userinfo.begin == parts.userinfo.end;
  const view_type amp("@", first ? 1 : 0);
  // or remove @ when clearing userinfo
  if (!first && str.empty())
    parts.userinfo.end++;

  // add // if authority was empty
  const bool firstauth = parts.userinfo.begin == parts.port.end;
  const view_type auth("//", firstauth ? 2 : 0);
  // or remove // when clearing authority
  if (!firstauth && str.empty() && parts.host.begin == parts.port.end)
    parts.userinfo.begin -= 2;

  ssize_t offset = (auth.size() + str.size()) -
      (parts.userinfo.end - parts.userinfo.begin);
  replace(parts.userinfo, auth, str, amp);
  parts.userinfo.begin += auth.size();
  parts.userinfo.end += offset;
  offset += amp.size();
  shift(offset, parts.host, parts.port,
        parts.path, parts.query, parts.fragment);
  return *this;
}
template <typename C, typename T, typename A>
basic_uri_string<C, T, A>&
basic_uri_string<C, T, A>::host(const view_type str)
{
  // add // if authority was empty
  const bool firstauth = parts.userinfo.begin == parts.port.end;
  const view_type auth("//", firstauth ? 2 : 0);
  // or remove // when clearing authority
  if (!firstauth && str.empty() &&
      parts.userinfo.begin == parts.userinfo.end &&
      parts.port.begin == parts.port.end) {
    shift(-2, parts.userinfo);
    parts.host.begin -= 2;
  }

  ssize_t offset = (auth.size() + str.size()) -
      (parts.host.end - parts.host.begin);
  if (!firstauth) {
    replace(parts.host, str);
  } else if (parts.userinfo.begin == parts.userinfo.end) {
    replace(parts.host, auth, str);
  } else {
    detail::uri_parts::part p{parts.userinfo.begin, parts.userinfo.begin};
    replace(p, auth, str);
    replace(parts.host, str);
  }
  shift(auth.size(), parts.userinfo);
  parts.host.begin += auth.size();
  parts.host.end += offset;
  shift(offset, parts.port, parts.path, parts.query, parts.fragment);
  return *this;
}
template <typename C, typename T, typename A>
basic_uri_string<C, T, A>&
basic_uri_string<C, T, A>::port(const view_type str)
{
  // add : if port was empty
  const bool first = parts.port.begin == parts.port.end;
  const view_type colon(":", first ? 1 : 0);
  // or remove : when clearing port
  if (!first && str.empty())
    parts.port.begin--;

  // add // if authority was empty
  const bool firstauth = parts.userinfo.begin == parts.port.end;
  const view_type auth("//", firstauth ? 2 : 0);
  // or remove // when clearing authority
  if (!firstauth && str.empty() && parts.userinfo.begin == parts.host.end) {
    shift(-2, parts.userinfo, parts.host);
    parts.port.begin -= 2;
  }

  ssize_t offset = (auth.size() + colon.size() + str.size()) -
      (parts.port.end - parts.port.begin);
  if (auth.empty()) {
    replace(parts.port, colon, str);
  } else if (parts.userinfo.begin == parts.userinfo.end) {
    replace(parts.port, auth, colon, str);
  } else {
    detail::uri_parts::part p{parts.userinfo.begin, parts.userinfo.begin};
    replace(p, auth, str);
    replace(parts.port, colon, str);
  }
  shift(auth.size(), parts.userinfo, parts.host);
  parts.port.begin += auth.size() + colon.size();
  parts.port.end += offset;
  shift(offset, parts.path, parts.query, parts.fragment);
  return *this;
}
template <typename C, typename T, typename A>
basic_uri_string<C, T, A>&
basic_uri_string<C, T, A>::path(const view_type str)
{
  const ssize_t offset = str.size() - (parts.path.end - parts.path.begin);
  replace(parts.path, str);
  parts.path.end += offset;
  shift(offset, parts.query, parts.fragment);
  return *this;
}
template <typename C, typename T, typename A>
basic_uri_string<C, T, A>&
basic_uri_string<C, T, A>::query(const view_type str)
{
  const bool first = (parts.path.end == parts.query.begin);
  if (!first && str.empty())
    parts.query.begin--; // remove ? when clearing query
  const char eq[1] = {'='};
  ssize_t offset = str.size() - (parts.query.end - parts.query.begin);
  if (first) {
    const char c[1] = {'?'};
    replace(parts.query, c, str);
    parts.query.begin++; // past ?
    offset++;
  } else {
    replace(parts.query, str);
  }
  parts.query.end += offset;
  shift(offset, parts.fragment);
  return *this;
}
template <typename C, typename T, typename A>
basic_uri_string<C, T, A>&
basic_uri_string<C, T, A>::query(const view_type key,
                                 const view_type val)
{
  const bool first = (parts.path.end == parts.query.begin);
  const char eq[1] = {'='};
  ssize_t offset = (key.size() + sizeof(eq) + val.size()) -
      (parts.query.end - parts.query.begin);
  if (first) {
    const char c[1] = {'?'};
    replace(parts.query, c, key, eq, val);
    parts.query.begin++; // past ?
    offset++;
  } else {
    replace(parts.query, key, eq, val);
  }
  parts.query.end += offset;
  shift(offset, parts.fragment);
  return *this;
}
template <typename C, typename T, typename A>
basic_uri_string<C, T, A>&
basic_uri_string<C, T, A>::add_query(const view_type str)
{
  const bool first = (parts.path.end == parts.query.begin);
  const char c[1] = {first ? '?' : '&'};
  const ssize_t offset = sizeof(c) + str.size();
  // append after existing query
  detail::uri_parts::part part{parts.query.end, parts.query.end};
  replace(part, c, str);
  if (first)
    parts.query.begin++; // past ?
  parts.query.end += offset;
  shift(offset, parts.fragment);
  return *this;
}
template <typename C, typename T, typename A>
basic_uri_string<C, T, A>&
basic_uri_string<C, T, A>::add_query(const view_type key,
                                     const view_type val)
{
  const bool first = (parts.path.end == parts.query.begin);
  const char c[1] = {first ? '?' : '&'};
  const char eq[1] = {'='};
  const ssize_t offset = sizeof(c) + key.size() + sizeof(eq) + val.size();
  // append after existing query
  detail::uri_parts::part part{parts.query.end, parts.query.end};
  replace(part, c, key, eq, val);
  if (first)
    parts.query.begin++; // past ?
  parts.query.end += offset;
  shift(offset, parts.fragment);
  return *this;
}
template <typename C, typename T, typename A>
basic_uri_string<C, T, A>&
basic_uri_string<C, T, A>::fragment(const view_type str)
{
  const bool first = (parts.query.end == parts.fragment.begin);
  if (!first && str.empty())
    parts.fragment.begin--; // remove # when clearing fragment
  ssize_t offset = str.size() - (parts.fragment.end - parts.fragment.begin);
  if (first) { // no # yet
    offset++;
    const char c[] = {'#'};
    replace(parts.fragment, c, str);
    parts.fragment.begin++; // doesn't include #
    parts.fragment.end += offset;
  } else {
    replace(parts.fragment, str);
  }
  parts.fragment.end += offset;
  return *this;
}

template <typename C, typename T, typename A>
basic_uri_string<C, T, A>
basic_uri_string<C, T, A>::parse(string_type str)
{
  detail::uri_parts parts;
  parts.parse(view_type(str));
  return basic_uri_string(std::move(str), parts);
}

using uri_string = basic_uri_string<char>;

} // namespace requests
