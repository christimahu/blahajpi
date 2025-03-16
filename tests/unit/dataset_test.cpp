/**
 * @file dataset_test.cpp
 * @brief Comprehensive unit tests for the Dataset class
 * 
 * These tests verify the functionality of the dataset management system,
 * including loading, saving, and manipulating datasets for training and evaluation.
 * 
 * @author BlahajPI Team
 * @date 2025
 */

#include "blahajpi/utils/dataset.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

/**
 * @class DatasetTest
 * @brief Test fixture for Dataset unit tests
 * 
 * This fixture provides common setup for testing dataset operations.
 */
class DatasetTest : public ::testing::Test {
protected:
    /**
     * @brief Set up test environment
     * 
     * Creates temporary directory and sample data files for testing.
     */
    void SetUp() override {
        // Create temporary directory
        tempDir = std::filesystem::temp_directory_path() / "blahajpi_tests";
        std::filesystem::create_directories(tempDir);
        
        // Create sample CSV file
        csvPath = tempDir / "test_data.csv";
        std::ofstream csvFile(csvPath);
        csvFile << "label,text\n"
               << "0,This is a safe message with normal content.\n"
               << "4,This is harmful content targeting groups.\n"
               << "0,Another safe message about everyday topics.\n"
               << "4,More harmful content with negative stereotypes.\n"
               << "0,A third safe message about technology.\n";
        csvFile.close();
        
        // Create sample TSV file
        tsvPath = tempDir / "test_data.tsv";
        std::ofstream tsvFile(tsvPath);
        tsvFile << "label\ttext\n"
               << "0\tSafe message in TSV format.\n"
               << "4\tHarmful message in TSV format.\n"
               << "0\tAnother safe message in TSV.\n";
        tsvFile.close();
        
        // Create sample JSON file
        jsonPath = tempDir / "test_data.json";
        std::ofstream jsonFile(jsonPath);
        jsonFile << "[\n"
                << "  {\"label\": 0, \"text\": \"Safe message in JSON format.\"},\n"
                << "  {\"label\": 4, \"text\": \"Harmful message in JSON format.\"},\n"
                << "  {\"label\": 0, \"text\": \"Another safe message in JSON.\"}\n"
                << "]\n";
        jsonFile.close();
    }
    
    /**
     * @brief Clean up test environment
     * 
     * Removes temporary files and directories.
     */
    void TearDown() override {
        if (std::filesystem::exists(tempDir)) {
            std::filesystem::remove_all(tempDir);
        }
    }
    
    std::filesystem::path tempDir;
    std::filesystem::path csvPath;
    std::filesystem::path tsvPath;
    std::filesystem::path jsonPath;
};

/**
 * @brief Tests the default constructor
 * 
 * Verifies that the Dataset class can be properly initialized.
 */
TEST_F(DatasetTest, DefaultConstructor) {
    blahajpi::utils::Dataset dataset;
    
    // New dataset should be empty
    EXPECT_EQ(dataset.size(), 0);
}

/**
 * @brief Tests constructor with initial data
 * 
 * Verifies that a dataset can be constructed with provided data.
 */
TEST_F(DatasetTest, DataConstructor) {
    std::vector<std::pair<int, std::string>> initialData = {
        {0, "Safe text 1"},
        {4, "Harmful text 1"},
        {0, "Safe text 2"}
    };
    
    blahajpi::utils::Dataset dataset(initialData);
    
    // Dataset size should match initial data
    EXPECT_EQ(dataset.size(), initialData.size());
    
    // Verify label distribution
    auto distribution = dataset.getLabelDistribution();
    EXPECT_EQ(distribution[0], 2);
    EXPECT_EQ(distribution[4], 1);
}

/**
 * @brief Tests loading from CSV file
 * 
 * Verifies that data can be loaded from a CSV file.
 */
TEST_F(DatasetTest, LoadFromCsv) {
    blahajpi::utils::Dataset dataset;
    
    // Load from CSV with format auto-detection
    bool result = dataset.loadFromFile(csvPath.string(), blahajpi::utils::Dataset::Format::AUTO, "label", "text");
    EXPECT_TRUE(result);
    
    // Dataset size should match CSV rows (excluding header)
    EXPECT_EQ(dataset.size(), 5);
    
    // Verify label distribution
    auto distribution = dataset.getLabelDistribution();
    EXPECT_EQ(distribution[0], 3);
    EXPECT_EQ(distribution[4], 2);
    
    // Try loading with explicit CSV format
    blahajpi::utils::Dataset csvDataset;
    result = csvDataset.loadFromFile(csvPath.string(), blahajpi::utils::Dataset::Format::CSV, "label", "text");
    EXPECT_TRUE(result);
    EXPECT_EQ(csvDataset.size(), 5);
}

