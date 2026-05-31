#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <optional>
#include <string_view>

#include "address.hxx"
#include "detail/classify.hxx"
#include "ipv4.hxx"

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

struct IPv6Scan {
  uint64_t colon_mask{};
  uint64_t dot_mask{};
};

inline constexpr auto low_mask64(std::size_t len) noexcept -> uint64_t {
  return len == 64 ? 0xFFFF'FFFF'FFFF'FFFFull : ((uint64_t{1} << len) - 1u);
}

inline constexpr auto low_mask32(std::size_t len) noexcept -> uint32_t {
  return len == 32 ? 0xFFFF'FFFFu : ((uint32_t{1} << len) - 1u);
}

inline constexpr auto range_mask64(std::size_t begin, std::size_t end) noexcept
    -> uint64_t {
  if (begin >= end)
    return 0;
  return low_mask64(end) & ~low_mask64(begin);
}

#if VIPA_USE_AVX2
inline auto classify_ipv6_avx2(__m256i chunk, uint32_t text_mask,
                               uint32_t& colon_mask,
                               uint32_t& dot_mask) noexcept -> bool;

inline auto scan_ipv6_half_avx2(const char* bytes, uint32_t text_mask,
                                uint32_t& colon_mask,
                                uint32_t& dot_mask) noexcept -> bool {
  const auto chunk =
      _mm256_loadu_si256(reinterpret_cast<const __m256i*>(bytes));
  return classify_ipv6_avx2(chunk, text_mask, colon_mask, dot_mask);
}

inline auto classify_ipv6_avx2(__m256i chunk, uint32_t text_mask,
                               uint32_t& colon_mask,
                               uint32_t& dot_mask) noexcept -> bool {
  const auto colon = _mm256_set1_epi8(':');
  const auto dot = _mm256_set1_epi8('.');
  const auto slash = _mm256_set1_epi8(static_cast<char>('0' - 1));
  const auto colon_after_nine = _mm256_set1_epi8(static_cast<char>('9' + 1));
  const auto before_upper_a = _mm256_set1_epi8(static_cast<char>('A' - 1));
  const auto after_upper_f = _mm256_set1_epi8(static_cast<char>('F' + 1));
  const auto before_lower_a = _mm256_set1_epi8(static_cast<char>('a' - 1));
  const auto after_lower_f = _mm256_set1_epi8(static_cast<char>('f' + 1));

  const auto is_colon = _mm256_cmpeq_epi8(chunk, colon);
  const auto is_dot = _mm256_cmpeq_epi8(chunk, dot);
  const auto is_digit =
      _mm256_and_si256(_mm256_cmpgt_epi8(chunk, slash),
                       _mm256_cmpgt_epi8(colon_after_nine, chunk));
  const auto is_upper =
      _mm256_and_si256(_mm256_cmpgt_epi8(chunk, before_upper_a),
                       _mm256_cmpgt_epi8(after_upper_f, chunk));
  const auto is_lower =
      _mm256_and_si256(_mm256_cmpgt_epi8(chunk, before_lower_a),
                       _mm256_cmpgt_epi8(after_lower_f, chunk));
  const auto is_hex =
      _mm256_or_si256(is_digit, _mm256_or_si256(is_upper, is_lower));
  const auto valid = _mm256_or_si256(is_hex, _mm256_or_si256(is_colon, is_dot));

  colon_mask =
      static_cast<uint32_t>(_mm256_movemask_epi8(is_colon)) & text_mask;
  dot_mask = static_cast<uint32_t>(_mm256_movemask_epi8(is_dot)) & text_mask;
  const auto valid_mask =
      static_cast<uint32_t>(_mm256_movemask_epi8(valid)) & text_mask;
  return valid_mask == text_mask;
}

inline auto scan_ipv6_tail_scalar(const char* bytes, std::size_t begin,
                                  std::size_t end, std::size_t offset,
                                  IPv6Scan& scan) noexcept -> bool {
  for (std::size_t i = begin; i < end; ++i) {
    const char c = bytes[i];
    const auto bit = uint64_t{1} << (offset + i);
    if (c == ':') {
      scan.colon_mask |= bit;
      continue;
    }
    if (c == '.') {
      scan.dot_mask |= bit;
      continue;
    }
    if (!is_hex_digit(c))
      return false;
  }
  return true;
}

