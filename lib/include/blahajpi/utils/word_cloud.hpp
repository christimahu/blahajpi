/**
 * @file word_cloud.hpp
 * @brief ASCII art word cloud generator for text visualization
 */

#pragma once

#include "blahajpi/analyzer.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <span>

namespace blahajpi {
namespace utils {

/**
 * @brief Generates ASCII art word clouds from text data
 */
class WordCloud {
public:
    /**
     * @brief Default constructor
     */
    WordCloud();
    
    /**
     * @brief Constructor with custom word lists
     */
    WordCloud(
        const std::unordered_set<std::string>& commonWords,
        const std::unordered_set<std::string>& harmfulWords,
        const std::unordered_set<std::string>& safeWords
    );

    /**
     * @brief Generates a simple ASCII word cloud
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
     */
    std::string generateCustomCloud(
        const std::vector<std::string>& texts,
        const CloudConfig& config,
        bool isHarmful = true
    );

    /**
     * @brief Displays a word cloud directly to the console
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
     */
    static bool saveWordCloud(
        const std::string& cloud,
        const std::string& outputPath
    );

private:
    /**
     * @brief Counts word frequencies in a collection of texts
     */
    std::unordered_map<std::string, int> countWordFrequencies(
        std::span<const std::string> texts
    );

    /**
     * @brief Gets the most frequent words
     */
    std::vector<std::pair<std::string, int>> getTopWords(
        const std::unordered_map<std::string, int>& wordFreqs, 
        size_t maxWords
    );

    /**
     * @brief Formats a word for display in the word cloud
     */
    std::string formatWord(
        const std::string& word, 
        int freq, 
        int maxFreq,
        bool isHarmful
    );

    /**
     * @brief Gets ANSI color code based on frequency and sentiment
     */
    std::string getColorCode(
        int freq, 
        int maxFreq, 
        bool isHarmful, 
        const std::string& word = ""
    ) const;

    /**
     * @brief Resets ANSI color formatting
     */
    static std::string resetColor();
    
    std::unordered_set<std::string> commonWords;  ///< Common words to filter out
    std::unordered_set<std::string> harmfulWords; ///< Words indicating harmful content
    std::unordered_set<std::string> safeWords;    ///< Words indicating safe content
};

} // namespace utils
} // namespace blahajpi
