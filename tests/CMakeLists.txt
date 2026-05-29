cmake_minimum_required(VERSION 3.16)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(vipa_tests
  test_ipv4.cxx
  test_ipv6.cxx
  test_parser.cxx
)

target_link_libraries(vipa_tests PRIVATE llmx::vipa GTest::gtest_main)

enable_testing()
include(GoogleTest)

gtest_discover_tests(vipa_tests)
