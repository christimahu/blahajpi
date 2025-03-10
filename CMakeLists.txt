# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(blahajpi VERSION 1.0 
        DESCRIPTION "Your friendly shark detective keeping social waters safe"
        LANGUAGES CXX)

# Set C++23 standard for modern features like concepts, ranges, and std::span
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Option to build as a shared library (default) or static library
option(BUILD_SHARED_LIBS "Build as shared library" ON)

# Enable verbose cmake output for debugging
set(CMAKE_VERBOSE_MAKEFILE ON)

# Define source directories for better organization
set(SRC_DIR ${CMAKE_SOURCE_DIR}/lib/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/include)
set(TEST_DIR ${CMAKE_SOURCE_DIR}/tests)
set(CLI_DIR ${CMAKE_SOURCE_DIR}/cli)
set(CONFIG_DIR ${CMAKE_SOURCE_DIR}/configs)
set(DATA_DIR ${CMAKE_SOURCE_DIR}/data)

# Set output directories for binaries and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find OpenMP if available (for parallel processing)
find_package(OpenMP QUIET)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    add_compile_definitions(HAVE_OPENMP)
    message(STATUS "OpenMP found and enabled")
endif()

# Enable compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

# Add include directories for header files
include_directories(${INCLUDE_DIR})

# Define the source files for the library explicitly
# Neural network classifier is added explicitly to ensure it's included
set(LIB_SOURCES
    ${SRC_DIR}/analyzer.cpp
    ${SRC_DIR}/config.cpp
    
    ${SRC_DIR}/models/sgd.cpp
    ${SRC_DIR}/models/neural_network.cpp
    
    ${SRC_DIR}/preprocessing/text_processor.cpp
    ${SRC_DIR}/preprocessing/vectorizer.cpp
    
    ${SRC_DIR}/utils/word_cloud.cpp
    ${SRC_DIR}/utils/dataset.cpp
    
    ${SRC_DIR}/evaluation/metrics.cpp
)

# Print the source files to verify neural_network_classifier.cpp is included
message(STATUS "Library source files: ${LIB_SOURCES}")

# Create the main library target
add_library(blahajpi_lib ${LIB_SOURCES})

# Set library properties including versioning and position-independent code
set_target_properties(blahajpi_lib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME BlahajPI
)

# Link with OpenMP if found
if(OpenMP_CXX_FOUND)
    target_link_libraries(blahajpi_lib PRIVATE OpenMP::OpenMP_CXX)
endif()

# Add the CLI target
add_subdirectory(cli)

# Install the library for system-wide use
install(TARGETS blahajpi_lib
    EXPORT BlahajPITargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install header files for developers to use the library
install(DIRECTORY lib/include/blahajpi
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Create export package for easy integration in other CMake projects
install(EXPORT BlahajPITargets
    FILE BlahajPITargets.cmake
    NAMESPACE BlahajPI::
    DESTINATION lib/cmake/BlahajPI
)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Make the test target a dependency of the build
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS blahajpi_lib
    COMMENT "Running tests - build will fail if tests fail"
)

# Generate and install CMake package config
include(CMakePackageConfigHelpers)

# Write version file for dependency management
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/BlahajPIConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Configure package config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/configs/cmake/BlahajPIConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/BlahajPIConfig.cmake"
    INSTALL_DESTINATION lib/cmake/BlahajPI
)

# Install config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/BlahajPIConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/BlahajPIConfigVersion.cmake"
    DESTINATION lib/cmake/BlahajPI
)

# Create output directories in the build folder
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/outputs)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/models)

# Copy configuration files to build directory
file(GLOB CONFIG_FILES "${CONFIG_DIR}/*.conf")
foreach(CONFIG_FILE ${CONFIG_FILES})
    get_filename_component(FILENAME ${CONFIG_FILE} NAME)
    configure_file(${CONFIG_FILE} ${CMAKE_BINARY_DIR}/configs/${FILENAME} COPYONLY)
endforeach()

# Ensure configs directory exists in the build directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/configs)

# Custom target for documentation
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E env DOXYGEN=${DOXYGEN_EXECUTABLE} 
                ${CMAKE_SOURCE_DIR}/dev.py --docs
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM)
else()
    message(STATUS "Doxygen not found, documentation generation disabled")
endif()

# Add custom target to run all examples
add_custom_target(run_examples
    COMMAND ${CMAKE_BINARY_DIR}/bin/blahajpi analyze --file ${DATA_DIR}/examples/twitter_example.csv --config default
    DEPENDS blahajpi_cli
    COMMENT "Running sentiment analysis examples"
    VERBATIM
)

# Print configuration summary
message(STATUS "=== Blahaj PI Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Building shared libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenMP support: ${OpenMP_CXX_FOUND}")
message(STATUS "Documentation: ${DOXYGEN_FOUND}")
message(STATUS "=============================")
