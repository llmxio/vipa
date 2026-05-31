#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

#if defined(__SSE2__)
#include <emmintrin.h>
#endif

namespace llmx::vipa::detail {

/** @brief Return true when `c` is an ASCII decimal digit. */
constexpr bool is_dec_digit(char c) noexcept { return c >= '0' && c <= '9'; }

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

/** @brief Return true when text contains only IPv4 digits and dots. */
inline bool contains_only_ipv4_chars(std::string_view text) noexcept {
#if defined(__SSE2__)
  const auto dot = _mm_set1_epi8('.');
  const auto zero = _mm_set1_epi8(static_cast<char>('0' - 1));
  const auto nine = _mm_set1_epi8(static_cast<char>('9' + 1));

  std::size_t i = 0;
  for (; i + 16 <= text.size(); i += 16) {
    const auto chunk =
        _mm_loadu_si128(reinterpret_cast<const __m128i*>(text.data() + i));
    const auto is_dot = _mm_cmpeq_epi8(chunk, dot);
    const auto gt_zero = _mm_cmpgt_epi8(chunk, zero);
    const auto lt_nine = _mm_cmplt_epi8(chunk, nine);
    const auto is_digit = _mm_and_si128(gt_zero, lt_nine);
    const auto valid = _mm_or_si128(is_dot, is_digit);
    if (_mm_movemask_epi8(valid) != 0xFFFF)
      return false;
  }

  for (; i < text.size(); ++i) {
    if (!is_dec_digit(text[i]) && text[i] != '.')
      return false;
  }
  return true;
#else
  for (char c : text) {
    if (!is_dec_digit(c) && c != '.')
      return false;
  }
  return true;
#endif
}

/** @brief Return true when text contains only IPv6 hex digits, colons, and
 * dots. */
inline bool contains_only_ipv6_chars(std::string_view text) noexcept {
  for (char c : text) {
    if (!is_hex_digit(c) && c != ':' && c != '.')
      return false;
  }
  return true;
}

} // namespace llmx::vipa::detail
