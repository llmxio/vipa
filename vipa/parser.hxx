#pragma once

#include <optional>
#include <string_view>

#include "address.hxx"
#include "ipv4.hxx"
#include "ipv6.hxx"

namespace llmx::vipa {

inline auto parse_address(std::string_view text) noexcept
    -> std::optional<ParseResult> {
  if (auto ipv4 = parse_ipv4(text)) {
    return ParseResult{.address = *ipv4, .family = AddressFamily::IPv4};
  }
  if (auto ipv6 = parse_ipv6(text)) {
    return ParseResult{.address = *ipv6, .family = AddressFamily::IPv6};
  }
  return std::nullopt;
}

} // namespace llmx::vipa
