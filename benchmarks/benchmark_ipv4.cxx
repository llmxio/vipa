#include <arpa/inet.h>
#include <array>
#include <benchmark/benchmark.h>
#include <string_view>

#include "vipa/ipv4.hxx"

namespace {

constexpr std::array<std::string_view, 8> kIPv4Inputs{
    "0.0.0.0",   "1.2.3.4",      "8.8.8.8",     "10.20.30.40",
    "127.0.0.1", "172.16.254.1", "192.168.0.1", "255.255.255.255"};

} // namespace

static void BM_ParseIPv4(benchmark::State& state) {
  std::size_t index = 0;
  for (auto _ : state) {
    const auto text = kIPv4Inputs[index++ & (kIPv4Inputs.size() - 1u)];
    benchmark::DoNotOptimize(text.data());
    auto result = llmx::vipa::parse_ipv4(text);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_ParseIPv4);

static void BM_InetPtonIPv4(benchmark::State& state) {
  std::size_t index = 0;
  struct in_addr addr{};
  for (auto _ : state) {
    const auto text = kIPv4Inputs[index++ & (kIPv4Inputs.size() - 1u)];
    benchmark::DoNotOptimize(text.data());
    benchmark::DoNotOptimize(inet_pton(AF_INET, text.data(), &addr));
    benchmark::DoNotOptimize(addr);
  }
}
BENCHMARK(BM_InetPtonIPv4);

BENCHMARK_MAIN();