/**
 * @brief Tests loading from TSV file
 * 
 * Verifies that data can be loaded from a TSV file.
 */
TEST_F(DatasetTest, LoadFromTsv) {
    blahajpi::utils::Dataset dataset;
    
    // Load from TSV with format auto-detection
    bool result = dataset.loadFromFile(tsvPath.string(), blahajpi::utils::Dataset::Format::AUTO, "label", "text");
    EXPECT_TRUE(result);
    
    // Dataset size should match TSV rows (excluding header)
    EXPECT_EQ(dataset.size(), 3);
    
    // Verify label distribution
    auto distribution = dataset.getLabelDistribution();
    EXPECT_EQ(distribution[0], 2);
    EXPECT_EQ(distribution[4], 1);
    
    // Try loading with explicit TSV format
    blahajpi::utils::Dataset tsvDataset;
    result = tsvDataset.loadFromFile(tsvPath.string(), blahajpi::utils::Dataset::Format::TSV, "label", "text");
    EXPECT_TRUE(result);
    EXPECT_EQ(tsvDataset.size(), 3);
}

/**
 * @brief Tests loading from JSON file
 * 
 * Verifies that data can be loaded from a JSON file.
 */
TEST_F(DatasetTest, LoadFromJson) {
    blahajpi::utils::Dataset dataset;
    
    // Load from JSON with format auto-detection
    bool result = dataset.loadFromFile(jsonPath.string(), blahajpi::utils::Dataset::Format::AUTO, "label", "text");
    EXPECT_TRUE(result);
    
    // Dataset size should match JSON entries
    EXPECT_EQ(dataset.size(), 3);
    
    // Verify label distribution
    auto distribution = dataset.getLabelDistribution();
    EXPECT_EQ(distribution[0], 2);
    EXPECT_EQ(distribution[4], 1);
    
    // Try loading with explicit JSON format
    blahajpi::utils::Dataset jsonDataset;
    result = jsonDataset.loadFromFile(jsonPath.string(), blahajpi::utils::Dataset::Format::JSON, "label", "text");
    EXPECT_TRUE(result);
    EXPECT_EQ(jsonDataset.size(), 3);
}

/**
 * @brief Tests loading from non-existent file
 * 
 * Verifies that attempting to load from a non-existent file fails gracefully.
 */
TEST_F(DatasetTest, LoadFromNonExistentFile) {
    blahajpi::utils::Dataset dataset;
    
    // Attempt to load from non-existent file
    bool result = dataset.loadFromFile("non_existent_file.csv");
    EXPECT_FALSE(result);
    EXPECT_EQ(dataset.size(), 0);
}

/**
 * @brief Tests saving to CSV file
 * 
 * Verifies that data can be saved to a CSV file.
 */
TEST_F(DatasetTest, SaveToCsv) {
    // Create dataset with sample data
    std::vector<std::pair<int, std::string>> data = {
        {0, "Text with, comma"},
        {4, "Text with \"quotes\""},
        {0, "Normal text"}
    };
    blahajpi::utils::Dataset dataset(data);
    
    // Save to CSV
    std::filesystem::path savePath = tempDir / "save_test.csv";
    bool result = dataset.saveToFile(savePath.string(), blahajpi::utils::Dataset::Format::CSV);
    EXPECT_TRUE(result);
    EXPECT_TRUE(std::filesystem::exists(savePath));
    
    // Load the saved file to verify
    blahajpi::utils::Dataset loadedDataset;
    result = loadedDataset.loadFromFile(savePath.string());
    EXPECT_TRUE(result);
    EXPECT_EQ(loadedDataset.size(), data.size());
    
    // Check label distribution
    auto distribution = loadedDataset.getLabelDistribution();
    EXPECT_EQ(distribution[0], 2);
    EXPECT_EQ(distribution[4], 1);
}

/**
 * @brief Tests saving to TSV file
 * 
 * Verifies that data can be saved to a TSV file.
 */
TEST_F(DatasetTest, SaveToTsv) {
    // Create dataset with sample data
    std::vector<std::pair<int, std::string>> data = {
        {0, "Text with tab\t character"},
        {4, "Normal text 1"},
        {0, "Normal text 2"}
    };
    blahajpi::utils::Dataset dataset(data);
    
    // Save to TSV
    std::filesystem::path savePath = tempDir / "save_test.tsv";
    bool result = dataset.saveToFile(savePath.string(), blahajpi::utils::Dataset::Format::TSV);
    EXPECT_TRUE(result);
    EXPECT_TRUE(std::filesystem::exists(savePath));
    
    // Load the saved file to verify
    blahajpi::utils::Dataset loadedDataset;
    result = loadedDataset.loadFromFile(savePath.string());
    EXPECT_TRUE(result);
    EXPECT_EQ(loadedDataset.size(), data.size());
}

