/**
 * @file train.cpp
 * @brief Implementation of the train command
 */

#include "bpicli/commands.hpp"
#include "bpicli/utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <iomanip>

namespace bpicli {

int handleTrain(const std::vector<std::string>& args, blahajpi::Analyzer& analyzer) {
    // Parse arguments
    auto parsedArgs = utils::parseArgs(args);
    
    // Check for required arguments
    if (parsedArgs.count("dataset") == 0) {
        utils::showError("Missing required argument: --dataset");
        std::cout << "Usage: blahajpi train --dataset <path> [--output <dir>]" << std::endl;
        return 1;
    }
    
    // Get dataset path
    std::string datasetPath = parsedArgs["dataset"];
    
    // Verify dataset exists
    if (!std::filesystem::exists(datasetPath)) {
        utils::showError("Dataset file not found: " + datasetPath);
        return 1;
    }
    
    // Get output directory (default to current model-dir from config)
    std::string outputDir;
    if (parsedArgs.count("output") > 0) {
        outputDir = parsedArgs["output"];
    } else {
        auto config = analyzer.getConfig();
        auto it = config.find("model-dir");
        if (it != config.end()) {
            outputDir = it->second;
        } else {
            outputDir = "../models/custom_model";
        }
    }
    
    // Apply any overrides to the configuration
    if (parsedArgs.count("alpha") > 0) {
        analyzer.setConfig("alpha", parsedArgs["alpha"]);
    }
    
    if (parsedArgs.count("eta0") > 0) {
        analyzer.setConfig("eta0", parsedArgs["eta0"]);
    }
    
    if (parsedArgs.count("epochs") > 0) {
        analyzer.setConfig("epochs", parsedArgs["epochs"]);
    }
    
    if (parsedArgs.count("seed") > 0) {
        analyzer.setConfig("seed", parsedArgs["seed"]);
    }
    
    // Show training configuration
    auto config = analyzer.getConfig();
    std::cout << "Training Configuration:" << std::endl;
    std::cout << "  Dataset: " << datasetPath << std::endl;
    std::cout << "  Output: " << outputDir << std::endl;
    std::cout << "  Regularization (alpha): " << config["alpha"] << std::endl;
    std::cout << "  Learning rate (eta0): " << config["eta0"] << std::endl;
    std::cout << "  Epochs: " << config["epochs"] << std::endl;
    std::cout << "  Max features: " << config["max-features"] << std::endl;
    
    // Confirm with user
    std::cout << "\nReady to start training. This may take a while depending on the dataset size." << std::endl;
    std::cout << "Do you want to continue? [y/N]: ";
    std::string response;
    std::getline(std::cin, response);
    
    if (response != "y" && response != "Y") {
        std::cout << "Training cancelled." << std::endl;
        return 0;
    }
    
    // Start training timer
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "\nTraining model..." << std::endl;
    
    try {
        // Train the model
        bool success = analyzer.trainModel(datasetPath, outputDir);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        if (success) {
            utils::showSuccess("Model trained successfully");
            std::cout << "Training completed in " << duration.count() << " seconds" << std::endl;
            std::cout << "Model saved to: " << outputDir << std::endl;
            
            // Load the newly trained model
            std::cout << "Loading the trained model..." << std::endl;
            if (analyzer.loadModel(outputDir)) {
                utils::showSuccess("Trained model loaded successfully");
            } else {
                utils::showWarning("Failed to load the trained model");
            }
            
            return 0;
        } else {
            utils::showError("Training failed");
            return 1;
        }
    } catch (const std::exception& e) {
        utils::showError("Training error: " + std::string(e.what()));
        return 1;
    }
}

} // namespace bpicli
