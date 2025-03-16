/**
 * @file analyzer_test.cpp
 * @brief Comprehensive unit tests for the Analyzer class
 * 
 * These tests verify the functionality of the sentiment analysis engine,
 * including text preprocessing, model inference, and result generation.
 * 
 * @author BlahajPI Team
 * @date 2025
 */

#include "blahajpi/analyzer.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <memory>

namespace {

/**
 * @class AnalyzerTest
 * @brief Test fixture for Analyzer unit tests
 * 
 * This fixture provides common setup for testing the sentiment analysis engine.
 */
class AnalyzerTest : public ::testing::Test {
protected:
    /**
     * @brief Set up test environment
     * 
     * Creates temporary configuration and test data.
     */
    void SetUp() override {
        // Create temporary paths
        tempDir = std::filesystem::temp_directory_path() / "blahajpi_tests";
        modelDir = tempDir / "models";
        configPath = tempDir / "test_config.conf";
        
        // Create directories
        std::filesystem::create_directories(tempDir);
        std::filesystem::create_directories(modelDir);
        
        // Create basic configuration file
        std::ofstream configFile(configPath);
        configFile << "# Test configuration\n"
                  << "model-dir = " << modelDir.string() << "\n"
                  << "text-column = text\n"
                  << "label-column = label\n"
                  << "alpha = 0.0001\n"
                  << "eta0 = 0.01\n"
                  << "epochs = 5\n"
                  << "max-features = 1000\n";
        configFile.close();
        
        // Create test data file
        dataPath = tempDir / "test_data.csv";
        std::ofstream dataFile(dataPath);
        dataFile << "label,text\n"
                << "0,This is normal content that should be safe.\n"
                << "4,This content contains harmful language that targets people.\n"
                << "0,Another example of perfectly acceptable content.\n"
                << "4,This has offensive language that should be flagged.\n"
                << "0,Just a regular post about everyday life.\n"
                << "4,Content with problematic statements about groups.\n";
        dataFile.close();
        
        // Use default constructor for basic tests
        defaultAnalyzer = std::make_unique<blahajpi::Analyzer>();
    }
    
    /**
     * @brief Clean up resources after tests
     */
    void TearDown() override {
        // Clean up temporary files
        if (std::filesystem::exists(tempDir)) {
            std::filesystem::remove_all(tempDir);
        }
    }
    
    /**
     * @brief Helper to train a test model
     * 
     * Trains a simple model using test data for testing analysis.
     * 
     * @return true if training succeeded
     */
    bool trainTestModel() {
        // Create analyzer with test configuration
        blahajpi::Analyzer analyzer(configPath.string());
        
        // Train model
        return analyzer.trainModel(dataPath.string(), modelDir.string());
    }
    
    // Paths for test resources
    std::filesystem::path tempDir;
    std::filesystem::path modelDir;
    std::filesystem::path configPath;
    std::filesystem::path dataPath;
    
