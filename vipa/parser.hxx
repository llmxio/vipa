#pragma once

#include <optional>
#include <string_view>

#include "address.hxx"
#include "ipv4.hxx"
#include "ipv6.hxx"

namespace llmx::vipa {

/**
 * @brief Parse an IPv4 or IPv6 text address.
 *
 * IPv4 parsing is attempted first. On success, the returned family identifies
 * which variant member is stored in the result.
 *
 * @param text Address text to parse.
 * @return Parsed address and family, or `std::nullopt` for malformed input.
 */
[[nodiscard]] inline auto parse_address(std::string_view text) noexcept
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
