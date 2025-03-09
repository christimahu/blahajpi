/**
 * @file dataset.cpp
 * @brief Implementation of the Dataset class
 */

#include "blahajpi/utils/dataset.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <random>
#include <cctype>
#include <filesystem>
#include <stdexcept>

namespace blahajpi {
namespace utils {

Dataset::Dataset(std::vector<std::pair<int, std::string>> data) : data(std::move(data)) {
    // Extract labels
    labels.reserve(this->data.size());
    for (const auto& [label, _] : this->data) {
        labels.push_back(label);
    }
}

bool Dataset::loadFromFile(
    const std::string& filePath,
    Format format,
    const std::string& labelColumn,
    const std::string& textColumn
) {
    // Auto-detect format from file extension if requested
    Format actualFormat = format;
    if (format == Format::AUTO) {
        std::string extension = std::filesystem::path(filePath).extension().string();
        if (extension == ".csv") {
            actualFormat = Format::CSV;
        } else if (extension == ".tsv") {
            actualFormat = Format::TSV;
        } else if (extension == ".json") {
            actualFormat = Format::JSON;
        } else {
            std::cerr << "Warning: Could not auto-detect format from extension: " << extension << std::endl;
            std::cerr << "Defaulting to CSV format." << std::endl;
            actualFormat = Format::CSV;
        }
    }
    
    // Call the appropriate loader based on format
    switch (actualFormat) {
        case Format::CSV:
            return loadFromCSV(filePath, labelColumn, textColumn);
        case Format::TSV:
            return loadFromTSV(filePath, labelColumn, textColumn);
        case Format::JSON:
            return loadFromJSON(filePath, labelColumn, textColumn);
        default:
            std::cerr << "Error: Unsupported format" << std::endl;
            return false;
    }
}

bool Dataset::saveToFile(
    const std::string& filePath,
    Format format
) const {
    // Auto-detect format from file extension if requested
    Format actualFormat = format;
    if (format == Format::AUTO) {
        std::string extension = std::filesystem::path(filePath).extension().string();
        if (extension == ".csv") {
            actualFormat = Format::CSV;
        } else if (extension == ".tsv") {
            actualFormat = Format::TSV;
        } else if (extension == ".json") {
            actualFormat = Format::JSON;
        } else {
            std::cerr << "Warning: Could not auto-detect format from extension: " << extension << std::endl;
            std::cerr << "Defaulting to CSV format." << std::endl;
            actualFormat = Format::CSV;
        }
    }
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filePath << std::endl;
        return false;
    }
    
