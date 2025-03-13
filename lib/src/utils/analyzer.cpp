/**
 * @file analyzer.cpp
 * @brief Implementation of the Analyzer class
 */

#include "blahajpi/analyzer.hpp"
#include "blahajpi/config.hpp"
#include "blahajpi/models/sgd.hpp"
#include "blahajpi/preprocessing/text_processor.hpp"
#include "blahajpi/preprocessing/vectorizer.hpp"
#include "blahajpi/utils/dataset.hpp"
#include "blahajpi/utils/word_cloud.hpp"
#include "blahajpi/evaluation/metrics.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace blahajpi {

// ==========================================
// AnalysisResult Implementation
// ==========================================

std::unordered_map<std::string, std::string> AnalysisResult::toMap() const {
    std::unordered_map<std::string, std::string> map;
    
    map["text"] = text;
    map["cleaned_text"] = cleanedText;
    map["sentiment"] = sentiment;
    map["harm_score"] = std::to_string(harmScore);
    map["confidence"] = std::to_string(confidence);
    map["explanation"] = explanation;
    
    // Convert key terms to a comma-separated string
    std::stringstream termsStream;
    for (size_t i = 0; i < keyTerms.size(); ++i) {
        if (i > 0) termsStream << ",";
        termsStream << keyTerms[i];
    }
    map["key_terms"] = termsStream.str();
    
    return map;
}

AnalysisResult AnalysisResult::fromMap(const std::unordered_map<std::string, std::string>& map) {
    AnalysisResult result;
    
    // Helper lambda for safe map access
    auto getMapValue = [&map](const std::string& key) -> std::string {
        auto it = map.find(key);
        return (it != map.end()) ? it->second : "";
    };
    
    result.text = getMapValue("text");
    result.cleanedText = getMapValue("cleaned_text");
    result.sentiment = getMapValue("sentiment");
    
    // Convert numeric values safely
    try {
        result.harmScore = std::stod(getMapValue("harm_score"));
    } catch (...) {
        result.harmScore = 0.0;
    }
    
    try {
        result.confidence = std::stod(getMapValue("confidence"));
    } catch (...) {
        result.confidence = 0.0;
    }
    
    result.explanation = getMapValue("explanation");
    
    // Parse key terms from comma-separated string
    std::string termsStr = getMapValue("key_terms");
    std::stringstream termsStream(termsStr);
    std::string term;
    while (std::getline(termsStream, term, ',')) {
        if (!term.empty()) {
            result.keyTerms.push_back(term);
        }
    }
    
    return result;
}

// ==========================================
// AnalyzerImpl Implementation
// ==========================================

class AnalyzerImpl {
public:
    AnalyzerImpl() : config_(), textProcessor_(), vectorizer_(true, 0.5, 10000, 1, 2) {
        // Initialize with defaults
    }
    
    AnalyzerImpl(const std::string& configPath) : config_(configPath), 
                                         textProcessor_(), 
                                         vectorizer_(true, 0.5, 10000, 1, 2) {
        // Apply configuration
        applyConfig();
    }
    
    void applyConfig() {
        // Configure vectorizer from settings
        bool sublinearTf = config_.getBool("use-sublinear-tf", true);
        double maxDf = config_.getDouble("max-df", 0.5);
        int maxFeatures = config_.getInt("max-features", 10000);
        int minNgram = config_.getInt("min-ngram", 1);
        int maxNgram = config_.getInt("max-ngram", 2);
        
        // Create a new vectorizer with these settings
        vectorizer_ = preprocessing::TfidfVectorizer(
            sublinearTf, maxDf, maxFeatures, minNgram, maxNgram);
        
        // If model path is specified, try to load the model
        std::string modelDir = config_.getString("model-dir", "");
        if (!modelDir.empty()) {
            loadModel(modelDir);
        }
    }
    
