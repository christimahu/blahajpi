# Contributing to Blahaj PI

Thank you for considering contributing to Blahaj PI! This document outlines the process for contributing to the project and helps to ensure your contributions can be efficiently reviewed and potentially integrated into the codebase.

## Code of Conduct

This project adheres to the [Contributor Covenant](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior via [https://blahajpi.com/contact](https://blahajpi.com/contact).

## How Can I Contribute?

### Reporting Bugs

Bug reports are tracked as [GitHub issues](https://github.com/christimahu/blahajpi/issues). When creating a bug report, please include as much detail as possible:

1. **Use a clear and descriptive title** for the issue to identify the problem.
2. **Describe the exact steps to reproduce the problem** in as much detail as possible.
3. **Provide specific examples** to demonstrate the steps.
4. **Describe the behavior you observed** after following the steps and why you consider it a bug.
5. **Explain the behavior you expected** to see instead and why.
6. **Include screenshots or animations** demonstrating the problem if applicable.
7. **Include details about your environment** (OS, compiler version, library versions, etc.).

### Suggesting Enhancements

Enhancement suggestions are also tracked as [GitHub issues](https://github.com/christimahu/blahajpi/issues). When suggesting an enhancement:

1. **Use a clear and descriptive title** for the issue.
2. **Provide a detailed description of the suggested enhancement** and its expected behavior.
3. **Explain why this enhancement would be useful** to LGBTQIA+ users and the broader community.
4. **List similar analysis tools where this enhancement exists**, if applicable.

### Code Contributions

#### Development Workflow

1. **Fork the repository** on GitHub.
2. **Clone your fork** to your local machine.
3. **Create a new branch** from the `main` branch for your changes.
4. **Make your changes** following the coding conventions described below.
5. **Write or adapt tests** as necessary.
6. **Ensure all tests pass** by running the test script (`python tools/scripts/dev.py --test`).
7. **Commit your changes** with clear commit messages.
8. **Push your branch** to your fork on GitHub.
9. **Submit a Pull Request** from your branch to the project's `main` branch.

#### Pull Request Process

1. Ensure your code follows the coding conventions.
2. Update documentation as necessary.
3. Add tests that verify your changes.
4. Make sure all tests pass.
5. Ensure your changes meet the required test coverage threshold (80%).
6. The PR should clearly describe the problem and solution.
7. The PR will be reviewed by at least one maintainer.

#### Coding Conventions

- **Code Style**: The codebase follows modern C++ style.
  - Use C++23 features where appropriate.
  - Follow the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).
  - Prefer functional approaches where they improve readability.
  - We use clang-format for automated code formatting (configuration file in repository root).

- **Formatting Conventions**:
  - Brace style: Opening braces on the same line, not on a new line:
    ```cpp
    // Correct
    int main() {
        // code
    }
    
    // Incorrect
    int main()
    {
        // code
    }
    ```
  - Indentation: 4 spaces (no tabs)
  - Line length: Maximum 100 characters
  - Function and class definitions should have braces on the same line
  - Control statements (if, for, while, etc.) should always use braces, even for single-line blocks

- **Naming Conventions**:
  - Use `snake_case` for variables, functions, and methods.
  - Use `CamelCase` for class names.
  - Use `ALL_CAPS` for constants and macros.
  - Prefix member variables with `m_` (e.g., `m_variable`).
  - Use descriptive names for all identifiers.

- **Comments and Documentation**:
  - Use Doxygen-style comments for all public API elements.
  - Write clear comments explaining complex algorithms or non-obvious decisions.
  - Keep comments up-to-date with code changes.

- **Error Handling**:
  - Use exceptions for error handling.
  - Document all possible exceptions in API documentation.
  - Prefer std::optional or similar for functions that may not produce a result.

- **Performance Considerations**:
  - Consider both runtime and memory efficiency in your implementations.
  - Use move semantics where appropriate.
  - Use parallelism for operations on large datasets.
  - Document performance characteristics for complex algorithms.

#### Test Requirements

- All new code must have comprehensive tests using Google Test.
- Tests should follow the Given-When-Then pattern for clarity.
- Pull requests must maintain or improve the current test coverage ratio (minimum 80%).
- Tests should cover:
  - Basic functionality (normal operation)
  - Edge cases (boundary conditions)
  - Error handling (invalid inputs, error conditions)
  - Security considerations (where applicable)
- Integration tests should be included for features that span multiple components.

Example test structure:

```cpp
TEST_F(ComponentTest, FunctionHandlesNormalInput) {
    // Given: A properly configured component and valid input
    Component component;
    std::string validInput = "test input";
    
    // When: The function is called with valid input
    auto result = component.process(validInput);
    
    // Then: The result should meet expectations
    EXPECT_EQ(result.status, Status::Success);
    EXPECT_FALSE(result.output.empty());
}
```

## Development Setup

### Prerequisites

- C++23 compatible compiler
- CMake 3.14 or higher
- Python 3.6+ (for build scripts)
- Google Test (for running tests)

### Building for Development

1. Clone your fork:
   ```bash
   git clone https://github.com/yourusername/blahajpi.git
   cd blahajpi
   ```

2. Build using the Python script:
   ```bash
   python dev.py --build
   ```

   Or manually:
   ```bash
   mkdir build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Debug ..
   cmake --build .
   ```

3. Run the tests:
   ```bash
   python dev.py --test
   ```

4. Run with Address Sanitizer:
   ```bash
   python dev.py --run-asan
   ```

5. Run the linter (clang-format):
   ```bash
   python dev.py --format
   ```

## Testing Approach

We use several testing tools to ensure Blahaj PI is reliable, accurate, and secure:

1. **Google Test & Google Mock**: For unit and integration testing. All tests follow the Given-When-Then pattern for clarity.

2. **Sanitizers**:
   - AddressSanitizer (ASan): Detects memory errors like buffer overflows and use-after-free
   - UndefinedBehaviorSanitizer (UBSan): Catches issues like integer overflow

3. **Static Analysis**:
   - Clang-Tidy: Code linting to catch common errors and enforce style guidelines
   - CppCheck: Additional static analysis to find bugs not caught by the compiler

4. **Fuzzing Tests**:
   - For critical components like text processing and vectorization
   - Helps identify edge cases and unexpected inputs

For more details on running different test types, see [TESTING.md](TESTING.md).

## Documentation

Documentation is a crucial part of the project:

1. Update API documentation for any changes to the public API.
2. Add examples for new features.
3. Document any behavior changes.
4. Update the README.md if necessary.
5. Ensure test documentation follows our standard format.

## Community Priorities

As a project focused on supporting the LGBTQIA+ community, contributions that enhance these aspects are particularly welcome:

1. **Accuracy improvements** for detecting anti-trans and anti-LGBTQIA+ content
2. **Safety features** that help protect vulnerable users
3. **Accessibility improvements** that make the tool more usable for everyone
4. **Documentation and examples** that help people understand how to use the tool effectively
5. **Support for additional platforms** that are commonly used by the community

## Additional Notes

### Git Workflow Tips

- Keep your commits atomic and focused on a single issue.
- Write meaningful commit messages explaining what and why (not how).
- Rebase your branch onto the latest main before submitting a PR.
- Use `git pull --rebase` to avoid merge commits.

### Issue and PR Labels

We use GitHub labels to categorize issues and pull requests:

- `bug`: Indicates a confirmed bug or issue
- `enhancement`: Suggests a new feature or improvement
- `documentation`: Related to documentation
- `good first issue`: Good for newcomers
- `help wanted`: Extra attention is needed
- `performance`: Related to performance improvements
- `refactoring`: Code refactoring that doesn't add features or fix bugs
- `accessibility`: Improvements to make the tool more accessible
- `platform-support`: Adding support for additional platforms
- `test`: Related to testing improvements

## Questions?

If you have any questions about contributing, please open an issue and tag it with `question`.

Thank you for contributing to Blahaj PI and helping create safer online spaces for the LGBTQIA+ community!
