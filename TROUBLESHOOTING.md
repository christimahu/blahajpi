# 🛠️ Blahaj PI Troubleshooting Guide 🛠️

Welcome to the Blahaj PI troubleshooting guide! If you're encountering issues with our friendly shark detective, this document will help you resolve common problems. 🦈

## 📋 Table of Contents

- [Build Issues](#build-issues)
- [Model Training Issues](#model-training-issues)
- [Analysis Issues](#analysis-issues)
- [Visualization Issues](#visualization-issues)
- [Command-Line Interface Issues](#command-line-interface-issues)
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

## Getting Additional Help

If you're still experiencing issues after trying these solutions:

1. Check the [GitHub Issues](https://github.com/christimahu/blahajpi/issues) to see if others have reported similar problems
2. Create a new issue with:
   - The exact command you ran
   - The complete error message
   - Your operating system and environment details
   - Any logs or additional output
3. Reach out via Bluesky [@christimahu](https://bsky.app/profile/christimahu.bsky.social)

---

<p align="center">
  <i>Remember that Blahaj PI is still in development, and your feedback helps make it better! 🦈</i>
</p>
