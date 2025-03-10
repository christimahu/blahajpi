# lib/CMakeLists.txt
cmake_minimum_required(VERSION 3.14)

# Define source files
set(BLAHAJPI_LIB_SOURCES
    # Core
    src/analyzer.cpp
    src/config.cpp
    
    # Models
    src/models/sgd.cpp
    src/models/neural_network.cpp
    
    # Preprocessing
    src/preprocessing/text_processor.cpp
    src/preprocessing/vectorizer.cpp
    
    # Utils
    src/utils/word_cloud.cpp
    src/utils/dataset.cpp
    
    # Evaluation
    src/evaluation/metrics.cpp
)

# Define the library target
add_library(blahajpi_lib ${BLAHAJPI_LIB_SOURCES})

# Set include directories
target_include_directories(blahajpi_lib
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link with OpenMP if found
if(OpenMP_CXX_FOUND)
    target_link_libraries(blahajpi_lib PRIVATE OpenMP::OpenMP_CXX)
endif()

# Set library properties
set_target_properties(blahajpi_lib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME BlahajPI
    OUTPUT_NAME blahajpi
)

# Define install rules
install(TARGETS blahajpi_lib
    EXPORT BlahajPITargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install header files
install(DIRECTORY include/blahajpi
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Add alias target for use within the build tree
add_library(BlahajPI::blahajpi_lib ALIAS blahajpi_lib)

# Generate the export targets
export(TARGETS blahajpi_lib
    NAMESPACE BlahajPI::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/BlahajPITargets.cmake"
)
