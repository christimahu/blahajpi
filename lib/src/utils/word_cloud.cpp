/**
 * @file word_cloud.cpp
 * @brief Implementation of the ASCII word cloud generator
 */

#include "blahajpi/utils/word_cloud.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <random>
#include <iomanip>
#include <cmath>
#include <cctype>
#include <fstream>

namespace blahajpi {
namespace utils {

WordCloud::WordCloud() {
    // Initialize common words
    commonWords = {
        "the", "and", "that", "have", "for", "not", "with", "you", "this", "but",
        "his", "from", "they", "she", "will", "one", "all", "would", "there", "their",
        "what", "out", "about", "who", "get", "which", "when", "make", "can", "like",
        "time", "just", "him", "know", "take", "people", "into", "year", "your", "good",
        "some", "could", "them", "see", "other", "than", "then", "now", "look", "only",
        "come", "its", "over", "think", "also", "back", "after", "use", "two", "how",
        "our", "work", "first", "well", "way", "even", "new", "want", "because", "any",
        "these", "give", "day", "most", "say", "was", "been", "were", "being", "are"
    };
    
    // Initialize words indicating harmful content
    harmfulWords = {
        "hate", "kill", "attack", "terrible", "disgusting", "wrong", "sick", "fake",
        "evil", "disgusting", "abnormal", "mental", "illness", "disease", "disorder",
        "freak", "weird", "confused", "delusional", "agenda", "indoctrinate", "recruit",
        "mutilate", "dangerous", "threat", "groom", "predator", "pervert", "abomination",
        "unnatural", "deviant", "ridiculous", "stupid", "insane", "crazy", "deny", "erase",
        "harmful", "violence", "harass", "target", "ban", "illegal", "criminal", "immoral",
        "sin", "degenerate", "destroy", "ideology", "propaganda", "brainwash", "push"
    };
    
    // Initialize words indicating safe content
    safeWords = {
        "support", "protect", "rights", "equal", "human", "dignity", "respect", "ally",
        "affirm", "accept", "valid", "authentic", "real", "true", "health", "care",
        "help", "safe", "protect", "community", "solidarity", "embrace", "celebrate",
        "diverse", "diversity", "inclusion", "inclusive", "accept", "acceptance",
        "understand", "understanding", "empathy", "compassion", "kind", "kindness",
        "love", "identity", "expression", "self", "represent", "representation",
        "visibility", "visible", "voice", "justice", "equality", "equity", "freedom"
    };
}

WordCloud::WordCloud(
    const std::unordered_set<std::string>& customCommonWords,
    const std::unordered_set<std::string>& customHarmfulWords,
    const std::unordered_set<std::string>& customSafeWords
) : commonWords(customCommonWords),
    harmfulWords(customHarmfulWords),
    safeWords(customSafeWords) {
}

std::string WordCloud::generateWordCloud(
    const std::vector<std::string>& texts,
    size_t maxWords, 
    size_t width, 
    size_t height,
    bool isHarmful
) {
    // Use the CloudConfig struct for configuration
    CloudConfig config;
    config.maxWords = maxWords;
    config.width = width;
    config.height = height;
    config.useColor = true;
    
    return generateCustomCloud(texts, config, isHarmful);
}

std::string WordCloud::generateCustomCloud(
    const std::vector<std::string>& texts,
    const CloudConfig& config,
    bool isHarmful
) {
    // Count word frequencies
    auto wordFreqs = countWordFrequencies(std::span{texts});
    
    // Get top words
    auto topWords = getTopWords(wordFreqs, config.maxWords);
    
    if (topWords.empty()) {
        return "No words found to create a word cloud.";
    }
    
    // Find the maximum frequency for scaling
    int maxFreq = topWords[0].second;
    
    // Prepare the output
    std::ostringstream cloud;
    
    // Add a title
    cloud << "Word Frequency Visualization";
    cloud << (isHarmful ? " (Harmful Content)" : " (Safe Content)") << "\n\n";
    
    // Two possible visualization styles
    if (config.useBars) {
        // Bar chart style
        for (const auto& [word, freq] : topWords) {
            // Calculate bar length scaled to width
            int barLength = static_cast<int>(
                (static_cast<double>(freq) / maxFreq) * (config.width - 20)
            );
            
            // Format the word and frequency
            cloud << std::left << std::setw(15) << word;
            cloud << " ";
            
            // Add the bar
            if (config.useColor) {
                cloud << getColorCode(freq, maxFreq, isHarmful, word);
            }
            
            for (int i = 0; i < barLength; ++i) {
                cloud << "█";
            }
            
            if (config.useColor) {
                cloud << resetColor();
            }
            
            // Show frequency if requested
            if (config.showFrequencies) {
                cloud << " (" << freq << ")";
            }
            
            cloud << "\n";
        }
    } else {
        // Word cloud style - simplified ASCII version
        for (const auto& [word, freq] : topWords) {
            cloud << formatWord(word, freq, maxFreq, isHarmful);
            
            // Show frequency if requested
            if (config.showFrequencies) {
                cloud << " (" << freq << ")";
            }
            
            cloud << " ";
            
            // Rough word wrapping based on width
            if (cloud.tellp() % config.width > config.width - 20) {
                cloud << "\n";
            }
        }
    }
    
    return cloud.str();
}

void WordCloud::displayWordCloud(
    const std::vector<std::string>& texts,
    size_t maxWords, 
    size_t width, 
    size_t height,
    bool isHarmful
) {
    // Generate the word cloud
    std::string cloudText = generateWordCloud(texts, maxWords, width, height, isHarmful);
    
    // Display directly to console
    std::cout << cloudText << std::endl;
}

bool WordCloud::saveWordCloud(
    const std::string& cloud,
    const std::string& outputPath
) {
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << outputPath << std::endl;
        return false;
    }
    
