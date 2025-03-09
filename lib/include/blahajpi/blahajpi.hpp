/**
 * @file blahajpi.hpp
 * @brief Main include file for the Blahaj PI library
 * 
 * This file includes all the necessary headers for using the Blahaj PI
 * library. By including this single header, users get access to the
 * complete public API of the library.
 */

#pragma once

// Core components
#include "analyzer.hpp"
#include "config.hpp"

// Models
#include "models/classifier.hpp"
#include "models/sgd.hpp"
#include "models/neural_network.hpp"

// Preprocessing
#include "preprocessing/text_processor.hpp"
#include "preprocessing/vectorizer.hpp"

// Utilities
#include "utils/word_cloud.hpp"
#include "utils/dataset.hpp"

// Evaluation
#include "evaluation/metrics.hpp"

/**
 * @namespace blahajpi
 * @brief Main namespace for Blahaj PI functionality
 * 
 * The blahajpi namespace contains all components of the Blahaj PI
 * system for analyzing social media content to identify potentially
 * harmful content targeting transgender individuals.
 */
namespace blahajpi {

/**
 * @brief Gets the library version information
 * @return Version string in format "major.minor.patch"
 */
std::string getVersion();

/**
 * @brief Gets information about the library
 * @return Map containing information about the library
 */
std::unordered_map<std::string, std::string> getLibraryInfo();

} // namespace blahajpi
