# Repository Guidelines

## Project Structure & Module Organization

VIPA is a header-only C++23 library for SIMD-assisted IPv4/IPv6 text parsing.
Public headers live in `vipa/`; keep parser APIs there and put private
implementation helpers under `vipa/detail/`. Unit tests live in `tests/` and
are built into the single `vipa_tests` GoogleTest executable. CMake package
support is under `cmake/`. Doxygen configuration and documentation helpers live
in `docs/`.

## Build, Test, and Development Commands

Configure a debug build with tests:

```bash
cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
```

Build everything configured in `build/`:

```bash
cmake --build build
```

Run all tests through CTest:

```bash
ctest --test-dir build -V
```

Run one GoogleTest case directly:

```bash
build/tests/vipa_tests --gtest_filter=TestSuite.TestName
```

Build API docs when Doxygen is available:

```bash
cmake -S . -B build -DBUILD_DOCS=ON
cmake --build build --target docs
```

## Coding Style & Naming Conventions

Use the repository `.clang-format` for C++ formatting: two-space indentation,
attached braces, and an 80-column limit. Keep files ASCII unless existing text
requires otherwise. Header files use `.hxx`; tests use `test_*.cxx`. Put public
API in namespace `llmx::vipa`. Keep SIMD-specific code isolated behind
portable helper functions with scalar fallbacks. Parser functions should be
allocation-free, bounds-aware, and explicit about accepted textual forms.

## Testing Guidelines

Tests use GoogleTest via CMake `FetchContent`. Add new test files to
`tests/CMakeLists.txt`. Prefer focused tests for successful IPv4/IPv6 parses,
truncated or malformed input, boundary values, compressed IPv6 forms, and
IPv4-embedded IPv6 forms. Follow the existing naming pattern:
`TEST(ComponentTest, ScenarioName)`.

## Commit & Pull Request Guidelines

Use short conventional-style subjects such as `feat: ...` or `fix: ...`; keep
commit messages imperative and scoped. Pull requests should describe the
behavior change, list test commands run, link related issues, and mention API or
accepted-input changes.

## Agent-Specific Instructions

Do not run Git commands that modify history, the index, or branches unless the
user explicitly requests them. Before changing parser behavior, add or update
tests for valid input, invalid input, and boundary cases.
