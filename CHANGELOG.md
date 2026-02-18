# Changelog

All notable changes to the COW Programming Language project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

Maintainer: [Trompis](https://github.com/tronpis)

### Added
- Modern C++17 implementation with proper encapsulation
- CMake build system with install targets
- Exception-based error handling (no more exit() calls)
- Source location tracking for better error messages
- Configurable execution limits (memory, steps, output)
- Professional CLI with --check, --version, --help options
- Formal language specification (SPECIFICATION.md)
- GitHub Actions CI/CD pipeline
- Comprehensive test suite
- Static analysis with sanitizers support

### Changed
- VM state now encapsulated in CowVM class (thread-safe)
- Replaced recursive execution with iterative loop (no stack overflow)
- Compiler auto-detects available C++ compiler (no hardcoded g++)
- Parser now tracks line/column for error reporting

### Fixed
- Memory underflow now properly reported as error
- Loop matching validated at parse time
- Unbalanced brackets detected before execution

### Security
- Added execution sandbox with configurable limits
- Input validation prevents buffer overflows

## [1.0.0] - 2000-01-01

### Added
- Initial release by Sean Heber (BigZaphod)
- 12 instruction COW language
- Interpreter implementation
- Compiler to C++
- Example programs (Fibonacci, 99 bottles)

[Unreleased]: https://github.com/tronpis/cow/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/tronpis/cow/releases/tag/v1.0.0
