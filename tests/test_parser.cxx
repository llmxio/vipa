#include <gtest/gtest.h>

#include "vipa/parser.hxx"

using namespace llmx::vipa;

TEST(AddressParserTest, DetectsAddressFamily) {
  const auto ipv4 = parse_address("127.0.0.1");
  ASSERT_TRUE(ipv4);
  EXPECT_EQ(ipv4->family, AddressFamily::IPv4);

  const auto ipv6 = parse_address("::1");
  ASSERT_TRUE(ipv6);
  EXPECT_EQ(ipv6->family, AddressFamily::IPv6);
}

TEST(AddressParserTest, RejectsUnknownAddress) {
  EXPECT_FALSE(parse_address("not-an-address"));
}
