# Privacy

Last updated: 2026-07-18

Vacuo is an offline desktop utility. The application has no networking code, telemetry, analytics, account system, advertising SDK, crash-report upload, update checker or remote configuration.

## Data processed locally

Vacuo reads only the local information needed to build a cleanup plan:

- `/etc/os-release` and `/proc/self/mountinfo`;
- desktop/session environment variables;
- metadata required to measure known cache paths, such as allocated size, file type and ownership;
- the cleanup categories selected by the user.

The GUI stores window geometry through `QSettings`. On a normal Linux desktop this is kept in the user's local configuration area. Vacuo does not store a file list, cache contents, browser data or cleanup history.

`vacuoctl --json` writes its report to standard output only. The caller decides whether to save or transmit it.

## Privileged actions

PolicyKit and the system journal may record that an authorization or command occurred. Those records are produced and controlled by the operating system, not collected by Vacuo or its maintainer.

## Deletion

Selected cache is permanently removed from the local machine. Vacuo does not upload a copy and does not provide remote recovery. Filesystems, backups and snapshots can retain data independently of Vacuo.

## Project services

Downloading releases, opening issues or contributing uses GitHub and is governed by GitHub's own privacy terms. This is separate from running the Vacuo application.
