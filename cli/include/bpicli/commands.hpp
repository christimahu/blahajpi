/**
 * @file commands.hpp
 * @brief Declaration of command handlers and registry
 */

#pragma once

#include "blahajpi/analyzer.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace bpicli {

/**
 * @brief Command structure for CLI commands
 */
struct Command {
    std::string description;
    std::function<int(const std::vector<std::string>&, blahajpi::Analyzer&)> handler;
};

/**
 * @brief Creates a registry of all available commands
 * @return Map of command names to Command objects
 */
std::unordered_map<std::string, Command> createCommandRegistry();

/**
 * @brief Handle the analyze command
 * @param args Command arguments
 * @param analyzer Analyzer instance
 * @return Exit code
 */
int handleAnalyze(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer);

/**
 * @brief Handle the batch command
 * @param args Command arguments
 * @param analyzer Analyzer instance
 * @return Exit code
 */
int handleBatch(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer);

/**
 * @brief Handle the config command
 * @param args Command arguments
 * @param analyzer Analyzer instance
 * @return Exit code
 */
int handleConfig(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer);

/**
 * @brief Handle the help command
 * @param args Command arguments
 * @param analyzer Analyzer instance
 * @return Exit code
 */
int handleHelp(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer);

/**
 * @brief Handle the train command
 * @param args Command arguments
 * @param analyzer Analyzer instance
 * @return Exit code
 */
int handleTrain(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer);

/**
 * @brief Handle the version command
 * @param args Command arguments
 * @param analyzer Analyzer instance
 * @return Exit code
 */
int handleVersion(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer);

/**
 * @brief Handle the visualize command
 * @param args Command arguments
 * @param analyzer Analyzer instance
 * @return Exit code
 */
int handleVisualize(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer);

} // namespace bpicli