inline auto scan_ipv6_segment_avx2(const char* bytes, std::size_t len,
                                   std::size_t offset, IPv6Scan& scan) noexcept
    -> bool {
  const std::size_t full_words = len / 4u;
  const std::size_t full_bytes = full_words * 4u;
  if (full_words != 0) {
    const auto load_mask =
        _mm256_setr_epi32(full_words > 0 ? -1 : 0, full_words > 1 ? -1 : 0,
                          full_words > 2 ? -1 : 0, full_words > 3 ? -1 : 0,
                          full_words > 4 ? -1 : 0, full_words > 5 ? -1 : 0,
                          full_words > 6 ? -1 : 0, full_words > 7 ? -1 : 0);
    const auto chunk =
        _mm256_maskload_epi32(reinterpret_cast<const int*>(bytes), load_mask);

    uint32_t colon_mask = 0;
    uint32_t dot_mask = 0;
    const bool valid =
        classify_ipv6_avx2(chunk, low_mask32(full_bytes), colon_mask, dot_mask);
    scan.colon_mask |= static_cast<uint64_t>(colon_mask) << offset;
    scan.dot_mask |= static_cast<uint64_t>(dot_mask) << offset;
    if (!valid)
      return false;
  }

  return scan_ipv6_tail_scalar(bytes, full_bytes, len, offset, scan);
}

inline auto scan_ipv6_avx2(std::string_view text, IPv6Scan& scan) noexcept
    -> bool {
  uint32_t colon_lo = 0;
  uint32_t dot_lo = 0;
  if (text.size() >= 32) {
    if (!scan_ipv6_half_avx2(text.data(), 0xFFFF'FFFFu, colon_lo, dot_lo))
      return false;
    scan.colon_mask = colon_lo;
    scan.dot_mask = dot_lo;
    return scan_ipv6_segment_avx2(text.data() + 32, text.size() - 32, 32, scan);
  }

  return scan_ipv6_segment_avx2(text.data(), text.size(), 0, scan);
}
#endif

inline auto scan_ipv6_scalar(std::string_view text, IPv6Scan& scan) noexcept
    -> bool {
  for (std::size_t i = 0; i < text.size(); ++i) {
    const auto bit = uint64_t{1} << i;
    const char c = text[i];
    if (c == ':') {
      scan.colon_mask |= bit;
      continue;
    }
    if (c == '.') {
      scan.dot_mask |= bit;
      continue;
    }
    if (!is_hex_digit(c))
      return false;
  }
  return true;
}

inline auto scan_ipv6(std::string_view text, IPv6Scan& scan) noexcept -> bool {
#if VIPA_USE_AVX2
  return scan_ipv6_avx2(text, scan);
#else
  return scan_ipv6_scalar(text, scan);
#endif
}

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

inline auto find_next_colon(IPv6Scan scan, std::size_t begin,
                            std::size_t end) noexcept -> std::size_t {
  const auto mask = scan.colon_mask & range_mask64(begin, end);
  if (mask == 0)
    return end;
  return static_cast<std::size_t>(std::countr_zero(mask));
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

inline auto parse_ipv6_side(std::string_view text, std::size_t begin,
                            std::size_t end, IPv6Scan scan,
                            std::array<uint16_t, 8>& groups,
                            uint8_t& count) noexcept -> bool {
  while (begin < end) {
    const auto next = find_next_colon(scan, begin, end);
    const auto piece = text.substr(begin, next - begin);
    const bool contains_dot = (scan.dot_mask & range_mask64(begin, next)) != 0;
    if (contains_dot) {
      if (next != end)
        return false;
      return append_ipv4_tail(groups, count, piece);
    }

    uint16_t value = 0;
    if (!parse_ipv6_piece(piece, value) || count >= 8)
      return false;
    groups[count++] = value;

    if (next == end)
      return true;
    begin = next + 1;
    if (begin == end)
      return false;
  }
  return true;
}

} // namespace detail

inline auto parse_ipv6(std::string_view text) noexcept
    -> std::optional<IPv6Address> {
  if (text.empty() || text.size() > 45) {
    return std::nullopt;
  }

  detail::IPv6Scan scan{};
  if (!detail::scan_ipv6(text, scan))
    return std::nullopt;

  std::array<uint16_t, 8> head{};
  std::array<uint16_t, 8> tail{};
  uint8_t head_count = 0;
  uint8_t tail_count = 0;
  std::optional<std::size_t> compress_at{};

  const auto double_colon_mask = scan.colon_mask & (scan.colon_mask >> 1u);
  if (double_colon_mask != 0) {
    if (std::popcount(double_colon_mask) != 1)
      return std::nullopt;

    const auto double_colon =
        static_cast<std::size_t>(std::countr_zero(double_colon_mask));
    compress_at = double_colon;
    if ((double_colon != 0 &&
         !detail::parse_ipv6_side(text, 0, double_colon, scan, head,
                                  head_count)) ||
        (double_colon + 2 < text.size() &&
         !detail::parse_ipv6_side(text, double_colon + 2, text.size(), scan,
                                  tail, tail_count))) {
      return std::nullopt;
    }
    if (head_count + tail_count >= 8)
      return std::nullopt;
  } else {
    if (!detail::parse_ipv6_side(text, 0, text.size(), scan, head,
                                 head_count) ||
        head_count != 8)
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
