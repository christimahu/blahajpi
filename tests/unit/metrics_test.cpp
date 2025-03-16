/**
 * @file metrics_test.cpp
 * @brief Comprehensive unit tests for the evaluation metrics component
 * 
 * These tests verify the functionality of all metric calculation methods,
 * covering normal use cases, edge cases, and boundary conditions.
 * 
 * @author BlahajPI Team
 * @date 2025
 */

#include "blahajpi/evaluation/metrics.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <unordered_map>
#include <cmath>

namespace {

/**
 * @class MetricsTest
 * @brief Test fixture for evaluation metrics unit tests
 * 
 * This fixture provides test data for various evaluation metric calculations.
 */
class MetricsTest : public ::testing::Test {
protected:
    /**
     * @brief Set up test data for metric calculations
     * 
     * Creates various test datasets with known expected results.
     */
    void SetUp() override {
        // Balanced dataset (4 true positives, 4 true negatives, 1 false positive, 1 false negative)
        balancedTrueLabels = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1};
        balancedPredLabels = {0, 0, 0, 0, 1, 0, 1, 1, 1, 1};
        balancedScores = {-0.8, -0.7, -0.6, -0.5, 0.1, -0.2, 0.3, 0.6, 0.8, 0.9};
        
        // Perfect classification
        perfectTrueLabels = {0, 0, 0, 0, 1, 1, 1, 1};
        perfectPredLabels = {0, 0, 0, 0, 1, 1, 1, 1};
        perfectScores = {-0.9, -0.8, -0.7, -0.6, 0.6, 0.7, 0.8, 0.9};
        
        // Worst case (all predictions wrong)
        worstTrueLabels = {0, 0, 0, 0, 1, 1, 1, 1};
        worstPredLabels = {1, 1, 1, 1, 0, 0, 0, 0};
        worstScores = {0.6, 0.7, 0.8, 0.9, -0.9, -0.8, -0.7, -0.6};
        
        // All positives
        allPosTrueLabels = {1, 1, 1, 1, 1, 1};
        allPosPredLabels = {1, 1, 1, 0, 0, 0};
        allPosScores = {0.9, 0.8, 0.7, -0.1, -0.2, -0.3};
        
        // All negatives
        allNegTrueLabels = {0, 0, 0, 0, 0, 0};
        allNegPredLabels = {0, 0, 0, 1, 1, 1};
        allNegScores = {-0.9, -0.8, -0.7, 0.1, 0.2, 0.3};
        
        // Empty dataset
        emptyLabels = {};
        emptyScores = {};
        
        // Single sample datasets
        singlePosTrueLabels = {1};
        singlePosPredLabels = {1};
        singlePosScores = {0.9};
        
        singleNegTrueLabels = {0};
        singleNegPredLabels = {0};
        singleNegScores = {-0.9};
    }
    
    // Various test datasets
    std::vector<int> balancedTrueLabels, balancedPredLabels;
    std::vector<double> balancedScores;
    
    std::vector<int> perfectTrueLabels, perfectPredLabels;
    std::vector<double> perfectScores;
    
    std::vector<int> worstTrueLabels, worstPredLabels;
    std::vector<double> worstScores;
    
    std::vector<int> allPosTrueLabels, allPosPredLabels;
    std::vector<double> allPosScores;
    
    std::vector<int> allNegTrueLabels, allNegPredLabels;
    std::vector<double> allNegScores;
    
    std::vector<int> emptyLabels;
    std::vector<double> emptyScores;
    
    std::vector<int> singlePosTrueLabels, singlePosPredLabels;
    std::vector<double> singlePosScores;
    
    std::vector<int> singleNegTrueLabels, singleNegPredLabels;
    std::vector<double> singleNegScores;
};

/**
 * @brief Tests the confusion matrix calculation
 * 
 * Verifies that confusion matrices are correctly calculated
 * for various input patterns.
 */
