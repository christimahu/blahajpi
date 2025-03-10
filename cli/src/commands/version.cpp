/**
 * @file version.cpp
 * @brief Implementation of the version command
 */

#include "bpicli/commands.hpp"
#include "bpicli/utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

namespace bpicli {

int handleVersion(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer) {
    // Get version information
    std::string version = utils::getVersion();
    std::string buildDate = utils::getBuildDate();
    
    // Get library information if available
    std::unordered_map<std::string, std::string> libraryInfo;
    try {
        // This would require the library to expose getLibraryInfo() as in blahajpi.hpp
        // For now, we'll just use the version from utils
    } catch (...) {
        // Ignore errors
    }
    
    // Display version information
    std::cout << "Blahaj PI - Your friendly shark detective keeping social waters safe\n\n";
    std::cout << "Version:      " << version << std::endl;
    std::cout << "Build Date:   " << buildDate << std::endl;
    
    // Get compiler and platform information
    #if defined(__clang__)
        std::cout << "Compiler:     Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__ << std::endl;
    #elif defined(__GNUC__)
        std::cout << "Compiler:     GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << std::endl;
    #elif defined(_MSC_VER)
        std::cout << "Compiler:     MSVC " << _MSC_VER << std::endl;
    #else
        std::cout << "Compiler:     Unknown" << std::endl;
    #endif
    
    // Platform information
    #if defined(_WIN32) || defined(_WIN64)
        std::cout << "Platform:     Windows" << std::endl;
    #elif defined(__APPLE__)
        std::cout << "Platform:     macOS" << std::endl;
    #elif defined(__linux__)
        std::cout << "Platform:     Linux" << std::endl;
    #else
        std::cout << "Platform:     Unknown" << std::endl;
    #endif
    
    // C++ Standard version
    #if __cplusplus >= 202300L
        std::cout << "C++ Standard: C++23" << std::endl;
    #elif __cplusplus >= 202002L
        std::cout << "C++ Standard: C++20" << std::endl;
    #elif __cplusplus >= 201703L
        std::cout << "C++ Standard: C++17" << std::endl;
    #elif __cplusplus >= 201402L
        std::cout << "C++ Standard: C++14" << std::endl;
    #elif __cplusplus >= 201103L
        std::cout << "C++ Standard: C++11" << std::endl;
    #else
        std::cout << "C++ Standard: Pre-C++11" << std::endl;
    #endif
    
    // Show current configuration file
    std::cout << "\nConfiguration: ";
    auto config = analyzer.getConfig();
    auto it = config.find("config-path");
    if (it != config.end() && !it->second.empty()) {
        std::cout << it->second << std::endl;
    } else {
        std::cout << "Default" << std::endl;
    }
    
    // Show copyright notice
    std::cout << "\nCopyright © 2025, Blahaj PI Team" << std::endl;
    std::cout << "License: MIT License" << std::endl;
    
    return 0;
}

} // namespace bpicli
