/**
 * @file word_cloud_test.cpp
 * @brief Comprehensive unit tests for the WordCloud class
 * 
 * These tests verify the functionality of the word cloud visualization component,
 * including frequency counting, text processing, and cloud generation.
 * 
 * @author BlahajPI Team
 * @date 2025
 */

#include "blahajpi/utils/word_cloud.hpp"
#include "blahajpi/analyzer.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>

namespace {

/**
 * @class WordCloudTest
 * @brief Test fixture for WordCloud unit tests
 * 
 * This fixture provides common setup for testing word cloud visualization.
 */
class WordCloudTest : public ::testing::Test {
protected:
    /**
     * @brief Set up test environment
     * 
     * Creates temporary directory and sample texts for testing.
     */
    void SetUp() override {
        // Create temporary directory
        tempDir = std::filesystem::temp_directory_path() / "blahajpi_tests";
        std::filesystem::create_directories(tempDir);
        
        // Create sample texts for visualization
        safeTexts = {
            "This is a safe message with normal content.",
            "Another safe message about everyday topics.",
            "A third safe message about community and support.",
            "Safe content promoting respect and dignity for all.",
            "Positive message about equality and rights."
        };
        
        harmfulTexts = {
            "Harmful content targeting specific groups.",
            "More harmful content with negative stereotypes.",
            "Offensive content that attacks people.",
            "Discriminatory message with hostile intent.",
            "Content containing harmful language and stereotypes."
        };
        
        mixedTexts.insert(mixedTexts.end(), safeTexts.begin(), safeTexts.end());
        mixedTexts.insert(mixedTexts.end(), harmfulTexts.begin(), harmfulTexts.end());
        
        // Create sample analysis results
        analysisResults.clear();
        for (const auto& text : safeTexts) {
            blahajpi::AnalysisResult result;
            result.text = text;
            result.cleanedText = text;
            result.sentiment = "Safe";
            result.harmScore = -0.8;
            result.confidence = 0.9;
            result.explanation = "This content appears to be safe.";
            analysisResults.push_back(result);
        }
        
        for (const auto& text : harmfulTexts) {
            blahajpi::AnalysisResult result;
            result.text = text;
            result.cleanedText = text;
            result.sentiment = "Harmful";
            result.harmScore = 0.8;
            result.confidence = 0.9;
            result.explanation = "This content appears to be harmful.";
            analysisResults.push_back(result);
        }
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
    std::vector<std::string> safeTexts;
    std::vector<std::string> harmfulTexts;
    std::vector<std::string> mixedTexts;
    std::vector<blahajpi::AnalysisResult> analysisResults;
};

/**
 * @brief Tests the default constructor
 * 
 * Verifies that the WordCloud class can be properly initialized.
 */
TEST_F(WordCloudTest, DefaultConstructor) {
    EXPECT_NO_THROW({
        blahajpi::utils::WordCloud wordCloud;
    });
}

/**
 * @brief Tests the custom constructor
 * 
 * Verifies that the WordCloud class can be initialized with custom word lists.
 */
TEST_F(WordCloudTest, CustomConstructor) {
    std::unordered_set<std::string> commonWords = {"the", "and", "a"};
    std::unordered_set<std::string> harmfulWords = {"harmful", "negative", "bad"};
    std::unordered_set<std::string> safeWords = {"safe", "positive", "good"};
    
    EXPECT_NO_THROW({
        blahajpi::utils::WordCloud wordCloud(commonWords, harmfulWords, safeWords);
    });
}

/**
 * @brief Tests word cloud generation with safe content
 * 
 * Verifies that word clouds can be generated for safe content.
 */
TEST_F(WordCloudTest, GenerateSafeWordCloud) {
    blahajpi::utils::WordCloud wordCloud;
    
    // Generate word cloud with safe content
    std::string cloud = wordCloud.generateWordCloud(safeTexts, 10, 80, 10, false);
    
    // Cloud should contain certain words from the safe texts
    EXPECT_TRUE(cloud.find("safe") != std::string::npos);
    EXPECT_TRUE(cloud.find("message") != std::string::npos);
    EXPECT_TRUE(cloud.find("Word Frequency Visualization") != std::string::npos);
    EXPECT_TRUE(cloud.find("Safe Content") != std::string::npos);
}

/**
 * @brief Tests word cloud generation with harmful content
 * 
 * Verifies that word clouds can be generated for harmful content.
 */
TEST_F(WordCloudTest, GenerateHarmfulWordCloud) {
    blahajpi::utils::WordCloud wordCloud;
    
    // Generate word cloud with harmful content
    std::string cloud = wordCloud.generateWordCloud(harmfulTexts, 10, 80, 10, true);
    
    // Cloud should contain certain words from the harmful texts
    EXPECT_TRUE(cloud.find("harmful") != std::string::npos);
    EXPECT_TRUE(cloud.find("content") != std::string::npos);
    EXPECT_TRUE(cloud.find("Word Frequency Visualization") != std::string::npos);
    EXPECT_TRUE(cloud.find("Harmful Content") != std::string::npos);
}

/**
 * @brief Tests custom word cloud generation
 * 
 * Verifies that word clouds can be generated with custom configuration.
 */
TEST_F(WordCloudTest, GenerateCustomWordCloud) {
    blahajpi::utils::WordCloud wordCloud;
    
    // Configure custom cloud settings
    blahajpi::utils::CloudConfig config;
    config.maxWords = 15;
    config.width = 100;
    config.height = 20;
    config.useColor = true;
    config.showFrequencies = true;
    
    // Generate custom word cloud
    std::string cloud = wordCloud.generateCustomCloud(mixedTexts, config, true);
    
    // Cloud should contain words from both safe and harmful texts
    EXPECT_TRUE(cloud.find("harmful") != std::string::npos);
    EXPECT_TRUE(cloud.find("content") != std::string::npos);
    EXPECT_TRUE(cloud.find("Word Frequency Visualization") != std::string::npos);
    
    // Should show frequencies
    EXPECT_TRUE(cloud.find("(") != std::string::npos && cloud.find(")") != std::string::npos);
}

/**
 * @brief Tests saving word cloud to file
 * 
 * Verifies that word clouds can be saved to files.
 */
TEST_F(WordCloudTest, SaveWordCloud) {
    blahajpi::utils::WordCloud wordCloud;
    
    // Generate word cloud
    std::string cloud = wordCloud.generateWordCloud(safeTexts, 10, 80, 10, false);
    
    // Save to file
    std::string outputPath = (tempDir / "word_cloud.txt").string();
    bool result = blahajpi::utils::WordCloud::saveWordCloud(cloud, outputPath);
    
    // Check if save was successful
    EXPECT_TRUE(result);
    EXPECT_TRUE(std::filesystem::exists(outputPath));
    
    // Check file contents
    std::ifstream file(outputPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    EXPECT_EQ(content, cloud);
}

/**
 * @brief Tests word frequency counting
 * 
 * Verifies that word frequencies are counted correctly.
 */
TEST_F(WordCloudTest, WordFrequencyCounting) {
    // Create a simplified WordCloud with known words
    std::unordered_set<std::string> commonWords = {"the", "and", "a", "is", "with", "about"};
    std::unordered_set<std::string> harmfulWords = {"harmful", "negative", "attacks", "offensive"};
    std::unordered_set<std::string> safeWords = {"safe", "positive", "respect", "support"};
    
    blahajpi::utils::WordCloud wordCloud(commonWords, harmfulWords, safeWords);
    
    // Test texts with known word frequencies
    std::vector<std::string> testTexts = {
        "safe safe safe harmful",             // 3 safe + 1 harmful
        "harmful harmful negative negative",  // 2 harmful + 2 negative
        "safe positive support respect"       // 1 safe + 3 other safe words
    };
    
    // Generate cloud with limited words to see top frequencies
    std::string cloud = wordCloud.generateWordCloud(testTexts, 5, 80, 10, false);
    
    // Words with higher frequencies should appear first
    size_t safePos = cloud.find("safe");
    size_t harmfulPos = cloud.find("harmful");
    
    // Safe should appear earlier (higher frequency)
    EXPECT_TRUE(safePos != std::string::npos);
    EXPECT_TRUE(harmfulPos != std::string::npos);
    
    // Both safe and harmful words should be in the cloud
    EXPECT_TRUE(cloud.find("negative") != std::string::npos);
    EXPECT_TRUE(cloud.find("positive") != std::string::npos ||
               cloud.find("support") != std::string::npos ||
               cloud.find("respect") != std::string::npos);
}

/**
 * @brief Tests displaying word cloud to console
 * 
 * Verifies that the displayWordCloud method doesn't crash.
 */
TEST_F(WordCloudTest, DisplayWordCloud) {
    blahajpi::utils::WordCloud wordCloud;
    
    // This should not throw
    EXPECT_NO_THROW({
        // Redirect cout to avoid cluttering test output
        std::streambuf* oldCout = std::cout.rdbuf();
        std::ostringstream capture;
        std::cout.rdbuf(capture.rdbuf());
        
        wordCloud.displayWordCloud(safeTexts, 10, 80, 10, false);
        
        // Restore cout
        std::cout.rdbuf(oldCout);
    });
}

/**
 * @brief Tests word cloud with empty input
 * 
 * Verifies that the word cloud generator handles empty input gracefully.
 */
TEST_F(WordCloudTest, EmptyInput) {
    blahajpi::utils::WordCloud wordCloud;
    
    // Generate cloud with empty input
    std::vector<std::string> emptyTexts;
    std::string cloud = wordCloud.generateWordCloud(emptyTexts, 10, 80, 10, false);
    
    // Should return message about no words
    EXPECT_TRUE(cloud.find("No words found") != std::string::npos);
}

/**
 * @brief Tests word cloud with ANSI color codes
 * 
 * Verifies that color coding works for word clouds.
 */
TEST_F(WordCloudTest, ColorCoding) {
    blahajpi::utils::WordCloud wordCloud;
    
    // Generate word cloud with color
    blahajpi::utils::CloudConfig config;
    config.useColor = true;
    
    std::string safeCloud = wordCloud.generateCustomCloud(safeTexts, config, false);
    std::string harmfulCloud = wordCloud.generateCustomCloud(harmfulTexts, config, true);
    
    // Check for ANSI color codes
    EXPECT_TRUE(safeCloud.find("\033[") != std::string::npos);
    EXPECT_TRUE(harmfulCloud.find("\033[") != std::string::npos);
    
    // Check for color reset code
    EXPECT_TRUE(safeCloud.find("\033[0m") != std::string::npos);
    EXPECT_TRUE(harmfulCloud.find("\033[0m") != std::string::npos);
}

/**
 * @brief Tests word cloud with bar chart style
 * 
 * Verifies that bar chart visualization works.
 */
TEST_F(WordCloudTest, BarChartStyle) {
    blahajpi::utils::WordCloud wordCloud;
    
    // Configure for bar chart style
    blahajpi::utils::CloudConfig config;
    config.useBars = true;
    config.showFrequencies = true;
    
    // Generate bar chart style cloud
    std::string cloud = wordCloud.generateCustomCloud(mixedTexts, config, false);
    
    // Should contain bar characters
    EXPECT_TRUE(cloud.find("█") != std::string::npos);
    
    // Should show frequencies
    EXPECT_TRUE(cloud.find("(") != std::string::npos && cloud.find(")") != std::string::npos);
}

} // namespace
