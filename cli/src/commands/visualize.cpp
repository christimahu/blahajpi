/**
 * @file visualize.cpp
 * @brief Implementation of the visualize command
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
    
    try {
        // Check if this is a CSV file with analysis results
        if (inputPath.ends_with(".csv")) {
            std::ifstream file(inputPath);
            if (!file.is_open()) {
                utils::showError("Failed to open file: " + inputPath);
                return 1;
            }
            
            std::string line;
            bool firstLine = true;
            
            // Find column indices
            int textIndex = -1;
            int sentimentIndex = -1;
            
            while (std::getline(file, line)) {
                std::vector<std::string> columns;
                std::string column;
                std::stringstream ss(line);
                bool inQuotes = false;
                
                while (ss.good()) {
                    char c = ss.get();
                    if (ss.eof()) break;
                    
                    if (c == '"') {
                        inQuotes = !inQuotes;
                    } else if (c == ',' && !inQuotes) {
                        columns.push_back(column);
                        column.clear();
                    } else {
                        column += c;
                    }
                }
                columns.push_back(column);
                
                if (firstLine) {
                    // Parse header to find column indices
                    for (size_t i = 0; i < columns.size(); ++i) {
                        std::string header = columns[i];
                        std::transform(header.begin(), header.end(), header.begin(), ::tolower);
                        
                        if (header == "text" || header == "content" || header == "file") {
                            textIndex = static_cast<int>(i);
                        } else if (header == "sentiment" || header == "label") {
                            sentimentIndex = static_cast<int>(i);
                        }
                    }
                    
                    if (textIndex == -1) {
                        utils::showError("Could not find text column in CSV");
                        return 1;
                    }
                    
                    firstLine = false;
                } else {
                    // Process data rows
                    if (textIndex < static_cast<int>(columns.size())) {
                        std::string text = columns[textIndex];
                        
                        // Check if we should filter by sentiment
                        if (sentimentIndex != -1 && !includeSafe) {
                            if (sentimentIndex < static_cast<int>(columns.size())) {
                                std::string sentiment = columns[sentimentIndex];
                                if (sentiment == "Harmful") {
                                    texts.push_back(text);
                                }
                            }
                        } else {
                            texts.push_back(text);
                        }
                    }
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
        int maxWords = 50;
        if (parsedArgs.count("max-words") > 0) {
            try {
                maxWords = std::stoi(parsedArgs["max-words"]);
            } catch (...) {
                // Ignore conversion errors
            }
        }
        
        int width = 80;
        if (parsedArgs.count("width") > 0) {
            try {
                width = std::stoi(parsedArgs["width"]);
            } catch (...) {
                // Ignore conversion errors
            }
        }
        
        int height = 20;
        if (parsedArgs.count("height") > 0) {
            try {
                height = std::stoi(parsedArgs["height"]);
            } catch (...) {
                // Ignore conversion errors
            }
        }
        
        bool useColor = parsedArgs.count("color") > 0;
        
        // Format type (wordcloud or bars)
        std::string format = "wordcloud";
        if (parsedArgs.count("format") > 0) {
            format = parsedArgs["format"];
        }
        
        // Generate visualization
        bool success = analyzer.generateVisualization(results, outputPath, !includeSafe);
        
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
