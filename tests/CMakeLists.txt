# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.14)

# Enable testing functionality
enable_testing()

# Find Google Test if not already added
if (NOT TARGET gtest)
    find_package(GTest QUIET)
    if (NOT GTest_FOUND)
        message(STATUS "Using FetchContent to get GoogleTest")
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.13.0
        )
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()
endif()

set(UNIT_TESTS
    analyzer_test
    text_processor_test
    vectorizer_test
    sgd_classifier_test
    neural_network_test
    metrics_test
    config_test
	dataset_test 
	word_cloud_test
)

set(INTEGRATION_TESTS
    api_integration_test
    cli_integration_test
)

# Configure tests library
add_library(blahajpi_tests INTERFACE)
target_link_libraries(blahajpi_tests INTERFACE
    blahajpi_lib
    gtest
    gtest_main
)

# Unit tests
foreach(TEST_NAME ${UNIT_TESTS})
    # Create unit test executable
    add_executable(${TEST_NAME} unit/${TEST_NAME}.cpp)
    
    # Link against the tests library
    target_link_libraries(${TEST_NAME} PRIVATE blahajpi_tests)
    
    # Add the test with label
    add_test(
        NAME ${TEST_NAME}
        COMMAND ${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    
    # Set test labels
    set_tests_properties(${TEST_NAME} PROPERTIES LABELS "Unit")
endforeach()

# Integration tests
foreach(TEST_NAME ${INTEGRATION_TESTS})
    # Create integration test executable
    add_executable(${TEST_NAME} integration/${TEST_NAME}.cpp)
    
    # Link against the tests library
    target_link_libraries(${TEST_NAME} PRIVATE blahajpi_tests)
    
    # Add the test with label
    add_test(
        NAME ${TEST_NAME}
        COMMAND ${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    
    # Set test labels
    set_tests_properties(${TEST_NAME} PROPERTIES LABELS "Integration")
endforeach()

# End-to-end tests (Python-based)
find_package(Python3 COMPONENTS Interpreter)
if(Python3_FOUND)
    # Add Python-based end-to-end tests
    add_test(
        NAME cli_e2e_test
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/e2e/cli_runner.py
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    
    add_test(
        NAME api_e2e_test
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/e2e/test_api.py
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    
    # Set test labels for E2E tests
    set_tests_properties(cli_e2e_test api_e2e_test PROPERTIES LABELS "E2E")
    
    # Create e2e directory if it doesn't exist
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/e2e)
    
    # Only copy files if the directory exists
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/e2e)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/e2e/ 
             DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/e2e)
    endif()
endif()

# Copy test data to build directory
add_custom_target(copy_test_data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data/examples
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_data
    COMMENT "Copying test data files"
)

# Create a dependency on test data for all test executables
foreach(TEST_NAME ${UNIT_TESTS} ${INTEGRATION_TESTS})
    add_dependencies(${TEST_NAME} copy_test_data)
endforeach()

# Add a custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${UNIT_TESTS} ${INTEGRATION_TESTS}
    COMMENT "Running all tests"
)

# Add a custom target to run only unit tests
add_custom_target(run_unit_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -L Unit --output-on-failure
    DEPENDS ${UNIT_TESTS}
    COMMENT "Running unit tests"
)

# Add a custom target to run only integration tests
add_custom_target(run_integration_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -L Integration --output-on-failure
    DEPENDS ${INTEGRATION_TESTS}
    COMMENT "Running integration tests"
)

# Add a custom target to run only end-to-end tests
if(Python3_FOUND)
    add_custom_target(run_e2e_tests
        COMMAND ${CMAKE_CTEST_COMMAND} -L E2E --output-on-failure
        COMMENT "Running end-to-end tests"
    )
endif()

# Set timeout for longer-running tests
set_tests_properties(${INTEGRATION_TESTS} PROPERTIES TIMEOUT 60)
if(Python3_FOUND)
    set_tests_properties(cli_e2e_test api_e2e_test PROPERTIES TIMEOUT 120)
endif()
