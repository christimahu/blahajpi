/**
 * @file commands.cpp
 * @brief Implementation of the command registry
 */

#include "bpicli/commands.hpp"
#include <unordered_map>
#include <string>
#include <functional>

namespace bpicli {

std::unordered_map<std::string, Command> createCommandRegistry() {
    std::unordered_map<std::string, Command> commands;
    
    // Register commands with their descriptions and handlers
    commands["analyze"] = {
        "Analyze text for potentially harmful content",
        handleAnalyze
    };
    
    commands["batch"] = {
        "Batch process multiple files",
        handleBatch
    };
    
    commands["config"] = {
        "Manage configuration settings",
        handleConfig
    };
    
    commands["help"] = {
        "Display help information for commands",
        handleHelp
    };
    
    commands["train"] = {
        "Train a new sentiment analysis model",
        handleTrain
    };
    
    commands["version"] = {
        "Display version information",
        handleVersion
    };
    
    commands["visualize"] = {
        "Generate visualizations from analysis results",
        handleVisualize
    };
    
    return commands;
}

} // namespace bpicli
