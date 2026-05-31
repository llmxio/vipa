#pragma once

#include <bit>
#include <cstdint>
#include <optional>
#include <string_view>

#include "address.hxx"

#if !defined(VIPA_USE_AVX2) && defined(__AVX2__)
#define VIPA_USE_AVX2 1
#endif

#if !defined(VIPA_USE_AVX2)
#define VIPA_USE_AVX2 0
#endif

#if VIPA_USE_AVX2
#include <immintrin.h>
#endif

namespace llmx::vipa {

namespace detail {

/** @brief Character-position masks collected while scanning IPv4 text. */
struct IPv4Scan {
  /** Bit set for each dot position in the input. */
  uint32_t dot_mask{};
};

#if VIPA_USE_AVX2
/**
 * @brief Scan IPv4 text for dot positions using AVX2 masked loads.
 *
 * The scanner only loads complete 32-bit words covered by the input and handles
 * the remaining tail scalarly, so it does not read past the string view.
 */
inline auto scan_ipv4_avx2(std::string_view text, IPv4Scan& scan) noexcept
    -> bool {
  const auto full_words = text.size() / 4u;
  const auto load_mask =
      _mm_setr_epi32(full_words > 0 ? -1 : 0, full_words > 1 ? -1 : 0,
                     full_words > 2 ? -1 : 0, 0);
  const auto chunk =
      _mm_maskload_epi32(reinterpret_cast<const int*>(text.data()), load_mask);
  const auto dots = _mm_cmpeq_epi8(chunk, _mm_set1_epi8('.'));

  const auto full_bytes = full_words * 4u;
  scan.dot_mask = static_cast<uint32_t>(_mm_movemask_epi8(dots)) &
                  ((uint32_t{1} << full_bytes) - 1u);
  for (std::size_t i = full_bytes; i < text.size(); ++i) {
    if (text[i] == '.')
      scan.dot_mask |= uint32_t{1} << i;
  }
  return true;
}
#endif

/** @brief Scan IPv4 text for dot positions with portable scalar code. */
inline auto scan_ipv4_scalar(std::string_view text, IPv4Scan& scan) noexcept
    -> bool {
  for (std::size_t i = 0; i < text.size(); ++i) {
    const auto bit = uint32_t{1} << i;
    const char c = text[i];
    if (c == '.') {
      scan.dot_mask |= bit;
      continue;
    }
  }
  return true;
}

/** @brief Scan IPv4 text with the best enabled implementation. */
inline auto scan_ipv4(std::string_view text, IPv4Scan& scan) noexcept -> bool {
#if VIPA_USE_AVX2
  return scan_ipv4_avx2(text, scan);
#else
  return scan_ipv4_scalar(text, scan);
#endif
}

/**
 * @brief Parse IPv4 bytes after dot positions have been collected.
 *
 * Rejects empty octets, octets longer than three digits, leading-zero octets,
 * non-digit characters, and values above 255.
 */
inline auto parse_ipv4_from_scan(std::string_view text, IPv4Scan scan) noexcept
    -> std::optional<IPv4Address> {
  if (std::popcount(scan.dot_mask) != 3) {
    return std::nullopt;
  }

  IPv4Address out{};
  std::size_t begin = 0;
  uint8_t part = 0;
  uint32_t remaining_dots = scan.dot_mask;

  while (part < 4) {
    const std::size_t end =
        remaining_dots == 0 ? text.size() : std::countr_zero(remaining_dots);
    const std::size_t digits = end - begin;
    if (digits == 0 || digits > 3)
      return std::nullopt;
    if (digits > 1 && text[begin] == '0')
      return std::nullopt;

    uint32_t value = 0;
    for (std::size_t i = begin; i < end; ++i) {
      const char c = text[i];
      if (c < '0' || c > '9')
        return std::nullopt;
      value = value * 10u + static_cast<uint32_t>(c - '0');
    }
    if (value > 255u)
      return std::nullopt;

    out.bytes[part] = static_cast<uint8_t>(value);
    ++part;
    if (remaining_dots == 0)
      break;

    remaining_dots &= remaining_dots - 1u;
    begin = end + 1;
  }

  if (part != 4)
    return std::nullopt;
  return out;
}

} // namespace detail

/**
 * @brief Parse strict dotted-decimal IPv4 text.
 *
 * Accepted addresses contain exactly four decimal octets separated by dots.
 * Leading-zero multi-digit octets and values above 255 are rejected.
 *
 * @param text IPv4 address text.
 * @return Parsed address bytes, or `std::nullopt` for malformed input.
 */
inline auto parse_ipv4(std::string_view text) noexcept
    -> std::optional<IPv4Address> {
  if (text.empty() || text.size() > 15) {
    return std::nullopt;
  }

  detail::IPv4Scan scan{};
  if (!detail::scan_ipv4(text, scan))
    return std::nullopt;

  return detail::parse_ipv4_from_scan(text, scan);
}

/**
 * @brief Parse a fixed character array as IPv4 text.
 *
 * A trailing NUL byte is excluded from the parsed view when present.
 */
template <std::size_t N>
inline auto parse_ipv4(const char (&text)[N]) noexcept
    -> std::optional<IPv4Address> {
  static_assert(N > 0);
  const std::size_t length = text[N - 1] == '\0' ? N - 1 : N;
  return parse_ipv4(std::string_view{text, length});
}

} // namespace llmx::vipa
