cmake_minimum_required(VERSION 3.16)

include(FetchContent)

FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.9.5
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(benchmark)

find_package(Threads REQUIRED)

add_executable(vipa_ipv4_benchmarks
  benchmark_ipv4.cxx
)

target_link_libraries(vipa_ipv4_benchmarks PRIVATE
  llmx::vipa
  benchmark::benchmark
  Threads::Threads
)

target_compile_features(vipa_ipv4_benchmarks PRIVATE cxx_std_23)

add_executable(vipa_ipv6_benchmarks
  benchmark_ipv6.cxx
)

target_link_libraries(vipa_ipv6_benchmarks PRIVATE
  llmx::vipa
  benchmark::benchmark
  Threads::Threads
)

target_compile_features(vipa_ipv6_benchmarks PRIVATE cxx_std_23)
