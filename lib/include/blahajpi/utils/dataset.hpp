/**
 * @file dataset.hpp
 * @brief Dataset management for sentiment analysis
 * 
 * This file provides functionality for loading, preprocessing, and managing
 * datasets for training and evaluation. It handles data from various social
 * media sources and supports splitting data for training and testing.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <utility>
#include <memory>

namespace blahajpi {
namespace utils {

/**
 * @brief Manages data for content analysis and model training
 * 
 * This class handles loading, preprocessing, and organizing data
 * from different sources for training and evaluating models.
 */
class Dataset {
public:
    /**
     * @brief Supported dataset formats
     */
    enum class Format {
        CSV,       ///< Comma-separated values
        TSV,       ///< Tab-separated values
        JSON,      ///< JSON format
        AUTO       ///< Auto-detect format from file extension
    };
    
    /**
     * @brief Default constructor
     */
    Dataset() = default;
    
    /**
     * @brief Constructor with initial data
     * @param data Vector of (label, text) pairs
     */
    explicit Dataset(std::vector<std::pair<int, std::string>> data);
    
    /**
     * @brief Loads data from a file
     * @param filePath Path to the data file
     * @param format Format of the data file
     * @param labelColumn Name of the column containing labels
     * @param textColumn Name of the column containing text
     * @return True if loading was successful
     */
    bool loadFromFile(
        const std::string& filePath,
        Format format = Format::AUTO,
        const std::string& labelColumn = "label",
        const std::string& textColumn = "text"
    );
    
    /**
     * @brief Saves data to a file
     * @param filePath Path to save the data
     * @param format Format to use for saving
     * @return True if saving was successful
     */
    bool saveToFile(
        const std::string& filePath,
        Format format = Format::AUTO
    ) const;
    
    /**
     * @brief Splits data into training and test sets
     * @param testSize Fraction of data to use for testing (0.0-1.0)
     * @param stratify Whether to maintain class distribution
     * @param randomSeed Seed for random number generation
     */
    void splitTrainTest(
        double testSize = 0.25,
        bool stratify = true,
        unsigned int randomSeed = 42
    );
    
    /**
     * @brief Gets training data
     * @return Vector of (label, text) pairs for training
     */
    std::vector<std::pair<int, std::string>> getTrainData() const;
    
    /**
     * @brief Gets test data
     * @return Vector of (label, text) pairs for testing
     */
    std::vector<std::pair<int, std::string>> getTestData() const;
    
    /**
     * @brief Gets training texts
     * @return Vector of text samples for training
     */
    std::vector<std::string> getTrainTexts() const;
    
    /**
     * @brief Gets test texts
     * @return Vector of text samples for testing
     */
    std::vector<std::string> getTestTexts() const;
    
    /**
     * @brief Gets training labels
     * @return Vector of labels for training
     */
    std::vector<int> getTrainLabels() const;
    
    /**
     * @brief Gets test labels
     * @return Vector of labels for testing
     */
    std::vector<int> getTestLabels() const;
    
    /**
     * @brief Gets texts with a specific label
     * @param label Label value to filter by
     * @return Vector of texts with the specified label
     */
    std::vector<std::string> getTextsWithLabel(int label) const;
    
    /**
     * @brief Gets the number of samples in the dataset
     * @return Number of samples
     */
    size_t size() const;
    
    /**
     * @brief Gets the distribution of labels
     * @return Map of label values to counts
     */
    std::unordered_map<int, size_t> getLabelDistribution() const;
    
    /**
     * @brief Gets basic statistics about the dataset
     * @return Map of statistic names to values
     */
    std::unordered_map<std::string, std::string> getStatistics() const;

private:
    std::vector<std::pair<int, std::string>> data;      ///< Raw data (label, text)
    std::vector<std::string> cleanedTexts;              ///< Preprocessed texts
    std::vector<int> labels;                           ///< Extracted labels
    std::vector<size_t> trainIndices;                  ///< Indices for training samples
    std::vector<size_t> testIndices;                   ///< Indices for test samples
    
    /**
     * @brief Loads data from a CSV file
     * @param filePath Path to the CSV file
     * @param labelColumn Name of the column containing labels
     * @param textColumn Name of the column containing text
     * @return True if loading was successful
     */
    bool loadFromCSV(
        const std::string& filePath,
        const std::string& labelColumn,
        const std::string& textColumn
    );
    
    /**
     * @brief Loads data from a JSON file
     * @param filePath Path to the JSON file
     * @param labelField Name of the field containing labels
     * @param textField Name of the field containing text
     * @return True if loading was successful
     */
    bool loadFromJSON(
        const std::string& filePath,
        const std::string& labelField,
        const std::string& textField
    );
    
    /**
     * @brief Loads data from a TSV file
     * @param filePath Path to the TSV file
     * @param labelColumn Name of the column containing labels
     * @param textColumn Name of the column containing text
     * @return True if loading was successful
     */
    bool loadFromTSV(
        const std::string& filePath,
        const std::string& labelColumn,
        const std::string& textColumn
    );
};

} // namespace utils
} // namespace blahajpi
