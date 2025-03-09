/**
 * @file word_cloud.hpp
 * @brief ASCII art word cloud generator for text visualization
 * 
 * This file provides functionality to generate ASCII art word clouds,
 * which visually represent the frequency of words in a text collection.
 * This is useful for visualizing which terms are most common in harmful
 * or safe content.
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <span>

namespace blahajpi {
namespace utils {

/**
 * @brief Generates ASCII art word clouds from text data
 * 
 * ASCII word clouds provide a visual representation of term frequency,
 * helping analysts understand key themes in the analyzed content.
 */
class WordCloud {
public:
    /**
     * @brief Configuration options for word cloud generation
     */
    struct CloudConfig {
        size_t maxWords = 30;      ///< Maximum number of words to include
        size_t width = 80;         ///< Width of the word cloud in characters
        size_t height = 15;        ///< Height of the word cloud in characters
        bool useColor = true;      ///< Whether to use ANSI color codes
        bool useBars = false;      ///< Whether to use bar-style visualization
        bool showFrequencies = false; ///< Whether to show word frequencies
    };

    /**
     * @brief Default constructor
     */
    WordCloud();
    
    /**
     * @brief Constructor with custom word lists
     * @param commonWords Set of common words to filter out
     * @param harmfulWords Set of words that typically indicate harmful content
     * @param safeWords Set of words that typically indicate safe content
     */
    WordCloud(
        const std::unordered_set<std::string>& commonWords,
        const std::unordered_set<std::string>& harmfulWords,
        const std::unordered_set<std::string>& safeWords
    );

    /**
     * @brief Generates a simple ASCII word cloud
     * @param texts Collection of texts to analyze
     * @param maxWords Maximum number of words to include
     * @param width Width of the word cloud in characters
     * @param height Height of the word cloud in characters
     * @param isHarmful Whether this is a harmful content cloud (affects styling)
     * @return Formatted string containing the ASCII word cloud
     */
    std::string generateWordCloud(
        const std::vector<std::string>& texts,
        size_t maxWords = 30, 
        size_t width = 80, 
        size_t height = 15,
        bool isHarmful = true
    );

    /**
     * @brief Generates a customized word cloud
     * @param texts Collection of texts to analyze
     * @param config Configuration options for the word cloud
     * @param isHarmful Whether this is a harmful content cloud (affects styling)
     * @return Formatted string containing the ASCII word cloud
     */
    std::string generateCustomCloud(
        const std::vector<std::string>& texts,
        const CloudConfig& config,
        bool isHarmful = true
    );

    /**
     * @brief Displays a word cloud directly to the console
     * @param texts Collection of texts to analyze
     * @param maxWords Maximum number of words to include
     * @param width Width of the word cloud in characters
     * @param height Height of the word cloud in characters
     * @param isHarmful Whether this is a harmful content cloud (affects styling)
     */
    void displayWordCloud(
        const std::vector<std::string>& texts,
        size_t maxWords = 30, 
        size_t width = 80, 
        size_t height = 15,
        bool isHarmful = true
    );
    
    /**
     * @brief Save a word cloud to a file
     * @param cloud The generated word cloud string
     * @param outputPath Path to save the word cloud
     * @return True if saving was successful
     */
    static bool saveWordCloud(
        const std::string& cloud,
        const std::string& outputPath
    );

private:
    /**
     * @brief Counts word frequencies in a collection of texts
     * @param texts Collection of texts to analyze
     * @return Map of words to frequency counts
     */
    std::unordered_map<std::string, int> countWordFrequencies(
        std::span<const std::string> texts
    );

    /**
     * @brief Gets the most frequent words
     * @param wordFreqs Map of word frequencies
     * @param maxWords Maximum number of words to return
     * @return Vector of (word, frequency) pairs sorted by frequency
     */
    std::vector<std::pair<std::string, int>> getTopWords(
        const std::unordered_map<std::string, int>& wordFreqs, 
        size_t maxWords
    );

    /**
     * @brief Formats a word for display in the word cloud
     * @param word The word to format
     * @param freq The word's frequency
     * @param maxFreq The maximum frequency in the dataset
     * @param isHarmful Whether this is a harmful content cloud
     * @return Formatted word string
     */
    std::string formatWord(
        const std::string& word, 
        int freq, 
        int maxFreq,
        bool isHarmful
    );

    /**
     * @brief Gets ANSI color code based on frequency and sentiment
     * @param freq Word frequency
     * @param maxFreq Maximum frequency
     * @param isHarmful Whether this is a harmful content cloud
     * @param word The word being colored
     * @return ANSI color code string
     */
    std::string getColorCode(
        int freq, 
        int maxFreq, 
        bool isHarmful, 
        const std::string& word = ""
    ) const;

    /**
     * @brief Resets ANSI color formatting
     * @return ANSI reset code
     */
    static std::string resetColor();
    
    std::unordered_set<std::string> commonWords;  ///< Common words to filter out
    std::unordered_set<std::string> harmfulWords; ///< Words indicating harmful content
    std::unordered_set<std::string> safeWords;    ///< Words indicating safe content
};

} // namespace utils
} // namespace blahajpi
