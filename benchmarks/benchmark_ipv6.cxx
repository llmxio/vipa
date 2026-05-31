#include <arpa/inet.h>
#include <array>
#include <benchmark/benchmark.h>
#include <string_view>

#include "vipa/ipv6.hxx"

namespace {

constexpr std::array<std::string_view, 8> kIPv6Inputs{
    "::",
    "::1",
    "2001:db8::1",
    "2001:db8::ff00:42:8329",
    "::ffff:192.0.2.128",
    "fe80::1234:5678:9abc:def0",
    "2001:0db8:0000:0000:0000:ff00:0042:8329",
    "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"};

} // namespace

static void BM_ParseIPv6(benchmark::State& state) {
  std::size_t index = 0;
  for (auto _ : state) {
    const auto text = kIPv6Inputs[index++ & (kIPv6Inputs.size() - 1u)];
    benchmark::DoNotOptimize(text.data());
    auto result = llmx::vipa::parse_ipv6(text);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_ParseIPv6);

static void BM_InetPtonIPv6(benchmark::State& state) {
  std::size_t index = 0;
  struct in6_addr addr{};
  for (auto _ : state) {
    const auto text = kIPv6Inputs[index++ & (kIPv6Inputs.size() - 1u)];
    benchmark::DoNotOptimize(text.data());
    benchmark::DoNotOptimize(inet_pton(AF_INET6, text.data(), &addr));
    benchmark::DoNotOptimize(addr);
  }
}
BENCHMARK(BM_InetPtonIPv6);

BENCHMARK_MAIN();
