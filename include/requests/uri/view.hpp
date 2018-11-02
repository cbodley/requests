#pragma once

#include <requests/uri/parser.hpp>

namespace requests {

// an immutable uri view
class uri_view {
  std::string_view value;
  uri_parts parts;
  uri_view(std::string_view value, const uri_parts& parts)
    : value(value), parts(parts) {}
  std::string_view part(const uri_parts::part& p) const noexcept {
    return value.substr(p.begin, p.end - p.begin);
  }
 public:
  uri_view() = default;
  uri_view(const uri_view&) = default;
  uri_view& operator=(const uri_view&) = default;
  uri_view(uri_view&&) = default;
  uri_view& operator=(uri_view&&) = default;

  std::string_view get() const noexcept { return value; }
  bool empty() const noexcept {
    return parts.scheme.begin == parts.fragment.end;
  }

  std::string_view scheme() const noexcept { return part(parts.scheme); }
  std::string_view authority() const noexcept {
    return value.substr(parts.userinfo.begin,
                        parts.port.end - parts.userinfo.begin);
  }
  std::string_view userinfo() const noexcept { return part(parts.userinfo); }
  std::string_view host() const noexcept { return part(parts.host); }
  std::string_view port() const noexcept { return part(parts.port); }
  std::string_view path() const noexcept { return part(parts.path); }
  std::string_view query() const noexcept { return part(parts.query); }
  std::string_view fragment() const noexcept { return part(parts.fragment); }

  static uri_view parse(std::string_view str);
};

uri_view uri_view::parse(std::string_view str)
{
  uri_parts parts;
  parts.parse(str);
  return uri_view(str, parts);
}

} // namespace requests
