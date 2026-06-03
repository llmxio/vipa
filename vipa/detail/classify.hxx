#pragma once

#include <cstdint>

namespace llmx::vipa::detail {

/** @brief Return true when `c` is an ASCII hexadecimal digit. */
constexpr bool is_hex_digit(char c) noexcept {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

/** @brief Convert an ASCII hexadecimal digit to its numeric value. */
constexpr auto hex_value(char c) noexcept -> uint8_t {
  if (c >= '0' && c <= '9')
    return static_cast<uint8_t>(c - '0');
  if (c >= 'a' && c <= 'f')
    return static_cast<uint8_t>(10 + c - 'a');
  return static_cast<uint8_t>(10 + c - 'A');
}

} // namespace llmx::vipa::detail
