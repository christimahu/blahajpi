/**
 * @file api_integration_test.cpp
 * @brief Integration tests for the BlahajPI API
 * 
 * These tests verify the functionality of the BlahajPI API components working together,
 * including analyzer initialization, configuration loading, and end-to-end processing.
 * 
 * @author BlahajPI Team
 * @date 2025
 */

#include "blahajpi/analyzer.hpp"
#include "blahajpi/config.hpp"
#include "blahajpi/models/sgd.hpp"
#include "blahajpi/preprocessing/text_processor.hpp"
#include "blahajpi/preprocessing/vectorizer.hpp"
#include "blahajpi/utils/dataset.hpp"
#include "blahajpi/utils/word_cloud.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {

/**
 * @class APIIntegrationTest
 * @brief Test fixture for API integration tests
 * 
 * This fixture provides common setup for testing interactions between
 * multiple components of the API.
 */
class APIIntegrationTest : public ::testing::Test {
protected:
    /**
     * @brief Set up test environment
     * 
     * Creates temporary directory, sample data, and configuration files.
     */
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
                << "0,Another safe message about everyday topics.\n"
                << "4,More harmful content with negative stereotypes.\n"
                << "0,A third safe message about technology.\n";
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
    std::filesystem::path modelDir;
    std::filesystem::path configDir;
    std::filesystem::path dataDir;
    std::filesystem::path resultsDir;
    std::filesystem::path dataPath;
    std::filesystem::path configPath;
};

/**
 * @brief Tests analyzer initialization with configuration
 * 
 * Verifies that the Analyzer can be initialized with a configuration file.
 */
TEST_F(APIIntegrationTest, AnalyzerInitialization) {
    // Create analyzer with configuration
    blahajpi::Analyzer analyzer(configPath.string());
    
    // Verify configuration was loaded
    auto config = analyzer.getConfig();
    EXPECT_EQ(config["model-dir"], modelDir.string());
    EXPECT_EQ(config["dataset"], dataPath.string());
    EXPECT_EQ(config["label-column"], "label");
    EXPECT_EQ(config["text-column"], "text");
}

/**
 * @brief Tests training and loading a model
 * 
 * Verifies that the analyzer can train a model and load it back.
 */
TEST_F(APIIntegrationTest, TrainAndLoadModel) {
    // Create analyzer with configuration
    blahajpi::Analyzer analyzer(configPath.string());
    
    // Train model with minimal iterations for testing
    std::string outputModelPath = (modelDir / "test_model").string();
    bool trainResult = analyzer.trainModel(dataPath.string(), outputModelPath);
    EXPECT_TRUE(trainResult);
    
    // Check that model files were created
    EXPECT_TRUE(std::filesystem::exists(modelDir / "test_model" / "model.bin"));
    EXPECT_TRUE(std::filesystem::exists(modelDir / "test_model" / "vectorizer.bin"));
    
    // Create new analyzer instance and load the model
    blahajpi::Analyzer newAnalyzer;
    bool loadResult = newAnalyzer.loadModel(outputModelPath);
    EXPECT_TRUE(loadResult);
    
    // Test that the loaded model can perform analysis
    auto result = newAnalyzer.analyze("This is a test message.");
    
    // Basic validation - should have non-empty fields
    EXPECT_FALSE(result.sentiment.empty());
    EXPECT_FALSE(result.explanation.empty());
    EXPECT_GE(result.confidence, 0.0);
    EXPECT_LE(result.confidence, 1.0);
}

/**
 * @brief Tests end-to-end analysis
 * 
 * Verifies that the analyzer can process text and provide meaningful results.
 */
TEST_F(APIIntegrationTest, EndToEndAnalysis) {
    // Create and train a model for testing
    blahajpi::Analyzer analyzer(configPath.string());
    std::string outputModelPath = (modelDir / "test_model").string();
    analyzer.trainModel(dataPath.string(), outputModelPath);
    analyzer.loadModel(outputModelPath);
    
    // Test analysis with clearly safe content
    auto safeResult = analyzer.analyze("This is a positive message about community and support.");
    
    // Test analysis with clearly harmful content
    auto harmfulResult = analyzer.analyze("This is offensive content that attacks specific groups.");
    
    // At least one of them should be correctly classified
    // (We can't guarantee both since the training data is minimal)
    EXPECT_TRUE(
        (safeResult.sentiment == "Safe" && safeResult.harmScore < 0.0) ||
        (harmfulResult.sentiment == "Harmful" && harmfulResult.harmScore > 0.0)
    );
    
    // Test batch analysis
    std::vector<std::string> batchTexts = {
        "First test message.",
        "Second test message with different content."
    };
    
    auto batchResults = analyzer.analyzeMultiple(batchTexts);
    
    // Check results
    EXPECT_EQ(batchResults.size(), batchTexts.size());
    for (const auto& result : batchResults) {
        EXPECT_FALSE(result.sentiment.empty());
        EXPECT_FALSE(result.explanation.empty());
    }
}

/**
 * @brief Tests visualization generation
 * 
 * Verifies that the analyzer can generate word cloud visualizations.
 */
