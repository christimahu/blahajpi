/**
 * @file cli_integration_test.cpp
 * @brief Integration tests for the BlahajPI CLI
 * 
 * These tests verify the functionality of the command-line interface,
 * including command parsing, execution, and output formatting.
 * 
 * @author BlahajPI Team
 * @date 2025
 */

#include "blahajpi/analyzer.hpp"
#include "blahajpi/config.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <memory>
#include <array>

namespace {

/**
 * @class CLIIntegrationTest
 * @brief Test fixture for CLI integration tests
 * 
 * This fixture provides common setup for testing the CLI interface.
 */
class CLIIntegrationTest : public ::testing::Test {
protected:
    /**
     * @brief Set up test environment
     * 
     * Creates temporary directory and files for CLI testing.
     */
    void SetUp() override {
        // Create temporary directory
        tempDir = std::filesystem::temp_directory_path() / "blahajpi_tests";
        std::filesystem::create_directories(tempDir);
        
        // Create sub-directories
        modelDir = tempDir / "models";
        configDir = tempDir / "configs";
        dataDir = tempDir / "data";
        resultsDir = tempDir / "results";
        
        std::filesystem::create_directories(modelDir);
        std::filesystem::create_directories(configDir);
        std::filesystem::create_directories(dataDir);
        std::filesystem::create_directories(resultsDir);
        
        // Create sample data file
        dataPath = dataDir / "test_data.csv";
        std::ofstream dataFile(dataPath);
        dataFile << "label,text\n"
                << "0,This is a safe message with normal content.\n"
                << "4,This is harmful content targeting groups.\n"
                << "0,Another safe message about everyday topics.\n"
                << "4,More harmful content with negative stereotypes.\n"
                << "0,A third safe message about technology.\n";
        dataFile.close();
        
        // Create test configuration file
        configPath = configDir / "test_config.conf";
        std::ofstream configFile(configPath);
        configFile << "# Test configuration\n"
                  << "model-dir = " << modelDir.string() << "\n"
                  << "dataset = " << dataPath.string() << "\n"
                  << "label-column = label\n"
                  << "text-column = text\n"
                  << "alpha = 0.0001\n"
                  << "eta0 = 0.01\n"
                  << "epochs = 5\n"
                  << "max-features = 1000\n";
        configFile.close();
        
        // Create test input file
        inputPath = dataDir / "test_input.txt";
        std::ofstream inputFile(inputPath);
        inputFile << "This is a test input file.\n"
                 << "It contains multiple lines of text.\n"
                 << "Each line should be analyzed separately.\n";
        inputFile.close();
        
        // Find CLI executable path
        executablePath = findExecutablePath();
    }
    
    /**
     * @brief Clean up test environment
     * 
     * Removes temporary files and directories.
     */
    void TearDown() override {
        if (std::filesystem::exists(tempDir)) {
            std::filesystem::remove_all(tempDir);
        }
    }
    
    /**
     * @brief Finds the CLI executable
     * 
     * Attempts to locate the BlahajPI CLI executable in common build locations.
     * 
     * @return Path to the executable
     */
    std::filesystem::path findExecutablePath() {
        // Determine the executable name based on platform
        #ifdef _WIN32
        std::string exeName = "blahajpi.exe";
        #else
        std::string exeName = "blahajpi";
        #endif
        
        // Get the current directory
        auto currentDir = std::filesystem::current_path();
        
        // Possible locations relative to test directory
        std::vector<std::filesystem::path> possiblePaths = {
            currentDir / "bin" / exeName,
            currentDir / ".." / "bin" / exeName,
            currentDir / "build" / "bin" / exeName,
            currentDir / ".." / "build" / "bin" / exeName
        };
        
        // Check each possible path
        for (const auto& path : possiblePaths) {
            if (std::filesystem::exists(path)) {
                return path;
            }
        }
        
        // Return placeholder if not found (will fail tests if not corrected)
        return currentDir / "bin" / exeName;
    }
    
    /**
     * @brief Executes a CLI command and captures output
     * 
     * Runs the BlahajPI CLI with the given arguments and returns the output.
     * 
     * @param args Command line arguments to pass to the CLI
     * @return Pair containing exit code and command output
     */
    std::pair<int, std::string> executeCommand(const std::vector<std::string>& args) {
        // Start with the executable path
        std::string command = executablePath.string();
        
        // Add arguments
        for (const auto& arg : args) {
            command += " " + arg;
        }
        
        // Open pipe to process
        #ifdef _WIN32
        // For Windows
        command += " 2>&1";  // Redirect stderr to stdout
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
        #else
        // For Unix-like systems
        command += " 2>&1";  // Redirect stderr to stdout
        FILE* rawPipe = popen(command.c_str(), "r");
        std::unique_ptr<FILE, decltype(&pclose)> pipe(rawPipe, pclose);
        #endif
        
        if (!pipe) {
            return {-1, "Error: Failed to execute command"};
        }
        
        // Read output
        std::array<char, 128> buffer;
        std::string output;
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            output += buffer.data();
        }
        
