/**
* @file vectorizer_test.cpp
* @brief Comprehensive unit tests for the TfidfVectorizer class
* 
* These tests verify the functionality of the TF-IDF vectorization component,
* including feature extraction, vocabulary building, and document transformation.
* 
* @author BlahajPI Team
* @date 2024
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
* @class TfidfVectorizerTest
* @brief Test fixture for TfidfVectorizer unit tests
* 
* This fixture provides document samples and expected outputs
* for testing the vectorization process.
*/
class TfidfVectorizerTest : public ::testing::Test {
protected:
   /**
    * @brief Set up test documents and expected results
    */
   void SetUp() override {
       // Simple document collection
       simpleDocs = {
           "this is a sample",
           "this is another example",
           "this is a third sample document"
       };
       
       // More complex document collection
       complexDocs = {
           "The quick brown fox jumps over the lazy dog.",
           "A fast auburn fox leaps above the sleepy hound.",
           "The lazy dog sleeps all day long.",
           "Quick foxes jump over lazy dogs repeatedly."
       };
       
       // Documents with special tokens
       specialDocs = {
           "email@example.com and https://example.com are special tokens",
           "#hashtag and @mention should be handled properly",
           "numbers like 123 and punctuation !@#$% need special care"
       };
       
       // Empty and edge case documents
       edgeDocs = {
           "",                                                  // Empty document
           "a",                                                 // Single character
           "repeated repeated repeated repeated repeated",      // Repetition
           "completely unique content with rare words",         // Unique content
           "a b c d e f g h i j k l m n o p q r s t u v w x y z" // All letters
       };
       
       // Temporary directory for file operations
       tempDir = std::filesystem::temp_directory_path() / "blahajpi_tests";
       std::filesystem::create_directories(tempDir);
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
    * @brief Create a temporary vectorizer file for testing
    * 
    * @param vectorizer The vectorizer to save
    * @return Path to the temporary file
    */
   std::string createTempVectorizerFile(const blahajpi::preprocessing::TfidfVectorizer& vectorizer) {
       std::string filePath = (tempDir / "vectorizer_test.bin").string();
       vectorizer.save(filePath);
       return filePath;
   }
   
   /**
    * @brief Verify that two feature matrices are approximately equal
    * 
    * @param mat1 First feature matrix
    * @param mat2 Second feature matrix
    * @param epsilon Maximum allowed difference between elements
    * @return true if matrices are approximately equal
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
   
   // Test document collections
   std::vector<std::string> simpleDocs;
   std::vector<std::string> complexDocs;
   std::vector<std::string> specialDocs;
   std::vector<std::string> edgeDocs;
   
   // Temporary directory for file operations
   std::filesystem::path tempDir;
};

/**
* @brief Tests the tokenization functionality
* 
* Verifies that documents are properly tokenized into words
* and n-grams based on specified parameters.
*/
TEST_F(TfidfVectorizerTest, TokenizesDocumentsCorrectly) {
   // Create vectorizer with unigrams only
   blahajpi::preprocessing::TfidfVectorizer unigramVectorizer(true, 0.5, 10000, 1, 1);
   
   // Test tokenization of a simple document
   auto tokens = unigramVectorizer.tokenize("this is a test");
   
   // Expecting 4 tokens: "this", "is", "a", "test"
   ASSERT_EQ(tokens.size(), 4);
   EXPECT_EQ(tokens[0], "this");
   EXPECT_EQ(tokens[1], "is");
   EXPECT_EQ(tokens[2], "a");
   EXPECT_EQ(tokens[3], "test");
   
   // Test with bigrams
   blahajpi::preprocessing::TfidfVectorizer bigramVectorizer(true, 0.5, 10000, 1, 2);
   
   tokens = bigramVectorizer.tokenize("this is a test");
   
   // Expecting 7 tokens: 4 unigrams + 3 bigrams
   ASSERT_EQ(tokens.size(), 7);
   // Check some bigrams
   EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), "this_is") != tokens.end());
   EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), "is_a") != tokens.end());
   EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), "a_test") != tokens.end());
   
   // Test with trigrams
   blahajpi::preprocessing::TfidfVectorizer trigramVectorizer(true, 0.5, 10000, 1, 3);
   
   tokens = trigramVectorizer.tokenize("this is a test document");
   
   // Expecting unigrams + bigrams + trigrams
   // Check some trigrams
   EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), "this_is_a") != tokens.end());
   EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), "is_a_test") != tokens.end());
   EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), "a_test_document") != tokens.end());
   
   // Test with n-gram range (2-3) - only bigrams and trigrams, no unigrams
   blahajpi::preprocessing::TfidfVectorizer rangeVectorizer(true, 0.5, 10000, 2, 3);
   
   tokens = rangeVectorizer.tokenize("this is a test document");
   
   // Should not contain unigrams
   EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), "this") == tokens.end());
   EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), "is") == tokens.end());
   
   // Should contain bigrams and trigrams
   EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), "this_is") != tokens.end());
   EXPECT_TRUE(std::find(tokens.begin(), tokens.end(), "this_is_a") != tokens.end());
}

