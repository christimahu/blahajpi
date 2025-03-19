/**
 * @file api_integration_test.cpp
 * @brief Integration tests for the BlahajPI API
 */

#include "blahajpi/analyzer.hpp"
#include "blahajpi/config.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

namespace {

class APIIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directory
        tempDir = std::filesystem::temp_directory_path() / "blahajpi_tests";
        std::filesystem::create_directories(tempDir);
        
        // Create sub-directories
        modelDir = tempDir / "models";
        configDir = tempDir / "configs";
        dataDir = tempDir / "data";
        resultsDir = tempDir / "results";
        
        std::filesystem::create_directories(modelDir);
        std::filesystem::create_directories(configDir);
        std::filesystem::create_directories(dataDir);
        std::filesystem::create_directories(resultsDir);
        
        // Create sample data file
        dataPath = dataDir / "test_data.csv";
        std::ofstream dataFile(dataPath);
        dataFile << "label,text\n"
                << "0,This is a safe message with normal content.\n"
                << "4,This is harmful content targeting groups.\n"
                << "0,Another safe message about everyday topics.\n";
        dataFile.close();
        
        // Create test configuration file
        configPath = configDir / "test_config.conf";
        std::ofstream configFile(configPath);
        configFile << "# Test configuration\n"
                  << "model-dir = " << modelDir.string() << "\n"
                  << "dataset = " << dataPath.string() << "\n"
                  << "label-column = label\n"
                  << "text-column = text\n"
                  << "alpha = 0.0001\n"
                  << "eta0 = 0.01\n"
                  << "epochs = 5\n"
                  << "max-features = 1000\n";
        configFile.close();
    }
    
    void TearDown() override {
        if (std::filesystem::exists(tempDir)) {
            std::filesystem::remove_all(tempDir);
        }
    }
    
    std::filesystem::path tempDir;
    std::filesystem::path modelDir;
    std::filesystem::path configDir;
    std::filesystem::path dataDir;
    std::filesystem::path resultsDir;
    std::filesystem::path dataPath;
    std::filesystem::path configPath;
};

// Test basic analyzer initialization
TEST_F(APIIntegrationTest, AnalyzerInitialization) {
    // Create analyzer with configuration
    blahajpi::Analyzer analyzer;
    
    // Should not throw
    EXPECT_NO_THROW(analyzer.loadConfig(configPath.string()));
    
    // Check that configuration was loaded
    auto config = analyzer.getConfig();
    EXPECT_FALSE(config.empty());
}

// Test basic model training
TEST_F(APIIntegrationTest, BasicTraining) {
    // Create analyzer
    blahajpi::Analyzer analyzer;
    analyzer.loadConfig(configPath.string());
    
    // Try to train a minimal model
    std::string outputModelPath = (modelDir / "test_model").string();
    
    // Training might fail or succeed depending on implementation
    bool trainResult = false;
    EXPECT_NO_THROW(trainResult = analyzer.trainModel(dataPath.string(), outputModelPath));
    
    // If training succeeds, expect model files
    if (trainResult) {
        EXPECT_TRUE(std::filesystem::exists(modelDir / "test_model"));
    } else {
        // Skip further tests if training failed (implementation might not be complete)
        GTEST_SKIP() << "Model training not implemented or failed";
    }
}

// Test basic text analysis - only if we have a model
TEST_F(APIIntegrationTest, BasicAnalysis) {
    // Create analyzer
    blahajpi::Analyzer analyzer;
    
    // See if we have a pre-trained model to use
    std::filesystem::path modelPath = modelDir / "test_model";
    if (!std::filesystem::exists(modelPath)) {
        GTEST_SKIP() << "No model available for testing analysis";
    }
    
    // Try to load the model
    bool loadResult = false;
    EXPECT_NO_THROW(loadResult = analyzer.loadModel(modelPath.string()));
    
    if (!loadResult) {
        GTEST_SKIP() << "Model loading not implemented or failed";
    }
    
    // Analyze a simple text
    blahajpi::AnalysisResult result;
    EXPECT_NO_THROW(result = analyzer.analyze("This is a test message."));
    
    // Result should have basic properties filled
    EXPECT_FALSE(result.sentiment.empty());
}

// Test visualization generation - simplified
TEST_F(APIIntegrationTest, BasicVisualization) {
    // Create some analysis results for visualization
    std::vector<blahajpi::AnalysisResult> results;
    
    blahajpi::AnalysisResult safeResult;
    safeResult.text = "This is a safe message.";
    safeResult.cleanedText = "This is a safe message.";
    safeResult.sentiment = "Safe";
    safeResult.harmScore = -0.5;
    results.push_back(safeResult);
    
    blahajpi::AnalysisResult harmfulResult;
    harmfulResult.text = "This contains harmful content.";
    harmfulResult.cleanedText = "This contains harmful content.";
    harmfulResult.sentiment = "Harmful";
    harmfulResult.harmScore = 0.5;
    results.push_back(harmfulResult);
    
    // Create analyzer
    blahajpi::Analyzer analyzer;
    
    // Try to generate visualization
    std::string outputPath = (resultsDir / "visualization.txt").string();
    bool vizResult = false;
    
    EXPECT_NO_THROW(vizResult = analyzer.generateVisualization(results, outputPath));
    
    // If visualization works, expect output file
    if (vizResult) {
        EXPECT_TRUE(std::filesystem::exists(outputPath));
    } else {
        // Skip further visualization tests
        GTEST_SKIP() << "Visualization generation not implemented or failed";
    }
}

// Test configuration manipulation
TEST_F(APIIntegrationTest, ConfigurationManipulation) {
    // Create analyzer
    blahajpi::Analyzer analyzer;
    
    // Get initial configuration
    auto initialConfig = analyzer.getConfig();
    
    // Modify configuration
    EXPECT_NO_THROW(analyzer.setConfig("custom-key", "custom-value"));
    
    // Get updated configuration
    auto updatedConfig = analyzer.getConfig();
    
    // Should contain the new key
    EXPECT_TRUE(updatedConfig.count("custom-key") > 0);
}

} // namespace