TEST_F(MetricsTest, ConfusionMatrixCalculation) {
    // Test with balanced dataset
    auto balancedMatrix = blahajpi::evaluation::Metrics::confusionMatrix(balancedTrueLabels, balancedPredLabels);
    
    // Validate matrix dimensions
    ASSERT_EQ(balancedMatrix.size(), 2);
    ASSERT_EQ(balancedMatrix[0].size(), 2);
    ASSERT_EQ(balancedMatrix[1].size(), 2);
    
    // Check matrix values
    // TN = 4, FP = 1, FN = 1, TP = 4
    EXPECT_EQ(balancedMatrix[0][0], 4); // True negatives
    EXPECT_EQ(balancedMatrix[0][1], 1); // False positives
    EXPECT_EQ(balancedMatrix[1][0], 1); // False negatives
    EXPECT_EQ(balancedMatrix[1][1], 4); // True positives
    
    // Test with perfect classification
    auto perfectMatrix = blahajpi::evaluation::Metrics::confusionMatrix(perfectTrueLabels, perfectPredLabels);
    
    // TN = 4, FP = 0, FN = 0, TP = 4
    EXPECT_EQ(perfectMatrix[0][0], 4); // True negatives
    EXPECT_EQ(perfectMatrix[0][1], 0); // False positives
    EXPECT_EQ(perfectMatrix[1][0], 0); // False negatives
    EXPECT_EQ(perfectMatrix[1][1], 4); // True positives
    
    // Test with worst case
    auto worstMatrix = blahajpi::evaluation::Metrics::confusionMatrix(worstTrueLabels, worstPredLabels);
    
    // TN = 0, FP = 4, FN = 4, TP = 0
    EXPECT_EQ(worstMatrix[0][0], 0); // True negatives
    EXPECT_EQ(worstMatrix[0][1], 4); // False positives
    EXPECT_EQ(worstMatrix[1][0], 4); // False negatives
    EXPECT_EQ(worstMatrix[1][1], 0); // True positives
    
    // Test with mismatched vector sizes (should use the shorter one)
    std::vector<int> shortTrue = {0, 1, 0};
    std::vector<int> longPred = {0, 1, 0, 1, 0};
    auto mismatchMatrix = blahajpi::evaluation::Metrics::confusionMatrix(shortTrue, longPred);
    
    // Should only process the first 3 elements
    EXPECT_EQ(mismatchMatrix[0][0], 2); // True negatives
    EXPECT_EQ(mismatchMatrix[0][1], 0); // False positives
    EXPECT_EQ(mismatchMatrix[1][0], 0); // False negatives
    EXPECT_EQ(mismatchMatrix[1][1], 1); // True positives
    
    // Test with empty vectors
    auto emptyMatrix = blahajpi::evaluation::Metrics::confusionMatrix(emptyLabels, emptyLabels);
    
    // Should have zeros everywhere
    EXPECT_EQ(emptyMatrix[0][0], 0);
    EXPECT_EQ(emptyMatrix[0][1], 0);
    EXPECT_EQ(emptyMatrix[1][0], 0);
    EXPECT_EQ(emptyMatrix[1][1], 0);
}

/**
 * @brief Tests metric calculation for balanced data
 * 
 * Verifies that precision, recall, F1-score and other metrics
 * are calculated correctly for a balanced dataset.
 */
