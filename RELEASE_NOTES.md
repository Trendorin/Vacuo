# Vacuo 0.1.5

Vacuo 0.1.5 adds a directly installable Arch Linux package and rebuilds each native package in its target distribution. Cleanup behavior and safety boundaries are unchanged.

## Included

| Asset | Target |
|---|---|
| `vacuo_0.1.5_amd64.deb` | Ubuntu 24.04 |
| `vacuo-0.1.5-1.x86_64.rpm` | Fedora 44 |
| `vacuo-0.1.5-1-x86_64.pkg.tar.zst` | Arch Linux |
| `PKGBUILD` | Arch source rebuild |
| `vacuo-0.1.5-x86_64.tar.gz` | Installable Linux tree |
| `vacuo-0.1.5-source.tar.gz` | Release source |
| `vacuo-0.1.5.spdx` | SPDX 2.3 SBOM |
| `SHA256SUMS` | Integrity manifest |

## Validation

- DEB: built and installed on Ubuntu 24.04.
- RPM: built and installed on Fedora 44 with an explicit PolicyKit dependency.
- Arch: clean-built as an unprivileged user and installed with `pacman -U`.
- Every package checks the CLI, ELF dependencies, PolicyKit helper path, root ownership and `0755` mode.

Install on Arch Linux:

```bash
sudo pacman -U ./vacuo-0.1.5-1-x86_64.pkg.tar.zst
```

For package-cache cleanup install `pacman-contrib`; Vacuo never falls back to `pacman -Scc`. Verify downloads with `sha256sum --ignore-missing --check SHA256SUMS`.
