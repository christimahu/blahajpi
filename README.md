# 🦈 Blahaj PI: Your Friendly Shark Detective 🔍

*Keeping social waters safe for our trans and non-binary friends!* 🏳️‍⚧️ 🌈 💜

[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)
[![C++: 23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Cuddles: Maximum](https://img.shields.io/badge/Cuddles-Maximum-ff69b4.svg)](https://blahajpi.com)

## 💜 What is Blahaj PI? 💜

Blahaj PI (Private Investigator) is a protective blue shark 🦈 who swims through social media to identify potentially harmful content targeting transgender and non-binary individuals. In these challenging times, when our community faces increasing hostility, Blahaj is here to help create safer online spaces. 

Just like the cuddly IKEA shark that has become a beloved symbol in our community, Blahaj PI offers comfort through protection - standing guard against bullying, harassment, and harmful rhetoric while you navigate online spaces.

**Currently in development:** While our ultimate goal is to scan live social media, this initial MVP version only analyzes the included example dataset. Future versions will support processing content from specific social media accounts and additional data formats.

## ✨ Features ✨

- 🧠 **Sentiment Analysis:** Identifies potentially harmful content with machine learning
- 📊 **Content Classification:** Labels text as "Safe" or "Harmful" with confidence scores
- 📝 **Detailed Explanations:** Provides reasoning behind classifications
- 📈 **Visualization Tools:** Generates word clouds to understand content patterns
- 🧮 **Batch Processing:** Analyzes multiple files for efficient processing
- ⚙️  **Configurable Models:** Different configurations for speed, accuracy, or balanced performance
- 🔧 **Command Line Interface:** Easy to use for both beginners and experts
- 🧪 **Comprehensive Testing:** Ensures reliable performance

## 📋 Getting Started 📋

### Prerequisites

- C++23 compatible compiler
- CMake 3.14 or higher
- Python 3.6+ (for build scripts)

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/christimahu/blahajpi.git
   cd blahajpi
   ```

2. **Build the project**
   ```bash
   # Using the development script (recommended)
   ./dev.py --build

   # Or manually with CMake
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

## 🚀 Full Tutorial: Using Blahaj PI 🚀

### Step 1: Build the Project 🔨

First, we need to build Blahaj PI:

```bash
./dev.py --build
```

This compiles the C++ code and creates the executable in the `build/bin` directory. Tests run automatically during build to ensure everything works correctly.

### Step 2: Train Your Shark Detective 🦈🎓

Before Blahaj can analyze text, it needs training on what harmful content looks like:

```bash
# Using the provided example data
./dev.py --run -- train --dataset data/examples/twitter_example.csv --output models/my_model
```

This trains a machine learning model using labeled examples from the included sample dataset:
- `0` in the dataset represents safe content
- `4` represents potentially harmful content

The model learns patterns associated with each category and saves the trained model to the specified location.

### Step 3: Analyze Text 🔍

Now Blahaj is ready to analyze text from the example dataset:

```bash
# Analyze the sample dataset
./dev.py --run -- analyze --file data/examples/twitter_example.csv
```

Blahaj will analyze the content and tell you whether it appears harmful or safe, along with a confidence score and explanation. Results are stored in the `results` directory at the project root.

### Step 4: Visualize Results 📊

Generate visualizations to understand patterns in analyzed content:

```bash
./dev.py --run -- visualize --input data/examples/twitter_example.csv --output results/visualization.txt
```

This creates a word cloud showing the most frequent terms in the analyzed content, helping you identify patterns of harmful language.

### Step 5: Batch Processing 📚

For demonstration purposes, you can see how batch processing would work:

```bash
./dev.py --run -- batch --input-dir data/examples
```

This processes all files in the example directory and provides a summary report.

## 💫 Quick Start with Demo 💫

For a guided tour of all features, simply run:

```bash
./dev.py --demo
```

This will:
1. Clean any previous builds and results
2. Build the project if needed
3. Train a model using example data
4. Demonstrate text analysis
5. Show visualization capabilities
6. Explain configuration options

The demo is perfect for first-time users or when showcasing the tool to others!

## ⚙️ Configuration Options ⚙️

Blahaj PI comes with several configuration presets in the `configs/` directory:

- `default.conf`: Balanced settings for general use
- `fast_model.conf`: Optimized for speed (when you need quick results)
- `accurate_model.conf`: Optimized for maximum accuracy (when precision matters most)
- `production_model.conf`: Balanced for deployment environments

To use a specific configuration:

```bash
./dev.py --run -- --config configs/fast_model.conf analyze --file data/examples/twitter_example.csv
```

## 🔧 Command Reference 🔧

| Command | Description | Example |
|---------|-------------|---------|
| `analyze` | Analyze text for harmful content | `analyze --file data/examples/twitter_example.csv` |
| `train` | Train a new sentiment analysis model | `train --dataset data/examples/twitter_example.csv --output models/custom` |
| `batch` | Process multiple files | `batch --input-dir data/examples` |
| `visualize` | Generate word cloud visualization | `visualize --input data/examples/twitter_example.csv --output results/cloud.txt` |
| `config` | Manage configuration settings | `config list` |
| `help` | Show command information | `help analyze` |
| `version` | Display version information | `version` |

For detailed help on any command:

```bash
./dev.py --run -- help <command>
```

## 🦈 Why "Blahaj PI"? 🦈

The name combines two significant elements:

1. **Blahaj** (pronounced "blah-haj") is the name of IKEA's blue shark plush toy that has become a beloved symbol in the trans community - offering comfort, support, and a sense of safety.

2. **PI** stands for "Private Investigator" - like the detective shark on our logo, complete with magnifying glass and trenchcoat, investigating to keep online spaces safe.

Together, they represent our mission: a friendly guardian helping to create safer spaces online for trans and non-binary individuals. 🏳️‍⚧️

## 📚 Additional Documentation 📚

- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Solutions for common issues
- [TESTING.md](TESTING.md) - Information about the test suite and how to run tests
- [CONTRIBUTING.md](CONTRIBUTING.md) - Guidelines for contributing to the project
- [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) - Our community standards

## 🤝 Contributing 🤝

We welcome contributions from everyone, especially those who understand the importance of creating safe spaces for the trans and non-binary community. See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

All contributors are expected to follow our [Code of Conduct](CODE_OF_CONDUCT.md).

## 📜 License 📜

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 💕 Support & Community 💕

If you need help, have suggestions, or just want to connect:

- 🐙 [GitHub Issues](https://github.com/christimahu/blahajpi/issues) for bugs and features
- 🌐 [BlahajPI.com](https://blahajpi.com) for documentation and community resources
- 🔷 [@christimahu on Bluesky](https://bsky.app/profile/christimahu.bsky.social) for direct contact

---

<p align="center">
  <i>Made with 💜 for the trans and non-binary community</i><br>
  <i>Remember: you are valid, you are loved, and you deserve to be safe online.</i><br>
  <i>Blahaj is swimming by your side! 🦈</i>
</p>