    AnalysisResult analyze(const std::string& text) {
        if (!model_) {
            throw std::runtime_error("No model loaded. Call loadModel() first.");
        }
        
        AnalysisResult result;
        result.text = text;
        
        // Preprocess the text
        result.cleanedText = textProcessor_.preprocess(text);
        
        // Extract features
        std::vector<std::string> texts = {result.cleanedText};
        std::vector<std::vector<double>> features = vectorizer_.transform(texts);
        
        if (features.empty() || features[0].empty()) {
            // Handle empty features case
            result.sentiment = "Safe";
            result.harmScore = 0.0;
            result.confidence = 0.5;
            result.explanation = "Unable to analyze text (no features extracted).";
            return result;
        }
        
        // Get raw score from model
        std::vector<double> scores = model_->decisionFunction(features);
        result.harmScore = scores[0];
        
        // Get probabilities
        std::vector<double> probs = model_->predictProbability(features);
        result.confidence = probs[0];
        
        // Determine sentiment label
        if (result.harmScore > 0.0) {
            result.sentiment = "Harmful";
        } else {
            result.sentiment = "Safe";
        }
        
        // Extract key terms that contributed to classification
        result.keyTerms = extractKeyTerms(result.cleanedText, result.harmScore);
        
        // Generate explanation
        result.explanation = generateExplanation(result.harmScore, result.confidence, result.keyTerms);
        
        return result;
    }
    
    std::vector<AnalysisResult> analyzeMultiple(const std::vector<std::string>& texts) {
        std::vector<AnalysisResult> results;
        results.reserve(texts.size());
        
        for (const auto& text : texts) {
            results.push_back(analyze(text));
        }
        
        return results;
    }
    