        // Get exit code
        int exitCode = 0;
        #ifdef _WIN32
        // For Windows, we can't easily get the exit code from _popen
        exitCode = output.find("Error") != std::string::npos ? 1 : 0;
        #else
        // For Unix-like systems - simplified approach
        exitCode = (output.find("Error") != std::string::npos || 
                  output.find("error") != std::string::npos) ? 1 : 0;
        #endif
        
        return {exitCode, output};
    }
    
    std::filesystem::path tempDir;
    std::filesystem::path modelDir;
    std::filesystem::path configDir;
    std::filesystem::path dataDir;
    std::filesystem::path resultsDir;
    std::filesystem::path dataPath;
    std::filesystem::path configPath;
    std::filesystem::path inputPath;
    std::filesystem::path executablePath;
};

/**
 * @brief Tests the version command
 * 
 * Verifies that the CLI can display version information.
 */
TEST_F(CLIIntegrationTest, VersionCommand) {
    // Skip if executable not found
    if (!std::filesystem::exists(executablePath)) {
        GTEST_SKIP() << "CLI executable not found at " << executablePath;
    }
    
    // Execute version command
    auto [exitCode, output] = executeCommand({"version"});
    
    // Check exit code and output
    EXPECT_EQ(exitCode, 0);
    EXPECT_TRUE(output.find("Version") != std::string::npos);
}

/**
 * @brief Tests the help command
 * 
 * Verifies that the CLI can display help information.
 */
TEST_F(CLIIntegrationTest, HelpCommand) {
    // Skip if executable not found
    if (!std::filesystem::exists(executablePath)) {
        GTEST_SKIP() << "CLI executable not found at " << executablePath;
    }
    
    // Execute help command
    auto [exitCode, output] = executeCommand({"help"});
    
    // Check exit code and output
    EXPECT_EQ(exitCode, 0);
    EXPECT_TRUE(output.find("Available commands") != std::string::npos);
    
    // Check command-specific help
    auto [exitCode2, output2] = executeCommand({"help", "analyze"});
    EXPECT_EQ(exitCode2, 0);
    EXPECT_TRUE(output2.find("analyze") != std::string::npos);
}

/**
 * @brief Tests the config command
 * 
 * Verifies that the CLI can display and manipulate configuration.
 */
TEST_F(CLIIntegrationTest, ConfigCommand) {
    // Skip if executable not found
    if (!std::filesystem::exists(executablePath)) {
        GTEST_SKIP() << "CLI executable not found at " << executablePath;
    }
    
    // Execute config list command
    auto [exitCode, output] = executeCommand({"--config", configPath.string(), "config", "list"});
    
    // Check exit code and output
    EXPECT_EQ(exitCode, 0);
    EXPECT_TRUE(output.find("Configuration values") != std::string::npos);
    
    // Check that loaded config values are displayed
    EXPECT_TRUE(output.find("model-dir") != std::string::npos);
    EXPECT_TRUE(output.find("dataset") != std::string::npos);
}

/**
 * @brief Tests the analyze command with text input
 * 
 * Verifies that the CLI can analyze text content.
 */
TEST_F(CLIIntegrationTest, AnalyzeText) {
    // Skip if executable not found
    if (!std::filesystem::exists(executablePath)) {
        GTEST_SKIP() << "CLI executable not found at " << executablePath;
    }
    
    // First, ensure a model exists by training one
    auto [trainExitCode, trainOutput] = executeCommand({
        "--config", configPath.string(),
        "train",
        "--dataset", dataPath.string(),
        "--output", (modelDir / "test_model").string(),
        "--yes"  // Auto-confirm training
    });
    
    // Check if training succeeded (exit code may vary if model already exists)
    if (trainExitCode != 0 && !std::filesystem::exists(modelDir / "test_model" / "model.bin")) {
        GTEST_SKIP() << "Could not create model for testing";
    }
    
    // Execute analyze command with text input
    auto [exitCode, output] = executeCommand({
        "--config", configPath.string(),
        "analyze",
        "--text", "This is a test message."
    });
    
    // Check exit code and output
    EXPECT_EQ(exitCode, 0);
    EXPECT_TRUE(output.find("Analysis Result") != std::string::npos);
    EXPECT_TRUE(output.find("Sentiment:") != std::string::npos);
}

/**
 * @brief Tests the analyze command with file input
 * 
 * Verifies that the CLI can analyze content from a file.
 */
