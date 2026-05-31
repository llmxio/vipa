#include <gtest/gtest.h>

#include "vipa/ipv6.hxx"

using namespace llmx::vipa;

TEST(IPv6ParserTest, ParsesFullAddress) {
  const auto parsed = parse_ipv6("2001:0db8:0000:0000:0000:ff00:0042:8329");
  ASSERT_TRUE(parsed);

  EXPECT_EQ(parsed->bytes[0], 0x20u);
  EXPECT_EQ(parsed->bytes[1], 0x01u);
  EXPECT_EQ(parsed->bytes[14], 0x83u);
  EXPECT_EQ(parsed->bytes[15], 0x29u);
}

TEST(IPv6ParserTest, ParsesCompressedAddress) {
  const auto parsed = parse_ipv6("2001:db8::ff00:42:8329");
  ASSERT_TRUE(parsed);

  EXPECT_EQ(parsed->bytes[0], 0x20u);
  EXPECT_EQ(parsed->bytes[1], 0x01u);
  EXPECT_EQ(parsed->bytes[4], 0x00u);
  EXPECT_EQ(parsed->bytes[5], 0x00u);
  EXPECT_EQ(parsed->bytes[10], 0xffu);
  EXPECT_EQ(parsed->bytes[11], 0x00u);
}

TEST(IPv6ParserTest, ParsesIpv4MappedAddress) {
  const auto parsed = parse_ipv6("::ffff:192.0.2.128");
  ASSERT_TRUE(parsed);

  EXPECT_EQ(parsed->bytes[10], 0xffu);
  EXPECT_EQ(parsed->bytes[11], 0xffu);
  EXPECT_EQ(parsed->bytes[12], 192u);
  EXPECT_EQ(parsed->bytes[13], 0u);
  EXPECT_EQ(parsed->bytes[14], 2u);
  EXPECT_EQ(parsed->bytes[15], 128u);
}

TEST(IPv6ParserTest, ParsesBoundaryCompressedAddresses) {
  const auto loopback = parse_ipv6("::1");
  ASSERT_TRUE(loopback);
  EXPECT_EQ(loopback->bytes[15], 1u);

  const auto unspecified = parse_ipv6("::");
  ASSERT_TRUE(unspecified);
  for (const auto byte : unspecified->bytes) {
    EXPECT_EQ(byte, 0u);
  }

  const auto trailing = parse_ipv6("2001:db8::");
  ASSERT_TRUE(trailing);
  EXPECT_EQ(trailing->bytes[0], 0x20u);
  EXPECT_EQ(trailing->bytes[1], 0x01u);
  EXPECT_EQ(trailing->bytes[2], 0x0du);
  EXPECT_EQ(trailing->bytes[3], 0xb8u);
}

TEST(IPv6ParserTest, RejectsMalformedInput) {
  EXPECT_FALSE(parse_ipv6(""));
  EXPECT_FALSE(parse_ipv6("2001:::1"));
  EXPECT_FALSE(parse_ipv6("2001:db8::1::1"));
  EXPECT_FALSE(parse_ipv6("2001:db8:zzzz::1"));
  EXPECT_FALSE(parse_ipv6("1:2:3:4:5:6:7"));
  EXPECT_FALSE(parse_ipv6("1:2:3:4:5:6:7:8:9"));
  EXPECT_FALSE(parse_ipv6(":1:2:3:4:5:6:7"));
  EXPECT_FALSE(parse_ipv6("1:2:3:4:5:6:7:"));
}

TEST(IPv6ParserTest, RejectsLeadingZeroIpv4TailOctets) {
  EXPECT_FALSE(parse_ipv6("::ffff:192.168.001.1"));
  EXPECT_FALSE(parse_ipv6("2001:db8::1.2.03.4"));
}
