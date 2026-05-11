## Description
<!-- Explain what this PR does and why. Link the related issue if one exists. -->

Closes #<!-- issue number, or remove this line -->

---

## Type of Change

- [ ] Bug fix (non-breaking change that fixes an issue)
- [ ] New feature (non-breaking change that adds functionality)
- [ ] Breaking change (fix or feature that causes existing behaviour to change)
- [ ] Refactor / code quality (no behaviour change)
- [ ] Documentation update
- [ ] Security fix (see [SECURITY.md](../SECURITY.md) if this addresses a vulnerability)

---

## Changes Made
<!-- List the key files / classes changed and what was done. -->

- `backend/` —
- `frontend/` —
- `docs/` —
- Other —

---

## Security Checklist
<!-- Required for all PRs that touch data handling, auth, or user input. -->

- [ ] No plaintext passwords, tokens, or credentials are introduced or committed (including in `data.json`).
- [ ] `data.json` is **not** included in this PR with real user data.
- [ ] All new user input is validated before use in backend handlers.
- [ ] No raw `new`/`delete` introduced without a clear ownership and cleanup strategy.
- [ ] No fixed-size buffers used where dynamic input length is unbounded.

---

## Testing

- [ ] Builds successfully with `run.ps1` (MinGW) **or** the relevant CMake preset.
- [ ] Tested the affected feature(s) manually.
- [ ] Existing functionality in unrelated areas is unaffected.

**Test steps performed:**
1. 
2. 

---

## Screenshots / Demo
<!-- Add before/after screenshots for any UI change. Remove this section if not applicable. -->

---

## Notes for Reviewers
<!-- Anything the reviewer should pay special attention to, known limitations, or follow-up work. -->
