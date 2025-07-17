# Contributing to PRNG_mini

Thank you for your interest in contributing to **PRNG_mini**, a cross-platform, C-based pseudo-random number generator library designed for cryptographic and secure applications. This project is focused on delivering fast, secure, and platform-independent randomness with bindings for C/C++, Flutter, Python, and C#.

> 🔐 Security-sensitive: please follow secure development practices.

---

## Table of Contents

- [Getting Started](#getting-started)
- [Code Guidelines](#code-guidelines)
- [Security Considerations](#security-considerations)
- [Reporting Vulnerabilities](#reporting-vulnerabilities)
- [Submitting Issues](#submitting-issues)
- [Pull Requests](#pull-requests)
- [License](#license)

---

## Getting Started

To build the project locally:

```bash
git clone https://github.com/your-org/PRNG_mini.git
cd PRNG_mini
mkdir build && cd build
cmake ..
make
```

This will compile the core C library. The project has **no external dependencies**.

### Project Layout

- `src/` — Core C implementation
- `include/` — Public headers
- `tests/` — Unit and example-based test cases
- `tools/` — Internal tooling (e.g., license key utilities)
- `build/` — CMake build artifacts (excluded from Git)

---

## Code Guidelines

- Use **C99 or C11**, no C++ in core files.
- Avoid heap unless necessary; use `pm_free()` to zero memory before release.
- Public API functions should use the `pm_` prefix.
- Follow modular design and name files by purpose (e.g., `random.c`, `license_key.c`).
- Comment all public functions with usage and parameter details.

---

## Security Considerations

This library is intended for **cryptographic applications**, so we require that contributors:

- Avoid use of insecure random sources (e.g., `rand()`, `srand()`, `time()`)
- Do not introduce non-deterministic behavior without justification
- Reference NIST, RFC, or cryptographic standards where applicable
- Raise questions early if unsure about cryptographic implications

---

## Reporting Vulnerabilities

To report a security vulnerability:

1. **Do not create a GitHub issue.**
2. Email the maintainers privately at [security@example.com](mailto:security@example.com)
3. Include a minimal reproducible example and describe the impact clearly.

We respond within 72 hours and follow responsible disclosure.

---

## Submitting Issues

Use GitHub Issues to report:

- Bugs or unexpected behavior
- Feature requests with clear motivation
- Platform-specific compatibility problems
- Questions related to supported bindings (C, Flutter, Python, etc.)

Please include:

- Platform (e.g., Windows 10, macOS 13, Android NDK version)
- Expected vs actual behavior
- Test case or code snippet if possible

---

## Pull Requests

- Fork the repository and work on a branch
- Add tests for new features or fixes under `tests/`
- Validate across platforms if relevant (Windows/Linux/macOS)
- Format C code consistently; avoid nonstandard extensions
- Document any new public API in header comments

Example test apps:

- `tests/randomness/` for uniformity and entropy checks
- `tests/license_key/` for license key generation/validation

---

## License

By contributing, you agree your code will be licensed under the [GNU General Public License v3.0 (GPLv3)](./LICENSE).

If you require a CLA or special licensing arrangement, contact us first.

---

Thank you for supporting secure open-source software!
