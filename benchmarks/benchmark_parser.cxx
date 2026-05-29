#include <arpa/inet.h>
#include <benchmark/benchmark.h>

#include "vipa/parser.hxx"

static void BM_ParseIPv4(benchmark::State& state) {
  constexpr char text[] = "192.168.0.1";
  for (auto _ : state) {
    auto result = llmx::vipa::parse_ipv4(text);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_ParseIPv4);

static void BM_InetPtonIPv4(benchmark::State& state) {
  constexpr char text[] = "192.168.0.1";
  struct in_addr addr{};
  for (auto _ : state) {
    benchmark::DoNotOptimize(inet_pton(AF_INET, text, &addr));
    benchmark::DoNotOptimize(addr);
  }
}
BENCHMARK(BM_InetPtonIPv4);

static void BM_ParseIPv6(benchmark::State& state) {
  constexpr char text[] = "2001:db8::ff00:42:8329";
  for (auto _ : state) {
    auto result = llmx::vipa::parse_ipv6(text);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_ParseIPv6);

static void BM_InetPtonIPv6(benchmark::State& state) {
  constexpr char text[] = "2001:db8::ff00:42:8329";
  struct in6_addr addr{};
  for (auto _ : state) {
    benchmark::DoNotOptimize(inet_pton(AF_INET6, text, &addr));
    benchmark::DoNotOptimize(addr);
  }
}
BENCHMARK(BM_InetPtonIPv6);

BENCHMARK_MAIN();
