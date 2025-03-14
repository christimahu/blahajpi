/**
 * @file text_processor.hpp
 * @brief Text preprocessing utilities for sentiment analysis
 * 
 * This file provides functionality for cleaning and normalizing text data
 * before feature extraction and analysis. It handles operations like
 * lowercasing, removing punctuation, and handling special cases for
 * social media content.
 */

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace blahajpi {
namespace preprocessing {

/**
 * @brief Handles text preprocessing for sentiment analysis
 * 
 * Provides methods to clean and normalize text by applying various
 * transformations like lowercasing, removing punctuation, handling
 * negations, and removing stopwords.
 */
class TextProcessor {
public:
    /**
     * @brief Default constructor initializes internal data structures
     */
    TextProcessor();
    
    /**
     * @brief Constructor with custom stopwords and negation words
     * @param stopwords Set of words to ignore during processing
     * @param negationWords Set of words that indicate negation
     */
    TextProcessor(
        const std::unordered_set<std::string>& stopwords,
        const std::unordered_set<std::string>& negationWords
    );
    
    /**
     * @brief Applies a sequence of preprocessing steps to text
     * @param text Input text to process
     * @param steps Vector of preprocessing step names, empty for default pipeline
     * @return Preprocessed text
     */
    std::string preprocess(std::string_view text, 
                         const std::vector<std::string>& steps = {}) const;
    
    /**
     * @brief Get all available preprocessing functions
     * @return Map of function names to implementation functions
     */
    std::unordered_map<std::string, std::function<std::string(std::string_view)>> 
    getPreprocessingFunctions() const;
    
    /**
     * @brief Add custom preprocessing step
     * @param name Name of the preprocessing step
     * @param func Implementation function
     */
    void addPreprocessingStep(
        const std::string& name,
        std::function<std::string(std::string_view)> func
    );
    
    /**
     * @brief Add words to the stopword list
     * @param words Words to add to stopwords
     */
    void addStopwords(const std::vector<std::string>& words);
    
    /**
     * @brief Add words to the negation words list
     * @param words Words to add to negation words
     */
    void addNegationWords(const std::vector<std::string>& words);

private:
    /**
     * @brief Type alias for text transformation functions
     */
    using PreprocessingFunc = std::function<std::string(std::string_view)>;
    
    /**
     * @brief Map of preprocessing function names to implementations
     */
    std::unordered_map<std::string, PreprocessingFunc> preprocessingFunctions;
    
    /**
     * @brief Set of stopwords to remove during preprocessing
     */
    std::unordered_set<std::string> stopwords;
    
    /**
     * @brief Set of words that indicate negation
     */
    std::unordered_set<std::string> negationWords;
    
    /**
     * @brief Initialize default preprocessing functions
     */
    void initializePreprocessingFunctions();
    
    // Standard preprocessing functions
    
    /**
     * @brief Converts text to lowercase
     * @param text Input text
     * @return Lowercase text
     */
    std::string lowercase(std::string_view text) const;
    
    /**
     * @brief Removes punctuation characters
     * @param text Input text
     * @return Text without punctuation
     */
    std::string removePunctuation(std::string_view text) const;
    
    /**
     * @brief Removes numerical digits
     * @param text Input text
     * @return Text without numbers
     */
    std::string removeNumbers(std::string_view text) const;
    
    /**
     * @brief Removes non-ASCII characters
     * @param text Input text
     * @return Text with only ASCII characters
     */
    std::string removeNonAscii(std::string_view text) const;
    
    /**
     * @brief Normalizes whitespace
     * @param text Input text
     * @return Text with normalized whitespace
     */
    std::string normalizeWhitespace(std::string_view text) const;
    
    /**
     * @brief Removes common stopwords
     * @param text Input text
     * @return Text without stopwords
     */
    std::string removeStopwords(std::string_view text) const;
    
    /**
     * @brief Handles negations by marking negated words
     * @param text Input text
     * @return Text with negations handled
     */
    std::string handleNegations(std::string_view text) const;
    
    /**
     * @brief Handles common social media abbreviations
     * @param text Input text
     * @return Text with expanded abbreviations
     */
    std::string expandAbbreviations(std::string_view text) const;
    
    /**
     * @brief Normalizes repeated characters (e.g., "soooo" -> "so")
     * @param text Input text
     * @return Text with normalized repeated characters
     */
    std::string normalizeRepeatedChars(std::string_view text) const;
    
    /**
     * @brief Processes hashtags (e.g., "#TransRights" -> "trans rights")
     * @param text Input text
     * @return Text with processed hashtags
     */
    std::string processHashtags(std::string_view text) const;
    
    /**
     * @brief Removes mentions (e.g., "@username")
     * @param text Input text
     * @return Text without mentions
     */
    std::string removeMentions(std::string_view text) const;
    
    /**
     * @brief Removes URLs
     * @param text Input text
     * @return Text without URLs
     */
    std::string removeUrls(std::string_view text) const;
};

} // namespace preprocessing
} // namespace blahajpi
