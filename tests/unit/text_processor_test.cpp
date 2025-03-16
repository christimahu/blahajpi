/**
 * @file text_processor_test.cpp
 * @brief Comprehensive unit tests for the TextProcessor class
 * 
 * These tests verify the functionality of the text preprocessing component,
 * including edge cases, normal operation, and special character handling.
 * 
 * @author BlahajPI Team
 * @date 2025
 */

#include "blahajpi/preprocessing/text_processor.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <unordered_set>

namespace {

/**
 * @class TextProcessorTest
 * @brief Test fixture for TextProcessor unit tests
 * 
 * This fixture provides a fresh TextProcessor instance for each test
 * and includes setup for various preprocessing scenarios.
 */
class TextProcessorTest : public ::testing::Test {
protected:
    /**
     * @brief Set up the test fixture
     * 
     * Creates a default TextProcessor instance and a custom TextProcessor
     * with defined stopwords and negation words for specific tests.
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
    
    /**
     * @brief Clean up resources after test
     */
    void TearDown() override {
        // Nothing specific to clean up
    }
    
    std::unique_ptr<blahajpi::preprocessing::TextProcessor> defaultProcessor;
    std::unique_ptr<blahajpi::preprocessing::TextProcessor> customProcessor;
};

/**
 * @brief Tests the lowercase conversion functionality
 * 
 * Verifies that uppercase letters are properly converted to lowercase
 * while preserving non-alphabetic characters.
 */
TEST_F(TextProcessorTest, LowercasesText) {
    // Standard case
    EXPECT_EQ(defaultProcessor->preprocess("HELLO World", {"lowercase"}), "hello world");
    
    // Mixed case with numbers and symbols
    EXPECT_EQ(defaultProcessor->preprocess("HeLLo 123 WoRLD!", {"lowercase"}), "hello 123 world!");
    
    // Already lowercase
    EXPECT_EQ(defaultProcessor->preprocess("hello world", {"lowercase"}), "hello world");
    
    // Special characters
    EXPECT_EQ(defaultProcessor->preprocess("CAFÉ", {"lowercase"}), "café");
}

/**
 * @brief Tests punctuation removal functionality
 * 
 * Verifies that punctuation marks are correctly removed from text
 * while preserving alphanumeric characters and spaces.
 */
TEST_F(TextProcessorTest, RemovesPunctuation) {
    // Basic punctuation
    EXPECT_EQ(defaultProcessor->preprocess("Hello, world!", {"remove_punctuation"}), "Hello world");
    
    // Multiple punctuation marks
    EXPECT_EQ(defaultProcessor->preprocess("Hello! How are you? I'm fine.", {"remove_punctuation"}), 
              "Hello How are you Im fine");
    
    // Text with no punctuation
    EXPECT_EQ(defaultProcessor->preprocess("Hello World", {"remove_punctuation"}), "Hello World");
    
    // Only punctuation
    EXPECT_EQ(defaultProcessor->preprocess(".,;:!?-", {"remove_punctuation"}), "");
    
    // Mixed punctuation and other characters
    EXPECT_EQ(defaultProcessor->preprocess("a,b.c;d:e!f?g-h", {"remove_punctuation"}), "abcdefgh");
}

/**
 * @brief Tests empty string handling
 * 
 * Verifies that the processor correctly handles empty strings
 * for both single operations and the full preprocessing pipeline.
 */
TEST_F(TextProcessorTest, HandlesEmptyString) {
    // Empty string with default pipeline
    EXPECT_EQ(defaultProcessor->preprocess(""), "");
    
    // Empty string with specific operations
    EXPECT_EQ(defaultProcessor->preprocess("", {"lowercase", "remove_punctuation"}), "");
    
    // Empty string with all operations
    std::vector<std::string> allOps = {
        "lowercase", "remove_punctuation", "remove_numbers", "normalize_whitespace",
        "remove_stopwords", "handle_negations", "process_hashtags", "remove_mentions",
        "remove_urls", "expand_abbreviations", "normalize_repeated_chars"
    };
    EXPECT_EQ(defaultProcessor->preprocess("", allOps), "");
}

/**
 * @brief Tests URL removal functionality
 * 
 * Verifies that various URL formats are correctly identified and removed
 * from the text while preserving the surrounding context.
 */
TEST_F(TextProcessorTest, RemovesURLs) {
    // Standard URLs
    EXPECT_EQ(defaultProcessor->preprocess("Check out https://example.com for more info", {"remove_urls"}), 
              "Check out  for more info");
    
    // URLs without protocol
    EXPECT_EQ(defaultProcessor->preprocess("Visit example.com/page for details", {"remove_urls"}), 
              "Visit  for details");
    
    // Multiple URLs
    EXPECT_EQ(defaultProcessor->preprocess("See https://site1.com and site2.net", {"remove_urls"}), 
              "See  and ");
    
    // URLs with query parameters
    EXPECT_EQ(defaultProcessor->preprocess("Link: https://example.com/path?query=test&param=value", {"remove_urls"}), 
              "Link: ");
    
    // URLs with fragments
    EXPECT_EQ(defaultProcessor->preprocess("Go to example.org/path#section", {"remove_urls"}), 
              "Go to ");
}

/**
 * @brief Tests hashtag processing functionality
 * 
 * Verifies that hashtags are correctly processed by removing the hash symbol
 * and separating camelCase or PascalCase into individual words.
 */
TEST_F(TextProcessorTest, ProcessesHashtags) {
    // Basic hashtag
    std::string result1 = defaultProcessor->preprocess("#TransRights", {"process_hashtags"});
    EXPECT_TRUE(result1.find("trans") != std::string::npos);
    EXPECT_TRUE(result1.find("rights") != std::string::npos);
    EXPECT_EQ(result1.find('#'), std::string::npos);
    
    // Multiple hashtags
    std::string result2 = defaultProcessor->preprocess("#HashTag #Multiple #Tags", {"process_hashtags"});
    EXPECT_TRUE(result2.find("hashtag") != std::string::npos);
    EXPECT_TRUE(result2.find("multiple") != std::string::npos);
    EXPECT_TRUE(result2.find("tags") != std::string::npos);
    EXPECT_EQ(result2.find('#'), std::string::npos);
    
    // Hashtags with numbers
    std::string result3 = defaultProcessor->preprocess("#Trans2024", {"process_hashtags"});
    EXPECT_TRUE(result3.find("trans") != std::string::npos);
    EXPECT_TRUE(result3.find("2024") != std::string::npos);
    
    // No hashtags
    EXPECT_EQ(defaultProcessor->preprocess("No hashtags here", {"process_hashtags"}), 
              "No hashtags here");
}

/**
 * @brief Tests mention removal functionality
 * 
 * Verifies that social media mentions (@username) are correctly
 * identified and removed from the text.
 */
TEST_F(TextProcessorTest, RemovesMentions) {
    // Basic mention
    EXPECT_EQ(defaultProcessor->preprocess("Hi @username how are you?", {"remove_mentions"}), 
              "Hi  how are you?");
    
    // Multiple mentions
    EXPECT_EQ(defaultProcessor->preprocess("Thanks @person1 and @person2", {"remove_mentions"}), 
              "Thanks  and ");
    
    // Mentions with underscores and numbers
    EXPECT_EQ(defaultProcessor->preprocess("CC: @user_name123", {"remove_mentions"}), 
              "CC: ");
    
    // No mentions
    EXPECT_EQ(defaultProcessor->preprocess("No mentions here", {"remove_mentions"}), 
              "No mentions here");
    
    // Email addresses should not be treated as mentions
    EXPECT_EQ(defaultProcessor->preprocess("Contact person@example.com", {"remove_mentions"}), 
              "Contact person@example.com");
}

/**
 * @brief Tests number removal functionality
 * 
 * Verifies that numeric digits are correctly removed from the text
 * while preserving non-numeric characters.
 */
TEST_F(TextProcessorTest, RemovesNumbers) {
    // Simple numbers
    EXPECT_EQ(defaultProcessor->preprocess("Hello 123 world", {"remove_numbers"}), 
              "Hello  world");
    
    // Mixed text and numbers
    EXPECT_EQ(defaultProcessor->preprocess("a1b2c3", {"remove_numbers"}), 
              "abc");
    
    // Only numbers
    EXPECT_EQ(defaultProcessor->preprocess("12345", {"remove_numbers"}), 
              "");
    
    // No numbers
    EXPECT_EQ(defaultProcessor->preprocess("Hello world", {"remove_numbers"}), 
              "Hello world");
}

/**
 * @brief Tests whitespace normalization functionality
 * 
 * Verifies that multiple consecutive spaces, tabs, and newlines
 * are normalized to a single space character.
 */
TEST_F(TextProcessorTest, NormalizesWhitespace) {
    // Multiple spaces
    EXPECT_EQ(defaultProcessor->preprocess("Hello    world", {"normalize_whitespace"}), 
              "Hello world");
    
    // Tabs and newlines
    EXPECT_EQ(defaultProcessor->preprocess("Hello\t\tworld\n\nnext line", {"normalize_whitespace"}), 
              "Hello world next line");
    
    // Leading and trailing whitespace
    EXPECT_EQ(defaultProcessor->preprocess("  Hello world  ", {"normalize_whitespace"}), 
              "Hello world");
    
    // Only whitespace
    EXPECT_EQ(defaultProcessor->preprocess("   \t\n  ", {"normalize_whitespace"}), 
              "");
}

/**
 * @brief Tests stopword removal functionality
 * 
 * Verifies that common stopwords are correctly identified and removed
 * from the text based on the configured stopword list.
 */
TEST_F(TextProcessorTest, RemovesStopwords) {
    // Default processor
    EXPECT_EQ(defaultProcessor->preprocess("the cat and the dog", {"remove_stopwords"}), 
              "cat dog");
    
    // Custom processor with limited stopwords
    EXPECT_EQ(customProcessor->preprocess("the cat and the dog with a hat", {"remove_stopwords"}), 
              "cat dog with a hat");
    
    // No stopwords
    EXPECT_EQ(defaultProcessor->preprocess("cat dog hat", {"remove_stopwords"}), 
              "cat dog hat");
    
    // Only stopwords
    EXPECT_EQ(defaultProcessor->preprocess("and the with a", {"remove_stopwords"}), 
              "");
}

/**
 * @brief Tests abbreviation expansion functionality
 * 
 * Verifies that common social media abbreviations are correctly
 * expanded to their full forms.
 */
TEST_F(TextProcessorTest, ExpandsAbbreviations) {
    // Common abbreviations
    std::string result = defaultProcessor->preprocess("btw idk lol", {"expand_abbreviations"});
    EXPECT_TRUE(result.find("by the way") != std::string::npos);
    EXPECT_TRUE(result.find("i do not know") != std::string::npos);
    EXPECT_TRUE(result.find("laugh") != std::string::npos);
    
    // Mixed abbreviations and regular words
    std::string result2 = defaultProcessor->preprocess("ty for the help", {"expand_abbreviations"});
    EXPECT_TRUE(result2.find("thank you") != std::string::npos);
    EXPECT_TRUE(result2.find("for the help") != std::string::npos);
    
    // No abbreviations
    EXPECT_EQ(defaultProcessor->preprocess("no abbreviations here", {"expand_abbreviations"}), 
              "no abbreviations here");
}

/**
 * @brief Tests negation handling functionality
 * 
 * Verifies that negation words are correctly identified and attached
 * to subsequent words within the specified scope.
 */
TEST_F(TextProcessorTest, HandlesNegations) {
    // Simple negation
    std::string result = defaultProcessor->preprocess("not good", {"handle_negations"});
    EXPECT_TRUE(result.find("NOT_good") != std::string::npos);
    
    // Multiple negations
    std::string result2 = defaultProcessor->preprocess("not good but never bad", {"handle_negations"});
    EXPECT_TRUE(result2.find("NOT_good") != std::string::npos);
    EXPECT_TRUE(result2.find("NOT_bad") != std::string::npos);
    
    // No negations
    EXPECT_EQ(defaultProcessor->preprocess("all good here", {"handle_negations"}), 
              "all good here");
    
    // Custom processor with limited negation words
    std::string result3 = customProcessor->preprocess("not good don't like", {"handle_negations"});
    EXPECT_TRUE(result3.find("NOT_good") != std::string::npos);
    EXPECT_FALSE(result3.find("NOT_like") != std::string::npos);  // "don't" not in custom list
}

/**
 * @brief Tests repeated character normalization
 * 
 * Verifies that repeated characters (like "soooo" -> "so") are
 * properly normalized while preserving the word's meaning.
 */
TEST_F(TextProcessorTest, NormalizesRepeatedChars) {
    // Repeated letters
    EXPECT_EQ(defaultProcessor->preprocess("sooooo goooood", {"normalize_repeated_chars"}), 
              "so good");
    
    // Mixed repeats
    EXPECT_EQ(defaultProcessor->preprocess("hiiiii thereeee", {"normalize_repeated_chars"}), 
              "hi there");
    
    // No repeats
    EXPECT_EQ(defaultProcessor->preprocess("normal text", {"normalize_repeated_chars"}), 
              "normal text");
    
    // Words with legitimate doubles
    std::string result = defaultProcessor->preprocess("happy", {"normalize_repeated_chars"});
    EXPECT_TRUE(result.find("hap") == std::string::npos);  // Shouldn't become "hapy"
    EXPECT_EQ(result, "happy");
}

/**
 * @brief Tests the full preprocessing pipeline
 * 
 * Verifies that the default preprocessing pipeline correctly applies
 * all operations in the expected sequence.
 */
TEST_F(TextProcessorTest, FullPreprocessingPipeline) {
    // No specific operations = default pipeline
    std::string input = "Hey @user! Check out #TransRights at https://example.com. It's REALLY good!!! btw idk what else to say...";
    std::string result = defaultProcessor->preprocess(input);
    
    // Verify results match expected transformations
    EXPECT_EQ(result.find('@'), std::string::npos);  // No mentions
    EXPECT_EQ(result.find('#'), std::string::npos);  // No hashtags
    EXPECT_EQ(result.find("https"), std::string::npos);  // No URLs
    EXPECT_EQ(result.find('!'), std::string::npos);  // No punctuation
    EXPECT_TRUE(result.find("trans") != std::string::npos);  // Hashtag processed
    EXPECT_TRUE(result.find("rights") != std::string::npos);  // Hashtag processed
    EXPECT_TRUE(result.find("really") != std::string::npos || 
                result.find("realy") != std::string::npos);  // Lowercase and possibly normalized repeats
    
    // Ensure specific transformations occurred
    EXPECT_TRUE(result.find("by the way") != std::string::npos);  // Expanded abbreviation
    EXPECT_TRUE(result.find("i do not know") != std::string::npos);  // Expanded abbreviation
}

/**
 * @brief Tests addPreprocessingStep functionality
 * 
 * Verifies that custom preprocessing steps can be added and used
 * correctly in the preprocessing pipeline.
 */
TEST_F(TextProcessorTest, AddsCustomPreprocessingStep) {
    // Create a custom preprocessing step
    auto customStep = [](std::string_view text) -> std::string {
        std::string result(text);
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        return result;
    };
    
    // Add the custom step
    auto processor = std::make_unique<blahajpi::preprocessing::TextProcessor>();
    processor->addPreprocessingStep("make_uppercase", customStep);
    
    // Test that the step works
    EXPECT_EQ(processor->preprocess("hello world", {"make_uppercase"}), 
              "HELLO WORLD");
    
    // Test with multiple steps including custom one
    EXPECT_EQ(processor->preprocess("Hello, world!", {"make_uppercase", "remove_punctuation"}), 
              "HELLO WORLD");
}

/**
 * @brief Tests adding new stopwords
 * 
 * Verifies that new stopwords can be added and are correctly used
 * in subsequent preprocessing operations.
 */
TEST_F(TextProcessorTest, AddsStopwords) {
    // Create processor and add custom stopwords
    auto processor = std::make_unique<blahajpi::preprocessing::TextProcessor>();
    processor->addStopwords({"cat", "dog", "bird"});
    
    // Test with both default and added stopwords
    std::string result = processor->preprocess("the cat and dog with a bird", {"remove_stopwords"});
    
    // The default stopwords should be removed
    EXPECT_EQ(result.find("the"), std::string::npos);
    EXPECT_EQ(result.find("and"), std::string::npos);
    EXPECT_EQ(result.find("with"), std::string::npos);
    EXPECT_EQ(result.find("a"), std::string::npos);
    
    // Custom stopwords should also be removed
    EXPECT_EQ(result.find("cat"), std::string::npos);
    EXPECT_EQ(result.find("dog"), std::string::npos);
    EXPECT_EQ(result.find("bird"), std::string::npos);
    
    // Result should be empty as all words are stopwords
    EXPECT_TRUE(result.empty());
}

/**
 * @brief Tests adding new negation words
 * 
 * Verifies that new negation words can be added and are correctly used
 * in subsequent preprocessing operations.
 */
TEST_F(TextProcessorTest, AddsNegationWords) {
    // Create processor and add custom negation words
    auto processor = std::make_unique<blahajpi::preprocessing::TextProcessor>();
    processor->addNegationWords({"avoid", "refuse"});
    
    // Test with both default and added negation words
    std::string result1 = processor->preprocess("not good", {"handle_negations"});
    std::string result2 = processor->preprocess("avoid bad", {"handle_negations"});
    
    // Default negation should work
    EXPECT_TRUE(result1.find("NOT_good") != std::string::npos);
    
    // Custom negation should also work
    EXPECT_TRUE(result2.find("NOT_bad") != std::string::npos);
}

} // namespace
