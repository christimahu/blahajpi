/**
 * @file visualize.cpp
 * @brief Implementation of the visualize command
 * 
 * This command generates word cloud visualizations from analysis results
 * or directly from text data, showing frequency patterns in the content.
 * 
 * @author Christi Mahu
 * @date 2024
 */

#include "bpicli/commands.hpp"
#include "bpicli/utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <algorithm>

namespace bpicli {

/**
 * @brief Handles the visualize command execution
 * 
 * Processes command-line arguments and generates text visualizations
 * based on content analysis.
 * 
 * @param args Command arguments
 * @param analyzer Analyzer instance
 * @return Exit code (0 on success)
 */
int handleVisualize(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer) {
    // Parse arguments
    auto parsedArgs = utils::parseArgs(args);
    
    // Check for required arguments
    if (parsedArgs.count("input") == 0) {
        utils::showError("Missing required argument: --input");
        std::cout << "Usage: blahajpi visualize --input <path> [--output <path>]" << std::endl;
        return 1;
    }
    
    // Get input path
    std::string inputPath = parsedArgs["input"];
    
    // Verify input file exists
    if (!std::filesystem::exists(inputPath)) {
        utils::showError("Input file not found: " + inputPath);
        return 1;
    }
    
    // Get output path
    std::string outputPath;
    if (parsedArgs.count("output") > 0) {
        outputPath = parsedArgs["output"];
    } else {
        // Default to same directory as input with _visualization suffix
        std::filesystem::path inputFilePath(inputPath);
        std::string stem = inputFilePath.stem().string();
        std::filesystem::path outputDir = inputFilePath.parent_path();
        outputPath = (outputDir / (stem + "_visualization.txt")).string();
    }
    
    // Get visualization parameters
    bool includeSafe = parsedArgs.count("all") > 0;
    
    // Read the input file
    std::vector<std::string> texts;
    std::vector<blahajpi::AnalysisResult> results;
    
    // Get column names for CSV files (if provided)
    std::string labelColumn = parsedArgs.count("label-column") > 0 ? 
                             parsedArgs["label-column"] : "sentiment_label";
    std::string textColumn = parsedArgs.count("text-column") > 0 ? 
                            parsedArgs["text-column"] : "tweet_text";
    
    // Debug output to verify column names
    std::cout << "Using column names for visualization: " << std::endl;
    std::cout << "  Label column: '" << labelColumn << "'" << std::endl;
    std::cout << "  Text column: '" << textColumn << "'" << std::endl;
    
    try {
        // Check if this is a CSV file with analysis results
        if (inputPath.ends_with(".csv")) {
            std::ifstream file(inputPath);
            if (!file.is_open()) {
                utils::showError("Failed to open file: " + inputPath);
                return 1;
            }
            
            // Debug - print first few lines of the file
            std::cout << "First few lines of the CSV file:" << std::endl;
            std::string line;
            int lineCount = 0;
            std::stringstream previewStream;
            while (std::getline(file, line) && lineCount < 3) {
                previewStream << line << std::endl;
                lineCount++;
            }
            std::cout << previewStream.str() << std::endl;
            
            // Reset file position
            file.clear();
            file.seekg(0);
            
            // Read header line
            std::getline(file, line);
            std::cout << "CSV header: " << line << std::endl;
            
            // Parse header to find column indices - with improved robustness
            std::vector<std::string> headers;
            std::stringstream headerStream(line);
            std::string headerField;
            
            // Handle quoted fields and commas properly
            bool inQuotedField = false;
            std::string currentField;
            
            for (char c : line) {
                if (c == '"') {
                    inQuotedField = !inQuotedField;
                } else if (c == ',' && !inQuotedField) {
                    // End of field
                    // Trim whitespace and quotes
                    currentField.erase(0, currentField.find_first_not_of(" \t\""));
                    size_t lastNonSpace = currentField.find_last_not_of(" \t\"");
                    if (lastNonSpace != std::string::npos) {
                        currentField.erase(lastNonSpace + 1);
                    }
                    headers.push_back(currentField);
                    currentField.clear();
                } else {
                    currentField += c;
                }
            }
            
            // Add the last field if not empty
            if (!currentField.empty()) {
                // Trim whitespace and quotes
                currentField.erase(0, currentField.find_first_not_of(" \t\""));
                size_t lastNonSpace = currentField.find_last_not_of(" \t\"");
                if (lastNonSpace != std::string::npos) {
                    currentField.erase(lastNonSpace + 1);
                }
                headers.push_back(currentField);
            }
            
            // Debug - print all headers
            std::cout << "Parsed headers (" << headers.size() << "):" << std::endl;
            for (size_t i = 0; i < headers.size(); i++) {
                std::cout << "  " << i << ": '" << headers[i] << "'" << std::endl;
            }
            
            // Find column indices using case-insensitive matching
            auto caseInsensitiveCompare = [](const std::string& a, const std::string& b) -> bool {
                if (a.size() != b.size()) return false;
                return std::equal(a.begin(), a.end(), b.begin(),
                                 [](char a, char b) {
                                     return std::tolower(a) == std::tolower(b);
                                 });
            };
            
            int textIndex = -1;
            int sentimentIndex = -1;
            
            for (size_t i = 0; i < headers.size(); ++i) {
                std::string header = headers[i];
                
                // Match with provided column names
                if (caseInsensitiveCompare(header, textColumn)) {
                    textIndex = static_cast<int>(i);
                    std::cout << "Found text column at index " << textIndex << std::endl;
                }
                
                if (caseInsensitiveCompare(header, labelColumn)) {
                    sentimentIndex = static_cast<int>(i);
                    std::cout << "Found label column at index " << sentimentIndex << std::endl;
                }
            }
            
            if (textIndex == -1) {
                utils::showError("Could not find text column in CSV");
                std::cout << "Available columns: ";
                for (const auto& header : headers) {
                    std::cout << "'" << header << "' ";
                }
                std::cout << std::endl;
                std::cout << "Looking for column: '" << textColumn << "'" << std::endl;
                return 1;
            }
            
            // Process data rows with more robust CSV parsing
            while (std::getline(file, line)) {
                std::vector<std::string> values;
                
                // Handle quoted fields and commas properly
                inQuotedField = false;
                currentField.clear();
                
                for (char c : line) {
                    if (c == '"') {
                        inQuotedField = !inQuotedField;
                    } else if (c == ',' && !inQuotedField) {
                        // End of field
                        values.push_back(currentField);
                        currentField.clear();
                    } else {
                        currentField += c;
                    }
                }
                
                // Add the last field
                values.push_back(currentField);
                
                // Skip lines with wrong number of columns
                if (values.size() <= static_cast<size_t>(std::max(textIndex, sentimentIndex))) {
                    continue;
                }
                
                // Clean text value (remove quotes)
                std::string text = values[textIndex];
                if (text.size() >= 2 && text.front() == '"' && text.back() == '"') {
                    text = text.substr(1, text.size() - 2);
                }
                
                // Extract and clean label
                std::string sentimentValue = "0";  // Default
                if (sentimentIndex >= 0) {
                    sentimentValue = values[sentimentIndex];
                    // Remove quotes and trim
                    sentimentValue.erase(std::remove(sentimentValue.begin(), sentimentValue.end(), '"'), sentimentValue.end());
                    sentimentValue.erase(std::remove(sentimentValue.begin(), sentimentValue.end(), ' '), sentimentValue.end());
                }
                
                // Check if we should filter by sentiment
                if (sentimentIndex != -1 && !includeSafe) {
                    // Only include harmful content (label 4 or "Harmful")
                    if (sentimentValue == "4" || sentimentValue == "Harmful") {
                        texts.push_back(text);
                    }
                } else {
                    texts.push_back(text);
                }
            }
        } else {
            // Read as regular text file
            std::string content = utils::loadFileContent(inputPath);
            
            // Split into lines
            std::istringstream iss(content);
            std::string line;
            while (std::getline(iss, line)) {
                if (!line.empty()) {
                    texts.push_back(line);
                }
            }
            
            // If not including safe content, analyze each text
            if (!includeSafe) {
                std::cout << "Analyzing texts for sentiment classification..." << std::endl;
                
                for (const auto& text : texts) {
                    blahajpi::AnalysisResult result = analyzer.analyze(text);
                    results.push_back(result);
                }
                
                // Filter for harmful content only
                std::vector<std::string> harmfulTexts;
                for (const auto& result : results) {
                    if (result.sentiment == "Harmful") {
                        harmfulTexts.push_back(result.cleanedText);
                    }
                }
                
                texts = harmfulTexts;
            }
        }
        
        // Generate visualization
        if (texts.empty()) {
            utils::showWarning("No content to visualize");
            return 0;
        }
        
        std::cout << "Generating visualization for " << texts.size() << " texts..." << std::endl;
        
        // Configure visualization parameters
        blahajpi::utils::CloudConfig config;
        
        // Apply parameters from command-line arguments
        if (parsedArgs.count("max-words") > 0) {
            try {
                config.maxWords = std::stoi(parsedArgs["max-words"]);
            } catch (...) {
                // Keep default if conversion fails
            }
        }
        
        if (parsedArgs.count("width") > 0) {
            try {
                config.width = std::stoi(parsedArgs["width"]);
            } catch (...) {
                // Keep default if conversion fails
            }
        }
        
        if (parsedArgs.count("height") > 0) {
            try {
                config.height = std::stoi(parsedArgs["height"]);
            } catch (...) {
                // Keep default if conversion fails
            }
        }
        
        config.useColor = parsedArgs.count("color") > 0;
        
        // Format type (wordcloud or bars)
        if (parsedArgs.count("format") > 0 && parsedArgs["format"] == "bars") {
            config.useBars = true;
        }
        
        // Show frequencies if requested
        config.showFrequencies = parsedArgs.count("show-frequencies") > 0;
        
        // Generate visualization
        bool success = analyzer.generateCustomVisualization(results, outputPath, !includeSafe, config);
        
        if (success) {
            utils::showSuccess("Visualization generated and saved to: " + outputPath);
            
            // Try to display the visualization in the terminal
            try {
                std::string visualization = utils::loadFileContent(outputPath);
                std::cout << "\n" << visualization << std::endl;
            } catch (...) {
                // Ignore errors
            }
        } else {
            utils::showError("Failed to generate visualization");
            return 1;
        }
        
        return 0;
    } catch (const std::exception& e) {
        utils::showError("Visualization failed: " + std::string(e.what()));
        return 1;
    }
}

} // namespace bpicli