TEST_F(MetricsTest, BalancedDataMetricsCalculation) {
    auto metrics = blahajpi::evaluation::Metrics::calculateMetrics(balancedTrueLabels, balancedPredLabels);
    
    // Check if metrics contain expected keys
    EXPECT_TRUE(metrics.count("accuracy") > 0);
    EXPECT_TRUE(metrics.count("precision_safe") > 0);
    EXPECT_TRUE(metrics.count("precision_harmful") > 0);
    EXPECT_TRUE(metrics.count("recall_safe") > 0);
    EXPECT_TRUE(metrics.count("recall_harmful") > 0);
    EXPECT_TRUE(metrics.count("f1_safe") > 0);
    EXPECT_TRUE(metrics.count("f1_harmful") > 0);
    EXPECT_TRUE(metrics.count("macro_precision") > 0);
    EXPECT_TRUE(metrics.count("macro_recall") > 0);
    EXPECT_TRUE(metrics.count("macro_f1") > 0);
    
    // Check accuracy: (TP + TN) / Total = (4 + 4) / 10 = 0.8
    EXPECT_NEAR(metrics["accuracy"], 0.8, 0.001);
    
    // Check precision and recall
    // precision_harmful = TP / (TP + FP) = 4 / (4 + 1) = 0.8
    EXPECT_NEAR(metrics["precision_harmful"], 0.8, 0.001);
    
    // recall_harmful = TP / (TP + FN) = 4 / (4 + 1) = 0.8
    EXPECT_NEAR(metrics["recall_harmful"], 0.8, 0.001);
    
    // F1 = 2 * (precision * recall) / (precision + recall) = 2 * 0.8 * 0.8 / 1.6 = 0.8
    EXPECT_NEAR(metrics["f1_harmful"], 0.8, 0.001);
    
    // For safe class
    // precision_safe = TN / (TN + FN) = 4 / (4 + 1) = 0.8
    EXPECT_NEAR(metrics["precision_safe"], 0.8, 0.001);
    
    // recall_safe = TN / (TN + FP) = 4 / (4 + 1) = 0.8
    EXPECT_NEAR(metrics["recall_safe"], 0.8, 0.001);
}

/**
 * @brief Tests metrics for perfect classification
 * 
 * Verifies that metrics show 100% performance when
 * all predictions match the true labels.
 */
TEST_F(MetricsTest, PerfectClassificationMetrics) {
    auto metrics = blahajpi::evaluation::Metrics::calculateMetrics(perfectTrueLabels, perfectPredLabels);
    
    // Perfect classification should have all metrics at 1.0
    EXPECT_NEAR(metrics["accuracy"], 1.0, 0.001);
    EXPECT_NEAR(metrics["precision_harmful"], 1.0, 0.001);
    EXPECT_NEAR(metrics["recall_harmful"], 1.0, 0.001);
    EXPECT_NEAR(metrics["f1_harmful"], 1.0, 0.001);
    EXPECT_NEAR(metrics["precision_safe"], 1.0, 0.001);
    EXPECT_NEAR(metrics["recall_safe"], 1.0, 0.001);
    EXPECT_NEAR(metrics["f1_safe"], 1.0, 0.001);
    EXPECT_NEAR(metrics["macro_precision"], 1.0, 0.001);
    EXPECT_NEAR(metrics["macro_recall"], 1.0, 0.001);
    EXPECT_NEAR(metrics["macro_f1"], 1.0, 0.001);
}

/**
 * @brief Tests metrics for worst-case classification
 * 
 * Verifies that metrics show 0% performance when
 * all predictions are opposite to the true labels.
 */
TEST_F(MetricsTest, WorstCaseClassificationMetrics) {
    auto metrics = blahajpi::evaluation::Metrics::calculateMetrics(worstTrueLabels, worstPredLabels);
    
    // Worst classification should have all metrics at 0.0 except recall and precision
    // which might be undefined and default to 0.0
    EXPECT_NEAR(metrics["accuracy"], 0.0, 0.001);
    EXPECT_NEAR(metrics["precision_harmful"], 0.0, 0.001);
    EXPECT_NEAR(metrics["recall_harmful"], 0.0, 0.001);
    EXPECT_NEAR(metrics["f1_harmful"], 0.0, 0.001);
    EXPECT_NEAR(metrics["precision_safe"], 0.0, 0.001);
    EXPECT_NEAR(metrics["recall_safe"], 0.0, 0.001);
    EXPECT_NEAR(metrics["f1_safe"], 0.0, 0.001);
    EXPECT_NEAR(metrics["macro_precision"], 0.0, 0.001);
    EXPECT_NEAR(metrics["macro_recall"], 0.0, 0.001);
    EXPECT_NEAR(metrics["macro_f1"], 0.0, 0.001);
}

/**
 * @brief Tests metrics for all-positive samples
 * 
 * Verifies metrics behavior when the dataset consists
 * only of positive samples.
 */
