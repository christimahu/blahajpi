/**
 * @file vectorizer.cpp
 * @brief Implementation of the TF-IDF vectorizer
 */

#include "blahajpi/preprocessing/vectorizer.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <set>
#include <numeric>
#include <iostream>
#include <fstream>

namespace blahajpi {
namespace preprocessing {

TfidfVectorizer::TfidfVectorizer(
    bool sublinearTf,
    double maxDf,
    size_t maxFeatures,
    size_t minNgram,
    size_t maxNgram
) : sublinearTf(sublinearTf),
    maxDf(maxDf),
    maxFeatures(maxFeatures),
    minNgram(minNgram),
    maxNgram(maxNgram),
    totalDocuments(0) {
    
    // Validate parameters
    if (minNgram < 1) {
        minNgram = 1;
    }
    
    if (maxNgram < minNgram) {
        maxNgram = minNgram;
    }
    
    if (maxDf <= 0.0 || maxDf > 1.0) {
        maxDf = 1.0;
    }
}

std::vector<std::string> TfidfVectorizer::tokenize(std::string_view text) const {
    // Extract individual words first
    std::istringstream iss{std::string(text)};
    std::vector<std::string> words;
    std::string word;

    while (iss >> word) {
        if (!word.empty()) {
            words.push_back(word);
        }
    }
    
    std::vector<std::string> tokens;
    
    // Generate n-grams of all lengths in the specified range
    for (size_t n = minNgram; n <= maxNgram; ++n) {
        // Skip if there aren't enough words for this n-gram size
        if (words.size() < n) continue;
        
        // Generate n-grams of length n
        for (size_t i = 0; i <= words.size() - n; ++i) {
            std::string ngram = words[i];
            
            // Build n-gram by concatenating words with underscores
            for (size_t j = 1; j < n; ++j) {
                ngram += "_" + words[i + j];
            }
            
            tokens.push_back(ngram);
        }
    }
    
    return tokens;
}

void TfidfVectorizer::fit(
    const std::vector<std::string>& texts,
    double maxDf,
    size_t maxFeatures
) {
    // Reset state
    vocabulary.clear();
    documentFrequencies.clear();
    totalDocuments = texts.size();
    
    if (totalDocuments == 0) {
        return;  // Nothing to fit
    }
    
    // Override parameters if provided
    if (maxDf > 0.0) {
        this->maxDf = maxDf;
    }
    
    if (maxFeatures > 0) {
        this->maxFeatures = maxFeatures;
    }
    
    // Tokenize all documents
    std::vector<std::vector<std::string>> tokenizedDocs(texts.size());
    
    for (size_t i = 0; i < texts.size(); ++i) {
        tokenizedDocs[i] = tokenize(texts[i]);
    }
    
    // Build vocabulary and calculate document frequencies
    buildVocabulary(tokenizedDocs, this->maxDf, this->maxFeatures);
    
    std::cout << "Built vocabulary with " << vocabulary.size() 
              << " features (n-gram range: " << minNgram << "-" << maxNgram << ")" << std::endl;
}

std::vector<std::vector<double>> TfidfVectorizer::transform(
    const std::vector<std::string>& texts
) const {
    if (vocabulary.empty()) {
        throw std::runtime_error("Vocabulary is empty. Call fit() first.");
    }
    
    std::vector<std::vector<double>> featureMatrix(texts.size());
    
    // Process each document
    for (size_t i = 0; i < texts.size(); ++i) {
        featureMatrix[i] = transformSingleDocument(texts[i]);
    }
    
    return featureMatrix;
}

std::vector<std::vector<double>> TfidfVectorizer::fitTransform(
    const std::vector<std::string>& texts
) {
    fit(texts);
    return transform(texts);
}

const std::unordered_map<std::string, int>& TfidfVectorizer::getVocabulary() const {
    return vocabulary;
}

const std::vector<int>& TfidfVectorizer::getDocumentFrequencies() const {
    return documentFrequencies;
}

size_t TfidfVectorizer::getNumFeatures() const {
    return vocabulary.size();
}

bool TfidfVectorizer::save(const std::string& filePath) const {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filePath << std::endl;
        return false;
    }
    
    try {
        // Write parameters
        file.write(reinterpret_cast<const char*>(&sublinearTf), sizeof(sublinearTf));
        file.write(reinterpret_cast<const char*>(&maxDf), sizeof(maxDf));
        file.write(reinterpret_cast<const char*>(&maxFeatures), sizeof(maxFeatures));
        file.write(reinterpret_cast<const char*>(&minNgram), sizeof(minNgram));
        file.write(reinterpret_cast<const char*>(&maxNgram), sizeof(maxNgram));
        file.write(reinterpret_cast<const char*>(&totalDocuments), sizeof(totalDocuments));
        
        // Write vocabulary size
        size_t vocabSize = vocabulary.size();
        file.write(reinterpret_cast<const char*>(&vocabSize), sizeof(vocabSize));
        
        // Write vocabulary entries
        for (const auto& [term, index] : vocabulary) {
            // Write term length and term
            size_t termLength = term.size();
            file.write(reinterpret_cast<const char*>(&termLength), sizeof(termLength));
            file.write(term.c_str(), termLength);
            
            // Write index
            file.write(reinterpret_cast<const char*>(&index), sizeof(index));
        }
        
        // Write document frequencies
        size_t dfSize = documentFrequencies.size();
        file.write(reinterpret_cast<const char*>(&dfSize), sizeof(dfSize));
        file.write(reinterpret_cast<const char*>(documentFrequencies.data()), 
                  dfSize * sizeof(int));
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving vectorizer: " << e.what() << std::endl;
        return false;
    }
}

bool TfidfVectorizer::load(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for reading: " << filePath << std::endl;
        return false;
    }
    
