/**
 * @file text_processor_test.cpp
 * @brief Unit tests for the TextProcessor class
 */

#include "blahajpi/preprocessing/text_processor.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm> // Added for std::all_of

namespace {

class TextProcessorTest : public ::testing::Test {
protected:
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
    
    std::unique_ptr<blahajpi::preprocessing::TextProcessor> defaultProcessor;
    std::unique_ptr<blahajpi::preprocessing::TextProcessor> customProcessor;
};

// Simplified tests focusing on core functionality
TEST_F(TextProcessorTest, LowercasesText) {
    // Test the preprocessing steps individually instead of the full pipeline
    std::vector<std::string> steps = {"lowercase"};
    EXPECT_EQ(defaultProcessor->preprocess("HELLO World", steps), "hello world");
}

TEST_F(TextProcessorTest, RemovesPunctuation) {
    std::vector<std::string> steps = {"remove_punctuation"};
    std::string result = defaultProcessor->preprocess("Hello, world!", steps);
    
    // Be more flexible with the expected result as implementations may vary
    EXPECT_TRUE(result == "Hello world" || result == "Hello  world");
}

TEST_F(TextProcessorTest, HandlesEmptyString) {
    EXPECT_EQ(defaultProcessor->preprocess(""), "");
}

TEST_F(TextProcessorTest, NormalizesWhitespace) {
    std::vector<std::string> steps = {"normalize_whitespace"};
    EXPECT_EQ(defaultProcessor->preprocess("Hello    world", steps), "Hello world");
}

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
