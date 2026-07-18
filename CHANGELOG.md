# Changelog

All notable changes to Vacuo are documented here. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and versions follow Semantic Versioning.

## [Unreleased]

### Planned

- Broader integration tests on distribution-native virtual machines.
- Optional translation catalogs after the user-facing vocabulary stabilizes.

## [0.1.3] - 2026-07-19

### Fixed

- Rebuilt the documentation and identity release from a corrected source tree; runtime cleanup behavior is unchanged.

## [0.1.2] - 2026-07-19

### Changed

- Replaced the application icon and compacted the project header around a consistent monochrome identity.
- Rebuilt the README around verified capabilities, package installation, source builds and uninstallation.
- Added complete Russian, Ukrainian and German README translations alongside the English default.
- Clarified release artifacts and integrity verification without changing cleanup categories or privilege boundaries.

## [0.1.1] - 2026-07-18

### Fixed

- Corrected localized AppStream description structure for strict metadata validation.
- Removed a Clang `-Wsign-conversion` failure in JSON escaping while preserving byte-safe output.

### Changed

- Updated checkout, CodeQL and build-provenance actions to their current major versions.

## [0.1.0] - 2026-07-18

### Added

- Native Qt 6 Widgets interface using the active system style and window frame.
- C++20 core for distribution, package manager, session and mount-filesystem detection.
- Read-only cache scan with per-category size, item count, risk and warnings.
- Safe user cleanup based on immutable paths and descriptor-relative, no-follow deletion.
- Fedora/RHEL, Arch, Debian/Ubuntu, openSUSE and Alpine package-cache adapters.
- Restricted PolicyKit helper for package cache, archived journal, crash dumps and Snap cache.
- `vacuoctl` scan and clean commands with versioned JSON output.
- DEB, RPM, source/TGZ and generated Arch packaging.
- CI on Ubuntu, Fedora and Arch, ASan/UBSan, CodeQL, checksums, SPDX SBOM and provenance.

[Unreleased]: https://github.com/Trendorin/Vacuo/compare/v0.1.3...HEAD
[0.1.3]: https://github.com/Trendorin/Vacuo/compare/v0.1.2...v0.1.3
[0.1.2]: https://github.com/Trendorin/Vacuo/compare/v0.1.1...v0.1.2
[0.1.1]: https://github.com/Trendorin/Vacuo/compare/v0.1.0...v0.1.1
[0.1.0]: https://github.com/Trendorin/Vacuo/releases/tag/v0.1.0
