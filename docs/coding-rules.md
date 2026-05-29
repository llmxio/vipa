# VIPA Coding Rules

## Project Shape

VIPA is a header-only C++23 library for SIMD-assisted IPv4/IPv6 text parsing.

- Public headers live in `vipa/`.
- Private implementation helpers live in `vipa/detail/`.
- Tests live in `tests/` and build into `vipa_tests`.
- CMake package support lives in `cmake/`.
- Doxygen configuration lives in `docs/`.

Use namespace `llmx::vipa` for public API. Keep the CMake target name
`vipa::vipa`.

## C++ Style

- Use C++23.
- Use `.hxx` for headers and `test_*.cxx` for tests.
- Follow `.clang-format`: two-space indentation, attached braces, 80-column
  limit.
- Keep files ASCII unless a file already requires non-ASCII.
- Keep parser APIs allocation-free unless the user explicitly requests a richer
  owning abstraction.
- Isolate SIMD-specific code behind small helper functions with scalar
  fallbacks.
- Avoid speculative abstractions. Add a helper only when it removes real
  duplication or clarifies an invariant.

## Parser Rules

- Treat accepted textual forms as part of the public contract.
- Add tests before or with changes to accepted/rejected input.
- Cover malformed input, boundary values, compressed IPv6, IPv4-embedded IPv6,
  and successful parse paths when touched.
- Do not guess data shape from probes. Validate boundaries and reject malformed
  input explicitly.
- Do not read past `std::string_view` bounds.
- Prefer fixed-size byte arrays for parsed address storage.

## Behavioral Rules

Think before coding:

- State material assumptions.
- Surface ambiguity instead of silently choosing.
- Ask when the request cannot be interpreted safely.
- Push back when a simpler solution fits better.

Prefer simplicity:

- No features beyond the request.
- No single-use abstraction.
- No configurability unless requested.
- Rewrite overcomplicated changes before handing them off.

Make surgical changes:

- Touch only files needed for the task.
- Match existing style.
- Do not refactor adjacent code unless it is required.
- Remove only unused code introduced by the current change.
- Mention unrelated dead code instead of deleting it.

Drive by verifiable goals:

- Convert requested behavior into concrete checks.
- For bug fixes, prefer a regression test that fails before the fix and passes
  after it.
- For refactors, keep behavior checks green before and after.
- Report the exact verification commands used.

## Verification Commands

Default full local verification:

```bash
cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build -V
```

Focused test:

```bash
build/tests/vipa_tests --gtest_filter=TestSuite.TestName
```

Docs:

```bash
cmake -S . -B build -DBUILD_DOCS=ON
cmake --build build --target docs
```

Formatting:

```bash
clang-format -i vipa/*.hxx vipa/detail/*.hxx tests/*.cxx
```
