/**
 * @file text_processor.cpp
 * @brief Implementation of the TextProcessor class
 */

#include "blahajpi/preprocessing/text_processor.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <regex>
#include <unordered_map>

namespace blahajpi {
namespace preprocessing {

TextProcessor::TextProcessor() {
    // Initialize stopwords
    stopwords = {
        "a", "an", "and", "are", "as", "at", "be", "by", "for", "from",
        "has", "he", "in", "is", "it", "its", "of", "on", "that", "the",
        "to", "was", "were", "will", "with", "i", "me", "my", "myself",
        "we", "our", "ours", "ourselves", "you", "your", "yours",
        "yourself", "yourselves", "he", "him", "his", "himself", "she",
        "her", "hers", "herself", "it", "its", "itself", "they", "them",
        "their", "theirs", "themselves", "what", "which", "who", "whom",
        "this", "that", "these", "those", "am", "is", "are", "was", "were",
        "be", "been", "being", "have", "has", "had", "having", "do", "does",
        "did", "doing", "would", "should", "could", "ought", "i'm", "you're",
        "he's", "she's", "it's", "we're", "they're", "i've", "you've",
        "we've", "they've", "i'd", "you'd", "he'd", "she'd", "we'd",
        "they'd", "i'll", "you'll", "he'll", "she'll", "we'll", "they'll",
        "isn't", "aren't", "wasn't", "weren't", "hasn't", "haven't", "hadn't",
        "doesn't", "don't", "didn't", "won't", "wouldn't", "shan't", "shouldn't",
        "can't", "cannot", "couldn't", "mustn't", "let's", "that's", "who's",
        "what's", "here's", "there's", "when's", "where's", "why's", "how's",
        "so", "than", "too", "very", "just", "but", "however", "still"
    };
    
    // Initialize negation words
    negationWords = {
        "not", "no", "never", "neither", "none", "nobody", "nowhere",
        "don't", "dont", "can't", "cant", "won't", "wont", "isn't", "isnt",
        "aren't", "arent", "wasn't", "wasnt", "weren't", "werent", "hasn't",
        "hasnt", "haven't", "havent", "hadn't", "hadnt", "doesn't", "doesnt",
        "didn't", "didnt", "shouldn't", "shouldnt", "wouldn't", "wouldnt",
        "couldn't", "couldnt", "nothing"
    };
    
    // Initialize preprocessing functions
    initializePreprocessingFunctions();
}

TextProcessor::TextProcessor(
    const std::unordered_set<std::string>& customStopwords,
    const std::unordered_set<std::string>& customNegationWords
) : stopwords(customStopwords),
    negationWords(customNegationWords) {
    
    // Initialize preprocessing functions
    initializePreprocessingFunctions();
}

std::string TextProcessor::preprocess(
    std::string_view text,
    const std::vector<std::string>& steps
) const {
    // Use provided steps or default pipeline
    std::vector<std::string> preprocessingSteps = steps;
    if (preprocessingSteps.empty()) {
        // Default pipeline
        preprocessingSteps = {
            "remove_urls",
            "remove_mentions",
            "process_hashtags",
            "lowercase",
            "expand_abbreviations",
            "handle_negations",
            "remove_punctuation",
            "remove_numbers",
            "normalize_whitespace",
            "remove_stopwords",
            "normalize_repeated_chars"
        };
    }
    
    // Apply each preprocessing step in sequence
    std::string processedText(text);
    for (const auto& step : preprocessingSteps) {
        auto it = preprocessingFunctions.find(step);
        if (it != preprocessingFunctions.end()) {
            processedText = it->second(processedText);
        }
    }
    
    return processedText;
}

std::unordered_map<std::string, std::function<std::string(std::string_view)>>
TextProcessor::getPreprocessingFunctions() const {
    return preprocessingFunctions;
}

void TextProcessor::addPreprocessingStep(
    const std::string& name,
    std::function<std::string(std::string_view)> func
) {
    preprocessingFunctions[name] = std::move(func);
}

void TextProcessor::addStopwords(const std::vector<std::string>& words) {
    for (const auto& word : words) {
        stopwords.insert(word);
    }
}

void TextProcessor::addNegationWords(const std::vector<std::string>& words) {
    for (const auto& word : words) {
        negationWords.insert(word);
    }
}

void TextProcessor::initializePreprocessingFunctions() {
    preprocessingFunctions = {
        {"lowercase", [this](std::string_view text) { return this->lowercase(text); }},
        {"remove_punctuation", [this](std::string_view text) { return this->removePunctuation(text); }},
        {"remove_numbers", [this](std::string_view text) { return this->removeNumbers(text); }},
        {"remove_non_ascii", [this](std::string_view text) { return this->removeNonAscii(text); }},
        {"normalize_whitespace", [this](std::string_view text) { return this->normalizeWhitespace(text); }},
        {"remove_stopwords", [this](std::string_view text) { return this->removeStopwords(text); }},
        {"handle_negations", [this](std::string_view text) { return this->handleNegations(text); }},
        {"expand_abbreviations", [this](std::string_view text) { return this->expandAbbreviations(text); }},
        {"normalize_repeated_chars", [this](std::string_view text) { return this->normalizeRepeatedChars(text); }},
        {"process_hashtags", [this](std::string_view text) { return this->processHashtags(text); }},
        {"remove_mentions", [this](std::string_view text) { return this->removeMentions(text); }},
        {"remove_urls", [this](std::string_view text) { return this->removeUrls(text); }}
    };
}

std::string TextProcessor::lowercase(std::string_view text) const {
    std::string result(text);
    std::transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string TextProcessor::removePunctuation(std::string_view text) const {
    std::string result;
    result.reserve(text.size());
    
    for (char c : text) {
        if (!std::ispunct(static_cast<unsigned char>(c))) {
            result.push_back(c);
        }
    }
    
    return result;
}

std::string TextProcessor::removeNumbers(std::string_view text) const {
    std::string result;
    result.reserve(text.size());
    
    for (char c : text) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            result.push_back(c);
        }
    }
    
