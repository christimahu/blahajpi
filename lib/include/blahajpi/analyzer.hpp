/**
 * @file analyzer.hpp
 * @brief Main interface for the Blahaj PI sentiment analysis
 * 
 * This file defines the primary interface for analyzing social media content
 * to identify potentially harmful content targeting transgender individuals.
 * 
 * @author Christi Mahu
 * @date 2025
 */

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <unordered_map>

namespace blahajpi {

// Forward declaration for CloudConfig
namespace utils {
    // Forward declare the CloudConfig struct directly instead of as part of WordCloud
    struct CloudConfig {
        size_t maxWords = 30;      ///< Maximum number of words to include
        size_t width = 80;         ///< Width of the word cloud in characters
        size_t height = 15;        ///< Height of the word cloud in characters
        bool useColor = true;      ///< Whether to use ANSI color codes
        bool useBars = false;      ///< Whether to use bar-style visualization
        bool showFrequencies = false; ///< Whether to show word frequencies
    };
    
    // Forward declare the WordCloud class separately
    class WordCloud;
}

/**
 * @brief Result of a sentiment analysis operation
 * 
 * Contains the analysis result including sentiment classification,
 * confidence score, and explanatory details.
 */
struct AnalysisResult {
    std::string text;             ///< Original input text
    std::string cleanedText;      ///< Preprocessed text
    std::string sentiment;        ///< "Harmful" or "Safe"
    double harmScore;             ///< Numeric score (higher = more harmful, range 0-1)
    double confidence;            ///< Confidence in the classification (0-1)
    std::string explanation;      ///< Human-readable explanation of the result
    std::vector<std::string> keyTerms; ///< Terms that contributed to the classification
    
    /**
     * @brief Convert result to a map for serialization
     * @return Map representation of the result
     */
    std::unordered_map<std::string, std::string> toMap() const;
    
    /**
     * @brief Create a result from a map representation
     * @param map Map containing result properties
     * @return Constructed AnalysisResult
     */
    static AnalysisResult fromMap(const std::unordered_map<std::string, std::string>& map);
};

// Forward declaration of implementation class
class AnalyzerImpl;

/**
 * @brief Main class for sentiment analysis of social media content
 * 
 * The Analyzer class provides methods to analyze text content for
 * potentially harmful content targeting transgender individuals.
 */
class Analyzer {
public:
    /**
     * @brief Default constructor
     */
    Analyzer();
    
    /**
     * @brief Constructor with explicit configuration path
     * @param configPath Path to configuration file
     */
    explicit Analyzer(const std::string& configPath);
    
    /**
     * @brief Destructor
     */
    ~Analyzer();
    
    /**
     * @brief Analyze text for harmful content
     * @param text Text to analyze
     * @return Analysis result
     */
    AnalysisResult analyze(const std::string& text);
    
    /**
     * @brief Analyze multiple texts
     * @param texts Collection of texts to analyze
     * @return Vector of analysis results
     */
    std::vector<AnalysisResult> analyzeMultiple(const std::vector<std::string>& texts);
    
    /**
     * @brief Load a model from a specified path
     * @param modelPath Path to the model directory
     * @return True if loading was successful
     */
    bool loadModel(const std::string& modelPath);
    
    /**
     * @brief Train a new model from labeled data
     * @param dataPath Path to labeled dataset file
     * @param outputPath Path to save the trained model
     * @return True if training was successful
     */
    bool trainModel(const std::string& dataPath, const std::string& outputPath);
    
    /**
     * @brief Generate a word cloud visualization
     * @param analysisResults Analysis results to visualize
     * @param outputPath Path to save the visualization
     * @param harmfulOnly Whether to include only harmful content
     * @return True if generation was successful
     */
    bool generateVisualization(
        const std::vector<AnalysisResult>& analysisResults,
        const std::string& outputPath,
        bool harmfulOnly = true);
    
    /**
     * @brief Generate a custom word cloud visualization with configuration
     * @param analysisResults Analysis results to visualize
     * @param outputPath Path to save the visualization
     * @param harmfulOnly Whether to include only harmful content
     * @param config Configuration for the word cloud
     * @return True if generation was successful
     */
    bool generateCustomVisualization(
        const std::vector<AnalysisResult>& analysisResults,
        const std::string& outputPath,
        bool harmfulOnly = true,
        const utils::CloudConfig& config = {});
    
    /**
     * @brief Get the current configuration
     * @return Map of configuration parameters
     */
    std::unordered_map<std::string, std::string> getConfig() const;
    
    /**
     * @brief Set a configuration parameter
     * @param key Parameter name
     * @param value Parameter value
     */
    void setConfig(const std::string& key, const std::string& value);
    
    /**
     * @brief Load configuration from a file
     * @param configPath Path to the configuration file
     * @return True if loading was successful
     */
    bool loadConfig(const std::string& configPath);

private:
    // Implementation details are in a separate class
    std::unique_ptr<AnalyzerImpl> pImpl;
};

} // namespace blahajpi
