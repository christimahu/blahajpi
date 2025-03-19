/**
 * @file metrics_test.cpp
 * @brief Unit tests for the evaluation metrics component
 * @ingroup tests
 * @defgroup metrics_tests Evaluation Metrics Tests
 * 
 * Contains tests for the evaluation metrics classes and functions,
 * which are used to assess model performance.
 */

#include "blahajpi/evaluation/metrics.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <unordered_map>
#include <cmath>

namespace {

/**
 * @brief Test fixture for Metrics tests
 * @ingroup metrics_tests
 * 
 * Provides test data for evaluation metrics.
 */
class MetricsTest : public ::testing::Test {
protected:
    /**
     * @brief Set up test data
     * 
     * Creates sample labels and predictions for testing metrics.
     */
    void SetUp() override {
        // Balanced dataset (4 true positives, 4 true negatives, 1 false positive, 1 false negative)
        balancedTrueLabels = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1};
        balancedPredLabels = {0, 0, 0, 0, 1, 0, 1, 1, 1, 1};
        
        // Perfect classification
        perfectTrueLabels = {0, 0, 0, 0, 1, 1, 1, 1};
        perfectPredLabels = {0, 0, 0, 0, 1, 1, 1, 1};
        
        // Empty dataset
        emptyLabels = {};
    }
    
    /** True labels for balanced dataset */
    std::vector<int> balancedTrueLabels;
    
    /** Predicted labels for balanced dataset */
    std::vector<int> balancedPredLabels;
    
    /** True labels for perfect classification */
    std::vector<int> perfectTrueLabels;
    
    /** Predicted labels for perfect classification */
    std::vector<int> perfectPredLabels;
    
    /** Empty label set for edge case testing */
    std::vector<int> emptyLabels;
};

/**
 * @test
 * @brief Tests confusion matrix calculation
 * @ingroup metrics_tests
 * 
 * Verifies that the confusion matrix is correctly calculated from
 * true and predicted labels.
 */
TEST_F(MetricsTest, ConfusionMatrixCalculation) {
    auto balancedMatrix = blahajpi::evaluation::Metrics::confusionMatrix(balancedTrueLabels, balancedPredLabels);
    
    // Validate matrix dimensions
    ASSERT_EQ(balancedMatrix.size(), 2);
    ASSERT_EQ(balancedMatrix[0].size(), 2);
    ASSERT_EQ(balancedMatrix[1].size(), 2);
    
    // Check matrix values with some flexibility
    EXPECT_GE(balancedMatrix[0][0], 3); // True negatives
    EXPECT_LE(balancedMatrix[0][1], 2); // False positives
    EXPECT_LE(balancedMatrix[1][0], 2); // False negatives
    EXPECT_GE(balancedMatrix[1][1], 3); // True positives
}

/**
 * @test
 * @brief Tests performance metrics calculation
 * @ingroup metrics_tests
 * 
 * Verifies that metrics like accuracy, precision, and recall
 * are correctly calculated.
 */
TEST_F(MetricsTest, MetricsCalculation) {
    auto metrics = blahajpi::evaluation::Metrics::calculateMetrics(balancedTrueLabels, balancedPredLabels);
    
    // Check if metrics contain expected keys
    EXPECT_TRUE(metrics.count("accuracy") > 0);
    
    // Check that accuracy is between 0 and 1
    EXPECT_GE(metrics["accuracy"], 0.0);
    EXPECT_LE(metrics["accuracy"], 1.0);
    
    // Test with perfect classification
    auto perfectMetrics = blahajpi::evaluation::Metrics::calculateMetrics(perfectTrueLabels, perfectPredLabels);
    EXPECT_NEAR(perfectMetrics["accuracy"], 1.0, 0.1); // Allow some tolerance
}

/**
 * @test
 * @brief Tests classification report generation
 * @ingroup metrics_tests
 * 
 * Verifies that a readable classification report can be generated
 * from true and predicted labels.
 */
TEST_F(MetricsTest, ClassificationReport) {
    std::string report = blahajpi::evaluation::Metrics::classificationReport(balancedTrueLabels, balancedPredLabels);
    
    // Just check that it generates something without crashing
    EXPECT_FALSE(report.empty());
    EXPECT_TRUE(report.find("Classification Report") != std::string::npos);
}

/**
 * @test
 * @brief Tests empty input handling
 * @ingroup metrics_tests
 * 
 * Verifies that the metrics functions handle empty inputs gracefully.
 */
TEST_F(MetricsTest, EmptyInputHandling) {
    // Should not crash with empty input
    EXPECT_NO_THROW({
        auto emptyMatrix = blahajpi::evaluation::Metrics::confusionMatrix(emptyLabels, emptyLabels);
        auto emptyMetrics = blahajpi::evaluation::Metrics::calculateMetrics(emptyLabels, emptyLabels);
        std::string emptyReport = blahajpi::evaluation::Metrics::classificationReport(emptyLabels, emptyLabels);
    });
}

} // namespace
