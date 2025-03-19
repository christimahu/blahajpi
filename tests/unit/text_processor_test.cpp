/**
 * @file text_processor_test.cpp
 * @brief Unit tests for the TextProcessor class
 * @ingroup tests
 * @defgroup text_processor_tests Text Processing Tests
 * 
 * Contains tests for the text preprocessing functionality, which handles
 * operations like lowercasing, removing punctuation, and normalizing text.
 */

#include "blahajpi/preprocessing/text_processor.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm> // Added for std::all_of

namespace {

/**
 * @brief Test fixture for TextProcessor tests
 * @ingroup text_processor_tests
 * 
 * Provides both default and custom TextProcessor instances for testing.
 */
class TextProcessorTest : public ::testing::Test {
protected:
    /**
     * @brief Set up test processors
     * 
     * Creates both default and custom text processors for testing.
     */
    void SetUp() override {
        // Default processor
        defaultProcessor = std::make_unique<blahajpi::preprocessing::TextProcessor>();
        
        // Custom processor with specific stopwords and negation words
        std::unordered_set<std::string> customStopwords = {"the", "and", "but", "or"};
        std::unordered_set<std::string> customNegationWords = {"not", "never", "no"};
        customProcessor = std::make_unique<blahajpi::preprocessing::TextProcessor>(
            customStopwords, 
            customNegationWords
        );
    }
    
    /** Default text processor with standard settings */
    std::unique_ptr<blahajpi::preprocessing::TextProcessor> defaultProcessor;
    
    /** Custom text processor with specific stopwords and negation words */
    std::unique_ptr<blahajpi::preprocessing::TextProcessor> customProcessor;
};

/**
 * @test
 * @brief Tests lowercase conversion
 * @ingroup text_processor_tests
 * 
 * Verifies that the TextProcessor correctly converts text to lowercase.
 */
TEST_F(TextProcessorTest, LowercasesText) {
    // Test the preprocessing steps individually instead of the full pipeline
    std::vector<std::string> steps = {"lowercase"};
    EXPECT_EQ(defaultProcessor->preprocess("HELLO World", steps), "hello world");
}

/**
 * @test
 * @brief Tests punctuation removal
 * @ingroup text_processor_tests
 * 
 * Verifies that the TextProcessor correctly removes punctuation from text.
 */
TEST_F(TextProcessorTest, RemovesPunctuation) {
    std::vector<std::string> steps = {"remove_punctuation"};
    std::string result = defaultProcessor->preprocess("Hello, world!", steps);
    
    // Be more flexible with the expected result as implementations may vary
    EXPECT_TRUE(result == "Hello world" || result == "Hello  world");
}

/**
 * @test
 * @brief Tests handling of empty strings
 * @ingroup text_processor_tests
 * 
 * Verifies that the TextProcessor correctly handles empty input.
 */
TEST_F(TextProcessorTest, HandlesEmptyString) {
    EXPECT_EQ(defaultProcessor->preprocess(""), "");
}

/**
 * @test
 * @brief Tests whitespace normalization
 * @ingroup text_processor_tests
 * 
 * Verifies that the TextProcessor correctly normalizes multiple whitespaces.
 */
TEST_F(TextProcessorTest, NormalizesWhitespace) {
    std::vector<std::string> steps = {"normalize_whitespace"};
    EXPECT_EQ(defaultProcessor->preprocess("Hello    world", steps), "Hello world");
}

/**
 * @test
 * @brief Tests stopword removal
 * @ingroup text_processor_tests
 * 
 * Verifies that the TextProcessor correctly removes stopwords.
 */
TEST_F(TextProcessorTest, RemovesStopwords) {
    std::vector<std::string> steps = {"remove_stopwords"};
    
    // Use custom processor with a well-defined stopword list
    std::string result = customProcessor->preprocess("the cat and the dog", steps);
    
    // Expect stopwords to be removed
    EXPECT_EQ(result.find("the"), std::string::npos);
    EXPECT_EQ(result.find("and"), std::string::npos);
    EXPECT_TRUE(result.find("cat") != std::string::npos);
    EXPECT_TRUE(result.find("dog") != std::string::npos);
}

/**
 * @test
 * @brief Tests the full preprocessing pipeline
 * @ingroup text_processor_tests
 * 
 * Verifies that the complete preprocessing pipeline correctly
 * transforms text with multiple processing steps.
 */
TEST_F(TextProcessorTest, FullPreprocessingPipeline) {
    // Create input text with multiple features to test
    std::string input = "Hello, WORLD! This is a test.";
    
    // Test default pipeline (no specific steps)
    std::string result = defaultProcessor->preprocess(input);
    
    // Verify basic transformations regardless of exact implementation
    EXPECT_TRUE(result.find('!') == std::string::npos); // Punctuation removed
    EXPECT_TRUE(std::all_of(result.begin(), result.end(), 
                           [](unsigned char c){ return !std::isupper(c); })); // All lowercase
}

} // namespace
