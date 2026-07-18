# Contributing

Vacuo welcomes focused changes that preserve its conservative cleanup boundary.

## Before coding

1. Search existing issues and open a proposal for a new cleanup category or privileged action.
2. State which distributions, paths and application versions were verified.
3. Document what the rule removes, what it preserves, how data is rebuilt, and the worst failure mode.

## Build and test

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DVACUO_WARNINGS_AS_ERRORS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Core-only work can use `./tests/run-core-tests.sh` without Qt or CMake.

Format C++ with the repository `.clang-format`. Keep the core independent of Qt; conversion to Qt types belongs in `src/gui`.

## Cleanup-rule checklist

- Use an exact, deterministic boundary; never accept an unchecked external path.
- Do not follow symlinks or recursively delete through a pathname API.
- Do not target settings, credentials, projects, downloads or other durable data.
- Keep the rule disabled by default when cleanup can trigger a large redownload, recompile or irreversible user-visible change.
- Add negative tests for exclusions, symlinks, broad paths and unknown distributions.
- A privileged action must be a fixed identifier with fixed absolute executables and arguments.
- Do not invoke a shell, `sudo`, wildcard expansion, `eval`, or a package-removal transaction.
- Update both READMEs, the security model and changelog when behavior changes.

## Pull requests

- Keep one logical change per pull request.
- Explain the threat model and verification, not just the happy path.
- Add a `Signed-off-by` line using `git commit -s`. This certifies the [Developer Certificate of Origin](https://developercertificate.org/).
- All contributions are licensed under GPL-3.0-or-later.

Maintainers can ask for distribution-specific evidence or decline a rule whose safe boundary cannot be defined precisely.
