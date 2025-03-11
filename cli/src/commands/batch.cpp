/**
 * @file batch.cpp
 * @brief Implementation of the batch command
 */

#include "bpicli/commands.hpp"
#include "bpicli/utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <chrono>

namespace bpicli {

int handleBatch(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer) {
    // Parse arguments
    auto parsedArgs = utils::parseArgs(args);
    
    // Variables to hold input and options
    std::vector<std::string> filePaths;
    std::string outputPath;
    bool recursive = parsedArgs.count("recursive") > 0;
    bool showHarmful = parsedArgs.count("show-harmful") > 0;
    
    // Get input source
    if (parsedArgs.count("input-dir") > 0) {
        std::string dirPath = parsedArgs["input-dir"];
        
        // Verify directory exists
        if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) {
            utils::showError("Directory not found: " + dirPath);
            return 1;
        }
        
        // Collect files from directory
        try {
            if (recursive) {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
                    if (entry.is_regular_file()) {
                        filePaths.push_back(entry.path().string());
                    }
                }
            } else {
                for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
                    if (entry.is_regular_file()) {
                        filePaths.push_back(entry.path().string());
                    }
                }
            }
        } catch (const std::exception& e) {
            utils::showError("Error reading directory: " + std::string(e.what()));
            return 1;
        }
    } else if (parsedArgs.count("input-file") > 0) {
        std::string listPath = parsedArgs["input-file"];
        
        // Verify file exists
        if (!std::filesystem::exists(listPath)) {
            utils::showError("File not found: " + listPath);
            return 1;
        }
        
        // Read file paths from list file
        try {
            std::ifstream file(listPath);
            std::string line;
            
            while (std::getline(file, line)) {
                // Skip empty lines and comments
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                
                // Trim whitespace
                line.erase(0, line.find_first_not_of(" \t"));
                line.erase(line.find_last_not_of(" \t") + 1);
                
                if (!line.empty()) {
                    filePaths.push_back(line);
                }
            }
        } catch (const std::exception& e) {
            utils::showError("Error reading file list: " + std::string(e.what()));
            return 1;
        }
    } else {
        // No input specified, show usage and exit
        std::cout << "Usage: blahajpi batch [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --input-dir <dir>     Process all files in a directory\n";
        std::cout << "  --input-file <path>   Process files listed in a file (one per line)\n";
        std::cout << "  --output <path>       Save batch analysis results to a file\n";
        std::cout << "  --recursive           Process files in subdirectories (with --input-dir)\n";
        std::cout << "  --show-harmful        Display detailed report for harmful content\n";
        return 1;
    }
    
    // Get output path if specified
    if (parsedArgs.count("output") > 0) {
        outputPath = parsedArgs["output"];
    }
    
    // Exit if no files were found
    if (filePaths.empty()) {
        utils::showWarning("No files found to process.");
        return 0;
    }
    
    std::cout << "Processing " << filePaths.size() << " files..." << std::endl;
    
    // Prepare result storage
    std::vector<std::pair<std::string, blahajpi::AnalysisResult>> results;
    results.reserve(filePaths.size());
    
    int harmfulCount = 0;
    int errorCount = 0;
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Process each file
    for (size_t i = 0; i < filePaths.size(); ++i) {
        const auto& path = filePaths[i];
        
        // Show progress
        if (i % 10 == 0 || i == filePaths.size() - 1) {
            std::cout << "\rProcessing file " << (i + 1) << " of " << filePaths.size() 
                     << " (" << (i * 100 / filePaths.size()) << "%)..." << std::flush;
        }
        
        try {
            // Check if file exists
            if (!std::filesystem::exists(path)) {
                std::cerr << "\nWarning: File not found: " << path << std::endl;
                errorCount++;
                continue;
            }
            
            // Read file content
            std::string content = utils::loadFileContent(path);
            
            // Analyze the content
            blahajpi::AnalysisResult result = analyzer.analyze(content);
            
            // Store the result
            results.emplace_back(path, result);
            
            // Count harmful content
            if (result.sentiment == "Harmful") {
                harmfulCount++;
            }
        } catch (const std::exception& e) {
            std::cerr << "\nError processing file " << path << ": " << e.what() << std::endl;
            errorCount++;
        }
    }
    
    // Calculate elapsed time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << "\nCompleted in " << duration.count() << " seconds." << std::endl;
    
    // Print summary
    std::cout << "\nAnalysis Summary:\n";
    std::cout << "Total files: " << filePaths.size() << std::endl;
    std::cout << "Harmful content: " << harmfulCount << " files (" 
              << (filePaths.size() > 0 ? (harmfulCount * 100 / filePaths.size()) : 0) << "%)" << std::endl;
    std::cout << "Safe content: " << (filePaths.size() - harmfulCount - errorCount) 
              << " files" << std::endl;
    
    if (errorCount > 0) {
        std::cout << "Errors: " << errorCount << " files" << std::endl;
    }
    
    // Show harmful content details if requested
    if (showHarmful && harmfulCount > 0) {
        std::cout << "\nHarmful Content Details:\n";
        std::cout << "------------------------\n";
        
        for (const auto& [path, result] : results) {
            if (result.sentiment == "Harmful") {
                std::cout << "File: " << path << std::endl;
                std::cout << "Score: " << result.harmScore << std::endl;
                std::cout << "Explanation: " << result.explanation << std::endl;
                std::cout << "------------------------\n";
            }
        }
    }
    
    // Save results to file if requested
    if (!outputPath.empty()) {
        try {
            std::ofstream outFile(outputPath);
            if (!outFile.is_open()) {
                utils::showError("Failed to open output file: " + outputPath);
                return 1;
            }
            
            // Write CSV header
            outFile << "file,sentiment,score,confidence,explanation\n";
            
            // Write results
            for (const auto& [path, result] : results) {
                // Escape commas and quotes in fields
                std::string escapedPath = path;
                std::string escapedExplanation = result.explanation;
                
                // Replace quotes with double quotes for CSV
                size_t pos = 0;
                while ((pos = escapedPath.find('"', pos)) != std::string::npos) {
                    escapedPath.replace(pos, 1, "\"\"");
                    pos += 2;
                }
                
                pos = 0;
                while ((pos = escapedExplanation.find('"', pos)) != std::string::npos) {
                    escapedExplanation.replace(pos, 1, "\"\"");
                    pos += 2;
                }
                
                outFile << "\"" << escapedPath << "\","
                        << "\"" << result.sentiment << "\","
                        << result.harmScore << ","
                        << result.confidence << ","
                        << "\"" << escapedExplanation << "\"\n";
            }
            
            outFile.close();
            utils::showSuccess("Results saved to: " + outputPath);
        } catch (const std::exception& e) {
            utils::showError("Error saving results: " + std::string(e.what()));
            return 1;
        }
    }
    
    return 0;
}

} // namespace bpicli
