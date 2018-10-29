#define REQUESTS_USE_BOOST_SYSTEM_ERROR
#include "requests/error.hpp"
#include <gtest/gtest.h>

TEST(http_error, errors)
{
  boost::system::error_code ec = requests::http::error::ok;
  EXPECT_EQ(ec, requests::http::error::ok);
  EXPECT_EQ(requests::http::error::ok, ec);
  EXPECT_NE(ec, requests::http::error::not_found);
  EXPECT_NE(requests::http::error::not_found, ec);

  // generic category
  EXPECT_EQ(requests::http::errc::successful, ec);
  EXPECT_EQ(ec, requests::http::errc::successful);
  EXPECT_NE(requests::http::errc::informational, ec);
  EXPECT_NE(ec, requests::http::errc::informational);
}
