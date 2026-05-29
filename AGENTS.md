# Agent Entry Point

Keep this file small. It is a map, not the rulebook.

Read these files before making changes:

- `docs/harness.md` for the agentic coding harness and workflow.
- `docs/coding-rules.md` for VIPA C++ rules, tests, and verification commands.

Key anchors:

- VIPA is a header-only C++23 IPv4/IPv6 parser.
- Public API namespace: `llmx::vipa`.
- CMake target: `vipa::vipa`.
- Public headers: `vipa/`; private helpers: `vipa/detail/`; tests: `tests/`.

Do not run Git commands that modify history, the index, or branches unless the
user explicitly requests them.