/**
 * @brief Tests saving to JSON file
 * 
 * Verifies that data can be saved to a JSON file.
 */
TEST_F(DatasetTest, SaveToJson) {
    // Create dataset with sample data
    std::vector<std::pair<int, std::string>> data = {
        {0, "Text with \"quotes\""},
        {4, "Text with \n newline"},
        {0, "Normal text"}
    };
    blahajpi::utils::Dataset dataset(data);
    
    // Save to JSON
    std::filesystem::path savePath = tempDir / "save_test.json";
    bool result = dataset.saveToFile(savePath.string(), blahajpi::utils::Dataset::Format::JSON);
    EXPECT_TRUE(result);
    EXPECT_TRUE(std::filesystem::exists(savePath));
    
    // Load the saved file to verify
    blahajpi::utils::Dataset loadedDataset;
    result = loadedDataset.loadFromFile(savePath.string());
    EXPECT_TRUE(result);
    EXPECT_EQ(loadedDataset.size(), data.size());
}

/**
 * @brief Tests train-test split functionality
 * 
 * Verifies that data can be split into training and test sets.
 */
TEST_F(DatasetTest, TrainTestSplit) {
    // Load sample data
    blahajpi::utils::Dataset dataset;
    dataset.loadFromFile(csvPath.string());
    
    // Split with 20% test size
    dataset.splitTrainTest(0.2);
    
    // Get train and test data
    auto trainData = dataset.getTrainData();
    auto testData = dataset.getTestData();
    
    // Check split sizes (with small dataset, rounding may affect exact percentages)
    EXPECT_GT(trainData.size(), 0);
    EXPECT_GT(testData.size(), 0);
    EXPECT_EQ(trainData.size() + testData.size(), dataset.size());
    
    // Test with absolute count
    dataset.splitTrainTest(2);  // 2 test samples
    
    trainData = dataset.getTrainData();
    testData = dataset.getTestData();
    
    EXPECT_EQ(testData.size(), 2);
    EXPECT_EQ(trainData.size(), dataset.size() - 2);
}

/**
 * @brief Tests train-test split with stratification
 * 
 * Verifies that stratified splitting maintains class distribution.
 */
TEST_F(DatasetTest, StratifiedSplit) {
    // Create dataset with balanced classes
    std::vector<std::pair<int, std::string>> data;
    for (int i = 0; i < 50; ++i) {
        data.push_back({0, "Safe text " + std::to_string(i)});
        data.push_back({4, "Harmful text " + std::to_string(i)});
    }
    blahajpi::utils::Dataset dataset(data);
    
    // Split with stratification
    dataset.splitTrainTest(0.2, true);
    
    // Get train and test data
    auto trainLabels = dataset.getTrainLabels();
    auto testLabels = dataset.getTestLabels();
    
    // Count class distribution in each split
    int trainSafe = 0, trainHarmful = 0;
    for (int label : trainLabels) {
        if (label == 0) trainSafe++;
        else if (label == 4) trainHarmful++;
    }
    
    int testSafe = 0, testHarmful = 0;
    for (int label : testLabels) {
        if (label == 0) testSafe++;
        else if (label == 4) testHarmful++;
    }
    
    // Check that class distributions are maintained (approximately)
    EXPECT_NEAR(static_cast<double>(trainSafe) / trainLabels.size(), 0.5, 0.1);
    EXPECT_NEAR(static_cast<double>(trainHarmful) / trainLabels.size(), 0.5, 0.1);
    EXPECT_NEAR(static_cast<double>(testSafe) / testLabels.size(), 0.5, 0.1);
    EXPECT_NEAR(static_cast<double>(testHarmful) / testLabels.size(), 0.5, 0.1);
}

/**
 * @brief Tests data extraction methods
 * 
 * Verifies that train and test data can be correctly extracted.
 */
TEST_F(DatasetTest, DataExtraction) {
    // Create dataset with sample data
    std::vector<std::pair<int, std::string>> data = {
        {0, "Safe text 1"},
        {0, "Safe text 2"},
        {4, "Harmful text 1"},
        {4, "Harmful text 2"},
        {0, "Safe text 3"}
    };
    blahajpi::utils::Dataset dataset(data);
    
    // Split with fixed test size
    dataset.splitTrainTest(2);  // 2 test samples
    
    // Get data in various formats
    auto trainData = dataset.getTrainData();
    auto testData = dataset.getTestData();
    auto trainTexts = dataset.getTrainTexts();
    auto testTexts = dataset.getTestTexts();
    auto trainLabels = dataset.getTrainLabels();
    auto testLabels = dataset.getTestLabels();
    
    // Check consistency
    EXPECT_EQ(trainData.size(), trainTexts.size());
    EXPECT_EQ(trainData.size(), trainLabels.size());
    EXPECT_EQ(testData.size(), testTexts.size());
    EXPECT_EQ(testData.size(), testLabels.size());
    
    // Check contents
    for (size_t i = 0; i < trainData.size(); ++i) {
        EXPECT_EQ(trainData[i].first, trainLabels[i]);
        EXPECT_EQ(trainData[i].second, trainTexts[i]);
    }
    
    for (size_t i = 0; i < testData.size(); ++i) {
        EXPECT_EQ(testData[i].first, testLabels[i]);
        EXPECT_EQ(testData[i].second, testTexts[i]);
    }
}

