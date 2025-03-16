/**
 * @file config_test.cpp
 * @brief Comprehensive unit tests for the Config class
 * 
 * These tests verify the functionality of the configuration management system,
 * including loading, saving, and manipulating configuration settings.
 * 
 * @author BlahajPI Team
 * @date 2025
 */

#include "blahajpi/config.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>

namespace {

/**
 * @class ConfigTest
 * @brief Test fixture for Config unit tests
 * 
 * This fixture provides common setup for testing the configuration system.
 */
class ConfigTest : public ::testing::Test {
protected:
    /**
     * @brief Set up test environment
     * 
     * Creates temporary directory and files for testing.
     */
    void SetUp() override {
        // Create temporary directory
        tempDir = std::filesystem::temp_directory_path() / "blahajpi_tests";
        std::filesystem::create_directories(tempDir);
        
        // Create test config file
        testConfigPath = tempDir / "test_config.conf";
        std::ofstream configFile(testConfigPath);
        configFile << "# Test configuration file\n"
                  << "test-string = test value\n"
                  << "test-int = 42\n"
                  << "test-double = 3.14159\n"
                  << "test-bool = true\n"
                  << "\n"
                  << "# Empty lines and comments should be ignored\n"
                  << "multi-word-key = multi word value\n";
        configFile.close();
    }
    
    /**
     * @brief Clean up test environment
     * 
     * Removes temporary files and directories.
     */
    void TearDown() override {
        if (std::filesystem::exists(tempDir)) {
            std::filesystem::remove_all(tempDir);
        }
    }
    