/**
* @brief Tests the vocabulary building functionality
* 
* Verifies that the vectorizer correctly extracts vocabulary
* from documents and respects maximum feature limitations.
*/
TEST_F(TfidfVectorizerTest, BuildsVocabularyCorrectly) {
   // Create a vectorizer with limited features
   blahajpi::preprocessing::TfidfVectorizer vectorizer(true, 0.5, 10, 1, 1);
   
   // Fit vectorizer to complexDocs
   vectorizer.fit(complexDocs);
   
   // Get the vocabulary
   auto vocabulary = vectorizer.getVocabulary();
   
   // Check if vocabulary size is limited to 10
   EXPECT_LE(vocabulary.size(), 10);
   
   // Check if common words are in vocabulary
   EXPECT_TRUE(vocabulary.find("the") != vocabulary.end());
   EXPECT_TRUE(vocabulary.find("fox") != vocabulary.end());
   EXPECT_TRUE(vocabulary.find("lazy") != vocabulary.end());
   EXPECT_TRUE(vocabulary.find("dog") != vocabulary.end());
   
   // Test with a larger vocabulary limit
   blahajpi::preprocessing::TfidfVectorizer largeVectorizer(true, 0.5, 100, 1, 1);
   largeVectorizer.fit(complexDocs);
   
   auto largeVocabulary = largeVectorizer.getVocabulary();
   
   // Should include more words
   EXPECT_GT(largeVocabulary.size(), vocabulary.size());
   
   // Test max_df parameter
   blahajpi::preprocessing::TfidfVectorizer lowDfVectorizer(true, 0.25, 100, 1, 1);
   lowDfVectorizer.fit(complexDocs);
   
   auto lowDfVocabulary = lowDfVectorizer.getVocabulary();
   
   // Words that appear in more than 25% of documents should not be in vocabulary
   // "the" appears in 3/4 documents, so it should be filtered out
   EXPECT_TRUE(lowDfVocabulary.find("the") == lowDfVocabulary.end());
}

/**
* @brief Tests the document transformation functionality
* 
* Verifies that documents are correctly transformed into
* TF-IDF feature vectors with expected properties.
*/
TEST_F(TfidfVectorizerTest, TransformsDocumentsCorrectly) {
   // Create a vectorizer with unigrams only
   blahajpi::preprocessing::TfidfVectorizer vectorizer(true, 0.5, 100, 1, 1);
   
   // Fit and transform simpleDocs
   auto features = vectorizer.fitTransform(simpleDocs);
   
   // Check dimensions: 3 documents x N features
   ASSERT_EQ(features.size(), 3);
   
   // All feature vectors should have the same dimension
   EXPECT_EQ(features[0].size(), features[1].size());
   EXPECT_EQ(features[1].size(), features[2].size());
   
   // For comparison, fit and transform separately
   blahajpi::preprocessing::TfidfVectorizer twoStepVectorizer(true, 0.5, 100, 1, 1);
   twoStepVectorizer.fit(simpleDocs);
   auto twoStepFeatures = twoStepVectorizer.transform(simpleDocs);
   
   // Results should be the same
   EXPECT_TRUE(areFeaturesEqual(features, twoStepFeatures));
   
   // Test normalization
   for (const auto& docFeatures : features) {
       // Calculate L2 norm
       double squaredSum = 0.0;
       for (double val : docFeatures) {
           squaredSum += val * val;
       }
       double norm = std::sqrt(squaredSum);
       
       // Verify vector is normalized (L2 norm should be 1.0 or very close)
       EXPECT_NEAR(norm, 1.0, 1e-5);
   }
   
   // Verify that terms that occur in only one document have higher TF-IDF
   // Get indices for some unique terms
   auto vocabulary = vectorizer.getVocabulary();
   
   int thirdIndex = -1;
   int anotherIndex = -1;
   
   for (const auto& [term, idx] : vocabulary) {
       if (term == "third") {
           thirdIndex = idx;
       } else if (term == "another") {
           anotherIndex = idx;
       }
   }
   
   // If these terms are in vocabulary, check their scores
   if (thirdIndex >= 0 && anotherIndex >= 0) {
       // "third" appears only in doc 2
       EXPECT_EQ(features[0][thirdIndex], 0.0);
       EXPECT_EQ(features[1][thirdIndex], 0.0);
       EXPECT_GT(features[2][thirdIndex], 0.0);
       
       // "another" appears only in doc 1
       EXPECT_EQ(features[0][anotherIndex], 0.0);
       EXPECT_GT(features[1][anotherIndex], 0.0);
       EXPECT_EQ(features[2][anotherIndex], 0.0);
   }
}