/**
 * @brief Tests filtering by label
 * 
 * Verifies that texts can be filtered by label.
 */
TEST_F(DatasetTest, FilterByLabel) {
    // Create dataset with sample data
    std::vector<std::pair<int, std::string>> data = {
        {0, "Safe text 1"},
        {0, "Safe text 2"},
        {4, "Harmful text 1"},
        {4, "Harmful text 2"},
        {0, "Safe text 3"}
    };
    blahajpi::utils::Dataset dataset(data);
    
    // Get texts with specific labels
    auto safeTexts = dataset.getTextsWithLabel(0);
    auto harmfulTexts = dataset.getTextsWithLabel(4);
    
    // Check counts
    EXPECT_EQ(safeTexts.size(), 3);
    EXPECT_EQ(harmfulTexts.size(), 2);
    
    // Check contents
    for (const auto& text : safeTexts) {
        EXPECT_TRUE(text.find("Safe") != std::string::npos);
    }
    
    for (const auto& text : harmfulTexts) {
        EXPECT_TRUE(text.find("Harmful") != std::string::npos);
    }
}

/**
 * @brief Tests dataset statistics
 * 
 * Verifies that correct statistics are generated for the dataset.
 */
TEST_F(DatasetTest, StatisticsGeneration) {
    // Create dataset with sample data
    std::vector<std::pair<int, std::string>> data = {
        {0, "Safe text 1"},
        {0, "Safe text 2"},
        {4, "Harmful text 1"},
        {4, "Harmful text 2"},
        {0, "Safe text 3"}
    };
    blahajpi::utils::Dataset dataset(data);
    
    // Split dataset
    dataset.splitTrainTest(0.4);
    
    // Get statistics
    auto stats = dataset.getStatistics();
    
    // Check basic stats
    EXPECT_EQ(std::stoi(stats["total_samples"]), 5);
    EXPECT_EQ(std::stoi(stats["label_0_count"]), 3);
    EXPECT_EQ(std::stoi(stats["label_4_count"]), 2);
    
    // Label percentages should add up to close to 100%
    int label0Percent = std::stoi(stats["label_0_percentage"]);
    int label4Percent = std::stoi(stats["label_4_percentage"]);
    EXPECT_NEAR(label0Percent + label4Percent, 100, 1);
    
    // Check train/test counts
    int trainSamples = std::stoi(stats["train_samples"]);
    int testSamples = std::stoi(stats["test_samples"]);
    EXPECT_EQ(trainSamples + testSamples, 5);
}

/**
 * @brief Tests dataset with invalid column names
 * 
 * Verifies that loading from a file with invalid column names fails gracefully.
 */
TEST_F(DatasetTest, InvalidColumnNames) {
    blahajpi::utils::Dataset dataset;
    
    // Try to load with non-existent column names
    bool result = dataset.loadFromFile(csvPath.string(), blahajpi::utils::Dataset::Format::CSV, 
                                      "non_existent_label", "non_existent_text");
    EXPECT_FALSE(result);
}

/**
 * @brief Tests dataset with empty data
 * 
 * Verifies that operations on an empty dataset behave correctly.
 */
TEST_F(DatasetTest, EmptyDataset) {
    blahajpi::utils::Dataset dataset;
    
    // Size should be 0
    EXPECT_EQ(dataset.size(), 0);
    
    // Train-test split should not crash
    EXPECT_NO_THROW(dataset.splitTrainTest(0.2));
    
    // Get methods should return empty vectors
    EXPECT_TRUE(dataset.getTrainData().empty());
    EXPECT_TRUE(dataset.getTestData().empty());
    EXPECT_TRUE(dataset.getTrainTexts().empty());
    EXPECT_TRUE(dataset.getTestTexts().empty());
    EXPECT_TRUE(dataset.getTrainLabels().empty());
    EXPECT_TRUE(dataset.getTestLabels().empty());
    
    // Label distribution should be empty
    EXPECT_TRUE(dataset.getLabelDistribution().empty());
    
    // Statistics should have zero counts
    auto stats = dataset.getStatistics();
    EXPECT_EQ(std::stoi(stats["total_samples"]), 0);
}

} // namespace
