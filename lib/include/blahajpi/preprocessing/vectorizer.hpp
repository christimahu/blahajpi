/**
 * @file vectorizer.hpp
 * @brief Text vectorization using TF-IDF for feature extraction
 * 
 * This file provides functionality to convert text data into numerical
 * feature vectors using the Term Frequency-Inverse Document Frequency
 * (TF-IDF) approach, which helps capture the importance of words in documents.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string_view>

namespace blahajpi {
namespace preprocessing {

/**
 * @brief Converts text into numerical feature vectors using TF-IDF
 * 
 * TF-IDF (Term Frequency-Inverse Document Frequency) helps measure the
 * importance of words in a document relative to a collection. This class
 * provides methods to build vocabulary, compute TF-IDF scores, and transform
 * documents into feature vectors.
 */
class TfidfVectorizer {
public:
    /**
     * @brief Constructor with customizable parameters
     * @param sublinearTf Whether to apply sublinear scaling to term frequencies
     * @param maxDf Maximum document frequency for feature filtering
     * @param maxFeatures Maximum vocabulary size
     * @param minNgram Minimum n-gram length
     * @param maxNgram Maximum n-gram length
     */
    TfidfVectorizer(
        bool sublinearTf = true,
        double maxDf = 0.5,
        size_t maxFeatures = 10000,
        size_t minNgram = 1,
        size_t maxNgram = 2
    );
    
    /**
     * @brief Tokenizes text into words and n-grams
     * @param text Input text to tokenize
     * @return Vector of tokens
     */
    std::vector<std::string> tokenize(std::string_view text) const;
    
    /**
     * @brief Builds vocabulary and calculates document frequencies
     * @param texts Collection of documents to analyze
     * @param maxDf Maximum document frequency threshold
     * @param maxFeatures Maximum vocabulary size
     */
    void fit(
        const std::vector<std::string>& texts,
        double maxDf = 0.5,
        size_t maxFeatures = 10000
    );
    
    /**
     * @brief Transforms documents into TF-IDF feature vectors
     * @param texts Collection of documents to transform
     * @return Matrix of TF-IDF features (one vector per document)
     */
    std::vector<std::vector<double>> transform(
        const std::vector<std::string>& texts
    ) const;
    
    /**
     * @brief Fits vocabulary and transforms documents in one step
     * @param texts Collection of documents to analyze and transform
     * @return Matrix of TF-IDF features
     */
    std::vector<std::vector<double>> fitTransform(
        const std::vector<std::string>& texts
    );
    
    /**
     * @brief Get the vocabulary (terms and their indices)
     * @return Map of terms to feature indices
     */
    const std::unordered_map<std::string, int>& getVocabulary() const;
    
    /**
     * @brief Get the document frequencies for each term
     * @return Vector of document frequencies
     */
    const std::vector<int>& getDocumentFrequencies() const;
    
    /**
     * @brief Get the number of features (vocabulary size)
     * @return Vocabulary size
     */
    size_t getNumFeatures() const;
    
    /**
     * @brief Serializes the vectorizer to a file
     * @param filePath Path where the vectorizer should be saved
     * @return True if serialization was successful
     */
    bool save(const std::string& filePath) const;
    
    /**
     * @brief Loads a vectorizer from a file
     * @param filePath Path to the saved vectorizer
     * @return True if loading was successful
     */
    bool load(const std::string& filePath);
    
private:
    bool sublinearTf;                           ///< Whether to apply sublinear TF scaling
    double maxDf;                               ///< Maximum document frequency
    size_t maxFeatures;                         ///< Maximum vocabulary size
    size_t minNgram;                            ///< Minimum n-gram length
    size_t maxNgram;                            ///< Maximum n-gram length
    std::unordered_map<std::string, int> vocabulary; ///< Maps terms to feature indices
    std::vector<int> documentFrequencies;       ///< Document frequency for each term
    int totalDocuments;                         ///< Total number of documents seen during fit
    
    /**
     * @brief Transforms a single document into a TF-IDF feature vector
     * @param text Document to transform
     * @return TF-IDF feature vector
     */
    std::vector<double> transformSingleDocument(const std::string& text) const;
    
    /**
     * @brief Normalizes a vector to unit length (L2 norm)
     * @param vector Vector to normalize
     */
    void normalizeVector(std::vector<double>& vector) const;
    
    /**
     * @brief Builds vocabulary from tokenized documents
     * @param tokenizedDocs Collection of tokenized documents
     * @param maxDf Maximum document frequency threshold
     * @param maxFeatures Maximum vocabulary size
     */
    void buildVocabulary(
        const std::vector<std::vector<std::string>>& tokenizedDocs,
        double maxDf,
        size_t maxFeatures
    );
    
    /**
     * @brief Calculates TF-IDF score for a term
     * @param termFreq Frequency of the term in the document
     * @param docFreq Number of documents containing the term
     * @param totalDocs Total number of documents in the corpus
     * @return TF-IDF score
     */
    double calculateTfIdf(int termFreq, int docFreq, int totalDocs) const;
};

} // namespace preprocessing
} // namespace blahajpi