    std::filesystem::path tempDir;
    std::filesystem::path testConfigPath;
};

/**
 * @brief Tests default constructor
 * 
 * Verifies that the Config class is properly initialized with default values.
 */
TEST_F(ConfigTest, DefaultConstructor) {
    blahajpi::Config config;
    
    // Check that default values are loaded
    EXPECT_TRUE(config.hasKey("model-dir"));
    EXPECT_TRUE(config.hasKey("alpha"));
    EXPECT_TRUE(config.hasKey("loss"));
}

/**
 * @brief Tests constructor with config file
 * 
 * Verifies that the Config class correctly loads settings from a file.
 */
TEST_F(ConfigTest, FileConstructor) {
    blahajpi::Config config(testConfigPath.string());
    
    // Check that values from file are loaded
    EXPECT_EQ(config.getString("test-string"), "test value");
    EXPECT_EQ(config.getInt("test-int"), 42);
    EXPECT_NEAR(config.getDouble("test-double"), 3.14159, 0.00001);
    EXPECT_EQ(config.getBool("test-bool"), true);
    EXPECT_EQ(config.getString("multi-word-key"), "multi word value");
    
    // Default values should still be available for keys not in the file
    EXPECT_TRUE(config.hasKey("model-dir"));
}

/**
 * @brief Tests loading from config file
 * 
 * Verifies that settings can be loaded from a file after construction.
 */
TEST_F(ConfigTest, LoadFromFile) {
    blahajpi::Config config;
    
    // Load from file
    bool result = config.loadFromFile(testConfigPath.string());
    EXPECT_TRUE(result);
    
    // Check that values from file are loaded
    EXPECT_EQ(config.getString("test-string"), "test value");
    EXPECT_EQ(config.getInt("test-int"), 42);
    
    // Test loading from non-existent file
    result = config.loadFromFile("non_existent_file.conf");
    EXPECT_FALSE(result);
}

/**
 * @brief Tests saving to config file
 * 
 * Verifies that settings can be saved to a file.
 */
TEST_F(ConfigTest, SaveToFile) {
    blahajpi::Config config;
    
    // Set some values
    config.set("test-key1", "test value 1");
    config.set("test-key2", 123);
    config.set("test-key3", 3.14);
    config.set("test-key4", true);
    
    // Save to file
    std::filesystem::path savePath = tempDir / "save_test.conf";
    bool result = config.saveToFile(savePath.string());
    EXPECT_TRUE(result);
    
    // Check that file exists
    EXPECT_TRUE(std::filesystem::exists(savePath));
    
    // Load the saved file to verify
    blahajpi::Config loadedConfig(savePath.string());
    EXPECT_EQ(loadedConfig.getString("test-key1"), "test value 1");
    EXPECT_EQ(loadedConfig.getInt("test-key2"), 123);
    EXPECT_NEAR(loadedConfig.getDouble("test-key3"), 3.14, 0.00001);
    EXPECT_EQ(loadedConfig.getBool("test-key4"), true);
}

/**
 * @brief Tests string getters and setters
 * 
 * Verifies that string values can be set and retrieved.
 */
TEST_F(ConfigTest, StringGetSet) {
    blahajpi::Config config;
    
    // Test default value
    EXPECT_EQ(config.getString("non-existent-key", "default"), "default");
    
    // Test setting and getting
    config.set("string-key", "string value");
    EXPECT_EQ(config.getString("string-key"), "string value");
    
    // Test overwriting
    config.set("string-key", "new value");
    EXPECT_EQ(config.getString("string-key"), "new value");
}

/**
 * @brief Tests integer getters and setters
 * 
 * Verifies that integer values can be set and retrieved.
 */
TEST_F(ConfigTest, IntGetSet) {
    blahajpi::Config config;
    
    // Test default value
    EXPECT_EQ(config.getInt("non-existent-key", 42), 42);
    
    // Test setting and getting
    config.set("int-key", 123);
    EXPECT_EQ(config.getInt("int-key"), 123);
    
    // Test overwriting
    config.set("int-key", 456);
    EXPECT_EQ(config.getInt("int-key"), 456);
    
    // Test conversion from string
    config.set("int-string", "789");
    EXPECT_EQ(config.getInt("int-string"), 789);
    
    // Test invalid conversion
    config.set("int-invalid", "not-a-number");
    EXPECT_EQ(config.getInt("int-invalid", 42), 42);
}

/**
 * @brief Tests double getters and setters
 * 
 * Verifies that floating-point values can be set and retrieved.
 */
TEST_F(ConfigTest, DoubleGetSet) {
    blahajpi::Config config;
    
    // Test default value
    EXPECT_NEAR(config.getDouble("non-existent-key", 3.14), 3.14, 0.00001);
    
    // Test setting and getting
    config.set("double-key", 2.71828);
    EXPECT_NEAR(config.getDouble("double-key"), 2.71828, 0.00001);
    
    // Test overwriting
    config.set("double-key", 1.41421);
    EXPECT_NEAR(config.getDouble("double-key"), 1.41421, 0.00001);
    
    // Test conversion from string
    config.set("double-string", "3.14159");
    EXPECT_NEAR(config.getDouble("double-string"), 3.14159, 0.00001);
    
    // Test invalid conversion
    config.set("double-invalid", "not-a-number");
    EXPECT_NEAR(config.getDouble("double-invalid", 2.71828), 2.71828, 0.00001);
}

/**
 * @brief Tests boolean getters and setters
 * 
 * Verifies that boolean values can be set and retrieved.
 */
TEST_F(ConfigTest, BoolGetSet) {
    blahajpi::Config config;
    
    // Test default value
    EXPECT_EQ(config.getBool("non-existent-key", true), true);
    EXPECT_EQ(config.getBool("non-existent-key-2", false), false);
    
    // Test setting and getting
    config.set("bool-key", true);
    EXPECT_EQ(config.getBool("bool-key"), true);
    
    // Test overwriting
    config.set("bool-key", false);
    EXPECT_EQ(config.getBool("bool-key"), false);
    
    // Test conversion from various string formats
    config.set("bool-true-1", "true");
    EXPECT_EQ(config.getBool("bool-true-1"), true);
    
    config.set("bool-true-2", "yes");
    EXPECT_EQ(config.getBool("bool-true-2"), true);
    
    config.set("bool-true-3", "1");
    EXPECT_EQ(config.getBool("bool-true-3"), true);
    
    config.set("bool-true-4", "on");
    EXPECT_EQ(config.getBool("bool-true-4"), true);
    
    config.set("bool-false-1", "false");
    EXPECT_EQ(config.getBool("bool-false-1"), false);
    
    config.set("bool-false-2", "no");
    EXPECT_EQ(config.getBool("bool-false-2"), false);
    
    config.set("bool-false-3", "0");
    EXPECT_EQ(config.getBool("bool-false-3"), false);
    
    config.set("bool-false-4", "off");
    EXPECT_EQ(config.getBool("bool-false-4"), false);
    
    // Test case insensitivity
    config.set("bool-case", "TRUE");
    EXPECT_EQ(config.getBool("bool-case"), true);
    
    // Test invalid conversion
    config.set("bool-invalid", "not-a-boolean");
    EXPECT_EQ(config.getBool("bool-invalid", true), true);
}

/**
 * @brief Tests key management
 * 
 * Verifies that keys can be checked, listed, and removed.
 */
TEST_F(ConfigTest, KeyManagement) {
    blahajpi::Config config;
    
    // Add some keys
    config.set("key1", "value1");
    config.set("key2", "value2");
    config.set("key3", "value3");
    
    // Check existence
    EXPECT_TRUE(config.hasKey("key1"));
    EXPECT_TRUE(config.hasKey("key2"));
    EXPECT_TRUE(config.hasKey("key3"));
    EXPECT_FALSE(config.hasKey("key4"));
    
    // Get all keys
    auto keys = config.getKeys();
    
    // Verify keys are included (not checking order)
    EXPECT_TRUE(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    EXPECT_TRUE(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    EXPECT_TRUE(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    // Remove a key
    bool removed = config.remove("key2");
    EXPECT_TRUE(removed);
    EXPECT_FALSE(config.hasKey("key2"));
    
    // Try to remove non-existent key
    removed = config.remove("non-existent-key");
    EXPECT_FALSE(removed);
    
    // Get all config key-value pairs
    auto allConfig = config.getAll();
    EXPECT_TRUE(allConfig.find("key1") != allConfig.end());
    EXPECT_TRUE(allConfig.find("key3") != allConfig.end());
    EXPECT_TRUE(allConfig.find("key2") == allConfig.end());
}

/**
 * @brief Tests clearing the configuration
 * 
 * Verifies that all custom settings can be cleared, restoring defaults.
 */
TEST_F(ConfigTest, ClearConfig) {
    blahajpi::Config config;
    
    // Add some keys
    config.set("custom-key", "custom value");
    EXPECT_TRUE(config.hasKey("custom-key"));
    
    // Clear the config
    config.clear();
    
    // Custom key should be gone
    EXPECT_FALSE(config.hasKey("custom-key"));
    
    // Default keys should still be there
    EXPECT_TRUE(config.hasKey("model-dir"));
    EXPECT_TRUE(config.hasKey("alpha"));
}

/**
 * @brief Tests config path tracking
 * 
 * Verifies that the config file path is correctly tracked.
 */
TEST_F(ConfigTest, ConfigPath) {
    // Default constructor should have empty path
    blahajpi::Config defaultConfig;
    EXPECT_TRUE(defaultConfig.getConfigPath().empty());
    
    // File constructor should track path
    blahajpi::Config fileConfig(testConfigPath.string());
    EXPECT_EQ(fileConfig.getConfigPath(), testConfigPath.string());
    
    // Loading from file should update path
    blahajpi::Config loadConfig;
    loadConfig.loadFromFile(testConfigPath.string());
    EXPECT_EQ(loadConfig.getConfigPath(), testConfigPath.string());
}

/**
 * @brief Tests handling of invalid config lines
 * 
 * Verifies that invalid configuration file lines are handled gracefully.
 */
TEST_F(ConfigTest, InvalidConfigLines) {
    // Create config with invalid lines
    std::filesystem::path invalidPath = tempDir / "invalid.conf";
    std::ofstream invalidFile(invalidPath);
    invalidFile << "# This is a comment\n"
               << "valid-key = valid value\n"
               << "invalid line without equals\n"
               << "=\n"
               << "= empty key\n"
               << "empty-value = \n";
    invalidFile.close();
    
    // Should load without crashing
    blahajpi::Config config;
    bool result = config.loadFromFile(invalidPath.string());
    EXPECT_TRUE(result);
    
    // Valid key should be loaded
    EXPECT_EQ(config.getString("valid-key"), "valid value");
    
    // Empty value should be loaded as empty string
    EXPECT_EQ(config.getString("empty-value"), "");
    
    // Invalid lines should be ignored
    EXPECT_FALSE(config.hasKey("invalid"));
    EXPECT_FALSE(config.hasKey(""));
}

} // namespace