    try {
        // Write based on format
        switch (actualFormat) {
            case Format::CSV:
                // Write CSV header
                file << "label,text" << std::endl;
                
                // Write data rows
                for (const auto& [label, text] : data) {
                    // Escape text: replace quotes with double quotes
                    std::string escapedText = text;
                    size_t pos = 0;
                    while ((pos = escapedText.find('"', pos)) != std::string::npos) {
                        escapedText.replace(pos, 1, "\"\"");
                        pos += 2;
                    }
                    
                    // Write row with text in quotes
                    file << label << ",\"" << escapedText << "\"" << std::endl;
                }
                break;
                
            case Format::TSV:
                // Write TSV header
                file << "label\ttext" << std::endl;
                
                // Write data rows
                for (const auto& [label, text] : data) {
                    // Escape text: replace tabs with spaces
                    std::string escapedText = text;
                    std::replace(escapedText.begin(), escapedText.end(), '\t', ' ');
                    
                    // Write row
                    file << label << "\t" << escapedText << std::endl;
                }
                break;
                
            case Format::JSON:
                // Write JSON array opening
                file << "[\n";
                
                // Write data rows
                for (size_t i = 0; i < data.size(); ++i) {
                    const auto& [label, text] = data[i];
                    
                    // Escape text for JSON
                    std::string escapedText = text;
                    size_t pos = 0;
                    while ((pos = escapedText.find('"', pos)) != std::string::npos) {
                        escapedText.replace(pos, 1, "\\\"");
                        pos += 2;
                    }
                    pos = 0;
                    while ((pos = escapedText.find('\n', pos)) != std::string::npos) {
                        escapedText.replace(pos, 1, "\\n");
                        pos += 2;
                    }
                    
                    // Write JSON object
                    file << "  {\"label\": " << label << ", \"text\": \"" << escapedText << "\"}";
                    
                    // Add comma if not the last item
                    if (i < data.size() - 1) {
                        file << ",";
                    }
                    
                    file << "\n";
                }
                
                // Write JSON array closing
                file << "]\n";
                break;
                
            default:
                std::cerr << "Error: Unsupported format for saving" << std::endl;
                return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving dataset: " << e.what() << std::endl;
        return false;
    }
}

void Dataset::splitTrainTest(
    double testSize,
    bool stratify,
    unsigned int randomSeed
) {
    if (data.empty()) {
        throw std::runtime_error("Dataset is empty. Cannot split.");
    }
    
    size_t datasetSize = data.size();
    size_t testCount;
    
    // Convert testSize to absolute count if it's a fraction
    if (testSize < 1.0) {
        testCount = static_cast<size_t>(testSize * datasetSize);
    } else {
        testCount = static_cast<size_t>(testSize);
    }
    
    // Ensure valid test count
    testCount = std::max(size_t(1), std::min(testCount, datasetSize - 1));
    
    // Clear existing train/test indices
    trainIndices.clear();
    testIndices.clear();
    
    if (stratify) {
        // Stratified split to maintain class distribution
        
        // Get label distribution
        auto distribution = getLabelDistribution();
        
        // Create a random engine
        std::mt19937 g(randomSeed);
        
        // For each label, split proportionally
        for (const auto& [label, count] : distribution) {
            // Get indices for this label
            std::vector<size_t> labelIndices;
            for (size_t i = 0; i < data.size(); ++i) {
                if (data[i].first == label) {
                    labelIndices.push_back(i);
                }
            }
            
            // Shuffle indices for this label
            std::shuffle(labelIndices.begin(), labelIndices.end(), g);
            
            // Calculate test count for this label
            size_t labelTestCount = static_cast<size_t>(
                testCount * static_cast<double>(count) / datasetSize
            );
            
            // Ensure at least one test sample if there are samples
            if (labelTestCount == 0 && !labelIndices.empty()) {
                labelTestCount = 1;
            }
            
            // Ensure we don't take all samples
            labelTestCount = std::min(labelTestCount, labelIndices.size() - 1);
            
            // Add test indices for this label
            testIndices.insert(
                testIndices.end(),
                labelIndices.begin(),
                labelIndices.begin() + labelTestCount
            );
            
            // Add train indices for this label
            trainIndices.insert(
                trainIndices.end(),
                labelIndices.begin() + labelTestCount,
                labelIndices.end()
            );
        }
    } else {
        // Simple random split
        
        // Create indices for all data points
        std::vector<size_t> indices(datasetSize);
        std::iota(indices.begin(), indices.end(), 0);
        
        // Shuffle indices
        std::mt19937 g(randomSeed);
        std::shuffle(indices.begin(), indices.end(), g);
        
        // Split into train and test
        testIndices.assign(indices.begin(), indices.begin() + testCount);
        trainIndices.assign(indices.begin() + testCount, indices.end());
    }
    
    // Print split information
    std::cout << "Dataset split: " << trainIndices.size() << " training samples, "
              << testIndices.size() << " test samples" << std::endl;
}

std::vector<std::pair<int, std::string>> Dataset::getTrainData() const {
    std::vector<std::pair<int, std::string>> trainData;
    trainData.reserve(trainIndices.size());
    
    for (size_t idx : trainIndices) {
        if (idx < data.size()) {
            trainData.push_back(data[idx]);
        }
    }
    
    return trainData;
}

std::vector<std::pair<int, std::string>> Dataset::getTestData() const {
    std::vector<std::pair<int, std::string>> testData;
    testData.reserve(testIndices.size());
    
    for (size_t idx : testIndices) {
        if (idx < data.size()) {
            testData.push_back(data[idx]);
        }
    }
    
    return testData;
}

std::vector<std::string> Dataset::getTrainTexts() const {
    std::vector<std::string> trainTexts;
    trainTexts.reserve(trainIndices.size());
    
    for (size_t idx : trainIndices) {
        if (idx < data.size()) {
            trainTexts.push_back(data[idx].second);
        }
    }
    
    return trainTexts;
}

std::vector<std::string> Dataset::getTestTexts() const {
    std::vector<std::string> testTexts;
    testTexts.reserve(testIndices.size());
    
    for (size_t idx : testIndices) {
        if (idx < data.size()) {
            testTexts.push_back(data[idx].second);
        }
    }
    
    return testTexts;
}

std::vector<int> Dataset::getTrainLabels() const {
    std::vector<int> trainLabels;
    trainLabels.reserve(trainIndices.size());
    
    for (size_t idx : trainIndices) {
        if (idx < data.size()) {
            trainLabels.push_back(data[idx].first);
        }
    }
    
    return trainLabels;
}

std::vector<int> Dataset::getTestLabels() const {
    std::vector<int> testLabels;
    testLabels.reserve(testIndices.size());
    
    for (size_t idx : testIndices) {
        if (idx < data.size()) {
            testLabels.push_back(data[idx].first);
        }
    }
    
    return testLabels;
}

std::vector<std::string> Dataset::getTextsWithLabel(int label) const {
    std::vector<std::string> filteredTexts;
    
    for (const auto& [dataLabel, text] : data) {
        if (dataLabel == label) {
            filteredTexts.push_back(text);
        }
    }
    
    return filteredTexts;
}

size_t Dataset::size() const {
    return data.size();
}

std::unordered_map<int, size_t> Dataset::getLabelDistribution() const {
    std::unordered_map<int, size_t> distribution;
    
    for (const auto& [label, _] : data) {
        distribution[label]++;
    }
    
    return distribution;
}

std::unordered_map<std::string, std::string> Dataset::getStatistics() const {
    std::unordered_map<std::string, std::string> stats;
    
    // Basic stats
    stats["total_samples"] = std::to_string(data.size());
    stats["train_samples"] = std::to_string(trainIndices.size());
    stats["test_samples"] = std::to_string(testIndices.size());
    
    // Label distribution
    auto distribution = getLabelDistribution();
    for (const auto& [label, count] : distribution) {
        stats["label_" + std::to_string(label) + "_count"] = std::to_string(count);
        double percentage = data.empty() ? 0.0 : (static_cast<double>(count) / data.size() * 100.0);
        stats["label_" + std::to_string(label) + "_percentage"] = 
            std::to_string(static_cast<int>(percentage)) + "%";
    }
    
    // Text length stats
    if (!data.empty()) {
        size_t minLength = SIZE_MAX;
        size_t maxLength = 0;
        size_t totalLength = 0;
        
        for (const auto& [_, text] : data) {
            size_t length = text.size();
            minLength = std::min(minLength, length);
            maxLength = std::max(maxLength, length);
            totalLength += length;
        }
        
        stats["min_text_length"] = std::to_string(minLength);
        stats["max_text_length"] = std::to_string(maxLength);
        stats["avg_text_length"] = std::to_string(totalLength / data.size());
    }
    
    return stats;
}

bool Dataset::loadFromCSV(
    const std::string& filePath,
    const std::string& labelColumn,
    const std::string& textColumn
) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filePath << std::endl;
        return false;
    }
    
