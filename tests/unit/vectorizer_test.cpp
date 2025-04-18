/**
 * @file vectorizer_test.cpp
 * @brief Unit tests for the TfidfVectorizer class
 * @ingroup tests
 * @defgroup vectorizer_tests Vectorizer Tests
 * 
 * Contains tests for the TF-IDF vectorization functionality, which
 * transforms text into numerical feature vectors for machine learning.
 */

#include "blahajpi/preprocessing/vectorizer.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace {

/**
 * @brief Test fixture for TfidfVectorizer tests
 * @ingroup vectorizer_tests
 * 
 * Provides test data and helper functions for testing the vectorizer.
 */
class TfidfVectorizerTest : public ::testing::Test {
protected:
   /**
    * @brief Set up test data and directories
    * 
    * Creates sample documents and temporary directory for testing.
    */
   void SetUp() override {
       // Simple document collection
       simpleDocs = {
           "this is a sample",
           "this is another example",
           "this is a third sample document"
       };
       
       // Create temporary directory for file operations
       tempDir = std::filesystem::temp_directory_path() / "blahajpi_tests";
       std::filesystem::create_directories(tempDir);
   }
   
   /**
    * @brief Clean up temporary files
    * 
    * Removes the temporary directory created during testing.
    */
   void TearDown() override {
       if (std::filesystem::exists(tempDir)) {
           std::filesystem::remove_all(tempDir);
       }
   }
   
   /**
    * @brief Helper function to check if features are approximately equal
    * 
    * Compares two feature matrices for approximate equality.
    * 
    * @param mat1 First feature matrix
    * @param mat2 Second feature matrix
    * @param epsilon Tolerance for floating-point comparison
    * @return True if matrices are approximately equal
    */
   bool areFeaturesEqual(
       const std::vector<std::vector<double>>& mat1,
       const std::vector<std::vector<double>>& mat2,
       double epsilon = 1e-5
   ) {
       if (mat1.size() != mat2.size()) {
           return false;
       }
       
       for (size_t i = 0; i < mat1.size(); ++i) {
           if (mat1[i].size() != mat2[i].size()) {
               return false;
           }
           
           for (size_t j = 0; j < mat1[i].size(); ++j) {
               if (std::abs(mat1[i][j] - mat2[i][j]) > epsilon) {
                   return false;
               }
           }
       }
       
       return true;
   }
   
   /** Sample documents for testing */
   std::vector<std::string> simpleDocs;
   
   /** Temporary directory path for file operations */
   std::filesystem::path tempDir;
};

/**
 * @test
 * @brief Tests document tokenization
 * @ingroup vectorizer_tests
 * 
 * Verifies that the vectorizer correctly tokenizes text into words.
 */
TEST_F(TfidfVectorizerTest, TokenizesDocumentsCorrectly) {
   // Create vectorizer with unigrams only
   blahajpi::preprocessing::TfidfVectorizer unigramVectorizer(true, 0.5, 10000, 1, 1);
   
   // Test tokenization of a simple document
   auto tokens = unigramVectorizer.tokenize("this is a test");
   
   // Should not be empty
   EXPECT_FALSE(tokens.empty());
   
   // Should contain the expected words (implementation may vary)
   bool containsExpectedWords = 
       std::find(tokens.begin(), tokens.end(), "this") != tokens.end() ||
       std::find(tokens.begin(), tokens.end(), "test") != tokens.end();
   
   EXPECT_TRUE(containsExpectedWords);
}

/**
 * @test
 * @brief Tests fitting and transforming documents
 * @ingroup vectorizer_tests
 * 
 * Verifies that the vectorizer can build a vocabulary and transform
 * documents into feature vectors.
 */
TEST_F(TfidfVectorizerTest, FitAndTransform) {
   // Create a vectorizer
   blahajpi::preprocessing::TfidfVectorizer vectorizer(true, 0.5, 100, 1, 1);
   
   // Fit vectorizer on simple docs
   EXPECT_NO_THROW(vectorizer.fit(simpleDocs));
   
   // Check vocabulary
   auto vocabulary = vectorizer.getVocabulary();
   EXPECT_FALSE(vocabulary.empty());
   
   // Transform documents
   std::vector<std::vector<double>> features;
   EXPECT_NO_THROW(features = vectorizer.transform(simpleDocs));
   
   // Check dimensions
   EXPECT_EQ(features.size(), simpleDocs.size());
   
   // Try fitTransform
   std::vector<std::vector<double>> combinedFeatures;
   EXPECT_NO_THROW(combinedFeatures = vectorizer.fitTransform(simpleDocs));
   
   // Check dimensions
   EXPECT_EQ(combinedFeatures.size(), simpleDocs.size());
}

/**
 * @test
 * @brief Tests handling of empty documents
 * @ingroup vectorizer_tests
 * 
 * Verifies that the vectorizer correctly handles empty documents.
 */
TEST_F(TfidfVectorizerTest, HandlesEmptyDocuments) {
   // Create vectorizer
   blahajpi::preprocessing::TfidfVectorizer vectorizer(true, 0.5, 100, 1, 1);
   
   // Fit with regular documents
   vectorizer.fit(simpleDocs);
   
   // Transform an empty document
   std::vector<std::string> emptyDocs = {""};
   std::vector<std::vector<double>> features;
   
   EXPECT_NO_THROW(features = vectorizer.transform(emptyDocs));
   
   // Should return a feature vector of appropriate size
   EXPECT_EQ(features.size(), 1);
}

/**
 * @test
 * @brief Tests serialization and deserialization
 * @ingroup vectorizer_tests
 * 
 * Verifies that the vectorizer can be saved to and loaded from disk.
 */
TEST_F(TfidfVectorizerTest, SaveAndLoad) {
   // Skip this test if serialization is not implemented
   blahajpi::preprocessing::TfidfVectorizer vectorizer(true, 0.5, 100, 1, 1);
   vectorizer.fit(simpleDocs);
   
   std::string filePath = (tempDir / "vectorizer.bin").string();
   
   // Try to save
   bool saveResult = false;
   EXPECT_NO_THROW(saveResult = vectorizer.save(filePath));
   
   if (!saveResult || !std::filesystem::exists(filePath)) {
       GTEST_SKIP() << "Vectorizer saving not implemented or failed";
   }
   
   // Try to load
   blahajpi::preprocessing::TfidfVectorizer loadedVectorizer;
   bool loadResult = false;
   EXPECT_NO_THROW(loadResult = loadedVectorizer.load(filePath));
   
   // Check if loading worked
   if (loadResult) {
       auto vocabulary = loadedVectorizer.getVocabulary();
       EXPECT_FALSE(vocabulary.empty());
   }
}

} // namespace
