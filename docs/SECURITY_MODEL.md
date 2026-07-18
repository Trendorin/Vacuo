# Security model

Vacuo operates on attacker-influenced directory trees and occasionally requests root actions. Its design assumes cache names, symlinks, mount points, permissions and application behavior can change between scan and cleanup.

## Protected assets

- user configuration and durable application data;
- files outside the selected cleanup boundary;
- the home directory and system root as a whole;
- integrity of privileged command execution;
- predictable, reviewable cleanup behavior.

## Trust boundaries

| Boundary | Untrusted input | Control |
|---|---|---|
| System detection | `os-release`, environment, `mountinfo` | Parsing is data-only; an unknown family disables package-cache actions. |
| Cache scan | names, types, permissions, symlinks | Read-only traversal; no symlink following; entry cap and cancellation. |
| User cleanup | mutable directory tree | Exact compiled roots, protected-path deny-list, owner check and descriptor-relative deletion. |
| GUI to helper | selected system category | Fixed action IDs only; no user path, executable, option or environment is accepted. |
| Helper to package tools | operating-system state | Absolute executable allow-list, fixed arguments, sanitized environment, no shell and timeout. |

## User cleanup

Before opening a boundary, `PathGuard` verifies that it is absolute, exactly present in the active rule catalog, located below the detected home, not a protected broad directory, owned by the current user, and free of symlinked path components.

The boundary is opened with `O_DIRECTORY | O_NOFOLLOW`. Descendants are inspected with `fstatat(..., AT_SYMLINK_NOFOLLOW)`, directories are opened with `openat(..., O_NOFOLLOW)`, and entries are removed with `unlinkat`. A symlink is unlinked as a symlink; its target is never traversed. The boundary directory itself is retained.

This descriptor-relative design reduces path-rebinding and time-of-check/time-of-use attacks. It does not make concurrent application writes transactional: applications should be closed before their caches are cleaned.

## Privileged helper

`vacuo-helper`:

- must be started as root by PolicyKit;
- is installed as root-owned mode `0755`, without setuid or setgid;
- accepts only `package-cache`, `system-journal`, `crash-dumps`, and `snap-cache`;
- rejects every other option or positional argument;
- takes an exclusive lock in `/run`;
- clears the environment and sets a constant `PATH`/locale;
- launches no shell and never concatenates a command string;
- uses fixed absolute executable paths and fixed arguments;
- refuses unsafe package-manager fallbacks.

The package-cache command is selected only after classification through `ID` and `ID_LIKE`. Unsupported and unknown distributions have no privileged package-cache rule.

## Deliberate exclusions

Vacuo does not remove packages, orphan dependencies, old kernels, Btrfs/ZFS snapshots, configuration, browser profiles, logs outside the fixed journal policy, downloads, projects, save games or arbitrary user-supplied paths. Those operations have different ownership and recovery semantics and do not belong in a cache cleaner.

## Remaining risks

- A compromised process running as the same user can create new files while cleanup is in progress.
- Deleted blocks can remain recoverable depending on the filesystem, storage device, snapshots and backups.
- Package-manager and journal tools remain part of the operating-system trusted computing base.
- A distribution may change command behavior; adapters require review and regression testing when this happens.

Report security problems through the process in [SECURITY.md](../SECURITY.md).
