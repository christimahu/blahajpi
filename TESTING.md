# 🧪 Blahaj PI Testing Guide 🧪

Welcome to the Blahaj PI testing documentation! This guide explains how to run tests, understand the test suite, and create new tests for our friendly shark detective. 🦈

## 📋 Table of Contents

- [Test Suite Overview](#test-suite-overview)
- [Running Tests](#running-tests)
- [Understanding Test Types](#understanding-test-types)
- [Test Framework](#test-framework)
- [Writing New Tests](#writing-new-tests)
- [Security Testing](#security-testing)
- [Test Coverage](#test-coverage)
- [Continuous Integration](#continuous-integration)
- [Testing Best Practices](#testing-best-practices)

## Test Suite Overview

Blahaj PI includes a comprehensive test suite that validates:

- 🧠 **Core Functionality**: Algorithm correctness and expected behavior
- 🛠️ **API Interactions**: Proper communication between components
- 🖥️ **CLI Operations**: Command-line interface behavior
- 🧮 **Numerical Accuracy**: Verification of analysis accuracy
- 🔒 **Security**: Protection against vulnerabilities and edge cases

Tests are organized into three categories:
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

### Running Specific Tests

To run specific tests by name:

```bash
# Run tests containing "TextProcessor" in their name
./dev.py --test --filter=TextProcessor

# Run a specific test case
./dev.py --test --filter=TextProcessorTest.HandlesUnicode
```

### Running Tests with Verbose Output

For more detailed test information:

```bash
./dev.py --test --verbose
```

### Running with Sanitizers

For memory and undefined behavior detection:

```bash
# Address Sanitizer (memory errors)
./dev.py --run-asan

# Undefined Behavior Sanitizer
./dev.py --run-ubsan
```

### Measuring Test Coverage

To measure test coverage:

```bash
./dev.py --coverage

# View the coverage report
open build/coverage/index.html
```

### Running Fuzz Tests

To run fuzz tests (requires clang):

```bash
# Run the text processor fuzzer for 60 seconds
./build/bin/text_processor_fuzzer -max_total_time=60
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

## Test Framework

Blahaj PI uses Google Test as its testing framework. Key features include:

### Test Fixtures

Test fixtures allow you to set up and tear down test environments:

```cpp
class TextProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize before each test
        processor = std::make_unique<blahajpi::preprocessing::TextProcessor>();
    }
    
    void TearDown() override {
        // Clean up after each test
    }
    
    std::unique_ptr<blahajpi::preprocessing::TextProcessor> processor;
};
```

### Test Cases

Test cases follow the Given-When-Then pattern for clarity:

```cpp
TEST_F(TextProcessorTest, RemovesPunctuation) {
    // Given: Text with punctuation
    std::string input = "Hello, world!";
    
    // When: We process the text
    std::string result = processor->preprocess(input);
    
    // Then: Punctuation should be removed
    EXPECT_EQ(result, "Hello world");
}
```

### Assertions

Google Test provides various assertion macros:

- `EXPECT_EQ(a, b)` - Tests that a == b
- `EXPECT_NE(a, b)` - Tests that a != b
- `EXPECT_LT(a, b)` - Tests that a < b
- `EXPECT_GT(a, b)` - Tests that a > b
- `EXPECT_TRUE(condition)` - Tests that condition is true
- `EXPECT_FALSE(condition)` - Tests that condition is false
- `EXPECT_THROW(statement, exception_type)` - Tests that statement throws the expected exception

### Parameterized Tests

For testing with multiple inputs:

```cpp
class RemovePunctuationTest : 
    public ::testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(RemovePunctuationTest, RemovesPunctuation) {
    auto [input, expected] = GetParam();
    TextProcessor processor;
    EXPECT_EQ(processor.removePunctuation(input), expected);
}

INSTANTIATE_TEST_SUITE_P(
    TextProcessor,
    RemovePunctuationTest,
    ::testing::Values(
        std::make_pair("Hello, world!", "Hello world"),
        std::make_pair("Test.with.dots", "Testwithdots"),
        std::make_pair("No-punctuation", "Nopunctuation")
    )
);
```

### Mocking

For isolating components from their dependencies:

```cpp
// Mock class for a classifier
class MockClassifier : public blahajpi::models::Classifier {
public:
    MOCK_METHOD(std::vector<int>, predict, (const std::vector<std::vector<double>>&), (override));
    MOCK_METHOD(std::vector<double>, predictProbability, (const std::vector<std::vector<double>>&), (override));
};

TEST_F(AnalyzerTest, UsesMockClassifier) {
    // Setup mock
    auto mockClassifier = std::make_unique<MockClassifier>();
    EXPECT_CALL(*mockClassifier, predict)
        .WillOnce(Return(std::vector<int>{1}));
        
    // Test using the mock
    // ...
}
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
#include <gtest/gtest.h>
#include <string>

class MyFeatureTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }

    void TearDown() override {
        // Cleanup code
    }
};

TEST_F(MyFeatureTest, HandlesValidInput) {
    // Given
    // ...
    
    // When
    // ...
    
    // Then
    EXPECT_TRUE(result);
}

TEST_F(MyFeatureTest, HandlesInvalidInput) {
    // Given invalid input
    // ...
    
    // When: Function is called with invalid input
    // ...
    
    // Then: Should handle error appropriately
    EXPECT_FALSE(result.isValid());
}
```

3. Add your test to `tests/CMakeLists.txt` in the `UNIT_TESTS` list

### Creating a Security Test

To create a security-focused test:

```cpp
TEST_F(TextProcessorTest, ResistsBufferOverflow) {
    // Given: Extremely long input
    std::string longInput(1000000, 'A');
    
    // When/Then: Processing should not crash
    EXPECT_NO_THROW({
        std::string result = processor->preprocess(longInput);
    });
}

TEST_F(VectorizerTest, HandlesZeroLengthInput) {
    // Given: Empty input
    std::vector<std::string> emptyTexts = {""};
    
    // When/Then: Should handle gracefully
    EXPECT_NO_THROW({
        auto features = vectorizer.transform(emptyTexts);
        EXPECT_TRUE(features[0].empty());
    });
}
```

### Creating Integration Tests

For integration tests that verify component interactions:

```cpp
class ConfigAnalyzerIntegrationTest : public ::testing::Test {
protected:
    blahajpi::Config config;
    blahajpi::Analyzer analyzer;
};

TEST_F(ConfigAnalyzerIntegrationTest, ConfigurationAffectsAnalysis) {
    // Given: Custom configuration
    config.set("threshold", "0.8");
    analyzer.loadConfig(config);
    
    // When: Analyzing text
    auto result = analyzer.analyze("Test text");
    
    // Then: Configuration should affect the result
    EXPECT_GT(result.confidence, 0.8);
}
```

## Security Testing

Blahaj PI includes specialized security testing to protect against vulnerabilities:

### Using Address Sanitizer

Address Sanitizer (ASan) detects memory errors:

```bash
./dev.py --run-asan
```

Common issues detected:
- Buffer overflows
- Stack-use-after-return
- Heap-use-after-free
- Memory leaks
- Double-free errors

### Using Undefined Behavior Sanitizer

UBSan finds undefined behavior:

```bash
./dev.py --run-ubsan
```

Issues detected:
- Integer overflow
- Null pointer dereference
- Division by zero
- Misaligned memory access

### Fuzz Testing

Fuzz testing generates random inputs to find crashes:

```bash
# Build the fuzzers
cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++ ..
cmake --build .

# Run the text processor fuzzer
./bin/text_processor_fuzzer -max_total_time=300
```

Our fuzzing targets include:
- Text processor input handling
- Vectorizer functionality
- Configuration parsing
- Model input processing

### Security-Specific Unit Tests

We include explicit tests for security properties:

- Input validation
- Handling of malicious inputs
- Buffer overflow prevention
- File path traversal prevention

## Test Coverage

We aim for at least 80% code coverage across the codebase:

### Measuring Coverage

```bash
./dev.py --coverage
```

This generates a coverage report in `build/coverage/index.html`.

### Coverage Requirements

- Core functionality should have >90% coverage
- All public API methods must have tests
- Error handling paths must be tested
- Security-critical code requires 100% coverage

### Improving Coverage

1. Identify uncovered areas with the coverage report
2. Add targeted tests for uncovered branches
3. Include tests for edge cases and error conditions
4. Test helper functions directly if they're not covered by API tests

## Continuous Integration

Blahaj PI uses automated testing for quality assurance:

### GitHub Actions Workflow

Tests run automatically on pull requests:
- Unit and integration tests
- Sanitizers to catch memory issues
- Static analysis with clang-tidy and cppcheck

### Local CI Verification

Before submitting a PR, run the full test suite:

```bash
./dev.py --full-check
```

This runs:
1. All tests
2. Sanitizer checks
3. Static analysis
4. Test coverage verification

## Testing Best Practices

When contributing tests to Blahaj PI, please follow these guidelines:

1. **Test One Thing at a Time**: Each test should verify a single specific behavior
2. **Use Descriptive Names**: Test names should describe what is being tested
3. **Follow Given-When-Then**: Structure tests with clear setup, action, and verification
4. **Test Edge Cases**: Include tests for boundary conditions and unusual inputs
5. **Test Failures**: Verify that functions fail appropriately with invalid inputs
6. **Keep Tests Fast**: Unit tests especially should execute quickly
7. **Document Test Purpose**: Each test class should have a brief description

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