TEST_F(MetricsTest, AllPositiveSamplesMetrics) {
    auto metrics = blahajpi::evaluation::Metrics::calculateMetrics(allPosTrueLabels, allPosPredLabels);
    
    // With all samples being positive:
    // TP = 3, FN = 3, FP = 0, TN = 0
    
    // Accuracy = (TP + TN) / total = 3/6 = 0.5
    EXPECT_NEAR(metrics["accuracy"], 0.5, 0.001);
    
    // precision_harmful = TP / (TP + FP) = 3 / 3 = 1.0
    EXPECT_NEAR(metrics["precision_harmful"], 1.0, 0.001);
    
    // recall_harmful = TP / (TP + FN) = 3 / 6 = 0.5
    EXPECT_NEAR(metrics["recall_harmful"], 0.5, 0.001);
    
    // F1 = 2 * (precision * recall) / (precision + recall) = 2 * 1.0 * 0.5 / 1.5 = 0.667
    EXPECT_NEAR(metrics["f1_harmful"], 0.667, 0.001);
    
    // Safe metrics will be undefined or 0 as there are no true negatives
    // We expect implementation to return 0 for these cases
    EXPECT_NEAR(metrics["precision_safe"], 0.0, 0.001);
    EXPECT_NEAR(metrics["recall_safe"], 0.0, 0.001);
    EXPECT_NEAR(metrics["f1_safe"], 0.0, 0.001);
}

/**
 * @brief Tests metrics for all-negative samples
 * 
 * Verifies metrics behavior when the dataset consists
 * only of negative samples.
 */
TEST_F(MetricsTest, AllNegativeSamplesMetrics) {
    auto metrics = blahajpi::evaluation::Metrics::calculateMetrics(allNegTrueLabels, allNegPredLabels);
    
    // With all samples being negative:
    // TP = 0, FN = 0, FP = 3, TN = 3
    
    // Accuracy = (TP + TN) / total = 3/6 = 0.5
    EXPECT_NEAR(metrics["accuracy"], 0.5, 0.001);
    
    // Harmful metrics will be undefined or 0 as there are no true positives
    // We expect implementation to return 0 for these cases
    EXPECT_NEAR(metrics["precision_harmful"], 0.0, 0.001);
    EXPECT_NEAR(metrics["recall_harmful"], 0.0, 0.001);
    EXPECT_NEAR(metrics["f1_harmful"], 0.0, 0.001);
    
    // precision_safe = TN / (TN + FN) = 3 / 3 = 1.0
    EXPECT_NEAR(metrics["precision_safe"], 1.0, 0.001);
    
    // recall_safe = TN / (TN + FP) = 3 / 6 = 0.5
    EXPECT_NEAR(metrics["recall_safe"], 0.5, 0.001);
    
    // F1 = 2 * (precision * recall) / (precision + recall) = 2 * 1.0 * 0.5 / 1.5 = 0.667
    EXPECT_NEAR(metrics["f1_safe"], 0.667, 0.001);
}

/**
 * @brief Tests metrics with empty input data
 * 
 * Verifies graceful handling of empty input vectors.
 */
TEST_F(MetricsTest, EmptyInputMetrics) {
    auto metrics = blahajpi::evaluation::Metrics::calculateMetrics(emptyLabels, emptyLabels);
    
    // With empty data, we expect all metrics to be 0.0 by default
    EXPECT_NEAR(metrics["accuracy"], 0.0, 0.001);
    EXPECT_NEAR(metrics["precision_harmful"], 0.0, 0.001);
    EXPECT_NEAR(metrics["recall_harmful"], 0.0, 0.001);
    EXPECT_NEAR(metrics["f1_harmful"], 0.0, 0.001);
}

/**
 * @brief Tests the format of the classification report
 * 
 * Verifies that the report contains the expected sections
 * and formatting.
 */
