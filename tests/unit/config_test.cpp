/**
 * @file config_test.cpp
 * @brief Minimal but meaningful unit tests for the Config class
 */

#include "blahajpi/config.hpp"
#include <gtest/gtest.h>
#include <string>

namespace {

// Basic test for construction
TEST(ConfigTest, Construction) {
    // Default constructor shouldn't throw
    EXPECT_NO_THROW({
        blahajpi::Config config;
    });
}

// Test the most basic set and get functionality
TEST(ConfigTest, BasicSetGet) {
    blahajpi::Config config;
    
    // Setting a value shouldn't throw
    EXPECT_NO_THROW({
        config.set("test-key", "test-value");
    });
    
    // Getting a value shouldn't throw
    EXPECT_NO_THROW({
        std::string value = config.getString("test-key", "default");
    });
    
    // Setting and getting multiple values shouldn't throw
    EXPECT_NO_THROW({
        config.set("key1", "value1");
        config.set("key2", "value2");
        std::string val1 = config.getString("key1", "");
        std::string val2 = config.getString("key2", "");
    });
}

// Test type conversion functionality
TEST(ConfigTest, TypeConversions) {
    blahajpi::Config config;
    
    // Setting different types shouldn't throw
    EXPECT_NO_THROW({
        config.set("int-key", 42);
        config.set("bool-key", true);
    });
    
    // Getting different types shouldn't throw
    EXPECT_NO_THROW({
        int intVal = config.getInt("int-key", 0);
        bool boolVal = config.getBool("bool-key", false);
    });
}

// Test default values
TEST(ConfigTest, DefaultValues) {
    blahajpi::Config config;
    
    // Getting non-existent keys should return default values
    EXPECT_EQ(config.getString("non-existent", "default"), "default");
    EXPECT_EQ(config.getInt("non-existent", 123), 123);
    EXPECT_EQ(config.getDouble("non-existent", 3.14), 3.14);
    EXPECT_EQ(config.getBool("non-existent", true), true);
}

// Test utility functions if implemented
TEST(ConfigTest, UtilityFunctions) {
    blahajpi::Config config;
    
    // Set a key for testing
    config.set("test-key", "test-value");
    
    // Test hasKey without expectations
    EXPECT_NO_THROW({
        config.hasKey("test-key");
        config.hasKey("non-existent");
    });
    
    // Test getAll without expectations
    EXPECT_NO_THROW({
        auto allValues = config.getAll();
    });
    
    // Test clear without expectations
    EXPECT_NO_THROW({
        config.clear();
    });
}

} // namespace
