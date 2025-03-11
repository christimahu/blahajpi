/**
 * @file main.cpp
 * @brief Main entry point for Blahaj PI CLI
 */

#include "blahajpi/analyzer.hpp"
#include "bpicli/commands.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <vector>
#include <iomanip>
#include <functional>

// Version info - define the global variables (without extern)
const char* VERSION = "1.0.0";
const char* BUILD_DATE = __DATE__;

/**
 * @brief Main entry point for the CLI application
 */
int main(int argc, char* argv[]) {
    // Create the command registry
    auto commands = bpicli::createCommandRegistry();
    
    // Process command line arguments
    std::vector<std::string> args;
    std::string configPath;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--config" && i + 1 < argc) {
            configPath = argv[++i];
        } else if (arg == "--version") {
            blahajpi::Analyzer analyzer;
            return bpicli::handleVersion({}, analyzer);
        } else if (arg == "--help" || arg == "-h") {
            blahajpi::Analyzer analyzer;
            return bpicli::handleHelp({}, analyzer);
        } else {
            args.push_back(arg);
        }
    }
    
    // If no command specified, show help
    if (args.empty()) {
        blahajpi::Analyzer analyzer;
        return bpicli::handleHelp({}, analyzer);
    }
    
    // Parse command
    std::string command = args[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    args.erase(args.begin());
    
    // Create analyzer with configuration if provided
    blahajpi::Analyzer analyzer;
    if (!configPath.empty()) {
        if (!analyzer.loadConfig(configPath)) {
            std::cerr << "Error: Failed to load configuration from " << configPath << std::endl;
            return 1;
        }
        std::cout << "Loaded configuration from " << configPath << std::endl;
    }
    
    // Execute command
    auto it = commands.find(command);
    if (it != commands.end()) {
        return it->second.handler(args, analyzer);
    } else {
        std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
        std::cerr << "Run 'blahajpi --help' for usage information" << std::endl;
        return 1;
    }
}
