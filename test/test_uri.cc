#include "requests/uri.hpp"
#include <gtest/gtest.h>

TEST(uri_view, parse_absolute)
{
  {
    auto u = requests::uri_view::parse("http:");
    EXPECT_EQ("http", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("http:", u.get());
  }
  {
    auto u = requests::uri_view::parse("scheme:path");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("scheme:path", u.get());
  }
  {
    auto u = requests::uri_view::parse("scheme:path?query");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("scheme:path?query", u.get());
  }
  {
    auto u = requests::uri_view::parse("scheme:path#fragment");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("scheme:path#fragment", u.get());
  }
  {
    auto u = requests::uri_view::parse("scheme:path?query#fragment");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("scheme:path?query#fragment", u.get());
  }
  {
    auto u = requests::uri_view::parse("scheme:?query");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("scheme:?query", u.get());
  }
  {
    auto u = requests::uri_view::parse("scheme:#fragment");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("scheme:#fragment", u.get());
  }
}

TEST(uri_view, parse_absolute_authority)
{
  {
    auto u = requests::uri_view::parse("http://host.com");
    EXPECT_EQ("http", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("host.com", u.authority());
    EXPECT_EQ("http://host.com", u.get());
  }
  {
    auto u = requests::uri_view::parse("http://host.com:8080");
    EXPECT_EQ("http", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("8080", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("host.com:8080", u.authority());
    EXPECT_EQ("http://host.com:8080", u.get());
  }
  {
    auto u = requests::uri_view::parse("http://userinfo@host.com");
    EXPECT_EQ("http", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("userinfo@host.com", u.authority());
    EXPECT_EQ("http://userinfo@host.com", u.get());
  }
  {
    auto u = requests::uri_view::parse("http://userinfo@host.com:8080");
    EXPECT_EQ("http", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("8080", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("userinfo@host.com:8080", u.authority());
    EXPECT_EQ("http://userinfo@host.com:8080", u.get());
  }
  {
    auto u = requests::uri_view::parse("http://userinfo@host.com:8080/path");
    EXPECT_EQ("http", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("8080", u.port());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("userinfo@host.com:8080", u.authority());
    EXPECT_EQ("http://userinfo@host.com:8080/path", u.get());
  }
  {
    auto u = requests::uri_view::parse("http://userinfo@host.com:8080/path?query");
    EXPECT_EQ("http", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("8080", u.port());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("userinfo@host.com:8080", u.authority());
    EXPECT_EQ("http://userinfo@host.com:8080/path?query", u.get());
  }
  {
    auto u = requests::uri_view::parse("http://userinfo@host.com:8080/path?query#fragment");
    EXPECT_EQ("http", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("8080", u.port());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("userinfo@host.com:8080", u.authority());
    EXPECT_EQ("http://userinfo@host.com:8080/path?query#fragment", u.get());
  }
  {
    auto u = requests::uri_view::parse("http://userinfo@host.com:8080/path#fragment");
    EXPECT_EQ("http", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("8080", u.port());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("userinfo@host.com:8080", u.authority());
    EXPECT_EQ("http://userinfo@host.com:8080/path#fragment", u.get());
  }
}

TEST(uri_view, parse_relative)
{
  {
    auto u = requests::uri_view::parse("");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("", u.get());
  }
  {
    auto u = requests::uri_view::parse("path");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("path", u.get());
  }
  {
    auto u = requests::uri_view::parse("/path");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("/path", u.get());
  }
  {
    auto u = requests::uri_view::parse("path?query");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("path?query", u.get());
  }
  {
    auto u = requests::uri_view::parse("path#fragment");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("path#fragment", u.get());
  }
  {
    auto u = requests::uri_view::parse("/pa:th");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("/pa:th", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
  }
  {
    auto u = requests::uri_view::parse("./pa:th");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("./pa:th", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
  }
  {
    auto u = requests::uri_view::parse("?query");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("", u.fragment());
  }
  {
    auto u = requests::uri_view::parse("#fragment");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.authority());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("fragment", u.fragment());
  }
}

TEST(uri_view, parse_relative_authority)
{
  {
    auto u = requests::uri_view::parse("//host.com");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("host.com", u.authority());
    EXPECT_EQ("//host.com", u.get());
  }
  {
    auto u = requests::uri_view::parse("//host.com:8080");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("8080", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("host.com:8080", u.authority());
    EXPECT_EQ("//host.com:8080", u.get());
  }
  {
    auto u = requests::uri_view::parse("//userinfo@host.com");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("userinfo@host.com", u.authority());
    EXPECT_EQ("//userinfo@host.com", u.get());
  }
  {
    auto u = requests::uri_view::parse("//userinfo@host.com:8080");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("8080", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("userinfo@host.com:8080", u.authority());
    EXPECT_EQ("//userinfo@host.com:8080", u.get());
  }
  {
    auto u = requests::uri_view::parse("//userinfo@host.com:8080/path");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("8080", u.port());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("userinfo@host.com:8080", u.authority());
    EXPECT_EQ("//userinfo@host.com:8080/path", u.get());
  }
  {
    auto u = requests::uri_view::parse("//userinfo@host.com:8080/path?query");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("8080", u.port());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("userinfo@host.com:8080", u.authority());
    EXPECT_EQ("//userinfo@host.com:8080/path?query", u.get());
  }
  {
    auto u = requests::uri_view::parse("//userinfo@host.com:8080/path?query#fragment");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("8080", u.port());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("userinfo@host.com:8080", u.authority());
    EXPECT_EQ("//userinfo@host.com:8080/path?query#fragment", u.get());
  }
  {
    auto u = requests::uri_view::parse("//userinfo@host.com:8080/path#fragment");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host.com", u.host());
    EXPECT_EQ("8080", u.port());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("userinfo@host.com:8080", u.authority());
    EXPECT_EQ("//userinfo@host.com:8080/path#fragment", u.get());
  }
}
