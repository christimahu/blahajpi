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

// Version info
constexpr const char* VERSION = "1.0.0";
constexpr const char* BUILD_DATE = __DATE__;

/**
 * @brief Displays help message for the CLI
 * @param commands Map of available commands
 */
void showHelp(const std::unordered_map<std::string, bpicli::Command>& commands) {
    std::cout << "Blahaj PI - Your friendly shark detective keeping social waters safe\n\n";
    std::cout << "Usage: blahajpi [options] command [command_options]\n\n";
    
    std::cout << "Global Options:\n";
    std::cout << "  --config <file>    Specify configuration file\n";
    std::cout << "  --version          Display version information\n";
    std::cout << "  --help, -h         Display this help message\n\n";
    
    std::cout << "Commands:\n";
    
    // Sort commands alphabetically for display
    std::vector<std::string> commandNames;
    for (const auto& [name, cmd] : commands) {
        commandNames.push_back(name);
    }
    std::sort(commandNames.begin(), commandNames.end());
    
    for (const auto& name : commandNames) {
        const auto& cmd = commands.at(name);
        std::cout << "  " << std::left << std::setw(15) << name << cmd.description << std::endl;
    }
    
    std::cout << "\nFor help on a specific command, run: blahajpi help <command>\n";
}

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
            return bpicli::handleVersion({}, blahajpi::Analyzer());
        } else if (arg == "--help" || arg == "-h") {
            showHelp(commands);
            return 0;
        } else {
            args.push_back(arg);
        }
    }
    
    // If no command specified, show help
    if (args.empty()) {
        showHelp(commands);
        return 0;
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
