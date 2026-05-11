# Security Policy

## Supported Versions

| Version | Supported |
|---------|-----------|
| Latest (`main`) | ✅ |
| Older branches | ❌ |

Only the latest commit on `main` receives security fixes. Please update before reporting.

---

## ⚠️ Known Security Issues (Action Required)

The following issues exist in the current codebase and **must be resolved before any public or production deployment**:

### 1. Plaintext Passwords in `data.json`
User passwords are stored and persisted in `data.json` without any hashing or encryption. This means anyone with read access to the file (or the repository) can obtain credentials directly.

**Fix:** Hash all passwords with a strong algorithm (e.g., bcrypt, Argon2) before writing to `data.json`. Never store or commit raw passwords.

### 2. Credentials Committed to Version Control
`data.json` currently contains real usernames, passwords, and an email address committed directly into the repository. These credentials are now part of the git history and must be treated as **fully compromised**.

**Immediate steps:**
- Rotate all passwords present in the file.
- Remove the file from git history using `git filter-repo` or BFG Repo Cleaner.
- Add `data.json` to `.gitignore` going forward, or replace it with a sanitized `data.example.json`.

### 3. Fixed-size Comment Array (`comments[50]` in `Post`)
The `Post` class uses a raw fixed-size array for comments with no bounds checking beyond a silent drop at index 50. While not currently exploitable remotely, this is a latent memory-safety issue.

**Fix:** Replace with `std::vector<std::string>`.

### 4. Raw Pointer Arrays in `User`
The `User` class manages `friends`, `request`, `follower`, `following`, and `posts` as manually allocated raw pointer arrays with no capacity tracking. The copy constructor performs shallow copies of these arrays. This can lead to double-free or use-after-free bugs.

**Fix:** Replace with `std::vector` and proper ownership semantics, or use smart pointers.

---

## Reporting a Vulnerability

If you discover a security vulnerability in CONNECT, please report it **privately** — do not open a public GitHub Issue.

**How to report:**
1. Email the maintainers at the address listed in the repository's contact section, with the subject line `[SECURITY] <brief description>`.
2. Or use [GitHub's private vulnerability reporting](https://docs.github.com/en/code-security/security-advisories/guidance-on-reporting-and-writing/privately-reporting-a-security-vulnerability) if enabled on this repository.

**What to include:**
- A clear description of the vulnerability.
- Steps to reproduce or a proof-of-concept.
- The component affected (e.g., `backend/src/user.cpp`, `data.json` handling).
- Potential impact (data exposure, crash, privilege escalation, etc.).

**What to expect:**
- Acknowledgement within **48 hours**.
- A status update (confirmed / not confirmed / needs more info) within **7 days**.
- A fix or mitigation plan within **30 days** for confirmed issues.
- Credit in the changelog if you wish.

We ask that you give us reasonable time to address the issue before any public disclosure.

---

## Scope

| In Scope | Out of Scope |
|----------|--------------|
| Authentication & authorization logic | Third-party Qt framework bugs |
| Password storage and `data.json` handling | Build toolchain (CMake, MinGW) vulnerabilities |
| Input validation in backend (`user.cpp`, `post.cpp`, etc.) | Issues in already-deprecated branches |
| Memory safety in C++ backend classes | Theoretical issues with no realistic attack vector |
| Admin privilege escalation via `AdminUser` | |

---

## Security Best Practices for Contributors

- **Never commit `data.json`** with real user data. Use `data.example.json` with dummy values.
- **Never store passwords in plaintext.** Always hash before writing to disk.
- **Prefer `std::vector` and smart pointers** over raw arrays and manual `new`/`delete`.
- **Validate all user input** in backend handlers before processing or persisting.
- **Do not hardcode credentials** or file paths in source files.