/**
* @brief Tests the behavior with empty documents
* 
* Verifies that the vectorizer correctly handles empty
* documents and other edge cases.
*/
TEST_F(TfidfVectorizerTest, HandlesEmptyDocuments) {
   // Create vectorizer
   blahajpi::preprocessing::TfidfVectorizer vectorizer(true, 0.5, 100, 1, 1);
   
   // Fit with documents including an empty one
   vectorizer.fit(edgeDocs);
   
   // Transform the documents
   auto features = vectorizer.transform(edgeDocs);
   
   // Number of feature vectors should match number of documents
   ASSERT_EQ(features.size(), edgeDocs.size());
   
   // Empty document should have all zeros
   for (double val : features[0]) {
       EXPECT_DOUBLE_EQ(val, 0.0);
   }
   
   // Single character document should have sparse representation
   int nonZeroCount = 0;
   for (double val : features[1]) {
       if (val > 0.0) {
           nonZeroCount++;
       }
   }
   EXPECT_EQ(nonZeroCount, 1);
   
   // Repeated word document should have very sparse representation
   nonZeroCount = 0;
   for (double val : features[2]) {
       if (val > 0.0) {
           nonZeroCount++;
       }
   }
   EXPECT_EQ(nonZeroCount, 1);
   
   // Test transform with completely empty input
   std::vector<std::string> emptyDocs = {""};
   auto emptyFeatures = vectorizer.transform(emptyDocs);
   
   ASSERT_EQ(emptyFeatures.size(), 1);
   for (double val : emptyFeatures[0]) {
       EXPECT_DOUBLE_EQ(val, 0.0);
   }
}

/**
* @brief Tests that TF-IDF values are calculated correctly
* 
* Verifies the numerical properties of TF-IDF calculations
* including sublinear scaling and IDF normalization.
*/
TEST_F(TfidfVectorizerTest, CalculatesTfIdfValuesCorrectly) {
   // Small document set with known term frequencies
   std::vector<std::string> testDocs = {
       "apple apple apple banana",         // 3 apples, 1 banana
       "banana banana banana orange",      // 3 bananas, 1 orange
       "apple orange orange orange"        // 1 apple, 3 oranges
   };
   
   // Create vectorizer with sublinear TF scaling
   blahajpi::preprocessing::TfidfVectorizer sublinearVectorizer(true, 1.0, 100, 1, 1);
   auto sublinearFeatures = sublinearVectorizer.fitTransform(testDocs);
   
   // Create vectorizer without sublinear TF scaling
   blahajpi::preprocessing::TfidfVectorizer regularVectorizer(false, 1.0, 100, 1, 1);
   auto regularFeatures = regularVectorizer.fitTransform(testDocs);
   
   // Get vocabulary indices
   auto vocabulary = sublinearVectorizer.getVocabulary();
   int appleIdx = vocabulary["apple"];
   int bananaIdx = vocabulary["banana"];
   int orangeIdx = vocabulary["orange"];
   
   // With sublinear scaling, TF = 1 + log(tf)
   // So 3 occurrences should have lower relative weight compared to regular TF
   double appleRatio = sublinearFeatures[0][appleIdx] / regularFeatures[0][appleIdx];
   
   // The ratio should be approximately (1 + log(3))/3 = 0.7324...
   EXPECT_NEAR(appleRatio, (1.0 + std::log(3.0))/3.0, 0.01);
   
   // Check document frequency calculations
   auto docFreqs = sublinearVectorizer.getDocumentFrequencies();
   
   // Apple appears in 2 documents
   EXPECT_EQ(docFreqs[appleIdx], 2);
   
   // Banana appears in 2 documents
   EXPECT_EQ(docFreqs[bananaIdx], 2);
   
   // Orange appears in 2 documents
   EXPECT_EQ(docFreqs[orangeIdx], 2);
}

