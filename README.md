# COW Programming Language (Modern C++ Implementation)

[![CI](https://github.com/tronpis/cow/actions/workflows/ci.yml/badge.svg)](https://github.com/tronpis/cow/actions)
[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](CHANGELOG.md)
[![C++](https://img.shields.io/badge/C++-17-orange.svg)](https://isocpp.org/)

The COW Programming Language is an esoteric programming language created by [BigZaphod (Sean Heber)](http://www.bigzaphod.org/cow/).

**Maintainer:** [Trompis](https://github.com/tronpis)  
This is the official modern C++ implementation with professional tooling, comprehensive tests, and formal specification.

## Features

- **Modern C++17**: Clean, type-safe implementation
- **Professional CLI**: Syntax checking, debug mode, execution limits
- **Sandboxed Execution**: Configurable memory, step, and output limits
- **Comprehensive Tests**: Unit and integration tests with Catch2
- **CI/CD**: GitHub Actions with sanitizers and multi-platform builds
- **Formal Specification**: Complete language specification document

## Quick Start

```bash
# Build
mkdir build && cd build
cmake .. -DCOW_BUILD_TESTS=ON
make -j$(nproc)

# Run tests
make test

# Execute a COW program
./cow ../examples/fib.cow

# Check syntax only
./cow --check ../examples/fib.cow

# Run with safety limits
./cow --safe ../examples/fib.cow
```

## Installation

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
sudo cmake --install build
```

## Usage

### Interpreter

```bash
# Basic execution
cow program.cow

# Check syntax without executing
cow --check program.cow

# Debug mode (step-by-step)
cow --debug program.cow

# Safe mode with limits
cow --safe --max-steps 1000000 program.cow

# Quiet mode (no banners)
cow -q program.cow
```

### Compiler

```bash
# Compile to native executable
cowcomp program.cow

# Specify output name
cowcomp -o myprogram program.cow

# Use specific compiler
cowcomp --cxx clang++ program.cow
```

## Language

COW has 12 instructions, all variations of "moo":

| Instruction | Description |
|-------------|-------------|
| `moo` | Loop end |
| `mOo` | Move memory pointer back |
| `moO` | Move memory pointer forward |
| `mOO` | Execute instruction from memory |
| `Moo` | I/O (output char / input char) |
| `MOo` | Decrement memory |
| `MoO` | Increment memory |
| `MOO` | Loop start |
| `OOO` | Zero memory |
| `MMM` | Memory/register exchange |
| `OOM` | Output number |
| `oom` | Input number |

See [SPECIFICATION.md](SPECIFICATION.md) for complete language definition.

## Example

Fibonacci sequence:
```cow
MoO
moO
MoO

mOo

MOO
    OOM
    MMM
    moO
    moO
    MMM
    mOo
    mOo
    moO
    MMM
    mOo
    MMM
    moO
    moO
    MOO
        MOo
        mOo
        MoO
        moO
    moo
    mOo
    mOo
moo
```

## Architecture

### Project Structure
```
include/cow/          - Public headers
  error.hpp           - Error handling classes
  instruction.hpp     - Instruction definitions
  limits.hpp          - Sandbox limits
  parser.hpp          - Parser interface
  source_location.hpp - Source location tracking
  vm.hpp              - Virtual Machine

src/                  - Implementation
  compiler.cpp        - COW-to-C++ compiler
  instruction.cpp     - Instruction utilities
  main.cpp            - CLI interpreter
  parser.cpp          - Parser implementation
  vm.cpp              - VM implementation

tests/                - Test suite
.github/workflows/    - CI/CD configuration
```

### Key Design Decisions

1. **Encapsulation**: All VM state in `CowVM` class (thread-safe)
2. **Error Handling**: Exceptions instead of exit() calls
3. **No Recursion**: Iterative execution prevents stack overflow
4. **Source Locations**: Error messages include line/column
5. **Configurable Limits**: Prevent infinite loops and excessive resource use

## Development

See [CONTRIBUTING.md](CONTRIBUTING.md) for development guidelines.

### Building with Sanitizers

```bash
cmake -B build -DCOW_ENABLE_SANITIZERS=ON
cmake --build build
./build/cow examples/fib.cow
```

### Running Tests

```bash
cmake -B build -DCOW_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Documentation

- [SPECIFICATION.md](SPECIFICATION.md) - Formal language specification
- [CHANGELOG.md](CHANGELOG.md) - Version history
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution guidelines

## License

Public Domain - Original code by Sean Heber (BigZaphod)

Modern implementation maintained by [Trompis](https://github.com/tronpis).  
Contributions are also released to the Public Domain.

## Credits

- **Original Author:** Sean Heber (BigZaphod) - Created the COW language
- **Maintainer:** Trompis - Modern C++ implementation and tooling
