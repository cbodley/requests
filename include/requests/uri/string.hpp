#pragma once

#include <algorithm>

#include <requests/uri/parser.hpp>

namespace requests {

// mutable uri string
class uri_string {
  std::string value;
  uri_parts parts;
  uri_string(std::string&& value, const uri_parts& parts)
    : value(std::move(value)), parts(parts) {}
  std::string_view part(const uri_parts::part& p) const noexcept {
    return get().substr(p.begin, p.end - p.begin);
  }
 public:
  uri_string() = default;
  uri_string(const uri_string&) = default;
  uri_string& operator=(const uri_string&) = default;
  uri_string(uri_string&&) = default;
  uri_string& operator=(uri_string&&) = default;

  std::string_view get() const noexcept { return value; }
  bool empty() const noexcept {
    return parts.scheme.begin == parts.fragment.end;
  }

  std::string_view scheme() const noexcept { return part(parts.scheme); }
  std::string_view authority() const noexcept {
    return get().substr(parts.userinfo.begin,
                        parts.port.end - parts.userinfo.begin);
  }
  std::string_view userinfo() const noexcept { return part(parts.userinfo); }
  std::string_view host() const noexcept { return part(parts.host); }
  std::string_view port() const noexcept { return part(parts.port); }
  std::string_view path() const noexcept { return part(parts.path); }
  std::string_view query() const noexcept { return part(parts.query); }
  std::string_view fragment() const noexcept { return part(parts.fragment); }

  uri_string& scheme(std::string_view str);
  uri_string& userinfo(std::string_view str);
  uri_string& host(std::string_view str);
  uri_string& port(std::string_view str);
  uri_string& path(std::string_view str);
  uri_string& query(std::string_view str);
  uri_string& query(std::string_view key, std::string_view val);
  uri_string& add_query(std::string_view str);
  uri_string& add_query(std::string_view key, std::string_view val);
  uri_string& fragment(std::string_view str);

  static uri_string parse(std::string str);
 private:
  void replace(const uri_parts::part& p, std::string_view str) {
    value.replace(p.begin, p.end - p.begin, str);
  }
  template <typename ...Strings>
  auto replace(const uri_parts::part& part, Strings&& ...str) {
    const ssize_t offset = (std::size(str) + ...);
    // replace the existing part with zeros
    value.replace(part.begin, part.end - part.begin, offset, 0);
    // copy each string
    auto p = value.begin() + part.begin;
    ((p = std::copy(std::begin(str), std::end(str), p)), ...);
    return p;
  }
  void shift_part(ssize_t offset, uri_parts::part& p) {
    p.begin += offset;
    p.end += offset;
  }
  template <typename ...Parts>
  void shift(ssize_t offset, Parts& ...p) {
    (shift_part(offset, p), ...);
  }
};

uri_string& uri_string::scheme(std::string_view str)
{
  // add : if scheme was empty
  const bool first = parts.scheme.begin == parts.scheme.end;
  const std::string_view colon(":", first ? 1 : 0);
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
uri_string& uri_string::userinfo(const std::string_view str)
{
  // add @ if userinfo was empty
  const bool first = parts.userinfo.begin == parts.userinfo.end;
  const std::string_view amp("@", first ? 1 : 0);
  // or remove @ when clearing userinfo
  if (!first && str.empty())
    parts.userinfo.end++;

  // add // if authority was empty
  const bool firstauth = parts.userinfo.begin == parts.port.end;
  const std::string_view auth("//", firstauth ? 2 : 0);
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
uri_string& uri_string::host(const std::string_view str)
{
  // add // if authority was empty
  const bool firstauth = parts.userinfo.begin == parts.port.end;
  const std::string_view auth("//", firstauth ? 2 : 0);
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
    uri_parts::part p{parts.userinfo.begin, parts.userinfo.begin};
    replace(p, auth, str);
    replace(parts.host, str);
  }
  shift(auth.size(), parts.userinfo);
  parts.host.begin += auth.size();
  parts.host.end += offset;
  shift(offset, parts.port, parts.path, parts.query, parts.fragment);
  return *this;
}
uri_string& uri_string::port(const std::string_view str)
{
  // add : if port was empty
  const bool first = parts.port.begin == parts.port.end;
  const std::string_view colon(":", first ? 1 : 0);
  // or remove : when clearing port
  if (!first && str.empty())
    parts.port.begin--;

  // add // if authority was empty
  const bool firstauth = parts.userinfo.begin == parts.port.end;
  const std::string_view auth("//", firstauth ? 2 : 0);
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
    uri_parts::part p{parts.userinfo.begin, parts.userinfo.begin};
    replace(p, auth, str);
    replace(parts.port, colon, str);
  }
  shift(auth.size(), parts.userinfo, parts.host);
  parts.port.begin += auth.size() + colon.size();
  parts.port.end += offset;
  shift(offset, parts.path, parts.query, parts.fragment);
  return *this;
}
uri_string& uri_string::path(const std::string_view str)
{
  const ssize_t offset = str.size() - (parts.path.end - parts.path.begin);
  replace(parts.path, str);
  parts.path.end += offset;
  shift(offset, parts.query, parts.fragment);
  return *this;
}
uri_string& uri_string::query(const std::string_view str)
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
uri_string& uri_string::query(const std::string_view key,
                              const std::string_view val)
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
uri_string& uri_string::add_query(const std::string_view str)
{
  const bool first = (parts.path.end == parts.query.begin);
  const char c[1] = {first ? '?' : '&'};
  const ssize_t offset = sizeof(c) + str.size();
  // append after existing query
  uri_parts::part part{parts.query.end, parts.query.end};
  replace(part, c, str);
  if (first)
    parts.query.begin++; // past ?
  parts.query.end += offset;
  shift(offset, parts.fragment);
  return *this;
}
uri_string& uri_string::add_query(const std::string_view key,
                                  const std::string_view val)
{
  const bool first = (parts.path.end == parts.query.begin);
  const char c[1] = {first ? '?' : '&'};
  const char eq[1] = {'='};
  const ssize_t offset = sizeof(c) + key.size() + sizeof(eq) + val.size();
  // append after existing query
  uri_parts::part part{parts.query.end, parts.query.end};
  replace(part, c, key, eq, val);
  if (first)
    parts.query.begin++; // past ?
  parts.query.end += offset;
  shift(offset, parts.fragment);
  return *this;
}
uri_string& uri_string::fragment(const std::string_view str)
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

uri_string uri_string::parse(std::string str)
{
  uri_parts parts;
  parts.parse(str);
  return uri_string(std::move(str), parts);
}

} // namespace requests
