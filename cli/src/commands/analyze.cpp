/**
 * @file analyze.cpp
 * @brief Implementation of the analyze command
 */

#include "bpicli/commands.hpp"
#include "bpicli/utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace bpicli {

int handleAnalyze(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer) {
    // Parse arguments
    auto parsedArgs = utils::parseArgs(args);
    
    // Variables to hold input and options
    std::string inputText;
    std::string outputPath;
    bool verbose = parsedArgs.count("verbose") > 0 || parsedArgs.count("v") > 0;
    bool exitOnHarmful = parsedArgs.count("exit-on-harmful") > 0;
    
    // Get input text from file or command line argument
    if (parsedArgs.count("file") > 0) {
        std::string filePath = parsedArgs["file"];
        
        // Verify file exists
        if (!std::filesystem::exists(filePath)) {
            utils::showError("File not found: " + filePath);
            return 1;
        }
        
        try {
            inputText = utils::loadFileContent(filePath);
        } catch (const std::exception& e) {
            utils::showError("Failed to read file: " + std::string(e.what()));
            return 1;
        }
    } else if (parsedArgs.count("text") > 0) {
        inputText = parsedArgs["text"];
    } else {
        // No input specified, show usage and exit
        std::cout << "Usage: blahajpi analyze [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --file <path>         Analyze content from a file\n";
        std::cout << "  --text <text>         Analyze the provided text\n";
        std::cout << "  --output <path>       Save analysis result to a file\n";
        std::cout << "  --verbose, -v         Show detailed analysis information\n";
        std::cout << "  --exit-on-harmful     Return non-zero exit code if harmful content detected\n";
        return 1;
    }
    
    // Get output path if specified
    if (parsedArgs.count("output") > 0) {
        outputPath = parsedArgs["output"];
    }
    
    // Analyze the text
    try {
        // Perform analysis
        blahajpi::AnalysisResult result = analyzer.analyze(inputText);
        
        // Format the result
        std::string formattedResult = utils::formatResult(result, verbose);
        
        // Print result
        std::cout << "Analysis Result:\n" << formattedResult << std::endl;
        
        // Save to file if requested
        if (!outputPath.empty()) {
            if (utils::saveToFile(formattedResult, outputPath)) {
                utils::showSuccess("Results saved to: " + outputPath);
            } else {
                utils::showError("Failed to save results to: " + outputPath);
                return 1;
            }
        }
        
        // Return non-zero exit code if harmful content detected and exit-on-harmful is specified
        if (exitOnHarmful && result.sentiment == "Harmful") {
            return 2;  // Use exit code 2 to indicate harmful content
        }
        
        return 0;
    } catch (const std::exception& e) {
        utils::showError("Analysis failed: " + std::string(e.what()));
        return 1;
    }
}

} // namespace bpicli