    try {
        // Read parameters
        file.read(reinterpret_cast<char*>(&sublinearTf), sizeof(sublinearTf));
        file.read(reinterpret_cast<char*>(&maxDf), sizeof(maxDf));
        file.read(reinterpret_cast<char*>(&maxFeatures), sizeof(maxFeatures));
        file.read(reinterpret_cast<char*>(&minNgram), sizeof(minNgram));
        file.read(reinterpret_cast<char*>(&maxNgram), sizeof(maxNgram));
        file.read(reinterpret_cast<char*>(&totalDocuments), sizeof(totalDocuments));
        
        // Read vocabulary
        size_t vocabSize;
        file.read(reinterpret_cast<char*>(&vocabSize), sizeof(vocabSize));
        
        vocabulary.clear();
        for (size_t i = 0; i < vocabSize; ++i) {
            // Read term
            size_t termLength;
            file.read(reinterpret_cast<char*>(&termLength), sizeof(termLength));
            
            std::string term(termLength, '\0');
            file.read(&term[0], termLength);
            
            // Read index
            int index;
            file.read(reinterpret_cast<char*>(&index), sizeof(index));
            
            // Insert into vocabulary
            vocabulary[term] = index;
        }
        
        // Read document frequencies
        size_t dfSize;
        file.read(reinterpret_cast<char*>(&dfSize), sizeof(dfSize));
        
        documentFrequencies.resize(dfSize);
        file.read(reinterpret_cast<char*>(documentFrequencies.data()), 
                 dfSize * sizeof(int));
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading vectorizer: " << e.what() << std::endl;
        return false;
    }
}

std::vector<double> TfidfVectorizer::transformSingleDocument(const std::string& text) const {
    // Count term frequencies in this document
    std::unordered_map<std::string, int> termFreqs;
    for (const auto& token : tokenize(text)) {
        termFreqs[token]++;
    }
    
    // Create and initialize feature vector
    std::vector<double> featureVector(vocabulary.size(), 0.0);
    
    // Calculate TF-IDF for each term in the vocabulary
    for (const auto& [term, freq] : termFreqs) {
        // Skip terms not in vocabulary
        auto it = vocabulary.find(term);
        if (it == vocabulary.end()) {
            continue;
        }
        
        // Get the feature index and document frequency
        int featureIdx = it->second;
        int docFreq = documentFrequencies[featureIdx];
        
        // Calculate TF-IDF
        double tf = static_cast<double>(freq);
        if (sublinearTf) {
            tf = 1.0 + std::log(tf);  // Sublinear scaling
        }
        
        double idf = std::log(static_cast<double>(totalDocuments + 1) / 
                             (docFreq + 1)) + 1.0;  // Smoothed IDF
        
        featureVector[featureIdx] = tf * idf;
    }
    
    // Normalize the feature vector (L2 norm)
    normalizeVector(featureVector);
    
    return featureVector;
}

void TfidfVectorizer::normalizeVector(std::vector<double>& vector) const {
    double squaredSum = 0.0;
    for (const auto& val : vector) {
        squaredSum += val * val;
    }
    
    if (squaredSum > 0.0) {
        double norm = std::sqrt(squaredSum);
        for (auto& val : vector) {
            val /= norm;
        }
    }
}

void TfidfVectorizer::buildVocabulary(
    const std::vector<std::vector<std::string>>& tokenizedDocs,
    double maxDf,
    size_t maxFeatures
) {
    // Count document frequency for each term
    std::unordered_map<std::string, int> docFreqs;
    
    // Process each document
    for (const auto& doc : tokenizedDocs) {
        // Use set to count each term only once per document
        std::set<std::string> uniqueTerms(doc.begin(), doc.end());
        
        // Count document frequency for each unique term
        for (const auto& term : uniqueTerms) {
            docFreqs[term]++;
        }
    }
    
    // Convert maxDf from fraction to absolute count if needed
    int maxDfCount = (maxDf < 1.0) ? 
                     static_cast<int>(maxDf * totalDocuments) : 
                     static_cast<int>(maxDf);
    
    // Filter terms by document frequency
    std::vector<std::pair<std::string, int>> filteredTerms;
    filteredTerms.reserve(docFreqs.size());
    
    for (const auto& [term, freq] : docFreqs) {
        if (freq <= maxDfCount) {
            filteredTerms.emplace_back(term, freq);
        }
    }
    
    // Sort by descending frequency for feature selection
    std::sort(
        filteredTerms.begin(), filteredTerms.end(),
        [](const auto& a, const auto& b) { 
            return a.second > b.second; 
        }
    );
    
    // Limit to maxFeatures
    if (filteredTerms.size() > maxFeatures) {
        filteredTerms.resize(maxFeatures);
    }
    
    // Build vocabulary and document frequency vector
    vocabulary.clear();
    documentFrequencies.clear();
    documentFrequencies.reserve(filteredTerms.size());
    
    for (size_t i = 0; i < filteredTerms.size(); ++i) {
        const auto& [term, freq] = filteredTerms[i];
        vocabulary[term] = static_cast<int>(i);
        documentFrequencies.push_back(freq);
    }
}

double TfidfVectorizer::calculateTfIdf(int termFreq, int docFreq, int totalDocs) const {
    double tf = static_cast<double>(termFreq);
    if (sublinearTf) {
        tf = 1.0 + std::log(tf);
    }
    
    double idf = std::log(static_cast<double>(totalDocs + 1) / (docFreq + 1)) + 1.0;
    
    return tf * idf;
}

} // namespace preprocessing
} // namespace blahajpi