TEST_F(MetricsTest, ClassificationReportFormat) {
    std::string report = blahajpi::evaluation::Metrics::classificationReport(balancedTrueLabels, balancedPredLabels);
    
    // Check if report contains key elements
    EXPECT_TRUE(report.find("Classification Report") != std::string::npos);
    EXPECT_TRUE(report.find("precision") != std::string::npos);
    EXPECT_TRUE(report.find("recall") != std::string::npos);
    EXPECT_TRUE(report.find("f1-score") != std::string::npos);
    EXPECT_TRUE(report.find("support") != std::string::npos);
    EXPECT_TRUE(report.find("Safe") != std::string::npos);
    EXPECT_TRUE(report.find("Harmful") != std::string::npos);
    EXPECT_TRUE(report.find("macro avg") != std::string::npos);
    EXPECT_TRUE(report.find("accuracy") != std::string::npos);
    
    // Check for expected metrics (80%)
    EXPECT_TRUE(report.find("80%") != std::string::npos);
}

/**
 * @brief Tests the area under the ROC curve calculation
 * 
 * Verifies that AUC-ROC is calculated correctly for various
 * datasets and edge cases.
 */
TEST_F(MetricsTest, AreaUnderROCCalculation) {
    // Test with balanced dataset
    double aucBalanced = blahajpi::evaluation::Metrics::areaUnderROC(balancedTrueLabels, balancedScores);
    
    // AUC should be between 0 and 1
    EXPECT_GE(aucBalanced, 0.0);
    EXPECT_LE(aucBalanced, 1.0);
    
    // For balanced dataset, AUC should be reasonably good
    EXPECT_GT(aucBalanced, 0.5);
    
    // Test with perfect classification
    double aucPerfect = blahajpi::evaluation::Metrics::areaUnderROC(perfectTrueLabels, perfectScores);
    
    // For perfect separation, AUC should be 1.0
    EXPECT_NEAR(aucPerfect, 1.0, 0.001);
    
    // Test with worst case
    double aucWorst = blahajpi::evaluation::Metrics::areaUnderROC(worstTrueLabels, worstScores);
    
    // For perfectly wrong classification, AUC should be 0.0
    EXPECT_NEAR(aucWorst, 0.0, 0.001);
    
    // Test with empty data
    double aucEmpty = blahajpi::evaluation::Metrics::areaUnderROC(emptyLabels, emptyScores);
    
    // For empty data, AUC is typically defined as 0.5
    EXPECT_NEAR(aucEmpty, 0.5, 0.001);
    
    // Test with single sample of each class (edge case)
    double aucSinglePos = blahajpi::evaluation::Metrics::areaUnderROC(singlePosTrueLabels, singlePosScores);
    double aucSingleNeg = blahajpi::evaluation::Metrics::areaUnderROC(singleNegTrueLabels, singleNegScores);
    
    // Single sample cases typically default to 0.5
    EXPECT_NEAR(aucSinglePos, 0.5, 0.001);
    EXPECT_NEAR(aucSingleNeg, 0.5, 0.001);
}

/**
 * @brief Tests the precision-recall curve calculation
 * 
 * Verifies that precision values at different recall levels
 * are calculated correctly.
 */
TEST_F(MetricsTest, PrecisionRecallCurveCalculation) {
    // Standard recall levels to evaluate
    std::vector<double> recallLevels = {0.25, 0.5, 0.75, 0.9};
    
    // Test with balanced dataset
    auto precRecallBalanced = blahajpi::evaluation::Metrics::precisionRecallCurve(
        balancedTrueLabels, balancedScores, recallLevels);
    
    // Check that all requested recall levels are present
    EXPECT_EQ(precRecallBalanced.size(), recallLevels.size());
    
    for (double level : recallLevels) {
        EXPECT_TRUE(precRecallBalanced.count(level) > 0);
        
        // Precision should be between 0 and 1
        EXPECT_GE(precRecallBalanced[level], 0.0);
        EXPECT_LE(precRecallBalanced[level], 1.0);
    }
    
    // Test with perfect classification
    auto precRecallPerfect = blahajpi::evaluation::Metrics::precisionRecallCurve(
        perfectTrueLabels, perfectScores, recallLevels);
    
    // For perfect classification, precision should be 1.0 at all recall levels
    for (double level : recallLevels) {
        EXPECT_NEAR(precRecallPerfect[level], 1.0, 0.001);
    }
    
    // Test with worst case
    auto precRecallWorst = blahajpi::evaluation::Metrics::precisionRecallCurve(
        worstTrueLabels, worstScores, recallLevels);
    
    // For worst case, precision should be 0 at all recall levels
    for (double level : recallLevels) {
        EXPECT_NEAR(precRecallWorst[level], 0.0, 0.001);
    }
    
    // Test with empty data
    auto precRecallEmpty = blahajpi::evaluation::Metrics::precisionRecallCurve(
        emptyLabels, emptyScores, recallLevels);
    
    // Empty data should return an empty map
    EXPECT_EQ(precRecallEmpty.size(), 0);
}

