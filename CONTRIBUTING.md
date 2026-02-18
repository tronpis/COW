# Contributing to COW

Thank you for your interest in contributing to the COW Programming Language!

This project is maintained by [Trompis](https://github.com/tronpis).

## Code of Conduct

This project adheres to a code of conduct. By participating, you are expected to uphold this code:
- Be respectful and inclusive
- Accept constructive criticism gracefully
- Focus on what is best for the community

## How to Contribute

### Reporting Bugs

Before creating a bug report, please:
1. Check if the issue already exists
2. Use the latest version
3. Include a minimal reproducible example

Bug reports should include:
- COW version
- Operating system
- Compiler version
- Steps to reproduce
- Expected vs actual behavior
- Sample COW program that demonstrates the issue

### Suggesting Enhancements

Enhancement suggestions are welcome! Please provide:
- Clear description of the feature
- Use cases and rationale
- Possible implementation approach (optional)

### Pull Requests

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Run tests (`make test`)
5. Commit with clear messages (`git commit -m 'feat: add amazing feature'`)
6. Push to your fork (`git push origin feature/amazing-feature`)
7. Open a Pull Request

## Development Setup

### Prerequisites
- C++17 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- CMake 3.14 or higher
- Git

### Building
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

### Running Tests
```bash
make test
```

### With Sanitizers
```bash
cmake .. -DCOW_ENABLE_SANITIZERS=ON
make
./cow ../examples/fib.cow
```

## Coding Standards

### C++ Style
- Use C++17 features appropriately
- Prefer `std::` types over C equivalents
- No raw pointers (use smart pointers or references)
- Use `enum class` for enumerations
- Use `const` and `constexpr` where possible
- Follow the existing code style

### Naming Conventions
- Classes: `PascalCase`
- Functions: `camelCase`
- Variables: `snake_case`
- Constants: `UPPER_SNAKE_CASE`
- Private members: trailing underscore (`name_`)

### Error Handling
- Use exceptions for exceptional conditions
- Use `std::optional` for values that may not exist
- Always provide meaningful error messages
- Include source location when applicable

### Documentation
- Document all public APIs
- Use Doxygen-style comments
- Include examples where helpful
- Update SPECIFICATION.md for language changes

## Testing

### Test Categories
1. **Unit Tests**: Individual component testing
2. **Integration Tests**: End-to-end execution tests
3. **Conformance Tests**: Specification compliance

### Writing Tests
```cpp
TEST_CASE("Increment instruction") {
    cow::CowVM vm;
    vm.load({cow::Instruction(cow::OpCode::MoO)});
    vm.run();
    REQUIRE(vm.currentMemoryValue() == 1);
}
```

## Commit Message Format

We follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>: <description>

[optional body]

[optional footer]
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation only
- `style`: Code style changes
- `refactor`: Code refactoring
- `test`: Adding/updating tests
- `chore`: Build process, dependencies, etc.

Examples:
```
feat: add --check option for syntax validation
fix: handle memory underflow in mOo instruction
docs: update SPECIFICATION with mOO semantics
```

## Release Process

1. Update version in `CMakeLists.txt`
2. Update `CHANGELOG.md`
3. Create a git tag
4. CI will build and publish release artifacts

## Questions?

Feel free to open an issue for discussion!
