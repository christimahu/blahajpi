/**
 * @file utils.hpp
 * @brief Utility functions for the CLI application
 */

#pragma once

#include "blahajpi/analyzer.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace bpicli {
namespace utils {

/**
 * @brief Parse command-line arguments into a key-value map
 * @param args Arguments to parse
 * @return Map of argument keys to values
 */
std::unordered_map<std::string, std::string> parseArgs(const std::vector<std::string>& args);

/**
 * @brief Load content from a file
 * @param filePath Path to the file
 * @return Content as string
 * @throws std::runtime_error If file cannot be opened
 */
std::string loadFileContent(const std::string& filePath);

/**
 * @brief Save content to a file
 * @param content Content to save
 * @param filePath Path where to save
 * @return True if saving was successful
 */
bool saveToFile(const std::string& content, const std::string& filePath);

/**
 * @brief Print a formatted table
 * @param headers Column headers
 * @param rows Table rows
 * @param columnWidths Column widths (optional)
 */
void printTable(
    const std::vector<std::string>& headers,
    const std::vector<std::vector<std::string>>& rows,
    const std::vector<int>& columnWidths = {}
);

/**
 * @brief Format an analysis result for display
 * @param result Analysis result to format
 * @param verbose Whether to include detailed information
 * @return Formatted string
 */
std::string formatResult(const blahajpi::AnalysisResult& result, bool verbose = false);

/**
 * @brief Read input from file or standard input
 * @param prompt Prompt to display (if reading from stdin)
 * @param filePath Path to file (if reading from file)
 * @return Input content
 */
std::string readInput(const std::string& prompt = "", const std::string& filePath = "");

/**
 * @brief Get the default configuration path
 * @return Path to default configuration file
 */
std::string getDefaultConfigPath();

/**
 * @brief Show an error message
 * @param message Error message to display
 */
void showError(const std::string& message);

/**
 * @brief Show a success message
 * @param message Success message to display
 */
void showSuccess(const std::string& message);

/**
 * @brief Show a warning message
 * @param message Warning message to display
 */
void showWarning(const std::string& message);

/**
 * @brief Colorize text for terminal output
 * @param text Text to colorize
 * @param colorCode ANSI color code
 * @return Colorized text
 */
std::string colorize(const std::string& text, const std::string& colorCode);

/**
 * @brief Get the version of the application
 * @return Version string
 */
std::string getVersion();

/**
 * @brief Get the build date of the application
 * @return Build date string
 */
std::string getBuildDate();

} // namespace utils
} // namespace bpicli
