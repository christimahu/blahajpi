/**
 * @file config.hpp
 * @brief Configuration management for Blahaj PI
 * 
 * This file provides functionality for loading, managing, and accessing
 * configuration settings for the Blahaj PI system. It handles loading
 * from config files and providing defaults when needed.
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <any>

namespace blahajpi {

/**
 * @brief Manages configuration settings for Blahaj PI
 * 
 * The Config class provides a centralized way to handle configuration
 * settings from various sources, with support for different types and defaults.
 */
class Config {
public:
    /**
     * @brief Default constructor
     */
    Config();
    
    /**
     * @brief Constructor that loads from a config file
     * @param configPath Path to configuration file
     */
    explicit Config(const std::string& configPath);
    
    /**
     * @brief Loads configuration from a file
     * @param configPath Path to configuration file
     * @return True if loading was successful
     */
    bool loadFromFile(const std::string& configPath);
    
    /**
     * @brief Saves configuration to a file
     * @param configPath Path to save configuration
     * @return True if saving was successful
     */
    bool saveToFile(const std::string& configPath) const;
    
    /**
     * @brief Gets a configuration value as a string
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist
     * @return Configuration value as string
     */
    std::string getString(const std::string& key, 
                        const std::string& defaultValue = "") const;
    
    /**
     * @brief Gets a configuration value as an integer
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist
     * @return Configuration value as integer
     */
    int getInt(const std::string& key, int defaultValue = 0) const;
    
    /**
     * @brief Gets a configuration value as a double
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist
     * @return Configuration value as double
     */
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    
    /**
     * @brief Gets a configuration value as a boolean
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist
     * @return Configuration value as boolean
     */
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    /**
     * @brief Sets a configuration value
     * @param key Configuration key
     * @param value Value to set
     */
    void set(const std::string& key, const std::string& value);
    
    /**
     * @brief Sets an integer configuration value
     * @param key Configuration key
     * @param value Integer value to set
     */
    void set(const std::string& key, int value);
    
    /**
     * @brief Sets a double configuration value
     * @param key Configuration key
     * @param value Double value to set
     */
    void set(const std::string& key, double value);
    
    /**
     * @brief Sets a boolean configuration value
     * @param key Configuration key
     * @param value Boolean value to set
     */
    void set(const std::string& key, bool value);
    
    /**
     * @brief Checks if a configuration key exists
     * @param key Configuration key to check
     * @return True if the key exists
     */
    bool hasKey(const std::string& key) const;
    
    /**
     * @brief Removes a configuration key
     * @param key Configuration key to remove
     * @return True if the key was removed
     */
    bool remove(const std::string& key);
    
    /**
     * @brief Gets all configuration keys
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Gets the entire configuration as a map
     * @return Map of all configuration key-value pairs
     */
    std::unordered_map<std::string, std::string> getAll() const;
    
    /**
     * @brief Clears all configuration settings
     */
    void clear();
    
    /**
     * @brief Gets the path to the currently loaded config file
     * @return Path to the config file, or empty string if none loaded
     */
    std::string getConfigPath() const;

private:
    std::unordered_map<std::string, std::string> configValues; ///< Configuration values
    std::string configFilePath; ///< Path to the loaded config file
    
    /**
     * @brief Parses a string value from the configuration file
     * @param line Line from the configuration file
     * @return Optional pair of key and value if parsing succeeds
     */
    std::optional<std::pair<std::string, std::string>> parseLine(const std::string& line) const;
    
    /**
     * @brief Loads the default configuration
     * Sets up default values for various configuration parameters
     */
    void loadDefaults();
};

} // namespace blahajpi
