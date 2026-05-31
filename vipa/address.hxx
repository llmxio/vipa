#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <variant>

namespace llmx::vipa {

/** @brief Parsed IP address family. */
enum class AddressFamily : uint8_t { IPv4, IPv6 };

/** @brief Parsed IPv4 address in network byte order. */
struct IPv4Address {
  /** Raw IPv4 octets, from most-significant to least-significant address byte.
   */
  std::array<uint8_t, 4> bytes{};

  /** @brief View the address as four read-only octets. */
  constexpr auto octets() const noexcept -> std::span<const uint8_t, 4> {
    return std::span<const uint8_t, 4>{bytes};
  }
};

/** @brief Parsed IPv6 address in network byte order. */
struct IPv6Address {
  /** Raw IPv6 octets, from most-significant to least-significant address byte.
   */
  std::array<uint8_t, 16> bytes{};

  /** @brief View the address as sixteen read-only octets. */
  constexpr auto octets() const noexcept -> std::span<const uint8_t, 16> {
    return std::span<const uint8_t, 16>{bytes};
  }
};

/** @brief Parsed IPv4 or IPv6 address value. */
using Address = std::variant<IPv4Address, IPv6Address>;

/** @brief Address parser result with the concrete address family. */
struct ParseResult {
  /** Parsed address payload. */
  Address address{};
  /** Family selected by the parser. */
  AddressFamily family{};
};

} // namespace llmx::vipa
