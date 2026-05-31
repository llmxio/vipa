<!-- markdownlint-disable MD041 -->

# VIPA - Vectorized IP Address Parser

VIPA is a small, header-only C++23 library for SIMD-assisted IPv4/IPv6 text
parsing. It exposes allocation-free parser functions under the `llmx::vipa`
namespace and ships as an installable CMake interface target.

## Features

- Header-only C++23 library
- Strict IPv4 dotted-decimal parser
- IPv6 parser with compressed forms and IPv4-embedded tails
- AVX2-assisted parser scans with portable scalar fallbacks
- Portable scalar fallback for other targets
- GoogleTest-based parser tests

AVX2 compile options are enabled by default through the `llmx::vipa` CMake
target. Configure with `-DVIPA_ENABLE_AVX2=OFF` to force the scalar fallback.

## Using VIPA from CMake

### Via `add_subdirectory`

```cmake
add_subdirectory(path/to/vipa)
target_link_libraries(my_app PRIVATE llmx::vipa)
```

### Via `FetchContent`

```cmake
include(FetchContent)

FetchContent_Declare(vipa
  GIT_REPOSITORY https://github.com/llmxio/vipa.git
  GIT_TAG        vX.Y.Z
)
FetchContent_MakeAvailable(vipa)

target_link_libraries(my_app PRIVATE llmx::vipa)
```

Installed packages can be consumed with:

```cmake
find_package(VIPA CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE llmx::vipa)
```

## Build And Test

```bash
cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build -V
```

## Build And Benchmark

```bash
cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKING=ON
cmake --build build --target vipa_benchmarks
./build/vipa_benchmarks
```

## Usage

```cpp
#include "vipa/parser.hxx"

auto parsed = llmx::vipa::parse_address("2001:db8::1");
if (parsed && parsed->family == llmx::vipa::AddressFamily::IPv6) {
  // Use parsed->address.
}
```

## Documentation

Build the HTML API docs using Doxygen:

```bash
cmake -S . -B build -DBUILD_DOCS=ON
cmake --build build --target docs
```

HTML output is written to `build/docs/html`.