    bool loadModel(const std::string& modelPath) {
        // Try to determine model type from path or configuration
        std::string modelType = "sgd"; // Default model type
        
        // Create appropriate model based on type
        if (modelType == "sgd") {
            model_ = std::make_unique<models::SGDClassifier>();
        } else {
            // Use SGD as default if model type is not recognized
            model_ = std::make_unique<models::SGDClassifier>();
        }
        
        // Try to load the model
        std::string modelFilePath = modelPath + "/model.bin";
        bool modelLoaded = model_->load(modelFilePath);
        
        if (!modelLoaded) {
            std::cerr << "Failed to load model from: " << modelFilePath << std::endl;
            return false;
        }
        
        // Try to load the vectorizer
        std::string vectorizerPath = modelPath + "/vectorizer.bin";
        bool vectorizerLoaded = vectorizer_.load(vectorizerPath);
        
        if (!vectorizerLoaded) {
            std::cerr << "Failed to load vectorizer from: " << vectorizerPath << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool trainModel(const std::string& dataPath, const std::string& outputPath) {
        // Get column names from configuration
        std::string labelColumn = config_.getString("label-column", "label");
        std::string textColumn = config_.getString("text-column", "text");
        
        // Load dataset
        utils::Dataset dataset;
        if (!dataset.loadFromFile(dataPath, utils::Dataset::Format::AUTO, labelColumn, textColumn)) {
            std::cerr << "Failed to load dataset from: " << dataPath << std::endl;
            return false;
        }
        
        // Split data for training and testing
        dataset.splitTrainTest(0.2);
        
        // Get training data
        auto trainData = dataset.getTrainData();
        auto trainTexts = dataset.getTrainTexts();
        auto trainLabels = dataset.getTrainLabels();
        
        // Preprocess texts
        std::vector<std::string> cleanedTexts;
        cleanedTexts.reserve(trainTexts.size());
        
        for (const auto& text : trainTexts) {
            cleanedTexts.push_back(textProcessor_.preprocess(text));
        }
        
        // Extract features
        vectorizer_.fit(cleanedTexts);
        std::vector<std::vector<double>> features = vectorizer_.transform(cleanedTexts);
        
        // Create and train model
        double alpha = config_.getDouble("alpha", 0.0001);
        double eta0 = config_.getDouble("eta0", 0.01);
        int epochs = config_.getInt("epochs", 10);
        
        model_ = std::make_unique<models::SGDClassifier>("log", alpha, epochs, eta0);
        model_->fit(features, trainLabels);
        
        // Evaluate model on test data
        auto testTexts = dataset.getTestTexts();
        auto testLabels = dataset.getTestLabels();
        
        std::vector<std::string> cleanedTestTexts;
        cleanedTestTexts.reserve(testTexts.size());
        
        for (const auto& text : testTexts) {
            cleanedTestTexts.push_back(textProcessor_.preprocess(text));
        }
        
        std::vector<std::vector<double>> testFeatures = vectorizer_.transform(cleanedTestTexts);
        double accuracy = model_->score(testFeatures, testLabels);
        
        std::cout << "Model training complete. Test accuracy: " << accuracy << std::endl;
        
        // Save model and vectorizer
        if (!outputPath.empty()) {
            // Create directory if it doesn't exist
            std::filesystem::create_directories(outputPath);
            
            // Save model
            std::string modelPath = outputPath + "/model.bin";
            if (!model_->save(modelPath)) {
                std::cerr << "Failed to save model to: " << modelPath << std::endl;
                return false;
            }
            
            // Save vectorizer
            std::string vectorizerPath = outputPath + "/vectorizer.bin";
            if (!vectorizer_.save(vectorizerPath)) {
                std::cerr << "Failed to save vectorizer to: " << vectorizerPath << std::endl;
                return false;
            }
            
            // Save model info
            std::string infoPath = outputPath + "/model_info.txt";
            std::ofstream infoFile(infoPath);
            if (infoFile.is_open()) {
                infoFile << "Model Type: SGD Classifier\n";
                infoFile << "Training Date: " << getCurrentDateString() << "\n";
                infoFile << "Accuracy: " << accuracy << "\n";
                infoFile << "Parameters:\n";
                infoFile << "  alpha: " << alpha << "\n";
                infoFile << "  eta0: " << eta0 << "\n";
                infoFile << "  epochs: " << epochs << "\n";
                infoFile << "  vocabulary size: " << vectorizer_.getNumFeatures() << "\n";
                infoFile.close();
            }
        }
        
        return true;
    }
    
    bool generateVisualization(
        const std::vector<AnalysisResult>& analysisResults,
        const std::string& outputPath,
        bool harmfulOnly) {
        
        // Extract texts based on filter
        std::vector<std::string> textsToVisualize;
        
        for (const auto& result : analysisResults) {
            if (!harmfulOnly || result.sentiment == "Harmful") {
                textsToVisualize.push_back(result.cleanedText);
            }
        }
        
        if (textsToVisualize.empty()) {
            std::cerr << "No content to visualize." << std::endl;
            return false;
        }
        
        std::cout << "Generating visualization for " << textsToVisualize.size() << " texts..." << std::endl;
        
        // Configure word cloud
        utils::WordCloud wordCloud;
        utils::WordCloud::CloudConfig config;
        config.maxWords = 50;
        config.width = 80;
        config.height = 20;
        config.useColor = true;
        config.showFrequencies = true;
        
        // Generate word cloud
        std::string cloud = wordCloud.generateCustomCloud(
            textsToVisualize, config, harmfulOnly);
        
        // Save to file if path provided
        if (!outputPath.empty()) {
            return utils::WordCloud::saveWordCloud(cloud, outputPath);
        }
        
        // Otherwise, display to console
        std::cout << cloud << std::endl;
        return true;
    }
    
    std::unordered_map<std::string, std::string> getConfig() const {
        return config_.getAll();
    }
    
    void setConfig(const std::string& key, const std::string& value) {
        config_.set(key, value);
        
        // Apply any configuration changes that affect components
        applyConfig();
    }
    
    bool loadConfig(const std::string& configPath) {
        bool success = config_.loadFromFile(configPath);
        if (success) {
            applyConfig();
        }
        return success;
    }

private:
    Config config_;
    preprocessing::TextProcessor textProcessor_;
    preprocessing::TfidfVectorizer vectorizer_;
    std::unique_ptr<models::Classifier> model_;
    
    /**
     * @brief Extracts key terms that contributed to the classification
     * @param text Preprocessed text
     * @param score Model score
     * @return Vector of key terms
     */
    std::vector<std::string> extractKeyTerms(const std::string& text, double score) {
        // This is a simplified implementation
        // In a real system, you would analyze feature coefficients
        
        std::vector<std::string> terms;
        std::stringstream ss(text);
        std::string word;
        
        while (ss >> word) {
            // Check if this word is in the vocabulary and has a high coefficient
            // This is just a placeholder - actual implementation would be more complex
            if (word.length() > 3) {
                terms.push_back(word);
                
                // Limit to top 5 terms
                if (terms.size() >= 5) {
                    break;
                }
            }
        }
        
        return terms;
    }
    
    /**
     * @brief Generates an explanation of the sentiment classification
     * @param score Model score
     * @param confidence Model confidence
     * @param keyTerms Key terms that contributed to classification
     * @return Human-readable explanation
     */
    std::string generateExplanation(
        double score, 
        double confidence, 
        const std::vector<std::string>& keyTerms) {
        
        std::stringstream explanation;
        
        // Determine sentiment description based on score
        if (score > 0.5) {
            explanation << "This content appears to be highly harmful";
        } else if (score > 0.0) {
            explanation << "This content may contain harmful elements";
        } else if (score > -0.5) {
            explanation << "This content appears to be mostly safe";
        } else {
            explanation << "This content appears to be safe";
        }
        
        // Add confidence information
        explanation << " (confidence: ";
        if (confidence > 0.9) {
            explanation << "very high";
        } else if (confidence > 0.7) {
            explanation << "high";
        } else if (confidence > 0.5) {
            explanation << "moderate";
        } else {
            explanation << "low";
        }
        explanation << ").";
        
        // Add key terms
        if (!keyTerms.empty()) {
            explanation << " Key terms detected: ";
            for (size_t i = 0; i < keyTerms.size(); ++i) {
                if (i > 0) explanation << ", ";
                explanation << keyTerms[i];
            }
            explanation << ".";
        }
        
        return explanation.str();
    }
    
    /**
     * @brief Gets the current date as a string
     * @return Current date string
     */
    std::string getCurrentDateString() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// ==========================================
// Analyzer Implementation - Public
// ==========================================

Analyzer::Analyzer() : pImpl(std::make_unique<AnalyzerImpl>()) {}

Analyzer::Analyzer(const std::string& configPath) : pImpl(std::make_unique<AnalyzerImpl>(configPath)) {}

Analyzer::~Analyzer() = default;

AnalysisResult Analyzer::analyze(const std::string& text) {
    return pImpl->analyze(text);
}

std::vector<AnalysisResult> Analyzer::analyzeMultiple(const std::vector<std::string>& texts) {
    return pImpl->analyzeMultiple(texts);
}

bool Analyzer::loadModel(const std::string& modelPath) {
    return pImpl->loadModel(modelPath);
}

bool Analyzer::trainModel(const std::string& dataPath, const std::string& outputPath) {
    return pImpl->trainModel(dataPath, outputPath);
}

bool Analyzer::generateVisualization(
    const std::vector<AnalysisResult>& analysisResults,
    const std::string& outputPath,
    bool harmfulOnly) {
    
    return pImpl->generateVisualization(analysisResults, outputPath, harmfulOnly);
}

std::unordered_map<std::string, std::string> Analyzer::getConfig() const {
    return pImpl->getConfig();
}

void Analyzer::setConfig(const std::string& key, const std::string& value) {
    pImpl->setConfig(key, value);
}

bool Analyzer::loadConfig(const std::string& configPath) {
    return pImpl->loadConfig(configPath);
}

} // namespace blahajpi
