#include <gtest/gtest.h>

#include "vipa/ipv4.hxx"

using namespace llmx::vipa;

TEST(IPv4ParserTest, ParsesDottedDecimal) {
  const auto parsed = parse_ipv4("192.168.0.1");
  ASSERT_TRUE(parsed);

  EXPECT_EQ(parsed->bytes[0], 192u);
  EXPECT_EQ(parsed->bytes[1], 168u);
  EXPECT_EQ(parsed->bytes[2], 0u);
  EXPECT_EQ(parsed->bytes[3], 1u);
}

TEST(IPv4ParserTest, RejectsMalformedInput) {
  EXPECT_FALSE(parse_ipv4(""));
  EXPECT_FALSE(parse_ipv4("192.168.0"));
  EXPECT_FALSE(parse_ipv4("192.168.0.1."));
  EXPECT_FALSE(parse_ipv4("192..0.1"));
  EXPECT_FALSE(parse_ipv4("256.0.0.1"));
  EXPECT_FALSE(parse_ipv4("1.2.3.999"));
  EXPECT_FALSE(parse_ipv4("1.2.3.a"));
}
