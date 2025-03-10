/**
 * @file help.cpp
 * @brief Implementation of the help command
 */

#include "bpicli/commands.hpp"
#include "bpicli/utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <functional>

namespace bpicli {

// Forward declaration of createCommandRegistry (defined in main.cpp)
std::unordered_map<std::string, Command> createCommandRegistry();

// Helper function to display command-specific help
static void showCommandHelp(const std::string& command) {
    if (command == "analyze") {
        std::cout << "Analyze text for potentially harmful content\n\n";
        std::cout << "Usage: blahajpi analyze [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --file <path>         Analyze content from a file\n";
        std::cout << "  --text <text>         Analyze the provided text\n";
        std::cout << "  --output <path>       Save analysis result to a file\n";
        std::cout << "  --verbose, -v         Show detailed analysis information\n";
        std::cout << "  --exit-on-harmful     Return non-zero exit code if harmful content detected\n\n";
        std::cout << "Examples:\n";
        std::cout << "  blahajpi analyze --file input.txt\n";
        std::cout << "  blahajpi analyze --text \"Text to analyze\"\n";
        std::cout << "  blahajpi analyze --file input.txt --output result.txt\n";
        
    }

int handleHelp(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer) {
    // Get command registry
    auto commands = createCommandRegistry();
    
    if (args.empty()) {
        // Display general help if no specific command is requested
        std::cout << "Blahaj PI - Your friendly shark detective keeping social waters safe\n\n";
        std::cout << "Usage: blahajpi [options] command [command_options]\n\n";
        
        std::cout << "Global Options:\n";
        std::cout << "  --config <file>    Specify configuration file\n";
        std::cout << "  --version          Display version information\n";
        std::cout << "  --help, -h         Display this help message\n\n";
        
        std::cout << "Commands:\n";
        
        // Extract command names and sort them
        std::vector<std::string> commandNames;
        for (const auto& [name, cmd] : commands) {
            commandNames.push_back(name);
        }
        std::sort(commandNames.begin(), commandNames.end());
        
        // Display command list with descriptions
        for (const auto& name : commandNames) {
            const auto& cmd = commands.at(name);
            std::cout << "  " << std::left << std::setw(15) << name << cmd.description << std::endl;
        }
        
        std::cout << "\nFor help on a specific command, run: blahajpi help <command>\n";
    } else {
        // Display help for the specified command
        std::string command = args[0];
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        if (commands.find(command) != commands.end()) {
            showCommandHelp(command);
        } else {
            utils::showError("Unknown command: " + command);
            std::cout << "Run 'blahajpi help' without arguments to see available commands." << std::endl;
            return 1;
        }
    }
    
    return 0;
} else if (command == "batch") {
        std::cout << "Batch process multiple files\n\n";
        std::cout << "Usage: blahajpi batch [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --input-dir <dir>     Process all files in a directory\n";
        std::cout << "  --input-file <path>   Process files listed in a file (one per line)\n";
        std::cout << "  --output <path>       Save batch analysis results to a file\n";
        std::cout << "  --recursive           Process files in subdirectories (with --input-dir)\n";
        std::cout << "  --show-harmful        Display detailed report for harmful content\n\n";
        std::cout << "Examples:\n";
        std::cout << "  blahajpi batch --input-dir ./documents --output results.csv\n";
        std::cout << "  blahajpi batch --input-file file_list.txt --show-harmful\n";
        
    } else if (command == "config") {
        std::cout << "Manage configuration settings\n\n";
        std::cout << "Usage: blahajpi config [command] [options]\n\n";
        std::cout << "Commands:\n";
        std::cout << "  list                 List all configuration settings\n";
        std::cout << "  get <key>            Get a specific configuration value\n";
        std::cout << "  set <key> <value>    Set a configuration value\n";
        std::cout << "  load <file>          Load configuration from a file\n";
        std::cout << "  save [file]          Save current configuration to a file\n";
        std::cout << "  reset                Reset configuration to defaults\n\n";
        std::cout << "Examples:\n";
        std::cout << "  blahajpi config list\n";
        std::cout << "  blahajpi config get model-dir\n";
        std::cout << "  blahajpi config set max-features 20000\n";
        std::cout << "  blahajpi config load ./configs/fast_model.conf\n";
        
    } else if (command == "train") {
        std::cout << "Train a new sentiment analysis model\n\n";
        std::cout << "Usage: blahajpi train [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --dataset <path>       Path to training dataset\n";
        std::cout << "  --output <dir>         Directory to save the trained model\n";
        std::cout << "  --test-size <fraction> Fraction of data to use for testing (default: 0.2)\n";
        std::cout << "  --alpha <value>        Regularization strength (default: from config)\n";
        std::cout << "  --epochs <number>      Number of training epochs (default: from config)\n";
        std::cout << "  --eta0 <value>         Initial learning rate (default: from config)\n";
        std::cout << "  --seed <number>        Random seed for reproducibility\n\n";
        std::cout << "Examples:\n";
        std::cout << "  blahajpi train --dataset data.csv --output models/custom_model\n";
        std::cout << "  blahajpi train --dataset data.csv --alpha 0.0001 --epochs 15\n";
        
    } else if (command == "version") {
        std::cout << "Display version information\n\n";
        std::cout << "Usage: blahajpi version\n\n";
        std::cout << "Examples:\n";
        std::cout << "  blahajpi version\n";
        std::cout << "  blahajpi --version\n";
        
    } else if (command == "visualize") {
        std::cout << "Generate visualizations from analysis results\n\n";
        std::cout << "Usage: blahajpi visualize [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --input <path>         Input file containing text to visualize\n";
        std::cout << "  --output <path>        Path to save the visualization\n";
        std::cout << "  --format <type>        Visualization format (wordcloud, bars)\n";
        std::cout << "  --max-words <number>   Maximum number of words to include (default: 50)\n";
        std::cout << "  --width <number>       Width of visualization (default: 80)\n";
        std::cout << "  --height <number>      Height of visualization (default: 20)\n";
        std::cout << "  --all                  Include both harmful and safe content\n";
        std::cout << "  --color                Use color in the visualization\n\n";
        std::cout << "Examples:\n";
        std::cout << "  blahajpi visualize --input texts.txt --output wordcloud.txt\n";
        std::cout << "  blahajpi visualize --input analyzed_data.csv --format bars --width 100\n";
        
    } else {
        std::cout << "Unknown command: " << command << "\n";
        std::cout << "Run 'blahajpi help' without arguments to see available commands.\n";
    }
}