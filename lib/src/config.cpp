/**
 * @file config.cpp
 * @brief Implementation of the configuration management system
 */

#include "blahajpi/config.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace blahajpi {

Config::Config() {
    loadDefaults();
}

Config::Config(const std::string& configPath) {
    loadDefaults();
    loadFromFile(configPath);
}

bool Config::loadFromFile(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open configuration file: " << configPath << std::endl;
        return false;
    }
    
    configFilePath = configPath;
    std::string line;
    size_t lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key-value pairs
        auto keyValuePair = parseLine(line);
        if (keyValuePair) {
            configValues[keyValuePair->first] = keyValuePair->second;
            // Debug output to verify configuration is loaded
            std::cout << "Loaded config: " << keyValuePair->first << " = " << keyValuePair->second << std::endl;
        } else {
            std::cerr << "Warning: Invalid configuration line " << lineNum 
                      << ": '" << line << "'" << std::endl;
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& configPath) const {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << configPath << std::endl;
        return false;
    }
    
    // Get current date/time for comment
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream dateStr;
    dateStr << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    file << "# Blahaj PI Configuration\n";
    file << "# Generated on " << dateStr.str() << "\n\n";
    
    // Write configuration values grouped by category
    std::unordered_map<std::string, std::vector<std::string>> categories;
    
    for (const auto& [key, value] : configValues) {
        // Extract category from key (part before first '-')
        size_t dashPos = key.find('-');
        std::string category = (dashPos != std::string::npos) ? 
                              key.substr(0, dashPos) : "general";
        
        categories[category].push_back(key);
    }
    
    // Sort categories and keys within each category
    std::vector<std::string> categoryNames;
    for (const auto& [category, _] : categories) {
        categoryNames.push_back(category);
    }
    
    std::sort(categoryNames.begin(), categoryNames.end());
    
    for (const auto& category : categoryNames) {
        file << "# " << category << " settings\n";
        
        auto& keys = categories[category];
        std::sort(keys.begin(), keys.end());
        
        for (const auto& key : keys) {
            file << key << " = " << configValues.at(key) << "\n";
        }
        
        file << "\n";
    }
    
    return true;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = configValues.find(key);
    if (it != configValues.end()) {
        return it->second;
    }
    return defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) const {
    auto it = configValues.find(key);
    if (it != configValues.end()) {
        try {
            return std::stoi(it->second);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid integer value for key '" << key 
                      << "': " << it->second << std::endl;
        }
    }
    return defaultValue;
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    auto it = configValues.find(key);
    if (it != configValues.end()) {
        try {
            return std::stod(it->second);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid double value for key '" << key 
                      << "': " << it->second << std::endl;
        }
    }
    return defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    auto it = configValues.find(key);
    if (it != configValues.end()) {
        std::string value = it->second;
        std::transform(value.begin(), value.end(), value.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        
        if (value == "true" || value == "yes" || value == "1" || value == "on") {
            return true;
        } else if (value == "false" || value == "no" || value == "0" || value == "off") {
            return false;
        } else {
            std::cerr << "Warning: Invalid boolean value for key '" << key 
                      << "': " << it->second << std::endl;
        }
    }
    return defaultValue;
}

void Config::set(const std::string& key, const std::string& value) {
    configValues[key] = value;
}

void Config::set(const std::string& key, int value) {
    configValues[key] = std::to_string(value);
}

void Config::set(const std::string& key, double value) {
    configValues[key] = std::to_string(value);
}

void Config::set(const std::string& key, bool value) {
    configValues[key] = value ? "true" : "false";
}

bool Config::hasKey(const std::string& key) const {
    return configValues.find(key) != configValues.end();
}

bool Config::remove(const std::string& key) {
    auto it = configValues.find(key);
    if (it != configValues.end()) {
        configValues.erase(it);
        return true;
    }
    return false;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(configValues.size());
    
    for (const auto& [key, _] : configValues) {
        keys.push_back(key);
    }
    
    return keys;
}

std::unordered_map<std::string, std::string> Config::getAll() const {
    return configValues;
}

void Config::clear() {
    configValues.clear();
    loadDefaults();
}

std::string Config::getConfigPath() const {
    return configFilePath;
}

std::optional<std::pair<std::string, std::string>> Config::parseLine(const std::string& line) const {
    size_t equalPos = line.find('=');
    if (equalPos == std::string::npos) {
        return std::nullopt;
    }
    
    std::string key = line.substr(0, equalPos);
    std::string value = line.substr(equalPos + 1);
    
    // Trim whitespace
    key.erase(0, key.find_first_not_of(" \t"));
    key.erase(key.find_last_not_of(" \t") + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);
    
    if (key.empty()) {
        return std::nullopt;
    }
    
    return std::make_pair(key, value);
}

void Config::loadDefaults() {
    // Model settings
    configValues["model-type"] = "sgd";             // SGD classifier by default
    configValues["alpha"] = "0.0001";               // Regularization strength
    configValues["eta0"] = "0.01";                  // Learning rate
    configValues["epochs"] = "10";                  // Number of training epochs
    configValues["loss"] = "log";                   // Loss function (log for logistic regression)
    
    // Feature extraction settings
    configValues["use-sublinear-tf"] = "true";      // Use sublinear scaling for term frequencies
    configValues["max-df"] = "0.5";                 // Maximum document frequency
    configValues["max-features"] = "10000";         // Maximum number of features
    configValues["min-ngram"] = "1";                // Minimum n-gram size
    configValues["max-ngram"] = "2";                // Maximum n-gram size
    
    // Text preprocessing settings
    configValues["preprocessing-pipeline"] = "remove_urls,remove_mentions,process_hashtags,lowercase,expand_abbreviations,handle_negations,remove_punctuation,remove_numbers,normalize_whitespace,remove_stopwords,normalize_repeated_chars";
    
    // Analysis settings
    configValues["threshold"] = "0.5";              // Classification threshold
    configValues["confidence-scaling"] = "2.0";     // Scaling factor for confidence scores
    
    // Visualization settings
    configValues["word-cloud-max-words"] = "50";    // Maximum words in word cloud
    configValues["word-cloud-width"] = "80";        // Width of word cloud
    configValues["word-cloud-height"] = "20";       // Height of word cloud
    configValues["word-cloud-use-color"] = "true";  // Use ANSI color codes in word cloud
    
    // Path settings
    configValues["model-dir"] = "../models/default"; // Directory for model files
    configValues["output-dir"] = "../results";       // Directory for output files
    
    // Dataset column settings
    configValues["label-column"] = "label";          // Default label column name
    configValues["text-column"] = "text";            // Default text column name
}

} // namespace blahajpi