    outFile << cloud;
    outFile.close();
    
    return true;
}

std::unordered_map<std::string, int> WordCloud::countWordFrequencies(
    std::span<const std::string> texts
) {
    std::unordered_map<std::string, int> wordFreqs;
    
    // Process each text
    for (const auto& text : texts) {
        std::istringstream iss{text};
        std::string word;
        
        // Count each word
        while (iss >> word) {
            // Filter out very short words
            if (word.length() < 3) {
                continue;
            }
            
            // Skip common words
            if (commonWords.contains(word)) {
                continue;
            }
            
            // Check for word categories
            if (harmfulWords.contains(word)) {
                // Harmful words get extra weight
                wordFreqs[word] += 3;
            }
            else if (safeWords.contains(word)) {
                // Safe words get double weight
                wordFreqs[word] += 2;
            }
            else {
                // Regular words get normal weight
                wordFreqs[word]++;
            }
        }
    }
    
    return wordFreqs;
}

std::vector<std::pair<std::string, int>> WordCloud::getTopWords(
    const std::unordered_map<std::string, int>& wordFreqs, 
    size_t maxWords
) {
    // Convert map to vector for sorting
    std::vector<std::pair<std::string, int>> sortedWords;
    sortedWords.reserve(wordFreqs.size());
    
    for (const auto& [word, freq] : wordFreqs) {
        sortedWords.emplace_back(word, freq);
    }
    
    // Sort by frequency (descending)
    std::sort(
        sortedWords.begin(), sortedWords.end(), 
        [](const auto& a, const auto& b) {
            return a.second > b.second;
        }
    );
    
    // Limit to maxWords
    if (sortedWords.size() > maxWords) {
        sortedWords.resize(maxWords);
    }
    
    return sortedWords;
}

std::string WordCloud::formatWord(
    const std::string& word, 
    int freq, 
    int maxFreq,
    bool isHarmful
) {
    std::ostringstream formatted;
    
    // Determine if this word has a sentiment indicator
    bool isHarmfulWord = harmfulWords.contains(word);
    bool isSafeWord = safeWords.contains(word);
    
    // Apply color coding based on frequency and sentiment
    formatted << getColorCode(freq, maxFreq, isHarmful, word);
    
    // Format the word based on relative frequency and sentiment
    double relativeFreq = static_cast<double>(freq) / maxFreq;
    
    if (relativeFreq > 0.8 || isHarmfulWord || isSafeWord) {
        // Very common or sentiment-indicative words - uppercase and bold
        std::string upperWord = word;
        std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        formatted << upperWord;
    } else if (relativeFreq > 0.5) {
        // Common words - uppercase
        std::string upperWord = word;
        std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        formatted << upperWord;
    } else {
        // Less common words - normal
        formatted << word;
    }
    
    // Reset the color
    formatted << resetColor();
    
    return formatted.str();
}

std::string WordCloud::getColorCode(
    int freq, 
    int maxFreq, 
    bool isHarmful, 
    const std::string& word
) const {
    double relativeFreq = static_cast<double>(freq) / maxFreq;
    
    // Scale: 0-6 represents different intensities
    int colorScale = static_cast<int>(relativeFreq * 6);
    
    // Check word categories
    bool isHarmfulWord = harmfulWords.contains(word);
    bool isSafeWord = safeWords.contains(word);
    
    // ANSI color codes
    if (isHarmful) {
        // Override for sentiment words
        if (isHarmfulWord) {
            return "\033[38;5;196m";  // Bright red for harmful words
        } else if (isSafeWord) {
            return "\033[38;5;34m";   // Green for safe words in harmful context
        }
        
        // Red to yellow scale for harmful sentiment
        switch (colorScale) {
            case 0: return "\033[38;5;196m"; // Bright red
            case 1: return "\033[38;5;202m";
            case 2: return "\033[38;5;208m";
            case 3: return "\033[38;5;214m";
            case 4: return "\033[38;5;220m";
            case 5: return "\033[38;5;226m";
            case 6: return "\033[38;5;227m"; // Yellow
            default: return "\033[38;5;196m"; // Default bright red
        }
    } else {
        // Override for sentiment words
        if (isSafeWord) {
            return "\033[38;5;46m";  // Bright green for safe words
        } else if (isHarmfulWord) {
            return "\033[38;5;196m"; // Red for harmful words in safe context
        }
        
        // Blue to green scale for safe sentiment
        switch (colorScale) {
            case 0: return "\033[38;5;39m";  // Light blue
            case 1: return "\033[38;5;38m";
            case 2: return "\033[38;5;37m";
            case 3: return "\033[38;5;36m";
            case 4: return "\033[38;5;35m";
            case 5: return "\033[38;5;34m";
            case 6: return "\033[38;5;46m";  // Bright green
            default: return "\033[38;5;39m"; // Default light blue
        }
    }
}

std::string WordCloud::resetColor() {
    return "\033[0m";
}

} // namespace utils
} // namespace blahajpi