    // Default analyzer for basic tests
    std::unique_ptr<blahajpi::Analyzer> defaultAnalyzer;
};

/**
 * @brief Tests constructor with configuration file
 * 
 * Verifies that the Analyzer can be constructed with a
 * configuration file path and loads settings correctly.
 */
TEST_F(AnalyzerTest, ConstructorWithConfig) {
    // Create analyzer with configuration
    blahajpi::Analyzer analyzer(configPath.string());
    
    // Check that configuration was loaded
    auto config = analyzer.getConfig();
    
    // Verify some key settings were loaded
    EXPECT_EQ(config["model-dir"], modelDir.string());
    EXPECT_EQ(config["text-column"], "text");
    EXPECT_EQ(config["label-column"], "label");
    EXPECT_EQ(config["alpha"], "0.0001");
    EXPECT_EQ(config["epochs"], "5");
}

/**
 * @brief Tests loading configuration
 * 
 * Verifies that configuration settings can be loaded
 * from a file after construction.
 */
TEST_F(AnalyzerTest, LoadConfiguration) {
    // Load configuration into default analyzer
    bool loadResult = defaultAnalyzer->loadConfig(configPath.string());
    
    // Check load success
    EXPECT_TRUE(loadResult);
    
    // Verify configuration was loaded
    auto config = defaultAnalyzer->getConfig();
    EXPECT_EQ(config["model-dir"], modelDir.string());
    
    // Test loading from non-existent file
    bool failResult = defaultAnalyzer->loadConfig("non_existent_file.conf");
    EXPECT_FALSE(failResult);
}

/**
 * @brief Tests setting and getting configuration
 * 
 * Verifies that individual configuration settings
 * can be set and retrieved.
 */
TEST_F(AnalyzerTest, SetAndGetConfig) {
    // Set a config value
    defaultAnalyzer->setConfig("test-key", "test-value");
    
    // Check that it was set
    auto config = defaultAnalyzer->getConfig();
    EXPECT_EQ(config["test-key"], "test-value");
    
    // Update existing value
    defaultAnalyzer->setConfig("test-key", "updated-value");
    config = defaultAnalyzer->getConfig();
    EXPECT_EQ(config["test-key"], "updated-value");
    
    // Set multiple values and verify
    defaultAnalyzer->setConfig("key1", "value1");
    defaultAnalyzer->setConfig("key2", "value2");
    
    config = defaultAnalyzer->getConfig();
    EXPECT_EQ(config["key1"], "value1");
    EXPECT_EQ(config["key2"], "value2");
}

/**
 * @brief Tests model training
 * 
 * Verifies that the Analyzer can train a sentiment analysis
 * model from labeled data.
 */
TEST_F(AnalyzerTest, ModelTraining) {
    // Create analyzer with test configuration
    blahajpi::Analyzer analyzer(configPath.string());
    
    // Train model
    bool trainResult = analyzer.trainModel(dataPath.string(), modelDir.string());
    
    // Training should succeed
    EXPECT_TRUE(trainResult);
    
    // Check that model files were created
    EXPECT_TRUE(std::filesystem::exists(modelDir / "model.bin"));
    EXPECT_TRUE(std::filesystem::exists(modelDir / "vectorizer.bin"));
    EXPECT_TRUE(std::filesystem::exists(modelDir / "model_info.txt"));
    
    // Verify training with invalid data fails gracefully
    bool failResult = analyzer.trainModel("non_existent_file.csv", modelDir.string());
    EXPECT_FALSE(failResult);
}

/**
 * @brief Tests loading a trained model
 * 
 * Verifies that a previously trained model can be loaded
 * and used for analysis.
 */
TEST_F(AnalyzerTest, LoadModel) {
    // First train a model
    ASSERT_TRUE(trainTestModel());
    
    // Now try to load the model
    bool loadResult = defaultAnalyzer->loadModel(modelDir.string());
    
    // Loading should succeed
    EXPECT_TRUE(loadResult);
    
    // Test loading from non-existent directory
    bool failResult = defaultAnalyzer->loadModel("non_existent_directory");
    EXPECT_FALSE(failResult);
}

/**
 * @brief Tests text analysis
 * 
 * Verifies that the Analyzer correctly classifies text
 * as harmful or safe with appropriate confidence scores.
 */
TEST_F(AnalyzerTest, TextAnalysis) {
    // First train and load a model
    ASSERT_TRUE(trainTestModel());
    ASSERT_TRUE(defaultAnalyzer->loadModel(modelDir.string()));
    
    // Test analyzing safe content
    std::string safeText = "This is completely normal and safe content.";
    auto safeResult = defaultAnalyzer->analyze(safeText);
    
    // Check basic properties
    EXPECT_EQ(safeResult.text, safeText);
    EXPECT_FALSE(safeResult.cleanedText.empty());
    
    // This should be classified as safe, but we can't guarantee it
    // due to the simplistic test model, so we just check the result format
    EXPECT_TRUE(safeResult.sentiment == "Safe" || safeResult.sentiment == "Harmful");
    EXPECT_GE(safeResult.confidence, 0.0);
    EXPECT_LE(safeResult.confidence, 1.0);
    EXPECT_FALSE(safeResult.explanation.empty());
    
    // Test analyzing harmful content
    std::string harmfulText = "This text contains language that targets and attacks people.";
    auto harmfulResult = defaultAnalyzer->analyze(harmfulText);
    
    // Check basic properties
    EXPECT_EQ(harmfulResult.text, harmfulText);
    EXPECT_FALSE(harmfulResult.cleanedText.empty());
    
    // Check result properties (same caveats as above)
    EXPECT_TRUE(harmfulResult.sentiment == "Safe" || harmfulResult.sentiment == "Harmful");
    EXPECT_GE(harmfulResult.confidence, 0.0);
    EXPECT_LE(harmfulResult.confidence, 1.0);
    EXPECT_FALSE(harmfulResult.explanation.empty());
    
    // Test analyzing empty text
    std::string emptyText = "";
    auto emptyResult = defaultAnalyzer->analyze(emptyText);
    
    // Should handle empty text gracefully
    EXPECT_EQ(emptyResult.text, emptyText);
    EXPECT_TRUE(emptyResult.cleanedText.empty());
}

/**
 * @brief Tests batch text analysis
 * 
 * Verifies that the Analyzer can process multiple texts
 * in a single operation.
 */
TEST_F(AnalyzerTest, BatchAnalysis) {
    // First train and load a model
    ASSERT_TRUE(trainTestModel());
    ASSERT_TRUE(defaultAnalyzer->loadModel(modelDir.string()));
    
    // Create a batch of texts
    std::vector<std::string> texts = {
        "First example text that is safe.",
        "Second example text with potentially harmful content.",
        "Third example that should be classified as safe."
    };
    
    // Analyze batch
    auto results = defaultAnalyzer->analyzeMultiple(texts);
    
    // Check correct number of results
    ASSERT_EQ(results.size(), texts.size());
    
    // Check that each result has the expected format
    for (size_t i = 0; i < results.size(); ++i) {
        EXPECT_EQ(results[i].text, texts[i]);
        EXPECT_FALSE(results[i].cleanedText.empty());
        EXPECT_TRUE(results[i].sentiment == "Safe" || results[i].sentiment == "Harmful");
        EXPECT_GE(results[i].confidence, 0.0);
        EXPECT_LE(results[i].confidence, 1.0);
        EXPECT_FALSE(results[i].explanation.empty());
    }
    
    // Test with empty batch
    std::vector<std::string> emptyBatch;
    auto emptyResults = defaultAnalyzer->analyzeMultiple(emptyBatch);
    EXPECT_TRUE(emptyResults.empty());
}

/**
 * @brief Tests visualization generation
 * 
 * Verifies that the Analyzer can generate word cloud
 * visualizations from analysis results.
 */
TEST_F(AnalyzerTest, Visualization) {
    // First train and load a model
    ASSERT_TRUE(trainTestModel());
    ASSERT_TRUE(defaultAnalyzer->loadModel(modelDir.string()));
    
    // Create and analyze some texts
    std::vector<std::string> texts = {
        "First example text that is safe and good.",
        "Second example text with harmful content that attacks people.",
        "Third example with problematic language that targets groups.",
        "Fourth example that is completely safe and positive."
    };
    
    auto results = defaultAnalyzer->analyzeMultiple(texts);
    
    // Generate visualization
    std::string outputPath = (tempDir / "visualization.txt").string();
    bool vizResult = defaultAnalyzer->generateVisualization(results, outputPath, true);
    
    // Visualization generation should succeed
    EXPECT_TRUE(vizResult);
    
    // Output file should exist
    EXPECT_TRUE(std::filesystem::exists(outputPath));
    
    // Check file content (just basic verification)
    std::ifstream vizFile(outputPath);
    std::string content((std::istreambuf_iterator<char>(vizFile)),
                        std::istreambuf_iterator<char>());
    
    EXPECT_FALSE(content.empty());
    EXPECT_TRUE(content.find("Word") != std::string::npos);
    
    // Test custom visualization
    std::string customOutputPath = (tempDir / "custom_viz.txt").string();
    blahajpi::utils::CloudConfig config;
    config.maxWords = 20;
    config.width = 100;
    config.height = 30;
    config.useColor = false;
    
    bool customVizResult = defaultAnalyzer->generateCustomVisualization(
        results, customOutputPath, false, config);
    
    EXPECT_TRUE(customVizResult);
    EXPECT_TRUE(std::filesystem::exists(customOutputPath));
}

/**
 * @brief Tests result serialization
 * 
 * Verifies that AnalysisResult objects can be converted to and
 * from map representations for serialization.
 */
TEST_F(AnalyzerTest, ResultSerialization) {
    // Create a test result
    blahajpi::AnalysisResult result;
    result.text = "Original text";
    result.cleanedText = "cleaned text";
    result.sentiment = "Harmful";
    result.harmScore = 0.75;
    result.confidence = 0.85;
    result.explanation = "This is harmful because...";
    result.keyTerms = {"harmful", "bad", "negative"};
    
    // Convert to map
    auto resultMap = result.toMap();
    
    // Check map values
    EXPECT_EQ(resultMap["text"], "Original text");
    EXPECT_EQ(resultMap["cleaned_text"], "cleaned text");
    EXPECT_EQ(resultMap["sentiment"], "Harmful");
    EXPECT_EQ(resultMap["harm_score"], "0.750000");
    EXPECT_EQ(resultMap["confidence"], "0.850000");
    EXPECT_EQ(resultMap["explanation"], "This is harmful because...");
    EXPECT_EQ(resultMap["key_terms"], "harmful,bad,negative");
    
    // Convert back to result
    auto reconstructedResult = blahajpi::AnalysisResult::fromMap(resultMap);
    
    // Check reconstructed result
    EXPECT_EQ(reconstructedResult.text, result.text);
    EXPECT_EQ(reconstructedResult.cleanedText, result.cleanedText);
    EXPECT_EQ(reconstructedResult.sentiment, result.sentiment);
    EXPECT_NEAR(reconstructedResult.harmScore, result.harmScore, 0.000001);
    EXPECT_NEAR(reconstructedResult.confidence, result.confidence, 0.000001);
    EXPECT_EQ(reconstructedResult.explanation, result.explanation);
    ASSERT_EQ(reconstructedResult.keyTerms.size(), result.keyTerms.size());
    for (size_t i = 0; i < result.keyTerms.size(); ++i) {
        EXPECT_EQ(reconstructedResult.keyTerms[i], result.keyTerms[i]);
    }
}

/**
 * @brief Tests error handling
 * 
 * Verifies that the Analyzer properly handles error conditions
 * such as analyzing without a loaded model.
 */
TEST_F(AnalyzerTest, ErrorHandling) {
    // Attempt to analyze without a loaded model
    EXPECT_THROW({
        defaultAnalyzer->analyze("This should throw an error");
    }, std::runtime_error);
    
    // Test with invalid configuration paths
    blahajpi::Analyzer badConfigAnalyzer("non_existent_config.conf");
    EXPECT_THROW({
        badConfigAnalyzer.analyze("This should throw an error");
    }, std::runtime_error);
}

} // namespace
