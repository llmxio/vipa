#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <variant>

namespace llmx::vipa {

enum class AddressFamily : uint8_t { IPv4, IPv6 };

struct IPv4Address {
  std::array<uint8_t, 4> bytes{};

  constexpr auto octets() const noexcept -> std::span<const uint8_t, 4> {
    return std::span<const uint8_t, 4>{bytes};
  }
};

struct IPv6Address {
  std::array<uint8_t, 16> bytes{};

  constexpr auto octets() const noexcept -> std::span<const uint8_t, 16> {
    return std::span<const uint8_t, 16>{bytes};
  }
};

using Address = std::variant<IPv4Address, IPv6Address>;

struct ParseResult {
  Address address{};
  AddressFamily family{};
};

} // namespace llmx::vipa
