/**
 * @file metrics.hpp
 * @brief Evaluation metrics for classification performance
 * 
 * This file provides functions for evaluating model performance,
 * including metrics like accuracy, precision, recall, and F1 score.
 * It helps measure how well the model identifies harmful content.
 */

#pragma once

#include <vector>
#include <unordered_map>
#include <string>

namespace blahajpi {
namespace evaluation {

/**
 * @brief Contains methods for evaluating classification performance
 * 
 * This class provides static methods to compute various evaluation metrics
 * for classification tasks, including confusion matrix, precision, recall,
 * F1 score, and accuracy.
 */
class Metrics {
public:
    /**
     * @brief Computes the confusion matrix
     * 
     * The confusion matrix shows the counts of true positives, false positives,
     * true negatives, and false negatives in a 2x2 matrix format.
     * 
     * @param yTrue Ground truth labels
     * @param yPred Predicted labels
     * @return 2D matrix as [[TN, FP], [FN, TP]]
     */
    static std::vector<std::vector<int>> confusionMatrix(
        const std::vector<int>& yTrue,
        const std::vector<int>& yPred
    );

    /**
     * @brief Generates a classification report with precision, recall, and F1-score
     * 
     * This method creates a formatted string containing key performance metrics
     * for each class and the overall model.
     * 
     * @param yTrue Ground truth labels
     * @param yPred Predicted labels
     * @param classNames Names for the classes (defaults to "Safe"/"Harmful")
     * @return Formatted classification report
     */
    static std::string classificationReport(
        const std::vector<int>& yTrue,
        const std::vector<int>& yPred,
        const std::vector<std::string>& classNames = {"Safe", "Harmful"}
    );

    /**
     * @brief Computes precision, recall, F1-score, and accuracy
     * 
     * This method calculates all standard classification metrics in one call,
     * returning them in a map for convenient access.
     * 
     * @param yTrue Ground truth labels
     * @param yPred Predicted labels
     * @return Map containing all computed metrics
     */
    static std::unordered_map<std::string, double> calculateMetrics(
        const std::vector<int>& yTrue,
        const std::vector<int>& yPred
    );
    
    /**
     * @brief Computes the Area Under the ROC Curve (AUC-ROC)
     * 
     * This metric measures the model's ability to discriminate between classes
     * across different threshold values.
     * 
     * @param yTrue Ground truth labels
     * @param scores Predicted scores or probabilities
     * @return AUC-ROC score between 0.0 and 1.0
     */
    static double areaUnderROC(
        const std::vector<int>& yTrue,
        const std::vector<double>& scores
    );
    
    /**
     * @brief Computes precision at different recall levels
     * 
     * This method calculates precision values at different recall thresholds,
     * which is useful for evaluating model performance at different operating points.
     * 
     * @param yTrue Ground truth labels
     * @param scores Predicted scores or probabilities
     * @param recallLevels Recall levels at which to compute precision
     * @return Map of recall levels to precision values
     */
    static std::unordered_map<double, double> precisionRecallCurve(
        const std::vector<int>& yTrue,
        const std::vector<double>& scores,
        const std::vector<double>& recallLevels = {0.25, 0.5, 0.75, 0.9, 0.95}
    );
    
    /**
     * @brief Finds the optimal classification threshold
     * 
     * This method finds the threshold that maximizes a specified metric like
     * F1 score, accuracy, or precision.
     * 
     * @param yTrue Ground truth labels
     * @param scores Predicted scores or probabilities
     * @param metric Metric to optimize ("f1", "accuracy", "precision", "recall")
     * @return Optimal threshold value
     */
    static double findOptimalThreshold(
        const std::vector<int>& yTrue,
        const std::vector<double>& scores,
        const std::string& metric = "f1"
    );

private:
    /**
     * @brief Calculates precision score
     * @param tp True positives
     * @param fp False positives
     * @return Precision score
     */
    static double precision(int tp, int fp);

    /**
     * @brief Calculates recall score
     * @param tp True positives
     * @param fn False negatives
     * @return Recall score
     */
    static double recall(int tp, int fn);

    /**
     * @brief Calculates F1 score
     * @param precision Precision value
     * @param recall Recall value
     * @return F1 score
     */
    static double f1Score(double precision, double recall);
    
    /**
     * @brief Calculates accuracy
     * @param tp True positives
     * @param tn True negatives
     * @param total Total number of samples
     * @return Accuracy score
     */
    static double accuracy(int tp, int tn, int total);
};

} // namespace evaluation
} // namespace blahajpi
