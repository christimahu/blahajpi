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
option(ENABLE_TESTING "Enable testing" ON)
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_FUZZING "Enable fuzz testing" OFF)

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

# Find Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.13.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

# Find Doxygen for documentation
find_package(Doxygen)
if(NOT DOXYGEN_FOUND)
    message(STATUS "Doxygen not found, documentation will not be generated")
endif()

# Enable compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

# Configure sanitizers
if(ENABLE_ASAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Enabling Address Sanitizer")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    else()
        message(WARNING "Address Sanitizer not supported with this compiler")
    endif()
endif()

if(ENABLE_UBSAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Enabling Undefined Behavior Sanitizer")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
    else()
        message(WARNING "Undefined Behavior Sanitizer not supported with this compiler")
    endif()
endif()

# Configure code coverage
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Enabling code coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0 -g")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    else()
        message(WARNING "Code coverage not supported with this compiler")
    endif()
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
if(ENABLE_TESTING)
    add_subdirectory(tests)
    
    # Make the test target a dependency of the build
    add_custom_target(check
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS blahajpi_lib
        COMMENT "Running tests - build will fail if tests fail"
    )
endif()

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
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/results)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/models)

# Copy configuration files to build directory
file(GLOB CONFIG_FILES "${CONFIG_DIR}/*.conf")
foreach(CONFIG_FILE ${CONFIG_FILES})
    get_filename_component(FILENAME ${CONFIG_FILE} NAME)
    configure_file(${CONFIG_FILE} ${CMAKE_BINARY_DIR}/configs/${FILENAME} COPYONLY)
endforeach()

# Ensure configs directory exists in the build directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/configs)

# Configure Doxygen documentation
if(DOXYGEN_FOUND)
    # Create Doxyfile if it doesn't exist
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile")
        message(STATUS "Creating default Doxyfile")
        execute_process(
            COMMAND ${DOXYGEN_EXECUTABLE} -g
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        
        # Append project-specific settings
        file(APPEND "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile"
            "\n# Blahaj PI specific settings\n"
            "PROJECT_NAME = \"Blahaj PI\"\n"
            "PROJECT_BRIEF = \"Your friendly shark detective keeping social waters safe\"\n"
            "OUTPUT_DIRECTORY = docs/doxygen\n"
            "INPUT = lib/include lib/src cli/include cli/src\n"
            "RECURSIVE = YES\n"
            "EXTRACT_ALL = YES\n"
            "GENERATE_HTML = YES\n"
            "HTML_OUTPUT = html\n"
            "GENERATE_LATEX = NO\n"
        )
    endif()
    
    # Add custom target to generate documentation
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM
    )
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

# Fuzzing targets (if enabled)
if(ENABLE_FUZZING)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(STATUS "Configuring fuzz testing targets")
        
        # Set fuzzing flags
        set(FUZZING_FLAGS "-fsanitize=fuzzer,address")
        
        # Create fuzzer targets
        add_executable(text_processor_fuzzer tests/fuzz/text_processor_fuzzer.cpp)
        target_link_libraries(text_processor_fuzzer PRIVATE blahajpi_lib)
        target_compile_options(text_processor_fuzzer PRIVATE ${FUZZING_FLAGS})
        set_target_properties(text_processor_fuzzer PROPERTIES LINK_FLAGS ${FUZZING_FLAGS})
        
        add_executable(vectorizer_fuzzer tests/fuzz/vectorizer_fuzzer.cpp)
        target_link_libraries(vectorizer_fuzzer PRIVATE blahajpi_lib)
        target_compile_options(vectorizer_fuzzer PRIVATE ${FUZZING_FLAGS})
        set_target_properties(vectorizer_fuzzer PROPERTIES LINK_FLAGS ${FUZZING_FLAGS})
    else()
        message(WARNING "Fuzzing is only supported with Clang compiler")
    endif()
endif()

# Print configuration summary
message(STATUS "=== Blahaj PI Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Building shared libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenMP support: ${OpenMP_CXX_FOUND}")
message(STATUS "Testing enabled: ${ENABLE_TESTING}")
message(STATUS "Address Sanitizer: ${ENABLE_ASAN}")
message(STATUS "Undefined Behavior Sanitizer: ${ENABLE_UBSAN}")
message(STATUS "Code coverage: ${ENABLE_COVERAGE}")
message(STATUS "Fuzz testing: ${ENABLE_FUZZING}")
message(STATUS "Documentation: ${DOXYGEN_FOUND}")
message(STATUS "=============================")
