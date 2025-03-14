# 🧪 Blahaj PI Testing Guide 🧪

Welcome to the Blahaj PI testing documentation! This guide explains how to run tests, understand the test suite, and create new tests for our friendly shark detective. 🦈

## 📋 Table of Contents

- [Test Suite Overview](#test-suite-overview)
- [Running Tests](#running-tests)
- [Understanding Test Types](#understanding-test-types)
- [Test File Structure](#test-file-structure)
- [Writing New Tests](#writing-new-tests)
- [Continuous Integration](#continuous-integration)
- [Testing Best Practices](#testing-best-practices)

## Test Suite Overview

Blahaj PI includes a comprehensive test suite that validates:

- 🧠 **Core Functionality**: Algorithm correctness and expected behavior
- 🛠️ **API Interactions**: Proper communication between components
- 🖥️ **CLI Operations**: Command-line interface behavior
- 🧮 **Numerical Accuracy**: Verification of analysis accuracy

Tests are located in the `tests/` directory and are organized into three categories:
- **Unit Tests**: Testing individual components in isolation
- **Integration Tests**: Testing component interactions
- **End-to-End (E2E) Tests**: Testing complete workflows

## Running Tests

### Running All Tests

The simplest way to run all tests is using the dev script:

```bash
./dev.py --test
```

This will compile the project if needed and run all tests, showing you which tests passed and failed.

### Running Specific Test Categories

You can run specific categories of tests:

```bash
# Run only unit tests
./dev.py --test --unit

# Run only integration tests
./dev.py --test --integration

# Run only end-to-end tests
./dev.py --test --e2e
```

### Running Tests with Verbose Output

For more detailed test information:

```bash
./dev.py --test --verbose
```

### Running Tests Manually

If you prefer to run tests manually with CTest:

```bash
cd build
ctest --output-on-failure
```

## Understanding Test Types

### Unit Tests

Unit tests check individual components in isolation. These tests focus on:
- Text processing functions
- Vectorization algorithms
- Classification logic
- Metrics calculation

Sample unit test files:
- `tests/unit/analyzer_test.cpp`
- `tests/unit/text_processor_test.cpp`
- `tests/unit/vectorizer_test.cpp`

### Integration Tests

Integration tests verify that different components work together correctly:
- API compatibility between modules
- Data flow across component boundaries
- Correct handling of configuration options

Sample integration test files:
- `tests/integration/api_integration_test.cpp`
- `tests/integration/cli_integration_test.cpp`

### End-to-End Tests

E2E tests validate complete workflows from input to output:
- Entire analysis pipeline
- CLI command execution
- File I/O operations

Sample E2E test files:
- `tests/e2e/cli_runner.py`
- `tests/e2e/test_api.py`

## Test File Structure

Tests are organized in the following directory structure:

```
tests/
├── CMakeLists.txt           # Test build configuration
├── unit/                    # Unit tests
│   ├── analyzer_test.cpp
│   ├── metrics_test.cpp
│   ├── neural_network_test.cpp
│   ├── sgd_classifier_test.cpp  
│   ├── text_processor_test.cpp
│   └── vectorizer_test.cpp
├── integration/             # Integration tests
│   ├── api_integration_test.cpp
│   └── cli_integration_test.cpp
└── e2e/                     # End-to-end tests
    ├── cli_runner.py
    └── test_api.py
```

## Writing New Tests

### Creating a New Unit Test

To create a new unit test:

1. Create a new C++ file in the `tests/unit/` directory
2. Follow this basic structure:

```cpp
/**
 * @file my_feature_test.cpp
 * @brief Unit tests for the MyFeature functionality
 */

#include "blahajpi/my_feature.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

int main() {
    try {
        // Setup test data
        // ...
        
        // Test your feature
        // ...
        
        // Assert expected outcomes
        // assert(result == expected);
        
        std::cout << "MyFeature test passed" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
```

3. Add your test to `tests/CMakeLists.txt` in the `UNIT_TESTS` list

### Creating Integration or E2E Tests

Similar processes apply for integration and E2E tests. For Python-based E2E tests, use the standard Python unittest framework.

## Continuous Integration

Blahaj PI uses automated testing through our CI pipeline, which:
- Runs all tests on each pull request
- Verifies compatibility across operating systems
- Reports test coverage statistics

The CI pipeline is configured to fail if any tests fail, ensuring that only working code is merged.

## Testing Best Practices

When contributing tests to Blahaj PI, please follow these guidelines:

1. **Test One Thing at a Time**: Each test should verify a single specific behavior
2. **Descriptive Names**: Use clear function names that describe what is being tested
3. **Setup and Teardown**: Clean up after tests to avoid affecting other tests
4. **Test Edge Cases**: Include tests for boundary conditions and unusual inputs
5. **Test Failures**: Verify that functions fail appropriately with invalid inputs
6. **Keep Tests Fast**: Unit tests especially should execute quickly
7. **Document Test Purpose**: Comment each test with what it's verifying and why

For text analysis tests in particular:
- Include diverse examples representing various types of content
- Test with both obvious and subtle examples of harmful content
- Ensure test data does not contain personally identifiable information
- Avoid perpetuating harmful language in test cases

---

<p align="center">
  <i>Testing helps Blahaj be a better detective! 🦈🔍</i><br>
  <i>If you have questions about testing, please reach out via GitHub issues.</i>
</p>
