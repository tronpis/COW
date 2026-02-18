# COW Programming Language (Modernized Version 2.0)

The COW Programming Language is an esoteric programming language created by [BigZaphod (Sean Heber)](http://www.bigzaphod.org/cow/). This repository contains a modernized C++ implementation that addresses architectural issues in the original code.

## Architecture Improvements

This modernized version addresses the following issues from the original implementation:

### 1. Encapsulated VM Class
- **Before**: Global variables for memory, program, registers
- **After**: `CowVM` class encapsulates all state, making it thread-safe and reusable

### 2. Exception-Based Error Handling
- **Before**: `exit()` calls that terminate the process immediately
- **After**: Exceptions (`VMException`, `LimitExceededException`) allow graceful error handling

### 3. Loop-Based Execution
- **Before**: Recursive `exec()` function that could cause stack overflow
- **After**: Main execution loop with `run()` and `step()` methods

### 4. Configurable Memory Limits
- **Before**: Unbounded memory growth
- **After**: `Limits` struct with configurable max memory, max steps, and max output

### 5. Compiler Auto-Detection
- **Before**: Hardcoded `g++` compiler path
- **After**: Auto-detects available compiler (g++, clang++, c++) or uses `CXX` environment variable

### 6. Modern C++ (C++17)
- Strongly-typed `enum class` for opcodes
- `std::function` for I/O handlers (testable)
- Namespace organization
- CMake build system

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

### Interpreter

```bash
# Basic usage
./cow program.cow

# Safe mode with limits
./cow --safe program.cow

# Custom memory size and limits
./cow --memory 60000 --max-steps 1000000 program.cow

# Quiet mode (no greetings)
./cow -q program.cow

# Debug mode (step-by-step)
./cow -d program.cow
```

### Compiler

```bash
# Compile to native executable
./cowcomp program.cow

# Specify output name
./cowcomp -o myprogram program.cow

# Use specific compiler
./cowcomp --cxx clang++ program.cow

# Generate C++ source only
./cowcomp --cpp-only program.cow

# Keep generated C++ source
./cowcomp -k program.cow
```

## Project Structure

```
include/cow/
  instruction.hpp  - Instruction enum and OpCode definitions
  vm.hpp           - CowVM class header
  parser.hpp       - Parser for COW source files
  limits.hpp       - Sandbox limits configuration

src/
  instruction.cpp  - Instruction implementation
  vm.cpp           - CowVM implementation (main execution loop)
  parser.cpp       - Parser implementation
  main.cpp         - CLI interpreter entry point
  compiler.cpp     - COW-to-C++ compiler

examples/
  fib.cow          - Fibonacci sequence
  99.cow           - 99 bottles of beer
  i.cow            - Input/output test
```

## Example Program

Fibonacci sequence (fib.cow):
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

## Library Usage

The CowVM can be used as a library:

```cpp
#include "cow/vm.hpp"
#include "cow/parser.hpp"

// Parse program
auto program = cow::Parser::parseFile("program.cow");

// Create VM with limits
cow::Limits limits;
limits.max_steps = 1000000;
cow::CowVM vm(limits, 30000);

// Load and run
vm.load(program);
vm.run();
```

## COW Instruction Set

| Instruction | Description |
|-------------|-------------|
| `moo` | Loop end (like Brainfuck `]`) |
| `mOo` | Move memory pointer back (like `<`) |
| `moO` | Move memory pointer forward (like `>`) |
| `mOO` | Execute instruction at memory position |
| `Moo` | Output char / Input char (like `.` / `,`) |
| `MOo` | Decrement memory (like `-`) |
| `MoO` | Increment memory (like `+`) |
| `MOO` | Loop start (like Brainfuck `[`) |
| `OOO` | Zero memory |
| `MMM` | Memory/register exchange |
| `OOM` | Output number |
| `oom` | Input number |

## License

Public Domain (original code by Sean Heber)
