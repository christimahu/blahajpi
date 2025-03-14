/**
 * @file metrics.cpp
 * @brief Implementation of the evaluation metrics
 */

#include "blahajpi/evaluation/metrics.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <vector>
#include <utility>

namespace blahajpi {
namespace evaluation {

std::vector<std::vector<int>> Metrics::confusionMatrix(
    const std::vector<int>& yTrue,
    const std::vector<int>& yPred
) {
    // Initialize 2x2 confusion matrix
    std::vector<std::vector<int>> matrix(2, std::vector<int>(2, 0));
    
    // Count each prediction type
    for (size_t i = 0; i < yTrue.size() && i < yPred.size(); ++i) {
        // Convert labels from {0,4} to {0,1} for matrix indexing
        int actual = (yTrue[i] == 0) ? 0 : 1;
        int predicted = (yPred[i] == 0) ? 0 : 1;
        
        // Increment the appropriate cell in the confusion matrix
        matrix[actual][predicted]++;
    }
    
    return matrix;
}

std::string Metrics::classificationReport(
    const std::vector<int>& yTrue,
    const std::vector<int>& yPred,
    const std::vector<std::string>& classNames
) {
    // Compute confusion matrix
    auto matrix = confusionMatrix(yTrue, yPred);
    
    // Get matrix values
    int tn = matrix[0][0];  // True Negatives (Safe correctly identified as Safe)
    int fp = matrix[0][1];  // False Positives (Safe incorrectly identified as Harmful)
    int fn = matrix[1][0];  // False Negatives (Harmful incorrectly identified as Safe)
    int tp = matrix[1][1];  // True Positives (Harmful correctly identified as Harmful)
    
    // Calculate metrics
    double precisionSafe = precision(tn, fn);
    double precisionHarmful = precision(tp, fp);
    double recallSafe = recall(tn, fp);
    double recallHarmful = recall(tp, fn);
    double f1Safe = f1Score(precisionSafe, recallSafe);
    double f1Harmful = f1Score(precisionHarmful, recallHarmful);
    
    // Calculate macro-averaged metrics
    double macroPrec = (precisionSafe + precisionHarmful) / 2.0;
    double macroRec = (recallSafe + recallHarmful) / 2.0;
    double macroF1 = (f1Safe + f1Harmful) / 2.0;
    
    // Calculate accuracy
    int totalSamples = tn + fp + fn + tp;
    double accuracy = totalSamples > 0 ? 
                     static_cast<double>(tn + tp) / totalSamples : 0.0;
    
    // Format the report
    std::ostringstream report;
    report << std::fixed << std::setprecision(2);
    
    // Header
    report << "Classification Report:\n";
    report << std::setw(20) << "precision" << std::setw(10) << "recall"
           << std::setw(10) << "f1-score" << std::setw(10) << "support" << "\n";
    report << std::string(50, '-') << "\n";
    
    // Class: Safe
    report << std::setw(20) << classNames[0] << std::setw(10) << (precisionSafe * 100)
           << "%" << std::setw(9) << (recallSafe * 100) << "%"
           << std::setw(9) << (f1Safe * 100) << "%"
           << std::setw(10) << (tn + fp) << "\n";
    
    // Class: Harmful
    report << std::setw(20) << classNames[1] << std::setw(10) << (precisionHarmful * 100)
           << "%" << std::setw(9) << (recallHarmful * 100) << "%"
           << std::setw(9) << (f1Harmful * 100) << "%"
           << std::setw(10) << (tp + fn) << "\n";
    
    report << std::string(50, '-') << "\n";
    
    // Macro average
    report << std::setw(20) << "macro avg" << std::setw(10) << (macroPrec * 100)
           << "%" << std::setw(9) << (macroRec * 100) << "%"
           << std::setw(9) << (macroF1 * 100) << "%"
           << std::setw(10) << totalSamples << "\n";
    
    // Accuracy
    report << std::setw(20) << "accuracy" << std::setw(29) << (accuracy * 100)
           << "%" << std::setw(10) << totalSamples << "\n";
    
    return report.str();
}

std::unordered_map<std::string, double> Metrics::calculateMetrics(
    const std::vector<int>& yTrue,
    const std::vector<int>& yPred
) {
    // Compute confusion matrix
    auto matrix = confusionMatrix(yTrue, yPred);
    
    // Get matrix values
    int tn = matrix[0][0];  // True Negatives
    int fp = matrix[0][1];  // False Positives
    int fn = matrix[1][0];  // False Negatives
    int tp = matrix[1][1];  // True Positives
    
    // Calculate metrics
    double precSafe = precision(tn, fn);
    double precHarmful = precision(tp, fp);
    double recallSafe = recall(tn, fp);
    double recallHarmful = recall(tp, fn);
    double f1Safe = f1Score(precSafe, recallSafe);
    double f1Harmful = f1Score(precHarmful, recallHarmful);
    
    // Calculate macro-averaged metrics
    double macroPrec = (precSafe + precHarmful) / 2.0;
    double macroRec = (recallSafe + recallHarmful) / 2.0;
    double macroF1 = (f1Safe + f1Harmful) / 2.0;
    
    // Calculate micro-averaged metrics (same as accuracy in binary case)
    int totalSamples = tn + fp + fn + tp;
    double accuracy = totalSamples > 0 ? 
                    static_cast<double>(tn + tp) / totalSamples : 0.0;
    
    // Store in map
    std::unordered_map<std::string, double> metrics;
    metrics["accuracy"] = accuracy;
    metrics["precision_safe"] = precSafe;
    metrics["precision_harmful"] = precHarmful;
    metrics["recall_safe"] = recallSafe;
    metrics["recall_harmful"] = recallHarmful;
    metrics["f1_safe"] = f1Safe;
    metrics["f1_harmful"] = f1Harmful;
    metrics["macro_precision"] = macroPrec;
    metrics["macro_recall"] = macroRec;
    metrics["macro_f1"] = macroF1;
    metrics["true_negatives"] = static_cast<double>(tn);
    metrics["false_positives"] = static_cast<double>(fp);
    metrics["false_negatives"] = static_cast<double>(fn);
    metrics["true_positives"] = static_cast<double>(tp);
    
    return metrics;
}

double Metrics::areaUnderROC(
    const std::vector<int>& yTrue,
    const std::vector<double>& scores
) {
    // Pair scores with truth for sorting
    std::vector<std::pair<double, int>> scoreLabelPairs;
    for (size_t i = 0; i < scores.size() && i < yTrue.size(); ++i) {
        scoreLabelPairs.emplace_back(scores[i], yTrue[i]);
    }
    
    // Sort by score in descending order
    std::sort(scoreLabelPairs.begin(), scoreLabelPairs.end(),
             [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Count positive and negative samples
    int positiveCount = 0;
    int negativeCount = 0;
    
    for (int label : yTrue) {
        if (label != 0) {  // Assuming 0 is the negative class
            positiveCount++;
        } else {
            negativeCount++;
        }
    }
    
    // If all samples are of the same class, AUC is not well-defined
    if (positiveCount == 0 || negativeCount == 0) {
        return 0.5;  // By convention
    }
    
    // Compute AUC using the trapezoidal rule
    double auc = 0.0;
    double tpPrev = 0.0;
    double fpPrev = 0.0;
    double fpRate, tpRate;
    
    for (const auto& [score, label] : scoreLabelPairs) {
        // Update true positive and false positive counts
        if (label != 0) {  // Positive class
            tpRate = (tpPrev + 1.0) / positiveCount;
            fpRate = fpPrev / negativeCount;
        } else {  // Negative class
            tpRate = tpPrev / positiveCount;
            fpRate = (fpPrev + 1.0) / negativeCount;
        }
        
        // Add trapezoidal area
        auc += (fpRate - fpPrev) * (tpRate + tpPrev) / 2.0;
        
        // Update counters for next iteration
        if (label != 0) {
            tpPrev += 1.0;
        } else {
            fpPrev += 1.0;
        }
    }
    
    return auc;
}

std::unordered_map<double, double> Metrics::precisionRecallCurve(
    const std::vector<int>& yTrue,
    const std::vector<double>& scores,
    const std::vector<double>& recallLevels
) {
    // Pair scores with truth for sorting
    std::vector<std::pair<double, int>> scoreLabelPairs;
    for (size_t i = 0; i < scores.size() && i < yTrue.size(); ++i) {
        scoreLabelPairs.emplace_back(scores[i], yTrue[i]);
    }
    
    // Sort by score in descending order
    std::sort(scoreLabelPairs.begin(), scoreLabelPairs.end(),
             [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Count positive samples
    int positiveCount = 0;
    for (int label : yTrue) {
        if (label != 0) {  // Positive class
            positiveCount++;
        }
    }
    
    // If no positive samples, return empty result
    if (positiveCount == 0) {
        return {};
    }
    
    // Compute precision and recall at every threshold
    std::vector<double> precisions;
    std::vector<double> recalls;
    std::vector<double> thresholds;
    
    int tp = 0;
    int fp = 0;
    double prevScore = std::numeric_limits<double>::infinity();
    
    for (const auto& [score, label] : scoreLabelPairs) {
        // If score changes, save metrics at this threshold
        if (score != prevScore) {
            precisions.push_back(tp > 0 ? static_cast<double>(tp) / (tp + fp) : 1.0);
            recalls.push_back(static_cast<double>(tp) / positiveCount);
            thresholds.push_back(score);
            prevScore = score;
        }
        
        // Update counters
        if (label != 0) {  // Positive class
            tp++;
        } else {  // Negative class
            fp++;
        }
    }
    
    // Add final point
    precisions.push_back(tp > 0 ? static_cast<double>(tp) / (tp + fp) : 1.0);
    recalls.push_back(static_cast<double>(tp) / positiveCount);
    
    // Interpolate precision at given recall levels
    std::unordered_map<double, double> precisionAtRecall;
    
    for (double targetRecall : recallLevels) {
        double maxPrecision = 0.0;
        
        // Find the maximum precision for recall >= targetRecall
        for (size_t i = 0; i < recalls.size(); ++i) {
            if (recalls[i] >= targetRecall) {
                maxPrecision = std::max(maxPrecision, precisions[i]);
            }
        }
        
        precisionAtRecall[targetRecall] = maxPrecision;
    }
    
    return precisionAtRecall;
}

double Metrics::findOptimalThreshold(
    const std::vector<int>& yTrue,
    const std::vector<double>& scores,
    const std::string& metric
) {
    // Pair scores with truth for sorting
    std::vector<std::pair<double, int>> scoreLabelPairs;
    for (size_t i = 0; i < scores.size() && i < yTrue.size(); ++i) {
        scoreLabelPairs.emplace_back(scores[i], yTrue[i]);
    }
    
    // Sort by score in descending order
    std::sort(scoreLabelPairs.begin(), scoreLabelPairs.end(),
             [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Test each threshold (each unique score value)
    double bestThreshold = 0.0;
    double bestScore = -1.0;  // Start with worst possible score
    double prevScore = std::numeric_limits<double>::infinity();
    
    int tp = 0, fp = 0, tn = 0, fn = 0;
    
    // Count total positives and negatives
    int totalPositives = 0, totalNegatives = 0;
    for (int label : yTrue) {
        if (label != 0) {  // Positive class
            totalPositives++;
        } else {  // Negative class
            totalNegatives++;
        }
    }
    
    // Initially, all predictions are negative
    tn = totalNegatives;
    fn = totalPositives;
    
    for (const auto& [score, label] : scoreLabelPairs) {
        // If score changes, evaluate this threshold
        if (score != prevScore) {
            double metricValue = 0.0;
            
            if (metric == "f1") {
                double prec = (tp + fp > 0) ? static_cast<double>(tp) / (tp + fp) : 0.0;
                double rec = (tp + fn > 0) ? static_cast<double>(tp) / (tp + fn) : 0.0;
                metricValue = (prec + rec > 0) ? 2.0 * prec * rec / (prec + rec) : 0.0;
            } else if (metric == "accuracy") {
                metricValue = static_cast<double>(tp + tn) / (tp + tn + fp + fn);
            } else if (metric == "precision") {
                metricValue = (tp + fp > 0) ? static_cast<double>(tp) / (tp + fp) : 0.0;
            } else if (metric == "recall") {
                metricValue = (tp + fn > 0) ? static_cast<double>(tp) / (tp + fn) : 0.0;
            } else {
                // Default to F1
                double prec = (tp + fp > 0) ? static_cast<double>(tp) / (tp + fp) : 0.0;
                double rec = (tp + fn > 0) ? static_cast<double>(tp) / (tp + fn) : 0.0;
                metricValue = (prec + rec > 0) ? 2.0 * prec * rec / (prec + rec) : 0.0;
            }
            
            // Update best threshold if this is better
            if (metricValue > bestScore) {
                bestScore = metricValue;
                bestThreshold = score;
            }
            
            prevScore = score;
        }
        
        // Update counters for next threshold
        if (label != 0) {  // Positive class
            fn--;
            tp++;
        } else {  // Negative class
            tn--;
            fp++;
        }
    }
    
    return bestThreshold;
}

double Metrics::precision(int tp, int fp) {
    return (tp + fp > 0) ? static_cast<double>(tp) / (tp + fp) : 0.0;
}

double Metrics::recall(int tp, int fn) {
    return (tp + fn > 0) ? static_cast<double>(tp) / (tp + fn) : 0.0;
}

double Metrics::f1Score(double precision, double recall) {
    return (precision + recall > 0) ? 
           2.0 * (precision * recall) / (precision + recall) : 0.0;
}

double Metrics::accuracy(int tp, int tn, int total) {
    return (total > 0) ? static_cast<double>(tp + tn) / total : 0.0;
}

} // namespace evaluation
} // namespace blahajpi
