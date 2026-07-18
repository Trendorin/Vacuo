# Security policy

## Supported versions

| Version | Security updates |
|---|:---:|
| 0.1.x | Yes |
| Older / unreleased snapshots | No |

## Report a vulnerability

Use [GitHub private vulnerability reporting](https://github.com/Trendorin/Vacuo/security/advisories/new). Do not open a public issue for a suspected vulnerability before a fix is available.

Include:

- affected Vacuo version and distribution;
- exact cleanup category and filesystem involved;
- minimal reproduction steps;
- expected and observed behavior;
- impact, including whether data outside the selected boundary was read or changed;
- logs with usernames, home paths, tokens and unrelated personal data removed.

The target response time is 72 hours for acknowledgement and 14 days for an initial assessment. Complex fixes or coordinated disclosure can take longer. Credit is given with the reporter's consent.

## Scope

High-priority reports include path traversal, symlink following, deletion outside an allow-listed boundary, arbitrary command/argument injection, PolicyKit bypass, unintended persistent-data deletion, unsafe distribution classification, and release-workflow compromise.

Requests to add broader destructive behavior are feature proposals, not vulnerabilities. Package-manager, PolicyKit, Qt and kernel vulnerabilities should also be reported to their upstream projects.
