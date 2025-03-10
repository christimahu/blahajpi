/**
 * @file metrics_test.cpp
 * @brief Unit tests for the evaluation metrics
 */

#include "blahajpi/evaluation/metrics.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    // Simple functionality test
    try {
        // Create test data
        std::vector<int> yTrue = {0, 1, 0, 1};
        std::vector<int> yPred = {0, 1, 0, 0};
        
        // Calculate confusion matrix
        auto matrix = blahajpi::evaluation::Metrics::confusionMatrix(yTrue, yPred);
        
        // This is just a placeholder test
        std::cout << "Metrics test placeholder - Test passed" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
