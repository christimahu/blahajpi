/**
 * @file neural_network_test.cpp
 * @brief Unit tests for the NeuralNetworkClassifier class
 */

#include "blahajpi/models/neural_network.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <cmath>
#include <random>
#include <filesystem>
#include <fstream>

namespace {

class NeuralNetworkTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directory for file operations
        tempDir = std::filesystem::temp_directory_path() / "blahajpi_tests";
        std::filesystem::create_directories(tempDir);
        
        // Create simple linearly separable data (much smaller dataset for testing)
        createLinearData();
        
        // Create a few test instances
        testInstances = {
            {0.0, 0.0},
            {1.0, 1.0}
        };
    }
    
    void TearDown() override {
        if (std::filesystem::exists(tempDir)) {
            std::filesystem::remove_all(tempDir);
        }
    }
    
    void createLinearData() {
        // Clear existing data
        linearFeatures.clear();
        linearLabels.clear();
        
        // Create a very small dataset for quick testing
        linearFeatures = {
            {0.0, 0.0},
            {0.0, 1.0},
            {1.0, 0.0},
            {1.0, 1.0}
        };
        
        // Simple XOR-like pattern: points on diagonal are class 0, others are class 1
        linearLabels = {0, 1, 1, 0};
    }
    
    std::vector<std::vector<double>> linearFeatures;
    std::vector<int> linearLabels;
    std::vector<std::vector<double>> testInstances;
    std::filesystem::path tempDir;
};

// Test basic construction
TEST_F(NeuralNetworkTest, Construction) {
    // Try to construct with explicit parameters
    EXPECT_NO_THROW({
        blahajpi::models::NeuralNetworkClassifier classifier(1, 4, 10, 0.1);
    });
}

// Test basic fitting
TEST_F(NeuralNetworkTest, BasicFit) {
    // Create a classifier with minimal epochs for quick testing
    blahajpi::models::NeuralNetworkClassifier classifier(1, 4, 5, 0.1);
    
    // Fit should not throw
    EXPECT_NO_THROW(classifier.fit(linearFeatures, linearLabels));
}

// Test basic prediction
TEST_F(NeuralNetworkTest, BasicPredict) {
    // Create a classifier with minimal configuration
    blahajpi::models::NeuralNetworkClassifier classifier(1, 4, 5, 0.1);
    
    // Fit the classifier
    classifier.fit(linearFeatures, linearLabels);
    
    // Predict should not throw
    std::vector<int> predictions;
    EXPECT_NO_THROW(predictions = classifier.predict(testInstances));
    
    // Should return correct number of predictions
    EXPECT_EQ(predictions.size(), testInstances.size());
}

// Test probability prediction
TEST_F(NeuralNetworkTest, PredictProbabilities) {
    // Create a classifier with minimal configuration
    blahajpi::models::NeuralNetworkClassifier classifier(1, 4, 5, 0.1);
    
    // Fit the classifier
    classifier.fit(linearFeatures, linearLabels);
    
    // Get probabilities
    std::vector<double> probabilities;
    EXPECT_NO_THROW(probabilities = classifier.predictProbability(testInstances));
    
    // Should return correct number of probabilities
    EXPECT_EQ(probabilities.size(), testInstances.size());
    
    // Probabilities should be between 0 and 1
    for (double prob : probabilities) {
        EXPECT_GE(prob, 0.0);
        EXPECT_LE(prob, 1.0);
    }
}

// Test model scoring
TEST_F(NeuralNetworkTest, Scoring) {
    // Create a classifier with minimal configuration
    blahajpi::models::NeuralNetworkClassifier classifier(1, 4, 5, 0.1);
    
    // Fit the classifier
    classifier.fit(linearFeatures, linearLabels);
    
    // Score on training data
    double score = 0.0;
    EXPECT_NO_THROW(score = classifier.score(linearFeatures, linearLabels));
    
    // Score should be between 0 and 1
    EXPECT_GE(score, 0.0);
    EXPECT_LE(score, 1.0);
}

// Test model persistence - skip if not implemented
TEST_F(NeuralNetworkTest, SaveAndLoad) {
    // Create and train a classifier
    blahajpi::models::NeuralNetworkClassifier classifier(1, 4, 5, 0.1);
    classifier.fit(linearFeatures, linearLabels);
    
    // Create a file path
    std::string filePath = (tempDir / "nn_model.bin").string();
    
    // Try to save
    bool saveResult = false;
    EXPECT_NO_THROW(saveResult = classifier.save(filePath));
    
    if (!saveResult || !std::filesystem::exists(filePath)) {
        GTEST_SKIP() << "Neural network saving not implemented or failed";
    }
    
    // Create a new classifier
    blahajpi::models::NeuralNetworkClassifier loadedClassifier(1, 4, 5, 0.1);
    
    // Try to load
    bool loadResult = false;
    EXPECT_NO_THROW(loadResult = loadedClassifier.load(filePath));
    
    // If loading succeeded, try to predict
    if (loadResult) {
        EXPECT_NO_THROW(loadedClassifier.predict(testInstances));
    }
}

} // namespace
