# VIPA Harness Design

This repository treats the harness as the system that lets coding agents make
reliable changes with limited human attention. The harness is not a prompt blob.
It is the combination of repository-local documentation, build commands,
mechanical checks, review loops, and cleanup rules that make the codebase
legible to agents.

## Source Receipts

The design is based on these agentic coding receipts:

- OpenAI, "Harness engineering: leveraging Codex in an agent-first world",
  February 11, 2026:
  <https://openai.com/index/harness-engineering/>
- Yanli Liu, "Harness Engineering: What Every AI Engineer Needs to Know in
  2026", April 17, 2026:
  <https://ai.gopubby.com/harness-engineering-what-every-ai-engineer-needs-to-know-in-2026-0ab649e5686a>

The OpenAI article is the primary source for the repo-local system-of-record
pattern: keep `AGENTS.md` short, put deeper knowledge in `docs/`, enforce
architecture mechanically, and improve agent legibility through local tools and
feedback loops. The second article's accessible preview reinforces the same
definition of harness engineering: humans design the environment, constraints,
feedback loops, documentation structure, and dependency rules while agents
execute code changes. It also warns that harness parts can become dead weight as
models improve, so this design should stay small and periodically pruned.

## Operating Model

Humans define intent, acceptance criteria, and constraints. Agents inspect the
current tree, make surgical changes, run the repo's verification commands, and
update documentation when the harness itself is missing guidance.

For VIPA, the current harness is intentionally small:

- `AGENTS.md` is a table of contents, not the source of detailed rules.
- `docs/harness.md` defines the operating model.
- `docs/coding-rules.md` defines coding, testing, and review rules.
- CMake, GoogleTest, and `.clang-format` are the mechanical checks.

## Repository Knowledge System

Use repository-local files as the source of truth. Do not rely on chat memory,
external notes, or unstated conventions when the rule can be recorded here.

Keep knowledge split by purpose:

- `AGENTS.md`: short entry point for agents.
- `docs/harness.md`: why the harness exists and how to operate it.
- `docs/coding-rules.md`: concrete coding, testing, and review rules.
- `README.md`: user-facing project overview and basic usage.

If an agent repeatedly makes the same mistake, do not only add prose. Prefer a
test, lint, script, CMake check, or small doc rule with a specific verification
command.

## Agent Workflow

For non-trivial changes:

1. Inspect the current tree before relying on assumptions.
2. State any material ambiguity or assumption before editing.
3. Make the smallest change that satisfies the requested outcome.
4. Update or add tests for behavior changes.
5. Run the narrowest useful verification first, then the broader check when the
   change affects public behavior.
6. Report exact commands run and any checks that could not be run.

For bug fixes:

1. Reproduce the failure or identify the missing regression coverage.
2. Add or update a focused test when practical.
3. Implement the fix.
4. Re-run the focused test and relevant suite.

For documentation-only changes:

1. Verify that links and referenced files exist.
2. Keep `AGENTS.md` concise.
3. Put detailed rules in `docs/`.

## Feedback Loops

The default local feedback loop is:

```bash
cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build -V
```

Use direct test execution for focused iteration:

```bash
build/tests/vipa_tests --gtest_filter=TestSuite.TestName
```

Use formatting as a mechanical taste check:

```bash
clang-format -i vipa/*.hxx vipa/detail/*.hxx tests/*.cxx
```

When the sandbox or local environment blocks the normal command, document the
blocker and use an equivalent command only when it proves the same requirement.
For example, using `-DCMAKE_CXX_COMPILER=/usr/bin/c++` is acceptable when the
default compiler wrapper cannot write its cache.

## Mechanical Invariants

VIPA should encode important rules where agents can verify them:

- Public API stays under `llmx::vipa`.
- Public headers live under `vipa/`.
- Private helpers live under `vipa/detail/`.
- The library remains header-only and exposed as `vipa::vipa`.
- Parser behavior changes require tests for valid input, invalid input, and
  boundary cases.
- SIMD code must have a portable scalar fallback.
- Parser functions should avoid allocation and should not read past the input
  span/string view.

Promote repeated review feedback into tests, compile checks, scripts, or docs.
Do not let one-off comments become scattered tribal knowledge.

## Entropy Control

Agent throughput can replicate existing bad patterns. Keep the harness clean:

- Remove stale documentation when code behavior changes.
- Prefer one shared helper over repeated hand-rolled parsing logic.
- Prune harness rules that no longer affect outcomes.
- Keep `AGENTS.md` small enough to be read every run.
- When a rule becomes critical, enforce it mechanically instead of relying only
  on prose.