    return result;
}

std::string TextProcessor::removeNonAscii(std::string_view text) const {
    std::string result;
    result.reserve(text.size());
    
    for (char c : text) {
        if (static_cast<unsigned char>(c) < 128) {
            result.push_back(c);
        }
    }
    
    return result;
}

std::string TextProcessor::normalizeWhitespace(std::string_view text) const {
    std::string result;
    result.reserve(text.size());
    
    bool lastWasSpace = true;  // Start with true to trim leading spaces
    
    for (char c : text) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!lastWasSpace) {
                result.push_back(' ');
            }
            lastWasSpace = true;
        } else {
            result.push_back(c);
            lastWasSpace = false;
        }
    }
    
    // Trim trailing space if present
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

std::string TextProcessor::removeStopwords(std::string_view text) const {
    std::istringstream iss{std::string(text)};
    std::ostringstream oss;
    
    std::string word;
    bool firstWord = true;
    
    while (iss >> word) {
        if (stopwords.find(word) == stopwords.end()) {
            if (!firstWord) {
                oss << ' ';
            }
            oss << word;
            firstWord = false;
        }
    }
    
    return oss.str();
}

std::string TextProcessor::handleNegations(std::string_view text) const {
    std::istringstream iss{std::string(text)};
    std::ostringstream oss;
    std::string word;
    bool negate = false;
    bool firstWord = true;
    
    // The scope of negation (how many words after a negation word to mark)
    constexpr int negationScope = 3;
    int wordsToNegate = 0;
    
    while (iss >> word) {
        // Check if this is a negation word
        if (negationWords.find(word) != negationWords.end()) {
            negate = true;
            wordsToNegate = negationScope;
            
            if (!firstWord) oss << ' ';
            oss << word;
        } 
        else {
            if (negate && wordsToNegate > 0) {
                // Mark this word as negated by adding a prefix
                if (!firstWord) oss << ' ';
                oss << "NOT_" << word;
                wordsToNegate--;
                
                // If we've negated enough words, reset the negation flag
                if (wordsToNegate == 0) {
                    negate = false;
                }
            } 
            else {
                if (!firstWord) oss << ' ';
                oss << word;
            }
        }
        firstWord = false;
    }
    
    return oss.str();
}