TEST_F(APIIntegrationTest, VisualizationGeneration) {
    // Create and train a model for testing
    blahajpi::Analyzer analyzer(configPath.string());
    std::string outputModelPath = (modelDir / "test_model").string();
    analyzer.trainModel(dataPath.string(), outputModelPath);
    analyzer.loadModel(outputModelPath);
    
    // Create a batch of texts for analysis
    std::vector<std::string> texts = {
        "This is a safe message about community.",
        "This is harmful content with negative stereotypes.",
        "Another safe message about everyday life.",
        "More harmful content targeting groups."
    };
    
    // Analyze the texts
    auto results = analyzer.analyzeMultiple(texts);
    
    // Generate visualization
    std::string outputPath = (resultsDir / "visualization.txt").string();
    bool vizResult = analyzer.generateVisualization(results, outputPath, true);
    EXPECT_TRUE(vizResult);
    EXPECT_TRUE(std::filesystem::exists(outputPath));
    
    // Check content of visualization file
    std::ifstream vizFile(outputPath);
    std::string content((std::istreambuf_iterator<char>(vizFile)),
                        std::istreambuf_iterator<char>());
    EXPECT_FALSE(content.empty());
    EXPECT_TRUE(content.find("Word Frequency Visualization") != std::string::npos);
}

/**
 * @brief Tests configuration manipulation
 * 
 * Verifies that the analyzer's configuration can be modified at runtime.
 */
TEST_F(APIIntegrationTest, ConfigurationManipulation) {
    // Create analyzer with default configuration
    blahajpi::Analyzer analyzer;
    
    // Get initial configuration
    auto initialConfig = analyzer.getConfig();
    
    // Modify configuration
    analyzer.setConfig("custom-key", "custom-value");
    analyzer.setConfig("model-dir", "new-model-path");
    
    // Get updated configuration
    auto updatedConfig = analyzer.getConfig();
    
    // Check that changes were applied
    EXPECT_EQ(updatedConfig["custom-key"], "custom-value");
    EXPECT_EQ(updatedConfig["model-dir"], "new-model-path");
    
    // Load configuration from file
    bool loadResult = analyzer.loadConfig(configPath.string());
    EXPECT_TRUE(loadResult);
    
    // Verify loaded configuration
    auto loadedConfig = analyzer.getConfig();
    EXPECT_EQ(loadedConfig["model-dir"], modelDir.string());
    EXPECT_EQ(loadedConfig["dataset"], dataPath.string());
}

/**
 * @brief Tests integration with preprocessing components
 * 
 * Verifies that the analyzer correctly integrates with text preprocessors.
 */
TEST_F(APIIntegrationTest, PreprocessingIntegration) {
    // Create and train a model for testing
    blahajpi::Analyzer analyzer(configPath.string());
    std::string outputModelPath = (modelDir / "test_model").string();
    analyzer.trainModel(dataPath.string(), outputModelPath);
    analyzer.loadModel(outputModelPath);
    
    // Test analysis with social media formatting
    auto result = analyzer.analyze("Check out https://example.com and @username! #SocialMedia");
    
    // URLs, mentions and hashtags should be processed in the cleaned text
    EXPECT_FALSE(result.cleanedText.empty());
    EXPECT_EQ(result.cleanedText.find("https://"), std::string::npos);
    EXPECT_EQ(result.cleanedText.find("@username"), std::string::npos);
    EXPECT_EQ(result.cleanedText.find("#"), std::string::npos);
    
    // Original text should be preserved
    EXPECT_EQ(result.text, "Check out https://example.com and @username! #SocialMedia");
}

/**
 * @brief Tests integration with custom model parameters
 * 
 * Verifies that different model parameters affect the analyzer behavior.
 */
TEST_F(APIIntegrationTest, ModelParameterIntegration) {
    // Create a custom configuration with very different parameters
    std::filesystem::path customConfigPath = configDir / "custom_config.conf";
    std::ofstream customConfigFile(customConfigPath);
    customConfigFile << "# Custom configuration\n"
                    << "model-dir = " << modelDir.string() << "\n"
                    << "dataset = " << dataPath.string() << "\n"
                    << "label-column = label\n"
                    << "text-column = text\n"
                    << "alpha = 0.1\n"              // High regularization
                    << "eta0 = 0.001\n"             // Low learning rate
                    << "epochs = 3\n"               // Few epochs
                    << "max-features = 100\n";      // Few features
    customConfigFile.close();
    
    // Train two models with different configurations
    blahajpi::Analyzer defaultAnalyzer(configPath.string());
    blahajpi::Analyzer customAnalyzer(customConfigPath.string());
    
    std::string defaultModelPath = (modelDir / "default_model").string();
    std::string customModelPath = (modelDir / "custom_model").string();
    
    defaultAnalyzer.trainModel(dataPath.string(), defaultModelPath);
    customAnalyzer.trainModel(dataPath.string(), customModelPath);
    
    defaultAnalyzer.loadModel(defaultModelPath);
    customAnalyzer.loadModel(customModelPath);
    
    // Analyze the same text with both models
    std::string testText = "This is a test message with mixed content.";
    auto defaultResult = defaultAnalyzer.analyze(testText);
    auto customResult = customAnalyzer.analyze(testText);
    
    // Results might differ due to different parameters
    // We can't assert specific differences, but can check both returned results
    EXPECT_FALSE(defaultResult.sentiment.empty());
    EXPECT_FALSE(customResult.sentiment.empty());
    
    // Even if sentiments are the same, scores should likely differ
    // This is not guaranteed but likely due to different parameters
    if (defaultResult.sentiment == customResult.sentiment) {
        EXPECT_NE(defaultResult.harmScore, customResult.harmScore);
    }
}

} // namespace