/**
 * @brief Tests the optimal threshold finding
 * 
 * Verifies that the algorithm correctly finds thresholds
 * that maximize different performance metrics.
 */
TEST_F(MetricsTest, OptimalThresholdFinding) {
    // Test finding threshold that maximizes F1 score
    double thresholdF1 = blahajpi::evaluation::Metrics::findOptimalThreshold(
        balancedTrueLabels, balancedScores, "f1");
    
    // Threshold should be between min and max score
    double minScore = *std::min_element(balancedScores.begin(), balancedScores.end());
    double maxScore = *std::max_element(balancedScores.begin(), balancedScores.end());
    
    EXPECT_GE(thresholdF1, minScore);
    EXPECT_LE(thresholdF1, maxScore);
    
    // Test finding threshold that maximizes accuracy
    double thresholdAcc = blahajpi::evaluation::Metrics::findOptimalThreshold(
        balancedTrueLabels, balancedScores, "accuracy");
    
    EXPECT_GE(thresholdAcc, minScore);
    EXPECT_LE(thresholdAcc, maxScore);
    
    // Test finding threshold that maximizes precision
    double thresholdPrec = blahajpi::evaluation::Metrics::findOptimalThreshold(
        balancedTrueLabels, balancedScores, "precision");
    
    EXPECT_GE(thresholdPrec, minScore);
    EXPECT_LE(thresholdPrec, maxScore);
    
    // Precision threshold should typically be higher than recall threshold
    double thresholdRec = blahajpi::evaluation::Metrics::findOptimalThreshold(
        balancedTrueLabels, balancedScores, "recall");
    
    EXPECT_GE(thresholdPrec, thresholdRec);
}

/**
 * @brief Tests threshold finding with different metrics
 * 
 * Verifies that different optimization metrics result
 * in different threshold values.
 */
TEST_F(MetricsTest, ThresholdMetricDifferences) {
    // Get thresholds optimized for different metrics
    double thresholdF1 = blahajpi::evaluation::Metrics::findOptimalThreshold(
        balancedTrueLabels, balancedScores, "f1");
    
    double thresholdPrec = blahajpi::evaluation::Metrics::findOptimalThreshold(
        balancedTrueLabels, balancedScores, "precision");
    
    double thresholdRec = blahajpi::evaluation::Metrics::findOptimalThreshold(
        balancedTrueLabels, balancedScores, "recall");
    
    // These thresholds should typically be different
    // Precision-optimized threshold is usually higher than recall-optimized
    EXPECT_GE(thresholdPrec, thresholdRec);
    
    // F1-optimized threshold should be somewhere between precision and recall thresholds
    // or equal to one of them
    bool f1Between = (thresholdF1 >= thresholdRec && thresholdF1 <= thresholdPrec) ||
                     (thresholdF1 <= thresholdRec && thresholdF1 >= thresholdPrec);
    bool f1EqualToOne = (thresholdF1 == thresholdPrec || thresholdF1 == thresholdRec);
    
    EXPECT_TRUE(f1Between || f1EqualToOne);
    
    // Test with invalid metric name (should default to F1)
    double thresholdInvalid = blahajpi::evaluation::Metrics::findOptimalThreshold(
        balancedTrueLabels, balancedScores, "invalid_metric_name");
    
    EXPECT_EQ(thresholdInvalid, thresholdF1);
}

} // namespace
