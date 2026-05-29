#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include "address.hxx"
#include "detail/classify.hxx"

namespace llmx::vipa {

inline auto parse_ipv4(std::string_view text) noexcept
    -> std::optional<IPv4Address> {
  if (text.empty() || text.size() > 15 ||
      !detail::contains_only_ipv4_chars(text)) {
    return std::nullopt;
  }

  IPv4Address out{};
  uint32_t value = 0;
  uint8_t digits = 0;
  uint8_t part = 0;

  for (char c : text) {
    if (c == '.') {
      if (digits == 0 || part >= 3)
        return std::nullopt;
      out.bytes[part++] = static_cast<uint8_t>(value);
      value = 0;
      digits = 0;
      continue;
    }

    value = value * 10u + static_cast<uint32_t>(c - '0');
    ++digits;
    if (digits > 3 || value > 255u)
      return std::nullopt;
  }

  if (digits == 0 || part != 3)
    return std::nullopt;

  out.bytes[part] = static_cast<uint8_t>(value);
  return out;
}

} // namespace llmx::vipa
