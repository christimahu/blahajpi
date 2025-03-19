/**
 * @file dataset_test.cpp
 * @brief Simplified but meaningful unit tests for the Dataset class
 * @ingroup tests
 * @defgroup dataset_tests Dataset Tests
 * 
 * Contains tests for the Dataset class, which handles loading, processing,
 * and managing data for training and evaluation.
 */

#include "blahajpi/utils/dataset.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace {

/**
 * @brief Test fixture for Dataset tests
 * @ingroup dataset_tests
 * 
 * Provides common setup and sample data for testing the Dataset class.
 */
class DatasetTest : public ::testing::Test {
protected:
    /**
     * @brief Set up test data
     * 
     * Creates sample data for testing Dataset functionality.
     */
    void SetUp() override {
        // Create sample data
        sampleData = {
            {0, "This is a safe message with normal content."},
            {4, "This is harmful content targeting groups."},
            {0, "Another safe message about everyday topics."}
        };
    }
    
    /** Sample data for testing */
    std::vector<std::pair<int, std::string>> sampleData;
};

/**
 * @test
 * @brief Tests basic construction of the Dataset class
 * @ingroup dataset_tests
 * 
 * Verifies that the Dataset can be constructed with and without initial data.
 */
TEST_F(DatasetTest, Construction) {
    // Default constructor
    EXPECT_NO_THROW({
        blahajpi::utils::Dataset dataset;
    });
    
    // Constructor with data
    EXPECT_NO_THROW({
        blahajpi::utils::Dataset dataset(sampleData);
    });
}

/**
 * @test
 * @brief Tests size calculation
 * @ingroup dataset_tests
 * 
 * Verifies that the Dataset correctly reports its size.
 */
TEST_F(DatasetTest, Size) {
    // Empty dataset
    blahajpi::utils::Dataset emptyDataset;
    EXPECT_EQ(emptyDataset.size(), 0);
    
    // Dataset with data
    blahajpi::utils::Dataset dataset(sampleData);
    EXPECT_EQ(dataset.size(), sampleData.size());
}

/**
 * @test
 * @brief Tests train-test split functionality
 * @ingroup dataset_tests
 * 
 * Verifies that the Dataset can be split into training and test sets.
 */
TEST_F(DatasetTest, TrainTestSplit) {
    blahajpi::utils::Dataset dataset(sampleData);
    
    // Split without exceptions
    EXPECT_NO_THROW({
        dataset.splitTrainTest(0.25);
    });
    
    // Get split data
    auto trainData = dataset.getTrainData();
    auto testData = dataset.getTestData();
    
    // Should split into non-empty sets
    EXPECT_FALSE(trainData.empty());
    // Test set might be empty if rounding causes all data to go to training
    // So we only check the combined size
    EXPECT_EQ(trainData.size() + testData.size(), sampleData.size());
}

/**
 * @test
 * @brief Tests retrieval of texts with specific labels
 * @ingroup dataset_tests
 * 
 * Verifies that the Dataset can filter texts by their labels.
 */
TEST_F(DatasetTest, GetTextsWithLabel) {
    blahajpi::utils::Dataset dataset(sampleData);
    
    // Get safe texts (label 0)
    auto safeTexts = dataset.getTextsWithLabel(0);
    EXPECT_EQ(safeTexts.size(), 2); // We have 2 safe texts in sample data
    
    // Get harmful texts (label 4)
    auto harmfulTexts = dataset.getTextsWithLabel(4);
    EXPECT_EQ(harmfulTexts.size(), 1); // We have 1 harmful text in sample data
    
    // Get non-existent label
    auto nonExistentTexts = dataset.getTextsWithLabel(999);
    EXPECT_TRUE(nonExistentTexts.empty());
}

/**
 * @test
 * @brief Tests label distribution calculation
 * @ingroup dataset_tests
 * 
 * Verifies that the Dataset correctly calculates the distribution of labels.
 */
TEST_F(DatasetTest, GetLabelDistribution) {
    blahajpi::utils::Dataset dataset(sampleData);
    
    // Get label distribution
    auto distribution = dataset.getLabelDistribution();
    
    // Should have distribution for our labels
    EXPECT_TRUE(distribution.count(0) > 0);
    EXPECT_TRUE(distribution.count(4) > 0);
    
    // Distribution should match our data
    EXPECT_EQ(distribution[0], 2); // 2 safe texts
    EXPECT_EQ(distribution[4], 1); // 1 harmful text
}

/**
 * @test
 * @brief Tests statistics generation
 * @ingroup dataset_tests
 * 
 * Verifies that the Dataset can generate statistical information about its contents.
 */
TEST_F(DatasetTest, GetStatistics) {
    blahajpi::utils::Dataset dataset(sampleData);
    
    // Get statistics
    auto stats = dataset.getStatistics();
    
    // Should have some statistics
    EXPECT_FALSE(stats.empty());
    
    // Should have total samples statistic
    EXPECT_TRUE(stats.count("total_samples") > 0);
    
    // Total samples should match our data
    EXPECT_EQ(stats["total_samples"], std::to_string(sampleData.size()));
}

/**
 * @test
 * @brief Tests format auto-detection functionality
 * @ingroup dataset_tests
 * 
 * Verifies that the Dataset can handle format auto-detection without crashing.
 */
TEST_F(DatasetTest, FormatAutoDetection) {
    // This test simply verifies that calling loadFromFile with AUTO format 
    // doesn't crash, without actually loading a file
    blahajpi::utils::Dataset dataset;
    
    EXPECT_NO_THROW({
        // Use a non-existent file to verify graceful failure
        bool result = dataset.loadFromFile(
            "non_existent_file.csv", 
            blahajpi::utils::Dataset::Format::AUTO,
            "label", 
            "text"
        );
        EXPECT_FALSE(result);
    });
}

} // namespace
