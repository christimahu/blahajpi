/**
 * @file config.cpp
 * @brief Implementation of the config command
 */

#include "bpicli/commands.hpp"
#include "bpicli/utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <sstream>  // Add this include for stringstream

namespace bpicli {

int handleConfig(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer) {
    if (args.empty()) {
        std::cout << "Usage: blahajpi config [command] [options]\n\n";
        std::cout << "Commands:\n";
        std::cout << "  list                 List all configuration settings\n";
        std::cout << "  get <key>            Get a specific configuration value\n";
        std::cout << "  set <key> <value>    Set a configuration value\n";
        std::cout << "  load <file>          Load configuration from a file\n";
        std::cout << "  save [file]          Save current configuration to a file\n";
        std::cout << "  reset                Reset configuration to defaults\n";
        return 1;
    }
    
    std::string subcommand = args[0];
    
    // Convert to lowercase
    std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(), ::tolower);
    
    if (subcommand == "list") {
        // Get current configuration
        auto config = analyzer.getConfig();
        
        // Sort keys for display
        std::vector<std::string> keys;
        keys.reserve(config.size());
        
        for (const auto& [key, _] : config) {
            keys.push_back(key);
        }
        
        std::sort(keys.begin(), keys.end());
        
        // Display configuration
        std::cout << "Configuration Settings:\n";
        std::cout << "------------------------\n";
        
        for (const auto& key : keys) {
            std::cout << key << " = " << config.at(key) << std::endl;
        }
        
        std::cout << "------------------------\n";
        std::cout << "Total settings: " << config.size() << std::endl;
        
        return 0;
    } 
    else if (subcommand == "get") {
        if (args.size() < 2) {
            utils::showError("Missing key argument");
            std::cout << "Usage: blahajpi config get <key>" << std::endl;
            return 1;
        }
        
        std::string key = args[1];
        auto config = analyzer.getConfig();
        
        if (config.find(key) != config.end()) {
            std::cout << key << " = " << config.at(key) << std::endl;
        } else {
            utils::showError("Key not found: " + key);
            return 1;
        }
        
        return 0;
    } 
    else if (subcommand == "set") {
        if (args.size() < 3) {
            utils::showError("Missing arguments");
            std::cout << "Usage: blahajpi config set <key> <value>" << std::endl;
            return 1;
        }
        
        std::string key = args[1];
        std::string value = args[2];
        
        analyzer.setConfig(key, value);
        utils::showSuccess("Set " + key + " = " + value);
        
        return 0;
    } 
    else if (subcommand == "load") {
        if (args.size() < 2) {
            utils::showError("Missing file argument");
            std::cout << "Usage: blahajpi config load <file>" << std::endl;
            return 1;
        }
        
        std::string configPath = args[1];
        
        // Verify file exists
        if (!std::filesystem::exists(configPath)) {
            utils::showError("File not found: " + configPath);
            return 1;
        }
        
        if (analyzer.loadConfig(configPath)) {
            utils::showSuccess("Configuration loaded from: " + configPath);
        } else {
            utils::showError("Failed to load configuration from: " + configPath);
            return 1;
        }
        
        return 0;
    } 
    else if (subcommand == "save") {
        std::string configPath;
        
        if (args.size() >= 2) {
            configPath = args[1];
        } else {
            // Use default path if not specified
            configPath = utils::getDefaultConfigPath();
            if (configPath.empty()) {
                configPath = "blahajpi_config.conf";
            }
        }
        
        // Create the configuration content
        std::stringstream configContent;
        auto config = analyzer.getConfig();
        
        for (const auto& [key, value] : config) {
            configContent << key << " = " << value << "\n";
        }
        
        // Save to file
        if (utils::saveToFile(configContent.str(), configPath)) {
            utils::showSuccess("Configuration saved to: " + configPath);
        } else {
            utils::showError("Failed to save configuration to: " + configPath);
            return 1;
        }
        
        return 0;
    } 
    else if (subcommand == "reset") {
        // Currently not implemented - would require Analyzer to have a reset method
        utils::showWarning("Reset functionality not yet implemented");
        return 1;
    }
    else {
        utils::showError("Unknown config command: " + subcommand);
        std::cout << "Run 'blahajpi config' without arguments to see available commands." << std::endl;
        return 1;
    }
}

} // namespace bpicli