    data.clear();
    
    std::string line;
    std::getline(file, line); // Read header line
    
    // Parse header to find column indices
    std::vector<std::string> headers;
    std::stringstream headerStream(line);
    std::string header;
    
    while (std::getline(headerStream, header, ',')) {
        // Remove quotes and whitespace
        header.erase(std::remove(header.begin(), header.end(), '"'), header.end());
        header.erase(std::remove(header.begin(), header.end(), ' '), header.end());
        headers.push_back(header);
    }
    
    // Find column indices
    int labelIdx = -1;
    int textIdx = -1;
    
    for (size_t i = 0; i < headers.size(); ++i) {
        if (headers[i] == labelColumn) {
            labelIdx = static_cast<int>(i);
        }
        if (headers[i] == textColumn) {
            textIdx = static_cast<int>(i);
        }
    }
    
    if (labelIdx == -1 || textIdx == -1) {
        std::cerr << "Error: Could not find required columns in CSV file." << std::endl;
        std::cerr << "Looking for: '" << labelColumn << "' and '" << textColumn << "'" << std::endl;
        std::cerr << "Available columns: ";
        for (const auto& h : headers) {
            std::cerr << "'" << h << "' ";
        }
        std::cerr << std::endl;
        return false;
    }
    
    // Parse data rows
    while (std::getline(file, line)) {
        std::vector<std::string> values;
        bool inQuotes = false;
        std::string value;
        
        // Parse CSV with proper handling of quotes
        for (char c : line) {
            if (c == '"') {
                inQuotes = !inQuotes;
            } else if (c == ',' && !inQuotes) {
                values.push_back(value);
                value.clear();
            } else {
                value.push_back(c);
            }
        }
        values.push_back(value);
        
        // Skip lines with wrong number of columns
        if (values.size() <= static_cast<size_t>(std::max(labelIdx, textIdx))) {
            continue;
        }
        
        // Extract label and text
        try {
            int label = std::stoi(values[labelIdx]);
            std::string text = values[textIdx];
            
            // Remove quotes if present
            if (!text.empty() && text.front() == '"' && text.back() == '"') {
                text = text.substr(1, text.size() - 2);
            }
            
            data.emplace_back(label, text);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error parsing CSV line: " << e.what() << std::endl;
        }
    }
    
    std::cout << "Loaded " << data.size() << " samples from " << filePath << std::endl;
    return !data.empty();
}