/**
* @brief Tests save and load functionality
* 
* Verifies that vectorizers can be correctly serialized and
* deserialized with all their state preserved.
*/
TEST_F(TfidfVectorizerTest, SavesAndLoadsCorrectly) {
   // Create and fit a vectorizer
   blahajpi::preprocessing::TfidfVectorizer vectorizer(true, 0.5, 100, 1, 2);
   vectorizer.fit(complexDocs);
   
   // Transform documents to get original features
   auto originalFeatures = vectorizer.transform(complexDocs);
   
   // Save the vectorizer to a temporary file
   std::string filePath = createTempVectorizerFile(vectorizer);
   
   // Create a new vectorizer and load from file
   blahajpi::preprocessing::TfidfVectorizer loadedVectorizer;
   bool loadSuccess = loadedVectorizer.load(filePath);
   
   // Check loading was successful
   EXPECT_TRUE(loadSuccess);
   
   // Transform the same documents with loaded vectorizer
   auto loadedFeatures = loadedVectorizer.transform(complexDocs);
   
   // Features should be identical
   EXPECT_TRUE(areFeaturesEqual(originalFeatures, loadedFeatures));
   
   // Vocabularies should match
   auto originalVocab = vectorizer.getVocabulary();
   auto loadedVocab = loadedVectorizer.getVocabulary();
   
   EXPECT_EQ(originalVocab.size(), loadedVocab.size());
   for (const auto& [term, idx] : originalVocab) {
       EXPECT_TRUE(loadedVocab.find(term) != loadedVocab.end());
       EXPECT_EQ(loadedVocab[term], idx);
   }
   
   // Document frequencies should match
   auto originalDocFreqs = vectorizer.getDocumentFrequencies();
   auto loadedDocFreqs = loadedVectorizer.getDocumentFrequencies();
   
   EXPECT_EQ(originalDocFreqs.size(), loadedDocFreqs.size());
   for (size_t i = 0; i < originalDocFreqs.size(); ++i) {
       EXPECT_EQ(originalDocFreqs[i], loadedDocFreqs[i]);
   }
   
   // Test loading from non-existent file
   blahajpi::preprocessing::TfidfVectorizer failVectorizer;
   bool loadFail = failVectorizer.load("non_existent_file.bin");
   EXPECT_FALSE(loadFail);
}

/**
* @brief Tests error handling and robustness
* 
* Verifies that the vectorizer gracefully handles error conditions
* such as using transform() before fit() and other edge cases.
*/
TEST_F(TfidfVectorizerTest, HandlesErrorConditionsGracefully) {
   // Using transform before fit should throw an exception
   blahajpi::preprocessing::TfidfVectorizer vectorizer;
   
   EXPECT_THROW({
       vectorizer.transform(simpleDocs);
   }, std::runtime_error);
   
   // Using invalid parameters should result in corrections
   blahajpi::preprocessing::TfidfVectorizer invalidParamsVectorizer(
       true,     // sublinearTf
       1.5,      // maxDf > 1.0 should be capped to 1.0
       10000,    // maxFeatures
       0,        // minNgram = 0 should be corrected to 1
       0         // maxNgram = 0 should be corrected to 1
   );
   
   // Should not throw when fitting
   EXPECT_NO_THROW({
       invalidParamsVectorizer.fit(simpleDocs);
   });
   
   // Check if parameter correction happened (indirectly through tokenization)
   auto tokens = invalidParamsVectorizer.tokenize("test correction");
   
   // Should have at least unigrams (if minNgram was corrected to 1)
   EXPECT_EQ(tokens.size(), 2);
   
   // Test with inconsistent document sets
   vectorizer.fit(simpleDocs);
   
   // Transform a different document set
   auto features = vectorizer.transform(complexDocs);
   
   // Should still produce feature vectors of correct dimensions
   EXPECT_EQ(features.size(), complexDocs.size());
   EXPECT_EQ(features[0].size(), vectorizer.getVocabulary().size());
   
   // Words not in training vocabulary should have zero TF-IDF
   // "quick" and "brown" are in complexDocs but not in simpleDocs
   for (size_t i = 0; i < features[0].size(); ++i) {
       // These features should be sparse
       // Can't test specific words as we don't know their indices
       // But overall vector should be sparse with mostly zeros
       int nonZeroCount = 0;
       for (double val : features[0]) {
           if (val > 0.0) {
               nonZeroCount++;
           }
       }
       // Expect few non-zero values
       EXPECT_LT(nonZeroCount, features[0].size() / 2);
   }
}

} // namespace