std::string TextProcessor::expandAbbreviations(std::string_view text) const {
    // Common social media abbreviations
    static const std::unordered_map<std::string, std::string> abbreviations = {
        {"u", "you"},
        {"r", "are"},
        {"ur", "your"},
        {"n", "and"},
        {"y", "why"},
        {"w/", "with"},
        {"w/o", "without"},
        {"btw", "by the way"},
        {"imo", "in my opinion"},
        {"idk", "i do not know"},
        {"lol", "laugh"},
        {"rofl", "laugh"},
        {"lmao", "laugh"},
        {"b/c", "because"},
        {"cuz", "because"},
        {"bc", "because"},
        {"b4", "before"},
        {"ppl", "people"},
        {"sry", "sorry"},
        {"thx", "thanks"},
        {"ty", "thank you"},
        {"gd", "good"},
        {"fwiw", "for what it is worth"},
        {"tbh", "to be honest"},
        {"iirc", "if i recall correctly"},
        {"nvm", "never mind"},
        {"omg", "oh my god"},
        {"gtg", "got to go"},
        {"brb", "be right back"},
        {"afaik", "as far as i know"},
        {"irl", "in real life"},
        {"jk", "just kidding"},
        {"tfw", "that feeling when"},
        {"mfw", "my face when"},
        {"rn", "right now"},
        {"smh", "shaking my head"},
        {"tbf", "to be fair"},
        {"tldr", "too long did not read"},
        {"yolo", "you only live once"},
        {"fomo", "fear of missing out"}
    };
    
    std::istringstream iss{std::string(text)};
    std::ostringstream oss;
    std::string word;
    bool firstWord = true;
    
    while (iss >> word) {
        // Convert to lowercase for matching
        std::string lowerWord = word;
        std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        
        // Replace abbreviation if found
        auto it = abbreviations.find(lowerWord);
        if (it != abbreviations.end()) {
            if (!firstWord) oss << ' ';
            oss << it->second;
        } else {
            if (!firstWord) oss << ' ';
            oss << word;
        }
        firstWord = false;
    }
    
    return oss.str();
}

std::string TextProcessor::normalizeRepeatedChars(std::string_view text) const {
    std::string result;
    result.reserve(text.size());
    
    if (text.empty()) {
        return result;
    }
    
    // Add first character
    result.push_back(text[0]);
    
    // Add subsequent characters only if they're not repeating more than twice
    for (size_t i = 1; i < text.size(); ++i) {
        if (text[i] != text[i-1] || 
            (result.size() >= 2 && result[result.size()-1] != result[result.size()-2])) {
            result.push_back(text[i]);
        }
    }
    
    return result;
}

std::string TextProcessor::processHashtags(std::string_view text) const {
    std::regex hashtagPattern(R"(#([a-zA-Z0-9_]+))");
    std::string input(text);
    
    // Lambda function to process a hashtag match
    auto processHashtag = [](const std::string& tag) -> std::string {
        // Remove the # symbol
        std::string processed = tag.substr(1);
        
        // Convert from camelCase to separate words
        std::string separated;
        for (char c : processed) {
            if (std::isupper(c) && !separated.empty() && separated.back() != ' ') {
                separated.push_back(' ');
            }
            separated.push_back(std::tolower(c));
        }
        
        return separated;
    };
    
    // Use a temporary string to build the result
    std::string result = input;
    
    // Find all hashtags and replace them
    std::smatch match;
    std::string::const_iterator searchStart(input.cbegin());
    
    // Find all occurrences
    while (std::regex_search(searchStart, input.cend(), match, hashtagPattern)) {
        // Get the hashtag
        std::string hashtag = match[0].str();
        
        // Process it
        std::string processed = processHashtag(hashtag);
        
        // Replace in the result string
        size_t pos = result.find(hashtag);
        if (pos != std::string::npos) {
            result.replace(pos, hashtag.length(), processed);
        }
        
        // Move to the next position after this match
        searchStart = match.suffix().first;
    }
    
    return result;
}

std::string TextProcessor::removeMentions(std::string_view text) const {
    std::regex mentionPattern(R"(@[a-zA-Z0-9_]+)");
    std::string input(text);
    
    // Replace mentions with empty string
    return std::regex_replace(input, mentionPattern, "");
}

std::string TextProcessor::removeUrls(std::string_view text) const {
    std::regex urlPattern(R"((https?://)?([a-zA-Z0-9]+\.)+[a-zA-Z0-9]{2,}(/[a-zA-Z0-9_./?=%&-]*)?)");
    std::string input(text);
    
    // Replace URLs with empty string
    return std::regex_replace(input, urlPattern, "");
}

} // namespace preprocessing
} // namespace blahajpi