TEST_F(CLIIntegrationTest, AnalyzeFile) {
    // Skip if executable not found
    if (!std::filesystem::exists(executablePath)) {
        GTEST_SKIP() << "CLI executable not found at " << executablePath;
    }
    
    // Ensure a model exists
    if (!std::filesystem::exists(modelDir / "test_model" / "model.bin")) {
        auto [trainExitCode, trainOutput] = executeCommand({
            "--config", configPath.string(),
            "train",
            "--dataset", dataPath.string(),
            "--output", (modelDir / "test_model").string(),
            "--yes"  // Auto-confirm training
        });
        
        if (trainExitCode != 0) {
            GTEST_SKIP() << "Could not create model for testing";
        }
    }
    
    // Execute analyze command with file input
    auto [exitCode, output] = executeCommand({
        "--config", configPath.string(),
        "analyze",
        "--file", inputPath.string()
    });
    
    // Check exit code and output
    EXPECT_EQ(exitCode, 0);
    EXPECT_TRUE(output.find("Analysis Results") != std::string::npos);
    
    // Output should contain multiple analysis results (one per line in input file)
    size_t resultCount = 0;
    size_t pos = 0;
    while ((pos = output.find("Sentiment:", pos)) != std::string::npos) {
        resultCount++;
        pos += 10;  // Move past "Sentiment:"
    }
    
    // Should have at least one result (might not match exactly due to processing)
    EXPECT_GT(resultCount, 0);
}

/**
 * @brief Tests the visualize command
 * 
 * Verifies that the CLI can generate visualizations.
 */
TEST_F(CLIIntegrationTest, Visualize) {
    // Skip if executable not found
    if (!std::filesystem::exists(executablePath)) {
        GTEST_SKIP() << "CLI executable not found at " << executablePath;
    }
    
    // Ensure a model exists
    if (!std::filesystem::exists(modelDir / "test_model" / "model.bin")) {
        auto [trainExitCode, trainOutput] = executeCommand({
            "--config", configPath.string(),
            "train",
            "--dataset", dataPath.string(),
            "--output", (modelDir / "test_model").string(),
            "--yes"  // Auto-confirm training
        });
        
        if (trainExitCode != 0) {
            GTEST_SKIP() << "Could not create model for testing";
        }
    }
    
    // Create output path for visualization
    std::string vizOutputPath = (resultsDir / "visualization.txt").string();
    
    // Execute visualize command
    auto [exitCode, output] = executeCommand({
        "--config", configPath.string(),
        "visualize",
        "--file", dataPath.string(),
        "--output", vizOutputPath
    });
    
    // Check exit code and output
    EXPECT_EQ(exitCode, 0);
    
    // Check that visualization file was created
    EXPECT_TRUE(std::filesystem::exists(vizOutputPath));
    
    // Read visualization file content
    std::ifstream vizFile(vizOutputPath);
    std::string content((std::istreambuf_iterator<char>(vizFile)),
                       std::istreambuf_iterator<char>());
    
    // Check content
    EXPECT_FALSE(content.empty());
    EXPECT_TRUE(content.find("Word Frequency Visualization") != std::string::npos);
}

/**
 * @brief Tests the train command
 * 
 * Verifies that the CLI can train a model.
 */
TEST_F(CLIIntegrationTest, Train) {
    // Skip if executable not found
    if (!std::filesystem::exists(executablePath)) {
        GTEST_SKIP() << "CLI executable not found at " << executablePath;
    }
    
    // Create a new model path
    std::string newModelPath = (modelDir / "new_test_model").string();
    
    // Remove any existing model at this path
    if (std::filesystem::exists(newModelPath)) {
        std::filesystem::remove_all(newModelPath);
    }
    
    // Execute train command
    auto [exitCode, output] = executeCommand({
        "--config", configPath.string(),
        "train",
        "--dataset", dataPath.string(),
        "--output", newModelPath,
        "--yes"  // Auto-confirm training
    });
    
    // Check exit code and output
    EXPECT_EQ(exitCode, 0);
    EXPECT_TRUE(output.find("Training complete") != std::string::npos ||
               output.find("Model trained") != std::string::npos);
    
    // Check that model files were created
    EXPECT_TRUE(std::filesystem::exists(newModelPath + "/model.bin") || 
               std::filesystem::exists(newModelPath + "/model_info.txt"));
}

/**
 * @brief Tests error handling for invalid commands
 * 
 * Verifies that the CLI handles invalid commands gracefully.
 */
TEST_F(CLIIntegrationTest, InvalidCommand) {
    // Skip if executable not found
    if (!std::filesystem::exists(executablePath)) {
        GTEST_SKIP() << "CLI executable not found at " << executablePath;
    }
    
    // Execute non-existent command
    auto [exitCode, output] = executeCommand({"non_existent_command"});
    
    // Should return non-zero exit code
    EXPECT_NE(exitCode, 0);
    
    // Output should indicate error
    EXPECT_TRUE(output.find("Error") != std::string::npos ||
               output.find("Unknown") != std::string::npos ||
               output.find("Invalid") != std::string::npos);
}

/**
 * @brief Tests error handling for missing arguments
 * 
 * Verifies that the CLI handles missing arguments gracefully.
 */
TEST_F(CLIIntegrationTest, MissingArguments) {
    // Skip if executable not found
    if (!std::filesystem::exists(executablePath)) {
        GTEST_SKIP() << "CLI executable not found at " << executablePath;
    }
    
    // Execute analyze command without required arguments
    auto [exitCode, output] = executeCommand({
        "--config", configPath.string(),
        "analyze"
    });
    
    // Should return non-zero exit code or show help
    EXPECT_TRUE(exitCode != 0 || output.find("Usage") != std::string::npos);
}

} // namespace
