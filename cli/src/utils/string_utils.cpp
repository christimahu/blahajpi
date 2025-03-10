/**
 * @file string_utils.cpp
 * @brief Implementation of string utility functions
 */

#include "bpicli/utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <filesystem>
#include <unordered_map>
#include <cctype>
#include <cstdlib>

// Version and build date from main.cpp
extern const char* VERSION;
extern const char* BUILD_DATE;

namespace bpicli {
namespace utils {

std::unordered_map<std::string, std::string> parseArgs(const std::vector<std::string>& args) {
    std::unordered_map<std::string, std::string> parsedArgs;
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        // Check if this is an option (starts with -- or -)
        if (arg.substr(0, 2) == "--" || (arg[0] == '-' && arg.length() == 2)) {
            std::string key = arg.substr(arg[0] == '-' && arg[1] == '-' ? 2 : 1);
            
            // Remove trailing '=' if present
            size_t equalsPos = key.find('=');
            if (equalsPos != std::string::npos) {
                std::string value = key.substr(equalsPos + 1);
                key = key.substr(0, equalsPos);
                parsedArgs[key] = value;
            }

std::string formatResult(const blahajpi::AnalysisResult& result, bool verbose) {
    std::stringstream output;
    
    // Format the basic information
    output << "Sentiment: " << colorize(result.sentiment, 
                                      result.sentiment == "Harmful" ? "\033[1;31m" : "\033[1;32m")
           << std::endl;
    
    output << "Harm Score: " << result.harmScore << std::endl;
    output << "Confidence: " << (result.confidence * 100) << "%" << std::endl;
    
    // Add explanation
    output << "Explanation: " << result.explanation << std::endl;
    
    // Add key terms if present
    if (!result.keyTerms.empty()) {
        output << "Key Terms: ";
        for (size_t i = 0; i < result.keyTerms.size(); ++i) {
            if (i > 0) output << ", ";
            output << result.keyTerms[i];
        }
        output << std::endl;
    }
    
    // Add detailed information if verbose
    if (verbose) {
        output << "\nDetailed Information:" << std::endl;
        output << "Original Text: " << result.text << std::endl;
        output << "Cleaned Text: " << result.cleanedText << std::endl;
    }
    
    return output.str();
}

std::string readInput(const std::string& prompt, const std::string& filePath) {
    if (!filePath.empty()) {
        return loadFileContent(filePath);
    }
    
    if (!prompt.empty()) {
        std::cout << prompt << std::endl;
    }
    
    std::stringstream buffer;
    std::string line;
    
    while (std::getline(std::cin, line)) {
        buffer << line << std::endl;
    }
    
    // Reset cin for future input
    std::cin.clear();
    
    return buffer.str();
}

std::string getDefaultConfigPath() {
    // Look in common locations for configuration
    std::vector<std::string> searchPaths = {
        "./configs/default.conf",
        "../configs/default.conf",
        "../../configs/default.conf",
        "./default.conf"
    };
    
    for (const auto& path : searchPaths) {
        if (std::filesystem::exists(path)) {
            return path;
        }
    }
    
    return "";
}

void showError(const std::string& message) {
    std::cerr << colorize("Error: " + message, "\033[1;31m") << std::endl;
}

void showSuccess(const std::string& message) {
    std::cout << colorize("Success: " + message, "\033[1;32m") << std::endl;
}

void showWarning(const std::string& message) {
    std::cout << colorize("Warning: " + message, "\033[1;33m") << std::endl;
}

std::string colorize(const std::string& text, const std::string& colorCode) {
    // Check if colors are supported and enabled
    static bool useColors = []() {
        const char* noColor = std::getenv("NO_COLOR");
        const char* term = std::getenv("TERM");
        
        // Don't use colors if NO_COLOR is set or TERM is dumb
        if (noColor != nullptr || (term != nullptr && std::string(term) == "dumb")) {
            return false;
        }
        
        // Use colors by default
        return true;
    }();
    
    if (!useColors) {
        return text;
    }
    
    return colorCode + text + "\033[0m";
}

std::string getVersion() {
    return VERSION;
}

std::string getBuildDate() {
    return BUILD_DATE;
}

} // namespace utils
} // namespace bpicli else if (i + 1 < args.size() && args[i + 1][0] != '-') {
                // The next argument is a value (doesn't start with -)
                parsedArgs[key] = args[++i];
            } else {
                // Flag without value
                parsedArgs[key] = "true";
            }
        } else if (arg.substr(0, 1) == "-" && arg.length() > 2) {
            // Combined short options like -abc
            for (size_t j = 1; j < arg.length(); ++j) {
                std::string key(1, arg[j]);
                parsedArgs[key] = "true";
            }
        } else {
            // Positional argument (would be handled by the specific command)
        }
    }
    
    return parsedArgs;
}

std::string loadFileContent(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool saveToFile(const std::string& content, const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return !file.fail();
}

void printTable(
    const std::vector<std::string>& headers,
    const std::vector<std::vector<std::string>>& rows,
    const std::vector<int>& columnWidths) {
    
    // Calculate column widths if not provided
    std::vector<int> widths = columnWidths;
    
    if (widths.empty()) {
        widths.resize(headers.size(), 0);
        
        // Calculate width for each column based on content
        for (size_t i = 0; i < headers.size(); ++i) {
            widths[i] = std::max(widths[i], static_cast<int>(headers[i].length()));
        }
        
        for (const auto& row : rows) {
            for (size_t i = 0; i < row.size() && i < widths.size(); ++i) {
                widths[i] = std::max(widths[i], static_cast<int>(row[i].length()));
            }
        }
        
        // Add some padding
        for (auto& width : widths) {
            width += 2;
        }
    }
    
    // Print header
    for (size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::left << std::setw(widths[i]) << headers[i];
    }
    std::cout << std::endl;
    
    // Print separator
    for (size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::string(widths[i], '-');
    }
    std::cout << std::endl;
    
    // Print rows
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size() && i < widths.size(); ++i) {
            std::cout << std::left << std::setw(widths[i]) << row[i];
        }
        std::cout << std::endl;
    }
}