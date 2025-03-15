# 🛠️ Blahaj PI Troubleshooting Guide 🛠️

Welcome to the Blahaj PI troubleshooting guide! If you're encountering issues with our friendly shark detective, this document will help you resolve common problems. 🦈

## 📋 Table of Contents

- [Build Issues](#build-issues)
- [Model Training Issues](#model-training-issues)
- [Analysis Issues](#analysis-issues)
- [Visualization Issues](#visualization-issues)
- [Command-Line Interface Issues](#command-line-interface-issues)
- [Testing Issues](#testing-issues)
- [Getting Additional Help](#getting-additional-help)

## Build Issues

### Error: "CMake not found" or CMake version too old

**Problem:** When running `./dev.py --build`, you get an error about CMake not being found or the version being too old.

**Solution:** 
1. Install CMake 3.14 or newer:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install cmake
   
   # macOS
   brew install cmake
   
   # Windows
   # Download and install from https://cmake.org/download/
   ```
2. Verify your installation: `cmake --version`

### Error: "C++ compiler does not support C++23"

**Problem:** Your compiler doesn't support the required C++23 features.

**Solution:**
1. Update your compiler:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install g++-13  # or newer
   
   # macOS
   brew install gcc@13  # or newer
   
   # Windows
   # Update to the latest Visual Studio
   ```
2. If using a specific compiler, specify it to CMake:
   ```bash
   export CC=/path/to/gcc
   export CXX=/path/to/g++
   ./dev.py --build
   ```

### Error: "Could not find Python"

**Problem:** The build script can't locate Python.

**Solution:**
1. Make sure Python 3.6+ is installed: `python3 --version`
2. If needed, install Python:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install python3
   
   # macOS
   brew install python
   
   # Windows
   # Download and install from python.org
   ```

### Error: "Could not find GTest"

**Problem:** Google Test is missing or not properly installed.

**Solution:**
1. Install Google Test:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libgtest-dev
   sudo apt-get install cmake
   cd /usr/src/gtest
   sudo cmake .
   sudo make
   sudo cp lib/*.a /usr/lib
   
   # macOS
   brew install googletest
   
   # Windows
   # Use vcpkg or build from source
   ```
2. If installed in a non-standard location, specify the path:
   ```bash
   cmake -DGTEST_ROOT=/path/to/gtest ..
   ```

## Model Training Issues

### Error: "No model loaded. Call loadModel() first."

**Problem:** You're trying to analyze text without having a model loaded.

**Solution:**
1. Train a model first:
   ```bash
   ./dev.py --run -- train --dataset data/examples/twitter_example.csv --output models/my_model
   ```
2. Make sure the model directory exists and has been created successfully

### Error: "Could not open file for writing"

**Problem:** The system can't write to the specified output directory.

**Solution:**
1. Check directory permissions: `ls -la models/`
2. Create the directory if it doesn't exist: `mkdir -p models/my_model`
3. Check if you have write permissions in the project directory

### Warning: "Training might not have completed successfully"

**Problem:** The training process encountered issues but tried to continue.

**Solution:**
1. Check the error message for specific issues
2. Ensure your dataset is properly formatted
3. Try using the `--verbose` flag for more detailed information:
   ```bash
   ./dev.py --run -- train --dataset data/examples/twitter_example.csv --output models/my_model --verbose
   ```

## Analysis Issues

### Error: "Could not find text column in CSV"

**Problem:** The analyzer can't find the expected column in your CSV file.

**Solution:**
1. Ensure your CSV has the right column names: the example dataset uses `sentiment_label` for labels and `tweet_text` for content
2. If your columns have different names, use a configuration file:
   ```
   # Your config file
   text-column = your_text_column_name
   label-column = your_label_column_name
   ```
   Then run:
   ```bash
   ./dev.py --run -- --config your_config.conf analyze --file your_file.csv
   ```

### Error: "Invalid file format"

**Problem:** The file you're trying to analyze isn't in a supported format.

**Solution:**
1. Ensure you're using a properly formatted CSV file
2. Check file encoding (UTF-8 is recommended)
3. In the current MVP, only use the included example dataset

## Visualization Issues

### Error: "No content to visualize"

**Problem:** The visualize command can't find any content to create visualizations.

**Solution:**
1. Make sure you've analyzed content first
2. Check that you're pointing to the correct file
3. Verify the results folder exists: `mkdir -p results`

### Empty or Incomplete Word Cloud

**Problem:** The generated word cloud is empty or has very few words.

**Solution:**
1. Ensure the dataset contains sufficient text
2. Try increasing the word count parameter:
   ```bash
   ./dev.py --run -- visualize --input data/examples/twitter_example.csv --output results/viz.txt --max-words 100
   ```

## Command-Line Interface Issues

### Error: "Command not found"

**Problem:** The system can't find the `dev.py` script or the blahajpi executable.

**Solution:**
1. Make sure you're in the project root directory
2. Check if the file is executable: `chmod +x dev.py`
3. Use the full path: `./dev.py` instead of just `dev.py`

### Permission Denied

**Problem:** You get "Permission denied" when trying to run commands.

**Solution:**
1. Make script executable: `chmod +x dev.py`
2. Make sure the build created an executable: `ls -la build/bin/`
3. If needed, make the executable runnable: `chmod +x build/bin/blahajpi`

## Testing Issues

### Error: "Test binary not found"

**Problem:** The test executables can't be found.

**Solution:**
1. Make sure you've built the project: `./dev.py --build`
2. Check that test binaries exist in build directory: `ls build/bin/*_test`
3. If not, rebuild with tests enabled: `cmake -DBUILD_TESTING=ON ..`

### Tests Are Failing

**Problem:** You're seeing test failures when running the test suite.

**Solution:**
1. Run with verbose output to see detailed error messages:
   ```bash
   ./dev.py --test --verbose
   ```
2. Run specific failing tests to isolate the issue:
   ```bash
   ./dev.py --test --filter=TextProcessorTest
   ```
3. Check for environment-specific issues (file paths, permissions, etc.)

### Google Test Framework Not Found

**Problem:** CMake can't find the Google Test framework.

**Solution:**
1. Verify Google Test is installed: `pkg-config --modversion gtest`
2. Install it if needed (see [Build Issues](#build-issues) section)
3. Set GTEST_ROOT environment variable if installed in non-standard location:
   ```bash
   export GTEST_ROOT=/path/to/googletest
   ```

### Address Sanitizer Reports Errors

**Problem:** When running with ASAN (`--run-asan`), you see memory errors.

**Solution:**
1. Check the specific error type (buffer overflow, use-after-free, etc.)
2. Look at the call stack to identify the problematic code
3. Common fixes include:
   - Ensuring proper array bounds checking
   - Fixing memory management issues
   - Correcting pointer usage
   - Resolving lifetime issues with references

### Undefined Behavior Sanitizer Reports Errors

**Problem:** UBSAN is reporting undefined behavior.

**Solution:**
1. Look for integer overflow, null dereferences, or invalid enum values
2. Add appropriate checks and validations
3. For integer overflows, consider using larger types or explicit overflow checking

### Fuzz Testing Crashes or Timeouts

**Problem:** Your fuzz tests are crashing or timing out.

**Solution:**
1. For crashes:
   - Check the generated inputs that caused the crash
   - Look for edge cases in your input handling
   - Add input validation before processing
   - Fix memory management issues highlighted by the crash

2. For timeouts:
   - Look for infinite loops or exponential complexity in your code
   - Add early termination conditions for pathological inputs
   - Implement resource limits for processing large inputs
   - Consider optimizing performance-critical code paths

3. Save and analyze crash-producing inputs:
   ```bash
   # Run the fuzzer and save crash-producing inputs
   ./build/bin/text_processor_fuzzer -max_total_time=300 -artifact_prefix=fuzz-
   
   # Reproduce and debug the specific crash
   ./build/bin/text_processor_fuzzer fuzz-crash-123abc
   ```

### Integration Test Failures

**Problem:** Integration tests fail even though individual unit tests pass.

**Solution:**
1. Check component interactions:
   - Verify interface contracts between components
   - Look for inconsistent state handling between components
   - Check for race conditions in multi-threaded code

2. Inspect test data flow:
   - Ensure test fixtures are properly set up for integration tests
   - Verify that mock objects behave consistently with real implementations
   - Check that initialization and cleanup are correct

3. Use logging for diagnosis:
   ```bash
   ./dev.py --test --filter=AnalyzerIntegrationTest --verbose --log-level=debug
   ```

### Test Coverage Problems

**Problem:** Test coverage reports show insufficient coverage.

**Solution:**
1. Identify uncovered areas:
   ```bash
   ./dev.py --coverage
   # Check the generated report in build/coverage/
   ```

2. Common coverage issues:
   - Error handling paths not tested
   - Edge cases and boundary conditions missed
   - Helper functions called only in specific contexts
   - Platform-specific code paths

3. Add targeted tests for uncovered code:
   ```cpp
   // Example: Testing error handling path
   TEST_F(TextProcessorTest, HandlesInvalidInput) {
       // Test specific error conditions
   }
   ```

4. Use branch coverage to find untested conditions:
   ```bash
   ./dev.py --coverage --branch
   ```

### Cross-Platform Testing Issues

**Problem:** Tests pass on one platform but fail on another.

**Solution:**
1. Look for platform-specific issues:
   - File path separators (\ vs /)
   - Line ending differences (CRLF vs LF)
   - Character encoding issues
   - Endianness differences

2. Check for platform-dependent code:
   - Use conditional compilation only when necessary
   - Implement platform abstraction layers
   - Use cross-platform libraries for OS-specific functionality

3. Test on multiple platforms:
   ```bash
   # On Windows
   cmake -G "Visual Studio 17 2022" -A x64 ..
   
   # On macOS
   cmake -G "Xcode" ..
   
   # On Linux
   cmake -G "Unix Makefiles" ..
   ```

### Test Data Issues

**Problem:** Tests fail because of problems with test data or fixtures.

**Solution:**
1. Check test data availability:
   - Ensure test data files are included in repository
   - Verify paths to test data are correct
   - Make sure test data is properly copied to build directory

2. Fix test fixture problems:
   - Verify SetUp() and TearDown() methods work correctly
   - Ensure fixtures initialize all required state
   - Check for leaks or side effects between tests

3. Use test data generators for consistent input:
   ```cpp
   // Example: Generate test data instead of hard-coding
   std::vector<std::string> GenerateTestCases() {
       std::vector<std::string> cases;
       // Add various test cases
       return cases;
   }
   
   TEST_P(ParameterizedTest, ProcessesAllCases) {
       // Test with generated data
   }
   ```

4. For data-dependent tests, use appropriate test doubles:
   - Mock objects for external services
   - Fake implementations for complex dependencies
   - Stubs for simple dependency replacement

## Getting Additional Help

If you're still experiencing issues after trying these solutions:

1. Check the [GitHub Issues](https://github.com/christimahu/blahajpi/issues) to see if others have reported similar problems
2. Create a new issue with:
   - The exact command you ran
   - The complete error message
   - Your operating system and environment details
   - Any logs or additional output
3. Reach out via the [contact form](https://blahajpi.com/contact)

---

<p align="center">
  <i>Remember that Blahaj PI is still in development, and your feedback helps make it better! 🦈</i>
</p>
