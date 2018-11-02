#include "requests/uri.hpp"
#include <iostream>
#include <gtest/gtest.h>

TEST(uri_string, scheme)
{
  {
    auto u = requests::uri_string{};
    u.scheme("scheme");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("scheme:", u.get());

    u.scheme("");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.userinfo("userinfo");
    ASSERT_EQ("userinfo", u.userinfo());
    ASSERT_EQ("//userinfo@", u.get());

    u.scheme("scheme");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("scheme://userinfo@", u.get());

    u.scheme("s");
    EXPECT_EQ("s", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("s://userinfo@", u.get());

    u.scheme("schemeschemescheme");
    EXPECT_EQ("schemeschemescheme", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("schemeschemescheme://userinfo@", u.get());

    u.scheme("");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("//userinfo@", u.get());

    u.scheme("scheme");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("scheme://userinfo@", u.get());
  }
}

TEST(uri_string, userinfo)
{
  {
    auto u = requests::uri_string{};
    u.userinfo("userinfo");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("//userinfo@", u.get());

    u.userinfo("");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.scheme("scheme");
    ASSERT_EQ("scheme", u.scheme());
    ASSERT_EQ("", u.userinfo());
    ASSERT_EQ("scheme:", u.get());

    u.userinfo("userinfo");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("scheme://userinfo@", u.get());

    u.userinfo("");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("scheme:", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.host("host");
    ASSERT_EQ("", u.userinfo());
    ASSERT_EQ("host", u.host());
    ASSERT_EQ("//host", u.get());

    u.userinfo("userinfo");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("//userinfo@host", u.get());

    u.userinfo("");
    EXPECT_EQ("", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("//host", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.scheme("scheme").host("host");
    ASSERT_EQ("scheme", u.scheme());
    ASSERT_EQ("", u.userinfo());
    ASSERT_EQ("host", u.host());
    ASSERT_EQ("scheme://host", u.get());

    u.userinfo("userinfo");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("scheme://userinfo@host", u.get());

    u.userinfo("u");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("u", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("scheme://u@host", u.get());

    u.userinfo("userinfouserinfouserinfo");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("userinfouserinfouserinfo", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("scheme://userinfouserinfouserinfo@host", u.get());

    u.userinfo("");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("scheme://host", u.get());

    u.userinfo("userinfo");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("scheme://userinfo@host", u.get());
  }
}

TEST(uri_string, host)
{
  {
    auto u = requests::uri_string{};
    u.host("host");
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("//host", u.get());

    u.host("");
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.userinfo("userinfo");
    ASSERT_EQ("userinfo", u.userinfo());
    ASSERT_EQ("", u.host());
    ASSERT_EQ("//userinfo@", u.get());

    u.host("host");
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("//userinfo@host", u.get());

    u.host("");
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("//userinfo@", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.port("port");
    ASSERT_EQ("", u.host());
    ASSERT_EQ("port", u.port());
    ASSERT_EQ("//:port", u.get());

    u.host("host");
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("//host:port", u.get());

    u.host("");
    EXPECT_EQ("", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("//:port", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.userinfo("userinfo").port("port");
    ASSERT_EQ("userinfo", u.userinfo());
    ASSERT_EQ("", u.host());
    ASSERT_EQ("port", u.port());
    ASSERT_EQ("//userinfo@:port", u.get());

    u.host("host");
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("//userinfo@host:port", u.get());

    u.host("h");
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("h", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("//userinfo@h:port", u.get());

    u.host("hosthosthost");
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("hosthosthost", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("//userinfo@hosthosthost:port", u.get());

    u.host("");
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("//userinfo@:port", u.get());

    u.host("host");
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("//userinfo@host:port", u.get());
  }
}

TEST(uri_string, port)
{
  {
    auto u = requests::uri_string{};
    u.port("port");
    EXPECT_EQ("", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("//:port", u.get());

    u.port("");
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.host("host");
    ASSERT_EQ("host", u.host());
    ASSERT_EQ("", u.port());
    ASSERT_EQ("//host", u.get());

    u.port("port");
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("//host:port", u.get());

    u.port("");
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("//host", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.path("/path");
    ASSERT_EQ("", u.port());
    ASSERT_EQ("/path", u.path());
    ASSERT_EQ("/path", u.get());

    u.port("port");
    EXPECT_EQ("", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("//:port/path", u.get());

    u.port("");
    EXPECT_EQ("", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("/path", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.host("host").path("path");
    ASSERT_EQ("host", u.host());
    ASSERT_EQ("", u.port());
    ASSERT_EQ("path", u.path());
    ASSERT_EQ("//hostpath", u.get());

    u.port("port");
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("//host:portpath", u.get());

    u.port("p");
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("p", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("//host:ppath", u.get());

    u.port("portportport");
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("portportport", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("//host:portportportpath", u.get());

    u.port("");
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("//hostpath", u.get());

    u.port("port");
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("//host:portpath", u.get());
  }
}

TEST(uri_string, path)
{
  {
    auto u = requests::uri_string{};
    u.path("path");
    EXPECT_EQ("", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("path", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.port("port");
    ASSERT_EQ("port", u.port());
    ASSERT_EQ("", u.path());
    ASSERT_EQ("//:port", u.get());

    u.path("/path");
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("//:port/path", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.query("query");
    ASSERT_EQ("", u.path());
    ASSERT_EQ("query", u.query());
    ASSERT_EQ("?query", u.get());

    u.path("path");
    EXPECT_EQ("", u.port());
    EXPECT_EQ("path", u.path());
    ASSERT_EQ("query", u.query());
    EXPECT_EQ("path?query", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.port("port").query("query");
    ASSERT_EQ("port", u.port());
    ASSERT_EQ("", u.path());
    ASSERT_EQ("query", u.query());
    ASSERT_EQ("//:port?query", u.get());

    u.path("path");
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("//:portpath?query", u.get());

    u.path("p");
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("p", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("//:portp?query", u.get());

    u.path("pathpathpath");
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("pathpathpath", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("//:portpathpathpath?query", u.get());

    u.path("");
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("//:port?query", u.get());

    u.path("path");
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("//:portpath?query", u.get());
  }
}

TEST(uri_string, fragment)
{
  {
    auto u = requests::uri_string{};
    u.fragment("fragment");
    EXPECT_EQ("", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("#fragment", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.query("query");
    ASSERT_EQ("query", u.query());
    ASSERT_EQ("?query", u.get());

    u.fragment("fragment");
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("?query#fragment", u.get());

    u.fragment("f");
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("f", u.fragment());
    EXPECT_EQ("?query#f", u.get());

    u.fragment("fragmentfragmentfragment");
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("fragmentfragmentfragment", u.fragment());
    EXPECT_EQ("?query#fragmentfragmentfragment", u.get());

    u.fragment("");
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("", u.fragment());
    EXPECT_EQ("?query", u.get());

    u.fragment("fragment");
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("?query#fragment", u.get());
  }
}


TEST(uri_string, compose)
{
  {
    auto u = requests::uri_string{};
    u.scheme("scheme")
        .userinfo("userinfo")
        .host("host")
        .port("port")
        .path("/path")
        .query("que", "ry")
        .fragment("fragment");
    EXPECT_EQ("scheme", u.scheme());
    EXPECT_EQ("userinfo", u.userinfo());
    EXPECT_EQ("host", u.host());
    EXPECT_EQ("port", u.port());
    EXPECT_EQ("que=ry", u.query());
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("userinfo@host:port", u.authority());
    std::cout << u.get() << std::endl;
    EXPECT_EQ("scheme://userinfo@host:port/path?que=ry#fragment", u.get());
  }
}

TEST(uri_string, query)
{
  {
    auto u = requests::uri_string{};
    u.query("foo", "bar");
    EXPECT_EQ("", u.path());
    EXPECT_EQ("?foo=bar", u.get());
    EXPECT_EQ("foo=bar", u.query());
    EXPECT_EQ("", u.fragment());
  }
  {
    auto u = requests::uri_string::parse("?query");
    ASSERT_EQ("query", u.query());
    u.query("foo", "bar"); // overwrite
    EXPECT_EQ("", u.path());
    EXPECT_EQ("?foo=bar", u.get());
    EXPECT_EQ("foo=bar", u.query());
    EXPECT_EQ("", u.fragment());
  }
  {
    auto u = requests::uri_string{};
    u.query("foo", "bar");
    u.query("foo", "bar"); // overwrite
    EXPECT_EQ("", u.path());
    EXPECT_EQ("?foo=bar", u.get());
    EXPECT_EQ("foo=bar", u.query());
    EXPECT_EQ("", u.fragment());
  }
  {
    auto u = requests::uri_string::parse("path");
    ASSERT_EQ("path", u.path());
    ASSERT_EQ("path", u.get());
    u.query("foo", "bar");
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("foo=bar", u.query());
    EXPECT_EQ("path?foo=bar", u.get());
  }
  {
    auto u = requests::uri_string::parse("#frag");
    ASSERT_EQ("#frag", u.get());
    u.query("foo", "bar");
    EXPECT_EQ("", u.path());
    EXPECT_EQ("foo=bar", u.query());
    EXPECT_EQ("frag", u.fragment());
    EXPECT_EQ("?foo=bar#frag", u.get());
  }
  {
    auto u = requests::uri_string::parse("#fragmentfragmentfragmentfragment");
    ASSERT_EQ("#fragmentfragmentfragmentfragment", u.get());
    u.query("foo", "bar");
    EXPECT_EQ("", u.path());
    EXPECT_EQ("foo=bar", u.query());
    EXPECT_EQ("fragmentfragmentfragmentfragment", u.fragment());
    EXPECT_EQ("?foo=bar#fragmentfragmentfragmentfragment", u.get());
  }
  {
    auto u = requests::uri_string{};
    u.add_query("a", "1");
    u.add_query("b", "2");
    u.add_query("c", "3");
    EXPECT_EQ("", u.path());
    EXPECT_EQ("?a=1&b=2&c=3", u.get());
    EXPECT_EQ("a=1&b=2&c=3", u.query());
    EXPECT_EQ("", u.fragment());
  }
  {
    auto u = requests::uri_string{};
    u.query("a", "1");
    u.add_query("b", "2");
    u.query("c", "3");
    EXPECT_EQ("", u.path());
    EXPECT_EQ("?c=3", u.get());
    EXPECT_EQ("c=3", u.query());
    EXPECT_EQ("", u.fragment());
  }
  {
    auto u = requests::uri_string::parse("/path?query");
    ASSERT_EQ("/path?query", u.get());
    u.add_query("foo", "bar");
    EXPECT_EQ("/path", u.path());
    EXPECT_EQ("/path?query&foo=bar", u.get());
    EXPECT_EQ("query&foo=bar", u.query());
    EXPECT_EQ("", u.fragment());
  }
  {
    auto u = requests::uri_string{};
    u.path("path").fragment("fragment");
    ASSERT_EQ("path", u.path());
    ASSERT_EQ("", u.query());
    ASSERT_EQ("fragment", u.fragment());
    ASSERT_EQ("path#fragment", u.get());

    u.query("query");
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("path?query#fragment", u.get());

    u.query("p");
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("p", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("path?p#fragment", u.get());

    u.query("queryqueryquery");
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("queryqueryquery", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("path?queryqueryquery#fragment", u.get());

    u.query("");
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("path#fragment", u.get());

    u.query("query");
    EXPECT_EQ("path", u.path());
    EXPECT_EQ("query", u.query());
    EXPECT_EQ("fragment", u.fragment());
    EXPECT_EQ("path?query#fragment", u.get());
  }
}

TEST(uri_string, copy)
{
  auto uri = requests::uri_string::parse("https://domain.com/index.html");
  ASSERT_EQ("https", uri.scheme());
  ASSERT_EQ("domain.com", uri.authority());
  ASSERT_EQ("/index.html", uri.path());
  ASSERT_EQ("https://domain.com/index.html", uri.get());
  {
    requests::uri_string copied{uri}; // copy construct
    EXPECT_EQ("https", copied.scheme());
    EXPECT_EQ("domain.com", copied.authority());
    EXPECT_EQ("/index.html", copied.path());
    EXPECT_EQ("https://domain.com/index.html", copied.get());
  }
  {
    requests::uri_string copied;
    copied = uri; // copy assign
    EXPECT_EQ("https", copied.scheme());
    EXPECT_EQ("domain.com", copied.authority());
    EXPECT_EQ("/index.html", copied.path());
    EXPECT_EQ("https://domain.com/index.html", copied.get());
  }
}

TEST(uri_string, move)
{
  {
    auto uri = requests::uri_string::parse("https://domain.com/index.html");
    ASSERT_EQ("https://domain.com/index.html", uri.get());
    requests::uri_string copied{std::move(uri)}; // move construct
    EXPECT_EQ("https", copied.scheme());
    EXPECT_EQ("domain.com", copied.authority());
    EXPECT_EQ("/index.html", copied.path());
    EXPECT_EQ("https://domain.com/index.html", copied.get());
  }
  {
    auto uri = requests::uri_string::parse("https://domain.com/index.html");
    ASSERT_EQ("https://domain.com/index.html", uri.get());
    requests::uri_string copied;
    EXPECT_EQ("", copied.get());
    copied = std::move(uri); // move assign
    EXPECT_EQ("https", copied.scheme());
    EXPECT_EQ("domain.com", copied.authority());
    EXPECT_EQ("/index.html", copied.path());
    EXPECT_EQ("https://domain.com/index.html", copied.get());
  }
}
