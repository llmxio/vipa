#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

#include "address.hxx"
#include "detail/classify.hxx"
#include "ipv4.hxx"

namespace llmx::vipa {

namespace detail {

inline bool parse_ipv6_piece(std::string_view piece, uint16_t& value) noexcept {
  if (piece.empty() || piece.size() > 4)
    return false;

  uint16_t parsed = 0;
  for (char c : piece) {
    if (!is_hex_digit(c))
      return false;
    parsed = static_cast<uint16_t>((parsed << 4u) | hex_value(c));
  }

  value = parsed;
  return true;
}

inline auto append_ipv4_tail(std::array<uint16_t, 8>& groups, uint8_t& count,
                             std::string_view tail) noexcept -> bool {
  const auto ipv4 = parse_ipv4(tail);
  if (!ipv4 || count > 6)
    return false;

  groups[count++] =
      static_cast<uint16_t>((ipv4->bytes[0] << 8u) | ipv4->bytes[1]);
  groups[count++] =
      static_cast<uint16_t>((ipv4->bytes[2] << 8u) | ipv4->bytes[3]);
  return true;
}

} // namespace detail

inline auto parse_ipv6(std::string_view text) noexcept
    -> std::optional<IPv6Address> {
  if (text.empty() || text.size() > 45 ||
      !detail::contains_only_ipv6_chars(text)) {
    return std::nullopt;
  }

  std::array<uint16_t, 8> head{};
  std::array<uint16_t, 8> tail{};
  uint8_t head_count = 0;
  uint8_t tail_count = 0;
  std::optional<std::size_t> compress_at{};

  auto parse_side = [](std::string_view side, std::array<uint16_t, 8>& groups,
                       uint8_t& count) noexcept -> bool {
    while (!side.empty()) {
      const auto pos = side.find(':');
      const auto piece = side.substr(0, pos);
      if (piece.find('.') != std::string_view::npos) {
        if (pos != std::string_view::npos)
          return false;
        return detail::append_ipv4_tail(groups, count, piece);
      }

      uint16_t value = 0;
      if (!detail::parse_ipv6_piece(piece, value) || count >= 8)
        return false;
      groups[count++] = value;

      if (pos == std::string_view::npos)
        return true;
      side.remove_prefix(pos + 1);
      if (side.empty())
        return false;
    }
    return true;
  };

  const auto double_colon = text.find("::");
  if (double_colon != std::string_view::npos) {
    if (text.find("::", double_colon + 2) != std::string_view::npos)
      return std::nullopt;

    compress_at = double_colon;
    const auto left = text.substr(0, double_colon);
    const auto right = text.substr(double_colon + 2);
    if ((!left.empty() && !parse_side(left, head, head_count)) ||
        (!right.empty() && !parse_side(right, tail, tail_count))) {
      return std::nullopt;
    }
    if (head_count + tail_count >= 8)
      return std::nullopt;
  } else {
    if (!parse_side(text, head, head_count) || head_count != 8)
      return std::nullopt;
  }

  std::array<uint16_t, 8> groups{};
  uint8_t out = 0;
  for (uint8_t i = 0; i < head_count; ++i)
    groups[out++] = head[i];

  if (compress_at) {
    const uint8_t zeros = static_cast<uint8_t>(8 - head_count - tail_count);
    out = static_cast<uint8_t>(out + zeros);
    for (uint8_t i = 0; i < tail_count; ++i)
      groups[out++] = tail[i];
  }

  IPv6Address result{};
  for (std::size_t i = 0; i < groups.size(); ++i) {
    result.bytes[i * 2] = static_cast<uint8_t>(groups[i] >> 8u);
    result.bytes[i * 2 + 1] = static_cast<uint8_t>(groups[i] & 0xFFu);
  }

  return result;
}

} // namespace llmx::vipa
