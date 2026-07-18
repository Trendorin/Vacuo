## Summary

<!-- What changed and why? -->

## Safety impact

<!-- Which paths, actions, privileges or persistent data can this affect? Write "None" only after checking. -->

## Verification

- [ ] `ctest --test-dir build --output-on-failure`
- [ ] New cleanup behavior has negative path/symlink tests
- [ ] Documentation and changelog match the implementation
- [ ] No shell command or arbitrary privileged argument was introduced
- [ ] Commits include a DCO `Signed-off-by` line

## Distribution evidence

<!-- Distribution/version, desktop, session type, filesystem, and manual checks. Remove personal paths. -->