bool Dataset::loadFromTSV(
    const std::string& filePath,
    const std::string& labelColumn,
    const std::string& textColumn
) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filePath << std::endl;
        return false;
    }
    
    data.clear();
    
    std::string line;
    std::getline(file, line); // Read header line
    
    // Parse header to find column indices
    std::vector<std::string> headers;
    std::stringstream headerStream(line);
    std::string header;
    
    while (std::getline(headerStream, header, '\t')) {
        // Remove whitespace
        header.erase(std::remove(header.begin(), header.end(), ' '), header.end());
        headers.push_back(header);
    }
    
    // Find column indices
    int labelIdx = -1;
    int textIdx = -1;
    
    for (size_t i = 0; i < headers.size(); ++i) {
        if (headers[i] == labelColumn) {
            labelIdx = static_cast<int>(i);
        }
        if (headers[i] == textColumn) {
            textIdx = static_cast<int>(i);
        }
    }
    
    if (labelIdx == -1 || textIdx == -1) {
        std::cerr << "Error: Could not find required columns in TSV file." << std::endl;
        return false;
    }
    
    // Parse data rows
    while (std::getline(file, line)) {
        std::vector<std::string> values;
        std::stringstream lineStream(line);
        std::string value;
        
        while (std::getline(lineStream, value, '\t')) {
            values.push_back(value);
        }
        
        // Skip lines with wrong number of columns
        if (values.size() <= static_cast<size_t>(std::max(labelIdx, textIdx))) {
            continue;
        }
        
        // Extract label and text
        try {
            int label = std::stoi(values[labelIdx]);
            std::string text = values[textIdx];
            
            data.emplace_back(label, text);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error parsing TSV line: " << e.what() << std::endl;
        }
    }
    
    std::cout << "Loaded " << data.size() << " samples from " << filePath << std::endl;
    return !data.empty();
}

bool Dataset::loadFromJSON(
    const std::string& filePath,
    const std::string& labelField,
    const std::string& textField
) {
    // This is a simplified JSON parser for this specific use case
    // In a real implementation, you would use a full JSON library like nlohmann/json
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filePath << std::endl;
        return false;
    }
    
    data.clear();
    
    std::string line;
    bool inArray = false;
    
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty()) continue;
        
        // Check for array start/end
        if (line == "[") {
            inArray = true;
            continue;
        } else if (line == "]") {
            inArray = false;
            continue;
        }
        
        // Skip if not in array
        if (!inArray) continue;
        
        // Remove trailing comma if present
        if (line.back() == ',') {
            line.pop_back();
        }
        
        // Check for object start/end
        if (line == "{" || line == "}") continue;
        
        // Look for label field
        std::string labelPrefix = "\"" + labelField + "\":";
        std::string textPrefix = "\"" + textField + "\":";
        
        size_t labelPos = line.find(labelPrefix);
        size_t textPos = line.find(textPrefix);
        
        if (labelPos == std::string::npos || textPos == std::string::npos) {
            continue;
        }
        
        // Extract label
        labelPos += labelPrefix.size();
        size_t labelEnd = line.find(",", labelPos);
        if (labelEnd == std::string::npos) {
            labelEnd = line.find("}", labelPos);
        }
        if (labelEnd == std::string::npos) continue;
        
        std::string labelStr = line.substr(labelPos, labelEnd - labelPos);
        labelStr.erase(std::remove_if(labelStr.begin(), labelStr.end(), 
                                    [](char c) { return std::isspace(c); }), 
                     labelStr.end());
        
        // Extract text
        textPos += textPrefix.size();
        if (line[textPos] != '"') continue;
        textPos++; // Skip opening quote
        
        size_t textEnd = textPos;
        bool escaped = false;
        
        while (textEnd < line.size()) {
            if (line[textEnd] == '\\') {
                escaped = !escaped;
            } else if (line[textEnd] == '"' && !escaped) {
                break;
            } else {
                escaped = false;
            }
            textEnd++;
        }
        
        if (textEnd >= line.size()) continue;
        
        std::string text = line.substr(textPos, textEnd - textPos);
        
        // Unescape JSON string
        std::string unescaped;
        for (size_t i = 0; i < text.size(); i++) {
            if (text[i] == '\\' && i + 1 < text.size()) {
                if (text[i + 1] == 'n') {
                    unescaped += '\n';
                } else if (text[i + 1] == 't') {
                    unescaped += '\t';
                } else if (text[i + 1] == '\\') {
                    unescaped += '\\';
                } else if (text[i + 1] == '"') {
                    unescaped += '"';
                } else {
                    unescaped += text[i + 1];
                }
                i++;
            } else {
                unescaped += text[i];
            }
        }
        
        // Parse label and add to data
        try {
            int label = std::stoi(labelStr);
            data.emplace_back(label, unescaped);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Error parsing JSON: " << e.what() << std::endl;
        }
    }
    
    std::cout << "Loaded " << data.size() << " samples from " << filePath << std::endl;
    return !data.empty();
}

} // namespace utils
} // namespace blahajpi
